#pragma once

#include <quickjs.h>
#include <string>
#include <functional>
#include <optional>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/general.hpp>

using namespace geode::prelude;

namespace qjs {
    class Runtime;
    class Context;
    class Value;
    class Module;
    struct Promise;

    using CppFunction = Value(Context, Value, std::vector<Value> const&);
    using CppClassFinalizer = void(Runtime, Value);

    class Runtime final {
    public:
        class OpaqueData;
        
    private:
        JSRuntime* m_rt;
        bool m_managed = true;

        explicit Runtime();
        explicit Runtime(JSRuntime* rt);

    public:
        static Runtime null();
        static Runtime create();
        static Runtime weak(JSRuntime* rt);

        Runtime weakCopy() const;

        OpaqueData* getOpaque();
        JSRuntime* getRaw() const;

        Runtime(Runtime const&) = delete;
        Runtime& operator=(Runtime const&) = delete;
        Runtime(Runtime&&);
        Runtime& operator=(Runtime&&);
        ~Runtime();

        Result<JSClassID> createClass(std::string_view name, std::function<CppClassFinalizer> finalizer);
        std::optional<JSClassID> getClassID(std::string_view name) const;
    };

    class Context final {
    private:
        JSContext* m_ctx;

        explicit Context(Runtime const& rt);
        explicit Context(JSContext* ctx);

    public:
        static Context null();
        static Context create(Runtime const& rt);
        static Context from(JSContext* ctx);

        Context(Context const&);
        Context& operator=(Context const&);
        Context(Context&&);
        Context& operator=(Context&&);
        ~Context();

        Runtime getRuntime(std::source_location const src = std::source_location::current()) const;
        JSContext* getRaw() const;
        
        Value getGlobalObject() const;
        Value getException() const;

        template <class T>
        T* setOpaque(T* opaque) {
            auto old = this->getOpaque<T>();
            if (m_ctx) {
                JS_SetContextOpaque(m_ctx, opaque);
            }
            return old;
        }
        template <class T>
        T* getOpaque() const {
            return m_ctx ? static_cast<T*>(JS_GetContextOpaque(m_ctx)) : nullptr;
        }

        void setClassProto(JSClassID id, Value proto);

        template <class... Args>
        Value throwError(fmt::format_string<Args...> fmt, Args&&... args);
        template <class... Args>
        Value throwTypeError(fmt::format_string<Args...> fmt, Args&&... args);

        Value createUndefined();
        Value createNull();
        Value createBool(bool value);
        Value createInt32(int32_t value);
        Value createNumber(double value);
        Value createString(std::string_view value);
        Value createArray();
        Value createObject();
        Value createObject(JSClassID classID);
        Promise createPromise();
        Value createFunctionBare(std::string_view name, std::function<CppFunction> function);

        template <class F>
        Value createFunction(std::string_view name, F&& function);
        
        Result<Module> eval(std::string_view code, std::string_view filename);
    };

    class Value final {
    public:
        class Iterator;

    private:
        std::variant<Context, Runtime> m_ctxOrRt;
        JSValue m_value;

        explicit Value(Context ctx, JSValue value, std::source_location const src);
        explicit Value(Runtime rt, JSValue value, std::source_location const src);

        static std::variant<Context, Runtime> copyCtxOrRt(std::variant<Context, Runtime> const& other);

    public:
        static Value own(Context ctx, JSValue value, std::source_location const src = std::source_location::current());
        static Value copy(Context ctx, JSValue value, std::source_location const src = std::source_location::current());

        static Value own(Runtime rt, JSValue value, std::source_location const src = std::source_location::current());
        static Value copy(Runtime rt, JSValue value, std::source_location const src = std::source_location::current());

        Value(Value const&);
        Value& operator=(Value const&);
        Value(Value&&);
        Value& operator=(Value&&);

        ~Value();

        bool operator==(Value const& other) const;
        bool operator!=(Value const& other) const;

        int getRefCount() const;
        std::string getTypeName() const;

        bool isNull() const;
        bool isNumber() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;
        bool isPromise() const;
        bool isClass(JSClassID id) const;

        Runtime getRuntime() const;
        JSValue takeValue() &&;

