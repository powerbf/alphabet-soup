/*
 * Variant message types
 * Messages that change based on the grammatical person of the subject and object
 * (e.g. "You hit <the monster>", "<The monster> hits you", "<The monster> hits <the monster>")
 */
#pragma once

enum msg_variant_type
{
    MV_YOU_SUBJECT, // you are the subject (and the object, if there is one, is 3rd person)
    MV_YOU_OBJECT, // you are the object (and the subject is 3rd person)
    MV_THIRD_PERSON, // you are not involved (e.g. monster vs. monster)
    MV_YOURSELF, // you do something to yourself
    MV_ITSELF, // 3rd person does something to itself (avoid using itself/himself/herself
               // in the message- it's hard to translate)
};

enum variant_msg_type
{
    VMSG_NONE,

    // attack messages - simple
    VMSG_HIT,
    VMSG_BATTER,
    VMSG_BITE,
    VMSG_BLUDGEON,
    VMSG_CHOMP,
    VMSG_CLAW,
    VMSG_CLUMSILY_BASH,
    VMSG_DEVASTATE,
    VMSG_ENGULF,
    VMSG_EVISCERATE,
    VMSG_GOUGE,
    VMSG_IMPALE,
    VMSG_MANGLE,
    VMSG_MAUL,
    VMSG_NIP_AT,
    VMSG_PULVERISE,
    VMSG_PUMMEL,
    VMSG_PUNCH,
    VMSG_PUNCTURE,
    VMSG_RELEASE_SPORES_AT,
    VMSG_SCRATCH,
    VMSG_SHAVE,
    VMSG_SHRED,
    VMSG_SKEWER,
    VMSG_SLASH,
    VMSG_SLICE,
    VMSG_SMACK,
    VMSG_SOCK,
    VMSG_TENTACLE_SLAP,
    VMSG_THRASH,
    VMSG_THUMP,
    VMSG_TOUCH,
    VMSG_WEAKLY_HIT,
    VMSG_WHACK,

    // attack messages - colourful
    VMSG_SPIT_LIKE_PIG,
    VMSG_SPIT_LIKE_PROVERBIAL_PIG,
    VMSG_ATTACK_WEAK_POINT,
    VMSG_SKEWER_LIKE_KEBAB,
    VMSG_STICK_LIKE_PINCUSHION,
    VMSG_PERFORATE_LIKE_SIEVE,
    VMSG_DICE_LIKE_ONION,
    VMSG_FRACTURE_INTO_SPLINTERS,
    VMSG_SHATTER_INTO_SPLINTERS,
    VMSG_CARVE_LIKE_HAM,
    VMSG_CARVE_LIKE_PROVERBIAL_HAM,
    VMSG_CARVE_LIKE_TURKEY,
    VMSG_OPEN_LIKE_PILLOWCASE,
    VMSG_SLICE_LIKE_RIPE_CHOKO,
    VMSG_CUT_INTO_RIBBONS,
    VMSG_CHOP_INTO_PIECES,
    VMSG_SPLATTER_INTO_GOOEY_MESS,
    VMSG_CRUSH_LIKE_GRAPE,
    VMSG_BEAT_LIKE_DRUM,
    VMSG_HAMMER_LIKE_GONG,
    VMSG_POUND_LIKE_ANVIL,
    VMSG_FLATTEN_LIKE_PANCAKE,
    VMSG_PUNISH_CAUSING_IMMENSE_PAIN,
    VMSG_SQUASH_LIKE_ANT,
    VMSG_SQUASH_LIKE_PROVERBIAL_ANT,
    VMSG_POUND_INTO_FINE_DUST,
    VMSG_PUMMEL_LIKE_PUNCHING_BAG,
    VMSG_BEAT_INTO_BLOODY_PULP,

    // elemental effects
    VMSG_BURN,
    VMSG_FREEZE,
    VMSG_ELECTROCUTE,
    VMSG_CRUSH,

    // for projectile only
    VMSG_PIERCE_THROUGH,

    // for hailstorm spell
    VMSG_PELT,
};
