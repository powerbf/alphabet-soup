/*
 * Attack messages
*/

#include <map>
#include <vector>
#include <sstream>

#include "localize.h"
#include "mpr.h"
#include "stringutil.h"
#include "variant-msg.h"

using namespace std;

static map<variant_msg_type, vector<string> > _messages =
{
    // attack messages - simple
    {VMSG_HIT,
        {"You hit %s", "%s hits you", "%s hits %s"}},
    {VMSG_BATTER,
        {"You batter %s", "%s batters you", "%s batters %s"}},
    {VMSG_BITE,
        {"You bite %s", "%s bites you", "%s bites %s"}},
    {VMSG_BLUDGEON,
        {"You bludgeon %s", "%s bludgeons you", "%s bludgeons %s"}},
    {VMSG_CHOMP,
        {"You chomp %s", "%s chomps you", "%s chomps %s"}},
    {VMSG_CLAW,
        {"You claw %s", "%s claws you", "%s claws %s"}},
    {VMSG_CLUMSILY_BASH,
        {"You clumsily bash %s",
         "%s clumsily bashes you",
         "%s clumsily bashes %s"}},
    {VMSG_DEVASTATE,
        {"You devastate %s", "%s devastates you", "%s devastates %s"}},
    {VMSG_ENGULF,
        {"You engulf %s", "%s engulfs you", "%s engulfs %s"}},
    {VMSG_EVISCERATE,
        {"You eviscerate %s", "%s eviscerates you", "%s eviscerates %s"}},
    {VMSG_GOUGE,
        {"You gouge %s", "%s gouges you", "%s gouges %s"}},
    {VMSG_IMPALE,
        {"You impale %s", "%s impales you", "%s impales %s"}},
    {VMSG_MANGLE,
        {"You mangle %s", "%s mangles you", "%s mangles %s"}},
    {VMSG_MAUL,
        {"You maul %s", "%s mauls you", "%s mauls %s"}},
    {VMSG_NIP_AT,
        {"You nip at %s", "%s nips at you", "%s nips at %s"}},
    {VMSG_PULVERISE,
        {"You pulverise %s", "%s pulverises you", "%s pulverises %s"}},
    {VMSG_PUMMEL,
        {"You pummel %s", "%s pummels you", "%s pummels %s"}},
    {VMSG_PUNCH,
        {"You punch %s", "%s punches you", "%s punches %s"}},
    {VMSG_PUNCTURE,
        {"You puncture %s", "%s punctures you", "%s punctures %s"}},
    {VMSG_RELEASE_SPORES_AT,
        {"You release spores at %s",
         "%s releases spores at you",
         "%s releases spores at %s"}},
    {VMSG_SCRATCH,
        {"You scratch %s", "%s scratches you", "%s scratches %s"}},
    {VMSG_SHAVE,
        {"You shave %s", "%s shaves you", "%s shaves %s"}},
    {VMSG_SHRED,
        {"You shred %s", "%s shreds you", "%s shreds %s"}},
    {VMSG_SKEWER,
        {"You skewer %s", "%s skewers you", "%s skewers %s"}},
    {VMSG_SLASH,
        {"You slash %s", "%s slashes you", "%s slashes %s"}},
    {VMSG_SLICE,
        {"You slice %s", "%s slices you", "%s slices %s"}},
    {VMSG_SMACK,
        {"You smack %s", "%s smacks you", "%s smacks %s"}},
    {VMSG_SOCK,
        {"You sock %s", "%s socks you", "%s socks %s"}},
    {VMSG_TENTACLE_SLAP,
        {"You tentacle slap %s",
         "%s tentacle slaps you",
         "%s tentacle slaps %s"}},
    {VMSG_THRASH,
        {"You thrashe %s", "%s thrashes you", "%s thrashes %s"}},
    {VMSG_THUMP,
        {"You thump %s", "%s thumps you", "%s thumps %s"}},
    {VMSG_TOUCH,
        {"You touch %s", "%s touches you", "%s touches %s"}},
    {VMSG_WEAKLY_HIT,
        {"You weakly hit %s", "%s weakly hits you", "%s weakly hits %s"}},
    {VMSG_WHACK,
        {"You whack %s", "%s whacks you", "%s whacks %s"}},

    // attack messsages - colourful
    {VMSG_SPIT_LIKE_PIG,
        {"You spit %s like a pig",
         "%s spits you like a pig",
         "%s spits %s like a pig"}},
    {VMSG_SPIT_LIKE_PROVERBIAL_PIG,
        {"You spit %s like the proverbial pig",
         "%s spits you like the proverbial pig",
         "%s spits %s like the proverbial pig"}},
    {VMSG_ATTACK_WEAK_POINT,
        {"You attack %s's weak point",
         "%s attacks your weak point",
         "%s attacks %s's weak point"}},
    {VMSG_SKEWER_LIKE_KEBAB,
        {"You skewer %s like a kebab",
         "%s skewers you like a kebab",
         "%s skewers %s like a kebab"}},
    {VMSG_STICK_LIKE_PINCUSHION,
        {"You stick %s like a pincushion",
         "%s sticks you like a pincushion",
         "%s sticks %s like a pincushion"}},
    {VMSG_PERFORATE_LIKE_SIEVE,
        {"You perforate %s like a sieve",
         "%s perforates you like a sieve",
         "%s perforates %s like a sieve"}},
    {VMSG_DICE_LIKE_ONION,
        {"You dice %s like an onion",
         "%s dices you like an onion",
         "%s dices %s like an onion"}},
    {VMSG_FRACTURE_INTO_SPLINTERS,
        {"You fracture %s into splinters",
         "%s fractures you into splinters",
         "%s fractures %s into splinters"}},
    {VMSG_SHATTER_INTO_SPLINTERS,
        {"You shatter %s into splinters",
         "%s shatters you into splinters",
         "%s shatters %s into splinters"}},
    {VMSG_CARVE_LIKE_HAM,
        {"You carve %s like a ham",
         "%s carves you like a ham",
         "%s carves %s like a ham"}},
    {VMSG_CARVE_LIKE_PROVERBIAL_HAM,
        {"You carve %s like the proverbial ham",
         "%s carves you like the proverbial ham",
         "%s carves %s like the proverbial ham"}},
    {VMSG_CARVE_LIKE_TURKEY,
        {"You carve %s like a turkey",
         "%s carves you like a turkey",
         "%s carves %s like a turkey"}},
    {VMSG_OPEN_LIKE_PILLOWCASE,
        {"You open %s like a pillowcase",
         "%s opens you like a pillowcase",
         "%s opens %s like a pillowcase"}},
    {VMSG_SLICE_LIKE_RIPE_CHOKO,
        {"You slice %s like a ripe choko",
         "%s slices you like a ripe choko",
         "%s slices %s like a ripe choko"}},
    {VMSG_CUT_INTO_RIBBONS,
        {"You cut %s into ribbons",
         "%s cuts you into ribbons",
         "%s cuts %s into ribbons"}},
    {VMSG_CHOP_INTO_PIECES,
        {"You chop %s into pieces",
         "%s chops you into pieces",
         "%s chops %s into pieces"}},
    {VMSG_SPLATTER_INTO_GOOEY_MESS,
        {"You splatter %s into a gooey mess",
         "%s splatters you into a gooey mess",
         "%s splatters %s into a gooey mess"}},
    {VMSG_CRUSH_LIKE_GRAPE,
        {"You crush %s like a grape",
         "%s crushes you like a grape",
         "%s crushes %s like a grape"}},
    {VMSG_BEAT_LIKE_DRUM,
        {"You beat %s like a drum",
         "%s beats you like a drum",
         "%s beats %s like a drum"}},
    {VMSG_HAMMER_LIKE_GONG,
        {"You hammer %s like a gong",
         "%s hammers you like a gong",
         "%s hammers %s like a gong"}},
    {VMSG_POUND_LIKE_ANVIL,
        {"You pound %s like an anvil",
         "%s pounds you like an anvil",
         "%s pounds %s like an anvil"}},
    {VMSG_FLATTEN_LIKE_PANCAKE,
        {"You flatten %s like a pancake",
         "%s flattens you like a pancake",
         "%s flattens %s like a pancake"}},
    {VMSG_PUNISH_CAUSING_IMMENSE_PAIN,
        {"You punish %s, causing immense pain",
         "%s punishes you, causing immense pain",
         "%s punishes %s, causing immense pain"}},
    {VMSG_SQUASH_LIKE_ANT,
        {"You squash %s like an ant",
         "%s squashes you like an ant",
         "%s squashes %s like an ant"}},
    {VMSG_SQUASH_LIKE_PROVERBIAL_ANT,
        {"You squash %s like the proverbial ant",
         "%s squashes you like the proverbial ant",
         "%s squashes %s like the proverbial ant"}},
    {VMSG_POUND_INTO_FINE_DUST,
        {"You pound %s into fine dust",
         "%s pounds you into fine dust",
         "%s pounds %s into fine dust"}},
    {VMSG_PUMMEL_LIKE_PUNCHING_BAG,
        {"You pummel %s like a punching bag",
         "%s pummels you like a punching bag",
         "%s pummels %s like a punching bag"}},
    {VMSG_BEAT_INTO_BLOODY_PULP,
        {"You beat %s into a bloody pulp",
         "%s beats you into a bloody pulp",
         "%s beats %s into a bloody pulp"}},

    // for projectile only
    {VMSG_PIERCE_THROUGH,
        {"", "%s pierces through you", "%s pierces through %s"}},

    // for hailstorm spell
    {VMSG_PELT,
        {"", "%s pelts you", "%s pelts %s"}},

    // elemental damage - reflexive needed because of beams/spells
    {VMSG_MELT,
        {"You melt %s", "%s melts you", "%s melts %s",
         "You melt yourself", "%s is melted"}},
    {VMSG_BURN,
        {"You burn %s", "%s burns you", "%s burns %s",
         "You burn yourself", "%s is burnt"}},
    {VMSG_FREEZE,
        {"You freeze %s", "%s freezes you", "%s freezes %s",
         "You freeze yourself", "%s is frozen"}},
    {VMSG_ELECTROCUTE,
        {"You electrocute %s", "%s electrocutes you", "%s electrocutes %s",
         "You electrocute yourself", "%s is electrocuted"}},

    {VMSG_CRUSH,
        {"You crush %s", "%s crushes you", "%s crushes %s"}},
    {VMSG_SHATTER,
        {"You shatter %s", "%s shatters you", "%s shatters %s"}},
    {VMSG_ENVENOM,
        {"You envenom %s", "%s envenoms you", "%s envenoms %s"}},
    {VMSG_DRAIN,
        {"You drain %s", "%s drains you", "%s drains %s"}},
    {VMSG_BLAST,
        {"You blast %s", "%s blasts you", "%s blasts %s"}},

};

