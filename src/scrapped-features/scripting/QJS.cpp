#include "QJS.hpp"

using namespace qjs;

class Runtime::OpaqueData final {
private:
    std::unordered_map<std::string, JSClassID> m_classes;
    std::unordered_map<int, std::function<CppFunction>> m_functions;
    std::unordered_map<JSClassID, std::function<CppClassFinalizer>> m_classFinalizers;

public:
    static OpaqueData* get(JSRuntime* rt) {
        return static_cast<OpaqueData*>(JS_GetRuntimeOpaque(rt));
    }
    static OpaqueData* get(JSContext* ctx) {
        return OpaqueData::get(JS_GetRuntime(ctx));
    }

    int addFunction(std::function<CppFunction> function) {
        static int ID_COUNTER = 0;
        m_functions.emplace(++ID_COUNTER, std::move(function));
        return ID_COUNTER;
    }
    Value callFunction(int id, Context& ctx, Value thisValue, std::vector<Value> const& args) {
        return m_functions.at(id)(ctx, thisValue, args);
    }

    void addClass(std::string_view name, JSClassID id) {
        m_classes.insert({ std::string(name), id });
    }
    std::optional<JSClassID> getClass(std::string_view name) {
        auto owned = std::string(name);
        return m_classes.contains(owned) ? std::optional(m_classes.at(owned)) : std::nullopt;
    }

    JSClassID addClassFinalizer(JSClassID id, std::function<CppClassFinalizer> function) {
        m_classFinalizers.emplace(id, std::move(function));
        return id;
    }
    void callClassFinalizer(JSClassID id, Runtime rt, Value instance) {
        return m_classFinalizers.at(id)(std::move(rt), instance);
    }
};

/// Runtime

Runtime::Runtime() : m_rt(JS_NewRuntime()), m_managed(true) {
    auto opaque = new OpaqueData();
    JS_SetRuntimeOpaque(m_rt, opaque);
    JS_AddRuntimeFinalizer(m_rt, +[](JSRuntime*, void* opaque) {
        delete static_cast<OpaqueData*>(opaque);
    }, opaque);
}
Runtime::Runtime(JSRuntime* rt) : m_rt(rt), m_managed(false) {}

Runtime::Runtime(Runtime&& other) : m_rt(other.m_rt), m_managed(other.m_managed) {
    other.m_rt = nullptr;
}
Runtime& Runtime::operator=(Runtime&& other) {
    m_rt = other.m_rt;
    m_managed = other.m_managed;
    other.m_rt = nullptr;
    return *this;
}

Runtime::~Runtime() {
    if (m_managed && m_rt) {
        // log::info("freeing runtime");
        JS_FreeRuntime(m_rt);
    }
}

Runtime Runtime::weakCopy() const {
    return Runtime::weak(m_rt);
}

Runtime::OpaqueData* Runtime::getOpaque() {
    return m_rt ? OpaqueData::get(m_rt) : nullptr;
}
JSRuntime* Runtime::getRaw() const {
    return m_rt;
}

Runtime Runtime::null() {
    return Runtime(nullptr);
}
Runtime Runtime::create() {
    return Runtime();
}
Runtime Runtime::weak(JSRuntime* rt) {
    return Runtime(rt);
}

Result<JSClassID> Runtime::createClass(std::string_view name, std::function<CppClassFinalizer> finalizer) {
    auto def = JSClassDef {
        .class_name = name.data(),
        .finalizer = +[](JSRuntime* rt, JSValue value) {
            // log::info("class finalizer");
            OpaqueData::get(rt)->callClassFinalizer(
                JS_GetClassID(value),
                Runtime::weak(rt),
                Value::copy(Runtime::weak(rt), value)
            );
        },
        .gc_mark = nullptr, // idk what this is
        .call = nullptr,
        .exotic = nullptr,
    };
    JSClassID id = 0;
    JS_NewClassID(m_rt, &id);
    if (JS_NewClass(m_rt, id, &def) < 0) {
        return Err("Unable to create class");
    }
    OpaqueData::get(m_rt)->addClass(name, id);
    OpaqueData::get(m_rt)->addClassFinalizer(id, std::move(finalizer));
    return Ok(id);
}
std::optional<JSClassID> Runtime::getClassID(std::string_view name) const {
    if (m_rt) {
        return OpaqueData::get(m_rt)->getClass(name);
    }
    return std::nullopt;
}

