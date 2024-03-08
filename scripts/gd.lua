---@module 'gd'
---@meta

---@class GameObject
---@field id integer
---@field type GameObjectType
---@field x number
---@field y number
---@field scale number
---@field rotation number
GameObject = {}

---@param ... integer[]|integer
---@return table<number, number|string>
function GameObject:getProperties(...) end

---@return integer[]
function GameObject:getGroups() end

function GameObject:removeAllGroups() end

---object string in level string format (key,value,key,value)
---@return string
function GameObject:getSaveString() end

---overrides groups, use addGroups to append groups instead<br>
---@see GameObject.addGroups
---@param ... integer[]|integer
function GameObject:setGroups(...) end

---appends groups, use setGroup to override groups instead
---@see GameObject.setGroups
---@param ... integer[]|integer
function GameObject:addGroups(...) end

---@param f fun(group: integer)
function GameObject:forAllGroups(f) end


gd = {}
--TODO: opacity, color<br>
---@param properties table<number, number|boolean|string>
---@return GameObject
function gd.create(properties) end

---fast print (experimental)
function gd.print(...) end

---@return GameObject[]
function gd.getSelectedObjects() end

---@return GameObject[]
function gd.getAllObjects() end

---@return number
function gd.getObjectCount() end

---@param fun fun(obj: GameObject)
function gd.forAllObjects(fun) end

---@param fun fun(obj: GameObject)
function gd.forSelectedObjects(fun) end

---@param ... GameObject|GameObject[]
---Avoid calling this function in a loop, instead add all objects needed to be deleted in a table and just call the function once. 
function gd.deleteObjects(...) end

---------------------TODO--------------------


--deselects the object(s) if they aren't already
--@param object  GameObject
--function gd.deselectObject(object) end

--removes an object or an array objects
--@param object GameObject|table
--function gd.removeObject(object) end


--selects the object(s) if they aren't already
--@param object  GameObject|table|nil
--function gd.selectObject(object) end

---@enum GameObjectType
GameObjectType = {
	Solid = 0,
    Hazard = 2,
    InverseGravityPortal = 3,
    NormalGravityPortal = 4,
    ShipPortal = 5,
    CubePortal = 6,
    Decoration = 7,
    YellowJumpPad = 8,
    PinkJumpPad = 9,
    GravityPad = 10,
    YellowJumpRing = 11,
    PinkJumpRing = 12,
    GravityRing = 13,
    InverseMirrorPortal = 14,
    NormalMirrorPortal = 15,
    BallPortal = 16,
    RegularSizePortal = 17,
    MiniSizePortal = 18,
    UfoPortal = 19,
    Modifier = 20,
    Breakable = 21,
    SecretCoin = 22,
    DualPortal = 23,
    SoloPortal = 24,
    Slope = 25,
    WavePortal = 26,
    RobotPortal = 27,
    TeleportPortal = 28,
    GreenRing = 29,
    Collectible = 30,
    UserCoin = 31,
    DropRing = 32,
    SpiderPortal = 33,
    RedJumpPad = 34,
    RedJumpRing = 35,
    CustomRing = 36,
    DashRing = 37,
    GravityDashRing = 38,
    CollisionObject = 39,
    Special = 40,
    SwingPortal = 41,
    GravityTogglePortal = 42,
    SpiderOrb = 43,
    SpiderPad = 44,
    TeleportOrb = 46,
    AnimatedHazard = 47,
}

