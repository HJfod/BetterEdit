#include "gd.hpp"
#include <sol.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <matjson.hpp>
#include <Geode/utils/cocos.hpp>
#include <scripting/LuaHelper.hpp>
#include <scripting/LuaManager.hpp>
#include <Geode/loader/Log.hpp>

using namespace cocos2d;
using namespace geode;

static std::string objectStringForTable(const sol::lua_table& t)
{
    std::string ret;
    
    auto solValueToString = [](const sol::object& v) -> std::optional<std::string>
    {
        switch(v.get_type())
        {
            case sol::type::number:
            {
                if(LuaHelper::is_integer(v))
                    return fmt::format("{}", v.as<int>());
                else if(v.is<double>())
                    return fmt::format("{}", v.as<double>());
                else
                    LuaManager::fatalError("Unknown number type in objectStringForTable");
            }

            case sol::type::string:
                return v.as<std::string>();
            case sol::type::boolean:
                return v.as<bool>() ? "1" : "0";
            default:
                return {};
        }
    };

    for(const auto& property : t)
    {
        //if(!LuaHelper::is_integer(property.first)) continue;
        if(property.first.get_type() != sol::type::number)
        {

        }
        
        if(auto str = solValueToString(property.second))
        {
            std::string toInsert = fmt::format("{},{},", property.first.as<int>(), *str);
            ret += toInsert;
        }
    }

    if(!ret.empty())
    {
        ret.pop_back(); //remove last ,
    }
    return ret;
}


void gd::forAllObjects(const sol::function& f)
{
    for(GameObject* obj : geode::cocos::CCArrayExt<GameObject*>(LuaHelper::editor()->m_objects))
    {
        f(obj);
    }
}

void gd::forSelectedObjects(const sol::function& f)
{
    for(GameObject* obj : geode::cocos::CCArrayExt<GameObject*>(LuaHelper::ui()->getSelectedObjects()))
    {
        f(obj);
    }
}

std::vector<GameObject*> gd::getSelectedObjects()
{
    auto objs = LuaHelper::ui()->getSelectedObjects();

    std::vector<GameObject*> ret;
    ret.reserve(objs->count());

    for (GameObject* obj : geode::cocos::CCArrayExt<GameObject*>(objs))
    {
        ret.push_back(obj);
    }
    return ret;
}

std::vector<GameObject*> gd::getAllObjects()
{
    auto editor = LuaHelper::editor();
    std::vector<GameObject*> ret;
    ret.reserve(editor->m_objects->count());

    for (GameObject* obj : geode::cocos::CCArrayExt<GameObject*>(editor->m_objects))
    {
        ret.push_back(obj);
    }
    return ret;
}

void gd::deleteObjects(const sol::variadic_args& args)
{
    auto ui = LuaHelper::ui();
    for (GameObject* o : LuaHelper::getFromVarArgs<GameObject*>(args))
    {
        ui->deleteObject(o, false);
    }
}


void gd::registerTable(sol::state& lua)
{
    lua.create_named_table
    (   "gd", //name
        "create", gd::createObject, //key, value
        "getSelectedObjects", gd::getSelectedObjects,
        "getAllObjects", gd::getAllObjects,
        "deleteObjects", gd::deleteObjects,
        "getObjectCount", [](){ return LuaHelper::editor()->m_objectCount; },
        "forAllObjects", gd::forAllObjects,
        "forSelectedObjects", gd::forSelectedObjects
    );
}


GameObject* gd::createObject(const sol::lua_table& t)
{
    if(t.empty())
    {
        LuaManager::fatalError("Table can not be empty");
        geode::log::info("test");
    }

    std::string objectString = objectStringForTable(t);

    if(objectString.empty())
    {
        LuaManager::fatalError("No valid argument was found in gd.create");
    }

    if (CCArray* objects = LuaHelper::editor()->createObjectsFromString(objectString, true, true))
    {
        if(auto obj = static_cast<GameObject*>(objects->objectAtIndex(0)))
        {
            return obj;
        }
    }
    LuaManager::fatalError("Error creating object from [{}]", objectString);
    return nullptr;
}


