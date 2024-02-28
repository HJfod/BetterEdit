---@module 'gd'
---@meta

--@class Color3B
--@field r number
--@field g number
--@field b number

---@class float

---@class GameObject
---@field x float
---@field y float
---@field scale float The scale
---@field rotation float The rotation
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


---@return GameObject[]
function gd.getSelectedObjects() end

---@return GameObject[]
function gd.getAllObjects() end

---@return number
function gd.getObjectCount() end

---@param fun fun(obj: GameObject)
function gd.forAllObjects(fun) end

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


---@enum gd.p
gd.p = {
	object_common = {
		id = 1,--int
		x = 2,--float
		y = 3,--float
		flip_horiz = 4,--bool
		flip_vert = 5,--bool
		rotation = 6,--float
		scale = 32,--float
		scale_x = 128,--float
		scale_y = 129,--float
		warp_y_angle = 131,--float
		warp_x_angle = 132,--float
		color_1 = 21,--int
		color_2 = 22,--int
		single_color_type = 497,--int [0, 2]<br>that settings icon at the top of edit object that changes if single color objects are treated as base or detail
		color_1_hsv_enabled = 41,--bool
		color_1_hsv = 43,--hsv string
		color_2_hsv_enabled = 42,--bool
		color_2_hsv = 44,--hsv string
		groups = 57,--string<br>period separated list, includes the pink groups
		pink_groups = 274,--string<br>period separated list
		editor_layer = 20,--int
		editor_layer_2 = 61,--int
		z_layer = 24,--int
		z_order = 25,--int
		ord = 115,--int
		channel = 170,--int
		edp = 13,--bool
		linked_group = 108,--int<br>the id of the editor link group this object is in (link controls)
	},
	object_extra = {
		dont_fade = 64,--bool
		dont_enter = 67,--bool
		no_effects = 116,--bool
		group_parent = 34,--bool
		area_parent = 279,--bool
		dont_boost_y = 496,--bool
		dont_boost_x = 509,--bool
		single_ptouch = 284,--bool
		high_detail = 103,--bool
		no_touch = 121,--bool
		passable = 134,--bool
		hide = 135,--bool
		non_stick_x = 136,--bool
		extra_sticky = 495,--bool
		extended_collision = 511,--bool
		center_effect = 369,--bool
		ice_block = 137,--bool
		grip_slope = 193,--bool
		no_glow = 96,--bool
		no_particle = 507,--bool
		non_stick_y = 289,--bool
		scale_stick = 356,--bool
		no_audio_scale = 372,--bool
		reverse = 117,--bool
		enter_channel = 343,--int
		material = 446,--int
		control_id = 534,--int
	},
	misc = {
		orange_teleportation_portal_distance = 54,--float
	},
	dash_orb = {
		speed = 586,--float
		end_boost = 588,--float
		max_duration = 590,--float
		allow_collide = 587,--bool
		stop_slide = 589,--bool
	},
	trigger_common = {
		duration = 10,--float
		spawn_triggered = 62,--bool
		touch_triggered = 11,--bool
		multi_triggered = 87,--bool
		easing = 30,--int [0, 18]
		easing_rate = 85,--float
	},
	custom_particles = {
		particle_data = 145,--string<br>list separated by `a`, check above for properties in order
		use_obj_color = 146,--bool
		uniform_obj_color = 147,--bool<br>for some reason, this value is also present in the particle data string ­ƒÿ░
		animate_on_trigger = 123,--bool
		animate_active_only = 214,--bool
		quick_start = 211,--bool
	},
	text_object = {
		text = 31,--string<br>base64 encoded
		kerning = 488,--int
	},
	color_trigger = {
		red = 7,--int
		green = 8,--int
		blue = 9,--int
		opacity = 35,--float
		blending = 17,--bool
		player_color_1 = 15,--bool
		player_color_2 = 16,--bool
		target_color = 23,--int
	},
	collectible = {
		particle = 440,--int
		points = 383,--int
		toggle_trigger = 382,--bool
		group_id = 51,--int
		enable_group = 56,--bool
		pickup_item = 381,--bool
		item_id = 80,--int
		sub_count = 78,--bool
		no_anim = 463,--bool
	},
	rotating_objects = {
		rotation_speed = 97,--int
		disable_rotation = 98,--bool
	},
	animated_objects = {
		randomize_start = 106,--bool
		disable_delayed_loop = 126,--bool
		use_speed = 122,--bool
		speed = 107,--float
		disable_animshine = 127,--bool
		animate_on_trigger = 123,--bool
		only_if_active = 214,--bool
		single_frame = 462,--int
		offset_anim = 592,--bool
	},
	move_trigger = {
		target_group = 51,--int
		movex = 28,--int
		movey = 29,--int
		lock_x_player = 58,--bool
		modx = 143,--float
		lock_y_player = 59,--bool
		mody = 144,--float
		lock_x_camera = 141,--bool
		lock_y_camera = 142,--bool
		target_mode = 100,--bool
		target_dir_mode_centergroup = 395,--int
		target_dir_mode_targetgroup = 71,--int
		target_dir_mode_targetp1 = 138,--bool
		target_dir_mode_targetp2 = 200,--bool
		target_mode_x_y_only = 101,--int [0, 2]
		direction_mode = 394,--bool
		direction_mode_distance = 396,--int
		silent = 544,--bool
		dynamic_mode = 397,--bool
		small_step = 393,--bool
	},
	pulse_trigger = {
		target_id = 51,--int
		target_type = 52,--bool
		exclusive = 86,--bool
		main_only = 65,--bool
		detail_only = 66,--bool
		fade_in = 45,--float
		hold = 46,--float
		fade_out = 47,--float
		red = 7,--int
		green = 8,--int
		blue = 9,--int
		hsv_enabled = 48,--bool
		hsv = 49,--hsv string
		copy_color_id = 50,--int
	},
	alpha_trigger = {
		group_id = 51,--int
		opacity = 35,--float
	},
	toggle_trigger = {
		group_id = 51,--int
		activate_group = 56,--bool
	},
	shake_trigger = {
		strength = 75,--float
		interval = 84,--float
	},
	animate_trigger = {
		target_id = 51,--int
		animation_id = 76,--int
	},
	spawn_trigger = {
		target_id = 51,--int
		delay = 63,--float
		delay_rand = 556,--float
		preview_disable = 102,--bool
		spawn_ordered = 441,--bool
		reset_remap = 581,--bool
		group_remaps = 442,--string<br>example: 3.5.2.10 -> remaps 3 to 5, 2 to 10
	},
	rotate_trigger = {
		degrees = 68,--float
		x360 = 69,--int
		target_group = 51,--int
		center_group = 71,--int
		aim_mode = 100,--bool
		follow_mode = 394,--bool
		rot_target_id = 401,--int
		rot_offset = 402,--float
		aim_follow_mode_targetp1 = 138,--bool
		aim_follow_mode_targetp2 = 200,--bool
		dynamic_mode = 397,--bool
		dynamic_mode_easing = 403,--int
		lock_obj_rot = 70,--bool
		minx_id = 516,--int
		miny_id = 517,--int
		maxx_id = 518,--int
		maxy_id = 519,--int
	},
	scale_trigger = {
		scale_x = 150,--float
		scale_y = 151,--float
		div_x = 153,--bool
		div_y = 154,--bool
		target_group = 51,--int
		center_group = 71,--int
		only_move = 133,--bool
		relative_scale = 577,--bool
		relative_rot = 452,--bool
	},
	follow_trigger = {
		x_mod = 72,--float
		y_mod = 73,--float
		target_group = 51,--int
		follow_group = 71,--int
	},
	stop_trigger = {
		target_id = 51,--int
		use_control_id = 535,--bool
		stop_pause_resume = 580,--int [0, 2]
	},
	keyframe_anim_trigger = {
		animation_group_id = 76,--int
		target_id = 51,--int
		parent_id = 71,--int
		time_mod = 520,--float
		position_x_mod = 521,--float
		position_y_mod = 545,--float
		rotation_mod = 522,--float
		scale_x_mod = 523,--float
		scale_y_mod = 546,--float
	},
	follow_player_y_trigger = {
		speed = 90,--float
		delay = 91,--float
		offset = 92,--int
		max_speed = 105,--float
		target_id = 51,--int
	},
	advanced_follow_trigger = {
		target_id = 51,--int
		follow_id = 71,--int
		priority = 365,--int
		x_only = 306,--bool
		y_only = 307,--bool
		follow_p1 = 138,--bool
		follow_p2 = 200,--bool
		follow_c = 201,--bool
		rot_offset = 340,--int
		rot_easing = 363,--int
		rot_deadz = 364,--int
		ignore_disabled = 336,--bool
		rotate_dir = 339,--bool
		init_set_add = 572,--int [0, 2]
		exclusive = 571,--bool
		page_2_1_2_3 = 367,--int [0, 2]
		delay = 292,--float
		delay_rand = 293,--float
		max_speed = 298,--float
		max_speed_rand = 299,--float
		max_range = 308,--int
		max_range_rand = 309,--int
		max_range_unmarked = 366,--int
		easing = 361,--float
		easing_rand = 362,--float
		start_speed = 300,--float
		start_speed_rand = 301,--float
		start_speed_unmarked = 560,--int
		start_dir = 563,--int
		start_dir_rand = 564,--int
		start_dir_unmarked = 565,--int
		acceleration = 334,--float
		acceleration_rand = 335,--float
		friction = 558,--float
		friction_rand = 559,--float
		near_dist = 359,--float
		near_dist_rand = 360,--float
		near_friction = 561,--float
		near_friction_rand = 562,--float
		near_accel = 357,--float
		near_accel_rand = 358,--float
		steer_force = 316,--float
		steer_force_rand = 317,--float
		steer_force_low = 318,--float
		steer_force_low_rand = 319,--float
		steer_force_low_checkbox = 337,--bool
		speed_range_low = 322,--float
		speed_range_low_rand = 323,--float
		steer_force_high = 320,--float
		steer_force_high_rand = 321,--float
		steer_force_high_checkbox = 338,--bool
		speed_range_high = 324,--float
		speed_range_high_rand = 325,--float
		slow_dist = 359,--int
		slow_dist_rand = 360,--int
		break_force = 326,--float
		break_force_rand = 327,--float
		break_angle = 328,--int
		break_angle_rand = 329,--int
		break_steer_force = 330,--float
		break_steer_force_rand = 331,--float
		break_steer_speed_limit = 332,--float
		break_steer_speed_limit_rand = 333,--float
		target_dir = 305,--bool
	},
	edit_advanced_follow_trigger = {
		target_id = 51,--int
		x_only = 306,--bool
		y_only = 307,--bool
		mod_x = 566,--float
		mod_x_rand = 567,--float
		mod_y = 568,--float
		mod_y_rand = 569,--float
		speed = 300,--float
		speed_rand = 301,--float
		speed_unmarked = 560,--int
		dir = 363,--int
		dir_rand = 364,--int
		dir_unmarked = 565,--int
		target_control_id = 535,--bool
		redirect_dir = 570,--bool
	},
	retarget_advanced_follow_trigger = {
		target_id = 51,--int
		follow_id = 71,--int
		target_control_id = 535,--bool
		follow_p1 = 138,--bool
		follow_p2 = 200,--bool
		follow_c = 201,--bool
	},
	keyframe_object = {
		group_id = 51,--int
		x360 = 537,--int
		spawn_gid = 71,--int
		spawn_delay = 557,--float
		curve = 378,--bool
		cw_ccw = 536,--int [0, 2]<br>cw: 1, ccw: 2
		time_even_dist = 379,--int [0, 2]
		prox = 377,--bool
		ref_only = 375,--bool
		auto_layer = 459,--bool
		preview_art = 380,--bool
		close_loop = 376,--bool
		anim_id = 373,--int
		line_opacity = 524,--float
	},
	area_move_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		ease_out = 261,--bool
		easing2 = 248,--int [0, 18]
		easing2_rate = 249,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		dir_buttons_dir = 262,--int [0, 2]<br>all, horiz, vert
		dir_buttons_inwards = 276,--bool
		dir_buttons_single_arrow = 283,--bool<br>seems to be true when DIR is all
		de_ap = 539,--bool
		ignore_linked = 281,--bool
		ignore_gparent = 280,--bool
		effect_id = 225,--int
		target_id = 51,--int
		center_id = 71,--int
		priority = 341,--int
		center_p1_p2_c__tr = 538,--int [-11, 0]
		move_dist = 218,--int
		move_dist_rand = 219,--int
		move_angle = 231,--int
		move_angle_rand = 232,--int
		rfade = 288,--float
		relative = 287,--bool
		xy_mode = 241,--bool
		movex = 237,--int
		movex_rand = 238,--int
		movey = 239,--int
		movey_rand = 240,--int
	},
	area_rotate_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		ease_out = 261,--bool
		easing2 = 248,--int [0, 18]
		easing2_rate = 249,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		dir_buttons_dir = 262,--int [0, 2]<br>all, horiz, vert
		dir_buttons_inwards = 276,--bool
		dir_buttons_single_arrow = 283,--bool<br>seems to be true when DIR is all
		de_ap = 539,--bool
		ignore_linked = 281,--bool
		ignore_gparent = 280,--bool
		effect_id = 225,--int
		target_id = 51,--int
		center_id = 71,--int
		priority = 341,--int
		center_p1_p2_c__tr = 538,--int [-11, 0]
		rotation = 270,--float
		rotation_rand = 271,--float
	},
	area_scale_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		ease_out = 261,--bool
		easing2 = 248,--int [0, 18]
		easing2_rate = 249,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		dir_buttons_dir = 262,--int [0, 2]<br>all, horiz, vert
		dir_buttons_inwards = 276,--bool
		dir_buttons_single_arrow = 283,--bool<br>seems to be true when DIR is all
		de_ap = 539,--bool
		ignore_linked = 281,--bool
		ignore_gparent = 280,--bool
		effect_id = 225,--int
		target_id = 51,--int
		center_id = 71,--int
		priority = 341,--int
		center_p1_p2_c__tr = 538,--int [-11, 0]
		scale_x = 233,--float
		scale_x_rand = 234,--float
		scale_y = 235,--float
		scale_y_rand = 236,--float
	},
	area_fade_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing2_rate = 249,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		dir_buttons_dir = 262,--int [0, 2]<br>all, horiz, vert
		dir_buttons_inwards = 276,--bool
		dir_buttons_single_arrow = 283,--bool<br>seems to be true when DIR is all
		de_ap = 539,--bool
		ignore_linked = 281,--bool
		ignore_gparent = 280,--bool
		effect_id = 225,--int
		target_id = 51,--int
		center_id = 71,--int
		priority = 341,--int
		center_p1_p2_c__tr = 538,--int [-11, 0]
		from_opacity = 286,--float
		to_opacity = 275,--float
	},
	area_tint_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing2_rate = 249,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		dir_buttons_dir = 262,--int [0, 2]<br>all, horiz, vert
		dir_buttons_inwards = 276,--bool
		dir_buttons_single_arrow = 283,--bool<br>seems to be true when DIR is all
		de_ap = 539,--bool
		ignore_linked = 281,--bool
		ignore_gparent = 280,--bool
		effect_id = 225,--int
		target_id = 51,--int
		center_id = 71,--int
		priority = 341,--int
		center_p1_p2_c__tr = 538,--int [-11, 0]
		color_channel = 260,--int
		tint = 265,--float
		main_only = 65,--bool
		secondary_only = 66,--bool
		hsv_on = 278,--bool
		hsv = 49,--hsv string
	},
	edit_area_move_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		group_effect_id = 51,--int
		use_eid = 355,--bool
		move_dist = 218,--int
		move_dist_rand = 219,--int
		move_angle = 231,--int
		move_angle_rand = 232,--int
		rfade = 288,--float
		xy_mode = 241,--bool
		movex = 237,--int
		movex_rand = 238,--int
		movey = 239,--int
		movey_rand = 240,--int
	},
	edit_area_rotate_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		group_effect_id = 51,--int
		use_eid = 355,--bool
		rotation = 270,--float
		rotation_rand = 271,--float
	},
	edit_area_scale_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		group_effect_id = 51,--int
		use_eid = 355,--bool
		scale_x = 233,--float
		scale_x_rand = 234,--float
		scale_y = 235,--float
		scale_y_rand = 236,--float
	},
	edit_area_fade_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		group_effect_id = 51,--int
		use_eid = 355,--bool
		to_opacity = 275,--float
	},
	edit_area_tint_trigger = {
		length = 222,--int
		length_rand = 223,--int
		offset = 220,--int
		offset_rand = 221,--int
		easing = 242,--int [0, 18]
		easing_rate = 243,--float
		offset_y = 252,--int
		offset_y_rand = 253,--int
		mod_front = 263,--float
		mod_back = 264,--float
		deadzone = 282,--float
		group_effect_id = 51,--int
		use_eid = 355,--bool
		tint = 265,--float
		hsv_on = 278,--bool
		hsv = 49,--hsv string
	},
	stop_area_trigger = {
		effect_id = 51,--int
	},
	background_trigger = {
		background = 533,--int [1, 59]
	},
	ground_trigger = {
		ground = 533,--int [1, 22]
	},
	middle_ground_trigger = {
		middle_ground = 533,--int [0, 3]
	},
	touch_trigger = {
		group_id = 51,--int
		hold_mode = 81,--bool
		p1_p2_only = 198,--int [0, 2]
		toggle_on_off = 82,--int [0, 2]
		dual_mode = 89,--bool
	},
	count_trigger = {
		item_id = 80,--int
		target_id = 51,--int
		target_count = 77,--int
		activate_group = 56,--bool
		multi_activate = 104,--bool
	},
	instant_count_trigger = {
		item_id = 80,--int
		target_id = 51,--int
		target_count = 77,--int
		activate_group = 56,--bool
		equals_larger_smaller = 88,--int [0, 2]
	},
	gradient_trigger = {
		blending = 174,--int [0, 3]
		layer = 202,--int [1, 15]
		u_bl = 203,--int
		d_br = 204,--int
		l_tl = 205,--int
		r_tr = 206,--int
		vertex_mode = 207,--bool
		disable = 208,--bool
		id = 209,--int
		preview_opacity = 456,--float
		disable_all = 508,--bool
	},
	static_camera_trigger = {
		target_group_id = 71,--int
		follow = 212,--bool
		follow_easing = 213,--float
		smooth_velocity = 453,--bool
		smooth_velocity_modifier = 454,--float
		exit_instant = 465,--bool
		exit_static = 110,--bool
	},
	camera_zoom_trigger = {
		zoom = 371,--float
	},
	camera_rotate_trigger = {
		degrees = 68,--float
		add = 70,--bool
		snap360 = 394,--bool
	},
	camera_edge_trigger = {
		edge = 164,--int [0, 4]
		target_id = 51,--int
	},
	camera_mode_trigger = {
		free_mode = 111,--bool
		edit_camera_settings = 112,--bool
		easing = 113,--int
		padding = 114,--float
		disable_gridsnap = 370,--bool
	},
	camera_guide = {
		zoom = 371,--float
		offsetx = 28,--int
		offsety = 29,--int
		preview_opacity = 506,--float
	},
	camera_offset_trigger = {
		offsetx = 28,--int
		offsety = 29,--int
		x_y_only = 101,--int [0, 2]
	},
	song_trigger = {
		volume = 406,--float
		speed = 404,--int
		channel = 432,--int
		loop = 413,--bool
		start = 408,--int
		fade_in = 409,--int
		end_ = 410,--int
		fade_out = 411,--int
		prep = 399,--bool
		load_prep = 400,--bool
		song = 392,--int
	},
	sfx_trigger = {
		sfx_id = 392,--int
		sound_duration = 490,--float<br>affected by changing sfx, speed, start/end
		speed = 404,--int
		pitch = 405,--int
		volume = 406,--float
		reverb = 407,--bool
		reverb_type = 502,--int [0, 22]
		reverb_enable = 503,--bool
		fft = 412,--bool
		loop = 413,--bool
		pre_load = 433,--bool
		start = 408,--int
		end_ = 410,--int
		fade_in = 409,--int
		fade_out = 411,--int
		unique_id = 416,--int
		sfx_group = 455,--int
		is_unique = 415,--bool
		override = 420,--bool
		min_interval = 434,--float
		ignore_volume_test = 489,--bool
		group_id_1 = 51,--int
		group_id_2 = 71,--int
		p1 = 138,--bool
		p2 = 200,--bool
		cam = 428,--bool
		vol_near = 421,--float
		vol_med = 422,--float
		vol_far = 423,--float
		min_dist = 424,--int
		dist_2 = 425,--int
		dist_3 = 426,--int
		dir_buttons = 458,--int [0, 6]
	},
	pickup_trigger = {
		item_id = 80,--int
		count = 77,--int
		override = 139,--bool
		multiply_divide = 88,--int [0, 2]
		modifier = 449,--float
	},
	time_trigger = {
		start_time = 467,--float
		stop_time = 473,--float
		stop_checked = 474,--bool
		item_id = 80,--int
		target_id = 51,--int
		time_mod = 470,--float
		ignore_timewarp = 469,--bool
		start_paused = 471,--bool
		dont_override = 468,--bool
	},
	time_event_trigger = {
		item_id = 80,--int
		target_id = 51,--int
		target_time = 473,--float
		multi_activate = 475,--bool
	},
	time_control_trigger = {
		item_id = 80,--int
		start_stop = 472,--bool
	},
	item_edit_trigger = {
		item_id_1 = 80,--int
		item_id_2 = 95,--int
		target_item_id = 51,--int
		typ_1 = 476,--int [0, 5]
		typ_2 = 477,--int [0, 5]
		target_typ = 478,--int [1, 3]
		mod = 479,--float
		assign_op = 480,--int [0, 4]
		op1 = 481,--int [1, 4]
		op2 = 482,--int [3, 4]
		abs_neg_1 = 578,--int [0, 2]
		abs_neg_2 = 579,--int [0, 2]
		rnd_flr_cei_1 = 485,--int [0, 3]
		rnd_flr_cei_2 = 486,--int [0, 3]
	},
	item_compare_trigger = {
		item_id_1 = 80,--int
		item_id_2 = 95,--int
		typ_1 = 476,--int [0, 5]
		typ_2 = 477,--int [0, 5]
		mod1 = 479,--float
		mod2 = 483,--float
		true_id = 51,--int
		false_id = 71,--int
		tol_rand = 484,--float
		op1 = 480,--int [1, 4]
		op2 = 481,--int [1, 4]
		compare_op = 482,--int [0, 5]
		abs_neg_1 = 578,--int [0, 2]
		abs_neg_2 = 579,--int [0, 2]
		rnd_flr_cei_1 = 485,--int [0, 3]
		rnd_flr_cei_2 = 486,--int [0, 3]
	},
	item_persistence_trigger = {
		item_id = 80,--int
		timer = 494,--bool
		persistent = 491,--bool
		target_all = 492,--bool
		reset = 493,--bool
	},
	random_trigger = {
		group_id_1 = 51,--int
		group_id_2 = 71,--int
		chance = 10,--float
	},
	advanced_random_trigger = {
		list = 152,--string<br>example: 2.10.3.15 -> g2 weight 10, g3 weight 15
	},
	sequence_trigger = {
		sequence = 435,--string<br>period separated, similar to spawn remap
		min_int = 437,--float
		reset = 438,--float
		mode = 438,--int [0, 2]
		reset_full_step = 439,--bool
	},
	spawn_particle_trigger = {
		particle_group = 51,--int
		position_group = 71,--int
		offset_x = 547,--int
		offset_y = 548,--int
		offvar_x = 549,--int
		offvar_y = 550,--int
		rotation = 552,--int
		rotation_rand = 553,--int
		scale = 554,--float
		scale_rand = 555,--float
		match_rot = 551,--bool
	},
	reset_trigger = {
		group_id = 51,--int
	},
	rotate_gameplay___arrow_trigger = {
		edit_velocity = 169,--bool
		velocity_mod_x = 582,--float
		velocity_mod_y = 583,--float
		override_velocity = 584,--bool
		change_channel = 171,--bool
		channel_only = 172,--bool
		target_channel = 173,--int
		dont_slide = 585,--bool
		instant_offset = 368,--bool
		arrow_dir = 167,--int [1, 4]<br>1: up, 2: down, 3: left, 4: right
		gravity_dir = 166,--int [1, 4]<br>1: up, 2: down, 3: left, 4: right
	},
	event_trigger = {
		group_id = 51,--int
		extra_id = 447,--int
		extra_id_2 = 525,--int
		events = 430,--string<br>period separated list
	},
	timewarp_trigger = {
		time_mod = 120,--float
	},
	counter_display = {
		item_id = 80,--int
		time_counter = 466,--bool
		align = 391,--int [0, 2]
		seconds_only = 389,--bool
		maintime_points_attempts = 390,--int [-3, 0]
	},
	ui_trigger = {
		group_id = 51,--int
		ui_target = 71,--int
		xref_pos = 385,--int [1, 4]
		yref_pos = 386,--int [1, 4]
		xref_relative = 387,--bool
		yref_relative = 388,--bool
	},
	collision_trigger = {
		block_a_id = 80,--int
		block_b_id = 95,--int
		target_id = 51,--int
		activate_group = 56,--bool
		trigger_on_exit = 93,--bool
		p1 = 138,--bool
		p2 = 200,--bool
		pp = 201,--bool
	},
	instant_collision_trigger = {
		block_a_id = 80,--int
		block_b_id = 95,--int
		true_id = 51,--int
		false_id = 71,--int
		p1 = 138,--bool
		p2 = 200,--bool
		pp = 201,--bool
	},
	collision_state_block = {
		state_on = 51,--int
		state_off = 71,--int
	},
	collision_block = {
		block_id = 80,--int
		dynamic_block = 94,--bool
	},
	toggle_block = {
		group_id = 51,--int
		activate_group = 56,--bool
		claim_touch = 445,--bool
		spawn_only = 504,--bool
		multi_activate = 99,--bool
	},
	options_trigger = {
		streak_additive = 159,--int [-1, 1]
		hide_ground = 161,--int [-1, 1]
		hide_mg = 195,--int [-1, 1]
		hide_p1 = 162,--int [-1, 1]
		hide_p2 = 163,--int [-1, 1]
		disable_p1_controls = 165,--int [-1, 1]
		disable_p2_controls = 199,--int [-1, 1]
		unlink_dual_gravity = 160,--int [-1, 1]
		hide_attempts = 532,--int [-1, 1]
		audio_on_death = 575,--int [-1, 1]
		no_death_sfx = 576,--int [-1, 1]
		edit_respawn_time = 573,--int [-1, 1]
		respawn_time = 574,--float
	},
	on_death_trigger = {
		group_id = 51,--int
		activate_group = 56,--bool
	},
	gravity_trigger = {
		gravity = 148,--float
		p1 = 138,--bool
		p2 = 200,--bool
		pt = 201,--bool
	},
	player_control_trigger = {
		p1 = 138,--bool
		p2 = 200,--bool
		stop_jump = 540,--bool
		stop_move = 541,--bool
		stop_rot = 542,--bool
		stop_slide = 543,--bool
	},
}