static string _error;

/*
 * Return English message template
 * (Not translated and placeholders not replaced with values)
 */
const string& get_variant_template(variant_msg_type msg_id, msg_variant_type variant)
{
    auto it = _messages.find(msg_id);
    if (it != _messages.end())
    {
       const vector<string>& v = it->second;
       if (v.size() > variant && !v.at(variant).empty())
       {
           return v.at(variant);
       }
    }

    // something went wrong
    ostringstream os;
    if (msg_id == VMSG_NONE)
    {
        os << "ERROR: Attempt to use VMSG_NONE"; // noextract
    }
    else
    {
        os << "ERROR: Undefined variant message (" // noextract
           << msg_id << ", " << variant << ")";   // noextract
    }

    // return value is reference, so object referred to must be persistent
    _error = os.str();
    return _error;
}

/*
 * Return localized message, with subject/object placeholders replaced with their values
 */
string get_variant_message(variant_msg_type msg_id,
                           const string& subject, const string& object,
                           const string& punctuation)
{
    string subj = lowercase_string(subject);
    string obj = lowercase_string(object);
    msg_variant_type variant;

    if (subj == "you")
    {
        if (obj == "you" || obj == "yourself")
        {
            variant = MV_YOURSELF;
        }
        else
        {
            variant = MV_YOU_SUBJECT;
        }
    }
    else if (obj == "you")
    {
        variant = MV_YOU_OBJECT;
    }
    else if (obj == "itself" || obj == "himself" || obj == "herself")
    {
        variant = MV_ITSELF;
    }
    else
    {
        variant = MV_THIRD_PARTIES;
    }

    const string& temp = get_variant_template(msg_id, variant);

    string msg;
    if (starts_with(temp, "ERROR"))
        msg = temp + ", subj=\"" + subj + "\", obj=\"" + obj + "\""; // noextract
    else if (variant == MV_YOU_SUBJECT)
        msg = localize(temp, obj);
    else if (variant == MV_YOU_OBJECT)
        msg = localize(temp, subj);
    else if (variant == MV_THIRD_PARTIES)
        msg = localize(temp, subj, obj);
    else if (variant == MV_YOURSELF)
        msg = localize(temp);
    else if (variant == MV_ITSELF)
        msg = localize(temp, subj, obj);

    if (!punctuation.empty())
    {
        string punct;
        if (contains(punctuation, "%s"))
            punct = punctuation;
        else
            punct = "%s" + punctuation;

        // translate the punctuation, but don't translate the message again
        msg = localize(punct, LocalizationArg(msg, false));
    }

    return msg;
}

