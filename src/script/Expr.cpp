#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/string.hpp>

using namespace script;

Result<Rc<LitExpr>> LitExpr::pull(InputStream& stream, Attrs& attrs) {
    // arrays
    Rollback rb(stream);
    if (Token::pull('[', stream)) {
        ArrLit arr;
        while (true) {
            tickExecutionCounter();
            GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream, attrs));
            arr.push_back(expr);
            // allow trailing comma
            if (!Token::pull(',', stream) || Token::peek(']', stream)) {
                break;
            }
        }
        GEODE_UNWRAP(Token::pull(']', stream));
        return make<LitExpr>({
            .value = arr,
            .src = rb.commit(),
        });
    }
    if (Token::pull('{', stream)) {
        if (Token::pull('}', stream)) {
            return make<LitExpr>({
                .value = ObjLit(),
                .src = rb.commit(),
            });
        }
        ObjLit obj;
        while (true) {
            tickExecutionCounter();
            GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
            // if this is just an ident, treat it as `ident = ident`
            if (Token::peek(',', stream)) {
                obj.insert({
                    ident,
                    make<Expr>({
                        .value = make<IdentExpr>({
                            .ident = ident,
                            .src = ident,
                        }).unwrap()
                    }).unwrap()
                });
            }
            // otherwise expect `ident: value`
            else {
                GEODE_UNWRAP(Token::pull(':', stream));
                GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream, attrs));
                obj.insert({ ident, expr });
            }
            // allow trailing comma
            if (!Token::pull(',', stream) || Token::peek('}', stream)) {
                break;
            }
        }
        GEODE_UNWRAP(Token::pull('}', stream));
        return make<LitExpr>({
            .value = obj,
            .src = rb.commit(),
        });
    }
    GEODE_UNWRAP_INTO(auto value, Token::pull<Lit>(stream));
    return make<LitExpr>({
        .value = value,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> LitExpr::eval(State& state) {
    return Value::lit(value, state);
}

std::string LitExpr::debug() const {
    return fmt::format("LitExpr({})", tokenToString(value, true));
}

Result<Rc<IdentExpr>> IdentExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    return make<IdentExpr>({
        .ident = ident,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> IdentExpr::eval(State& state) {
    if (!state.has(ident)) {
        if (isSpecialIdent(ident)) {
            return Err("Identifier '{}' is not valid in this context", ident);
        }
        state.add(ident, Value::rc(NullLit()));
    }
    return Ok(state.get(ident));
}

std::string IdentExpr::debug() const {
    return fmt::format("IdentExpr({})", tokenToString(ident, true));
}

Result<Rc<ConstExpr>> ConstExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Const, stream));
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    GEODE_UNWRAP(Token::pull(Op::Seq, stream));
    GEODE_UNWRAP_INTO(auto value, Expr::pull(stream, attrs));
    return make<ConstExpr>({
        .ident = ident,
        .value = value,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> ConstExpr::eval(State& state) {
    if (state.has(ident, true)) {
        return Err("Variable {} already exists in this scope", ident);
    }
    if (isSpecialIdent(ident)) {
        return Err("Identifier '{}' may not be declared manually", ident);
    }
    GEODE_UNWRAP_INTO(auto val, value->eval(state));
    auto newVal = Value::rc(NullLit(), nullptr, true);
    newVal->value = val->value;
    state.add(ident, newVal);
    return Ok(state.get(ident));
}

std::string ConstExpr::debug() const {
    return fmt::format("ConstExpr({}, {})", tokenToString(ident, true), value->debug());
}

FunExpr::FunExpr(
    std::optional<Ident> const& ident,
    decltype(params) const& params,
    bool variadic,
    Rc<Expr> const& body,
    std::string const& src
) : ident(ident), params(params), variadic(variadic), body(body), src(src) {}

Result<std::pair<decltype(FunExpr::params), bool>> FunExpr::pullParams(InputStream& stream, Attrs& attrs) {
    GEODE_UNWRAP(Token::pull('(', stream));
    decltype(FunExpr::params) params;
    bool variadic = false;
    while (true) {
        tickExecutionCounter();
        if (Token::pull("...", stream)) {
            if (variadic) {
                return Err("Function already declared as variadic");
            }
            variadic = true;
        }
        else {
            GEODE_UNWRAP_INTO(auto name, Token::pull<Ident>(stream));
            if (Token::pull(Op::Seq, stream)) {
                GEODE_UNWRAP_INTO(auto value, Expr::pull(stream, attrs));
                params.push_back({ name, value });
            }
            else {
                params.push_back({ name, std::nullopt });
            }
        }
        if (!Token::pull(',', stream) || Token::peek(')', stream)) {
            break;
        }
    }
    GEODE_UNWRAP(Token::pull(')', stream));
    return Ok(std::pair { params, variadic });
}

Result<Rc<FunExpr>> FunExpr::pullArrow(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    std::pair<decltype(params), bool> params;
    if (Token::peek('(', stream)) {
        GEODE_UNWRAP_INTO(params, FunExpr::pullParams(stream, attrs));
    }
    else {
        GEODE_UNWRAP_INTO(auto x, Token::pull<Ident>(stream));
        params.first.push_back({ x, std::nullopt });
    }
    GEODE_UNWRAP(Token::pull(Op::Arrow, stream));
    Rc<Expr> body;
    if (Token::peek('{', stream)) {
        GEODE_UNWRAP_INTO(body, ListExpr::pullBlock(stream, attrs));
    }
    else {
        GEODE_UNWRAP_INTO(body, Expr::pull(stream, attrs));
    }
    return make<FunExpr>(std::nullopt, params.first, params.second, body, rb.commit());
}

Result<Rc<FunExpr>> FunExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Function, stream));
    std::optional<Ident> ident;
    if (Token::peek<Ident>(stream)) {
        GEODE_UNWRAP_INTO(ident, Token::pull<Ident>(stream));
    }
    GEODE_UNWRAP_INTO(auto params, FunExpr::pullParams(stream, attrs));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pullBlock(stream, attrs));
    return make<FunExpr>(ident, params.first, params.second, body, rb.commit());
}

