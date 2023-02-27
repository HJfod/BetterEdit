#pragma once

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <Geode/utils/Result.hpp>
#include <fmt/format.h>

USE_GEODE_NAMESPACE();

namespace script {
    using InputStream = std::istream;

    enum class Keyword {
        For, In, While,
        If, Else,
        Function, Return,
        New,
    };
    enum class Op {
        Assign, // a = b
        Add,    // a + b or +a
        Sub,    // a - b or -a
        Mul,    // a * b
        Div,    // a / b
        Eq,     // a == b
        NotEq,  // a != b
        Less,   // a < b
        LessEq, // a <= b
        More,   // a > b
        MoreEq, // a >= b
        Not,    // !a
        And,    // a && b
        Or,     // a || b
    };
    using StrLit = std::string;
    using IntLit = uint64_t;
    using FloatLit = double;
    using Lit = std::variant<StrLit, IntLit, FloatLit>;
    using Ident = std::string;
    using Punct = char;

    enum class OpDir : bool {
        LTR, RTL,
    };

    template <class T>
    struct TokenTraits;

    template <>
    struct TokenTraits<Keyword> {
        static constexpr const char* TYPE_NAME = "keyword";
    };

    template <>
    struct TokenTraits<Op> {
        static constexpr const char* TYPE_NAME = "operator";
    };

    template <>
    struct TokenTraits<Lit> {
        static constexpr const char* TYPE_NAME = "literal";
    };

    template <>
    struct TokenTraits<Ident> {
        static constexpr const char* TYPE_NAME = "identifier";
    };

    template <>
    struct TokenTraits<Punct> {
        static constexpr const char* TYPE_NAME = "punctuation";
    };

    template <class T>
    concept TokenType = requires {
        TokenTraits<T>::TYPE_NAME;
    };

    struct Token {
        std::variant<Keyword, Op, Lit, Punct, Ident> value;

        std::string toString(bool debug = false) const;
        static void skip(InputStream& stream);
        static Result<Token> pull(InputStream& stream);
        static std::optional<Token> peek(InputStream& stream);
        static size_t prio(InputStream& stream);

        template <TokenType T>
        static bool peek(InputStream& stream) {
            auto value = Token::peek(stream);
            return value && std::holds_alternative<T>(value.value().value);
        }

        template <TokenType T>
        static Result<T> pull(InputStream& stream) {
            GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
            if (auto value = std::get_if<T>(&token.value)) {
                return Ok(*value);
            }
            return Err(fmt::format(
                "Expected {}, got '{}'",
                TokenTraits<T>::TYPE_NAME, token.toString()
            ));
        }

        template <Op O>
        static Result<> pull(InputStream& stream) {
            GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
            if (auto value = std::get_if<Op>(&token.value)) {
                if (*value == O) {
                    return Ok();
                }
                else {
                    return Err(fmt::format(
                        "Expected {}, got {}",
                        tokenToString(O), tokenToString(token.unwrap())
                    ));
                }
            }
            return Err(fmt::format(
                "Expected {}, got '{}'",
                tokenToString(O), token.toString()
            ));
        }

        template <char C>
        static Result<> pull(InputStream& stream) {
            GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
            if (auto token = std::get_if<Paren>(&token.value)) {
                if (*token == C) {
                    return Ok();
                }
            }
            return Err(fmt::format("Expected '{}', got '{}'", C, token.toString()));
        }
    };

    std::string tokenToString(Keyword kw, bool debug = false);
    std::string tokenToString(Ident ident, bool debug = false);
    std::string tokenToString(Lit lit, bool debug = false);
    std::string tokenToString(Op op, bool debug = false);

    bool isIdent(std::string const& ident);
    bool isOp(std::string const& op);
    bool isUnOp(Op op);

    template <class T>
    using Rc = std::shared_ptr<T>;

    struct LitExpr;
    struct IdentExpr;
    struct UnOpExpr;
    struct BinOpExpr;
    struct CallExpr;
    struct ForInExpr;
    struct IfExpr;
    struct BlockExpr;
    struct Expr {
        std::variant<
            LitExpr, IdentExpr,
            UnOpExpr, BinOpExpr,
            CallExpr,
            ForInExpr, IfExpr,
            BlockExpr
        > value;

        Rc<Expr> rc() const;
        static Result<Expr> pullPrimary(InputStream& stream);
        static Result<Expr> pull(InputStream& stream);
    };

    struct LitExpr {
        Lit value;
        static Result<LitExpr> pull(InputStream& stream);
    };
    struct IdentExpr {
        Ident ident;
        static Result<IdentExpr> pull(InputStream& stream);
    };
    struct UnOpExpr {
        Rc<Expr> expr;
        Op op;
        static Result<UnOpExpr> pull(InputStream& stream);
    };
    struct BinOpExpr {
        Rc<Expr> lhs;
        Rc<Expr> rhs;
        Op op;
        static Result<Expr> pull(InputStream& stream, size_t p, Expr lhs);
        static Result<Expr> pull(InputStream& stream);
    };
    struct CallExpr {
        Rc<Expr> expr;
        std::vector<Expr> args;
        std::unordered_map<std::string, Expr> named;
        static Result<CallExpr> pull(InputStream& stream);
    };
    struct ForInExpr {
        Ident item;
        Rc<Expr> expr;
        Rc<Expr> body;
        static Result<ForInExpr> pull(InputStream& stream);
    };
    struct IfExpr {
        Rc<Expr> cond;
        Rc<Expr> truthy;
        Rc<Expr> falsy;
        static Result<IfExpr> pull(InputStream& stream);
    };
    struct BlockExpr {
        std::vector<Rc<Expr>> exprs;
        static Result<BlockExpr> pull(InputStream& stream);
    };
}
