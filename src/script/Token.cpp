
#include "Parser.hpp"
#include <Geode/utils/general.hpp>

using namespace script;

// this does mean that you can't run scripts in multiple threads, but eh you 
// couldn't do that anyway because of GameObject
static size_t EXECUTION_DEPTH = 0;
static size_t EXECUTION_LIMIT = 0xfffff;

void script::resetExecutionCounter() {
    EXECUTION_DEPTH = 0;
}

void script::tickExecutionCounter() {
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

std::string script::prettify(std::string const& msg) {
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

static std::unordered_map<Keyword, std::string> KEYWORDS {
    { Keyword::For,         "for" },
    { Keyword::While,       "while" },
    { Keyword::In,          "in" },
    { Keyword::If,          "if" },
    { Keyword::Else,        "else" },
    { Keyword::Try,         "try" },
    { Keyword::Function,    "function" },
    { Keyword::Return,      "return" },
    { Keyword::Break,       "break" },
    { Keyword::Continue,    "continue" },
    { Keyword::New,         "new" },
    { Keyword::Const,       "const" },
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
    { Op::Arrow,    { "=>", 0,  OpDir::RTL } },
};

static std::string INVALID_IDENT_CHARS = ".,;(){}[]@`\\´¨'\"";
static std::string VALID_OP_CHARS = "=+-/*<>!#?&|%:~^";

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

std::string Token::toString(bool debug) const {
    return std::visit(makeVisitor {
        [&](auto const& value) {
            return tokenToString(value, debug);
        },
    }, value);
}

void Token::skip(InputStream& stream) {
    while (true) {
        tickExecutionCounter();
        stream >> std::ws;
        if (stream.eof()) break;
        // comments
        auto pos = stream.tellg();
        if (stream.get() == '/') {
            if (stream.peek() == '/') {
                while (!stream.fail() && stream.get() != '\n') {}
            }
            else if (stream.peek() == '*') {
                while (!stream.fail() && (stream.get() != '*' || stream.peek() != '/')) {}
                stream.get(); // get last /
                // can't do while (get || get) because that causes an infinite 
                // loop at **/
            }
            else {
                stream.seekg(pos);
                stream.clear();
                break;
            }
        }
        else {
            stream.seekg(pos);
            stream.clear();
            break;
        }
    }
    // this tellg is needed for some reason. if you remove it, the parser can 
    // end up in an infinite loop at the end of a script. i don't know why
    stream.tellg();
}

Result<Token> Token::pull(InputStream& stream) {
    auto cpos = stream.tellg();
    Token::skip(stream);

    tickExecutionCounter();

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
    if (isdigit(c)) {
        NumLit lit;
        stream >> lit;
        if (stream.fail()) {
            return Err("Invalid number literal");
        }
        rb.commit();
        return Ok(Token(Lit(lit)));
    }

    // punctuation
    if (std::string("()[]{}:;,.@").find_first_of(c) != std::string::npos) {
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

Result<> Token::pull(const char* chs, InputStream& stream) {
    Rollback rb(stream);
    for (auto& c : std::string_view(chs)) {
        GEODE_UNWRAP(Token::pull(c, stream).expect("{error}\n * While getting \"{}\"", chs));
    }
    rb.commit();
    return Ok();
}

std::optional<Token> Token::peek(InputStream& stream) {
    Rollback rb(stream);
    return Token::pull(stream).ok();
}

bool Token::peek(const char* chs, InputStream& stream) {
    Rollback rb(stream);
    return Token::pull(chs, stream).isOk();
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
            return geode::utils::numToString(num);
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