/*
 * Return localized message, with subject/object placeholders replaced with their values
 */
string get_variant_message(variant_msg_type msg_id,
                           const actor* subject, const actor* object,
                           const string& punctuation)
{
    return get_variant_message(msg_id, subject, object, true, true, punctuation);
}

/*
 * Return localized message, with subject/object placeholders replaced with their values
 */
string get_variant_message(variant_msg_type msg_id,
                           const actor* subject, const actor* object,
                           bool subject_seen, bool object_seen,
                           const string& punctuation)
{
    string subj = actor_name(subject, DESC_THE, subject_seen);
    string obj;
    if (object && object == subject)
        obj = actor_pronoun(object, PRONOUN_REFLEXIVE, object_seen);
    else
        obj = actor_name(object, DESC_THE, object_seen);
    return get_variant_message(msg_id, subj, obj, punctuation);
}

/*
 * Output localized message, with subject/object placeholders replaced with their values
 */
void do_variant_message(variant_msg_type msg_id,
                        const string& subject, const string& object,
                        const string& punctuation)
{
    mpr_nolocalize(get_variant_message(msg_id, subject, object, punctuation));
}

/*
 * Output localized message, with subject/object placeholders replaced with their values
 */
void do_variant_message(variant_msg_type msg_id,
                        const actor* subject, const actor* object,
                        const string& punctuation)
{
    mpr_nolocalize(get_variant_message(msg_id, subject, object, punctuation));
}

/*
 * Output localized message, with subject/object placeholders replaced with their values
 */
void do_variant_message(variant_msg_type msg_id,
                        const actor* subject, const actor* object,
                        bool subject_seen, bool object_seen,
                        const string& punctuation)
{
    mpr_nolocalize(get_variant_message(msg_id, subject, object,
                                       subject_seen, object_seen,
                                       punctuation));
}
