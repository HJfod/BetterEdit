#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/utils/map.hpp>

using namespace script;

template <class T>
static impl::Failure<std::string> internal(std::string const& msg) {
    return Err(fmt::format(
        "Internal error in {}: {}. Please report this bug to HJfod!",
        T::EXPR_NAME, msg
    ));
}

template <class T>
static impl::Failure<std::string> unimplemented(Op op) {
    return internal<T>(fmt::format("Unimplemented {}", tokenToString(op, true)));
};

template <class T>
Result<Rc<T>> make(T value) {
    return Ok(std::make_shared<T>(value));
}

static std::unordered_map<Keyword, std::string> KEYWORDS {
    { Keyword::For,         "for" },
    { Keyword::While,       "while" },
    { Keyword::In,          "in" },
    { Keyword::If,          "if" },
    { Keyword::Else,        "else" },
    { Keyword::Function,    "function" },
    { Keyword::New,         "new" },
    { Keyword::True,        "true" },
    { Keyword::False,       "false" },
    { Keyword::Null,        "null" },
};

static std::unordered_map<Op, std::tuple<std::string, size_t, OpDir>> OPS {
    { Op::Not,      { "!",  7,  OpDir::RTL } },
    { Op::Mul,      { "*",  6,  OpDir::LTR } },
    { Op::Div,      { "/",  6,  OpDir::LTR } },
    { Op::Add,      { "+",  5,  OpDir::LTR } },
    { Op::Sub,      { "-",  5,  OpDir::LTR } },
    { Op::Eq,       { "==", 4,  OpDir::LTR } },
    { Op::NotEq,    { "!=", 4,  OpDir::LTR } },
    { Op::Less,     { "<",  4,  OpDir::LTR } },
    { Op::LessEq,   { "<=", 4,  OpDir::LTR } },
    { Op::More,     { ">",  4,  OpDir::LTR } },
    { Op::MoreEq,   { ">=", 4,  OpDir::LTR } },
    { Op::And,      { "&&", 3,  OpDir::LTR } },
    { Op::Or,       { "||", 2,  OpDir::LTR } },
    { Op::Assign,   { "=",  1,  OpDir::RTL } },
};
static constexpr size_t LONGEST_OP = 2;

static std::string INVALID_IDENT_CHARS = ".,;(){}[]`\\´¨'\"";
static std::string VALID_OP_CHARS = "=+-/*<>!#?&|%@:~^";

bool script::isIdent(std::string const& ident) {
    // can't be empty
    if (!ident.size()) {
        return false;
    }
    // can't start with a digit
    if (isdigit(ident.front())) {
        return false;
    }
    for (auto& c : ident) {
        // no reserved chars
        if (INVALID_IDENT_CHARS.find_first_of(c) != std::string::npos) {
            return false;
        }
        // no operators
        if (VALID_OP_CHARS.find_first_of(c) != std::string::npos) {
            return false;
        }
        // no spaces
        if (std::isspace(c)) {
            return false;
        }
    }
    // no keywords
    for (auto const& [kw, va] : KEYWORDS) {
        if (ident == va) {
            return false;
        }
    }
    // otherwise fine ig
    return true;
}

bool script::isOp(std::string const& op) {
    for (auto& c : op) {
        if (VALID_OP_CHARS.find_first_of(c) == std::string::npos) {
            return false;
        }
    }
    return op.size();
}

bool script::isUnOp(Op op) {
    switch (op) {
        case Op::Add:
        case Op::Sub:
        case Op::Not:
            return true;
        default: return false;
    }
}

std::string script::tokenToString(Keyword kw, bool debug) {
    if (debug) {
        return fmt::format("keyword({})", KEYWORDS.at(kw));
    }
    return KEYWORDS.at(kw);
}

std::string script::tokenToString(Ident ident, bool debug) {
    if (debug) {
        return fmt::format("identifier({:?})", ident);
    }
    return ident;
}

