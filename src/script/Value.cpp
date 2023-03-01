
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

static Rc<Expr> builtinLit(Lit const& value) {
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
        [&](Array&) -> std::optional<Rc<Value>> {
            return std::nullopt;
        },
        [&](Ref<GameObject>& obj) -> std::optional<Rc<Value>> {
            switch (hash(name.c_str())) {
                case hash("deselect"): {
                    return builtinFun(
                        {}, [&obj](State& state) {
                            EditorUI::get()->deselectObject(obj);
                            obj->deselectObject();
                            return Ok(Value::rc(NullLit()));
                        }
                    );
                } break;

                case hash("move"): {
                    return builtinFun(
                        {
                            { "x", builtinLit(0.0) },
                            { "y", builtinLit(0.0) },
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
                            return Ok(Value::rc(NullLit()));
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
        resetExecutionCounter();
        GEODE_UNWRAP_INTO(auto ast, ListExpr::pull(stream));
        if (debug) {
            log::info("AST: {}", ast->debug());
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
    this->add("print", builtinFun(
        {{ "msg", std::nullopt }},
        [](State& state) -> Result<Rc<Value>> {
            log::info("{}", state.get("msg")->toString());
            return Ok(state.get("msg"));
        }
    ));

    this->add("alert", builtinFun(
        {
            { "msg", std::nullopt },
            { "title", make<Expr>({
                .value = make<LitExpr>({
                    .value = NullLit(),
                    .src = "/* built-in expr */",
                }).unwrap()
            }).unwrap() },
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

    this->add("getSelectedObjects", builtinFun(
        {},
        [](State& state) -> Result<Rc<Value>> {
            auto ui = EditorUI::get();
            if (ui->m_selectedObject) {
                return Ok(Value::rc(Array { Value(ui->m_selectedObject) } ));
            }
            else if (ui->m_selectedObjects->count()) {
                Array res;
                res.reserve(ui->m_selectedObjects->count());
                for (auto obj : CCArrayExt<GameObject>(ui->m_selectedObjects)) {
                    res.push_back(Value(obj));
                }
                return Ok(Value::rc(res));
            }
            else {
                return Ok(Value::rc(Array()));
            }
        }
    ));
    
    this->add("random", builtinFun(
        {},
        [&](State& state) -> Result<Rc<Value>> {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.0, 1.0);
            return Ok(Value::rc(dis(gen)));
        }
    ));
}
