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
    void resetExecutionCounter();
    void tickExecutionCounter();
    std::string prettify(std::string const& debugMsg);

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
        std::streampos m_pos;

    public:
        Rollback(InputStream& stream) : m_stream(stream), m_commit(false), m_rolled(false) {
            m_pos = stream.tellg();
        }
        ~Rollback() {
            if (!m_commit) {
                this->ret();
            }
        }
        std::string commit() {
            m_commit = true;
            auto to = m_stream.tellg();
            m_stream.seekg(m_pos);
            std::string buf;
            while (m_stream.tellg() != to && !m_stream.eof()) {
                buf += m_stream.get();
            }
            m_stream.seekg(to);
            return buf;
        }
        void ret() {
            if (!m_rolled) {
                m_stream.seekg(m_pos);
                m_stream.clear();
                m_rolled = true;
            }
        }
        void retry() {
            this->ret();
            m_rolled = false;
            m_commit = false;
        }
    };

    enum class Keyword {
        For, In, While,
        If, Else,
        Function, Return, Break, Continue,
        New,
        True, False, Null,
    };
    enum class Op {
        Seq,    // a = b
        AddSeq, // a += b
        SubSeq, // a -= b
        MulSeq, // a *= b
        DivSeq, // a /= b
        ModSeq, // a %= b
        Add,    // a + b or +a
        Sub,    // a - b or -a
        Mul,    // a * b
        Div,    // a / b
        Mod,    // a % b
        Eq,     // a == b
        Neq,    // a != b
        Less,   // a < b
        Leq,    // a <= b
        More,   // a > b
        Meq,    // a >= b
        Not,    // !a
        And,    // a && b
        Or,     // a || b
    };
    using NullLit = std::monostate; // std::nullptr_t is not <=>!!!
    using BoolLit = bool;
    using StrLit = std::string;
    using NumLit = double;
    using ArrLit = std::vector<Rc<Expr>>;
    using Lit = std::variant<NullLit, BoolLit, StrLit, NumLit, ArrLit>;
    using Ident = std::string;
    using Punct = char;

    enum class OpDir : bool {
        LTR, RTL,
    };

    template <class T>
    struct TokenTraits;

    template <>
    struct TokenTraits<Keyword> {
        static inline const char* TYPE_NAME = "keyword";
    };

    template <>
    struct TokenTraits<Op> {
        static inline const char* TYPE_NAME = "operator";
    };

    template <>
    struct TokenTraits<Lit> {
        static inline const char* TYPE_NAME = "literal";
    };

    template <>
    struct TokenTraits<Ident> {
        static inline const char* TYPE_NAME = "identifier";
    };

    template <>
    struct TokenTraits<Punct> {
        static inline const char* TYPE_NAME = "punctuation";
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
        static OpDir dir(InputStream& stream);
        static bool eof(InputStream& stream);

        template <TokenType T>
        static std::optional<T> peek(InputStream& stream) {
            auto value = Token::peek(stream);
            if (value) {
                if (auto v = std::get_if<T>(&value.value().value)) {
                    return *v;
                }
            }
            return std::nullopt;
        }

        template <class T>
        static bool peek(T c, InputStream& stream) {
            auto value = Token::peek(stream);
            if (value) {
                if (auto pun = std::get_if<T>(&value.value().value)) {
                    return *pun == c;
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

    bool isIdentCh(char ch);
    bool isIdent(std::string const& ident);
    bool isOpCh(char ch);
    bool isOp(std::string const& op);
    bool isUnOp(Op op);

    struct Value;
    using Array = std::vector<Value>;
    struct Value {
        std::variant<
            NullLit, BoolLit, NumLit, StrLit,
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
        std::optional<Rc<Value>> member(std::string const& name);
    };

    // mfw when exceptions for control flow breaks
    struct ReturnSignal : public std::exception {
        Rc<Value> value;
        inline ReturnSignal(Rc<Value> value) : value(value) {}
        inline const char* what() const noexcept override {
            return "Internal error - Return signal not caught";
        }
    };
    struct BreakSignal : public ReturnSignal {
        inline BreakSignal(Rc<Value> value) : ReturnSignal(value) {}
        inline const char* what() const noexcept override {
            return "Internal error - Break signal not caught";
        }
    };
    struct ContinueSignal : public std::exception {
        inline ContinueSignal() {}
        inline const char* what() const noexcept override {
            return "Internal error - Continue signal not caught";
        }
    };

    struct State;
    struct Scope;

    struct LitExpr {
        Lit value;
        std::string src;
        static Result<Rc<LitExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct IdentExpr {
        Ident ident;
        std::string src;
        static Result<Rc<IdentExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct UnOpExpr {
        Rc<Expr> expr;
        Op op;
        std::string src;
        static Result<Rc<UnOpExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct BinOpExpr {
        Rc<Expr> lhs;
        Rc<Expr> rhs;
        Op op;
        std::string src;
        static Result<Rc<Expr>> pull(InputStream& stream, size_t p, Rc<Expr> lhs);
        static Result<Rc<Expr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct CallExpr {
        Rc<Expr> expr;
        std::vector<Rc<Expr>> args;
        std::unordered_map<std::string, Rc<Expr>> named;
        std::string src;
        static Result<Rc<CallExpr>> pull(Rc<Expr> before, InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct IndexExpr {
        Rc<Expr> expr;
        Rc<Expr> index;
        std::string src;
        static Result<Rc<IndexExpr>> pull(Rc<Expr> before, InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct MemberExpr {
        Rc<Expr> expr;
        Ident member;
        std::string src;
        static Result<Rc<MemberExpr>> pull(Rc<Expr> before, InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct ForInExpr {
        Ident item;
        Rc<Expr> expr;
        Rc<Expr> body;
        std::string src;
        static Result<Rc<ForInExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct IfExpr {
        Rc<Expr> cond;
        Rc<Expr> truthy;
        std::optional<Rc<Expr>> falsy;
        std::string src;
        static Result<Rc<IfExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct FunExpr : public std::enable_shared_from_this<FunExpr> {
        Ident ident;
        std::vector<std::pair<Ident, std::optional<Rc<Expr>>>> params;
        Rc<Expr> body;
        std::string src;
        bool added = false;
        FunExpr(Ident const&, decltype(params) const&, Rc<Expr> const&, std::string const&);
        static Result<Rc<FunExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        Result<> add(State& state);
        std::string debug() const;
    };
    struct ReturnExpr {
        std::optional<Rc<Expr>> value;
        std::string src;
        static Result<Rc<ReturnExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct BreakExpr {
        std::optional<Rc<Expr>> value;
        std::string src;
        static Result<Rc<BreakExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct ContinueExpr {
        std::string src;
        static Result<Rc<ContinueExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct ListExpr {
        std::vector<Rc<Expr>> exprs;
        std::string src;
        static Result<Rc<Expr>> pullBlock(InputStream& stream);
        static Result<Rc<ListExpr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
    };
    struct CppExpr {
        std::function<Result<Rc<Value>>(State&)> eval;
        std::string src = "/* built-in expr */";
        std::string debug() const;
    };
    struct Expr {
        std::variant<
            Rc<LitExpr>, Rc<IdentExpr>,
            Rc<UnOpExpr>, Rc<BinOpExpr>,
            Rc<CallExpr>, Rc<FunExpr>, Rc<IndexExpr>, Rc<MemberExpr>,
            Rc<ForInExpr>, Rc<IfExpr>,
            Rc<ReturnExpr>, Rc<BreakExpr>, Rc<ContinueExpr>,
            Rc<ListExpr>, Rc<CppExpr>
        > value;

        static Result<Rc<Expr>> pullPrimaryNonCall(InputStream& stream);
        static Result<Rc<Expr>> pullPrimary(InputStream& stream);
        static Result<Rc<Expr>> pull(InputStream& stream);
        Result<Rc<Value>> eval(State& state);
        std::string debug() const;
        std::string src() const;
    };

    bool operator==(Array const& a, Array const b);
    bool operator!=(Array const& a, Array const b);
    bool operator<(Array const& a, Array const b);
    bool operator<=(Array const& a, Array const b);
    bool operator>(Array const& a, Array const b);
    bool operator>=(Array const& a, Array const b);

    struct Scope final {
        State& state;
        Scope(State& state);
        ~Scope();
    };
    class State final {
    private:
        std::vector<std::unordered_map<std::string, Rc<Value>>> entities = {{}};
        
        friend struct Scope;

        void push();
        void pop();

        State();

    public:
        static Result<> run(ghc::filesystem::path const& path, bool debug = false);
        static Result<> run(std::string const& code, bool debug = false);
        static Result<> run(InputStream& stream, bool debug = false);

        void add(std::string const& name, Rc<Value> value);
        bool has(std::string const& name) const;
        Rc<Value> get(std::string const& name);
        Scope scope();
    };
    
    template <class T, class... Args>
    Result<Rc<T>> make(Args&&... args) {
        return Ok(std::make_shared<T>(std::forward<Args>(args)...));
    }

    template <class T>
    Result<Rc<T>> make(T value) {
        return Ok(std::make_shared<T>(value));
    }
}