/// Context

Context::Context(Runtime const& rt) : m_ctx(JS_NewContext(rt.getRaw())) {
    // log::info("created context {}", fmt::ptr(m_ctx));
    // JS_AddIntrinsicBaseObjects(m_value);
    // JS_AddIntrinsicTypedArrays(m_value);
    // JS_AddIntrinsicRegExp(m_value);
    // JS_AddIntrinsicJSON(m_value);
    // JS_AddIntrinsicEval(m_value);
}
Context::Context(JSContext* ctx) : m_ctx(ctx) {
    if (m_ctx) {
        // log::info("duping context {}", fmt::ptr(m_ctx));
        JS_DupContext(ctx);
    }
}

Context::Context(Context const& other) : m_ctx(other.m_ctx) {
    if (m_ctx) {
        // log::info("duping context {}", fmt::ptr(m_ctx));
        JS_DupContext(m_ctx);
    }
}
Context& Context::operator=(Context const& other) {
    m_ctx = other.m_ctx;
    if (m_ctx) {
        // log::info("duping context {}", fmt::ptr(m_ctx));
        JS_DupContext(m_ctx);
    }
    return *this;
}

Context::Context(Context&& other) : m_ctx(other.m_ctx) {
    other.m_ctx = nullptr;
}
Context& Context::operator=(Context&& other) {
    m_ctx = other.m_ctx;
    other.m_ctx = nullptr;
    return *this;
}

Context::~Context() {
    if (m_ctx) {
        // log::info("freeing context {}", fmt::ptr(m_ctx));
        JS_FreeContext(m_ctx);
    }
}

Context Context::null() {
    return Context(nullptr);
}
Context Context::create(Runtime const& rt) {
    return Context(rt);
}
Context Context::from(JSContext* ctx) {
    return Context(ctx);
}

Runtime Context::getRuntime(std::source_location const src) const {
    if (!m_ctx) {
        (void)src;
        // log::info("got null runtime from {}:{}", src.function_name(), src.line());
        return Runtime::weak(nullptr);
    }
    return Runtime::weak(JS_GetRuntime(m_ctx));
}
JSContext* Context::getRaw() const {
    return m_ctx;
}

Value Context::getGlobalObject() const {
    return Value::own(*this, JS_GetGlobalObject(m_ctx));
}
Value Context::getException() const {
    return Value::own(*this, JS_GetException(m_ctx));
}

void Context::setClassProto(JSClassID id, Value proto) {
    JS_SetClassProto(m_ctx, id, std::move(proto).takeValue());
}

Value Context::createUndefined() {
    return Value::own(*this, JS_UNDEFINED);
}
Value Context::createNull() {
    return Value::own(*this, JS_NULL);
}
Value Context::createBool(bool value) {
    return Value::own(*this, JS_NewBool(m_ctx, value));
}
Value Context::createInt32(int32_t value) {
    return Value::own(*this, JS_NewInt32(m_ctx, value));
}
Value Context::createNumber(double value) {
    return Value::own(*this, JS_NewFloat64(m_ctx, value));
}
Value Context::createString(std::string_view value) {
    return Value::own(*this, JS_NewString(m_ctx, value.data()));
}
Value Context::createArray() {
    return Value::own(*this, JS_NewArray(m_ctx));
}
Value Context::createObject() {
    return Value::own(*this, JS_NewObject(m_ctx));
}
Value Context::createObject(JSClassID classID) {
    return Value::own(*this, JS_NewObjectClass(m_ctx, classID));
}
Promise Context::createPromise() {
    std::array<JSValue, 2> resolvers = { JS_UNDEFINED, JS_UNDEFINED };
    auto res = JS_NewPromiseCapability(m_ctx, resolvers.data());
    return Promise {
        .value = Value::own(*this, res),
        .resolve = Value::copy(*this, resolvers[0]),
        .reject = Value::copy(*this, resolvers[1]),
    };
}
Value Context::createFunctionBare(std::string_view name, std::function<CppFunction> function) {
    return Value::own(*this, JS_NewCFunctionMagic(
        m_ctx,
        +[](JSContext* ctx, JSValueConst thisVal, int argc, JSValue* argv, int magic) {
            auto wctx = Context::from(ctx);
            std::vector<Value> args;
            for (int i = 0; i < argc; i += 1) {
                args.push_back(Value::copy(wctx, argv[i]));
            }
            return Runtime::OpaqueData::get(ctx)->callFunction(magic, wctx, Value::copy(wctx, thisVal), args).takeValue();
        },
        name.data(), 1, JS_CFUNC_generic_magic,
        Runtime::OpaqueData::get(m_ctx)->addFunction(std::move(function))
    ));
}

