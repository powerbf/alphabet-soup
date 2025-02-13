#include "AppHdr.h"
#include "fake-main.hpp"
#include "localise.h"
#include "database.h"
#include "initfile.h"
#include "mgen-data.h"
#include "mon-place.h"
#include "options.h"
#include "unicode.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

static void _show_usage()
{
    cerr << "Usage: mon-speech-test <language> <string key> <iterations> [<RNG seed>]"
         << endl;
}

int main(int argc, char** argv)
{
    if (argc < 4 || argc > 5)
    {
        _show_usage();
        return 1;
    }

    string lang = argv[1];
    string key = argv[2];

    int iterations = 0;
    if (!parse_int(argv[3], iterations) || iterations < 1)
    {
        _show_usage();
        return 1;
    }

    if (argc > 4)
    {
        // seed RNG with specified seed value
        uint64_t seed = 0;
        if (sscanf(argv[4], "%" SCNu64, &seed) < 1)
        {
            _show_usage();
            return 1;
        }
        //cout << "Using RNG seed: " << seed << endl;
        rng::seed(seed);
    }
    else
    {
        // use a random seed
        rng::seed();
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

    monster *dummy = get_free_monster();
    dummy->type = MONS_ORC;
    dummy->hit_points = 20;
    dummy->position = coord_def(10, 9);

    for (int i = 0; i < iterations; i++)
    {
        string msg = getSpeakString(key);
        msg = do_mon_str_replacements(msg, *dummy);
        cout << msg << endl;
    }

    return 0;
}