---@enum trigger_common
trigger_common = {
	easing_rate = 85, --float
	touch_triggered = 11, --bool
	easing = 30, --int [0, 18]
	duration = 10, --float
	multi_triggered = 87, --bool
	spawn_triggered = 62, --bool
}
---@enum rotate_trigger
rotate_trigger = {
	dynamic_mode = 397, --bool
	maxx_id = 518, --int
	dynamic_mode_easing = 403, --int
	miny_id = 517, --int
	aim_follow_mode_targetp1 = 138, --bool
	rot_offset = 402, --float
	maxy_id = 519, --int
	degrees = 68, --float
	minx_id = 516, --int
	target_group = 51, --int
	aim_follow_mode_targetp2 = 200, --bool
	rot_target_id = 401, --int
	lock_obj_rot = 70, --bool
	x360 = 69, --int
	center_group = 71, --int
	follow_mode = 394, --bool
	aim_mode = 100, --bool
}
---@enum collision_trigger
collision_trigger = {
	target_id = 51, --int
	p1 = 138, --bool
	trigger_on_exit = 93, --bool
	activate_group = 56, --bool
	block_a_id = 80, --int
	block_b_id = 95, --int
	pp = 201, --bool
	p2 = 200, --bool
}
---@enum camera_zoom_trigger
camera_zoom_trigger = {
	zoom = 371, --float
}
---@enum pickup_trigger
pickup_trigger = {
	multiply_divide = 88, --int [0, 2]
	count = 77, --int
	modifier = 449, --float
	item_id = 80, --int
	override = 139, --bool
}
---@enum camera_rotate_trigger
camera_rotate_trigger = {
	snap360 = 394, --bool
	add = 70, --bool
	degrees = 68, --float
}
---@enum edit_advanced_follow_trigger
edit_advanced_follow_trigger = {
	target_id = 51, --int
	mod_x_rand = 567, --float
	speed = 300, --float
	dir = 363, --int
	dir_unmarked_ = 565, --int
	mod_y = 568, --float
	speed_unmarked_ = 560, --int
	speed_rand = 301, --float
	target_control_id = 535, --bool
	dir_rand = 364, --int
	mod_x = 566, --float
	x_only = 306, --bool
	mod_y_rand = 569, --float
	redirect_dir = 570, --bool
	y_only = 307, --bool
}
---@enum event_trigger
event_trigger = {
	events = 430, --string, period separated list
	group_id = 51, --int
	extra_id_2 = 525, --int
	extra_id = 447, --int
}
---@enum camera_guide
camera_guide = {
	preview_opacity = 506, --float
	offsety = 29, --int
	zoom = 371, --float
	offsetx = 28, --int
}
---@enum advanced_follow_trigger
advanced_follow_trigger = {
	target_id = 51, --int
	near_friction = 561, --float
	steer_force_low = 318, --float
	target_dir = 305, --bool
	friction_rand = 559, --float
	start_dir = 563, --int
	friction = 558, --float
	break_steer_force = 330, --float
	slow_dist_rand = 360, --int
	start_speed_unmarked_ = 560, --int
	speed_range_low = 322, --float
	break_steer_force_rand = 331, --float
	break_steer_speed_limit_rand = 333, --float
	steer_force_low_checkbox = 337, --bool
	break_steer_speed_limit = 332, --float
	break_angle_rand = 329, --int
	speed_range_low_rand = 323, --float
	break_angle = 328, --int
	rot_offset = 340, --int
	break_force_rand = 327, --float
	break_force = 326, --float
	slow_dist = 359, --int
	speed_range_high_rand = 325, --float
	ignore_disabled = 336, --bool
	start_dir_unmarked_ = 565, --int
	speed_range_high = 324, --float
	start_speed = 300, --float
	acceleration_rand = 335, --float
	easing = 361, --float
	follow_p1 = 138, --bool
	start_speed_rand = 301, --float
	follow_id = 71, --int
	steer_force_high_checkbox = 338, --bool
	priority = 365, --int
	steer_force_high = 320, --float
	rot_easing = 363, --int
	easing_rand = 362, --float
	max_range_rand = 309, --int
	follow_c = 201, --bool
	steer_force_low_rand = 319, --float
	page_2_1_2_3 = 367, --int [0, 2]
	rotate_dir = 339, --bool
	steer_force = 316, --float
	follow_p2 = 200, --bool
	steer_force_rand = 317, --float
	init_set_add = 572, --int [0, 2]
	delay = 292, --float
	near_friction_rand = 562, --float
	near_dist_rand = 360, --float
	max_range_unmarked_ = 366, --int
	near_accel_rand = 358, --float
	near_dist = 359, --float
	exclusive = 571, --bool
	near_accel = 357, --float
	max_speed_rand = 299, --float
	delay_rand = 293, --float
	rot_deadz = 364, --int
	max_range = 308, --int
	max_speed = 298, --float
	acceleration = 334, --float
	x_only = 306, --bool
	steer_force_high_rand = 321, --float
	start_dir_rand = 564, --int
	y_only = 307, --bool
}
---@enum camera_mode_trigger
camera_mode_trigger = {
	disable_gridsnap = 370, --bool
	easing = 113, --int
	padding = 114, --float
	free_mode = 111, --bool
	edit_camera_settings = 112, --bool
}
---@enum keyframe_anim_trigger
keyframe_anim_trigger = {
	target_id = 51, --int
	scale_x_mod = 523, --float
	parent_id = 71, --int
	time_mod = 520, --float
	animation_group_id = 76, --int
	position_x_mod = 521, --float
	rotation_mod = 522, --float
	scale_y_mod = 546, --float
	position_y_mod = 545, --float
}
---@enum animate_trigger
animate_trigger = {
	target_id = 51, --int
	animation_id = 76, --int
}
---@enum area_rotate_trigger
area_rotate_trigger = {
	easing_rate = 243, --float
	priority = 341, --int
	dir_buttons_dir = 262, --int [0, 2], all, horiz, vert
	offset = 220, --int
	effect_id = 225, --int
	mod_back = 264, --float
	rotation_rand = 271, --float
	rotation = 270, --float
	center_p1_p2_c__tr = 538, --int [-11, 0]
	mod_front = 263, --float
	offset_y_rand = 253, --int
	target_id = 51, --int
	deadzone = 282, --float
	center_id = 71, --int
	dir_buttons_single_arrow = 283, --bool, seems to be true when DIR is all
	offset_rand = 221, --int
	ignore_gparent = 280, --bool
	ignore_linked = 281, --bool
	de_ap = 539, --bool
	length = 222, --int
	ease_out = 261, --bool
	offset_y = 252, --int
	easing2 = 248, --int [0, 18]
	easing = 242, --int [0, 18]
	easing2_rate = 249, --float
	length_rand = 223, --int
	dir_buttons_inwards = 276, --bool
}
---@enum item_persistence_trigger
item_persistence_trigger = {
	persistent = 491, --bool
	reset = 493, --bool
	target_all = 492, --bool
	item_id = 80, --int
	timer = 494, --bool
}
---@enum reset_trigger
reset_trigger = {
	group_id = 51, --int
}
---@enum spawn_particle_trigger
spawn_particle_trigger = {
	rotation_rand = 553, --int
	offvar_x = 549, --int
	position_group = 71, --int
	offset_x = 547, --int
	particle_group = 51, --int
	offset_y = 548, --int
	scale_rand = 555, --float
	rotation = 552, --int
	match_rot = 551, --bool
	scale = 554, --float
	offvar_y = 550, --int
}
---@enum spawn_trigger
spawn_trigger = {
	target_id = 51, --int
	delay_rand = 556, --float
	group_remaps = 442, --string, example: 3.5.2.10 -> remaps 3 to 5, 2 to 10
	spawn_ordered = 441, --bool
	reset_remap = 581, --bool
	preview_disable = 102, --bool
	delay = 63, --float
}
---@enum random_trigger
random_trigger = {
	group_id_1 = 51, --int
	group_id_2 = 71, --int
	chance = 10, --float
}
---@enum area_move_trigger
area_move_trigger = {
	easing_rate = 243, --float
	movex = 237, --int
	movey_rand = 240, --int
	offset = 220, --int
	move_dist = 218, --int
	move_angle_rand = 232, --int
	move_angle = 231, --int
	center_p1_p2_c__tr = 538, --int [-11, 0]
	center_id = 71, --int
	xy_mode = 241, --bool
	ignore_linked = 281, --bool
	offset_y = 252, --int
	easing2 = 248, --int [0, 18]
	easing = 242, --int [0, 18]
	dir_buttons_inwards = 276, --bool
	priority = 341, --int
	dir_buttons_dir = 262, --int [0, 2], all, horiz, vert
	effect_id = 225, --int
	mod_back = 264, --float
	relative = 287, --bool
	rfade = 288, --float
	deadzone = 282, --float
	movey = 239, --int
	dir_buttons_single_arrow = 283, --bool, seems to be true when DIR is all
	offset_rand = 221, --int
	ignore_gparent = 280, --bool
	length_rand = 223, --int
	move_dist_rand = 219, --int
	offset_y_rand = 253, --int
	ease_out = 261, --bool
	length = 222, --int
	movex_rand = 238, --int
	target_id = 51, --int
	easing2_rate = 249, --float
	mod_front = 263, --float
	de_ap = 539, --bool
}
---@enum on_death_trigger
on_death_trigger = {
	activate_group = 56, --bool
	group_id = 51, --int
}
---@enum counter_display
counter_display = {
	time_counter = 466, --bool
	maintime_points_attempts = 390, --int [-3, 0]
	item_id = 80, --int
	seconds_only = 389, --bool
	align = 391, --int [0, 2]
}
---@enum count_trigger
count_trigger = {
	target_id = 51, --int
	target_count = 77, --int
	activate_group = 56, --bool
	item_id = 80, --int
	multi_activate = 104, --bool
}
---@enum object_extra
object_extra = {
	group_parent = 34, --bool
	dont_boost_y = 496, --bool
	hide = 135, --bool
	high_detail = 103, --bool
	no_effects = 116, --bool
	extended_collision = 511, --bool
	non_stick_x = 136, --bool
	no_touch = 121, --bool
	reverse = 117, --bool
	control_id = 534, --int
	material = 446, --int
	passable = 134, --bool
	extra_sticky = 495, --bool
	area_parent = 279, --bool
	enter_channel = 343, --int
	no_glow = 96, --bool
	no_particle = 507, --bool
	don_t_fade = 64, --bool
	non_stick_y = 289, --bool
	grip_slope = 193, --bool
	don_t_enter = 67, --bool
	ice_block = 137, --bool
	single_ptouch = 284, --bool
	dont_boost_x = 509, --bool
	scale_stick = 356, --bool
	center_effect = 369, --bool
	no_audio_scale = 372, --bool
}
---@enum time_control_trigger
time_control_trigger = {
	item_id = 80, --int
	start_stop = 472, --bool
}
---@enum stop_area_trigger
stop_area_trigger = {
	effect_id = 51, --int
}
---@enum sequence_trigger
sequence_trigger = {
	reset = 438, --float
	mode = 438, --int [0, 2]
	sequence = 435, --string, period separated, similar to spawn remap
	reset_full_step = 439, --bool
	min_int = 437, --float
}
---@enum background_trigger
background_trigger = {
	background = 533, --int [1, 59]
}
---@enum touch_trigger
touch_trigger = {
	hold_mode = 81, --bool
	group_id = 51, --int
	toggle_on_off = 82, --int [0, 2]
	dual_mode = 89, --bool
	p1_p2_only = 198, --int [0, 2]
}
---@enum shake_trigger
shake_trigger = {
	strength = 75, --float
	interval = 84, --float
}
---@enum edit_area_tint_trigger
edit_area_tint_trigger = {
	easing_rate = 243, --float
	deadzone = 282, --float
	hsv = 49, --hsv string
	hsv_on = 278, --bool
	offset_rand = 221, --int
	offset = 220, --int
	tint = 265, --float
	use_eid = 355, --bool
	group_effect_id = 51, --int
	mod_back = 264, --float
	offset_y = 252, --int
	mod_front = 263, --float
	easing = 242, --int [0, 18]
	length = 222, --int
	length_rand = 223, --int
	offset_y_rand = 253, --int
}
---@enum text_object
text_object = {
	kerning = 488, --int
	text = 31, --string, base64 encoded
}
---@enum area_tint_trigger
area_tint_trigger = {
	target_id = 51, --int
	priority = 341, --int
	ignore_gparent = 280, --bool
	offset = 220, --int
	effect_id = 225, --int
	de_ap = 539, --bool
	mod_back = 264, --float
	hsv_on = 278, --bool
	dir_buttons_dir = 262, --int [0, 2], all, horiz, vert
	length_rand = 223, --int
	length = 222, --int
	main_only = 65, --bool
	deadzone = 282, --float
	center_id = 71, --int
	dir_buttons_single_arrow = 283, --bool, seems to be true when DIR is all
	offset_rand = 221, --int
	tint = 265, --float
	color_channel = 260, --int
	center_p1_p2_c__tr = 538, --int [-11, 0]
	offset_y_rand = 253, --int
	ignore_linked = 281, --bool
	offset_y = 252, --int
	mod_front = 263, --float
	hsv = 49, --hsv string
	easing2_rate = 249, --float
	secondary_only = 66, --bool
	dir_buttons_inwards = 276, --bool
}
---@enum time_event_trigger
time_event_trigger = {
	target_id = 51, --int
	multi_activate = 475, --bool
	item_id = 80, --int
	target_time = 473, --float
}
---@enum custom_particles
custom_particles = {
	animate_on_trigger = 123, --bool
	quick_start = 211, --bool
	particle_data = 145, --string, list separated by `a`, check above for properties in order
	animate_active_only = 214, --bool
	use_obj_color = 146, --bool
	uniform_obj_color = 147, --bool, for some reason, this value is also present in the particle data string ­ƒÿ░
}
---@enum collision_state_block
collision_state_block = {
	state_on = 51, --int
	state_off = 71, --int
}
---@enum stop_trigger
stop_trigger = {
	target_id = 51, --int
	stop_pause_resume = 580, --int [0, 2]
	use_control_id = 535, --bool
}
---@enum collision_block
collision_block = {
	dynamic_block = 94, --bool
	block_id = 80, --int
}
---@enum area_scale_trigger
area_scale_trigger = {
	easing_rate = 243, --float
	priority = 341, --int
	dir_buttons_dir = 262, --int [0, 2], all, horiz, vert
	offset = 220, --int
	scale_y_rand = 236, --float
	effect_id = 225, --int
	mod_back = 264, --float
	scale_y = 235, --float
	ignore_gparent = 280, --bool
	scale_x = 233, --float
	center_p1_p2_c__tr = 538, --int [-11, 0]
	mod_front = 263, --float
	offset_y_rand = 253, --int
	target_id = 51, --int
	deadzone = 282, --float
	center_id = 71, --int
	dir_buttons_single_arrow = 283, --bool, seems to be true when DIR is all
	offset_rand = 221, --int
	scale_x_rand = 234, --float
	ignore_linked = 281, --bool
	de_ap = 539, --bool
	length = 222, --int
	ease_out = 261, --bool
	offset_y = 252, --int
	easing2 = 248, --int [0, 18]
	easing = 242, --int [0, 18]
	easing2_rate = 249, --float
	length_rand = 223, --int
	dir_buttons_inwards = 276, --bool
}
---@enum pulse_trigger
pulse_trigger = {
	target_id = 51, --int
	green = 8, --int
	hold = 46, --float
	fade_out = 47, --float
	exclusive = 86, --bool
	hsv = 49, --hsv string
	fade_in = 45, --float
	blue = 9, --int
	main_only = 65, --bool
	target_type = 52, --bool
	hsv_enabled = 48, --bool
	detail_only = 66, --bool
	copy_color_id = 50, --int
	red = 7, --int
}
---@enum dash_orb
dash_orb = {
	allow_collide = 587, --bool
	end_boost = 588, --float
	stop_slide = 589, --bool
	speed = 586, --float
	max_duration = 590, --float
}
---@enum keyframe_object
keyframe_object = {
	preview_art = 380, --bool
	cw_ccw = 536, --int [0, 2], cw: 1, ccw: 2
	prox = 377, --bool
	group_id = 51, --int
	spawn_gid = 71, --int
	ref_only = 375, --bool
	line_opacity = 524, --float
	anim_id_ = 373, --int
	spawn_delay = 557, --float
	close_loop = 376, --bool
	x360 = 537, --int
	auto_layer = 459, --bool
	curve = 378, --bool
	time_even_dist = 379, --int [0, 2]
}
---@enum gradient_trigger
gradient_trigger = {
	layer = 202, --int [1, 15]
	id = 209, --int
	vertex_mode = 207, --bool
	blending = 174, --int [0, 3]
	l_tl = 205, --int
	u_bl = 203, --int
	disable = 208, --bool
	r_tr = 206, --int
	preview_opacity = 456, --float
	disable_all = 508, --bool
	d_br = 204, --int
}
---@enum object_common
object_common = {
	color_1_hsv = 43, --hsv string
	color_2 = 22, --int
	flip_horiz = 4, --bool
	pink_groups = 274, --string, period separated list
	scale = 32, --float
	color_2_hsv = 44, --hsv string
	linked_group = 108, --int, the id of the editor link group this object is in (link controls)
	single_color_type = 497, --int [0, 2], that settings icon at the top of edit object that changes if single color objects are treated as base or detail
	warp_y_angle = 131, --float
	channel = 170, --int
	x = 2, --float
	y = 3, --float
	editor_layer_2 = 61, --int
	color_1 = 21, --int
	scale_y = 129, --float
	edp = 13, --bool
	scale_x = 128, --float
	groups = 57, --string, period separated list, includes the pink groups
	id = 1, --int
	ord = 115, --int
	z_order = 25, --int
	z_layer = 24, --int
	editor_layer = 20, --int
	color_2_hsv_enabled = 42, --bool
	color_1_hsv_enabled = 41, --bool
	flip_vert = 5, --bool
	rotation = 6, --float
	warp_x_angle = 132, --float
}
---@enum follow_trigger
follow_trigger = {
	x_mod = 72, --float
	target_group = 51, --int
	follow_group = 71, --int
	y_mod = 73, --float
}
---@enum sfx_trigger
sfx_trigger = {
	volume = 406, --float
	fade_in = 409, --int
	fft = 412, --bool
	ignore_volume_test = 489, --bool
	min_interval = 434, --float
	sfx_id = 392, --int
	cam = 428, --bool
	override = 420, --bool
	p1 = 138, --bool
	dist_3 = 426, --int
	dist_2 = 425, --int
	pre_load = 433, --bool
	reverb = 407, --bool
	speed = 404, --int
	start = 408, --int
	end_ = 410, --int
	vol_near = 421, --float
	group_id_2 = 71, --int
	vol_med = 422, --float
	vol_far = 423, --float
	is_unique = 415, --bool
	loop = 413, --bool
	group_id_1 = 51, --int
	fade_out = 411, --int
	reverb_enable = 503, --bool
	p2 = 200, --bool
	reverb_type = 502, --int [0, 22]
	sfx_group = 455, --int
	dir_buttons = 458, --int [0, 6]
	min_dist = 424, --int
	unique_id = 416, --int
	sound_duration = 490, --float, affected by changing sfx, speed, start/end
	pitch = 405, --int
}
---@enum edit_area_move_trigger
edit_area_move_trigger = {
	easing_rate = 243, --float
	movex = 237, --int
	movey_rand = 240, --int
	offset = 220, --int
	move_dist = 218, --int
	mod_back = 264, --float
	group_effect_id = 51, --int
	move_angle = 231, --int
	mod_front = 263, --float
	length = 222, --int
	deadzone = 282, --float
	offset_rand = 221, --int
	movey = 239, --int
	xy_mode = 241, --bool
	move_dist_rand = 219, --int
	move_angle_rand = 232, --int
	rfade = 288, --float
	use_eid = 355, --bool
	movex_rand = 238, --int
	easing = 242, --int [0, 18]
	offset_y_rand = 253, --int
	length_rand = 223, --int
	offset_y = 252, --int
}
---@enum player_control_trigger
player_control_trigger = {
	stop_move = 541, --bool
	stop_jump = 540, --bool
	p2 = 200, --bool
	stop_rot = 542, --bool
	p1 = 138, --bool
	stop_slide = 543, --bool
}
---@enum camera_offset_trigger
camera_offset_trigger = {
	x_y_only = 101, --int [0, 2]
	offsetx = 28, --int
	offsety = 29, --int
}
---@enum gravity_trigger
gravity_trigger = {
	gravity = 148, --float
	p1 = 138, --bool
	p2 = 200, --bool
	pt = 201, --bool
}
---@enum alpha_trigger
alpha_trigger = {
	opacity = 35, --float
	group_id = 51, --int
}
---@enum ground_trigger
ground_trigger = {
	ground = 533, --int [1, 22]
}
---@enum edit_area_scale_trigger
edit_area_scale_trigger = {
	easing_rate = 243, --float
	offset = 220, --int
	mod_back = 264, --float
	group_effect_id = 51, --int
	mod_front = 263, --float
	length = 222, --int
	deadzone = 282, --float
	scale_y = 235, --float
	offset_rand = 221, --int
	scale_x_rand = 234, --float
	scale_y_rand = 236, --float
	use_eid = 355, --bool
	offset_y_rand = 253, --int
	easing = 242, --int [0, 18]
	length_rand = 223, --int
	offset_y = 252, --int
	scale_x = 233, --float
}
---@enum scale_trigger
scale_trigger = {
	div_y = 154, --bool
	scale_y = 151, --float
	div_x = 153, --bool
	scale_x = 150, --float
	target_group = 51, --int
	relative_rot = 452, --bool
	center_group = 71, --int
	only_move = 133, --bool
	relative_scale = 577, --bool
}
---@enum toggle_block
toggle_block = {
	claim_touch = 445, --bool
	multi_activate = 99, --bool
	activate_group = 56, --bool
	group_id = 51, --int
	spawn_only = 504, --bool
}
---@enum instant_collision_trigger
instant_collision_trigger = {
	pp = 201, --bool
	p1 = 138, --bool
	p2 = 200, --bool
	block_a_id = 80, --int
	block_b_id = 95, --int
	false_id = 71, --int
	true_id = 51, --int
}
---@enum animated_objects
animated_objects = {
	speed = 107, --float
	disable_animshine = 127, --bool
	use_speed = 122, --bool
	only_if_active = 214, --bool
	randomize_start = 106, --bool
	offset_anim = 592, --bool
	disable_delayed_loop = 126, --bool
	animate_on_trigger = 123, --bool
	single_frame = 462, --int
}
---@enum ui_trigger
ui_trigger = {
	xref_relative = 387, --bool
	yref_relative = 388, --bool
	xref_pos = 385, --int [1, 4]
	group_id = 51, --int
	ui_target = 71, --int
	yref_pos = 386, --int [1, 4]
}
---@enum middle_ground_trigger
middle_ground_trigger = {
	middle_ground = 533, --int [0, 3]
}
---@enum timewarp_trigger
timewarp_trigger = {
	time_mod = 120, --float
}
---@enum misc
misc = {
	orange_teleportation_portal_distance = 54, --float
}
---@enum camera_edge_trigger
camera_edge_trigger = {
	edge = 164, --int [0, 4]
	target_id = 51, --int
}
---@enum edit_area_fade_trigger
edit_area_fade_trigger = {
	easing_rate = 243, --float
	deadzone = 282, --float
	offset_rand = 221, --int
	offset = 220, --int
	to_opacity = 275, --float
	use_eid = 355, --bool
	group_effect_id = 51, --int
	mod_back = 264, --float
	offset_y = 252, --int
	mod_front = 263, --float
	easing = 242, --int [0, 18]
	length = 222, --int
	length_rand = 223, --int
	offset_y_rand = 253, --int
}
---@enum options_trigger
options_trigger = {
	hide_p2 = 163, --int [-1, 1]
	unlink_dual_gravity = 160, --int [-1, 1]
	hide_p1 = 162, --int [-1, 1]
	no_death_sfx = 576, --int [-1, 1]
	hide_attempts = 532, --int [-1, 1]
	hide_ground = 161, --int [-1, 1]
	disable_p2_controls = 199, --int [-1, 1]
	disable_p1_controls = 165, --int [-1, 1]
	respawn_time = 574, --float
	edit_respawn_time = 573, --int [-1, 1]
	streak_additive = 159, --int [-1, 1]
	audio_on_death = 575, --int [-1, 1]
	hide_mg = 195, --int [-1, 1]
}
---@enum rotate_gameplay___arrow_trigger
rotate_gameplay___arrow_trigger = {
	arrow_dir = 167, --int [1, 4], 1: up, 2: down, 3: left, 4: right
	gravity_dir = 166, --int [1, 4], 1: up, 2: down, 3: left, 4: right
	velocity_mod_y = 583, --float
	edit_velocity = 169, --bool
	velocity_mod_x = 582, --float
	instant_offset = 368, --bool
	target_channel = 173, --int
	channel_only = 172, --bool
	change_channel = 171, --bool
	override_velocity = 584, --bool
	dont_slide = 585, --bool
}
---@enum item_compare_trigger
item_compare_trigger = {
	rnd_flr_cei_2 = 486, --int [0, 3]
	typ_2 = 477, --int [0, 5]
	rnd_flr_cei_1 = 485, --int [0, 3]
	abs_neg_2 = 579, --int [0, 2]
	item_id_2 = 95, --int
	abs_neg_1 = 578, --int [0, 2]
	false_id = 71, --int
	op2 = 481, --int [1, 4]
	item_id_1 = 80, --int
	compare_op = 482, --int [0, 5]
	mod1 = 479, --float
	tol_rand = 484, --float
	true_id = 51, --int
	op1 = 480, --int [1, 4]
	typ_1 = 476, --int [0, 5]
	mod2 = 483, --float
}
---@enum time_trigger
time_trigger = {
	target_id = 51, --int
	stop_checked = 474, --bool
	time_mod = 470, --float
	item_id = 80, --int
	start_time = 467, --float
	ignore_timewarp = 469, --bool
	dont_override = 468, --bool
	stop_time = 473, --float
	start_paused = 471, --bool
}
---@enum collectible
collectible = {
	no_anim = 463, --bool
	group_id = 51, --int
	points = 383, --int
	item_id = 80, --int
	sub_count = 78, --bool
	toggle_trigger = 382, --bool
	pickup_item = 381, --bool
	particle = 440, --int
	enable_group = 56, --bool
}
---@enum retarget_advanced_follow_trigger
retarget_advanced_follow_trigger = {
	target_id = 51, --int
	follow_c = 201, --bool
	target_control_id = 535, --bool
	follow_p2 = 200, --bool
	follow_p1 = 138, --bool
	follow_id = 71, --int
}
---@enum move_trigger
move_trigger = {
	mody = 144, --float
	movex = 28, --int
	target_dir_mode_centergroup = 395, --int
	lock_y_player = 59, --bool
	lock_x_player = 58, --bool
	direction_mode_distance = 396, --int
	target_dir_mode_targetp2 = 200, --bool
	lock_y_camera = 142, --bool
	lock_x_camera = 141, --bool
	direction_mode = 394, --bool
	small_step = 393, --bool
	target_dir_mode_targetp1 = 138, --bool
	target_mode = 100, --bool
	target_group = 51, --int
	silent = 544, --bool
	target_mode_x_y_only = 101, --int [0, 2]
	dynamic_mode = 397, --bool
	target_dir_mode_targetgroup = 71, --int
	movey = 29, --int
	modx = 143, --float
}
---@enum toggle_trigger
toggle_trigger = {
	activate_group = 56, --bool
	group_id = 51, --int
}
---@enum color_trigger
color_trigger = {
	blue = 9, --int
	opacity = 35, --float
	green = 8, --int
	blending = 17, --bool
	target_color = 23, --int
	player_color_1 = 15, --bool
	player_color_2 = 16, --bool
	red = 7, --int
}
---@enum edit_area_rotate_trigger
edit_area_rotate_trigger = {
	easing_rate = 243, --float
	deadzone = 282, --float
	rotation_rand = 271, --float
	offset_rand = 221, --int
	offset = 220, --int
	rotation = 270, --float
	use_eid = 355, --bool
	group_effect_id = 51, --int
	mod_back = 264, --float
	offset_y = 252, --int
	mod_front = 263, --float
	easing = 242, --int [0, 18]
	length = 222, --int
	length_rand = 223, --int
	offset_y_rand = 253, --int
}
---@enum area_fade_trigger
area_fade_trigger = {
	target_id = 51, --int
	priority = 341, --int
	ignore_gparent = 280, --bool
	offset = 220, --int
	effect_id = 225, --int
	de_ap = 539, --bool
	to_opacity = 275, --float
	length_rand = 223, --int
	length = 222, --int
	deadzone = 282, --float
	center_id = 71, --int
	dir_buttons_single_arrow = 283, --bool, seems to be true when DIR is all
	offset_rand = 221, --int
	from_opacity = 286, --float
	center_p1_p2_c__tr = 538, --int [-11, 0]
	offset_y_rand = 253, --int
	ignore_linked = 281, --bool
	offset_y = 252, --int
	mod_front = 263, --float
	mod_back = 264, --float
	easing2_rate = 249, --float
	dir_buttons_dir = 262, --int [0, 2], all, horiz, vert
	dir_buttons_inwards = 276, --bool
}
---@enum song_trigger
song_trigger = {
	volume = 406, --float
	speed = 404, --int
	fade_in = 409, --int
	fade_out = 411, --int
	end_ = 410, --int
	song = 392, --int
	start = 408, --int
	prep = 399, --bool
	loop = 413, --bool
	load_prep = 400, --bool
	channel = 432, --int
}
---@enum advanced_random_trigger
advanced_random_trigger = {
	list = 152, --string, example: 2.10.3.15 -> g2 weight 10, g3 weight 15
}
---@enum static_camera_trigger
static_camera_trigger = {
	follow = 212, --bool
	smooth_velocity_modifier = 454, --float
	exit_instant = 465, --bool
	target_group_id = 71, --int
	exit_static = 110, --bool
	follow_easing = 213, --float
	smooth_velocity = 453, --bool
}
---@enum item_edit_trigger
item_edit_trigger = {
	rnd_flr_cei_2 = 486, --int [0, 3]
	typ_2 = 477, --int [0, 5]
	item_id_2 = 95, --int
	assign_op = 480, --int [0, 4]
	typ_1 = 476, --int [0, 5]
	target_typ = 478, --int [1, 3]
	item_id_1 = 80, --int
	abs_neg_2 = 579, --int [0, 2]
	abs_neg_1 = 578, --int [0, 2]
	mod = 479, --float
	op2 = 482, --int [3, 4]
	op1 = 481, --int [1, 4]
	target_item_id = 51, --int
	rnd_flr_cei_1 = 485, --int [0, 3]
}
---@enum follow_player_y_trigger
follow_player_y_trigger = {
	max_speed = 105, --float
	speed = 90, --float
	offset = 92, --int
	delay = 91, --float
	target_id = 51, --int
}
---@enum instant_count_trigger
instant_count_trigger = {
	target_id = 51, --int
	target_count = 77, --int
	activate_group = 56, --bool
	equals_larger_smaller = 88, --int [0, 2]
	item_id = 80, --int
}
---@enum rotating_objects
rotating_objects = {
	disable_rotation = 98, --bool
	rotation_speed = 97, --int
}
