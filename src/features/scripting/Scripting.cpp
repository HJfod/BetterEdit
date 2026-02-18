#include "Scripting.hpp"
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <utils/Editor.hpp>

using namespace geode::prelude;

struct ContextData final {

};

struct ScriptInput final {
    enum class Type {
        Int,
        Number,
        String,
    };

    Type type;
    std::string name;
    std::optional<std::string> description;
};

namespace qjs::detail {
    template <>
    struct JsTypeToCpp<GameObject*> {
        static Result<GameObject*> from(Context ctx, Value arg) {
            if (!arg.isClass(*ctx.getRuntime().getClassID("GameObject"))) {
                return Err("Expected GameObject, got {}", arg.getTypeName());
            }
            return Ok(arg.getOpaque<GameObject>());
        }
        static Value to(Context ctx, GameObject* value) {
            auto ret = ctx.createObject(*ctx.getRuntime().getClassID("GameObject"));
            value->retain();
            ret.setOpaque(value);
            return ret;
        }
    };
    static_assert(IsValidJsTypeToCpp<GameObject*>);
    
    template <>
    struct JsTypeToCpp<CCPoint> {
        static Result<CCPoint> from(Context ctx, Value arg) {
            auto parsed = parseJsType<std::tuple<float, float>>(ctx, arg);
            if (!parsed) {
                return Err("{} (in point)", parsed.unwrapErr());
            }
            return Ok(ccp(std::get<0>(*parsed), std::get<1>(*parsed)));
        }
        static Value to(Context ctx, CCPoint value) {
            auto ret = ctx.createObject();
            ret.setProperty("x", ctx.createNumber(value.x));
            ret.setProperty("y", ctx.createNumber(value.y));
            return ret;
        }
    };
    static_assert(IsValidJsTypeToCpp<CCPoint>);

    template <>
    struct JsTypeToCpp<ScriptInput> {
        static Result<ScriptInput> from(Context ctx, Value arg) {
            auto parsed = parseJsType<std::unordered_map<std::string, std::string>>(ctx, arg);
            if (!parsed) {
                return Err("{} (in script input item)", parsed.unwrapErr());
            }
            auto map = *parsed;
            if (!map.contains("type")) {
                return Err("Script input item is missing required property \"type\"");
            }
            if (!map.contains("name")) {
                return Err("Script input item is missing required property \"name\"");
            }
            ScriptInput result;
            for (auto [name, prop] : std::move(map)) {
                switch (hash(name)) {
                    case hash("type"): {
                        switch (hash(prop)) {
                            case hash("int"):    result.type = ScriptInput::Type::Int; break;
                            case hash("number"): result.type = ScriptInput::Type::Number; break;
                            case hash("string"): result.type = ScriptInput::Type::String; break;
                            default: {
                                return Err("Invalid type script input item type \"{}\"", prop);
                            } break;
                        }
                    } break;

                    case hash("name"): {
                        result.name = prop;
                    } break;

                    case hash("description"): {
                        result.description = prop;
                    } break;

                    default: {
                        return Err("Invalid script input item key \"{}\"", name);
                    } break;
                }
            }
            return Ok(result);
        }
        static Value to(Context ctx, ScriptInput) {
            return ctx.createNull();
        }
    };
    static_assert(IsValidJsTypeToCpp<ScriptInput>);
}

std::shared_ptr<JsScript> JsScript::create(std::filesystem::path const& path) {
    auto ret = std::make_shared<JsScript>();
    ret->m_path = path;

    // Funny hack to prevent the log() calls here from posting events
    ret->m_queuedLogEvent = true;

    if (auto data = file::readString(path)) {
        ret->m_data = *data;
    }
    else {
        ret->log(Log::Level::Error, data.unwrapErr());
        ret->m_runnable = false;
    }
    ret->m_title = path.filename().string();
    ret->m_author = "[Unknown]";
    ret->m_version = VersionInfo(1, 0, 0);

    std::stringstream str(ret->m_data);
    std::string line;
    while (std::getline(str, line)) {
        if (!line.starts_with("///")) {
            continue;
        }
        std::stringstream data(line);

        // Consume the slashes
        data.get();
        data.get();
        data.get();

        // Eat any number of whitespace
        while (std::isspace(data.peek())) {
            data.get();
        }

        // Require a tag
        if (data.get() != '@') {
            continue;
        }

        // Get the name of the tag
        std::string tagName = "";
        while (std::isalnum(data.peek())) {
            tagName += data.get();
        }

        switch (hash(tagName)) {
            case hash("name"): {
                std::getline(data, ret->m_title);
                string::trimIP(ret->m_title);
                if (ret->m_title.empty()) {
                    ret->log(Log::Level::Error, "Script metadata `@name` requires a value");
                    ret->m_runnable = false;
                }
            } break;

            case hash("by"): {
                std::getline(data, ret->m_author);
                string::trimIP(ret->m_author);
                if (ret->m_author.empty()) {
                    ret->log(Log::Level::Error, "Script metadata `@by` requires a value");
                    ret->m_runnable = false;
                }
            } break;

            case hash("version"): {
                std::string ver;
                std::getline(data, ver);
                string::trimIP(ver);
                if (auto parsed = VersionInfo::parse(ver)) {
                    ret->m_version = *parsed;
                }
                else {
                    ret->log(
                        Log::Level::Error,
                        "Script metadata `@version` has invalid value: {}",
                        parsed.unwrapErr()
                    );
                    ret->m_runnable = false;
                }
            } break;

            default: {
                ret->log(Log::Level::Error, "Invalid metadata tag '{}'", tagName);
                ret->m_runnable = false;
            } break;
        }
    }

    if (ret->m_runnable) {
        ret->log(Log::Level::Status, "Ready to run script");
    }

    // OK stop the hack
    ret->m_queuedLogEvent = false;

    return ret;
}

