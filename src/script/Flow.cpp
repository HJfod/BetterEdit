#include "Parser.hpp"

using namespace script;

Result<Rc<ReturnExpr>> ReturnExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Return, stream));
    if (auto expr = Expr::pull(stream, attrs)) {
        return make<ReturnExpr>({
            .value = expr.unwrap(),
            .src = rb.commit(),
        });
    }
    else {
        return make<ReturnExpr>({
            .value = std::nullopt,
            .src = rb.commit(),
        });
    }
}

Result<Rc<Value>> ReturnExpr::eval(State& state) {
    if (value) {
        GEODE_UNWRAP_INTO(auto val, value.value()->eval(state));
        throw ReturnSignal(val);
    }
    else {
        throw ReturnSignal(Value::rc(NullLit()));
    }
}

std::string ReturnExpr::debug() const {
    if (value) {
        return fmt::format("ReturnExpr({})", value.value()->debug());
    }
    else {
        return fmt::format("ReturnExpr(Null)");
    }
}

Result<Rc<BreakExpr>> BreakExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Break, stream));
    if (auto expr = Expr::pull(stream, attrs)) {
        return make<BreakExpr>({
            .value = expr.unwrap(),
            .src = rb.commit(),
        });
    }
    else {
        return make<BreakExpr>({
            .value = std::nullopt,
            .src = rb.commit(),
        });
    }
}

Result<Rc<Value>> BreakExpr::eval(State& state) {
    if (value) {
        GEODE_UNWRAP_INTO(auto val, value.value()->eval(state));
        throw BreakSignal(val);
    }
    else {
        throw BreakSignal(Value::rc(NullLit()));
    }
}

std::string BreakExpr::debug() const {
    if (value) {
        return fmt::format("BreakExpr({})", value.value()->debug());
    }
    else {
        return fmt::format("BreakExpr(Null)");
    }
}

Result<Rc<ContinueExpr>> ContinueExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Continue, stream));
    return make<ContinueExpr>({
        .src = rb.commit()
    });
}

Result<Rc<Value>> ContinueExpr::eval(State& state) {
    throw ContinueSignal();
}

std::string ContinueExpr::debug() const {
    return "Continue()";
}

Result<Rc<ForInExpr>> ForInExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::For, stream));
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    GEODE_UNWRAP(Token::pull(Keyword::In, stream));
    GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream, attrs));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pullBlock(stream, attrs));
    return make<ForInExpr>({
        .item = ident,
        .expr = expr,
        .body = body,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> ForInExpr::eval(State& state) {
    GEODE_UNWRAP_INTO(auto value, expr->eval(state));
    if (auto arr = value->has<Array>()) {
        // result of last iteration will be the return value
        auto ret = Value::rc(NullLit());
        for (auto value : *arr) {
            auto scope = state.scope();
            state.add(item, std::make_shared<Value>(value));
            try {
                GEODE_UNWRAP_INTO(ret, body->eval(state));
            }
            catch (ContinueSignal const&) {
                continue;
            }
            catch (BreakSignal const&) {
                break;
            }
        }
        return Ok(ret);
    }
    else if (auto num = value->has<NumLit>()) {
        // result of last iteration will be the return value
        auto ret = Value::rc(NullLit());
        for (size_t i = 0; i < static_cast<size_t>(*num); i++) {
            auto scope = state.scope();
            state.add(item, std::make_shared<Value>(NumLit(i)));
            try {
                GEODE_UNWRAP_INTO(ret, body->eval(state));
            }
            catch (ContinueSignal const&) {
                continue;
            }
            catch (BreakSignal const&) {
                break;
            }
        }
        return Ok(ret);
    }
    else {
        return Err("Attempted to iterate {}, expected array", value->typeName());
    }
}

std::string ForInExpr::debug() const {
    return fmt::format(
        "ForInExpr({}, {}, {})",
        item, expr->debug(), body->debug()
    );
}

Result<Rc<IfExpr>> IfExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::If, stream));
    GEODE_UNWRAP_INTO(auto cond, Expr::pull(stream, attrs));
    GEODE_UNWRAP_INTO(auto truthy, ListExpr::pullBlock(stream, attrs));
    std::optional<Rc<Expr>> falsy;
    if (Token::pull(Keyword::Else, stream)) {
        // no block required
        GEODE_UNWRAP_INTO(falsy, Expr::pull(stream, attrs));
    }
    return make<IfExpr>({
        .cond = cond,
        .truthy = truthy,
        .falsy = falsy,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> IfExpr::eval(State& state) {
    GEODE_UNWRAP_INTO(auto condVal, cond->eval(state)
        .expect("{error}\n * In condition `{}`", cond->src())
    );
    if (condVal->truthy()) {
        GEODE_UNWRAP_INTO(auto ret, truthy->eval(state));
        return Ok(ret);
    }
    else if (falsy) {
        GEODE_UNWRAP_INTO(auto ret, falsy.value()->eval(state));
        return Ok(ret);
    }
    return Ok(Value::rc(NullLit()));
}

std::string IfExpr::debug() const {
    if (falsy) {
        return fmt::format(
            "IfExpr({}, {}, {})",
            cond->debug(), truthy->debug(), falsy.value()->debug()
        );
    }
    else {
        return fmt::format(
            "IfExpr({}, {})",
            cond->debug(), truthy->debug()
        );
    }
}

Result<Rc<TryExpr>> TryExpr::pull(InputStream& stream, Attrs& attrs) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Try, stream));
    GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream, attrs));
    std::optional<Rc<Expr>> faily;
    if (Token::pull(Keyword::Else, stream)) {
        GEODE_UNWRAP_INTO(faily, Expr::pull(stream, attrs));
    }
    return make<TryExpr>({
        .expr = expr,
        .faily = faily,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> TryExpr::eval(State& state) {
    auto res = expr->eval(state);
    if (res) {
        return Ok(res.unwrap());
    } else {
        if (faily) {
            GEODE_UNWRAP_INTO(auto el, faily.value()->eval(state));
            return Ok(el);
        }
        else {
            return Ok(Value::rc(NullLit()));
        }
    }
}

std::string TryExpr::debug() const {
    if (faily) {
        return fmt::format(
            "TryExpr({}, {})",
            expr->debug(), faily.value()->debug()
        );
    }
    else {
        return fmt::format("TryExpr({})", expr->debug());
    }
}