        bool isException() const;
        void setProperty(std::string_view prop, Value other, bool readonly = false);
        void setPropertyBare(std::string_view prop, std::function<CppFunction> get, std::function<CppFunction> set);
        void push(Value other);

        std::optional<JSPromiseStateEnum> getPromiseState();
        std::optional<Value> getPromiseResult();

        template <class Get>
        void setProperty(std::string_view prop, Get&& getter);
        template <class Get, class Set>
        void setProperty(std::string_view prop, Get&& getter, Set&& setter);

        Value call(Value self, std::vector<Value> const& args);

        std::optional<int64_t> getLength() const;
        std::optional<Value> getArrayItem(uint32_t prop) const;
        std::vector<Value> getArrayItems() const;
        std::optional<Value> getProperty(std::string_view prop) const;
        std::vector<std::string> getPropertyNames() const;
        std::unordered_map<std::string, Value> getProperties() const;

        // Iterator begin() const;
        // Iterator end() const;

        template <class T>
        bool setOpaque(T* opaque) {
            return JS_SetOpaque(m_value, opaque) == 0;
        }
        template <class T>
        T* getOpaque() const {
            JSClassID id;
            return static_cast<T*>(JS_GetAnyOpaque(m_value, &id));
        }

        bool toBool() const;
        std::optional<int32_t> toInt32() const;
        std::optional<double> toNumber() const;
        std::string toString() const;
    };

    class Module final {
    private:
        Context m_ctx;
        Value m_promise;
        JSModuleDef* m_def;

        Module(Context ctx, JSModuleDef* definition, Value modulePromise);

    public:
        static Module null();
        static Module own(Context ctx, JSModuleDef* definition, Value modulePromise);

        Module(Module const&);
        Module& operator=(Module const&);
        Module(Module&&);
        Module& operator=(Module&&);

        Result<std::optional<Value>> tick();

        ~Module();
    };

    struct Promise final {
        Value value;
        Value resolve;
        Value reject;
    };

    // class Value::Iterator final {
    // private:
    //     Context m_ctx;
    //     Value m_value;
    //     uint32_t m_index = 0;

    // public:
    //     using value_type = Value;

    //     // Value MUST be context or this will throw
    //     Iterator(Value value, size_t index);

    //     Iterator& operator++();
    //     Iterator& operator++(int);

    //     Value operator*();

    //     bool operator==(Iterator const& other) const;
    //     bool operator!=(Iterator const& other) const;
    // };

    // tyvm https://www.reddit.com/r/cpp_questions/comments/pm0f8g/how_do_i_convert_a_vector_of_stdany_to_a_tuple_of/

    namespace detail {
        template<int N, typename... Ts>
        using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

        template <class Arg>
        Result<std::remove_cvref_t<Arg>> parseJsType(Context ctx, Value arg);

        template <class... Args>
        Result<std::tuple<std::remove_cvref_t<Args>...>> parseJsTypes(Context ctx, std::vector<Value> const& args);

        template <class Ty>
        struct JsTypeToCpp;

        template <class Ty>
        concept IsValidJsTypeToCpp = requires {
            { JsTypeToCpp<Ty>::from(std::declval<Context>(), std::declval<Value>()) } -> std::same_as<Result<Ty>>;
            { JsTypeToCpp<Ty>::to(std::declval<Context>(), std::declval<Ty>()) } -> std::same_as<Value>;
        };

        template <>
        struct JsTypeToCpp<Value> {
            static Result<Value> from(Context, Value arg) {
                return Ok(std::move(arg));
            }
            static Value to(Context, Value value) {
                return value;
            }
        };
        static_assert(IsValidJsTypeToCpp<Value>);

        template <>
        struct JsTypeToCpp<bool> {
            static Result<bool> from(Context, Value arg) {
                return Ok(arg.toBool());
            }
            static Value to(Context ctx, bool value) {
                return ctx.createBool(value);
            }
        };
        static_assert(IsValidJsTypeToCpp<bool>);