std::string script::tokenToString(Lit lit, bool debug) {
    return std::visit(makeVisitor {
        [&](NullLit const& null) -> std::string {
            return "null";
        },
        [&](BoolLit const& b) -> std::string {
            if (debug) {
                return fmt::format("bool({})", b ? "true" : "false");
            }
            return b ? "true" : "false";
        },
        [&](StrLit const& str) -> std::string {
            if (debug) {
                return fmt::format("string({:?})", str);
            }
            return str;
        },
        [&](IntLit const& num) -> std::string {
            if (debug) {
                return fmt::format("int({})", num);
            }
            return std::to_string(num);
        },
        [&](FloatLit const& num) -> std::string {
            if (debug) {
                return fmt::format("float({})", num);
            }
            return std::to_string(num);
        },
        [&](ArrLit const& arr) -> std::string {
            std::string res = "[";
            bool first = true;
            for (auto& expr : arr) {
                if (!first) {
                    res += ", ";
                }
                first = false;
                if (auto lit = std::get_if<Rc<LitExpr>>(&expr->value)) {
                    res += tokenToString(lit->get()->value, debug);
                }
                else {
                    res += "Expr";
                }
            }
            res += "]";
            return res;
        },
    }, lit);
}

std::string script::tokenToString(Op op, bool debug) {
    if (debug) {
        return fmt::format("op({})", std::get<0>(OPS.at(op)));
    }
    return std::get<0>(OPS.at(op));
}

std::string script::tokenToString(Punct punct, bool debug) {
    if (debug) {
        return fmt::format("punct('{}')", punct);
    }
    return std::string(1, punct);
}

std::string Token::toString(bool debug) const {
    return std::visit(makeVisitor {
        [&](auto const& value) {
            return tokenToString(value, debug);
        },
    }, value);
}

void Token::skip(InputStream& stream) {
    stream >> std::ws;
}

Result<Token> Token::pull(InputStream& stream) {
    skip(stream);
    auto c = stream.peek();

    Rollback rb(stream);
    
    // string literals
    if (c == '"') {
        std::string str;
        stream >> std::quoted(str);
        if (stream.fail()) {
            return Err("Invalid string literal");
        }
        rb.commit();
        return Ok(Token(Lit(str)));
    }
    
    // number literals
    if (isdigit(c) || c == '.' || c == '+' || c == '-') {
        // remember position, as "75.0" would first be parsed as int but we 
        // need to backtrack to parse as float instead if 
        auto pos = stream.tellg();
        // first try parsing as int
        IntLit lit;
        stream >> lit;
        // if it's an invalid int, try parsing as float
        if (stream.fail() || stream.peek() == '.') {
            // go back to start of literal and clear errors
            stream.seekg(pos);
            stream.clear();
            FloatLit lit;
            stream >> lit;
            if (stream.fail()) {
                return Err("Invalid number literal");
            }
            rb.commit();
            return Ok(Token(Lit(lit)));
        }
        rb.commit();
        return Ok(Token(Lit(lit)));
    }

    // punctuation
    if (std::string("()[]{}:;,.").find_first_of(c) != std::string::npos) {
        return Ok(Token(Punct(c)));
    }

    // other
    std::string value;
    stream >> value;

    if (value == "true") {
        return Ok(Token(Lit(true)));
    }
    if (value == "false") {
        return Ok(Token(Lit(false)));
    }
    if (value == "null") {
        return Ok(Token(Lit(NullLit())));
    }

    // operator
    for (auto const& [op, va] : OPS) {
        if (std::get<0>(va) == value) {
            rb.commit();
            return Ok(Token(op));
        }
    }
    if (isOp(value)) {
        return Err(fmt::format("Invalid operator '{}'", value));
    }

    // keyword
    for (auto const& [kw, va] : KEYWORDS) {
        if (va == value) {
            rb.commit();
            return Ok(Token(kw));
        }
    }

    // identifier
    if (isIdent(value)) {
        rb.commit();
        return Ok(Token(value));
    }
    
    return Err(fmt::format("Invalid keyword or identifier '{}'", value));
}

