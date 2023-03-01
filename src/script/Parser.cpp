#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/utils/map.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/loader/Log.hpp>

using namespace script;

// this does mean that you can't run scripts in multiple threads, but eh you 
// couldn't do that anyway because of GameObject
static size_t EXECUTION_DEPTH = 0;
static size_t EXECUTION_LIMIT = 0xfff;

static void checkInfiniteLoop() {
    EXECUTION_DEPTH += 1;
    if (EXECUTION_DEPTH > EXECUTION_LIMIT) {
        throw std::runtime_error(fmt::format(
            "Script exceeded the execution limit of {}. "
            "This is likely because it ended up in an infinite loop, report "
            "this bug to HJfod!",
            EXECUTION_LIMIT
        ));
    }
}

static std::string prettify(std::string const& msg) {
    std::string res = "";
    size_t i = 0;
    for (auto& c : msg) {
        if (c == '(') {
            i += 4;
            res += "(\n" + std::string(i, ' ');
        }
        else if (c == ')') {
            i -= 4;
            res += "\n" + std::string(i, ' ') + ")";
        }
        else if (c == ',') {
            res += ",\n" + std::string(i, ' ');
        }
        else {
            res += c;
        }
    }
    return res;
}

template <class T, class... Args>
Result<Rc<T>> make(Args&&... args) {
    return Ok(std::make_shared<T>(std::forward<Args>(args)...));
}

template <class T>
Result<Rc<T>> make(T value) {
    return Ok(std::make_shared<T>(value));
}

std::string getStmt(std::streampos start, InputStream& stream) {
    auto to = stream.tellg();
    stream.seekg(start);
    std::string buf;
    buf.reserve(to - start);
    stream.read(buf.data(), to - start);
    return buf;
}

static std::unordered_map<Keyword, std::string> KEYWORDS {
    { Keyword::For,         "for" },
    { Keyword::While,       "while" },
    { Keyword::In,          "in" },
    { Keyword::If,          "if" },
    { Keyword::Else,        "else" },
    { Keyword::Function,    "function" },
    { Keyword::Return,      "return" },
    { Keyword::Break,       "break" },
    { Keyword::Continue,    "continue" },
    { Keyword::New,         "new" },
    { Keyword::True,        "true" },
    { Keyword::False,       "false" },
    { Keyword::Null,        "null" },
};

static std::unordered_map<Op, std::tuple<std::string, size_t, OpDir>> OPS {
    { Op::Not,      { "!",  7,  OpDir::RTL } },
    { Op::Mul,      { "*",  6,  OpDir::LTR } },
    { Op::Div,      { "/",  6,  OpDir::LTR } },
    { Op::Mod,      { "%",  6,  OpDir::LTR } },
    { Op::Add,      { "+",  5,  OpDir::LTR } },
    { Op::Sub,      { "-",  5,  OpDir::LTR } },
    { Op::Eq,       { "==", 4,  OpDir::LTR } },
    { Op::Neq,      { "!=", 4,  OpDir::LTR } },
    { Op::Less,     { "<",  4,  OpDir::LTR } },
    { Op::Leq,      { "<=", 4,  OpDir::LTR } },
    { Op::More,     { ">",  4,  OpDir::LTR } },
    { Op::Meq,      { ">=", 4,  OpDir::LTR } },
    { Op::And,      { "&&", 3,  OpDir::LTR } },
    { Op::Or,       { "||", 2,  OpDir::LTR } },
    { Op::ModSeq,   { "%=", 1,  OpDir::RTL } },
    { Op::DivSeq,   { "/=", 1,  OpDir::RTL } },
    { Op::MulSeq,   { "*=", 1,  OpDir::RTL } },
    { Op::SubSeq,   { "-=", 1,  OpDir::RTL } },
    { Op::AddSeq,   { "+=", 1,  OpDir::RTL } },
    { Op::Seq,      { "=",  1,  OpDir::RTL } },
};
static constexpr size_t LONGEST_OP = 2;

static std::string INVALID_IDENT_CHARS = ".,;(){}[]`\\´¨'\"";
static std::string VALID_OP_CHARS = "=+-/*<>!#?&|%@:~^";

