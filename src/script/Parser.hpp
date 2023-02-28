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
    struct FunExpr;

    template <class T>
    using Rc = std::shared_ptr<T>;

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
    using NullLit = std::monostate; // std::nullptr_t is not <=>!!!
    using BoolLit = bool;
    using StrLit = std::string;
    using IntLit = int64_t;
    using FloatLit = double;
    using ArrLit = std::vector<Rc<Expr>>;
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

        static Result<> pull(Op op, InputStream& stream);
        static Result<> pull(Keyword kw, InputStream& stream);
        static Result<> pull(char c, InputStream& stream);
    };

    bool isIdent(std::string const& ident);
    bool isOp(std::string const& op);
    bool isUnOp(Op op);

    struct Value;
    using Array = std::vector<Value>;
    struct Value {
        std::variant<
            NullLit, BoolLit, IntLit, FloatLit, StrLit,
            Array, Ref<GameObject>, Rc<const FunExpr>
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
        std::unordered_map<std::string, std::pair<Rc<Value>, size_t>> entities;
        size_t scope = 0;
        void add(std::string const& name, Rc<Value> value);
        Rc<Value> get(std::string const& name);
        void push();
        void drop();
    };

    struct LitExpr {
        Lit value;
        static constexpr auto EXPR_NAME = "LitExpr";
        static Result<Rc<LitExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct IdentExpr {
        Ident ident;
        static constexpr auto EXPR_NAME = "IdentExpr";
        static Result<Rc<IdentExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct UnOpExpr {
        Rc<Expr> expr;
        Op op;
        static constexpr auto EXPR_NAME = "UnOpExpr";
        static Result<Rc<UnOpExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct BinOpExpr {
        Rc<Expr> lhs;
        Rc<Expr> rhs;
        Op op;
        static constexpr auto EXPR_NAME = "BinOpExpr";
        static Result<Rc<Expr>> pull(InputStream& stream, size_t p, Rc<Expr> lhs);
        static Result<Rc<Expr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct CallExpr {
        Rc<Expr> expr;
        std::vector<Rc<Expr>> args;
        std::unordered_map<std::string, Rc<Expr>> named;
        static constexpr auto EXPR_NAME = "CallExpr";
        static Result<Rc<CallExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct ForInExpr {
        Ident item;
        Rc<Expr> expr;
        Rc<Expr> body;
        static constexpr auto EXPR_NAME = "ForInExpr";
        static Result<Rc<ForInExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct IfExpr {
        Rc<Expr> cond;
        Rc<Expr> truthy;
        Rc<Expr> falsy;
        static constexpr auto EXPR_NAME = "IfExpr";
        static Result<Rc<IfExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct FunExpr : public std::enable_shared_from_this<FunExpr> {
        Ident ident;
        std::vector<std::pair<Ident, std::optional<Rc<Expr>>>> params;
        Rc<Expr> body;
        static constexpr auto EXPR_NAME = "FunExpr";
        static Result<Rc<FunExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct ListExpr {
        std::vector<Rc<Expr>> exprs;
        static constexpr auto EXPR_NAME = "ListExpr";
        static Result<Rc<Expr>> pullBlock(InputStream& stream);
        static Result<Rc<ListExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };
    struct Expr {
        std::variant<
            Rc<LitExpr>, Rc<IdentExpr>,
            Rc<UnOpExpr>, Rc<BinOpExpr>,
            Rc<CallExpr>, Rc<FunExpr>,
            Rc<ForInExpr>, Rc<IfExpr>,
            Rc<ListExpr>
        > value;

        static Result<Rc<Expr>> pullPrimary(InputStream& stream);
        static Result<Rc<Expr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state) const;
        Result<> preval(State& state) const;
    };

    bool operator==(Array const& a, Array const b);
    bool operator!=(Array const& a, Array const b);
    bool operator<(Array const& a, Array const b);
    bool operator<=(Array const& a, Array const b);
    bool operator>(Array const& a, Array const b);
    bool operator>=(Array const& a, Array const b);
}
