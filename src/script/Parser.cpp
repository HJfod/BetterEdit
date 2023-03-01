#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/string.hpp>

using namespace script;

Result<Rc<LitExpr>> LitExpr::pull(InputStream& stream) {
    // arrays
    Rollback rb(stream);
    if (Token::pull('[', stream)) {
        ArrLit arr;
        while (true) {
            GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
            arr.push_back(expr);
            if (!Token::pull(',', stream)) {
                break;
            }
            // allow trailing comma
            if (Token::peek(']', stream)) {
                break;
            }
        }
        GEODE_UNWRAP(Token::pull(']', stream));
        return make<LitExpr>({
            .value = arr,
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
    return std::visit(makeVisitor {
        [&](NullLit const&) -> Result<Rc<Value>> {
            return Ok(Value::rc(NullLit()));
        },
        [&](BoolLit const& b) -> Result<Rc<Value>> {
            return Ok(Value::rc(b));
        },
        [&](NumLit const& num) -> Result<Rc<Value>> {
            return Ok(Value::rc(num));
        },
        [&](StrLit const& str) -> Result<Rc<Value>> {
            return Ok(Value::rc(str));
        },
        [&](ArrLit const& arr) -> Result<Rc<Value>> {
            Array value;
            value.reserve(arr.size());
            for (auto& expr : arr) {
                GEODE_UNWRAP_INTO(auto val, expr->eval(state));
                value.push_back(*val);
            }
            return Ok(Value::rc(value));
        },
    }, value);
}

std::string LitExpr::debug() const {
    return fmt::format("LitExpr({})", tokenToString(value, true));
}

Result<Rc<IdentExpr>> IdentExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    return make<IdentExpr>({
        .ident = ident,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> IdentExpr::eval(State& state) {
    if (!state.has(ident)) {
        state.add(ident, Value::rc(NullLit()));
    }
    return Ok(state.get(ident));
}

std::string IdentExpr::debug() const {
    return fmt::format("IdentExpr({})", tokenToString(ident, true));
}

FunExpr::FunExpr(
    Ident const& ident,
    decltype(params) const& params,
    Rc<Expr> const& body,
    std::string const& src
) : ident(ident), params(params), body(body), src(src) {}

Result<Rc<FunExpr>> FunExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Function, stream));
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    GEODE_UNWRAP(Token::pull('(', stream));
    std::vector<std::pair<Ident, std::optional<Rc<Expr>>>> params;
    while (true) {
        GEODE_UNWRAP_INTO(auto name, Token::pull<Ident>(stream));
        if (Token::pull(Op::Seq, stream)) {
            GEODE_UNWRAP_INTO(auto value, Expr::pull(stream));
            params.push_back({ name, value });
        }
        else {
            params.push_back({ name, std::nullopt });
        }
        if (!Token::pull(',', stream) || Token::peek(')', stream)) {
            break;
        }
    }
    GEODE_UNWRAP(Token::pull(')', stream));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pullBlock(stream));
    return make<FunExpr>(ident, params, body, rb.commit());
}

Result<Rc<Value>> FunExpr::eval(State& state) {
    GEODE_UNWRAP(this->add(state));
    return Ok(state.get(ident));
}

Result<> FunExpr::add(State& state) {
    if (!added) {
        if (state.has(ident)) {
            return Err(fmt::format("Function '{}' is already defined", ident));
        }
        state.add(ident, Value::rc(shared_from_this()));
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
    return fmt::format("FunExpr({}, params({}), {})", ident, par, body->debug());
}

Result<Rc<ListExpr>> ListExpr::pull(InputStream& stream) {
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
        GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
        list.push_back(expr);
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

Result<Rc<Expr>> ListExpr::pullBlock(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull('{', stream));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pull(stream));
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

Result<Rc<Expr>> Expr::pull(InputStream& stream) {
    return BinOpExpr::pull(stream);
}

#define TRY_PULL(expr) \
    if (auto value = expr::pull(stream)) {\
        rb.commit(); \
        return make<Expr>({ .value = value.unwrap(), }); \
    }

#define PULL_IF(cond, expr) \
    if (Token::peek(cond, stream)) {                        \
        GEODE_UNWRAP_INTO(auto expr, expr::pull(stream));   \
        rb.commit();                                        \
        return make<Expr>({ .value = expr });               \
    }

Result<Rc<Expr>> Expr::pullPrimary(InputStream& stream) {
    GEODE_UNWRAP_INTO(auto expr, Expr::pullPrimaryNonCall(stream));
    // get all indexes and calls (if chained)
    while (true) {
        if (Token::peek('(', stream)) {
            GEODE_UNWRAP_INTO(auto call, CallExpr::pull(expr, stream));
            expr = make<Expr>({ .value = call }).unwrap();
            continue;
        }
        if (Token::peek('[', stream)) {
            GEODE_UNWRAP_INTO(auto ix, IndexExpr::pull(expr, stream));
            expr = make<Expr>({ .value = ix }).unwrap();
            continue;
        }
        if (Token::peek('.', stream)) {
            GEODE_UNWRAP_INTO(auto mem, MemberExpr::pull(expr, stream));
            expr = make<Expr>({ .value = mem }).unwrap();
            continue;
        }
        break;
    }
    return Ok(expr);
}

Result<Rc<Expr>> Expr::pullPrimaryNonCall(InputStream& stream) {
    Rollback rb(stream);
    // parenthesis
    if (Token::pull('(', stream)) {
        GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
        GEODE_UNWRAP(Token::pull(')', stream));
        rb.commit();
        return Ok(expr);
    }
    TRY_PULL(UnOpExpr);
    // if we pull a specific keyword, then it has to be that statement
    PULL_IF(Keyword::If, IfExpr);
    PULL_IF(Keyword::For, ForInExpr);
    PULL_IF(Keyword::Function, FunExpr);
    PULL_IF(Keyword::Return, ReturnExpr);
    PULL_IF(Keyword::Break, BreakExpr);
    PULL_IF(Keyword::Continue, ContinueExpr);
    if (Token::pull('{', stream)) {
        GEODE_UNWRAP_INTO(auto expr, ListExpr::pull(stream));
        GEODE_UNWRAP(Token::pull('}', stream));
        rb.commit();
        return make<Expr>({ .value = expr });
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

std::string Expr::src() const {
    return std::visit(makeVisitor {
        [&](auto const& expr) {
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
