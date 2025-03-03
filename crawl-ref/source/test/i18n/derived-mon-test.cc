#include "AppHdr.h"
#include "fake-main.hpp"
#include "localise.h"
#include "database.h"
#include "initfile.h"
#include "mgen-data.h"
#include "mon-place.h"
#include "options.h"
#include "unicode.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

#if 0
#define TRACE(...) printf("DEBUG: %s, ", mons_class_name(base_type)); printf (__VA_ARGS__); printf("\n");
#else
#define TRACE(...)
#endif


static void _show_usage()
{
    cerr << "Usage: derived-mon-test <language> [zombie|skeleton|simulacrum|spectre]"
         << endl;
}

int main(int argc, char** argv)
{

    string lang = argc > 1 ? argv[1] : "";
    string derived_type_str = argc > 2 ? argv[2] : "";

    if (lang == "" || derived_type_str == "")
    {
        _show_usage();
        return 1;
    }

    monster_type derived_type;
    if (derived_type_str == "zombie")
        derived_type = MONS_ZOMBIE;
    else if (derived_type_str == "skeleton")
        derived_type = MONS_SKELETON;
    else if (derived_type_str == "simulacrum")
        derived_type = MONS_SIMULACRUM;
        else if (derived_type_str == "spectre")
        derived_type = MONS_SPECTRAL_THING;
    else
    {
        _show_usage();
        return 1;
    }

    Options.lang_name = lang;
    SysEnv.crawl_dir = ".";
    setlocale(LC_ALL, "");
    databaseSystemInit(true);
    init_localisation(lang);

    you.position = coord_def(10, 10);
    env.grid.init(DNGN_FLOOR);
    env.pgrid.init(FPROP_NONE);
    env.level_map_ids.init(INVALID_MAP_INDEX);
    env.level_map_mask.init(INVALID_MAP_INDEX);
    init_monsters();
    init_spell_descs();
    init_spell_name_cache();

    for (monster_type base_type = MONS_0; base_type < NUM_MONSTERS; base_type++)
    {
        if (invalid_monster_type(base_type) || mons_is_removed(base_type))
            continue;

        // skip dummy monsters
        if (mons_class_flag(base_type, M_CANT_SPAWN))
            continue;

        mon_holy_type holiness = mons_class_holiness(base_type);
        bool natural = (bool)(holiness & MH_NATURAL);
        bool undead = (bool)(holiness & MH_UNDEAD);
        //bool demonic = (bool)(holiness & MH_DEMONIC);
        //bool holy = (bool)(holiness & MH_HOLY);
        bool nonliving = (bool)(holiness & MH_NONLIVING);
        bool plant = (bool)(holiness & MH_PLANT);

        //bool insubstantial = mons_class_flag(base_type, M_INSUBSTANTIAL);
        bool zombified = mons_class_is_zombified(base_type);
        bool corpse = mons_class_can_leave_corpse(base_type);
        bool no_skeleton = mons_class_flag(base_type, M_NO_SKELETON);
        bool no_zombie = mons_class_flag(base_type, M_NO_ZOMBIE);

        TRACE("natural: %d, undead: %d, demonic: %d, holy: %d, nonliving:%d, "
              "plant: %d, insubstantial: %d, zombified: %d, corpse: %d, "
              "no_skeleton: %d, no_zombie: %d, %s, %s",
              natural, undead, demonic, holy, nonliving, plant, insubstantial,
              zombified, corpse, no_skeleton, no_zombie,
              mons_class_name(mons_species(base_type)),
              mons_class_name(mons_genus(base_type))
            );

        // skip if already a derived monster type
        if (zombified)
            continue;
        if (base_type == MONS_PLAYER_ILLUSION || base_type == MONS_PLAYER_GHOST || base_type == MONS_PLAYER_SHADOW)
            continue;

        if (nonliving || plant)
            continue;

        if (derived_type == MONS_SKELETON)
        {
            if (no_skeleton || !corpse)
                continue;
        }
        else if (derived_type == MONS_ZOMBIE)
        {
            if (no_zombie)
                continue;

            if (!natural && !corpse)
                continue;
        }
        else if (derived_type == MONS_SIMULACRUM)
        {
            // Simulacrum spell works on natural, demonic, holy
            if (undead)
                continue;
        }
        else if (derived_type == MONS_SPECTRAL_THING)
        {
            // Death Channel works on natural, demonic, holy
            if (undead)
                continue;
        }

        monster orig;
        orig.set_new_monster_id();
        orig.type = base_type;
        orig.position = coord_def(8, 8);
        orig.hit_points = 20;
        define_monster(orig);
        if (base_type == MONS_MUTANT_BEAST)
        {
            vector<int> facets;
            init_mutant_beast(orig, 2, facets);
        }

        monster mon;
        mon.position = coord_def(9, 9);
        mon.hit_points = 20;
        mon.set_new_monster_id();
        mon.type = derived_type;
        mon.base_monster = base_type;

        define_zombie(&mon, base_type, derived_type);
        name_zombie(mon, orig);
        if (orig.has_hydra_multi_attack())
            mon.num_heads = orig.num_heads;

        //string orig_name = mons_class_name(base_type);
        string orig_name = orig.full_name(DESC_THE);
        string name = mon.full_name(DESC_THE);
        printf("%s", chop_string(localise(orig_name), 40, true).c_str());
        printf("%s\n", localise(name).c_str());
    }

    return 0;
}