Result<> Token::pull(Op op, InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
    if (auto value = std::get_if<Op>(&token.value)) {
        if (*value == op) {
            rb.commit();
            return Ok();
        }
        else {
            return Err(fmt::format(
                "Expected {}, got {}",
                tokenToString(op), tokenToString(*value)
            ));
        }
    }
    return Err(fmt::format(
        "Expected {}, got '{}'",
        tokenToString(op), token.toString()
    ));
}

Result<> Token::pull(Keyword kw, InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
    if (auto value = std::get_if<Keyword>(&token.value)) {
        if (*value == kw) {
            rb.commit();
            return Ok();
        }
        else {
            return Err(fmt::format(
                "Expected {}, got {}",
                tokenToString(kw), tokenToString(*value)
            ));
        }
    }
    return Err(fmt::format(
        "Expected {}, got '{}'",
        tokenToString(kw), token.toString()
    ));
}

Result<> Token::pull(char c, InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
    if (auto value = std::get_if<Punct>(&token.value)) {
        if (*value == c) {
            rb.commit();
            return Ok();
        }
    }
    return Err(fmt::format("Expected '{}', got '{}'", c, token.toString()));
}

std::optional<Token> Token::peek(InputStream& stream) {
    auto pos = stream.tellg();
    auto token = Token::pull(stream);
    stream.seekg(pos);
    stream.clear();
    return token.ok();
}

size_t Token::prio(InputStream& stream) {
    auto pos = stream.tellg();
    auto token = Token::pull(stream);
    stream.seekg(pos);
    stream.clear();
    if (token) {
        if (auto op = std::get_if<Op>(&token.unwrap().value)) {
            return std::get<1>(OPS.at(*op));
        }
    }
    return 0;
}

bool Token::eof(InputStream& stream) {
    Token::skip(stream);
    return stream.eof();
}

bool Value::isNull() const {
    return std::holds_alternative<NullLit>(value);
}

bool Value::truthy() const {
    return std::visit(makeVisitor {
        [&](NullLit const&) {
            return false;
        },
        [&](BoolLit const& b) {
            return b;
        },
        [&](IntLit const& num) {
            return bool(num);
        },
        [&](FloatLit const& num) {
            return bool(num);
        },
        [&](StrLit const& str) {
            return !str.empty();
        },
        [&](Array const& arr) {
            return !arr.empty();
        },
        [&](Ref<GameObject> const&) {
            return true;
        },
        [&](Rc<const FunExpr> const&) {
            return true;
        },
    }, value);
}

bool Value::typeEq(Rc<Value> other) const {
    // null type compares equal to everything else :cowboy:
    // this allows things like obj = null; obj = <actual object>
    // this also allows circumventing the type system but that's a lil trolling
    if (
        std::holds_alternative<NullLit>(value) ||
        std::holds_alternative<NullLit>(other->value)
    ) {
        return true;
    }
    return value.index() == other->value.index();
}

std::string Value::toString(bool debug) const {
    return std::visit(makeVisitor {
        [&](Ref<GameObject> const& obj) {
            if (debug) {
                return fmt::format("object({})", static_cast<void*>(obj.data()));
            }
            return std::string("object");
        },
        [&](Array const& arr) {
            std::string res = debug ? "array(" : "[";
            bool first = true;
            for (auto& item : arr) {
                if (!first) {
                    res += ", ";
                }
                first = false;
                res += item.toString(debug);
            }
            res += debug ? ")" : "]";
            return res;
        },
        [&](Rc<const FunExpr> const& fun) {
            if (debug) {
                return fmt::format("function({})", fun->ident);
            }
            return fun->ident;
        },
        [&](auto const& lit) {
            return tokenToString(static_cast<Lit>(lit), debug);
        },
    }, value);
}

std::string Value::typeName() const {
    return std::visit(makeVisitor {
        [&](NullLit const&) {
            return "null";
        },
        [&](BoolLit const&) {
            return "bool";
        },
        [&](IntLit const&) {
            return "int";
        },
        [&](FloatLit const&) {
            return "float";
        },
        [&](StrLit const&) {
            return "string";
        },
        [&](std::vector<Value> const&) {
            return "array";
        },
        [&](Ref<GameObject> const&) {
            return "object";
        },
        [&](Rc<const FunExpr> const&) {
            return "function";
        },
    }, value);
}

