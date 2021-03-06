/*
 * Attack messages
*/

#include <map>
#include <vector>
#include <sstream>

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