Result<Module> Context::eval(std::string_view code, std::string_view filename) {
    auto mod = Value::own(*this, JS_Eval(
        m_ctx, code.data(), code.size(), filename.data(),
        JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_STRICT | JS_EVAL_FLAG_COMPILE_ONLY
    ));
    log::info("module returned: {}", mod.getTypeName());
    if (mod.isException()) {
        return Err(this->getException().toString());
    }
    auto modValue = std::move(mod).takeValue();
    auto def = static_cast<JSModuleDef*>(JS_VALUE_GET_PTR(modValue));
    auto eval = Value::own(*this, JS_EvalFunction(m_ctx, modValue));
    log::info("eval returned: {}", eval.getTypeName());
    if (eval.isException()) {
        return Err(this->getException().toString());
    }
    return Ok(Module::own(*this, def, std::move(eval)));
}

std::variant<Context, Runtime> Value::copyCtxOrRt(std::variant<Context, Runtime> const& other) {
    if (other.index() == 0) {
        return std::get<0>(other);
    }
    else {
        return std::get<1>(other).weakCopy();
    }
}

#define DEBUG_LOG(what) \
    // log::info(what " {} ({}, {})", fmt::ptr(this), this->getTypeName(), this->getRefCount())

#define DEBUG_LOG_AT(what) \
    (void) src
    // log::info(what " {} ({}, {}) from {}:{} ({})", fmt::ptr(this), this->getTypeName(), this->getRefCount(), src.file_name(), src.line(), src.function_name())

Value::Value(Context ctx, JSValue value, std::source_location const src) : m_ctxOrRt(ctx), m_value(value) {
    DEBUG_LOG_AT("created (ctx)");
}
Value::Value(Runtime rt, JSValue value, std::source_location const src) : m_ctxOrRt(rt.weakCopy()), m_value(value) {
    DEBUG_LOG_AT("created (rt)");
}

Value::Value(Value const& other) : m_ctxOrRt(Value::copyCtxOrRt(other.m_ctxOrRt)), m_value(other.m_value) {
    if (auto raw = this->getRuntime().getRaw()) {
        DEBUG_LOG("created (dupe)");
        JS_DupValueRT(raw, m_value);
    }
    else {
        DEBUG_LOG("created (failed to dupe)");
    }
}
Value& Value::operator=(Value const& other) {
    m_ctxOrRt = Value::copyCtxOrRt(other.m_ctxOrRt);
    m_value = other.m_value;
    if (auto raw = this->getRuntime().getRaw()) {
        DEBUG_LOG("duping");
        JS_DupValueRT(raw, m_value);
    }
    else {
        DEBUG_LOG("unable to dupe");
    }
    return *this;
}

Value::Value(Value&& other) : m_ctxOrRt(std::move(other.m_ctxOrRt)), m_value(std::move(other.m_value)) {
    other.m_value = JS_NULL;
    DEBUG_LOG("created (move)");
}
Value& Value::operator=(Value&& other) {
    m_ctxOrRt = std::move(other.m_ctxOrRt);
    m_value = std::move(other.m_value);
    other.m_value = JS_NULL;
    DEBUG_LOG("moved");
    return *this;
}

Value::~Value() {
    if (auto raw = this->getRuntime().getRaw()) {
        DEBUG_LOG("freeing");
        JS_FreeValueRT(raw, m_value);
    }
    else {
        DEBUG_LOG("unable to free");
    }
}