void State::add(std::string const& name, Rc<Value> value) {
    entities.insert({ name, { value, scope }});
}

Rc<Value> State::get(std::string const& name) {
    return entities.at(name).first;
}

void State::push() {
    scope += 1;
}

void State::drop() {
    for (auto& [ent, val] : entities) {
        if (val.second >= scope) {
            entities.erase(ent);
        }
    }
    if (scope) {
        scope -= 1;
    }
    else {
        throw std::runtime_error("Internal error: Base scope dropped");
    }
}

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
            if (Token::peek<']'>(stream)) {
                break;
            }
        }
        GEODE_UNWRAP(Token::pull(']', stream));
        rb.commit();
        return make<LitExpr>({
            .value = arr
        });
    }
    GEODE_UNWRAP_INTO(auto value, Token::pull<Lit>(stream));
    rb.commit();
    return make<LitExpr>({
        .value = value
    });
}

Result<Rc<Value>> LitExpr::eval(State& state) const {
    return std::visit(makeVisitor {
        [&](NullLit const&) -> Result<Rc<Value>> {
            return Ok(Value::rc(NullLit()));
        },
        [&](BoolLit const& b) -> Result<Rc<Value>> {
            return Ok(Value::rc(b));
        },
        [&](IntLit const& num) -> Result<Rc<Value>> {
            return Ok(Value::rc(num));
        },
        [&](FloatLit const& num) -> Result<Rc<Value>> {
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
                value.push_back(*val.get());
            }
            return Ok(Value::rc(value));
        },
    }, value);
}

Result<Rc<IdentExpr>> IdentExpr::pull(InputStream& stream) {
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    return make<IdentExpr>({
        .ident = ident
    });
}

Result<Rc<Value>> IdentExpr::eval(State& state) const {
    if (!state.entities.count(ident)) {
        state.add(ident, Value::rc(NullLit()));
    }
    return Ok(state.get(ident));
}

Result<Rc<UnOpExpr>> UnOpExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto op, Token::pull<Op>(stream));
    if (!isUnOp(op)) {
        return Err(fmt::format("Invalid unary operator '{}'", tokenToString(op)));
    }
    GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
    rb.commit();
    return make<UnOpExpr>({
        .expr = expr,
        .op = op
    });
}

Result<Rc<Value>> UnOpExpr::eval(State& state) const {
    GEODE_UNWRAP_INTO(auto value, expr->eval(state));
    switch (op) {
        case Op::Not: {
            return Ok(Value::rc(!value->truthy()));
        } break;

        case Op::Add: {
            if (auto num = value->has<IntLit>()) {
                return Ok(Value::rc(+*num));
            }
            if (auto num = value->has<FloatLit>()) {
                return Ok(Value::rc(+*num));
            }
            return Err(fmt::format(
                "Attempted to use unary plus operator on value of type {}", 
                value->typeName()
            ));
        } break;

        case Op::Sub: {
            if (auto num = value->has<IntLit>()) {
                return Ok(Value::rc(-*num));
            }
            if (auto num = value->has<FloatLit>()) {
                return Ok(Value::rc(-*num));
            }
            return Err(fmt::format(
                "Attempted to use unary minus operator on value of type {}", 
                value->typeName()
            ));
        } break;

        default: {
            return unimplemented<UnOpExpr>(op);
        } break;
    }
}

Result<Rc<Expr>> BinOpExpr::pull(InputStream& stream, size_t p, Rc<Expr> lhs) {
    Rollback rb(stream);
    while (true) {
        if (!Token::peek<Op>(stream)) break;

        GEODE_UNWRAP_INTO(auto op, Token::pull<Op>(stream));
        auto pop = std::get<1>(OPS.at(op));
        if (pop < p) {
            rb.commit();
            return Ok(lhs);
        }
        GEODE_UNWRAP_INTO(auto rhs, Expr::pullPrimary(stream));

        if (pop < Token::prio(stream)) {
            GEODE_UNWRAP_INTO(rhs, BinOpExpr::pull(stream, pop + 1, rhs));
        }

        lhs = Rc<Expr>(new Expr {
            .value = Rc<BinOpExpr>(new BinOpExpr {
                .lhs = lhs,
                .rhs = rhs,
                .op = op
            })
        });
    }
    rb.commit();
    return Ok(lhs);
}