bool script::isIdentCh(char ch) {
    return
        // no reserved chars
        INVALID_IDENT_CHARS.find_first_of(ch) == std::string::npos && 
        // no operators
        VALID_OP_CHARS.find_first_of(ch) == std::string::npos &&
        // no spaces
        !std::isspace(ch);
}

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
        if (!isIdentCh(c)) {
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

bool script::isOpCh(char ch) {
    return VALID_OP_CHARS.find_first_of(ch) != std::string::npos;
}

bool script::isOp(std::string const& op) {
    for (auto& c : op) {
        if (!isOpCh(c)) {
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
        [&](NumLit const& num) -> std::string {
            if (debug) {
                return fmt::format("number({})", num);
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
    // this tellg is needed for some reason. if you remove it, the parser can 
    // end up in an infinite loop at the end of a script. i don't know why
    stream.tellg();
}

Result<Token> Token::pull(InputStream& stream) {
    skip(stream);

    checkInfiniteLoop();

    Rollback rb(stream);

    if (stream.eof()) {
        return Err("Expected token, found end-of-file");
    }
    
    auto c = stream.peek();

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
    // todo: hex
    if (isdigit(c) || c == '.') {
        auto pos = stream.tellg();
        NumLit lit;
        stream >> lit;
        if (stream.fail()) {
            return Err("Invalid number literal");
        }
        rb.commit();
        return Ok(Token(Lit(lit)));
    }

    // punctuation
    if (std::string("()[]{}:;,.").find_first_of(c) != std::string::npos) {
        stream.get();
        rb.commit();
        return Ok(Token(Punct(c)));
    }

    // other
    auto pos = stream.tellg();
    std::string ident;
    while (isIdentCh(stream.peek())) {
        ident += stream.get();
    }

    // special literals
    if (ident == "true") {
        rb.commit();
        return Ok(Token(Lit(true)));
    }
    if (ident == "false") {
        rb.commit();
        return Ok(Token(Lit(false)));
    }
    if (ident == "null") {
        rb.commit();
        return Ok(Token(Lit(NullLit())));
    }

    // keyword
    for (auto const& [kw, va] : KEYWORDS) {
        if (va == ident) {
            rb.commit();
            return Ok(Token(kw));
        }
    }

    // identifier
    if (isIdent(ident)) {
        rb.commit();
        return Ok(Token(ident));
    }

    // backtrack consumed ident, consume operator instead
    stream.seekg(pos);
    stream.clear();

    // other
    std::string maybeOp;
    while (isOpCh(stream.peek())) {
        maybeOp += stream.get();
    }

    // operator
    for (auto const& [op, va] : OPS) {
        if (std::get<0>(va) == maybeOp) {
            rb.commit();
            return Ok(Token(op));
        }
    }
    if (!ident.size()) {
        return Err(fmt::format("Invalid operator '{}'", maybeOp));
    }
    return Err(fmt::format("Invalid keyword or identifier '{}'", maybeOp));
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
    return Err("Expected '{}', got '{}'", c, token.toString());
}

std::optional<Token> Token::peek(InputStream& stream) {
    Rollback rb(stream);
    return Token::pull(stream).ok();
}

size_t Token::prio(InputStream& stream) {
    Rollback rb(stream);
    auto token = Token::pull(stream);
    if (token) {
        if (auto op = std::get_if<Op>(&token.unwrap().value)) {
            return std::get<1>(OPS.at(*op));
        }
    }
    return 0;
}

OpDir Token::dir(InputStream& stream) {
    Rollback rb(stream);
    auto token = Token::pull(stream);
    if (token) {
        if (auto op = std::get_if<Op>(&token.unwrap().value)) {
            return std::get<2>(OPS.at(*op));
        }
    }
    return OpDir::LTR;
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
        [&](NumLit const& num) {
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
        [&](NumLit const&) {
            return "number";
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

Result<Rc<UnOpExpr>> UnOpExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto op, Token::pull<Op>(stream));
    if (!isUnOp(op)) {
        return Err(fmt::format("Invalid unary operator '{}'", tokenToString(op)));
    }
    GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
    return make<UnOpExpr>({
        .expr = expr,
        .op = op,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> UnOpExpr::eval(State& state) {
    GEODE_UNWRAP_INTO(auto value, expr->eval(state));
    switch (op) {
        case Op::Not: {
            return Ok(Value::rc(!value->truthy()));
        } break;

        case Op::Add: {
            if (auto num = value->has<NumLit>()) {
                return Ok(Value::rc(+*num));
            }
            return Err(fmt::format(
                "Attempted to use unary plus operator on value of type {}", 
                value->typeName()
            ));
        } break;

        case Op::Sub: {
            if (auto num = value->has<NumLit>()) {
                return Ok(Value::rc(-*num));
            }
            return Err(fmt::format(
                "Attempted to use unary minus operator on value of type {}", 
                value->typeName()
            ));
        } break;

        default: {
            throw std::runtime_error(fmt::format(
                "Internal error: Unimplemented unary operator {}",
                tokenToString(op, true)
            ));
        } break;
    }
}

std::string UnOpExpr::debug() const {
    return fmt::format(
        "UnOpExpr({}, {})",
        tokenToString(op, true), expr->debug()
    );
}

Result<Rc<Expr>> BinOpExpr::pull(InputStream& stream, size_t p, Rc<Expr> lhs) {
    Rollback rb(stream);
    while (true) {
        auto op = Token::peek<Op>(stream);
        if (!op) break;

        auto pop = std::get<1>(OPS.at(op.value()));
        if (pop < p) {
            rb.commit();
            return Ok(lhs);
        }
        // consume op
        (void)Token::pull<Op>(stream);
        GEODE_UNWRAP_INTO(auto rhs, Expr::pullPrimary(stream));

        if (Token::dir(stream) == OpDir::RTL) {
            GEODE_UNWRAP_INTO(rhs, BinOpExpr::pull(stream, p, rhs));
        }
        if (pop < Token::prio(stream)) {
            GEODE_UNWRAP_INTO(rhs, BinOpExpr::pull(stream, pop + 1, rhs));
        }

        lhs = make<Expr>({
            .value = make<BinOpExpr>({
                .lhs = lhs,
                .rhs = rhs,
                .op = op.value(),
                .src = rb.commit(),
            }).unwrap()
        }).unwrap();
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

Result<Rc<Value>> BinOpExpr::eval(State& state) {
    GEODE_UNWRAP_INTO(auto lhsValue, lhs->eval(state));
    GEODE_UNWRAP_INTO(auto rhsValue, rhs->eval(state));
    switch (op) {
        case Op::Seq: {
            if (!lhsValue->typeEq(rhsValue)) {
                return Err(fmt::format(
                    "Can't assign type {} to type {}",
                    lhsValue->typeName(), rhsValue->typeName()
                ));
            }
            lhsValue->value = rhsValue->value;
            return Ok(lhsValue);
        } break;

        case Op::AddSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a += b;
                    return Ok(Value::rc(a));
                },
                [&](StrLit& a, StrLit const& b) -> Result<Rc<Value>> {
                    a += b;
                    return Ok(Value::rc(a));
                },
                [&](Array& a, auto const&) -> Result<Rc<Value>> {
                    if (auto arr = rhsValue->has<Array>()) {
                        ranges::push(a, *arr);
                    }
                    else {
                        a.push_back(*rhsValue);
                    }
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot add {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::SubSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a -= b;
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot subtract {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::MulSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a *= b;
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot multiply {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::DivSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a /= b;
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::ModSeq: {
            return std::visit(makeVisitor {
                [&](NumLit& a, NumLit const& b) -> Result<Rc<Value>> {
                    a = remainder(a, b);
                    return Ok(Value::rc(a));
                },
                [&](auto&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Add: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a + b));
                },
                [&](StrLit const& a, StrLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a + b));
                },
                [&](Array const& a, auto const&) -> Result<Rc<Value>> {
                    Array copy = a;
                    if (auto arr = rhsValue->has<Array>()) {
                        ranges::push(copy, *arr);
                    }
                    else {
                        copy.push_back(*rhsValue);
                    }
                    return Ok(Value::rc(copy));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot add {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Sub: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a - b));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot subtract {} and {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Mul: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a * b));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot multiply {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Div: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(a / b));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
        } break;

        case Op::Mod: {
            return std::visit(makeVisitor {
                [&](NumLit const& a, NumLit const& b) -> Result<Rc<Value>> {
                    return Ok(Value::rc(remainder(a, b)));
                },
                [&](auto const&, auto const&) -> Result<Rc<Value>> {
                    return Err(fmt::format(
                        "Cannot divide {} with {}",
                        lhsValue->typeName(), rhsValue->typeName()
                    ));
                },
            }, lhsValue->value, rhsValue->value);
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

        case Op::Neq: {
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

        case Op::Leq: {
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

        case Op::Meq: {
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
            throw std::runtime_error(fmt::format(
                "Internal error: Unimplemented binary operator {}",
                tokenToString(op, true)
            ));
        } break;
    }
}

std::string BinOpExpr::debug() const {
    return fmt::format(
        "BinOpExpr({}, {}, {})",
        lhs->debug(), tokenToString(op, true), rhs->debug()
    );
}

Result<Rc<CallExpr>> CallExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto target, Expr::pullPrimaryNonCall(stream));
    GEODE_UNWRAP(Token::pull('(', stream));
    // handle ()
    if (Token::pull(')', stream)) {
        return make<CallExpr>({
            .expr = target,
            .args = {},
            .named = {},
            .src = rb.commit(),
        });
    }
    std::vector<Rc<Expr>> args;
    std::unordered_map<std::string, Rc<Expr>> named;
    while (true) {
        bool isNamed = false;
        Rollback namedrb(stream);
        // named args are in the form `<ident> = <expr>`
        if (auto ident = Token::pull<Ident>(stream)) {
            if (Token::pull(Op::Seq, stream)) {
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
        namedrb.commit();
        // allow trailing comma
        if (!Token::pull(',', stream) || Token::peek(')', stream)) {
            break;
        }
    }
    GEODE_UNWRAP(Token::pull(')', stream));
    return make<CallExpr>({
        .expr = target,
        .args = args,
        .named = named,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> CallExpr::eval(State& state) {
    GEODE_UNWRAP_INTO(auto val, expr->eval(state));
    auto funp = std::get_if<Rc<const FunExpr>>(&val->value);
    if (!funp) {
        return Err(
            "Attempted to call {} as a function\n * {} evaluated to null",
            val->typeName(), expr->src()
        );
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

    auto scope = state.scope();
    for (auto& [p, value] : res) {
        state.add(p, value);
    }
    try {
        GEODE_UNWRAP_INTO(auto ret, fun->body->eval(state));
        return Ok(ret);
    }
    catch(ReturnSignal& sig) {
        return Ok(sig.value);
    }
}

std::string CallExpr::debug() const {
    std::string a;
    bool first = true;
    for (auto& arg : args) {
        if (!first) {
            a += ", ";
        }
        first = false;
        a += arg->debug();
    }
    std::string n;
    first = true;
    for (auto& [na, va] : named) {
        if (!first) {
            n += ", ";
        }
        first = false;
        n += fmt::format("Named({}, {})", na, va->debug());
    }
    return fmt::format("CallExpr({}, [{}], [{}])", expr->debug(), a, n);
}

Result<Rc<ForInExpr>> ForInExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::For, stream));
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    GEODE_UNWRAP(Token::pull(Keyword::In, stream));
    GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
    GEODE_UNWRAP_INTO(auto body, ListExpr::pullBlock(stream));
    return make<ForInExpr>({
        .item = ident,
        .expr = expr,
        .body = body,
        .src = rb.commit(),
    });
}

Result<Rc<Value>> ForInExpr::eval(State& state) {
    GEODE_UNWRAP_INTO(auto value, expr->eval(state));
    auto arr = value->has<Array>();
    if (!arr) {
        return Err("Attempted to iterate {}, expected array", value->typeName());
    }
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

std::string ForInExpr::debug() const {
    return fmt::format(
        "ForInExpr({}, {}, {})",
        item, expr->debug(), body->debug()
    );
}

Result<Rc<IfExpr>> IfExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::If, stream));
    GEODE_UNWRAP_INTO(auto cond, Expr::pull(stream));
    GEODE_UNWRAP_INTO(auto truthy, ListExpr::pullBlock(stream));
    std::optional<Rc<Expr>> falsy;
    if (Token::pull(Keyword::Else, stream)) {
        // else if
        if (Token::peek(Keyword::If, stream)) {
            GEODE_UNWRAP_INTO(auto ifFalsy, IfExpr::pull(stream));
            falsy = make<Expr>(ifFalsy).unwrap();
        }
        // otherwise expect a block
        else {
            GEODE_UNWRAP_INTO(falsy, ListExpr::pullBlock(stream));
        }
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
    return fmt::format("FunExpr({}, [{}], {})", ident, par, body->debug());
}

Result<Rc<ReturnExpr>> ReturnExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Return, stream));
    if (auto expr = Expr::pull(stream)) {
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

Result<Rc<BreakExpr>> BreakExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Break, stream));
    if (auto expr = Expr::pull(stream)) {
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

Result<Rc<ContinueExpr>> ContinueExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP(Token::pull(Keyword::Continue, stream));
    return make<ContinueExpr>(rb.commit());
}

Result<Rc<Value>> ContinueExpr::eval(State& state) {
    throw ContinueSignal();
}

std::string ContinueExpr::debug() const {
    return "Continue()";
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
    {
        Rollback rb(stream);
        TRY_PULL(CallExpr);
    }
    return Expr::pullPrimaryNonCall(stream);
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

Scope::Scope(State& state) : state(state) {
    state.push();
}

Scope::~Scope() {
    state.pop();
}

void State::add(std::string const& name, Rc<Value> value) {
    entities.back().insert({ name, value });
}

bool State::has(std::string const& name) const {
    for (auto& ent : ranges::reverse(entities)) {
        if (ent.count(name)) {
            return true;
        }
    }
    return false;
}

Rc<Value> State::get(std::string const& name) {
    for (auto& ent : ranges::reverse(entities)) {
        if (ent.count(name)) {
            return ent.at(name);
        }
    }
    return Value::rc(NullLit());
}

Scope State::scope() {
    return Scope(*this);
}

void State::push() {
    entities.emplace_back();
}

void State::pop() {
    entities.pop_back();
    if (!entities.size()) {
        throw std::runtime_error("Internal error: Base scope dropped");
    }
}

Result<> State::run(ghc::filesystem::path const& path, bool debug) {
    std::ifstream stream(path, std::ios::binary);
    GEODE_UNWRAP(State::run(stream, debug));
    return Ok();
}

Result<> State::run(std::string const& code, bool debug) {
    std::stringstream stream(code);
    GEODE_UNWRAP(State::run(stream, debug));
    return Ok();
}

Result<> State::run(InputStream& stream, bool debug) {
    try {
        EXECUTION_DEPTH = 0;
        GEODE_UNWRAP_INTO(auto ast, ListExpr::pull(stream));
        if (debug) {
            log::info("AST: {}", prettify(ast->debug()));
        }
        auto state = State();
        GEODE_UNWRAP_INTO(auto val, ast->eval(state));
        if (debug) {
            log::info("Script evaluated to {}", val->toString(true));
        }
    }
    catch (std::exception& err) {
        return Err(err.what());
    }
    return Ok();
}

State::State() {
    this->add("print", Value::rc(make<FunExpr>(
        "print",
        decltype(FunExpr::params) {{ "msg", std::nullopt }},
        make<Expr>({
            .value = make<CppExpr>({
                .eval = [](State& state) -> Result<Rc<Value>> {
                    log::info("{}", state.get("msg")->toString());
                    return Ok(state.get("msg"));
                }
            }).unwrap(),
        }).unwrap(),
        "/* built-in function */"
    ).unwrap()));
}