        template <>
        struct JsTypeToCpp<std::nullptr_t> {
            static Result<std::nullptr_t> from(Context, Value arg) {
                if (arg.isNull()) {
                    return Err("Expected integer, got {}", arg.getTypeName());
                }
                return Ok(nullptr);
            }
            static Value to(Context ctx, std::nullptr_t) {
                return ctx.createNull();
            }
        };
        static_assert(IsValidJsTypeToCpp<std::nullptr_t>);

        template <>
        struct JsTypeToCpp<int32_t> {
            static Result<int32_t> from(Context, Value arg) {
                if (auto value = arg.toInt32(); arg.isNumber() && value) {
                    return Ok(*value);
                }
                else {
                    return Err("Expected integer, got {}", arg.getTypeName());
                }
            }
            static Value to(Context ctx, int32_t value) {
                return ctx.createInt32(value);
            }
        };
        static_assert(IsValidJsTypeToCpp<int32_t>);

        template <>
        struct JsTypeToCpp<float> {
            static Result<float> from(Context, Value arg) {
                if (arg.isNumber()) {
                    return Ok(static_cast<float>(*arg.toNumber()));
                }
                else {
                    return Err("Expected number, got {}", arg.getTypeName());
                }
            }
            static Value to(Context ctx, float value) {
                return ctx.createNumber(value);
            }
        };
        static_assert(IsValidJsTypeToCpp<float>);

        template <>
        struct JsTypeToCpp<double> {
            static Result<double> from(Context, Value arg) {
                if (arg.isNumber()) {
                    return Ok(*arg.toNumber());
                }
                else {
                    return Err("Expected number, got {}", arg.getTypeName());
                }
            }
            static Value to(Context ctx, double value) {
                return ctx.createNumber(value);
            }
        };
        static_assert(IsValidJsTypeToCpp<double>);

        template <>
        struct JsTypeToCpp<std::string> {
            static Result<std::string> from(Context, Value arg) {
                if (arg.isString()) {
                    return Ok(arg.toString());
                }
                else {
                    return Err("Expected string, got {}", arg.getTypeName());
                }
            }
            static Value to(Context ctx, std::string value) {
                return ctx.createString(value);
            }
        };
        static_assert(IsValidJsTypeToCpp<std::string>);

        template <class Ty>
        struct JsTypeToCpp<std::vector<Ty>> {
            static Result<std::vector<Ty>> from(Context ctx, Value arg) {
                if (!arg.isArray()) {
                    return Err("Expected array, got {}", arg.getTypeName());
                }
                std::vector<Ty> result;
                size_t ix = 0;
                for (auto js : arg.getArrayItems()) {
                    if (auto cpp = parseJsType<Ty>(ctx, js)) {
                        result.push_back(*cpp);
                    }
                    else {
                        return Err("{} (in array at index {})", cpp.unwrapErr(), ix);
                    }
                    ix += 1;
                }
                return Ok(result);
            }
            static Value to(Context ctx, std::vector<Ty> value) {
                auto arr = ctx.createArray();
                for (auto item : std::move(value)) {
                    arr.push(JsTypeToCpp<Ty>::to(ctx, std::move(item)));
                }
                return arr;
            }
        };
        static_assert(IsValidJsTypeToCpp<std::vector<Value>>);

        template <class... Tys>
        struct JsTypeToCpp<std::tuple<Tys...>> {
            static Result<std::tuple<Tys...>> from(Context ctx, Value arg) {
                if (!arg.isArray()) {
                    return Err("Expected array, got {}", arg.getTypeName());
                }
                if (arg.getLength() != sizeof...(Tys)) {
                    return Err("Expected array of length {}, got {}", sizeof...(Tys), arg.getLength().value_or(0));
                }
                if (auto ret = parseJsTypes<Tys...>(ctx, arg.getArrayItems())) {
                    return Ok(*ret);
                }
                else {
                    return Err("{} (in tuple of size {})", ret.unwrapErr(), sizeof...(Tys));
                }
            }
            template <size_t... Indices>
            static Value toImpl(Context ctx, std::tuple<Tys...> value, std::index_sequence<Indices...>) {
                auto arr = ctx.createArray();
                (arr.push(JsTypeToCpp<NthTypeOf<Indices, Tys...>>::to(ctx, std::move(std::get<Indices>(value)))), ...);
                return arr;
            }
            static Value to(Context ctx, std::tuple<Tys...> value) {
                return toImpl(std::move(ctx), std::move(value), std::make_index_sequence<sizeof...(Tys)>());
            }
        };
        static_assert(IsValidJsTypeToCpp<std::tuple<Value, Value, Value>>);