Result<Rc<Expr>> BinOpExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto lhs, Expr::pullPrimary(stream));
    if (Token::peek<Op>(stream)) {
        rb.commit();
        return BinOpExpr::pull(stream, 0, lhs);
    }
    rb.commit();
    return Ok(lhs);
}

Result<Rc<Value>> BinOpExpr::eval(State& state) const {
    GEODE_UNWRAP_INTO(auto lhsValue, lhs->eval(state));
    GEODE_UNWRAP_INTO(auto rhsValue, rhs->eval(state));
    switch (op) {
        case Op::Assign: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't assign type {} to type {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            lhsValue.swap(rhsValue);
        } break;

        case Op::Add: {
            auto value = std::visit(makeVisitor {
                [&](IntLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a + b);
                },
                [&](IntLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a + b);
                },
                [&](FloatLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a + b);
                },
                [&](FloatLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a + b);
                },
                [&](StrLit const& a, StrLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a + b);
                },
                [&](Array const& a, auto const& b) -> std::optional<Rc<Value>> {
                    Array copy = a;
                    if (auto arr = rhsValue->has<Array>()) {
                        ranges::push(copy, *arr);
                    }
                    else {
                        copy.push_back(static_cast<Value>(b));
                    }
                    return Value::rc(copy);
                },
                [&](auto const&, auto const&) -> std::optional<Rc<Value>> {
                    return std::nullopt;
                },
            }, lhsValue->value, rhsValue->value);
            if (value) {
                return Ok(value.value());
            }
            return Err(fmt::format(
                "Cannot add {} and {}",
                lhsValue->typeName(), rhsValue->typeName()
            ));
        } break;

        case Op::Sub: {
            auto value = std::visit(makeVisitor {
                [&](IntLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a - b);
                },
                [&](IntLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a - b);
                },
                [&](FloatLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a - b);
                },
                [&](FloatLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a - b);
                },
                [&](auto const&, auto const&) -> std::optional<Rc<Value>> {
                    return std::nullopt;
                },
            }, lhsValue->value, rhsValue->value);
            if (value) {
                return Ok(value.value());
            }
            return Err(fmt::format(
                "Cannot subtract {} and {}",
                lhsValue->typeName(), rhsValue->typeName()
            ));
        } break;

        case Op::Mul: {
            auto value = std::visit(makeVisitor {
                [&](IntLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a * b);
                },
                [&](IntLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a * b);
                },
                [&](FloatLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a * b);
                },
                [&](FloatLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a * b);
                },
                [&](auto const&, auto const&) -> std::optional<Rc<Value>> {
                    return std::nullopt;
                },
            }, lhsValue->value, rhsValue->value);
            if (value) {
                return Ok(value.value());
            }
            return Err(fmt::format(
                "Cannot multiply {} and {}",
                lhsValue->typeName(), rhsValue->typeName()
            ));
        } break;

        case Op::Div: {
            auto value = std::visit(makeVisitor {
                [&](IntLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a / b);
                },
                [&](IntLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a / b);
                },
                [&](FloatLit const& a, IntLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a / b);
                },
                [&](FloatLit const& a, FloatLit const& b) -> std::optional<Rc<Value>> {
                    return Value::rc(a / b);
                },
                [&](auto const&, auto const&) -> std::optional<Rc<Value>> {
                    return std::nullopt;
                },
            }, lhsValue->value, rhsValue->value);
            if (value) {
                return Ok(value.value());
            }
            return Err(fmt::format(
                "Cannot divide {} and {}",
                lhsValue->typeName(), rhsValue->typeName()
            ));
        } break;

        case Op::Eq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value == rhsValue->value));
        } break;

        case Op::NotEq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value != rhsValue->value));
        } break;

        case Op::Less: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value < rhsValue->value));
        } break;

        case Op::LessEq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value <= rhsValue->value));
        } break;

        case Op::More: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value > rhsValue->value));
        } break;

        case Op::MoreEq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't compare types {} and {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            return Ok(Value::rc(lhsValue->value >= rhsValue->value));
        } break;

        case Op::And: {
            return Ok(Value::rc(lhsValue->truthy() && rhsValue->truthy()));
        } break;

        case Op::Or: {
            return Ok(Value::rc(lhsValue->truthy() || rhsValue->truthy()));
        } break;

        default: {
            return unimplemented<UnOpExpr>(op);
        } break;
    }
}

