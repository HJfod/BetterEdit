#pragma once

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <Geode/utils/Result.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>
#include <fmt/format.h>

USE_GEODE_NAMESPACE();

namespace script {
    using InputStream = std::istream;

    struct Expr;

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
                this->ret();
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

    enum class Keyword {
        For, In, While,
        If, Else,
        Function, Return,
        New,
        True, False, Null,
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
    using NullLit = std::nullptr_t;
    using BoolLit = bool;
    using StrLit = std::string;
    using IntLit = int64_t;
    using FloatLit = double;
    using ArrLit = std::vector<Expr>;
    using Lit = std::variant<NullLit, BoolLit, StrLit, IntLit, FloatLit, ArrLit>;
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

    std::string tokenToString(Keyword kw, bool debug = false);
    std::string tokenToString(Ident ident, bool debug = false);
    std::string tokenToString(Lit lit, bool debug = false);
    std::string tokenToString(Op op, bool debug = false);
    std::string tokenToString(Punct punct, bool debug = false);

    struct Token {
        std::variant<Keyword, Op, Lit, Punct, Ident> value;

        std::string toString(bool debug = false) const;
        static void skip(InputStream& stream);
        static Result<Token> pull(InputStream& stream);
        static std::optional<Token> peek(InputStream& stream);
        static size_t prio(InputStream& stream);
        static bool eof(InputStream& stream);

        template <TokenType T>
        static bool peek(InputStream& stream) {
            auto value = Token::peek(stream);
            return value && std::holds_alternative<T>(value.value().value);
        }

        template <char C>
        static bool peek(InputStream& stream) {
            auto value = Token::peek(stream);
            if (value) {
                if (auto pun = std::get_if<Punct>(&value.value().value)) {
                    return *pun == C;
                }
            }
            return false;
        }

        template <TokenType T>
        static Result<T> pull(InputStream& stream) {
            Rollback rb(stream);
            GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
            if (auto value = std::get_if<T>(&token.value)) {
                rb.commit();
                return Ok(*value);
            }
            return Err(fmt::format(
                "Expected {}, got '{}'",
                TokenTraits<T>::TYPE_NAME, token.toString()
            ));
        }

        template <Op O>
        static Result<> pull(InputStream& stream) {
            Rollback rb(stream);
            GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
            if (auto value = std::get_if<Op>(&token.value)) {
                if (*value == O) {
                    rb.commit();
                    return Ok();
                }
                else {
                    return Err(fmt::format(
                        "Expected {}, got {}",
                        tokenToString(O), tokenToString(*value)
                    ));
                }
            }
            return Err(fmt::format(
                "Expected {}, got '{}'",
                tokenToString(O), token.toString()
            ));
        }

        template <Keyword Kw>
        static Result<> pull(InputStream& stream) {
            Rollback rb(stream);
            GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
            if (auto value = std::get_if<Keyword>(&token.value)) {
                if (*value == Kw) {
                    rb.commit();
                    return Ok();
                }
                else {
                    return Err(fmt::format(
                        "Expected {}, got {}",
                        tokenToString(Kw), tokenToString(*value)
                    ));
                }
            }
            return Err(fmt::format(
                "Expected {}, got '{}'",
                tokenToString(Kw), token.toString()
            ));
        }

        template <char C>
        static Result<> pull(InputStream& stream) {
            Rollback rb(stream);
            GEODE_UNWRAP_INTO(auto token, Token::pull(stream));
            if (auto value = std::get_if<Punct>(&token.value)) {
                if (*value == C) {
                    rb.commit();
                    return Ok();
                }
            }
            return Err(fmt::format("Expected '{}', got '{}'", C, token.toString()));
        }
    };

    bool isIdent(std::string const& ident);
    bool isOp(std::string const& op);
    bool isUnOp(Op op);

    template <class T>
    using Rc = std::shared_ptr<T>;

    struct Value;
    using Array = std::vector<Value>;
    struct Value {
        std::variant<
            NullLit, BoolLit, IntLit, FloatLit, StrLit,
            Array, Ref<GameObject>
        > value;

        Value(decltype(Value::value) const& value) : value(value) {}

        static Rc<Value> rc(decltype(Value::value) const& value) {
            return std::make_shared<Value>(value);
        }

        template <class T>
        T* has() {
            return std::get_if<T>(&value);
        }

        bool isNull() const;
        bool truthy() const;
        bool typeEq(Rc<Value> other) const;
        std::string typeName() const;
        std::string toString(bool debug = false) const;
    };

    struct State {
        std::unordered_map<std::string, Rc<Value>> variables;
    };

    struct LitExpr {
        Lit value;
        static constexpr auto EXPR_NAME = "LitExpr";
        static Result<LitExpr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct IdentExpr {
        Ident ident;
        static constexpr auto EXPR_NAME = "IdentExpr";
        static Result<IdentExpr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct UnOpExpr {
        Rc<Expr> expr;
        Op op;
        static constexpr auto EXPR_NAME = "UnOpExpr";
        static Result<UnOpExpr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct BinOpExpr {
        Rc<Expr> lhs;
        Rc<Expr> rhs;
        Op op;
        static constexpr auto EXPR_NAME = "BinOpExpr";
        static Result<Expr> pull(InputStream& stream, size_t p, Expr lhs);
        static Result<Expr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct CallExpr {
        Rc<Expr> expr;
        std::vector<Expr> args;
        std::unordered_map<std::string, Expr> named;
        static constexpr auto EXPR_NAME = "CallExpr";
        static Result<CallExpr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct ForInExpr {
        Ident item;
        Rc<Expr> expr;
        Rc<Expr> body;
        static constexpr auto EXPR_NAME = "ForInExpr";
        static Result<ForInExpr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct IfExpr {
        Rc<Expr> cond;
        Rc<Expr> truthy;
        Rc<Expr> falsy;
        static constexpr auto EXPR_NAME = "IfExpr";
        static Result<IfExpr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct ListExpr {
        std::vector<Expr> exprs;
        static constexpr auto EXPR_NAME = "ListExpr";
        static Result<ListExpr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
    struct Expr {
        std::variant<
            LitExpr, IdentExpr,
            UnOpExpr, BinOpExpr,
            CallExpr,
            ForInExpr, IfExpr,
            ListExpr
        > value;

        Rc<Expr> rc() const;
        static Result<Expr> pullPrimary(InputStream& stream);
        static Result<Expr> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
    };
}
