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

std::vector<sol::table> gd::getPropertyTable(sol::state& lua)
{
    constexpr const char* jsonstr = R"JSON({"follow_player_y_trigger":{"delay":91,"speed":90,"offset":92,"max_speed":105,"target_id":51},"counter_display":{"maintime_points_attempts":390,"item_id":80,"seconds_only":389,"align":391,"time_counter":466},"keyframe_object":{"curve":378,"prox":377,"preview_art":380,"time_even_dist":379,"auto_layer":459,"close_loop":376,"x360":537,"anim_id":373,"line_opacity":524,"ref_only":375,"spawn_delay":557,"spawn_gid":71,"group_id":51,"cw_ccw":536},"shake_trigger":{"interval":84,"strength":75},"text_object":{"kerning":488,"text":31},"item_compare_trigger":{"item_id_1":80,"abs_neg_1":578,"rnd_flr_cei_2":486,"true_id":51,"typ_1":476,"op1":480,"typ_2":477,"abs_neg_2":579,"item_id_2":95,"rnd_flr_cei_1":485,"false_id":71,"mod1":479,"mod2":483,"compare_op":482,"op2":481,"tol_rand":484},"edit_advanced_follow_trigger":{"dir":363,"y_only":307,"mod_x":566,"redirect_dir":570,"mod_x_rand":567,"target_control_id":535,"dir_unmarked":565,"dir_rand":364,"target_id":51,"speed_unmarked":560,"speed_rand":301,"speed":300,"x_only":306,"mod_y":568,"mod_y_rand":569},"touch_trigger":{"dual_mode":89,"toggle_on_off":82,"group_id":51,"p1_p2_only":198,"hold_mode":81},"random_trigger":{"chance":10,"group_id_1":51,"group_id_2":71},"alpha_trigger":{"group_id":51,"opacity":35},"keyframe_anim_trigger":{"position_y_mod":545,"position_x_mod":521,"rotation_mod":522,"time_mod":520,"target_id":51,"scale_y_mod":546,"animation_group_id":76,"parent_id":71,"scale_x_mod":523},"instant_collision_trigger":{"p2":200,"p1":138,"false_id":71,"true_id":51,"block_a_id":80,"block_b_id":95,"pp":201},"sfx_trigger":{"min_interval":434,"reverb_type":502,"vol_far":423,"end_":410,"group_id_1":51,"reverb":407,"speed":404,"p2":200,"cam":428,"is_unique":415,"fade_in":409,"sfx_id":392,"loop":413,"fade_out":411,"group_id_2":71,"reverb_enable":503,"start":408,"pre_load":433,"vol_near":421,"dist_2":425,"min_dist":424,"dist_3":426,"sfx_group":455,"p1":138,"pitch":405,"unique_id":416,"volume":406,"ignore_volume_test":489,"fft":412,"dir_buttons":458,"override":420,"vol_med":422,"sound_duration":490},"dash_orb":{"stop_slide":589,"max_duration":590,"speed":586,"allow_collide":587,"end_boost":588},"spawn_trigger":{"target_id":51,"preview_disable":102,"delay_rand":556,"spawn_ordered":441,"group_remaps":442,"reset_remap":581,"delay":63},"retarget_advanced_follow_trigger":{"follow_p2":200,"target_id":51,"target_control_id":535,"follow_id":71,"follow_c":201,"follow_p1":138},"item_edit_trigger":{"item_id_1":80,"mod":479,"abs_neg_2":579,"typ_1":476,"op1":481,"assign_op":480,"item_id_2":95,"rnd_flr_cei_1":485,"rnd_flr_cei_2":486,"abs_neg_1":578,"op2":482,"target_typ":478,"typ_2":477,"target_item_id":51},"advanced_random_trigger":{"list":152},"static_camera_trigger":{"smooth_velocity_modifier":454,"exit_static":110,"smooth_velocity":453,"follow_easing":213,"follow":212,"exit_instant":465,"target_group_id":71},"count_trigger":{"activate_group":56,"multi_activate":104,"item_id":80,"target_count":77,"target_id":51},"rotating_objects":{"rotation_speed":97,"disable_rotation":98},"background_trigger":{"background":533},"pickup_trigger":{"override":139,"item_id":80,"modifier":449,"count":77,"multiply_divide":88},"rotate_trigger":{"dynamic_mode_easing":403,"maxx_id":518,"center_group":71,"rot_target_id":401,"degrees":68,"follow_mode":394,"rot_offset":402,"aim_mode":100,"aim_follow_mode_targetp1":138,"maxy_id":519,"x360":69,"target_group":51,"dynamic_mode":397,"miny_id":517,"lock_obj_rot":70,"aim_follow_mode_targetp2":200,"minx_id":516},"custom_particles":{"particle_data":145,"use_obj_color":146,"animate_on_trigger":123,"uniform_obj_color":147,"quick_start":211,"animate_active_only":214},"camera_guide":{"zoom":371,"preview_opacity":506,"offsety":29,"offsetx":28},"edit_area_move_trigger":{"offset_y":252,"easing_rate":243,"movey":239,"move_dist_rand":219,"movey_rand":240,"move_angle":231,"offset_y_rand":253,"move_dist":218,"group_effect_id":51,"deadzone":282,"use_eid":355,"move_angle_rand":232,"offset":220,"length":222,"movex_rand":238,"movex":237,"xy_mode":241,"rfade":288,"length_rand":223,"easing":242,"mod_front":263,"mod_back":264,"offset_rand":221},"instant_count_trigger":{"activate_group":56,"equals_larger_smaller":88,"item_id":80,"target_count":77,"target_id":51},"time_control_trigger":{"start_stop":472,"item_id":80},"color_trigger":{"green":8,"player_color_2":16,"target_color":23,"blue":9,"blending":17,"opacity":35,"red":7,"player_color_1":15},"event_trigger":{"extra_id_2":525,"extra_id":447,"group_id":51,"events":430},"time_trigger":{"stop_time":473,"start_time":467,"time_mod":470,"ignore_timewarp":469,"start_paused":471,"stop_checked":474,"item_id":80,"dont_override":468,"target_id":51},"area_fade_trigger":{"offset_y":252,"ignore_linked":281,"offset_rand":221,"offset_y_rand":253,"from_opacity":286,"mod_back":264,"center_p1_p2_c__tr":538,"dir_buttons_inwards":276,"ignore_gparent":280,"easing2_rate":249,"priority":341,"target_id":51,"offset":220,"length":222,"dir_buttons_single_arrow":283,"to_opacity":275,"effect_id":225,"deadzone":282,"length_rand":223,"dir_buttons_dir":262,"mod_front":263,"center_id":71,"de_ap":539},"camera_rotate_trigger":{"degrees":68,"add":70,"snap360":394},"move_trigger":{"modx":143,"movey":29,"movex":28,"direction_mode_distance":396,"lock_y_player":59,"direction_mode":394,"mody":144,"target_mode_x_y_only":101,"target_group":51,"target_dir_mode_centergroup":395,"small_step":393,"lock_y_camera":142,"silent":544,"target_dir_mode_targetp2":200,"lock_x_camera":141,"lock_x_player":58,"target_dir_mode_targetgroup":71,"dynamic_mode":397,"target_dir_mode_targetp1":138,"target_mode":100},"options_trigger":{"disable_p2_controls":199,"unlink_dual_gravity":160,"hide_p1":162,"audio_on_death":575,"streak_additive":159,"hide_p2":163,"hide_ground":161,"hide_mg":195,"respawn_time":574,"edit_respawn_time":573,"no_death_sfx":576,"hide_attempts":532,"disable_p1_controls":165},"middle_ground_trigger":{"middle_ground":533},"rotate_gameplay___arrow_trigger":{"instant_offset":368,"target_channel":173,"dont_slide":585,"velocity_mod_y":583,"edit_velocity":169,"override_velocity":584,"velocity_mod_x":582,"gravity_dir":166,"arrow_dir":167,"change_channel":171,"channel_only":172},"trigger_common":{"easing_rate":85,"spawn_triggered":62,"multi_triggered":87,"duration":10,"easing":30,"touch_triggered":11},"pulse_trigger":{"copy_color_id":50,"blue":9,"target_type":52,"hsv":49,"red":7,"exclusive":86,"hsv_enabled":48,"target_id":51,"main_only":65,"hold":46,"green":8,"detail_only":66,"fade_in":45,"fade_out":47},"gradient_trigger":{"u_bl":203,"preview_opacity":456,"r_tr":206,"blending":174,"disable_all":508,"id":209,"d_br":204,"l_tl":205,"layer":202,"vertex_mode":207,"disable":208},"player_control_trigger":{"p2":200,"p1":138,"stop_slide":543,"stop_rot":542,"stop_jump":540,"stop_move":541},"gravity_trigger":{"p2":200,"p1":138,"gravity":148,"pt":201},"area_scale_trigger":{"offset_y":252,"easing_rate":243,"ignore_linked":281,"scale_y_rand":236,"dir_buttons_inwards":276,"offset_rand":221,"offset_y_rand":253,"mod_back":264,"scale_x":233,"center_p1_p2_c__tr":538,"de_ap":539,"center_id":71,"target_id":51,"dir_buttons_dir":262,"ignore_gparent":280,"easing2_rate":249,"priority":341,"easing2":248,"offset":220,"length":222,"dir_buttons_single_arrow":283,"easing":242,"effect_id":225,"deadzone":282,"length_rand":223,"scale_x_rand":234,"mod_front":263,"ease_out":261,"scale_y":235},"on_death_trigger":{"group_id":51,"activate_group":56},"toggle_block":{"claim_touch":445,"activate_group":56,"multi_activate":99,"group_id":51,"spawn_only":504},"collision_block":{"dynamic_block":94,"block_id":80},"collision_state_block":{"state_on":51,"state_off":71},"collision_trigger":{"p2":200,"target_id":51,"block_b_id":95,"activate_group":56,"block_a_id":80,"p1":138,"trigger_on_exit":93,"pp":201},"ui_trigger":{"yref_pos":386,"xref_pos":385,"ui_target":71,"yref_relative":388,"group_id":51,"xref_relative":387},"timewarp_trigger":{"time_mod":120},"stop_trigger":{"use_control_id":535,"stop_pause_resume":580,"target_id":51},"camera_edge_trigger":{"target_id":51,"edge":164},"spawn_particle_trigger":{"offset_y":548,"scale_rand":555,"rotation_rand":553,"offset_x":547,"rotation":552,"scale":554,"position_group":71,"offvar_x":549,"match_rot":551,"particle_group":51,"offvar_y":550},"song_trigger":{"prep":399,"song":392,"start":408,"load_prep":400,"volume":406,"fade_in":409,"end_":410,"speed":404,"loop":413,"channel":432,"fade_out":411},"follow_trigger":{"target_group":51,"follow_group":71,"x_mod":72,"y_mod":73},"area_rotate_trigger":{"offset_y":252,"easing_rate":243,"rotation_rand":271,"ignore_linked":281,"offset_rand":221,"offset_y_rand":253,"rotation":270,"center_p1_p2_c__tr":538,"de_ap":539,"center_id":71,"target_id":51,"dir_buttons_dir":262,"ignore_gparent":280,"easing2_rate":249,"priority":341,"easing2":248,"offset":220,"length":222,"dir_buttons_single_arrow":283,"easing":242,"effect_id":225,"deadzone":282,"length_rand":223,"mod_back":264,"mod_front":263,"ease_out":261,"dir_buttons_inwards":276},"item_persistence_trigger":{"timer":494,"item_id":80,"persistent":491,"reset":493,"target_all":492},"sequence_trigger":{"reset":438,"sequence":435,"reset_full_step":439,"min_int":437,"mode":438},"area_move_trigger":{"movex":237,"ignore_linked":281,"offset_rand":221,"offset_y_rand":253,"rfade":288,"ignore_gparent":280,"priority":341,"length":222,"movex_rand":238,"center_p1_p2_c__tr":538,"mod_front":263,"offset_y":252,"easing_rate":243,"movey":239,"move_dist_rand":219,"movey_rand":240,"move_angle":231,"target_id":51,"dir_buttons_inwards":276,"ease_out":261,"mod_back":264,"relative":287,"de_ap":539,"xy_mode":241,"move_angle_rand":232,"offset":220,"length_rand":223,"dir_buttons_single_arrow":283,"deadzone":282,"effect_id":225,"easing":242,"easing2":248,"easing2_rate":249,"dir_buttons_dir":262,"center_id":71,"move_dist":218},"edit_area_tint_trigger":{"offset_y":252,"easing_rate":243,"deadzone":282,"use_eid":355,"hsv":49,"offset":220,"length":222,"hsv_on":278,"offset_rand":221,"offset_y_rand":253,"tint":265,"length_rand":223,"group_effect_id":51,"mod_front":263,"mod_back":264,"easing":242},"camera_zoom_trigger":{"zoom":371},"misc":{"orange_teleportation_portal_distance":54},"reset_trigger":{"group_id":51},"animated_objects":{"use_speed":122,"offset_anim":592,"disable_delayed_loop":126,"randomize_start":106,"disable_animshine":127,"speed":107,"single_frame":462,"animate_on_trigger":123,"only_if_active":214},"animate_trigger":{"animation_id":76,"target_id":51},"edit_area_fade_trigger":{"offset_y":252,"easing_rate":243,"deadzone":282,"use_eid":355,"offset":220,"length":222,"offset_rand":221,"offset_y_rand":253,"to_opacity":275,"length_rand":223,"group_effect_id":51,"mod_front":263,"mod_back":264,"easing":242},"time_event_trigger":{"multi_activate":475,"target_id":51,"target_time":473,"item_id":80},"area_tint_trigger":{"offset_y":252,"ignore_linked":281,"hsv":49,"offset_rand":221,"offset_y_rand":253,"hsv_on":278,"mod_back":264,"main_only":65,"deadzone":282,"color_channel":260,"center_p1_p2_c__tr":538,"dir_buttons_inwards":276,"ignore_gparent":280,"easing2_rate":249,"priority":341,"target_id":51,"offset":220,"length":222,"dir_buttons_single_arrow":283,"secondary_only":66,"effect_id":225,"tint":265,"length_rand":223,"dir_buttons_dir":262,"mod_front":263,"center_id":71,"de_ap":539},"stop_area_trigger":{"effect_id":51},"object_extra":{"no_effects":116,"center_effect":369,"extended_collision":511,"single_ptouch":284,"reverse":117,"dont_enter":67,"area_parent":279,"ice_block":137,"dont_boost_y":496,"material":446,"no_glow":96,"dont_boost_x":509,"enter_channel":343,"non_stick_x":136,"dont_fade":64,"high_detail":103,"hide":135,"group_parent":34,"no_audio_scale":372,"no_particle":507,"scale_stick":356,"non_stick_y":289,"passable":134,"control_id":534,"no_touch":121,"grip_slope":193,"extra_sticky":495},"edit_area_rotate_trigger":{"offset_y":252,"easing_rate":243,"deadzone":282,"use_eid":355,"offset":220,"length":222,"rotation_rand":271,"offset_rand":221,"offset_y_rand":253,"rotation":270,"length_rand":223,"group_effect_id":51,"mod_front":263,"mod_back":264,"easing":242},"object_common":{"y":3,"color_1_hsv":43,"flip_horiz":4,"pink_groups":274,"z_layer":24,"single_color_type":497,"x":2,"linked_group":108,"ord":115,"color_2_hsv_enabled":42,"color_1":21,"color_2":22,"flip_vert":5,"editor_layer":20,"editor_layer_2":61,"warp_x_angle":132,"z_order":25,"groups":57,"rotation":6,"color_2_hsv":44,"scale":32,"edp":13,"scale_y":129,"id":1,"scale_x":128,"warp_y_angle":131,"channel":170,"color_1_hsv_enabled":41},"advanced_follow_trigger":{"target_dir":305,"break_steer_speed_limit_rand":333,"break_steer_speed_limit":332,"break_steer_force_rand":331,"near_friction_rand":562,"break_angle_rand":329,"max_speed":298,"near_accel":357,"break_force_rand":327,"y_only":307,"slow_dist_rand":360,"steer_force_rand":317,"delay":292,"follow_id":71,"start_speed":300,"steer_force_low":318,"steer_force_low_checkbox":337,"start_speed_rand":301,"start_dir_unmarked":565,"priority":365,"friction_rand":559,"x_only":306,"easing":361,"exclusive":571,"speed_range_high_rand":325,"rot_deadz":364,"steer_force_low_rand":319,"ignore_disabled":336,"friction":558,"max_range_unmarked":366,"steer_force_high_rand":321,"follow_p1":138,"steer_force_high":320,"speed_range_low_rand":323,"max_range":308,"near_dist":359,"speed_range_low":322,"start_dir_rand":564,"start_speed_unmarked":560,"acceleration":334,"rotate_dir":339,"target_id":51,"slow_dist":359,"near_dist_rand":360,"start_dir":563,"follow_p2":200,"max_range_rand":309,"break_angle":328,"rot_offset":340,"break_steer_force":330,"near_friction":561,"max_speed_rand":299,"acceleration_rand":335,"steer_force":316,"near_accel_rand":358,"follow_c":201,"delay_rand":293,"page_2_1_2_3":367,"rot_easing":363,"break_force":326,"speed_range_high":324,"init_set_add":572,"easing_rand":362,"steer_force_high_checkbox":338},"toggle_trigger":{"group_id":51,"activate_group":56},"edit_area_scale_trigger":{"offset_y":252,"easing_rate":243,"offset_rand":221,"offset_y_rand":253,"group_effect_id":51,"deadzone":282,"use_eid":355,"offset":220,"easing":242,"scale_y_rand":236,"length":222,"mod_back":264,"length_rand":223,"scale_x_rand":234,"mod_front":263,"scale_x":233,"scale_y":235},"ground_trigger":{"ground":533},"camera_mode_trigger":{"disable_gridsnap":370,"padding":114,"free_mode":111,"edit_camera_settings":112,"easing":113},"scale_trigger":{"scale_x":150,"relative_rot":452,"only_move":133,"relative_scale":577,"div_y":154,"div_x":153,"target_group":51,"center_group":71,"scale_y":151},"camera_offset_trigger":{"x_y_only":101,"offsetx":28,"offsety":29},"collectible":{"particle":440,"enable_group":56,"sub_count":78,"points":383,"no_anim":463,"item_id":80,"pickup_item":381,"group_id":51,"toggle_trigger":382}})JSON";
    
    auto v = matjson::parse(jsonstr);
    std::vector<sol::table> ret;
    ret.reserve(v.as_object().size());

    for(const auto& group : v.as_object())
    {
        sol::table gtable = lua.create_named_table(group.first);
        for(const auto& property : group.second.as_object())
        {
            gtable.set(property.first, property.second.as_int());
        }
        ret.push_back(gtable);
    }
    return ret;
}


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