Result<Rc<CallExpr>> CallExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto target, Expr::pull(stream));
    GEODE_UNWRAP(Token::pull('(', stream));
    std::vector<Rc<Expr>> args;
    std::unordered_map<std::string, Rc<Expr>> named;
    while (true) {
        bool isNamed = false;
        Rollback namedrb(stream);
        // named args are in the form `<ident> = <expr>`
        if (auto ident = Token::pull<Ident>(stream)) {
            if (Token::pull(Op::Eq, stream)) {
                GEODE_UNWRAP_INTO(auto value, Expr::pull(stream));
                if (named.count(ident.unwrap())) {
                    return Err(fmt::format(
                        "Named argument '{}' has already been passed",
                        ident.unwrap()
                    ));
                }
                named.insert({ ident.unwrap(), value });
                isNamed = true;
            }
            // if no eq operator, then this is a normal binop arg. whoops!
        }
        if (!isNamed) {
            namedrb.ret();
            GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
            args.push_back(expr);
        }
        // allow trailing comma
        if (!Token::pull(',', stream) || Token::peek<')'>(stream)) {
            break;
        }
    }
    GEODE_UNWRAP(Token::pull(')', stream));
    rb.commit();
    return make<CallExpr>({
        .expr = target,
        .args = args,
        .named = named,
    });
}

Result<Rc<Value>> CallExpr::eval(State& state) const {
    GEODE_UNWRAP_INTO(auto val, expr->eval(state));
    auto funp = std::get_if<Rc<const FunExpr>>(&val->value);
    if (!funp) {
        return Err("Attempted to call {} as a function", val->typeName());
    }
    auto fun = *funp;

    std::unordered_map<std::string, Rc<Value>> res;
    for (auto& [name, nexpr] : named) {
        GEODE_UNWRAP_INTO(auto val, nexpr->eval(state));
        res.insert({ name, val });
    }

    auto i = 0u;
    for (auto& [name, _] : fun->params) {
        if (args.size() > i) {
            auto arg = args.at(i);
            if (res.count(name)) {
                return Err(fmt::format(
                    "Argument '{}' passed multiple times",
                    name
                ));
            }
            GEODE_UNWRAP_INTO(auto val, arg->eval(state));
            res.insert({ name, val });
            i++;
        }
        else {
            break;
        }
    }
    if (args.size() != i) {
        return Err("Function called with too many arguments");
    }

    // insert default values
    for (auto& [name, def] : fun->params) {
        if (!res.count(name)) {
            if (!def) {
                return Err(fmt::format("Missing required parameter '{}'", name));
            }
            else {
                GEODE_UNWRAP_INTO(auto val, def.value()->eval(state));
                res.insert({ name, val });
            }
        }
    }

    state.push();
    for (auto& [p, value] : res) {
        state.add(p, value);
    }
    GEODE_UNWRAP_INTO(auto ret, fun->body->eval(state));
    state.drop();
    return Ok(ret);
}

Result<Rc<ForInExpr>> ForInExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::For, stream));
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    GEODE_UNWRAP(Token::pull(Keyword::In, stream));
    GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pullBlock(stream));
    rb.commit();
    return make<ForInExpr>({
        .item = ident,
        .expr = expr,
        .body = body,
    });
}