bool Value::operator==(Value const& other) const {
    if (m_ctxOrRt.index() == 0) {
        auto ctx = std::get<0>(m_ctxOrRt);
        return JS_IsStrictEqual(ctx.getRaw(), m_value, other.m_value);
    }
    return false;
}
bool Value::operator!=(Value const& other) const {
    return !(*this == other);
}

Value Value::own(Context ctx, JSValue value, std::source_location const src) {
    return Value(ctx, value, src);
}
Value Value::copy(Context ctx, JSValue value, std::source_location const src) {
    if (auto raw = ctx.getRaw()) {
        // log::info("duping before next create");
        JS_DupValue(raw, value);
    }
    return Value(ctx, value, src);
}

Value Value::own(Runtime rt, JSValue value, std::source_location const src) {
    return Value(std::move(rt), value, src);
}
Value Value::copy(Runtime rt, JSValue value, std::source_location const src) {
    if (auto raw = rt.getRaw()) {
        // log::info("duping before next create");
        JS_DupValueRT(raw, value);
    }
    return Value(std::move(rt), value, src);
}

int Value::getRefCount() const {
    struct X { int value; };
    if (JS_VALUE_HAS_REF_COUNT(m_value)) {
        return static_cast<X*>(JS_VALUE_GET_PTR(m_value))->value;
    }
    return -1;
}
std::string Value::getTypeName() const {
    switch (JS_VALUE_GET_TAG(m_value)) {
        case JS_TAG_BIG_INT: return "BigInt";
        case JS_TAG_SYMBOL: return "Symbol";
        case JS_TAG_STRING: return "string";
        case JS_TAG_MODULE: return "module";
        case JS_TAG_FUNCTION_BYTECODE: return "function";
        case JS_TAG_OBJECT: {
            auto ctx = std::get_if<0>(&m_ctxOrRt);
            if (!ctx) return "[object Object]";
            return Value::own(*ctx, JS_ToObjectString(ctx->getRaw(), m_value)).toString();
        } break;
        case JS_TAG_INT: return "int";
        case JS_TAG_BOOL: return "bool";
        case JS_TAG_NULL: return "null";
        case JS_TAG_UNDEFINED: return "undefined";
        case JS_TAG_UNINITIALIZED: return "uninitialized";
        case JS_TAG_CATCH_OFFSET: return "CatchOffset";
        case JS_TAG_EXCEPTION: return "Exception";
        case JS_TAG_FLOAT64: return "float64";
        default: return "<unknown>";
    }
}
bool Value::isNull() const {
    return JS_IsNull(m_value);
}
bool Value::isNumber() const {
    return JS_IsNumber(m_value);
}
bool Value::isString() const {
    return JS_IsString(m_value);
}
bool Value::isArray() const {
    return JS_IsArray(m_value);
}
bool Value::isObject() const {
    return JS_IsObject(m_value);
}
bool Value::isPromise() const {
    return JS_IsPromise(m_value);
}
bool Value::isClass(JSClassID id) const {
    return JS_GetClassID(m_value) == id;
}

Runtime Value::getRuntime() const {
    if (m_ctxOrRt.index() == 0) {
        return std::get<0>(m_ctxOrRt).getRuntime();
    }
    else {
        return std::get<1>(m_ctxOrRt).weakCopy();
    }
}
JSValue Value::takeValue() && {
    DEBUG_LOG("taking");
    auto value = m_value;
    m_value = JS_NULL;
    return value;
}

bool Value::isException() const {
    return JS_IsException(m_value);
}
void Value::setProperty(std::string_view prop, Value other, bool readonly) {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return;
    // Properties assume ownership of the value
    if (readonly) {
        JS_DefinePropertyValueStr(ctx->getRaw(), m_value, prop.data(), std::move(other).takeValue(), JS_PROP_THROW);
    }
    else {
        JS_SetPropertyStr(ctx->getRaw(), m_value, prop.data(), std::move(other).takeValue());
    }
}
void Value::setPropertyBare(std::string_view prop, std::function<CppFunction> get, std::function<CppFunction> set) {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return;
    auto atom = JS_NewAtom(ctx->getRaw(), prop.data());
    JS_DefinePropertyGetSet(
        ctx->getRaw(), m_value, atom,
        ctx->createFunctionBare("", get).takeValue(),
        (set ? ctx->createFunctionBare("", set).takeValue() : JS_NULL),
        JS_PROP_THROW
    );
    JS_FreeAtom(ctx->getRaw(), atom);
}
void Value::push(Value other) {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return;
    int64_t ix;
    if (JS_GetLength(ctx->getRaw(), m_value, &ix) < 0) {
        return;
    }
    JS_SetPropertyUint32(ctx->getRaw(), m_value, ix, std::move(other).takeValue());
}