//scripts/gencpp.lua
void gd::setObjectPropertyEnums(sol::state& lua) {
    enum class edit_area_rotate_trigger {};
    lua.new_enum<edit_area_rotate_trigger>("edit_area_rotate_trigger", {
        { "easing_rate", static_cast<edit_area_rotate_trigger>(243) },
        { "easing", static_cast<edit_area_rotate_trigger>(242) },
        { "offset", static_cast<edit_area_rotate_trigger>(220) },
        { "rotation", static_cast<edit_area_rotate_trigger>(270) },
        { "rotation_rand", static_cast<edit_area_rotate_trigger>(271) },
        { "group_effect_id", static_cast<edit_area_rotate_trigger>(51) },
        { "mod_back", static_cast<edit_area_rotate_trigger>(264) },
        { "deadzone", static_cast<edit_area_rotate_trigger>(282) },
        { "mod_front", static_cast<edit_area_rotate_trigger>(263) },
        { "offset_y_rand", static_cast<edit_area_rotate_trigger>(253) },
        { "length_rand", static_cast<edit_area_rotate_trigger>(223) },
        { "length", static_cast<edit_area_rotate_trigger>(222) },
        { "use_eid", static_cast<edit_area_rotate_trigger>(355) },
        { "offset_y", static_cast<edit_area_rotate_trigger>(252) },
        { "offset_rand", static_cast<edit_area_rotate_trigger>(221) },
    });
    enum class shake_trigger {};
    lua.new_enum<shake_trigger>("shake_trigger", {
        { "interval", static_cast<shake_trigger>(84) },
        { "strength", static_cast<shake_trigger>(75) },
    });
    enum class text_object {};
    lua.new_enum<text_object>("text_object", {
        { "text", static_cast<text_object>(31) },
        { "kerning", static_cast<text_object>(488) },
    });
    enum class follow_player_y_trigger {};
    lua.new_enum<follow_player_y_trigger>("follow_player_y_trigger", {
        { "delay", static_cast<follow_player_y_trigger>(91) },
        { "offset", static_cast<follow_player_y_trigger>(92) },
        { "max_speed", static_cast<follow_player_y_trigger>(105) },
        { "target_id", static_cast<follow_player_y_trigger>(51) },
        { "speed", static_cast<follow_player_y_trigger>(90) },
    });
    enum class time_trigger {};
    lua.new_enum<time_trigger>("time_trigger", {
        { "stop_time", static_cast<time_trigger>(473) },
        { "start_paused", static_cast<time_trigger>(471) },
        { "target_id", static_cast<time_trigger>(51) },
        { "ignore_timewarp", static_cast<time_trigger>(469) },
        { "dont_override", static_cast<time_trigger>(468) },
        { "item_id", static_cast<time_trigger>(80) },
        { "time_mod", static_cast<time_trigger>(470) },
        { "stop_checked", static_cast<time_trigger>(474) },
        { "start_time", static_cast<time_trigger>(467) },
    });
    enum class player_control_trigger {};
    lua.new_enum<player_control_trigger>("player_control_trigger", {
        { "stop_jump", static_cast<player_control_trigger>(540) },
        { "stop_rot", static_cast<player_control_trigger>(542) },
        { "stop_move", static_cast<player_control_trigger>(541) },
        { "p1", static_cast<player_control_trigger>(138) },
        { "stop_slide", static_cast<player_control_trigger>(543) },
        { "p2", static_cast<player_control_trigger>(200) },
    });
    enum class count_trigger {};
    lua.new_enum<count_trigger>("count_trigger", {
        { "multi_activate", static_cast<count_trigger>(104) },
        { "target_count", static_cast<count_trigger>(77) },
        { "item_id", static_cast<count_trigger>(80) },
        { "target_id", static_cast<count_trigger>(51) },
        { "activate_group", static_cast<count_trigger>(56) },
    });
    enum class rotating_objects {};
    lua.new_enum<rotating_objects>("rotating_objects", {
        { "rotation_speed", static_cast<rotating_objects>(97) },
        { "disable_rotation", static_cast<rotating_objects>(98) },
    });
    enum class sequence_trigger {};
    lua.new_enum<sequence_trigger>("sequence_trigger", {
        { "reset", static_cast<sequence_trigger>(438) },
        { "mode", static_cast<sequence_trigger>(438) },
        { "sequence", static_cast<sequence_trigger>(435) },
        { "reset_full_step", static_cast<sequence_trigger>(439) },
        { "min_int", static_cast<sequence_trigger>(437) },
    });
    enum class background_trigger {};
    lua.new_enum<background_trigger>("background_trigger", {
        { "background", static_cast<background_trigger>(533) },
    });
    enum class sfx_trigger {};
    lua.new_enum<sfx_trigger>("sfx_trigger", {
        { "start", static_cast<sfx_trigger>(408) },
        { "reverb", static_cast<sfx_trigger>(407) },
        { "speed", static_cast<sfx_trigger>(404) },
        { "sfx_group", static_cast<sfx_trigger>(455) },
        { "sound_duration", static_cast<sfx_trigger>(490) },
        { "dist_2", static_cast<sfx_trigger>(425) },
        { "vol_far", static_cast<sfx_trigger>(423) },
        { "fade_in", static_cast<sfx_trigger>(409) },
        { "fade_out", static_cast<sfx_trigger>(411) },
        { "is_unique", static_cast<sfx_trigger>(415) },
        { "loop", static_cast<sfx_trigger>(413) },
        { "group_id_2", static_cast<sfx_trigger>(71) },
        { "p2", static_cast<sfx_trigger>(200) },
        { "volume", static_cast<sfx_trigger>(406) },
        { "sfx_id", static_cast<sfx_trigger>(392) },
        { "pre_load", static_cast<sfx_trigger>(433) },
        { "vol_med", static_cast<sfx_trigger>(422) },
        { "ignore_volume_test", static_cast<sfx_trigger>(489) },
        { "unique_id", static_cast<sfx_trigger>(416) },
        { "vol_near", static_cast<sfx_trigger>(421) },
        { "reverb_type", static_cast<sfx_trigger>(502) },
        { "min_dist", static_cast<sfx_trigger>(424) },
        { "dir_buttons", static_cast<sfx_trigger>(458) },
        { "dist_3", static_cast<sfx_trigger>(426) },
        { "cam", static_cast<sfx_trigger>(428) },
        { "pitch", static_cast<sfx_trigger>(405) },
        { "end_", static_cast<sfx_trigger>(410) },
        { "reverb_enable", static_cast<sfx_trigger>(503) },
        { "override", static_cast<sfx_trigger>(420) },
        { "p1", static_cast<sfx_trigger>(138) },
        { "fft", static_cast<sfx_trigger>(412) },
        { "group_id_1", static_cast<sfx_trigger>(51) },
        { "min_interval", static_cast<sfx_trigger>(434) },
    });
    enum class gravity_trigger {};
    lua.new_enum<gravity_trigger>("gravity_trigger", {
        { "p1", static_cast<gravity_trigger>(138) },
        { "pt", static_cast<gravity_trigger>(201) },
        { "gravity", static_cast<gravity_trigger>(148) },
        { "p2", static_cast<gravity_trigger>(200) },
    });
    enum class scale_trigger {};
    lua.new_enum<scale_trigger>("scale_trigger", {
        { "center_group", static_cast<scale_trigger>(71) },
        { "target_group", static_cast<scale_trigger>(51) },
        { "only_move", static_cast<scale_trigger>(133) },
        { "relative_scale", static_cast<scale_trigger>(577) },
        { "relative_rot", static_cast<scale_trigger>(452) },
        { "div_x", static_cast<scale_trigger>(153) },
        { "div_y", static_cast<scale_trigger>(154) },
        { "scale_x", static_cast<scale_trigger>(150) },
        { "scale_y", static_cast<scale_trigger>(151) },
    });
    enum class edit_area_fade_trigger {};
    lua.new_enum<edit_area_fade_trigger>("edit_area_fade_trigger", {
        { "easing_rate", static_cast<edit_area_fade_trigger>(243) },
        { "easing", static_cast<edit_area_fade_trigger>(242) },
        { "offset", static_cast<edit_area_fade_trigger>(220) },
        { "to_opacity", static_cast<edit_area_fade_trigger>(275) },
        { "group_effect_id", static_cast<edit_area_fade_trigger>(51) },
        { "mod_back", static_cast<edit_area_fade_trigger>(264) },
        { "deadzone", static_cast<edit_area_fade_trigger>(282) },
        { "mod_front", static_cast<edit_area_fade_trigger>(263) },
        { "offset_y_rand", static_cast<edit_area_fade_trigger>(253) },
        { "length_rand", static_cast<edit_area_fade_trigger>(223) },
        { "length", static_cast<edit_area_fade_trigger>(222) },
        { "use_eid", static_cast<edit_area_fade_trigger>(355) },
        { "offset_y", static_cast<edit_area_fade_trigger>(252) },
        { "offset_rand", static_cast<edit_area_fade_trigger>(221) },
    });
    enum class area_move_trigger {};
    lua.new_enum<area_move_trigger>("area_move_trigger", {
        { "move_dist", static_cast<area_move_trigger>(218) },
        { "easing", static_cast<area_move_trigger>(242) },
        { "target_id", static_cast<area_move_trigger>(51) },
        { "ease_out", static_cast<area_move_trigger>(261) },
        { "priority", static_cast<area_move_trigger>(341) },
        { "offset_rand", static_cast<area_move_trigger>(221) },
        { "easing_rate", static_cast<area_move_trigger>(243) },
        { "movex_rand", static_cast<area_move_trigger>(238) },
        { "xy_mode", static_cast<area_move_trigger>(241) },
        { "mod_front", static_cast<area_move_trigger>(263) },
        { "move_angle", static_cast<area_move_trigger>(231) },
        { "de_ap", static_cast<area_move_trigger>(539) },
        { "ignore_linked", static_cast<area_move_trigger>(281) },
        { "relative", static_cast<area_move_trigger>(287) },
        { "move_dist_rand", static_cast<area_move_trigger>(219) },
        { "offset", static_cast<area_move_trigger>(220) },
        { "rfade", static_cast<area_move_trigger>(288) },
        { "easing2_rate", static_cast<area_move_trigger>(249) },
        { "deadzone", static_cast<area_move_trigger>(282) },
        { "offset_y_rand", static_cast<area_move_trigger>(253) },
        { "length_rand", static_cast<area_move_trigger>(223) },
        { "dir_buttons_inwards", static_cast<area_move_trigger>(276) },
        { "offset_y", static_cast<area_move_trigger>(252) },
        { "movey", static_cast<area_move_trigger>(239) },
        { "easing2", static_cast<area_move_trigger>(248) },
        { "movex", static_cast<area_move_trigger>(237) },
        { "mod_back", static_cast<area_move_trigger>(264) },
        { "move_angle_rand", static_cast<area_move_trigger>(232) },
        { "dir_buttons_dir", static_cast<area_move_trigger>(262) },
        { "length", static_cast<area_move_trigger>(222) },
        { "movey_rand", static_cast<area_move_trigger>(240) },
        { "dir_buttons_single_arrow", static_cast<area_move_trigger>(283) },
        { "ignore_gparent", static_cast<area_move_trigger>(280) },
        { "center_id", static_cast<area_move_trigger>(71) },
        { "center_p1_p2_c__tr", static_cast<area_move_trigger>(538) },
        { "effect_id", static_cast<area_move_trigger>(225) },
    });
    enum class collision_state_block {};
    lua.new_enum<collision_state_block>("collision_state_block", {
        { "state_off", static_cast<collision_state_block>(71) },
        { "state_on", static_cast<collision_state_block>(51) },
    });
    enum class area_scale_trigger {};
    lua.new_enum<area_scale_trigger>("area_scale_trigger", {
        { "easing", static_cast<area_scale_trigger>(242) },
        { "offset", static_cast<area_scale_trigger>(220) },
        { "scale_x_rand", static_cast<area_scale_trigger>(234) },
        { "target_id", static_cast<area_scale_trigger>(51) },
        { "easing2_rate", static_cast<area_scale_trigger>(249) },
        { "deadzone", static_cast<area_scale_trigger>(282) },
        { "ease_out", static_cast<area_scale_trigger>(261) },
        { "priority", static_cast<area_scale_trigger>(341) },
        { "offset_y_rand", static_cast<area_scale_trigger>(253) },
        { "length_rand", static_cast<area_scale_trigger>(223) },
        { "dir_buttons_inwards", static_cast<area_scale_trigger>(276) },
        { "scale_y_rand", static_cast<area_scale_trigger>(236) },
        { "offset_rand", static_cast<area_scale_trigger>(221) },
        { "easing_rate", static_cast<area_scale_trigger>(243) },
        { "scale_y", static_cast<area_scale_trigger>(235) },
        { "scale_x", static_cast<area_scale_trigger>(233) },
        { "center_p1_p2_c__tr", static_cast<area_scale_trigger>(538) },
        { "center_id", static_cast<area_scale_trigger>(71) },
        { "offset_y", static_cast<area_scale_trigger>(252) },
        { "mod_back", static_cast<area_scale_trigger>(264) },
        { "ignore_gparent", static_cast<area_scale_trigger>(280) },
        { "dir_buttons_single_arrow", static_cast<area_scale_trigger>(283) },
        { "mod_front", static_cast<area_scale_trigger>(263) },
        { "de_ap", static_cast<area_scale_trigger>(539) },
        { "easing2", static_cast<area_scale_trigger>(248) },
        { "length", static_cast<area_scale_trigger>(222) },
        { "ignore_linked", static_cast<area_scale_trigger>(281) },
        { "dir_buttons_dir", static_cast<area_scale_trigger>(262) },
        { "effect_id", static_cast<area_scale_trigger>(225) },
    });
    enum class instant_collision_trigger {};
    lua.new_enum<instant_collision_trigger>("instant_collision_trigger", {
        { "p2", static_cast<instant_collision_trigger>(200) },
        { "block_a_id", static_cast<instant_collision_trigger>(80) },
        { "block_b_id", static_cast<instant_collision_trigger>(95) },
        { "false_id", static_cast<instant_collision_trigger>(71) },
        { "pp", static_cast<instant_collision_trigger>(201) },
        { "p1", static_cast<instant_collision_trigger>(138) },
        { "true_id", static_cast<instant_collision_trigger>(51) },
    });
    enum class item_compare_trigger {};
    lua.new_enum<item_compare_trigger>("item_compare_trigger", {
        { "typ_2", static_cast<item_compare_trigger>(477) },
        { "item_id_2", static_cast<item_compare_trigger>(95) },
        { "op2", static_cast<item_compare_trigger>(481) },
        { "typ_1", static_cast<item_compare_trigger>(476) },
        { "rnd_flr_cei_2", static_cast<item_compare_trigger>(486) },
        { "rnd_flr_cei_1", static_cast<item_compare_trigger>(485) },
        { "abs_neg_2", static_cast<item_compare_trigger>(579) },
        { "abs_neg_1", static_cast<item_compare_trigger>(578) },
        { "mod1", static_cast<item_compare_trigger>(479) },
        { "op1", static_cast<item_compare_trigger>(480) },
        { "compare_op", static_cast<item_compare_trigger>(482) },
        { "item_id_1", static_cast<item_compare_trigger>(80) },
        { "false_id", static_cast<item_compare_trigger>(71) },
        { "tol_rand", static_cast<item_compare_trigger>(484) },
        { "mod2", static_cast<item_compare_trigger>(483) },
        { "true_id", static_cast<item_compare_trigger>(51) },
    });
    enum class advanced_random_trigger {};
    lua.new_enum<advanced_random_trigger>("advanced_random_trigger", {
        { "list", static_cast<advanced_random_trigger>(152) },
    });
    enum class options_trigger {};
    lua.new_enum<options_trigger>("options_trigger", {
        { "disable_p1_controls", static_cast<options_trigger>(165) },
        { "no_death_sfx", static_cast<options_trigger>(576) },
        { "hide_ground", static_cast<options_trigger>(161) },
        { "respawn_time", static_cast<options_trigger>(574) },
        { "edit_respawn_time", static_cast<options_trigger>(573) },
        { "hide_p1", static_cast<options_trigger>(162) },
        { "audio_on_death", static_cast<options_trigger>(575) },
        { "disable_p2_controls", static_cast<options_trigger>(199) },
        { "hide_attempts", static_cast<options_trigger>(532) },
        { "hide_mg", static_cast<options_trigger>(195) },
        { "unlink_dual_gravity", static_cast<options_trigger>(160) },
        { "streak_additive", static_cast<options_trigger>(159) },
        { "hide_p2", static_cast<options_trigger>(163) },
    });
    enum class toggle_trigger {};
    lua.new_enum<toggle_trigger>("toggle_trigger", {
        { "group_id", static_cast<toggle_trigger>(51) },
        { "activate_group", static_cast<toggle_trigger>(56) },
    });
    enum class gradient_trigger {};
    lua.new_enum<gradient_trigger>("gradient_trigger", {
        { "layer", static_cast<gradient_trigger>(202) },
        { "disable", static_cast<gradient_trigger>(208) },
        { "blending", static_cast<gradient_trigger>(174) },
        { "u_bl", static_cast<gradient_trigger>(203) },
        { "preview_opacity", static_cast<gradient_trigger>(456) },
        { "d_br", static_cast<gradient_trigger>(204) },
        { "l_tl", static_cast<gradient_trigger>(205) },
        { "r_tr", static_cast<gradient_trigger>(206) },
        { "id", static_cast<gradient_trigger>(209) },
        { "disable_all", static_cast<gradient_trigger>(508) },
        { "vertex_mode", static_cast<gradient_trigger>(207) },
    });
    enum class touch_trigger {};
    lua.new_enum<touch_trigger>("touch_trigger", {
        { "dual_mode", static_cast<touch_trigger>(89) },
        { "toggle_on_off", static_cast<touch_trigger>(82) },
        { "group_id", static_cast<touch_trigger>(51) },
        { "hold_mode", static_cast<touch_trigger>(81) },
        { "p1_p2_only", static_cast<touch_trigger>(198) },
    });
    enum class edit_area_scale_trigger {};
    lua.new_enum<edit_area_scale_trigger>("edit_area_scale_trigger", {
        { "easing", static_cast<edit_area_scale_trigger>(242) },
        { "offset", static_cast<edit_area_scale_trigger>(220) },
        { "scale_x_rand", static_cast<edit_area_scale_trigger>(234) },
        { "group_effect_id", static_cast<edit_area_scale_trigger>(51) },
        { "deadzone", static_cast<edit_area_scale_trigger>(282) },
        { "offset_y_rand", static_cast<edit_area_scale_trigger>(253) },
        { "length_rand", static_cast<edit_area_scale_trigger>(223) },
        { "scale_y", static_cast<edit_area_scale_trigger>(235) },
        { "easing_rate", static_cast<edit_area_scale_trigger>(243) },
        { "mod_back", static_cast<edit_area_scale_trigger>(264) },
        { "mod_front", static_cast<edit_area_scale_trigger>(263) },
        { "scale_y_rand", static_cast<edit_area_scale_trigger>(236) },
        { "offset_rand", static_cast<edit_area_scale_trigger>(221) },
        { "length", static_cast<edit_area_scale_trigger>(222) },
        { "offset_y", static_cast<edit_area_scale_trigger>(252) },
        { "use_eid", static_cast<edit_area_scale_trigger>(355) },
        { "scale_x", static_cast<edit_area_scale_trigger>(233) },
    });
    enum class dash_orb {};
    lua.new_enum<dash_orb>("dash_orb", {
        { "allow_collide", static_cast<dash_orb>(587) },
        { "speed", static_cast<dash_orb>(586) },
        { "stop_slide", static_cast<dash_orb>(589) },
        { "end_boost", static_cast<dash_orb>(588) },
        { "max_duration", static_cast<dash_orb>(590) },
    });
    enum class camera_offset_trigger {};
    lua.new_enum<camera_offset_trigger>("camera_offset_trigger", {
        { "x_y_only", static_cast<camera_offset_trigger>(101) },
        { "offsetx", static_cast<camera_offset_trigger>(28) },
        { "offsety", static_cast<camera_offset_trigger>(29) },
    });
    enum class advanced_follow_trigger {};
    lua.new_enum<advanced_follow_trigger>("advanced_follow_trigger", {
        { "steer_force", static_cast<advanced_follow_trigger>(316) },
        { "easing", static_cast<advanced_follow_trigger>(361) },
        { "break_force_rand", static_cast<advanced_follow_trigger>(327) },
        { "near_friction", static_cast<advanced_follow_trigger>(561) },
        { "delay_rand", static_cast<advanced_follow_trigger>(293) },
        { "target_id", static_cast<advanced_follow_trigger>(51) },
        { "steer_force_low", static_cast<advanced_follow_trigger>(318) },
        { "target_dir", static_cast<advanced_follow_trigger>(305) },
        { "break_steer_speed_limit_rand", static_cast<advanced_follow_trigger>(333) },
        { "start_dir_unmarked_", static_cast<advanced_follow_trigger>(565) },
        { "max_speed_rand", static_cast<advanced_follow_trigger>(299) },
        { "break_steer_speed_limit", static_cast<advanced_follow_trigger>(332) },
        { "follow_p2", static_cast<advanced_follow_trigger>(200) },
        { "init_set_add", static_cast<advanced_follow_trigger>(572) },
        { "steer_force_high_rand", static_cast<advanced_follow_trigger>(321) },
        { "start_dir_rand", static_cast<advanced_follow_trigger>(564) },
        { "page_2_1_2_3", static_cast<advanced_follow_trigger>(367) },
        { "ignore_disabled", static_cast<advanced_follow_trigger>(336) },
        { "rot_offset", static_cast<advanced_follow_trigger>(340) },
        { "friction", static_cast<advanced_follow_trigger>(558) },
        { "break_angle_rand", static_cast<advanced_follow_trigger>(329) },
        { "break_angle", static_cast<advanced_follow_trigger>(328) },
        { "follow_id", static_cast<advanced_follow_trigger>(71) },
        { "slow_dist_rand", static_cast<advanced_follow_trigger>(360) },
        { "near_accel", static_cast<advanced_follow_trigger>(357) },
        { "slow_dist", static_cast<advanced_follow_trigger>(359) },
        { "rot_easing", static_cast<advanced_follow_trigger>(363) },
        { "speed_range_high_rand", static_cast<advanced_follow_trigger>(325) },
        { "exclusive", static_cast<advanced_follow_trigger>(571) },
        { "speed_range_low_rand", static_cast<advanced_follow_trigger>(323) },
        { "steer_force_high_checkbox", static_cast<advanced_follow_trigger>(338) },
        { "max_range", static_cast<advanced_follow_trigger>(308) },
        { "max_range_rand", static_cast<advanced_follow_trigger>(309) },
        { "steer_force_high", static_cast<advanced_follow_trigger>(320) },
        { "speed_range_high", static_cast<advanced_follow_trigger>(324) },
        { "acceleration", static_cast<advanced_follow_trigger>(334) },
        { "speed_range_low", static_cast<advanced_follow_trigger>(322) },
        { "steer_force_low_checkbox", static_cast<advanced_follow_trigger>(337) },
        { "priority", static_cast<advanced_follow_trigger>(365) },
        { "break_steer_force", static_cast<advanced_follow_trigger>(330) },
        { "follow_p1", static_cast<advanced_follow_trigger>(138) },
        { "friction_rand", static_cast<advanced_follow_trigger>(559) },
        { "near_friction_rand", static_cast<advanced_follow_trigger>(562) },
        { "acceleration_rand", static_cast<advanced_follow_trigger>(335) },
        { "near_dist", static_cast<advanced_follow_trigger>(359) },
        { "near_accel_rand", static_cast<advanced_follow_trigger>(358) },
        { "near_dist_rand", static_cast<advanced_follow_trigger>(360) },
        { "rotate_dir", static_cast<advanced_follow_trigger>(339) },
        { "start_speed", static_cast<advanced_follow_trigger>(300) },
        { "start_speed_unmarked_", static_cast<advanced_follow_trigger>(560) },
        { "break_steer_force_rand", static_cast<advanced_follow_trigger>(331) },
        { "start_dir", static_cast<advanced_follow_trigger>(563) },
        { "max_range_unmarked_", static_cast<advanced_follow_trigger>(366) },
        { "steer_force_rand", static_cast<advanced_follow_trigger>(317) },
        { "follow_c", static_cast<advanced_follow_trigger>(201) },
        { "start_speed_rand", static_cast<advanced_follow_trigger>(301) },
        { "delay", static_cast<advanced_follow_trigger>(292) },
        { "break_force", static_cast<advanced_follow_trigger>(326) },
        { "easing_rand", static_cast<advanced_follow_trigger>(362) },
        { "max_speed", static_cast<advanced_follow_trigger>(298) },
        { "steer_force_low_rand", static_cast<advanced_follow_trigger>(319) },
        { "x_only", static_cast<advanced_follow_trigger>(306) },
        { "y_only", static_cast<advanced_follow_trigger>(307) },
        { "rot_deadz", static_cast<advanced_follow_trigger>(364) },
    });
    enum class rotate_trigger {};
    lua.new_enum<rotate_trigger>("rotate_trigger", {
        { "dynamic_mode", static_cast<rotate_trigger>(397) },
        { "center_group", static_cast<rotate_trigger>(71) },
        { "aim_mode", static_cast<rotate_trigger>(100) },
        { "target_group", static_cast<rotate_trigger>(51) },
        { "degrees", static_cast<rotate_trigger>(68) },
        { "aim_follow_mode_targetp2", static_cast<rotate_trigger>(200) },
        { "x360", static_cast<rotate_trigger>(69) },
        { "dynamic_mode_easing", static_cast<rotate_trigger>(403) },
        { "follow_mode", static_cast<rotate_trigger>(394) },
        { "rot_target_id", static_cast<rotate_trigger>(401) },
        { "maxx_id", static_cast<rotate_trigger>(518) },
        { "aim_follow_mode_targetp1", static_cast<rotate_trigger>(138) },
        { "rot_offset", static_cast<rotate_trigger>(402) },
        { "miny_id", static_cast<rotate_trigger>(517) },
        { "maxy_id", static_cast<rotate_trigger>(519) },
        { "lock_obj_rot", static_cast<rotate_trigger>(70) },
        { "minx_id", static_cast<rotate_trigger>(516) },
    });
    enum class collectible {};
    lua.new_enum<collectible>("collectible", {
        { "toggle_trigger", static_cast<collectible>(382) },
        { "group_id", static_cast<collectible>(51) },
        { "enable_group", static_cast<collectible>(56) },
        { "points", static_cast<collectible>(383) },
        { "particle", static_cast<collectible>(440) },
        { "pickup_item", static_cast<collectible>(381) },
        { "no_anim", static_cast<collectible>(463) },
        { "item_id", static_cast<collectible>(80) },
        { "sub_count", static_cast<collectible>(78) },
    });
    enum class camera_zoom_trigger {};
    lua.new_enum<camera_zoom_trigger>("camera_zoom_trigger", {
        { "zoom", static_cast<camera_zoom_trigger>(371) },
    });
    enum class on_death_trigger {};
    lua.new_enum<on_death_trigger>("on_death_trigger", {
        { "group_id", static_cast<on_death_trigger>(51) },
        { "activate_group", static_cast<on_death_trigger>(56) },
    });
    enum class animate_trigger {};
    lua.new_enum<animate_trigger>("animate_trigger", {
        { "animation_id", static_cast<animate_trigger>(76) },
        { "target_id", static_cast<animate_trigger>(51) },
    });
    enum class toggle_block {};
    lua.new_enum<toggle_block>("toggle_block", {
        { "claim_touch", static_cast<toggle_block>(445) },
        { "group_id", static_cast<toggle_block>(51) },
        { "activate_group", static_cast<toggle_block>(56) },
        { "spawn_only", static_cast<toggle_block>(504) },
        { "multi_activate", static_cast<toggle_block>(99) },
    });
    enum class collision_block {};
    lua.new_enum<collision_block>("collision_block", {
        { "block_id", static_cast<collision_block>(80) },
        { "dynamic_block", static_cast<collision_block>(94) },
    });
    enum class reset_trigger {};
    lua.new_enum<reset_trigger>("reset_trigger", {
        { "group_id", static_cast<reset_trigger>(51) },
    });
    enum class color_trigger {};
    lua.new_enum<color_trigger>("color_trigger", {
        { "red", static_cast<color_trigger>(7) },
        { "player_color_1", static_cast<color_trigger>(15) },
        { "opacity", static_cast<color_trigger>(35) },
        { "player_color_2", static_cast<color_trigger>(16) },
        { "blending", static_cast<color_trigger>(17) },
        { "blue", static_cast<color_trigger>(9) },
        { "target_color", static_cast<color_trigger>(23) },
        { "green", static_cast<color_trigger>(8) },
    });
    enum class alpha_trigger {};
    lua.new_enum<alpha_trigger>("alpha_trigger", {
        { "group_id", static_cast<alpha_trigger>(51) },
        { "opacity", static_cast<alpha_trigger>(35) },
    });
    enum class collision_trigger {};
    lua.new_enum<collision_trigger>("collision_trigger", {
        { "pp", static_cast<collision_trigger>(201) },
        { "target_id", static_cast<collision_trigger>(51) },
        { "block_a_id", static_cast<collision_trigger>(80) },
        { "block_b_id", static_cast<collision_trigger>(95) },
        { "p1", static_cast<collision_trigger>(138) },
        { "activate_group", static_cast<collision_trigger>(56) },
        { "trigger_on_exit", static_cast<collision_trigger>(93) },
        { "p2", static_cast<collision_trigger>(200) },
    });
    enum class stop_trigger {};
    lua.new_enum<stop_trigger>("stop_trigger", {
        { "use_control_id", static_cast<stop_trigger>(535) },
        { "target_id", static_cast<stop_trigger>(51) },
        { "stop_pause_resume", static_cast<stop_trigger>(580) },
    });
    enum class ui_trigger {};
    lua.new_enum<ui_trigger>("ui_trigger", {
        { "xref_pos", static_cast<ui_trigger>(385) },
        { "yref_relative", static_cast<ui_trigger>(388) },
        { "xref_relative", static_cast<ui_trigger>(387) },
        { "yref_pos", static_cast<ui_trigger>(386) },
        { "ui_target", static_cast<ui_trigger>(71) },
        { "group_id", static_cast<ui_trigger>(51) },
    });
    enum class follow_trigger {};
    lua.new_enum<follow_trigger>("follow_trigger", {
        { "follow_group", static_cast<follow_trigger>(71) },
        { "target_group", static_cast<follow_trigger>(51) },
        { "y_mod", static_cast<follow_trigger>(73) },
        { "x_mod", static_cast<follow_trigger>(72) },
    });
    enum class instant_count_trigger {};
    lua.new_enum<instant_count_trigger>("instant_count_trigger", {
        { "equals_larger_smaller", static_cast<instant_count_trigger>(88) },
        { "target_count", static_cast<instant_count_trigger>(77) },
        { "item_id", static_cast<instant_count_trigger>(80) },
        { "target_id", static_cast<instant_count_trigger>(51) },
        { "activate_group", static_cast<instant_count_trigger>(56) },
    });
    enum class timewarp_trigger {};
    lua.new_enum<timewarp_trigger>("timewarp_trigger", {
        { "time_mod", static_cast<timewarp_trigger>(120) },
    });
    enum class event_trigger {};
    lua.new_enum<event_trigger>("event_trigger", {
        { "extra_id", static_cast<event_trigger>(447) },
        { "events", static_cast<event_trigger>(430) },
        { "group_id", static_cast<event_trigger>(51) },
        { "extra_id_2", static_cast<event_trigger>(525) },
    });
    enum class rotate_gameplay___arrow_trigger {};
    lua.new_enum<rotate_gameplay___arrow_trigger>("rotate_gameplay___arrow_trigger", {
        { "instant_offset", static_cast<rotate_gameplay___arrow_trigger>(368) },
        { "target_channel", static_cast<rotate_gameplay___arrow_trigger>(173) },
        { "velocity_mod_x", static_cast<rotate_gameplay___arrow_trigger>(582) },
        { "edit_velocity", static_cast<rotate_gameplay___arrow_trigger>(169) },
        { "override_velocity", static_cast<rotate_gameplay___arrow_trigger>(584) },
        { "arrow_dir", static_cast<rotate_gameplay___arrow_trigger>(167) },
        { "velocity_mod_y", static_cast<rotate_gameplay___arrow_trigger>(583) },
        { "dont_slide", static_cast<rotate_gameplay___arrow_trigger>(585) },
        { "gravity_dir", static_cast<rotate_gameplay___arrow_trigger>(166) },
        { "change_channel", static_cast<rotate_gameplay___arrow_trigger>(171) },
        { "channel_only", static_cast<rotate_gameplay___arrow_trigger>(172) },
    });
    enum class middle_ground_trigger {};
    lua.new_enum<middle_ground_trigger>("middle_ground_trigger", {
        { "middle_ground", static_cast<middle_ground_trigger>(533) },
    });
    enum class keyframe_object {};
    lua.new_enum<keyframe_object>("keyframe_object", {
        { "anim_id_", static_cast<keyframe_object>(373) },
        { "group_id", static_cast<keyframe_object>(51) },
        { "line_opacity", static_cast<keyframe_object>(524) },
        { "cw_ccw", static_cast<keyframe_object>(536) },
        { "close_loop", static_cast<keyframe_object>(376) },
        { "time_even_dist", static_cast<keyframe_object>(379) },
        { "x360", static_cast<keyframe_object>(537) },
        { "ref_only", static_cast<keyframe_object>(375) },
        { "auto_layer", static_cast<keyframe_object>(459) },
        { "curve", static_cast<keyframe_object>(378) },
        { "spawn_delay", static_cast<keyframe_object>(557) },
        { "prox", static_cast<keyframe_object>(377) },
        { "spawn_gid", static_cast<keyframe_object>(71) },
        { "preview_art", static_cast<keyframe_object>(380) },
    });
    enum class song_trigger {};
    lua.new_enum<song_trigger>("song_trigger", {
        { "volume", static_cast<song_trigger>(406) },
        { "start", static_cast<song_trigger>(408) },
        { "fade_in", static_cast<song_trigger>(409) },
        { "fade_out", static_cast<song_trigger>(411) },
        { "song", static_cast<song_trigger>(392) },
        { "channel", static_cast<song_trigger>(432) },
        { "prep", static_cast<song_trigger>(399) },
        { "loop", static_cast<song_trigger>(413) },
        { "load_prep", static_cast<song_trigger>(400) },
        { "speed", static_cast<song_trigger>(404) },
        { "end_", static_cast<song_trigger>(410) },
    });
    enum class keyframe_anim_trigger {};
    lua.new_enum<keyframe_anim_trigger>("keyframe_anim_trigger", {
        { "rotation_mod", static_cast<keyframe_anim_trigger>(522) },
        { "position_x_mod", static_cast<keyframe_anim_trigger>(521) },
        { "scale_x_mod", static_cast<keyframe_anim_trigger>(523) },
        { "scale_y_mod", static_cast<keyframe_anim_trigger>(546) },
        { "parent_id", static_cast<keyframe_anim_trigger>(71) },
        { "position_y_mod", static_cast<keyframe_anim_trigger>(545) },
        { "animation_group_id", static_cast<keyframe_anim_trigger>(76) },
        { "time_mod", static_cast<keyframe_anim_trigger>(520) },
        { "target_id", static_cast<keyframe_anim_trigger>(51) },
    });
    enum class pickup_trigger {};
    lua.new_enum<pickup_trigger>("pickup_trigger", {
        { "multiply_divide", static_cast<pickup_trigger>(88) },
        { "count", static_cast<pickup_trigger>(77) },
        { "item_id", static_cast<pickup_trigger>(80) },
        { "modifier", static_cast<pickup_trigger>(449) },
        { "override", static_cast<pickup_trigger>(139) },
    });
    enum class time_control_trigger {};
    lua.new_enum<time_control_trigger>("time_control_trigger", {
        { "item_id", static_cast<time_control_trigger>(80) },
        { "start_stop", static_cast<time_control_trigger>(472) },
    });
    enum class edit_area_tint_trigger {};
    lua.new_enum<edit_area_tint_trigger>("edit_area_tint_trigger", {
        { "easing_rate", static_cast<edit_area_tint_trigger>(243) },
        { "easing", static_cast<edit_area_tint_trigger>(242) },
        { "offset", static_cast<edit_area_tint_trigger>(220) },
        { "hsv", static_cast<edit_area_tint_trigger>(49) },
        { "hsv_on", static_cast<edit_area_tint_trigger>(278) },
        { "group_effect_id", static_cast<edit_area_tint_trigger>(51) },
        { "mod_back", static_cast<edit_area_tint_trigger>(264) },
        { "deadzone", static_cast<edit_area_tint_trigger>(282) },
        { "tint", static_cast<edit_area_tint_trigger>(265) },
        { "mod_front", static_cast<edit_area_tint_trigger>(263) },
        { "offset_y_rand", static_cast<edit_area_tint_trigger>(253) },
        { "length_rand", static_cast<edit_area_tint_trigger>(223) },
        { "length", static_cast<edit_area_tint_trigger>(222) },
        { "use_eid", static_cast<edit_area_tint_trigger>(355) },
        { "offset_y", static_cast<edit_area_tint_trigger>(252) },
        { "offset_rand", static_cast<edit_area_tint_trigger>(221) },
    });
    enum class edit_advanced_follow_trigger {};
    lua.new_enum<edit_advanced_follow_trigger>("edit_advanced_follow_trigger", {
        { "speed_rand", static_cast<edit_advanced_follow_trigger>(301) },
        { "mod_x", static_cast<edit_advanced_follow_trigger>(566) },
        { "dir_unmarked_", static_cast<edit_advanced_follow_trigger>(565) },
        { "mod_x_rand", static_cast<edit_advanced_follow_trigger>(567) },
        { "target_id", static_cast<edit_advanced_follow_trigger>(51) },
        { "mod_y_rand", static_cast<edit_advanced_follow_trigger>(569) },
        { "speed_unmarked_", static_cast<edit_advanced_follow_trigger>(560) },
        { "mod_y", static_cast<edit_advanced_follow_trigger>(568) },
        { "redirect_dir", static_cast<edit_advanced_follow_trigger>(570) },
        { "speed", static_cast<edit_advanced_follow_trigger>(300) },
        { "target_control_id", static_cast<edit_advanced_follow_trigger>(535) },
        { "dir_rand", static_cast<edit_advanced_follow_trigger>(364) },
        { "x_only", static_cast<edit_advanced_follow_trigger>(306) },
        { "y_only", static_cast<edit_advanced_follow_trigger>(307) },
        { "dir", static_cast<edit_advanced_follow_trigger>(363) },
    });
    enum class random_trigger {};
    lua.new_enum<random_trigger>("random_trigger", {
        { "chance", static_cast<random_trigger>(10) },
        { "group_id_2", static_cast<random_trigger>(71) },
        { "group_id_1", static_cast<random_trigger>(51) },
    });
    enum class counter_display {};
    lua.new_enum<counter_display>("counter_display", {
        { "maintime_points_attempts", static_cast<counter_display>(390) },
        { "align", static_cast<counter_display>(391) },
        { "item_id", static_cast<counter_display>(80) },
        { "time_counter", static_cast<counter_display>(466) },
        { "seconds_only", static_cast<counter_display>(389) },
    });
    enum class move_trigger {};
    lua.new_enum<move_trigger>("move_trigger", {
        { "movex", static_cast<move_trigger>(28) },
        { "target_group", static_cast<move_trigger>(51) },
        { "small_step", static_cast<move_trigger>(393) },
        { "direction_mode", static_cast<move_trigger>(394) },
        { "target_mode", static_cast<move_trigger>(100) },
        { "mody", static_cast<move_trigger>(144) },
        { "lock_y_camera", static_cast<move_trigger>(142) },
        { "movey", static_cast<move_trigger>(29) },
        { "target_dir_mode_centergroup", static_cast<move_trigger>(395) },
        { "target_dir_mode_targetp1", static_cast<move_trigger>(138) },
        { "modx", static_cast<move_trigger>(143) },
        { "dynamic_mode", static_cast<move_trigger>(397) },
        { "target_dir_mode_targetp2", static_cast<move_trigger>(200) },
        { "direction_mode_distance", static_cast<move_trigger>(396) },
        { "lock_x_camera", static_cast<move_trigger>(141) },
        { "target_mode_x_y_only", static_cast<move_trigger>(101) },
        { "target_dir_mode_targetgroup", static_cast<move_trigger>(71) },
        { "silent", static_cast<move_trigger>(544) },
        { "lock_y_player", static_cast<move_trigger>(59) },
        { "lock_x_player", static_cast<move_trigger>(58) },
    });
    enum class area_rotate_trigger {};
    lua.new_enum<area_rotate_trigger>("area_rotate_trigger", {
        { "easing", static_cast<area_rotate_trigger>(242) },
        { "offset", static_cast<area_rotate_trigger>(220) },
        { "rotation", static_cast<area_rotate_trigger>(270) },
        { "rotation_rand", static_cast<area_rotate_trigger>(271) },
        { "target_id", static_cast<area_rotate_trigger>(51) },
        { "easing2_rate", static_cast<area_rotate_trigger>(249) },
        { "deadzone", static_cast<area_rotate_trigger>(282) },
        { "ease_out", static_cast<area_rotate_trigger>(261) },
        { "priority", static_cast<area_rotate_trigger>(341) },
        { "offset_y_rand", static_cast<area_rotate_trigger>(253) },
        { "length_rand", static_cast<area_rotate_trigger>(223) },
        { "dir_buttons_inwards", static_cast<area_rotate_trigger>(276) },
        { "offset_rand", static_cast<area_rotate_trigger>(221) },
        { "easing_rate", static_cast<area_rotate_trigger>(243) },
        { "center_p1_p2_c__tr", static_cast<area_rotate_trigger>(538) },
        { "center_id", static_cast<area_rotate_trigger>(71) },
        { "offset_y", static_cast<area_rotate_trigger>(252) },
        { "mod_back", static_cast<area_rotate_trigger>(264) },
        { "ignore_gparent", static_cast<area_rotate_trigger>(280) },
        { "dir_buttons_single_arrow", static_cast<area_rotate_trigger>(283) },
        { "mod_front", static_cast<area_rotate_trigger>(263) },
        { "de_ap", static_cast<area_rotate_trigger>(539) },
        { "easing2", static_cast<area_rotate_trigger>(248) },
        { "length", static_cast<area_rotate_trigger>(222) },
        { "ignore_linked", static_cast<area_rotate_trigger>(281) },
        { "dir_buttons_dir", static_cast<area_rotate_trigger>(262) },
        { "effect_id", static_cast<area_rotate_trigger>(225) },
    });
    enum class animated_objects {};
    lua.new_enum<animated_objects>("animated_objects", {
        { "disable_animshine", static_cast<animated_objects>(127) },
        { "disable_delayed_loop", static_cast<animated_objects>(126) },
        { "offset_anim", static_cast<animated_objects>(592) },
        { "single_frame", static_cast<animated_objects>(462) },
        { "speed", static_cast<animated_objects>(107) },
        { "use_speed", static_cast<animated_objects>(122) },
        { "randomize_start", static_cast<animated_objects>(106) },
        { "animate_on_trigger", static_cast<animated_objects>(123) },
        { "only_if_active", static_cast<animated_objects>(214) },
    });
    enum class time_event_trigger {};
    lua.new_enum<time_event_trigger>("time_event_trigger", {
        { "item_id", static_cast<time_event_trigger>(80) },
        { "target_id", static_cast<time_event_trigger>(51) },
        { "multi_activate", static_cast<time_event_trigger>(475) },
        { "target_time", static_cast<time_event_trigger>(473) },
    });
    enum class ground_trigger {};
    lua.new_enum<ground_trigger>("ground_trigger", {
        { "ground", static_cast<ground_trigger>(533) },
    });
    enum class trigger_common {};
    lua.new_enum<trigger_common>("trigger_common", {
        { "duration", static_cast<trigger_common>(10) },
        { "easing", static_cast<trigger_common>(30) },
        { "multi_triggered", static_cast<trigger_common>(87) },
        { "spawn_triggered", static_cast<trigger_common>(62) },
        { "touch_triggered", static_cast<trigger_common>(11) },
        { "easing_rate", static_cast<trigger_common>(85) },
    });
    enum class edit_area_move_trigger {};
    lua.new_enum<edit_area_move_trigger>("edit_area_move_trigger", {
        { "move_dist", static_cast<edit_area_move_trigger>(218) },
        { "easing", static_cast<edit_area_move_trigger>(242) },
        { "offset", static_cast<edit_area_move_trigger>(220) },
        { "group_effect_id", static_cast<edit_area_move_trigger>(51) },
        { "deadzone", static_cast<edit_area_move_trigger>(282) },
        { "offset_y_rand", static_cast<edit_area_move_trigger>(253) },
        { "length_rand", static_cast<edit_area_move_trigger>(223) },
        { "offset_rand", static_cast<edit_area_move_trigger>(221) },
        { "easing_rate", static_cast<edit_area_move_trigger>(243) },
        { "movex_rand", static_cast<edit_area_move_trigger>(238) },
        { "move_angle_rand", static_cast<edit_area_move_trigger>(232) },
        { "movey", static_cast<edit_area_move_trigger>(239) },
        { "xy_mode", static_cast<edit_area_move_trigger>(241) },
        { "mod_back", static_cast<edit_area_move_trigger>(264) },
        { "movex", static_cast<edit_area_move_trigger>(237) },
        { "rfade", static_cast<edit_area_move_trigger>(288) },
        { "mod_front", static_cast<edit_area_move_trigger>(263) },
        { "movey_rand", static_cast<edit_area_move_trigger>(240) },
        { "move_angle", static_cast<edit_area_move_trigger>(231) },
        { "length", static_cast<edit_area_move_trigger>(222) },
        { "offset_y", static_cast<edit_area_move_trigger>(252) },
        { "use_eid", static_cast<edit_area_move_trigger>(355) },
        { "move_dist_rand", static_cast<edit_area_move_trigger>(219) },
    });
    enum class item_edit_trigger {};
    lua.new_enum<item_edit_trigger>("item_edit_trigger", {
        { "typ_2", static_cast<item_edit_trigger>(477) },
        { "item_id_2", static_cast<item_edit_trigger>(95) },
        { "target_item_id", static_cast<item_edit_trigger>(51) },
        { "typ_1", static_cast<item_edit_trigger>(476) },
        { "assign_op", static_cast<item_edit_trigger>(480) },
        { "mod", static_cast<item_edit_trigger>(479) },
        { "rnd_flr_cei_2", static_cast<item_edit_trigger>(486) },
        { "op1", static_cast<item_edit_trigger>(481) },
        { "item_id_1", static_cast<item_edit_trigger>(80) },
        { "target_typ", static_cast<item_edit_trigger>(478) },
        { "abs_neg_2", static_cast<item_edit_trigger>(579) },
        { "abs_neg_1", static_cast<item_edit_trigger>(578) },
        { "op2", static_cast<item_edit_trigger>(482) },
        { "rnd_flr_cei_1", static_cast<item_edit_trigger>(485) },
    });
    enum class camera_guide {};
    lua.new_enum<camera_guide>("camera_guide", {
        { "offsetx", static_cast<camera_guide>(28) },
        { "preview_opacity", static_cast<camera_guide>(506) },
        { "zoom", static_cast<camera_guide>(371) },
        { "offsety", static_cast<camera_guide>(29) },
    });
    enum class misc {};
    lua.new_enum<misc>("misc", {
        { "orange_teleportation_portal_distance", static_cast<misc>(54) },
    });
    enum class custom_particles {};
    lua.new_enum<custom_particles>("custom_particles", {
        { "particle_data", static_cast<custom_particles>(145) },
        { "use_obj_color", static_cast<custom_particles>(146) },
        { "uniform_obj_color", static_cast<custom_particles>(147) },
        { "animate_active_only", static_cast<custom_particles>(214) },
        { "quick_start", static_cast<custom_particles>(211) },
        { "animate_on_trigger", static_cast<custom_particles>(123) },
    });
    enum class item_persistence_trigger {};
    lua.new_enum<item_persistence_trigger>("item_persistence_trigger", {
        { "target_all", static_cast<item_persistence_trigger>(492) },
        { "reset", static_cast<item_persistence_trigger>(493) },
        { "persistent", static_cast<item_persistence_trigger>(491) },
        { "item_id", static_cast<item_persistence_trigger>(80) },
        { "timer", static_cast<item_persistence_trigger>(494) },
    });
    enum class pulse_trigger {};
    lua.new_enum<pulse_trigger>("pulse_trigger", {
        { "main_only", static_cast<pulse_trigger>(65) },
        { "copy_color_id", static_cast<pulse_trigger>(50) },
        { "fade_in", static_cast<pulse_trigger>(45) },
        { "fade_out", static_cast<pulse_trigger>(47) },
        { "hsv_enabled", static_cast<pulse_trigger>(48) },
        { "green", static_cast<pulse_trigger>(8) },
        { "hsv", static_cast<pulse_trigger>(49) },
        { "hold", static_cast<pulse_trigger>(46) },
        { "blue", static_cast<pulse_trigger>(9) },
        { "red", static_cast<pulse_trigger>(7) },
        { "exclusive", static_cast<pulse_trigger>(86) },
        { "target_type", static_cast<pulse_trigger>(52) },
        { "target_id", static_cast<pulse_trigger>(51) },
        { "detail_only", static_cast<pulse_trigger>(66) },
    });
    enum class object_extra {};
    lua.new_enum<object_extra>("object_extra", {
        { "dont_boost_x", static_cast<object_extra>(509) },
        { "extended_collision", static_cast<object_extra>(511) },
        { "control_id", static_cast<object_extra>(534) },
        { "high_detail", static_cast<object_extra>(103) },
        { "don_t_fade", static_cast<object_extra>(64) },
        { "no_effects", static_cast<object_extra>(116) },
        { "enter_channel", static_cast<object_extra>(343) },
        { "single_ptouch", static_cast<object_extra>(284) },
        { "group_parent", static_cast<object_extra>(34) },
        { "scale_stick", static_cast<object_extra>(356) },
        { "material", static_cast<object_extra>(446) },
        { "hide", static_cast<object_extra>(135) },
        { "reverse", static_cast<object_extra>(117) },
        { "non_stick_y", static_cast<object_extra>(289) },
        { "passable", static_cast<object_extra>(134) },
        { "no_audio_scale", static_cast<object_extra>(372) },
        { "non_stick_x", static_cast<object_extra>(136) },
        { "no_glow", static_cast<object_extra>(96) },
        { "grip_slope", static_cast<object_extra>(193) },
        { "ice_block", static_cast<object_extra>(137) },
        { "dont_boost_y", static_cast<object_extra>(496) },
        { "area_parent", static_cast<object_extra>(279) },
        { "no_touch", static_cast<object_extra>(121) },
        { "extra_sticky", static_cast<object_extra>(495) },
        { "don_t_enter", static_cast<object_extra>(67) },
        { "center_effect", static_cast<object_extra>(369) },
        { "no_particle", static_cast<object_extra>(507) },
    });
    enum class camera_mode_trigger {};
    lua.new_enum<camera_mode_trigger>("camera_mode_trigger", {
        { "disable_gridsnap", static_cast<camera_mode_trigger>(370) },
        { "easing", static_cast<camera_mode_trigger>(113) },
        { "edit_camera_settings", static_cast<camera_mode_trigger>(112) },
        { "free_mode", static_cast<camera_mode_trigger>(111) },
        { "padding", static_cast<camera_mode_trigger>(114) },
    });
    enum class retarget_advanced_follow_trigger {};
    lua.new_enum<retarget_advanced_follow_trigger>("retarget_advanced_follow_trigger", {
        { "follow_p1", static_cast<retarget_advanced_follow_trigger>(138) },
        { "follow_id", static_cast<retarget_advanced_follow_trigger>(71) },
        { "follow_p2", static_cast<retarget_advanced_follow_trigger>(200) },
        { "target_id", static_cast<retarget_advanced_follow_trigger>(51) },
        { "follow_c", static_cast<retarget_advanced_follow_trigger>(201) },
        { "target_control_id", static_cast<retarget_advanced_follow_trigger>(535) },
    });
    enum class area_fade_trigger {};
    lua.new_enum<area_fade_trigger>("area_fade_trigger", {
        { "dir_buttons_dir", static_cast<area_fade_trigger>(262) },
        { "offset", static_cast<area_fade_trigger>(220) },
        { "to_opacity", static_cast<area_fade_trigger>(275) },
        { "target_id", static_cast<area_fade_trigger>(51) },
        { "easing2_rate", static_cast<area_fade_trigger>(249) },
        { "deadzone", static_cast<area_fade_trigger>(282) },
        { "priority", static_cast<area_fade_trigger>(341) },
        { "offset_y_rand", static_cast<area_fade_trigger>(253) },
        { "length_rand", static_cast<area_fade_trigger>(223) },
        { "dir_buttons_inwards", static_cast<area_fade_trigger>(276) },
        { "offset_rand", static_cast<area_fade_trigger>(221) },
        { "offset_y", static_cast<area_fade_trigger>(252) },
        { "from_opacity", static_cast<area_fade_trigger>(286) },
        { "mod_back", static_cast<area_fade_trigger>(264) },
        { "center_p1_p2_c__tr", static_cast<area_fade_trigger>(538) },
        { "center_id", static_cast<area_fade_trigger>(71) },
        { "mod_front", static_cast<area_fade_trigger>(263) },
        { "ignore_gparent", static_cast<area_fade_trigger>(280) },
        { "dir_buttons_single_arrow", static_cast<area_fade_trigger>(283) },
        { "de_ap", static_cast<area_fade_trigger>(539) },
        { "ignore_linked", static_cast<area_fade_trigger>(281) },
        { "length", static_cast<area_fade_trigger>(222) },
        { "effect_id", static_cast<area_fade_trigger>(225) },
    });
    enum class camera_rotate_trigger {};
    lua.new_enum<camera_rotate_trigger>("camera_rotate_trigger", {
        { "add", static_cast<camera_rotate_trigger>(70) },
        { "degrees", static_cast<camera_rotate_trigger>(68) },
        { "snap360", static_cast<camera_rotate_trigger>(394) },
    });
    enum class static_camera_trigger {};
    lua.new_enum<static_camera_trigger>("static_camera_trigger", {
        { "exit_instant", static_cast<static_camera_trigger>(465) },
        { "target_group_id", static_cast<static_camera_trigger>(71) },
        { "exit_static", static_cast<static_camera_trigger>(110) },
        { "smooth_velocity_modifier", static_cast<static_camera_trigger>(454) },
        { "follow_easing", static_cast<static_camera_trigger>(213) },
        { "follow", static_cast<static_camera_trigger>(212) },
        { "smooth_velocity", static_cast<static_camera_trigger>(453) },
    });
    enum class spawn_trigger {};
    lua.new_enum<spawn_trigger>("spawn_trigger", {
        { "delay", static_cast<spawn_trigger>(63) },
        { "spawn_ordered", static_cast<spawn_trigger>(441) },
        { "preview_disable", static_cast<spawn_trigger>(102) },
        { "delay_rand", static_cast<spawn_trigger>(556) },
        { "target_id", static_cast<spawn_trigger>(51) },
        { "group_remaps", static_cast<spawn_trigger>(442) },
        { "reset_remap", static_cast<spawn_trigger>(581) },
    });
    enum class camera_edge_trigger {};
    lua.new_enum<camera_edge_trigger>("camera_edge_trigger", {
        { "target_id", static_cast<camera_edge_trigger>(51) },
        { "edge", static_cast<camera_edge_trigger>(164) },
    });
    enum class object_common {};
    lua.new_enum<object_common>("object_common", {
        { "rotation", static_cast<object_common>(6) },
        { "ord", static_cast<object_common>(115) },
        { "color_2_hsv", static_cast<object_common>(44) },
        { "channel", static_cast<object_common>(170) },
        { "warp_y_angle", static_cast<object_common>(131) },
        { "editor_layer_2", static_cast<object_common>(61) },
        { "edp", static_cast<object_common>(13) },
        { "linked_group", static_cast<object_common>(108) },
        { "warp_x_angle", static_cast<object_common>(132) },
        { "flip_horiz", static_cast<object_common>(4) },
        { "pink_groups", static_cast<object_common>(274) },
        { "scale_y", static_cast<object_common>(129) },
        { "editor_layer", static_cast<object_common>(20) },
        { "z_order", static_cast<object_common>(25) },
        { "color_1_hsv", static_cast<object_common>(43) },
        { "scale", static_cast<object_common>(32) },
        { "z_layer", static_cast<object_common>(24) },
        { "color_2", static_cast<object_common>(22) },
        { "groups", static_cast<object_common>(57) },
        { "id", static_cast<object_common>(1) },
        { "flip_vert", static_cast<object_common>(5) },
        { "scale_x", static_cast<object_common>(128) },
        { "x", static_cast<object_common>(2) },
        { "y", static_cast<object_common>(3) },
        { "single_color_type", static_cast<object_common>(497) },
        { "color_1", static_cast<object_common>(21) },
        { "color_1_hsv_enabled", static_cast<object_common>(41) },
        { "color_2_hsv_enabled", static_cast<object_common>(42) },
    });
    enum class area_tint_trigger {};
    lua.new_enum<area_tint_trigger>("area_tint_trigger", {
        { "main_only", static_cast<area_tint_trigger>(65) },
        { "dir_buttons_dir", static_cast<area_tint_trigger>(262) },
        { "offset", static_cast<area_tint_trigger>(220) },
        { "target_id", static_cast<area_tint_trigger>(51) },
        { "easing2_rate", static_cast<area_tint_trigger>(249) },
        { "deadzone", static_cast<area_tint_trigger>(282) },
        { "priority", static_cast<area_tint_trigger>(341) },
        { "offset_y_rand", static_cast<area_tint_trigger>(253) },
        { "length_rand", static_cast<area_tint_trigger>(223) },
        { "dir_buttons_inwards", static_cast<area_tint_trigger>(276) },
        { "offset_rand", static_cast<area_tint_trigger>(221) },
        { "offset_y", static_cast<area_tint_trigger>(252) },
        { "secondary_only", static_cast<area_tint_trigger>(66) },
        { "color_channel", static_cast<area_tint_trigger>(260) },
        { "tint", static_cast<area_tint_trigger>(265) },
        { "hsv_on", static_cast<area_tint_trigger>(278) },
        { "hsv", static_cast<area_tint_trigger>(49) },
        { "mod_back", static_cast<area_tint_trigger>(264) },
        { "center_p1_p2_c__tr", static_cast<area_tint_trigger>(538) },
        { "center_id", static_cast<area_tint_trigger>(71) },
        { "mod_front", static_cast<area_tint_trigger>(263) },
        { "ignore_gparent", static_cast<area_tint_trigger>(280) },
        { "dir_buttons_single_arrow", static_cast<area_tint_trigger>(283) },
        { "de_ap", static_cast<area_tint_trigger>(539) },
        { "ignore_linked", static_cast<area_tint_trigger>(281) },
        { "length", static_cast<area_tint_trigger>(222) },
        { "effect_id", static_cast<area_tint_trigger>(225) },
    });
    enum class spawn_particle_trigger {};
    lua.new_enum<spawn_particle_trigger>("spawn_particle_trigger", {
        { "position_group", static_cast<spawn_particle_trigger>(71) },
        { "scale_rand", static_cast<spawn_particle_trigger>(555) },
        { "rotation", static_cast<spawn_particle_trigger>(552) },
        { "rotation_rand", static_cast<spawn_particle_trigger>(553) },
        { "particle_group", static_cast<spawn_particle_trigger>(51) },
        { "scale", static_cast<spawn_particle_trigger>(554) },
        { "offvar_x", static_cast<spawn_particle_trigger>(549) },
        { "offset_y", static_cast<spawn_particle_trigger>(548) },
        { "match_rot", static_cast<spawn_particle_trigger>(551) },
        { "offvar_y", static_cast<spawn_particle_trigger>(550) },
        { "offset_x", static_cast<spawn_particle_trigger>(547) },
    });
    enum class stop_area_trigger {};
    lua.new_enum<stop_area_trigger>("stop_area_trigger", {
        { "effect_id", static_cast<stop_area_trigger>(51) },
    });
}