Result<Rc<ListExpr>> ListExpr::pull(InputStream& stream) {
    // handle just {}
    if (Token::pull('}', stream)) {
        return make<ListExpr>({
            .exprs = {}
        });
    }
    Rollback rb(stream);
    std::vector<Rc<Expr>> list;
    while (true) {
        GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
        list.push_back(expr);
        // allow any number of semicolons between expressions
        while (Token::pull(';', stream)) {}
        if (Token::pull('}', stream) || Token::eof(stream)) {
            break;
        }
    }
    rb.commit();
    return make<ListExpr>({
        .exprs = list
    });
}

Result<Rc<Expr>> ListExpr::pullBlock(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull('{', stream));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pull(stream));
    GEODE_UNWRAP(Token::pull('}', stream));
    rb.commit();
    return make<Expr>({ .value = body });
}

Result<Rc<FunExpr>> FunExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Function, stream));
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    GEODE_UNWRAP(Token::pull('(', stream));
    std::vector<std::pair<Ident, std::optional<Rc<Expr>>>> params;
    while (true) {
        GEODE_UNWRAP_INTO(auto name, Token::pull<Ident>(stream));
        if (Token::pull(Op::Eq, stream)) {
            GEODE_UNWRAP_INTO(auto value, Expr::pull(stream));
            params.push_back({ name, value });
        }
        else {
            params.push_back({ name, std::nullopt });
        }
        if (!Token::pull(',', stream) || Token::peek<')'>(stream)) {
            break;
        }
    }
    GEODE_UNWRAP(Token::pull(')', stream));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pullBlock(stream));
    rb.commit();
    return make<FunExpr>({
        .ident = ident,
        .params = params,
        .body = body,
    });
}

Result<Rc<Value>> FunExpr::eval(State& state) const {
    // added during preval
    if (!state.entities.count(ident)) {
        return internal<FunExpr>("Function not added during preval");
    }
    return Ok(state.get(ident));
}

Result<> FunExpr::preval(State& state) const {
    if (state.entities.count(ident)) {
        return Err(fmt::format("Function '{}' is already defined", ident));
    }
    state.add(ident, Value::rc(shared_from_this()));
    return Ok();
}

Result<Rc<Expr>> Expr::pull(InputStream& stream) {
    return BinOpExpr::pull(stream);
}

Result<Rc<Expr>> Expr::pullPrimary(InputStream& stream) {
#define TRY_PULL(expr) \
    if (auto value = expr::pull(stream)) {\
        return make<Expr>({ .value = value.unwrap() }); \
    }
    // try first pulling an unary expr (which will pull the rest if succesful)
    TRY_PULL(UnOpExpr);
    // then try pulling a call expr (which will pull the rest if succesful)
    TRY_PULL(CallExpr);
    // then try pulling other stuff
    TRY_PULL(IfExpr);
    TRY_PULL(ForInExpr);
    if (Token::pull('{', stream)) {
        GEODE_UNWRAP_INTO(auto expr, ListExpr::pull(stream));
        GEODE_UNWRAP(Token::pull('}', stream));
        return make<Expr>({ .value = expr });
    }
    TRY_PULL(LitExpr);
    TRY_PULL(IdentExpr);
    if (auto token = Token::peek(stream)) {
        return Err("Expected expression, got '{}'", token.value().toString());
    }
    else {
        return Err("Expected expression");
    }
}

Result<Rc<Value>> Expr::eval(State& state) const {
    return std::visit(makeVisitor {
        [&](auto const& expr) {
            return expr->eval(state);
        },
    }, value);
}

Result<> Expr::preval(State& state) const {
    return std::visit(makeVisitor {
        [&](auto const& expr) {
            return expr->preval(state);
        },
    }, value);
}

bool script::operator==(Array const& a, Array const b) {
    if (a.size() != b.size()) return false;
    for (auto i = 0u; i < a.size(); i++) {
        if (a[i].value != b[i].value) {
            return false;
        }
    }
    return true;
}

bool script::operator!=(Array const& a, Array const b) {
    return !(a == b);
}

bool script::operator<(Array const& a, Array const b) {
    return false;
}

bool script::operator<=(Array const& a, Array const b) {
    return true;
}

bool script::operator>(Array const& a, Array const b) {
    return false;
}

bool script::operator>=(Array const& a, Array const b) {
    return true;
}