std::optional<JSPromiseStateEnum> Value::getPromiseState() {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx || !this->isPromise()) return std::nullopt;
    return JS_PromiseState(ctx->getRaw(), m_value);
}
std::optional<Value> Value::getPromiseResult() {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx || !this->isPromise()) return std::nullopt;
    return Value::own(*ctx, JS_PromiseResult(ctx->getRaw(), m_value));
}

Value Value::call(Value self, std::vector<Value> const& args) {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return Value::own(this->getRuntime(), JS_NULL);

    std::vector<JSValue> mapped;
    mapped.reserve(args.size());
    for (auto arg : args) {
        mapped.push_back(std::move(arg).takeValue());
    }
    return Value::own(*ctx, JS_Call(ctx->getRaw(), m_value, std::move(self).takeValue(), mapped.size(), mapped.data()));
}

std::optional<int64_t> Value::getLength() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return std::nullopt;
    int64_t length;
    if (JS_GetLength(ctx->getRaw(), m_value, &length) < 0) {
        return std::nullopt;
    }
    return length;
}
std::optional<Value> Value::getArrayItem(uint32_t prop) const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return std::nullopt;
    auto atom = JS_NewAtomUInt32(ctx->getRaw(), prop);
    if (JS_HasProperty(ctx->getRaw(), m_value, atom) != true) {
        JS_FreeAtom(ctx->getRaw(), atom);
        return std::nullopt;
    }
    auto value = JS_GetProperty(ctx->getRaw(), m_value, atom);
    JS_FreeAtom(ctx->getRaw(), atom);
    return Value::copy(*ctx, value);
}
std::vector<Value> Value::getArrayItems() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return {};
    std::vector<Value> props;
    for (int32_t i = 0; i < this->getLength().value_or(0); i += 1) {
        if (auto prop = this->getArrayItem(i)) {
            props.push_back(*prop);
        }
    }
    return props;
}
std::optional<Value> Value::getProperty(std::string_view prop) const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return std::nullopt;
    auto atom = JS_NewAtom(ctx->getRaw(), prop.data());
    if (JS_HasProperty(ctx->getRaw(), m_value, atom) != true) {
        JS_FreeAtom(ctx->getRaw(), atom);
        return std::nullopt;
    }
    auto value = JS_GetProperty(ctx->getRaw(), m_value, atom);
    JS_FreeAtom(ctx->getRaw(), atom);
    return Value::copy(*ctx, value);
}
std::vector<std::string> Value::getPropertyNames() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return {};

    JSPropertyEnum* props = nullptr;
    uint32_t propCount;
    if (JS_GetOwnPropertyNames(ctx->getRaw(), &props, &propCount, m_value, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) <= 0) {
        return {};
    }

    std::vector<std::string> names;
    for (uint32_t i = 0; i < propCount; i += 1) {
        auto raw = JS_AtomToCString(ctx->getRaw(), props[i].atom);
        names.push_back(raw);
        JS_FreeCString(ctx->getRaw(), raw);
    }
    JS_FreePropertyEnum(ctx->getRaw(), props, propCount);
    return names;
}
std::unordered_map<std::string, Value> Value::getProperties() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return {};

    JSPropertyEnum* props = nullptr;
    uint32_t propCount;
    if (JS_GetOwnPropertyNames(ctx->getRaw(), &props, &propCount, m_value, JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY) <= 0) {
        return {};
    }

    std::unordered_map<std::string, Value> ret;
    for (uint32_t i = 0; i < propCount; i += 1) {
        auto raw = JS_AtomToCString(ctx->getRaw(), props[i].atom);
        auto prop = Value::copy(*ctx, JS_GetProperty(ctx->getRaw(), m_value, props[i].atom));
        ret.insert({ raw, prop });
        JS_FreeCString(ctx->getRaw(), raw);
    }
    JS_FreePropertyEnum(ctx->getRaw(), props, propCount);

    return ret;
}