std::filesystem::path JsScript::getPath() const {
    return m_path;
}
std::string JsScript::getTitle() const {
    return m_title;
}
std::string JsScript::getAuthor() const {
    return m_author;
}
VersionInfo JsScript::getVersion() const {
    return m_version;
}
std::vector<JsScript::Log> JsScript::getLastRunLogs() const {
    return m_lastRunLogs;
}
bool JsScript::canRun() const {
    return m_runnable;
}
JsScript::Log::Level JsScript::getLastRunSeverity() const {
    Log::Level highest = Log::Level::Status;
    for (auto const& log : m_lastRunLogs) {
        if (int(log.level) > int(highest)) {
            highest = log.level;
        }
    }
    return highest;
}

void JsScript::log(Log::Level level, std::string_view message) {
    m_lastRunLogs.push_back(Log {
        .level = level,
        .message = std::string(message),
    });
    if (!m_queuedLogEvent) {
        m_queuedLogEvent = true;
        Loader::get()->queueInMainThread([weak = weak_from_this()] {
            if (auto ptr = weak.lock()) {
                ptr->m_queuedLogEvent = false;
                JsScriptLoggedEvent(ptr).send();
            }
        });
    }
}

bool JsScript::run() {
    // Note: script can not be running multiple times at once!
    // ^ This is currently not being checked in any way
    if (!this->canRun()) {
        return false;
    }

    // First make sure all previous context is destroyed
    m_lastRunLogs.clear();
    m_finished = false;
    m_runtime = qjs::Runtime::null();
    m_ctx = qjs::Context::null();
    m_module = qjs::Module::null();

    // Start up new runtime
    m_runtime = qjs::Runtime::create();
    m_ctx = qjs::Context::create(m_runtime);

    auto gameObjectClassID = m_runtime.createClass(
        "GameObject",
        +[](qjs::Runtime, qjs::Value const& value) {
            CC_SAFE_RELEASE(value.getOpaque<GameObject>());
        }
    );
    if (!gameObjectClassID) {
        this->log(Log::Level::Error, fmt::format(
            "Unable to setup GameObject class: {}",
            gameObjectClassID.unwrapErr()
        ));
    }

    auto gameObjectClassProto = m_ctx.createObject();
    gameObjectClassProto.setProperty(
        "id", 
        [](qjs::Context, GameObject* self) {
            return self->m_objectID;
        }
    );
    gameObjectClassProto.setProperty(
        "x",
        [](qjs::Context, GameObject* self) {
            return self->getPositionX();
        },
        [](qjs::Context, GameObject* self, float x) {
            EditorUI::get()->moveObject(self, ccp(x, 0));
            return self->getPositionX();
        }
    );
    gameObjectClassProto.setProperty(
        "y",
        [](qjs::Context, GameObject* self) {
            return self->getPositionY();
        },
        [](qjs::Context, GameObject* self, float y) {
            EditorUI::get()->moveObject(self, ccp(0, y));
            return self->getPositionY();
        }
    );
    gameObjectClassProto.setProperty(
        "selected",
        [](qjs::Context, GameObject* self) {
            return self->m_isSelected;
        },
        [](qjs::Context, GameObject* self, bool selected) {
            if (selected) {
                auto objs = EditorUI::get()->getSelectedObjects();
                objs->addObject(self);
                EditorUI::get()->selectObjects(objs, false);
            }
            else {
                EditorUI::get()->deselectObject(self);
            }
            EditorUI::get()->updateButtons();
            return self->m_isSelected;
        }
    );
    gameObjectClassProto.setProperty(
        "rotation",
        [](qjs::Context, GameObject* self) {
            return self->getRotation();
        },
        [](qjs::Context, GameObject* self, float rotation) {
            self->setRotation(rotation);
            return rotation;
        }
    );
    m_ctx.setClassProto(*gameObjectClassID, gameObjectClassProto);

    auto global = m_ctx.getGlobalObject();

    global.setProperty("print", m_ctx.createFunctionBare("", [this](qjs::Context ctx, auto, std::vector<qjs::Value> const& args) {
        std::string log = "";
        for (size_t i = 0; i < args.size(); i += 1) {
            if (i > 0) {
                log += " ";
            }
            log += args.at(i).toString();
        }
        this->log(Log::Level::Info, log);
        return ctx.createUndefined();
    }));
    global.setProperty("input", m_ctx.createFunction(
        "input",
        [](qjs::Context ctx, qjs::Value, std::unordered_map<std::string, ScriptInput> const& inputs) {
            auto promise = ctx.createPromise();
            auto result = ctx.createObject();
            for (auto& [name, input] : inputs) {
                log::info("wants input for '{}' ('{}') (type {})", input.name, input.description, static_cast<int>(input.type));
                switch (input.type) {
                    case ScriptInput::Type::Int: {
                        result.setProperty(name, ctx.createNumber(123));
                    } break;

                    case ScriptInput::Type::Number: {
                        result.setProperty(name, ctx.createNumber(456));
                    } break;

                    case ScriptInput::Type::String: {
                        result.setProperty(name, ctx.createString("hiii"));
                    } break;
                }
            }
            promise.resolve.call(ctx.createUndefined(), { std::move(result) });
            return promise.value;
        }
    ));

    auto editor = m_ctx.createObject();
    editor.setProperty("createObject", m_ctx.createFunction(
        "<Editor>.createObject",
        [](qjs::Context, qjs::Value, int32_t objID) {
            return EditorUI::get()->createObject(objID, ccp(0, 0));
        }
    ));
    editor.setProperty("moveObjectsBy", m_ctx.createFunction(
        "<Editor>.moveObjectsBy",
        [](qjs::Context, qjs::Value, std::vector<GameObject*> const& objs, CCPoint const& by) {
            for (auto obj : objs) {
                EditorUI::get()->moveObject(obj, by);
            }
            return nullptr;
        }
    ));
    editor.setProperty("getSelectedObjects", m_ctx.createFunction(
        "<Editor>.getSelectedObjects",
        [](qjs::Context, qjs::Value) {
            return be::getSelectedObjects(EditorUI::get());
        }
    ));
    editor.setProperty("getViewCenter", m_ctx.createFunction(
        "<Editor>.getViewCenter",
        [](qjs::Context, qjs::Value) {
            return LevelEditorLayer::get()->m_objectLayer->convertToNodeSpace(
                CCDirector::get()->getWinSize() / 2
            );
        }
    ));
    global.setProperty("editor", editor);

    auto value = m_ctx.eval(m_data, m_path.filename().string());
    if (!value) {
        this->log(Log::Level::Error, value.unwrapErr());
        return false;
    }
    m_module = *value;
    return true;
}
bool JsScript::tick() {
    if (m_finished) {
        return true;
    }
    auto res = m_module.tick();
    if (!res) {
        m_finished = true;
        this->log(Log::Level::Error, res.unwrapErr());
        return false;
    }
    auto value = res.unwrap();
    if (value) {
        m_finished = true;
        this->log(Log::Level::Status, "Finished running script with value {}", value->toString());
    }
    return true;
}

ScriptManager* ScriptManager::get() {
    static auto ret = ScriptManager();
    return &ret;
}

std::vector<std::shared_ptr<JsScript>> ScriptManager::getScripts() const {
    return m_scripts;
}

void ScriptManager::reloadScripts() {
    m_scripts.clear();
    for (auto& dir : {
        Mod::get()->getResourcesDir(),
        Mod::get()->getConfigDir() / "scripts"
    }) {
        if (auto files = file::readDirectory(dir)) {
            for (auto& file : files.unwrap()) {
                if (file.extension() != ".js" && file.extension() != ".mjs") {
                    continue;
                }
                m_scripts.push_back(JsScript::create(file));
            }
        }
    }
}

bool ScriptManager::tickAll() {
    bool success = true;
    for (auto script : m_scripts) {
        success &= !script->tick();
    }
    return success;
}

class $modify(TickEditorLayer, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelEditorLayer::init(level, p1))
            return false;
        
        // todo: this should be optimized to only run when scripts are running
        this->schedule(schedule_selector(TickEditorLayer::onTick));
        
        return true;
    }

    void onTick(float) {
        ScriptManager::get()->tickAll();
    }
};