        template <class Ty>
        struct JsTypeToCpp<std::unordered_map<std::string, Ty>> {
            static Result<std::unordered_map<std::string, Ty>> from(Context ctx, Value arg) {
                if (!arg.isObject()) {
                    return Err("Expected object, got {}", arg.getTypeName());
                }
                std::unordered_map<std::string, Ty> result;
                for (auto [name, js] : arg.getProperties()) {
                    if (auto cpp = parseJsType<Ty>(ctx, js)) {
                        result.insert({ name, *cpp });
                    }
                    else {
                        return Err("{} (in object key \"{}\")", cpp.unwrapErr(), name);
                    }
                }
                return Ok(result);
            }
            static Value to(Context ctx, std::unordered_map<std::string, Ty> value) {
                auto obj = ctx.createObject();
                for (auto [name, item] : std::move(value)) {
                    obj.setProperty(name, JsTypeToCpp<Ty>::to(std::move(item)));
                }
                return obj;
            }
        };
        static_assert(IsValidJsTypeToCpp<std::unordered_map<std::string, Value>>);

        template <class Arg>
        Result<std::remove_cvref_t<Arg>> parseJsType(Context ctx, Value arg) {
            return JsTypeToCpp<std::remove_cvref_t<Arg>>::from(std::move(ctx), std::move(arg));
        }

        template <class Arg, size_t Index, class... Args>
        void parseJsTypeHelper(
            Context ctx,
            Value arg,
            std::tuple<Args...>& result,
            std::optional<std::string>& error
        ) {
            if (error) return;
            if (auto res = parseJsType<Arg>(std::move(ctx), std::move(arg))) {
                std::get<Index>(result) = res.unwrap();
            }
            else {
                error = fmt::format("{} (at index {})", res.unwrapErr(), Index);
            }
        }

        template <class... Args, size_t... Indices>
        void parseJsTypesHelper(
            Context ctx,
            std::vector<Value> const& args,
            std::tuple<Args...>& result,
            std::optional<std::string>& error,
            std::index_sequence<Indices...>
        ) {
            (parseJsTypeHelper<NthTypeOf<Indices, Args...>, Indices, Args...>(
                std::move(ctx), args[Indices], result, error
            ), ...);
        }

        template <class... Args>
        Result<std::tuple<std::remove_cvref_t<Args>...>> parseJsTypes(Context ctx, std::vector<Value> const& args) {
            constexpr size_t EXPECTED_ARG_COUNT = sizeof...(Args);
            if (args.size() != EXPECTED_ARG_COUNT) {
                return Err("Expected {} arguments, got {}", EXPECTED_ARG_COUNT, args.size());
            }
            std::tuple<std::remove_cvref_t<Args>...> result = {};
            std::optional<std::string> error = std::nullopt;
            parseJsTypesHelper<std::remove_cvref_t<Args>...>(std::move(ctx), args, result, error, std::make_index_sequence<EXPECTED_ARG_COUNT>());
            if (error) {
                return Err(*error);
            }
            return Ok(result);
        }

        template <class... Args, size_t... Indices>
        auto apply(auto&& func, auto&& first, auto&& second, std::tuple<Args...>&& args, std::index_sequence<Indices...>) {
            return func(first, second, std::get<Indices>(args)...); 
        }
        template <class A, class B, class... Args>
        auto apply(auto&& func, A&& first, B&& second, std::tuple<Args...>&& args) {
            return apply(
                func,
                std::forward<A>(first),
                std::forward<B>(second),
                std::forward<std::tuple<Args...>>(args),
                std::make_index_sequence<sizeof...(Args)>()
            );
        }