// Value::Iterator Value::begin() const {
//     return Iterator(*this, 0);
// }
// Value::Iterator Value::end() const {
//     return Iterator(*this, this->getLength().value_or(0));
// }

bool Value::toBool() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return false;
    return JS_ToBool(ctx->getRaw(), m_value) == 1;
}
std::optional<int32_t> Value::toInt32() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return std::nullopt;
    int32_t res = 0;
    if (JS_ToInt32(ctx->getRaw(), &res, m_value) < 0) {
        return std::nullopt;
    }
    return res;
}
std::optional<double> Value::toNumber() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return std::nullopt;
    double res = 0;
    if (JS_ToFloat64(ctx->getRaw(), &res, m_value) < 0) {
        return std::nullopt;
    }
    return res;
}
std::string Value::toString() const {
    auto ctx = std::get_if<0>(&m_ctxOrRt);
    if (!ctx) return "";
    auto alloc = JS_ToCString(ctx->getRaw(), m_value);
    auto ret = std::string(alloc);
    JS_FreeCString(ctx->getRaw(), alloc);
    return ret;
}

Module::Module(Context ctx, JSModuleDef* modValue, Value modulePromise)
  : m_ctx(std::move(ctx)), m_promise(std::move(modulePromise)), m_def(modValue)
{}

Module Module::null() {
    return Module(Context::null(), nullptr, Value::own(Context::null(), JS_NULL));
}
Module Module::own(Context ctx, JSModuleDef* modValue, Value modulePromise) {
    return Module(std::move(ctx), modValue, std::move(modulePromise));
}

Result<std::optional<Value>> Module::tick() {
    auto ctx = m_ctx.getRaw();
    auto state = m_promise.getPromiseState();
    if (!state || !ctx) {
        return Ok(std::nullopt);
    }
    switch (*state) {
        case JS_PROMISE_FULFILLED: {
            return Ok(m_promise.getPromiseResult());
        } break;

        case JS_PROMISE_PENDING: {
            JSContext* next;
            // Try ticking the runtime once to resolve existing Promises
            if (JS_ExecutePendingJob(JS_GetRuntime(ctx), &next) >= 0) {
                return Ok(std::nullopt);
            }
            // If that failed, return error
            else {
                auto exp = m_ctx.getException().toString();
                log::info("error: {}", exp);
                return Err(exp);
            }
        } break;

        case JS_PROMISE_REJECTED: {
            return Err(m_promise.getPromiseResult()->toString());
        } break;
    }
}

Module::Module(Module const& other)
  : m_ctx(other.m_ctx),
  m_promise(other.m_promise),
  m_def(other.m_def)
{}
Module& Module::operator=(Module const& other) {
    m_ctx = other.m_ctx;
    m_promise = other.m_promise;
    m_def = other.m_def;
    return *this;
}
Module::Module(Module&& other)
  : m_ctx(std::move(other.m_ctx)),
    m_promise(std::move(other.m_promise)),
    m_def(other.m_def)
{
    other.m_def = nullptr;
}
Module& Module::operator=(Module&& other) {
    m_ctx = std::move(other.m_ctx);
    m_promise = std::move(other.m_promise);
    m_def = other.m_def;
    other.m_def = nullptr;
    return *this;
}

Module::~Module() {}

// Value::Iterator::Iterator(Value value, size_t index)
//   : m_ctx(std::get<0>(value.m_ctxOrRt)), m_value(value), m_index(index)
// {}

// Value::Iterator& Value::Iterator::operator++() {
//     m_index += 1;
//     return *this;
// }
// Value::Iterator& Value::Iterator::operator++(int) {
//     m_index += 1;
//     return *this;
// }

// Value Value::Iterator::operator*() {
//     return m_value.getProperty(m_index).value_or(m_ctx.createNull());
// }

// bool Value::Iterator::operator==(Iterator const& other) const {
//     return m_value == other.m_value && m_index == other.m_index;
// }
// bool Value::Iterator::operator!=(Iterator const& other) const {
//     return !(*this == other);
// }
