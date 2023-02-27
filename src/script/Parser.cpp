#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/map.hpp>

using namespace script;

static std::unordered_map<Keyword, std::string> KEYWORDS {
    { Keyword::For,         "for" },
    { Keyword::While,       "while" },
    { Keyword::In,          "in" },
    { Keyword::If,          "if" },
    { Keyword::Else,        "else" },
    { Keyword::Function,    "function" },
    { Keyword::New,         "new" },
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

struct Rollback {
private:
    InputStream& m_stream;
    bool m_commit;
    bool m_rolled;
    size_t m_pos;

public:
    Rollback(InputStream& stream) : m_stream(stream), m_commit(false), m_rolled(false) {
        m_pos = stream.tellg();
    }
    ~Rollback() {
        if (!m_commit) {
            this->back();
        }
    }
    void commit() {
        m_commit = true;
    }
    void ret() {
        if (!m_rolled) {
            m_stream.seekg(m_pos);
            m_stream.clear();
        }
    }
};

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
        return fmt::format("identifier({})", std::quoted(ident));
    }
    return ident;
}

std::string script::tokenToString(Lit lit, bool debug) {
    std::visit(makeVisitor {
        [&](StrLit const& str) {
            if (debug) {
                return fmt::format("string({})", std::quoted(str));
            }
            return str;
        },
        [&](IntLit const& num) {
            if (debug) {
                return fmt::format("int({})", num);
            }
            return std::to_string(num);
        },
        [&](FloatLit const& num) {
            if (debug) {
                return fmt::format("float({})", num);
            }
            return std::to_string(num);
        }
    }, lit);
}

std::string script::tokenToString(Op op, bool debug) {
    if (debug) {
        return fmt::format("op({})", std::get<0>(OPS.at(op)));
    }
    return std::get<0>(OPS.at(op));
}

std::string Token::toString(bool debug) const {
    std::visit(makeVisitor {
        [&](Keyword const& kw) {
            return tokenToString(kw, debug);
        },
        [&](Op const& op) {
            return tokenToString(op, debug);
        },
        [&](Lit const& lit) {
            return tokenToString(lit);
        },
        [&](Ident const& ident) {
            return tokenToString(ident, debug);
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

Result<LitExpr> LitExpr::pull(InputStream& stream) {
    GEODE_UNWRAP_INTO(auto value, Token::pull<Lit>(stream));
    return Ok(LitExpr {
        .value = value
    });
}

Result<IdentExpr> IdentExpr::pull(InputStream& stream) {
    GEODE_UNWRAP_INTO(auto ident, Token::pull<Ident>(stream));
    return Ok(IdentExpr {
        .ident = ident
    });
}

Result<UnOpExpr> UnOpExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto op, Token::pull<Op>(stream));
    if (!isUnOp(op)) {
        return Err(fmt::format("Invalid unary operator '{}'", tokenToString(op)));
    }
    GEODE_UNWRAP_INTO(auto expr, Expr::pull(stream));
    rb.commit();
    return Ok(UnOpExpr {
        .expr = expr.rc(),
        .op = op
    });
}

Result<Expr> BinOpExpr::pull(InputStream& stream, size_t p, Expr lhs) {
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

        lhs = Expr {
            .value = BinOpExpr {
                .lhs = lhs.rc(),
                .rhs = rhs.rc(),
                .op = op
            }
        };
    }
    rb.commit();
    return Ok(lhs);
}

Result<Expr> BinOpExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto lhs, Expr::pullPrimary(stream));
    if (Token::peek<Op>(stream)) {
        rb.commit();
        return BinOpExpr::pull(stream, 0, lhs);
    }
    rb.commit();
    return Ok(lhs);
}

Result<CallExpr> CallExpr::pull(InputStream& stream) {
    Rollback rb(stream);
    GEODE_UNWRAP_INTO(auto target, Expr::pull(stream));
    GEODE_UNWRAP(Token::pull<'('>(stream));
    std::vector<Expr> args;
    std::unordered_map<std::string, Expr> named;
    while (true) {
        Rollback namedrb(stream);
        // named args are in the form `<ident> = <expr>`
        if (auto ident = Token::pull<Ident>(stream)) {
            if (auto op = Token::pull<Op::Eq>(stream)) {
                
            }
            // if no eq operator, then this is a normal binop arg. whoops!
            namedrb.ret();
        }
    }
}

Result<ForInExpr> ForInExpr::pull(InputStream& stream) {

}

Result<Expr> Expr::pull(InputStream& stream) {
    return BinOpExpr::pull(stream);
}

Result<Expr> Expr::pullPrimary(InputStream& stream) {
#define TRY_PULL(expr) \
    if (auto value = expr::pull(stream)) {\
        return Ok(Expr { .value = value.unwrap() }); \
    }
    // try first pulling an unary expr (which will pull the rest if succesful)
    TRY_PULL(UnOpExpr);
    // then try pulling a call expr (which will pull the rest if succesful)
    TRY_PULL(CallExpr);
    // then try pulling other stuff
    TRY_PULL(IfExpr);
    TRY_PULL(ForInExpr);
    TRY_PULL(BlockExpr);
    TRY_PULL(LitExpr);
    TRY_PULL(IdentExpr);
    if (auto token = Token::peek(stream)) {
        return Err("Expected expression, got '{}'", token.value().toString());
    }
    else {
        return Err("Expected expression");
    }
}

Rc<Expr> Expr::rc() const {
    return std::make_shared<Expr>(*this);
}