        template <class This, class Ret, class... Args>
        Value parseJsTypesAndCall(
            std::string_view name,
            auto&& function,
            Context ctx,
            Value thisValue,
            std::vector<Value> const& args
        ) {
            auto parsedThis = parseJsType<This>(ctx, thisValue);
            if (!parsedThis) {
                return ctx.throwTypeError("{} ('this' in {})", parsedThis.unwrapErr(), name);
            }
            auto parsedArgs = parseJsTypes<Args...>(ctx, args);
            if (!parsedArgs) {
                return ctx.throwTypeError("{} (arguments in {})", parsedArgs.unwrapErr(), name);
            }
            return JsTypeToCpp<std::remove_cvref_t<Ret>>::to(
                ctx, apply(function, ctx, std::move(*parsedThis), std::move(*parsedArgs))
            );
        }

        template <class F>
        struct ImplExtract;

        template <class T, class R, class... A>
        struct ImplExtract<R(Context, T, A...)> {
            static Value apply(
                std::string_view name,
                auto&& function,
                Context ctx, Value thisValue, std::vector<Value> const& args
            ) {
                return parseJsTypesAndCall<T, R, A...>(name, std::move(function), ctx, thisValue, args);
            }
        };
        template <class T, class R, class... A>
        struct ImplExtract<R(*)(Context, T, A...)> {
            static Value apply(
                std::string_view name,
                auto&& function,
                Context ctx, Value thisValue, std::vector<Value> const& args
            ) {
                return parseJsTypesAndCall<T, R, A...>(name, std::move(function), ctx, thisValue, args);
            }
        };
        template <class C, class T, class R, class... A>
        struct ImplExtract<R(C::*)(Context, T, A...)> {
            static Value apply(
                std::string_view name,
                auto&& function,
                Context ctx, Value thisValue, std::vector<Value> const& args
            ) {
                return parseJsTypesAndCall<T, R, A...>(name, std::move(function), ctx, thisValue, args);
            }
        };
        template <class C, class T, class R, class... A>
        struct ImplExtract<R(C::*)(Context, T, A...) const> {
            static Value apply(
                std::string_view name,
                auto&& function,
                Context ctx, Value thisValue, std::vector<Value> const& args
            ) {
                return parseJsTypesAndCall<T, R, A...>(name, std::move(function), ctx, thisValue, args);
            }
        };
        template <class F>
            requires requires { &F::operator(); }
        struct ImplExtract<F> : public ImplExtract<decltype(&F::operator())> {};

        template <class F>
        using Extract = ImplExtract<std::remove_cvref_t<F>>;

        template <class F>
        std::function<CppFunction> wrapFunction(std::string_view name, F&& function) {
            return [name, function = std::move(function)](
                Context ctx, Value thisValue, std::vector<Value> const& args
            ) {
                return detail::Extract<F>::apply(name, function, ctx, thisValue, args);
            };
        }
    }

    template <class... Args>
    Value Context::throwError(fmt::format_string<Args...> fmt, Args&&... args) {
        return Value::own(*this, JS_ThrowPlainError(m_ctx, "%s", fmt::format(fmt, std::forward<Args>(args)...).c_str()));
    }
    template <class... Args>
    Value Context::throwTypeError(fmt::format_string<Args...> fmt, Args&&... args) {
        auto msg = fmt::format(fmt, std::forward<Args>(args)...);
        log::info("throwing type error: {}", msg);
        return Value::own(*this, JS_ThrowTypeError(m_ctx, "%s", msg.c_str()));
    }
    
    template <class F>
    Value Context::createFunction(std::string_view name, F&& function) {
        return this->createFunctionBare("", detail::wrapFunction<F>(name, std::move(function)));
    }

    template <class Get>
    void Value::setProperty(std::string_view prop, Get&& getter) {
        return this->setPropertyBare(
            prop,
            detail::wrapFunction<Get>(fmt::format("{}.get", prop), std::move(getter)),
            nullptr
        );
    }
    template <class Get, class Set>
    void Value::setProperty(std::string_view prop, Get&& getter, Set&& setter) {
        return this->setPropertyBare(
            prop,
            detail::wrapFunction<Get>(fmt::format("{}.get", prop), std::move(getter)),
            detail::wrapFunction<Set>(fmt::format("{}.set", prop), std::move(setter))
        );
    }
}
