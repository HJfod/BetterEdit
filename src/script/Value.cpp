
#include "Parser.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/ranges.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/binding/FLAlertLayer.hpp>

using namespace script;

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
        [&](std::vector<Value> const&) {
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
    this->add("print", Value::rc(make<FunExpr>(
        "print",
        decltype(FunExpr::params) {{ "msg", std::nullopt }},
        make<Expr>({
            .value = make<CppExpr>({
                .eval = [](State& state) -> Result<Rc<Value>> {
                    log::info("{}", state.get("msg")->toString());
                    return Ok(state.get("msg"));
                }
            }).unwrap(),
        }).unwrap(),
        "/* built-in function */"
    ).unwrap()));

    this->add("alert", Value::rc(make<FunExpr>(
        "alert",
        decltype(FunExpr::params) {
            { "msg", std::nullopt },
            { "title", make<Expr>({
                .value = make<LitExpr>({
                    .value = NullLit(),
                    .src = "/* built-in expr */",
                }).unwrap()
            }).unwrap() },
        },
        make<Expr>({
            .value = make<CppExpr>({
                .eval = [](State& state) -> Result<Rc<Value>> {
                    auto title = state.get("title");
                    FLAlertLayer::create(
                        (title->isNull() ? "Script Alert" : title->toString()).c_str(),
                        state.get("msg")->toString(),
                        "OK"
                    )->show();
                    return Ok(state.get("msg"));
                }
            }).unwrap(),
        }).unwrap(),
        "/* built-in function */"
    ).unwrap()));
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
