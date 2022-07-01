/**
 * @file
 * @brief Zap definitions. See zap_info struct in beam.cc.
**/

#pragma once

/*
struct zap_info
{
    zap_type ztype;
    const char* name;
    int player_power_cap;
    dam_deducer* player_damage;
    tohit_deducer* player_tohit;       // Enchantments have power modifier here
    dam_deducer* monster_damage;
    tohit_deducer* monster_tohit;      // Enchantments have power modifier here
    colour_t colour;
    bool is_enchantment;
    beam_type flavour;
    dungeon_char_type glyph;
    bool can_beam;
    bool is_explosion;
    int hit_loudness;
}
*/

/// Boilerplate monster hex.
static zap_info _mon_hex_zap(zap_type ztype, beam_type beam,
                             int player_pow_cap = 100,
                             colour_t colour = BLACK)
{
    return {
        ztype,
        "",
        player_pow_cap,
        nullptr,
        nullptr,
        nullptr,
        new tohit_calculator<0, 1, 3>, // ENCH_POW_FACTOR
        colour,
        true,
        beam,
        NUM_DCHAR_TYPES,
        false,
        false,
        0
    };
}

static const zap_info zap_data[] =
{

{
    ZAP_NOXIOUS_CLOUD,
    "noxious blast",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<1, 0, 0, 1>,
    new tohit_calculator<18, 1, 25>,
    LIGHTGREEN,
    false,
    BEAM_POISON,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_POISONOUS_CLOUD,
    "blast of poison",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 3, 1, 25>,
    new tohit_calculator<18, 1, 25>,
    LIGHTGREEN,
    false,
    BEAM_POISON,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_ENERGY_BOLT,
    "bolt of energy",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 20, 0, 1>,
    new tohit_calculator<15, 1, 30>,
    YELLOW,
    false,
    BEAM_DEVASTATION,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_METAL_SPLINTERS,
    "spray of metal splinters",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 20, 1, 20>,
    new tohit_calculator<19, 1, 30>,
    CYAN,
    false,
    BEAM_FRAG,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_SPLINTERSPRAY,
    "spray of wooden splinters",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 15, 1, 20>,
    new tohit_calculator<17, 1, 30>,
    BROWN,
    false,
    BEAM_FRAG,
    DCHAR_FIRED_MISSILE,
    true,
    false,
    0
},

{
    ZAP_THORN_VOLLEY,
    "volley of thorns",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 5, 1, 13>,
    new tohit_calculator<20, 1, 15>,
    BROWN,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    0
},

{
    ZAP_THROW_BARBS,
    "volley of spikes",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<2, 13, 0, 1>,
    new tohit_calculator<27>,
    LIGHTGREY,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    0
},

{
    ZAP_HURL_SLUDGE,
    "toxic sludge",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 3, 1, 16>,
    new tohit_calculator<20, 1, 15>,
    LIGHTGREEN,
    false,
    BEAM_POISON_ARROW,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    0
},

{
    ZAP_CALL_DOWN_DAMNATION,
    "damnation",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 15, 0, 10>,
    new tohit_calculator<20>,
    LIGHTRED,
    false,
    BEAM_DAMNATION,
    DCHAR_FIRED_ZAP,
    false,
    true,
    0
},

{
    ZAP_MIASMA_BREATH,
    "foul vapour",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 5, 1, 24>,
    new tohit_calculator<17, 1, 20>,
    DARKGREY,
    false,
    BEAM_MIASMA,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_HOLY_BREATH,
    "blast of cleansing flame",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 0, 1, 12>,
    new tohit_calculator<18, 1, 25>,
    ETC_HOLY,
    false,
    BEAM_HOLY,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_PETRIFYING_CLOUD,
    "blast of calcifying dust",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<2, 6, 0, 1>,
    new tohit_calculator<AUTOMATIC_HIT>,
    WHITE,
    false,
    BEAM_PETRIFYING_CLOUD,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_GHOSTLY_FIREBALL,
    "ghostly fireball",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 6, 1, 13>,
    new tohit_calculator<40>,
    CYAN,
    false,
    BEAM_NEG,
    DCHAR_FIRED_ZAP,
    false,
    true,
    0 // Noise comes from explosion
},

{
    ZAP_FLASH_FREEZE,
    "flash freeze",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 7, 1, 12>,
    new tohit_calculator<5, 1, 3>,
    WHITE,
    false,
    BEAM_ICE,
    DCHAR_FIRED_ZAP,
    false,
    false,
    0
},

{
    ZAP_SPIT_LAVA,
    "glob of lava",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 10, 0, 1>,
    new tohit_calculator<20>,
    RED,
    false,
    BEAM_LAVA,
    DCHAR_FIRED_ZAP,
    false,
    false,
    0
},

{
    ZAP_CHAOS_BREATH,
    "blast of chaos",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<1, 0, 1, 8>,
    new tohit_calculator<30>,
    ETC_RANDOM,
    false,
    BEAM_CHAOS,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_COLD_BREATH,
    "blast of cold",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 0, 1, 6>,
    new tohit_calculator<30>,
    WHITE,
    false,
    BEAM_COLD,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_FIRE_BREATH,
    "blast of flame",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 0, 1, 6>,
    new tohit_calculator<30>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_SEARING_BREATH,
    "searing blast",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 0, 1, 9>,
    new tohit_calculator<30>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_ELECTRICAL_BOLT,
    "bolt of electricity",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 3, 1, 12>,
    new tohit_calculator<35>,
    LIGHTCYAN,
    false,
    BEAM_ELECTRICITY,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_FLAMING_CLOUD,
    "blast of flame",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<1, 0, 1, 8>,
    new tohit_calculator<30>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_THROW_FLAME,
    "puff of flame",
    50,
    new dicedef_calculator<2, 4, 1, 10>,
    new tohit_calculator<8, 1, 10>,
    new dicedef_calculator<3, 5, 1, 40>,
    new tohit_calculator<25, 1, 40>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    false,
    false,
    2
},

{
    ZAP_THROW_FROST,
    "puff of frost",
    50,
    new dicedef_calculator<2, 4, 1, 10>,
    new tohit_calculator<8, 1, 10>,
    new dicedef_calculator<3, 5, 1, 40>,
    new tohit_calculator<25, 1, 40>,
    WHITE,
    false,
    BEAM_COLD,
    DCHAR_FIRED_ZAP,
    false,
    false,
    2
},

{
    ZAP_SLOW,
    "",
    100,
    nullptr,
    new tohit_calculator<0, 3, 2>,
    nullptr,
    new tohit_calculator<0, 1, 3>,
    BLACK,
    true,
    BEAM_SLOW,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_MEPHITIC,
    "stinking cloud",
    100,
    nullptr,
    new tohit_calculator<AUTOMATIC_HIT>,
    nullptr,
    nullptr,
    GREEN,
    false,
    BEAM_MEPHITIC,
    DCHAR_FIRED_ZAP,
    false,
    true,
    0 // Noise comes from explosion
},

{
    ZAP_HASTE,
    "",
    100,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    BLACK,
    true,
    BEAM_HASTE,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_MAGIC_DART,
    "magic dart",
    25,
    new dicedef_calculator<1, 3, 1, 5>,
    new tohit_calculator<AUTOMATIC_HIT>,
    new dicedef_calculator<3, 4, 1, 100>,
    new tohit_calculator<AUTOMATIC_HIT>,
    LIGHTMAGENTA,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_ZAP,
    false,
    false,
    1
},

_mon_hex_zap(ZAP_PARALYSE, BEAM_PARALYSIS),

{
    ZAP_BOLT_OF_FIRE,
    "bolt of fire",
    200,
    new calcdice_calculator<6, 18, 2, 3>,
    new tohit_calculator<10, 1, 25>,
    new dicedef_calculator<3, 8, 1, 11>,
    new tohit_calculator<17, 1, 25>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{
    ZAP_BOLT_OF_COLD,
    "bolt of cold",
    200,
    new calcdice_calculator<6, 18, 2, 3>,
    new tohit_calculator<10, 1, 25>,
    new dicedef_calculator<3, 8, 1, 11>,
    new tohit_calculator<17, 1, 25>,
    WHITE,
    false,
    BEAM_COLD,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{ // Used only by phial of floods
    ZAP_PRIMAL_WAVE,
    "torrent of water",
    200,
    new calcdice_calculator<4, 14, 3, 5>,
    new tohit_calculator<10, 1, 25>,
    // Water attack is weaker than the pure elemental damage attacks, but also
    // less resistible.
    new dicedef_calculator<3, 6, 1, 12>,
    // Huge wave of water is hard to dodge.
    new tohit_calculator<14, 1, 35>,
    LIGHTBLUE,
    false,
    BEAM_WATER,
    DCHAR_WAVY,
    false,
    false,
    6
},

_mon_hex_zap(ZAP_CONFUSE, BEAM_CONFUSION),
_mon_hex_zap(ZAP_TUKIMAS_DANCE, BEAM_TUKIMAS_DANCE, 100),

{
    ZAP_INVISIBILITY,
    "",
    100,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    BLACK,
    true,
    BEAM_INVISIBILITY,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_DIG,
    "",
    100,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    BLACK,
    true,
    BEAM_DIGGING,
    NUM_DCHAR_TYPES,
    true,
    false,
    4
},

{
    ZAP_FASTROOT,
    "",
    200,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    BROWN,
    true,
    BEAM_ROOTS,
    DCHAR_FIRED_BURST,
    false,
    true,
    0 // Noise comes from explosion
},

{
    ZAP_FIREBALL,
    "fireball",
    200,
    new calcdice_calculator<3, 10, 1, 2>,
    new tohit_calculator<40>,
    new dicedef_calculator<3, 7, 1, 10>,
    new tohit_calculator<40>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    false,
    true,
    0 // Noise comes from explosion
},


{
    ZAP_FLAME_WAVE,
    "flame wave",
    100,
    new calcdice_calculator<2, 9, 1, 4>,
    new tohit_calculator<40>,
    nullptr,
    nullptr,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    false,
    true,
    0 // Noise comes from explosion
},

{
    ZAP_IGNITION,
    "fireball",
    200,
    new calcdice_calculator<3, 10, 1, 3>, // less than fireball
    new tohit_calculator<40>,
    nullptr,
    nullptr,
    RED,
    false,
    BEAM_FIRE,
    NUM_DCHAR_TYPES,
    false,
    true,
    0
},

{
    ZAP_TELEPORT_OTHER,
    "",
    100,
    nullptr,
    new tohit_calculator<0, 3, 2>,
    nullptr,
    new tohit_calculator<0, 1, 3>,
    BLACK,
    true,
    BEAM_TELEPORT,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_LIGHTNING_BOLT,
    "bolt of lightning",
    200,
    new calcdice_calculator<1, 11, 3, 5>,
    new tohit_calculator<7, 1, 40>,
    new dicedef_calculator<3, 10, 1, 17>,
    new tohit_calculator<16, 1, 40>,
    LIGHTCYAN,
    false,
    BEAM_ELECTRICITY,
    DCHAR_FIRED_ZAP,
    true,
    false,
    5 // XXX: Maybe louder?
},

_mon_hex_zap(ZAP_POLYMORPH, BEAM_POLYMORPH),

{
    ZAP_VENOM_BOLT,
    "bolt of poison",
    200,
    new calcdice_calculator<4, 16, 3, 5>,
    new tohit_calculator<8, 1, 20>,
    new dicedef_calculator<3, 6, 1, 13>,
    new tohit_calculator<19, 1, 20>,
    LIGHTGREEN,
    false,
    BEAM_POISON,
    DCHAR_FIRED_ZAP,
    true,
    false,
    5 // XXX: Quieter because it's poison?
},

{
    ZAP_BOLT_OF_DRAINING,
    "bolt of negative energy",
    200,
    new calcdice_calculator<4, 15, 3, 5>,
    new tohit_calculator<8, 1, 20>,
    new dicedef_calculator<3, 9, 1, 13>,
    new tohit_calculator<16, 1, 35>,
    DARKGREY,
    false,
    BEAM_NEG,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0 // Draining is soundless
},

{
    ZAP_LEHUDIBS_CRYSTAL_SPEAR,      // was splinters
    "crystal spear",
    200,
    new calcdice_calculator<10, 23, 1, 1>,
    new tohit_calculator<10, 1, 15>,
    new dicedef_calculator<3, 16, 1, 10>,
    new tohit_calculator<22, 1, 20>,
    WHITE,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    8
},

_mon_hex_zap(ZAP_CHARMING, BEAM_CHARM),

{
    ZAP_PAIN,
    "",
    100,
    new dicedef_calculator<1, 4, 1, 5>,
    new tohit_calculator<0, 7, 2>,
    new dicedef_calculator<1, 7, 1, 20>,
    new tohit_calculator<0, 1, 3>,
    BLACK,
    true,
    BEAM_PAIN,
    NUM_DCHAR_TYPES,
    false,
    false,
    1 // XXX: Should this be soundless?
},

{
    ZAP_STICKY_FLAME,
    "sticky flame",
    100,
    new dicedef_calculator<2, 3, 1, 12>,
    new tohit_calculator<AUTOMATIC_HIT>,
    new dicedef_calculator<3, 3, 1, 50>,
    new tohit_calculator<AUTOMATIC_HIT>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    false,
    false,
    1
},

{
    ZAP_STICKY_FLAME_RANGE,
    "sticky flame",
    100,
    new dicedef_calculator<2, 3, 1, 12>,
    new tohit_calculator<AUTOMATIC_HIT>,
    new dicedef_calculator<3, 3, 1, 50>,
    new tohit_calculator<18, 1, 15>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    false,
    false,
    1
},

{
    ZAP_DISPEL_UNDEAD,
    "",
    100,
    new calcdice_calculator<3, 20, 3, 4>,
    new tohit_calculator<0, 3, 2>,
    new dicedef_calculator<3, 6, 1, 10>,
    new tohit_calculator<AUTOMATIC_HIT>,
    BLACK,
    true,
    BEAM_DISPEL_UNDEAD,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_DISPEL_UNDEAD_RANGE,
    "",
    100,
    new calcdice_calculator<3, 20, 3, 4>,
    new tohit_calculator<0, 3, 2>,
    new dicedef_calculator<3, 6, 1, 10>,
    new tohit_calculator<AUTOMATIC_HIT>,
    BLACK,
    true,
    BEAM_DISPEL_UNDEAD,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

_mon_hex_zap(ZAP_BANISHMENT, BEAM_BANISH, 150),

{
    ZAP_STING,
    "sting",
    25,
    new dicedef_calculator<1, 3, 1, 5>,
    new tohit_calculator<8, 1, 5>,
    new dicedef_calculator<1, 6, 1, 25>,
    new tohit_calculator<60, 0, 1>,
    GREEN,
    false,
    BEAM_POISON_ARROW,
    DCHAR_FIRED_ZAP,
    false,
    false,
    1 // XXX: Maybe silent because it's poison?
},

{
    ZAP_HURL_DAMNATION,
    "damnation",
    200,
    new calcdice_calculator<3, 8, 3, 5>,
    new tohit_calculator<20, 1, 10>,
    new dicedef_calculator<3, 20, 0, 10>,
    new tohit_calculator<24>,
    LIGHTRED,
    false,
    BEAM_DAMNATION,
    DCHAR_FIRED_ZAP,
    false,
    true,
    9 // XXX: Even louder because it's hellish?
},

{
    ZAP_IRON_SHOT,
    "iron shot",
    200,
    new calcdice_calculator<9, 15, 3, 4>,
    new tohit_calculator<7, 1, 15>,
    new dicedef_calculator<3, 8, 1, 9>,
    new tohit_calculator<20, 1, 25>,
    LIGHTCYAN,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    6
},

{
    ZAP_STONE_ARROW,
    "stone arrow",
    50,
    new dicedef_calculator<3, 7, 1, 8>,
    new tohit_calculator<8, 1, 10>,
    new dicedef_calculator<3, 5, 1, 10>,
    new tohit_calculator<14, 1, 35>,
    LIGHTGREY,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    3
},

{
    ZAP_SHOCK,
    "zap",
    25,
    new dicedef_calculator<1, 3, 1, 4>,
    new tohit_calculator<8, 1, 7>,
    new dicedef_calculator<1, 8, 1, 20>,
    new tohit_calculator<17, 1, 20>,
    LIGHTCYAN,
    false,
    BEAM_ELECTRICITY,             // beams & reflects
    DCHAR_FIRED_ZAP,
    true,
    false,
    1 // XXX: maybe electricity should be louder?
},

{
    ZAP_STUNNING_BURST,
    "stunning burst",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<2, 8, 1, 20>,
    new tohit_calculator<15, 1, 10>,
    LIGHTCYAN,
    false,
    BEAM_STUN_BOLT,
    DCHAR_FIRED_ZAP,
    true,
    false,
    8
},

{
    ZAP_ORB_OF_ELECTRICITY,
    "orb of electricity",
    200,
    new calcdice_calculator<0, 13, 4, 5>,
    new tohit_calculator<40>,
    new dicedef_calculator<3, 7, 1, 9>,
    new tohit_calculator<40>,
    LIGHTBLUE,
    false,
    BEAM_ELECTRICITY,
    DCHAR_FIRED_ZAP,
    false,
    true,
    6 // XXX: maybe electricity should be louder?
},

{
    ZAP_SPIT_POISON,
    "splash of poison",
    50,
    new dicedef_calculator<1, 4, 1, 2>,
    new tohit_calculator<5, 1, 6>,
    new dicedef_calculator<1, 4, 1, 10>,
    new tohit_calculator<16, 1, 20>,
    GREEN,
    false,
    BEAM_POISON,
    DCHAR_FIRED_ZAP,
    false,
    false,
    1
},

{
    ZAP_DEBUGGING_RAY,
    "debugging ray",
    10000,
    new dicedef_calculator<AUTOMATIC_HIT, 1, 0, 1>,
    new tohit_calculator<AUTOMATIC_HIT>,
    nullptr,
    nullptr,
    WHITE,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_DEBUG,
    false,
    false,
    0
},

// XXX: How loud should breath be?
{
    ZAP_BREATHE_FIRE,
    "fiery breath",
    50,
    new dicedef_calculator<3, 4, 1, 3>,
    new tohit_calculator<8, 1, 6>,
    nullptr,
    nullptr,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    7
},

{
    ZAP_BREATHE_FROST,
    "freezing breath",
    50,
    new dicedef_calculator<3, 4, 1, 3>,
    new tohit_calculator<8, 1, 6>,
    new dicedef_calculator<3, 0, 1, 9>,
    new tohit_calculator<30>,
    WHITE,
    false,
    BEAM_COLD,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{
    ZAP_BREATHE_ACID,
    "glob of acid",
    50,
    new dicedef_calculator<5, 4, 1, 3>,
    new tohit_calculator<7, 1, 6>,
    new dicedef_calculator<3, 14, 0, 1>,
    new tohit_calculator<20, 1, 4>,
    YELLOW,
    false,
    BEAM_ACID,
    DCHAR_FIRED_ZAP,
    false,
    false,
    6
},

{
    ZAP_BREATHE_POISON,
    "poison gas",
    50,
    new dicedef_calculator<3, 2, 1, 6>,
    new tohit_calculator<6, 1, 6>,
    nullptr,
    nullptr,
    GREEN,
    false,
    BEAM_POISON,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0 // Explosion does the noise.
},

{
    ZAP_BREATHE_POWER,
    "bolt of dispelling energy",
    50,
    new dicedef_calculator<3, 3, 1, 3>,
    new tohit_calculator<5, 1, 6>,
    nullptr,
    nullptr,
    BLUE,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{
    ZAP_SPIT_ACID,
    "splash of acid",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 14, 0, 1>,
    new tohit_calculator<20, 1, 4>,
    YELLOW,
    false,
    BEAM_ACID,
    DCHAR_FIRED_ZAP,
    false,
    false,
    6
},

{
    ZAP_AGONY,
    "agony",
    100,
    nullptr,
    new tohit_calculator<0, 5, 1>,
    nullptr,
    new tohit_calculator<0, 1, 3>,
    BLACK,
    true,
    BEAM_AGONY,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_AGONY_RANGE,
    "agony",
    100,
    nullptr,
    new tohit_calculator<0, 5, 1>,
    nullptr,
    new tohit_calculator<0, 1, 3>,
    BLACK,
    true,
    BEAM_AGONY,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_VAMPIRIC_DRAINING,
    "",
    200,
    new dicedef_calculator<2, 6, 1, 14>,
    new tohit_calculator<AUTOMATIC_HIT>,
    new dicedef_calculator<2, 6, 1, 14>,
    new tohit_calculator<AUTOMATIC_HIT>,
    BLACK,
    true,
    BEAM_VAMPIRIC_DRAINING,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_MINDBURST,
    "",
    100,
    new calcdice_calculator<3, 15, 3, 4>,
    new tohit_calculator<0, 5, 2>,
    new calcdice_calculator<1, 30, 1, 10>,
    new tohit_calculator<50, 0, 1>,
    BLACK,
    true,
    BEAM_MINDBURST,
    NUM_DCHAR_TYPES,
    true,
    false,
    6
},

{
    ZAP_BREATHE_STEAM,
    "ball of steam",
    50,
    new dicedef_calculator<3, 4, 1, 5>,
    new tohit_calculator<10, 1, 10>,
    new dicedef_calculator<3, 7, 1, 15>,
    new tohit_calculator<20, 1, 20>,
    LIGHTGREY,
    false,
    BEAM_STEAM,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0 // Explosion does the noise.
},

{
    ZAP_THROW_ICICLE,
    "shard of ice",
    100,
    new calcdice_calculator<3, 10, 1, 2>,
    new tohit_calculator<9, 1, 12>,
    new dicedef_calculator<3, 8, 1, 11>,
    new tohit_calculator<17, 1, 25>,
    WHITE,
    false,
    BEAM_ICE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    4
},

{
    ZAP_CORONA,
    "",
    100,
    nullptr,
    nullptr,
    nullptr,
    new tohit_calculator<0, 1, 3>,
    BLUE,
    true,
    BEAM_CORONA,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

// player spellpower is capped to 50 in spl-zap.cc:spell_zap_power.
_mon_hex_zap(ZAP_HIBERNATION, BEAM_HIBERNATION),

{
    ZAP_FLAME_TONGUE,
    "flame tongue",
    25,
    new dicedef_calculator<1, 8, 1, 4>,
    new tohit_calculator<11, 1, 6>,
    new dicedef_calculator<3, 3, 1, 12>,
    new tohit_calculator<7, 1, 6>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_BOLT,
    false,
    false,
    1
},

{
    ZAP_SANDBLAST,
    "blast of sand",
    50,
    new dicedef_calculator<2, 4, 1, 3>,
    new tohit_calculator<13, 1, 10>,
    new dicedef_calculator<3, 5, 1, 40>,
    new tohit_calculator<20, 1, 40>,
    BROWN,
    false,
    BEAM_FRAG,
    DCHAR_FIRED_BOLT,
    false,
    false,
    2 // XXX: Sound 2 for level one spell?
},

{
    ZAP_BOLT_OF_MAGMA,
    "bolt of magma",
    200,
    new calcdice_calculator<4, 16, 2, 3>,
    new tohit_calculator<8, 1, 25>,
    new dicedef_calculator<3, 8, 1, 11>,
    new tohit_calculator<17, 1, 25>,
    RED,
    false,
    BEAM_LAVA,
    DCHAR_FIRED_ZAP,
    true,
    false,
    5
},

{
    ZAP_POISON_ARROW,
    "poison arrow",
    200,
    new calcdice_calculator<4, 15, 1, 1>,
    new tohit_calculator<5, 1, 10>,
    new dicedef_calculator<3, 7, 1, 12>,
    new tohit_calculator<20, 1, 25>,
    LIGHTGREEN,
    false,
    BEAM_POISON_ARROW,             // extra damage
    DCHAR_FIRED_MISSILE,
    false,
    false,
    6 // XXX: Less noise because it's poison?
},

_mon_hex_zap(ZAP_PETRIFY, BEAM_PETRIFY),
_mon_hex_zap(ZAP_PORKALATOR, BEAM_PORKALATOR, 100, RED),
_mon_hex_zap(ZAP_SLEEP, BEAM_SLEEP),

{
    ZAP_IOOD,
    "",
    200,
    nullptr,
    new tohit_calculator<AUTOMATIC_HIT>,
    nullptr,
    nullptr,
    WHITE,
    false,
    BEAM_DEVASTATION,
    NUM_DCHAR_TYPES, // no dchar, to get bolt.glyph == 0,
                     // hence invisible
    true,
    false,
    0
},

{
    ZAP_BREATHE_MEPHITIC,
    "blast of choking fumes",
    50,
    new dicedef_calculator<3, 2, 1, 6>,
    new tohit_calculator<6, 1, 6>,
    nullptr,
    nullptr,
    GREEN,
    false,
    BEAM_MEPHITIC,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_INNER_FLAME,
    "",
    100,
    nullptr,
    new tohit_calculator<0, 3, 1>,
    nullptr,
    nullptr,
    BLACK,
    true,
    BEAM_INNER_FLAME,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_FORCE_LANCE,
    "lance of force",
    100,
    new dicedef_calculator<2, 6, 1, 6>,
    new tohit_calculator<10, 1, 7>,
    new dicedef_calculator<3, 6, 1, 15>,
    new tohit_calculator<20, 1, 20>,
    CYAN,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    5
},

{
    ZAP_SEARING_RAY,
    "searing ray",
    50,
    new calcdice_calculator<2, 9, 1, 7>,
    new tohit_calculator<11, 1, 8>,
    nullptr,
    nullptr,
    WHITE,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    2
},

{
    ZAP_CRYSTAL_BOLT,
    "crystal bolt",
    200,
    new calcdice_calculator<6, 18, 2, 3>,
    new tohit_calculator<10, 1, 25>,
    new dicedef_calculator<3, 8, 1, 11>,
    new tohit_calculator<17, 1, 25>,
    GREEN,
    false,
    BEAM_CRYSTAL,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{
    ZAP_QUICKSILVER_BOLT,
    "bolt of dispelling energy",
    200,
    new calcdice_calculator<6, 15, 2, 3>,
    new tohit_calculator<10, 1, 25>,
    new dicedef_calculator<3, 20, 0, 1>,
    new tohit_calculator<16, 1, 25>,
    ETC_RANDOM,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{
    ZAP_CORROSIVE_BOLT,
    "bolt of acid",
    200,
    new calcdice_calculator<4, 13, 3, 5>,
    new tohit_calculator<10, 1, 25>,
    new dicedef_calculator<3, 16, 1, 17>,
    new tohit_calculator<17, 1, 25>,
    YELLOW,
    false,
    BEAM_ACID,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{
    ZAP_BOLT_OF_LIGHT,
    "bolt of light",
    200,
    new calcdice_calculator<4, 13, 3, 5>,
    new tohit_calculator<10, 1, 25>,
    new dicedef_calculator<3, 16, 1, 17>,
    new tohit_calculator<17, 1, 25>,
    WHITE,
    false,
    BEAM_LIGHT,
    DCHAR_FIRED_ZAP,
    true,
    false,
    6
},

{
    ZAP_UNRAVELLING,
    "unravelling",
    200,
    new dicedef_calculator<3, 3, 1, 6>,
    new tohit_calculator<0, 1, 1>,
    nullptr,
    nullptr,
    ETC_MUTAGENIC,
    true,
    BEAM_UNRAVELLING,
    NUM_DCHAR_TYPES,
    false,
    false,
    0 // dubious
},

{
    ZAP_ICEBLAST,
    "iceblast",
    200,
    new calcdice_calculator<3, 14, 3, 5>,
    new tohit_calculator<40>,
    new dicedef_calculator<3, 6, 1, 12>, // slightly weaker than magma/fireball
    new tohit_calculator<40>,
    WHITE,
    false,
    BEAM_ICE,
    DCHAR_FIRED_MISSILE,
    false,
    true,
    0 // Noise comes from explosion
},

{
    ZAP_SLUG_DART,
    "slug dart",
    25,
    nullptr,
    nullptr,
    new dicedef_calculator<2, 3, 1, 25>,
    new tohit_calculator<14, 1, 35>,
    CYAN, // match slug's own colour
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    1
},

{
    ZAP_CHAIN_LIGHTNING,
    "bolt of lightning",
    200,
    new calcdice_calculator<3, 0, 2, 1>,
    new tohit_calculator<AUTOMATIC_HIT>,
    new dicedef_calculator<3, 27, 1, 12>,
    new tohit_calculator<AUTOMATIC_HIT>,
    LIGHTCYAN,
    false,
    BEAM_THUNDER,
    DCHAR_FIRED_ZAP,
    true,
    false,
    10,
},

{
    ZAP_BLINKBOLT,
    "living lightning",
    200,
    new dicedef_calculator<1, 11, 3, 5>,
    new tohit_calculator<16, 1, 4>,
    new dicedef_calculator<2, 10, 1, 17>,
    new tohit_calculator<16, 1, 40>,
    LIGHTCYAN,
    false,
    BEAM_ELECTRICITY,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_FREEZING_BLAST,
    "freezing blast",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<2, 9, 1, 11>,
    new tohit_calculator<17, 1, 25>,
    WHITE,
    false,
    BEAM_COLD,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_HARPOON_SHOT,
    "harpoon shot",
    50,
    nullptr,
    nullptr,
    new dicedef_calculator<2, 7, 1, 20>,
    new tohit_calculator<17, 1, 25>,
    CYAN,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    1
},

_mon_hex_zap(ZAP_DIMENSION_ANCHOR, BEAM_DIMENSION_ANCHOR),
_mon_hex_zap(ZAP_VULNERABILITY, BEAM_VULNERABILITY),
_mon_hex_zap(ZAP_SENTINEL_MARK, BEAM_SENTINEL_MARK),
_mon_hex_zap(ZAP_VIRULENCE, BEAM_VIRULENCE),
_mon_hex_zap(ZAP_SAP_MAGIC, BEAM_SAP_MAGIC),
_mon_hex_zap(ZAP_DRAIN_MAGIC, BEAM_DRAIN_MAGIC),

{
    ZAP_BECKONING,
    "beckoning",
    200,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ETC_WARP,
    true,
    BEAM_BECKONING,
    DCHAR_FIRED_ZAP,
    false,
    false,
    0
},

{
    ZAP_FIRE_STORM,
    "great blast of fire",
    200,
    new calcdice_calculator<8, 5, 1, 1>,
    new tohit_calculator<40>,
    new calcdice_calculator<8, 5, 1, 2>,
    new tohit_calculator<40>,
    RED,
    false,
    BEAM_LAVA, // partially unaffected by rF
    DCHAR_FIRED_ZAP,
    false,
    true,
    0 // handled by explosion
},

{
    ZAP_MALMUTATE,
    "malmutate",
    200,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    ETC_MUTAGENIC,
    true,
    BEAM_MALMUTATE,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_VILE_CLUTCH,
    "",
    200,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    GREEN,
    true,
    BEAM_VILE_CLUTCH,
    DCHAR_FIRED_BURST,
    true,
    false,
    5
},

{
    ZAP_REFRIGERATE,
    "refrigeration",
    200,
    new calcdice_calculator<4, 20, 4, 10>,
    new tohit_calculator<AUTOMATIC_HIT>,
    new calcdice_calculator<4, 20, 4, 10>,
    new tohit_calculator<AUTOMATIC_HIT>,
    LIGHTCYAN,
    false,
    BEAM_COLD,
    NUM_DCHAR_TYPES,
    false,
    false,
    0 // No additional effect noise.
},

{
    ZAP_DRAIN_LIFE,
    "drain life",
    200,
    new calcdice_calculator<1, 10, 1, 1>,
    new tohit_calculator<40>,
    new calcdice_calculator<1, 10, 1, 1>,
    new tohit_calculator<40>,
    DARKGREY,
    false,
    BEAM_NEG,
    NUM_DCHAR_TYPES,
    false,
    false,
    0 // No additional effect noise.
},

{
    ZAP_SONIC_WAVE,
    "sonic wave",
    200,
    new calcdice_calculator<2, 5, 1, 5>,
    new tohit_calculator<40>,
    new calcdice_calculator<2, 5, 1, 5>,
    new tohit_calculator<40>,
    YELLOW,
    false,
    BEAM_MMISSILE,
    NUM_DCHAR_TYPES,
    false,
    false,
    0 // No additional effect noise.
},

{
    ZAP_THROW_PIE,
    "klown pie",
    200,
    nullptr,
    nullptr,
    new dicedef_calculator<3, 20, 0, 1>,
    new tohit_calculator<16, 1, 25>,
    BROWN,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_MISSILE,
    false,
    false,
    6
},

{
    ZAP_FOXFIRE,
    "foxfire",
    50,
    new dicedef_calculator<1, 4, 1, 5>,
    new tohit_calculator<AUTOMATIC_HIT>,
    new dicedef_calculator<3, 3, 1, 12>,
    new tohit_calculator<AUTOMATIC_HIT>,
    RED,
    false,
    BEAM_FIRE,
    DCHAR_FIRED_BOLT,
    false,
    false,
    1
},

{
    ZAP_TREMORSTONE,
    "burst of rock shards",
    200,
    new dicedef_calculator<6, 6, 0, 1>,
    new tohit_calculator<40>,
    nullptr,
    nullptr,
    BROWN,
    false,
    BEAM_FRAG,
    DCHAR_FIRED_BURST,
    false,
    true,
    0 // Noise comes from explosion
},

{
    ZAP_HAILSTORM,
    "hail",
    100,
    new calcdice_calculator<3, 10, 1, 2>,
    new tohit_calculator<AUTOMATIC_HIT>,
    nullptr,
    nullptr,
    ETC_ICE,
    false,
    BEAM_ICE,
    DCHAR_FIRED_BURST,
    false,
    false,
    0
},

{
    ZAP_MYSTIC_BLAST,
    "mystic blast",
    100,
    new calcdice_calculator<2, 6, 1, 3>,
    new tohit_calculator<AUTOMATIC_HIT>,
    nullptr,
    nullptr,
    LIGHTMAGENTA,
    false,
    BEAM_MMISSILE,
    DCHAR_FIRED_ZAP,
    true,
    false,
    10
},

{
    ZAP_SWORD_BEAM,
    "bolt of energy",
    200,
    new calcdice_calculator<1, 40, 0, 1>,
    new tohit_calculator<5>,
    new calcdice_calculator<1, 40, 0, 1>,
    new tohit_calculator<5>,
    RED,
    false,
    BEAM_DEVASTATION,
    DCHAR_FIRED_ZAP,
    true,
    false,
    0
},

{
    ZAP_MIGHT,
    "",
    100,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    BLACK,
    true,
    BEAM_MIGHT,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_ENFEEBLE,
    "",
    200,
    nullptr,
    new tohit_calculator<0, 4, 1>,
    nullptr,
    new tohit_calculator<0, 1, 3>,
    BLACK,
    true,
    BEAM_ENFEEBLE,
    NUM_DCHAR_TYPES,
    false,
    false,
    0
},

{
    ZAP_NECROTIZE,
    "",
    100,
    new dicedef_calculator<1, 4, 1, 5>,
    new tohit_calculator<0, 7, 2>,
    nullptr,
    nullptr,
    BLACK,
    true,
    BEAM_NECROTIZE,
    NUM_DCHAR_TYPES,
    false,
    false,
    1 // XXX: Should this be soundless?
},

};