Result<Rc<Value>> FunExpr::eval(State& state) {
    if (ident) {
        GEODE_UNWRAP(this->add(state));
        return Ok(state.get(ident.value()));
    }
    else {
        return Ok(Value::rc(shared_from_this()));
    }
}

Result<> FunExpr::add(State& state) {
    if (!added && ident) {
        if (state.has(ident.value())) {
            return Err(fmt::format("Function '{}' is already defined", ident.value()));
        }
        state.add(ident.value(), Value::rc(shared_from_this()));
        added = true;
    }
    return Ok();
}

std::string FunExpr::debug() const {
    std::string par;
    bool first = true;
    for (auto& [na, va] : params) {
        if (!first) {
            par += ", ";
        }
        first = false;
        if (va) {
            par += fmt::format("Param({}, {})", na, va.value()->debug());
        }
        else {
            par += fmt::format("Param({})", na);
        }
    }
    return fmt::format("FunExpr({}, params({}), {})", ident.value_or("anon"), par, body->debug());
}

Result<Rc<ListExpr>> ListExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    // handle just {}
    if (Token::peek('}', stream)) {
        return make<ListExpr>({
            .exprs = {},
            .src = rb.commit(),
        });
    }
    std::vector<Rc<Expr>> list;
    while (true) {
        tickExecutionCounter();
        GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream, attrs));
        list.push_back(expr);
        // allow last statement in a function to be semicolonless
        if (attrs.strict && !Token::peek(';', stream) && !Token::peek('}', stream)) {
            return Err("Statements must be separated by semicolons in strict mode");
        }
        // allow any number of semicolons between expressions
        while (Token::pull(';', stream)) {}
        if (Token::peek('}', stream) || Token::eof(stream)) {
            break;
        }
    }
    rb.commit();
    return make<ListExpr>({
        .exprs = list,
        .src = rb.commit(),
    });
}

Result<Rc<Expr>> ListExpr::pullBlock(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull('{', stream));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pull(stream, attrs));
    GEODE_UNWRAP(Token::pull('}', stream));
    rb.commit();
    return make<Expr>({
        .value = body,
    });
}

Result<Rc<Value>> ListExpr::eval(State& state) {
    Rc<Value> ret = Value::rc(NullLit());
    for (auto& expr : exprs) {
        try {
            GEODE_UNWRAP_INTO(ret, expr->eval(state)
                .expect("{error}\n * While evaluating {}", expr->src())
            );
        }
        catch (BreakSignal& sig) {
            return Ok(sig.value);
        }
    }
    return Ok(ret);
}

std::string ListExpr::debug() const {
    std::string items;
    bool first = true;
    for (auto& expr : exprs) {
        if (!first) {
            items += ", ";
        }
        first = false;
        items += expr->debug();
    }
    return fmt::format("ListExpr({})", items);
}

