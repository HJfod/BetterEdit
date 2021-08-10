#pragma once

static int g_objIDs_glow[] = {
    1292,   // small line glow
    1010,   // small turn glow
    1009,   // small corner glow
    1271,   // small 45 slope corner glow
    1272,   // small 22.5 slope corner glow
    1760,   // small 45 slope glow
    1761,   // small 22.5 slope glow
    1887,   // small circle glow

    503,    // line glow
    505,    // turn glow
    504,    // corner glow
    1273,   // 45 slope corner glow
    1274,   // 22.5 slope corner glow
    1291,   // half line glow
    1758,   // 45 slope glow
    1759,   // 22.5 slope glow
    1886,   // circle glow

    1011,   // big line glow
    1013,   // big turn glow
    1012,   // big corner glow
    1269,   // big 45 slope corner glow
    1270,   // big 22.5 slope corner glow
    1293,   // big half line glow
    1762,   // big 45 slope glow
    1763,   // big 22.5 slope glow
    1888,   // big circle glow
};

static int g_objIDs_spikes[] = {
    8,      // normal spike
    39,     // half spike
    103,    // small spike
    392,    // tiny spike

    144,    // invisible spike
    205,    // invisible half spike
    145,    // invisible small spike
    459,    // invisible tiny spike

    216,    // colored spi- i mean spike of color
    217,    // half spike of color
    218,    // small spike of color
    458,    // tiny spike of color

    177,    // iceberg spike
    178,    // iceberg half spike
    179,    // iceberg small spike

    1889,   // fake spike (colorable version)
    1890,   // fake half spike (colorable version)
    1891,   // fake small spike (colorable version)
    1892,   // fake tiny spike (colorable version)

    191,    // fake spike (old version)
    198,    // fake small spike (old version)
    199,    // fake half spike (old version)
};

static int g_objIDs_saws_hazard[] = {
    1705, 1706, 1707,   // normal (colorable ver)
    1708, 1709, 1710,   // 1.9 spikey things (colorable ver)
    1734, 1735, 1736,   // gears (colorable ver)
    678, 679, 680,      // colorable saws
    186, 187, 188,      // line saws
    740, 741, 742,      // invisible saws
    1619, 1620,         // reapers
    183, 184, 185,      // 1.6 spikey things
    88, 89, 98,         // normal (old ver)
    397, 398, 399,      // 1.9 spikey things (old ver)
    675, 676, 677,      // gears (old 1.9 ver)
};

static int g_objIDs_saws_deco[] = {
    85, 86, 87, 97,         // gears
    137, 138, 139,          // inverse gears
    154, 155, 156,          // 1.6 spikey things
    180, 181, 182,          // hampster wheels
    222, 223, 224,          // bouba things (what)
    375, 376, 377, 378,     // pendulums
    1521, 1522, 1523, 1524, // literal fucking lines
    1525, 1526, 1527, 1528, // literal fucking dots
    394, 395, 396,          // hexagons
    997, 998, 999, 1000,    // 2.0 swaggy tech style circles with 4 holes things
    1019, 1020, 1021,       // 2.0 sunset glows
    1055, 1056, 1057,       // circles with extra circles on them
    1058, 1059, 1060, 1061, // fires
    1752,                   // the cool 2.1 triangle fire thing
    1831, 1832,             // quarter circles
    1833, 1834,             // quarter filled circles
};

static int g_objIDs_orbs[] = {
    36,     // yellow
    141,    // pink
    1333,   // red
    84,     // blue
    1022,   // green
    1330,   // black
    1704,   // dash orb
    1751,   // pink dash orb
    1594,   // trigger
};

static int g_objIDs_pads[] = {
    35,     // yellow
    140,    // pink
    1332,   // red
    67,     // blue
};

static int g_objIDs_portals_gamemode[] = {
    12,     // cube
    13,     // ship
    47,     // ball
    111,    // ufo
    660,    // wave
    745,    // robot
    1331,   // spider
};

static int g_objIDs_portals[] = {
    10,     // blue gravity
    11,     // yellow gravity
    45,     // orange mirror
    46,     // blue mirror
    99,     // normal size
    101,    // mini size
    286,    // orange dual
    287,    // blue dual
    747,    // blue teleport
    749,    // orange teleport
};

static int g_objIDs_portals_speed[] = {
    200,    // slow
    201,    // normal
    202,    // fast
    203,    // faster
    1334,   // fasterer
};

static int g_objIDs_layout[] = {
    467,    // block
    468,    // line
    469,    // turn
    470,    // pipe end
    471,    // pipe
    1338,   // 45 slope
    1339,   // 22.5 slope
    472,    // corner
    473,    // 45 corner
    474,    // 22.5 corner

    661,    // small block
    662,    // half block
    663,    // half pipe
    664,    // half pipe end
    1154,   // half line
    1155,   // small turn
    1156,   // small pipe end
    1157,   // small pipe
    1158,   // small corner

    475,    // line with a 22.5 end
    1260,   // wacky inverse line
    1261,   // wacky inverse line corner
    1262,   // wacky thick inverse line
    1263,   // wacky thick inverse line corner
    1264,   // wacky THICC inverse line
    1265,   // wacky THICC inverse line corner

    1210,   // thick block
    1202,   // thick line
    1203,   // thick turn
    1204,   // thick pipe end
    1209,   // thick pipe
    1205,   // thick corner
    1206,   // thick 45 corner
    1207,   // thick 22.5 corner
    1208,   // thick small block

    1227,   // THICC block
    1220,   // THICC line
    1221,   // THICC turn
    1222,   // THICC pipe end
    1226,   // THICC pipe
    1223,   // THICC corner
    1224,   // THICC 45 corner
    1225,   // THICC 22.5 corner
};

static int g_objIDs_triggers[] = {
    899,    // color (modern)
    901,    // move
    1616,   // stop
    1006,   // pulse
    1007,   // alpha
    1049,   // toggle
    1268,   // spawn
    1346,   // rotate
    1347,   // follow
    1520,   // shake
    1585,   // animate
    1814,   // follow player y
    1595,   // touch
    1611,   // count
    1811,   // instant count
    1817,   // pickup
    1815,   // collision
    1812,   // on death
};

static int g_objIDs_triggers_special[] = {
    32,     // enable shadow
    33,     // disable shadow
    1613,   // show player
    1612,   // hide player
    1818,   // bg effect on
    1819,   // bg effect off
};

static int g_objIDs_triggers_objectEnter[] = {
    22, // disable
    23, 24, 25, 26, 27, 28,
    56, 57, 58, 59,
    55, // hidden
};

// IDs of objects that aren't available in the editor
static int g_objIDs_hidden[] = {
    191,    // fake spike (old version)
    198,    // fake small spike (old version)
    199,    // fake half spike (old version)

    55,     // hidden enter type

    61,     // old circular floor hazard

    88, 89, 98,         // normal (old ver)
    397, 398, 399,      // 1.9 spikey things (old ver)
    675, 676, 677,      // gears (old 1.9 ver)
};
