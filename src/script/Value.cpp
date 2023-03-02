
#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <random>

using namespace script;

static Rc<Value> builtinFun(
    decltype(FunExpr::params) const& params,
    std::function<Result<Rc<Value>>(State&)> fun
) {
    return Value::rc(make<FunExpr>(
        "__builtin_fun",
        params,
        make<Expr>({
            .value = make<CppExpr>({
                .eval = fun
            }).unwrap(),
        }).unwrap(),
        "/* built-in function */"
    ).unwrap());
}

static Rc<Expr> builtinLitExpr(Lit const& value) {
    return make<Expr>({
        .value = make<LitExpr>({
            .value = value,
            .src = "/* built-in expr */",
        }).unwrap()
    }).unwrap();
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
                return fmt::format("function({})", fun->ident.value_or("anonymous"));
            }
            return fun->ident.value_or("anonymous function");
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
        [&](Array const&) {
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

std::optional<Rc<Value>> Value::member(std::string const& name) {
    return std::visit(makeVisitor {
        [&](NullLit&) -> std::optional<Rc<Value>> {
            return std::nullopt;
        },
        [&](BoolLit&) -> std::optional<Rc<Value>> {
            return std::nullopt;
        },
        [&](NumLit&) -> std::optional<Rc<Value>> {
            return std::nullopt;
        },
        [&](StrLit&) -> std::optional<Rc<Value>> {
            return std::nullopt;
        },
        [&](Array& arr) -> std::optional<Rc<Value>> {
            switch (hash(name.c_str())) {
                case hash("count"): {
                    return builtinFun(
                        {}, [&arr](State& state) {
                            return Ok(Value::rc(NumLit(arr.size())));
                        }
                    );
                } break;

                default: {
                    return std::nullopt;
                } break;
            }
        },
        [&](Ref<GameObject>& obj) -> std::optional<Rc<Value>> {
            switch (hash(name.c_str())) {
                case hash("x"): {
                    return Value::rc(obj->getPositionX(), [&](Value& value) {
                        if (auto num = value.has<NumLit>()) {
                            auto x = obj->getPositionX();
                            EditorUI::get()->moveObject(
                                obj, CCPoint { static_cast<float>(*num - x), 0.f }
                            );
                        }
                    });
                } break;

                case hash("y"): {
                    return Value::rc(obj->getPositionY(), [&](Value& value) {
                        if (auto num = value.has<NumLit>()) {
                            auto x = obj->getPositionY();
                            EditorUI::get()->moveObject(
                                obj, CCPoint { 0.f, static_cast<float>(*num - x) }
                            );
                        }
                    });
                } break;
                
                case hash("select"): {
                    return builtinFun(
                        {{ "unique", builtinLitExpr(false) }},
                        [&obj](State& state) {
                            if (state.get("unique")->truthy()) {
                                EditorUI::get()->selectObject(obj, false);
                            }
                            else {
                                auto objs = EditorUI::get()->getSelectedObjects();
                                objs->addObject(obj);
                                EditorUI::get()->selectObjects(objs, false);
                            }
                            EditorUI::get()->updateButtons();
                            return Ok(Value::rc(obj));
                        }
                    );
                } break;
                
                case hash("deselect"): {
                    return builtinFun(
                        {}, [&obj](State& state) {
                            EditorUI::get()->deselectObject(obj);
                            obj->deselectObject();
                            EditorUI::get()->updateButtons();
                            return Ok(Value::rc(obj));
                        }
                    );
                } break;

                case hash("move"): {
                    return builtinFun(
                        {
                            { "x", builtinLitExpr(0.0) },
                            { "y", builtinLitExpr(0.0) },
                        },
                        [&obj](State& state) -> Result<Rc<Value>> {
                            auto x = state.get("x")->has<NumLit>();
                            if (!x) {
                                return Err("x must be a number");
                            }
                            auto y = state.get("y")->has<NumLit>();
                            if (!y) {
                                return Err("y must be a number");
                            }
                            EditorUI::get()->moveObject(obj, CCPoint {
                                static_cast<float>(*x),
                                static_cast<float>(*y)
                            });
                            return Ok(Value::rc(obj));
                        }
                    );
                } break;

                default: {
                    return std::nullopt;
                } break;
            }
        },
        [&](Rc<const FunExpr>&) -> std::optional<Rc<Value>> {
            return std::nullopt;
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

bool State::has(std::string const& name, bool top) const {
    if (top) {
        return entities.back().count(name);
    }
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

Result<State> State::parse(ghc::filesystem::path const& path, bool debug) {
    std::ifstream stream(path, std::ios::binary);
    return State::parse(stream, debug);
}

Result<State> State::parse(std::string const& code, bool debug) {
    std::stringstream stream(code);
    return State::parse(stream, debug);
}

Result<State> State::parse(InputStream& stream, bool debug) {
    try {
        resetExecutionCounter();
        GEODE_UNWRAP_INTO(auto attrs, Attrs::pull(stream));

        if (!attrs.version) {
            log::warn("Script target version not specified");
        }
        else {
            if (attrs.version > SCRIPT_VERSION) {
                return Err("Script is written for a newer version of BetterEdit");
            }
        }

        GEODE_UNWRAP_INTO(auto ast, ListExpr::pull(stream, attrs));
        if (debug) {
            log::info("AST: {}", ast->debug());
        }

        return Ok(State(attrs));
    }
    catch (std::exception& err) {
        return Err("Uncaught exception: {}", err.what());
    }
    return Ok();
}

Result<Value> State::run() {
    try {
        GEODE_UNWRAP_INTO(auto val, ast->eval(state));
        return Ok(*val);
    }
    catch (std::exception& err) {
        return Err("Uncaught exception: {}", err.what());
    }
}

State::State(Rc<Expr> ast, Attrs const& attrs) : ast(ast), attrs(attrs) {
    // lang
    this->add("print", builtinFun(
        {{ "msg", std::nullopt }},
        [](State& state) -> Result<Rc<Value>> {
            log::info("{}", state.get("msg")->toString());
            return Ok(state.get("msg"));
        }
    ));
    this->add("assert", builtinFun(
        {
            { "expr", std::nullopt },
            { "msg", builtinLitExpr("Assertion failed") }
        },
        [&](State& state) -> Result<Rc<Value>> {
            if (!state.get("expr")->truthy()) {
                return Err(state.get("msg")->toString());
            }
            return Ok(state.get("expr"));
        }
    ));
    this->add("alert", builtinFun(
        {
            { "msg", std::nullopt },
            { "title", builtinLitExpr(NullLit()) },
        },
        [](State& state) -> Result<Rc<Value>> {
            auto title = state.get("title");
            FLAlertLayer::create(
                (title->isNull() ? "Script Alert" : title->toString()).c_str(),
                state.get("msg")->toString(),
                "OK"
            )->show();
            return Ok(state.get("msg"));
        }
    ));

    // gd
    this->add("getSelectedObjects", builtinFun(
        {},
        [](State& state) -> Result<Rc<Value>> {
            auto ui = EditorUI::get();
            if (ui->m_selectedObject) {
                return Ok(Value::rc(Array { Value(ui->m_selectedObject) } ));
            }
            else {
                Array res;
                res.reserve(ui->m_selectedObjects->count());
                for (auto obj : CCArrayExt<GameObject>(ui->m_selectedObjects)) {
                    res.push_back(Value(obj));
                }
                return Ok(Value::rc(res));
            }
        }
    ));
    this->add("getAllObjects", builtinFun(
        {},
        [](State& state) -> Result<Rc<Value>> {
            auto objs = EditorUI::get()->m_editorLayer->m_objects;
            Array res;
            res.reserve(objs->count());
            for (auto obj : CCArrayExt<GameObject>(objs)) {
                res.push_back(Value(obj));
            }
            return Ok(Value::rc(res));
        }
    ));
    this->add("deselectAll", builtinFun(
        {},
        [](State& state) -> Result<Rc<Value>> {
            EditorUI::get()->deselectAll();
            return Ok(Value::rc(NullLit()));
        }
    ));

    // math
    this->add("random", builtinFun(
        {},
        [&](State& state) -> Result<Rc<Value>> {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.0, 1.0);
            return Ok(Value::rc(dis(gen)));
        }
    ));
    this->add("sin", builtinFun(
        {{ "x", std::nullopt }},
        [](State& state) -> Result<Rc<Value>> {
            if (auto num = state.get("x")->has<NumLit>()) {
                return Ok(Value::rc(sin(*num)));
            }
            else {
                return Err("sin requires a number, got {}", state.get("x")->typeName());
            }
        }
    ));
    this->add("cos", builtinFun(
        {{ "x", std::nullopt }},
        [](State& state) -> Result<Rc<Value>> {
            if (auto num = state.get("x")->has<NumLit>()) {
                return Ok(Value::rc(cos(*num)));
            }
            else {
                return Err("cos requires a number, got {}", state.get("x")->typeName());
            }
        }
    ));
    this->add("tan", builtinFun(
        {{ "x", std::nullopt }},
        [](State& state) -> Result<Rc<Value>> {
            if (auto num = state.get("x")->has<NumLit>()) {
                return Ok(Value::rc(tan(*num)));
            }
            else {
                return Err("tan requires a number, got {}", state.get("x")->typeName());
            }
        }
    ));
}