std::string CppExpr::debug() const {
    return "CppExpr()";
}

Result<Rc<Expr>> Expr::pull(InputStream& stream, Attrs& attrs) {
    return BinOpExpr::pull(stream, attrs);
}

#define TRY_PULL(expr) \
    if (auto value = expr::pull(stream, attrs)) {\
        rb.commit(); \
        return make<Expr>({ .value = value.unwrap(), }); \
    }

#define PULL_IF(cond, expr) \
    if (Token::peek(cond, stream)) {                            \
        GEODE_UNWRAP_INTO(auto expr, expr::pull(stream, attrs));\
        rb.commit();                                            \
        return make<Expr>({ .value = expr });                   \
    }

Result<Rc<Expr>> Expr::pullPrimary(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto expr, Expr::pullPrimaryNonCall(stream, attrs));
    // get all indexes and calls (if chained)
    while (true) {
        tickExecutionCounter();
        if (Token::peek('(', stream)) {
            GEODE_UNWRAP_INTO(auto call, CallExpr::pull(expr, stream, attrs));
            expr = make<Expr>({ .value = call }).unwrap();
            continue;
        }
        if (Token::peek('[', stream)) {
            GEODE_UNWRAP_INTO(auto ix, IndexExpr::pull(expr, stream, attrs));
            expr = make<Expr>({ .value = ix }).unwrap();
            continue;
        }
        if (Token::peek("...", stream)) {
            GEODE_UNWRAP_INTO(auto mem, ForEachExpr::pull(expr, stream, attrs));
            expr = make<Expr>({ .value = mem }).unwrap();
            continue;
        }
        if (Token::peek('.', stream)) {
            GEODE_UNWRAP_INTO(auto mem, MemberExpr::pull(expr, stream, attrs));
            expr = make<Expr>({ .value = mem }).unwrap();
            continue;
        }
        break;
    }
    rb.commit();
    return Ok(expr);
}

Result<Rc<Expr>> Expr::pullPrimaryNonCall(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    // try pulling an arrow function first as it has very ambiguous syntax
    if (auto arrow = FunExpr::pullArrow(stream, attrs)) {
        rb.commit();
        return make<Expr>({ .value = arrow.unwrap(), });
    }
    if (Token::pull('(', stream)) {
        GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream, attrs));
        GEODE_UNWRAP(Token::pull(')', stream));
        rb.commit();
        return Ok(expr);
    }
    TRY_PULL(UnOpExpr);
    // if we pull a specific keyword, then it has to be that statement
    PULL_IF(Keyword::If, IfExpr);
    PULL_IF(Keyword::For, ForInExpr);
    PULL_IF(Keyword::Try, TryExpr);
    PULL_IF(Keyword::Function, FunExpr);
    PULL_IF(Keyword::Return, ReturnExpr);
    PULL_IF(Keyword::Break, BreakExpr);
    PULL_IF(Keyword::Continue, ContinueExpr);
    PULL_IF(Keyword::Const, ConstExpr);
    if (Token::peek('{', stream)) {
        // try object literal first
        TRY_PULL(LitExpr);
        // then list expr
        GEODE_UNWRAP(Token::pull('}', stream));
        GEODE_UNWRAP_INTO(auto expr, ListExpr::pull(stream, attrs));
        GEODE_UNWRAP(Token::pull('}', stream));
        rb.commit();
        return make<Expr>({ .value = expr });
    }
    if (Token::pull('@', stream)) {
        return Err("Attributes are only allowed at the start of a script");
    }
    TRY_PULL(LitExpr);
    TRY_PULL(IdentExpr);
    auto token = Token::pull(stream);
    if (token) {
        return Err("Expected expression, got '{}'", token.unwrap().toString());
    }
    else {
        return Err(token.unwrapErr());
    }
}

Result<Rc<Value>> Expr::eval(State& state) {
    return std::visit(makeVisitor {
        [&](auto const& expr) {
            return expr->eval(state);
        },
    }, value);
}

std::string Expr::debug() const {
    return std::visit(makeVisitor {
        [&](auto const& expr) {
            return expr->debug();
        },
    }, value);
}

std::string Expr::src(bool raw) const {
    return std::visit(makeVisitor {
        [&](auto const& expr) {
            if (raw) {
                return expr->src;
            }
            auto code = string::trim(expr->src);
            if (string::contains(code, '\n')) {
                return fmt::format("```\n{}\n```", code);
            }
            else {
                return fmt::format("`{}`", code);
            }
        },
    }, value);
}
