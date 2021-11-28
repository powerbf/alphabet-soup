#pragma once

#include "tag-version.h"

struct form_entry
{
    // Row 1:
    transformation tran;
    monster_type equivalent_mons;
    const char *short_name;
    const char *long_name;
    const char *wiz_name;

    // Row 2:
    const char *description;

    // Row 3:
    int blocked_slots;
    int resists;

    // Row 4:
    FormDuration duration;
    int str_mod;
    int dex_mod;
    size_type size;
    int hp_mod;

    // Row 5
    int flat_ac;
    int power_ac;
    int xl_ac;
    bool can_cast;
    int spellcasting_penalty;
    bool unarmed_hit_bonus;
    int base_unarmed_damage;

    // Row 6
    brand_type uc_brand;
    int uc_colour;
    const char *uc_attack;
    FormAttackVerbs uc_attack_verbs;

    // Row 7
    form_capability can_fly;
    form_capability can_swim;
    form_capability can_bleed;
    bool breathes;
    bool keeps_mutations;

    const char *shout_verb;
    int shout_volume_modifier;
    const char *hand_name;
    const char *foot_name;
    const char *prayer_action;
    const char *flesh_equivalent;
};

static const form_entry formdata[] =
{
{
    transformation::none, MONS_PLAYER, "", "", "none",
    "",
    EQF_NONE, MR_NO_FLAGS,
    FormDuration(0, PS_NONE, 0), 0, 0, SIZE_CHARACTER, 10,
    0, 0, 0, true, 0, false, 3,
    SPWPN_NORMAL, LIGHTGREY, "", DEFAULT_VERBS,
    FC_DEFAULT, FC_DEFAULT, FC_DEFAULT, true, true,
    "", 0, "", "", "", ""
},
{
    transformation::spider, MONS_SPIDER, "Spider", "spider-form", "spider",
    "a venomous arachnid creature",
    EQF_PHYSICAL, MR_VUL_POISON,
    FormDuration(10, PS_DOUBLE, 60), 0, 5, SIZE_TINY, 10,
    2, 0, 0, true, 10, true, 5,
    SPWPN_VENOM, LIGHTGREEN, "Fangs", ANIMAL_VERBS,
    FC_DEFAULT, FC_FORBID, FC_FORBID, true, false,
    "hiss", -4, "front leg", "", "You crawl onto the altar of %s.", "flesh"
},
{
    transformation::blade_hands, MONS_PLAYER, "Blade", "", "blade",
    "",
    EQF_HANDS, MR_NO_FLAGS,
    FormDuration(10, PS_SINGLE, 100), 0, 0, SIZE_CHARACTER, 10,
    0, 0, 0, true, 20, true, 22,
    SPWPN_NORMAL, RED, "", { FAV_HIT, FAV_SLASH, FAV_SLICE, FAV_SHRED },
    FC_DEFAULT, FC_DEFAULT, FC_DEFAULT, true, true,
    "", 0, "scythe-like blade", "", "", ""
},
{
    transformation::statue, MONS_STATUE, "Statue", "statue-form", "statue",
    "a stone statue",
    EQF_STATUE, MR_RES_ELEC | MR_RES_NEG | MR_RES_PETRIFY,
    DEFAULT_DURATION, 0, 0, SIZE_CHARACTER, 13,
    20, 12, 0, true, 0, true, 12,
    SPWPN_NORMAL, LIGHTGREY, "", DEFAULT_VERBS,
    FC_DEFAULT, FC_FORBID, FC_FORBID, false, true,
    "", 0, "", "", "You place yourself before the altar of %s.", "stone"
},
{
    transformation::ice_beast, MONS_ICE_BEAST, "Ice", "ice-form", "ice",
    "a creature of crystalline ice",
    EQF_PHYSICAL, MR_RES_POISON | MR_VUL_FIRE | mrd(MR_RES_COLD, 3),
    FormDuration(30, PS_DOUBLE, 100), 0, 0, SIZE_LARGE, 12,
    5, 7, 0, true, 0, true, 12,
    SPWPN_FREEZING, WHITE, "", DEFAULT_VERBS,
    FC_DEFAULT, FC_ENABLE, FC_FORBID, true, false,
    "", 0, "front paw", "paw", "You bow your head before the altar of %s.", "ice"
},

{
    transformation::dragon, MONS_PROGRAM_BUG, "Dragon", "dragon-form", "dragon",
    "a fearsome dragon",
    EQF_PHYSICAL, MR_RES_POISON,
    DEFAULT_DURATION, 10, 0, SIZE_GIANT, 15,
    16, 0, 0, true, 0, true, 32,
    SPWPN_NORMAL, GREEN, "Teeth and claws", { FAV_HIT, FAV_CLAW, FAV_BITE, FAV_MAUL },
    FC_ENABLE, FC_FORBID, FC_ENABLE, true, false,
    "roar", 6, "foreclaw", "", "You bow your head before the altar of %s.", "flesh"
},

{
    transformation::lich, MONS_LICH, "Lich", "lich-form", "lich",
    "a lich",
    EQF_NONE, MR_RES_COLD | mrd(MR_RES_NEG, 3),
    DEFAULT_DURATION, 0, 0, SIZE_CHARACTER, 10,
    6, 0, 0, true, 0, true, 5,
    SPWPN_DRAINING, MAGENTA, "", DEFAULT_VERBS,
    FC_DEFAULT, FC_DEFAULT, FC_FORBID, false, true,
    "", 0, "", "", "", "bone"
},

{
    transformation::bat, MONS_PROGRAM_BUG, "Bat", "bat-form", "bat",
    "",
    EQF_PHYSICAL | EQF_RINGS, MR_NO_FLAGS,
    DEFAULT_DURATION, 0, 5, SIZE_TINY, 10,
    0, 0, 0, false, 0, true, 1,
    SPWPN_NORMAL, LIGHTGREY, "Teeth", ANIMAL_VERBS,
    FC_ENABLE, FC_FORBID, FC_ENABLE, true, false,
    "squeak", -8, "foreclaw", "", "You perch on the altar of %s.", "flesh"
},

{
    transformation::pig, MONS_HOG, "Pig", "pig-form", "pig",
    "a filthy swine",
    EQF_PHYSICAL | EQF_RINGS, MR_NO_FLAGS,
    BAD_DURATION, 0, 0, SIZE_SMALL, 10,
    0, 0, 0, false, 0, false, 3,
    SPWPN_NORMAL, LIGHTGREY, "Teeth", ANIMAL_VERBS,
    FC_DEFAULT, FC_FORBID, FC_ENABLE, true, false,
    "squeal", 0, "front trotter", "trotter", "You bow your head before the altar of %s.", "flesh"
},

{
    transformation::appendage, MONS_PLAYER, "App", "appendages", "appendages",
    "",
    EQF_HEAD_FOOT, MR_NO_FLAGS,
    FormDuration(10, PS_DOUBLE, 60), 0, 0, SIZE_CHARACTER, 10,
    0, 0, 0, true, 0, false, 3,
    SPWPN_NORMAL, LIGHTGREY, "", DEFAULT_VERBS,
    FC_DEFAULT, FC_DEFAULT, FC_DEFAULT, true, true,
    "", 0, "", "", "", ""
},

{
    transformation::tree, MONS_ANIMATED_TREE, "Tree", "tree-form", "tree",
    "a tree",
    EQF_LEAR | SLOTF(EQ_CLOAK), MR_RES_POISON | mrd(MR_RES_NEG, 3),
    BAD_DURATION, 0, 0, SIZE_CHARACTER, 15,
    20, 0, 50, true, 0, true, 12,
    SPWPN_NORMAL, BROWN, "Branches", { FAV_HIT, FAV_SMACK,
                                       FAV_PUMMEL, FAV_THRASH },
    FC_FORBID, FC_FORBID, FC_FORBID, false, false,
    "creak", 0, "branch", "root", "You sway towards the altar of %s.", "wood"
},

#if TAG_MAJOR_VERSION == 34
{
    transformation::porcupine, MONS_PORCUPINE, "Porc", "porcupine-form", "porcupine",
    "a spiny porcupine",
    EQF_ALL, MR_NO_FLAGS,
    BAD_DURATION, 0, 0, SIZE_TINY, 10,
    0, 0, 0, false, 0, false, 3,
    SPWPN_NORMAL, LIGHTGREY, "Teeth", ANIMAL_VERBS,
    FC_DEFAULT, FC_FORBID, FC_ENABLE, true, false,
    "squeak", -8, "front leg", "", "You curl into a sanctuary of spikes before the altar of %s.", "flesh"
},
#endif

{
    transformation::wisp, MONS_INSUBSTANTIAL_WISP, "Wisp", "wisp-form", "wisp",
    "an insubstantial wisp",
    EQF_ALL, mrd(MR_RES_FIRE, 2) | mrd(MR_RES_COLD, 2) | MR_RES_ELEC
             | MR_RES_STICKY_FLAME | mrd(MR_RES_NEG, 3) | MR_RES_ACID
             | MR_RES_PETRIFY,
    BAD_DURATION, 0, 0, SIZE_TINY, 10,
    5, 0, 50, false, 0, true, 5,
    SPWPN_NORMAL, LIGHTGREY, "Misty tendrils", { FAV_TOUCH, FAV_TOUCH,
                                                 FAV_ENGULF, FAV_ENGULF },
    FC_ENABLE, FC_FORBID, FC_FORBID, false, false,
    "whoosh", -8, "misty tendril", "strand", "You swirl around the altar of %s.", "vapour"
},

#if TAG_MAJOR_VERSION == 34
{
    transformation::jelly, MONS_JELLY, "Jelly", "jelly-form", "jelly",
    "a lump of jelly",
    EQF_PHYSICAL | EQF_RINGS, MR_NO_FLAGS,
    BAD_DURATION, 0, 0, SIZE_CHARACTER, 10,
    0, 0, 0, false, 0, false, 3,
    SPWPN_NORMAL, LIGHTGREY, "", DEFAULT_VERBS,
    FC_DEFAULT, FC_FORBID, FC_FORBID, false, false,
    "", 0, "", "", "", ""
},
#endif

{
    transformation::fungus, MONS_WANDERING_MUSHROOM, "Fungus", "fungus-form", "fungus",
    "a sentient fungus",
    EQF_PHYSICAL, MR_RES_POISON | mrd(MR_RES_NEG, 3),
    BAD_DURATION, 0, 0, SIZE_TINY, 10,
    12, 0, 0, false, 0, true, 12,
    SPWPN_CONFUSE, BROWN, "Spores", FormAttackVerbs(FAV_RELEASE_SPORES_AT),
    FC_DEFAULT, FC_FORBID, FC_FORBID, false, false,
    "sporulate", -8, "hypha", "", "You release spores on the altar of %s.", "flesh"
},

{
    transformation::shadow, MONS_PLAYER_SHADOW, "Shadow", "shadow-form", "shadow",
    "a swirling mass of dark shadows",
    EQF_NONE, mrd(MR_RES_POISON, 3) | mrd(MR_RES_NEG, 3) | MR_RES_MIASMA
                                                         | MR_RES_PETRIFY,
    DEFAULT_DURATION, 0, 0, SIZE_CHARACTER, 10,
    0, 0, 0, true, 0, false, 3,
    SPWPN_NORMAL, MAGENTA, "", DEFAULT_VERBS,
    FC_DEFAULT, FC_FORBID, FC_FORBID, true, true,
    "", 0, "", "", "", "shadow"
},

{
    transformation::hydra, MONS_HYDRA, "Hydra", "hydra-form", "hydra",
    "",
    EQF_PHYSICAL, MR_RES_POISON,
    DEFAULT_DURATION, 0, 0, SIZE_BIG, 13,
    6, 5, 0, true, 0, true, -1,
    SPWPN_NORMAL, GREEN, "", { FAV_NIP_AT, FAV_BITE, FAV_GOUGE, FAV_CHOMP },
    FC_DEFAULT, FC_ENABLE, FC_ENABLE, true, false,
    "roar", 4, "foreclaw", "", "You bow your heads before the altar of %s.", "flesh"
}
};
COMPILE_CHECK(ARRAYSZ(formdata) == NUM_TRANSFORMS);
