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

int num_passes = 0;
int num_fails = 0;

static void show_result(const string& actual, const string& expected)
{
    string status;
    if (actual == expected)
    {
        num_passes++;
        status = "PASS:   ";
    }
    else
    {
        num_fails++;
        status = "*FAIL*: ";
    }

    cout << status << "got: \"" << actual << "\"" << endl;
    cout << "   expected: \"" << expected << "\"" << endl;
}

int main()
{
    Options.lang_name = "de";
    SysEnv.crawl_dir = ".";
    setlocale(LC_ALL, "");
    databaseSystemInit(true);
    init_localisation("de");

    string s = "Eure Zaubersprüche";
    cout << "strwidth(\"" << s << "\") = " << strwidth(s) << endl;
    /*char32_t c;
    utf8towc(&c, "ü");
    printf("%02X\n", c);*/
    string result = localise("%-34.34s", "Your Spells");
    show_result(result, "Eure Zaubersprüche                ");

    string msg, expected;

    msg = localise("%s is %s.", "a gray horse", "happy");
    msg = uppercase_first(msg);
    show_result(msg, "Ein graues Pferd ist glücklich.");

    msg = localise("%s is %s.", "a gray cow", "happy");
    msg = uppercase_first(msg);
    show_result(msg, "Eine graue Kuh ist glücklich.");

    msg = localise("large closed door, spattered with blood");
    show_result(msg, "große geschlossene Tür, mit Blut bespritzt");

    // test with @foo@ style placeholders

    map<string, string> params;
    params["The_monster"] = "the orc";
    msg = localise("@The_monster@ hits you.", params);
    show_result(msg, "Der Ork schlägt Euch.");

    params.clear();
    params["the_monster"] = "the orc";
    msg = localise("You hit @the_monster@.", params);
    show_result(msg, "Ihr schlagt den Ork.");

    // test with %s params

    msg = uppercase_first(localise("%s hits %s", "an orc", "a skeleton"));
    expected = "Ein Ork schlägt ein Skelett";
    show_result(msg, expected);

    msg = uppercase_first(localise("%s hits %s", "a skeleton", "an orc"));
    expected = "Ein Skelett schlägt einen Ork";
    show_result(msg, expected);

    // test list with acc context
    msg = localise("You see here %s.", "a potion of lignification, a +0 short sword and a +2 arbalest");
    expected = "Ihr seht hier einen Trank der Verholzung, ein +0 Kurzschwert und eine +2 Armbrust.";
    show_result(msg, expected);

    // test list with dat context
    msg = localise("You begin with the following equipment: %s", "a potion of lignification, a +0 short sword and a +2 arbalest");
    expected = "Ihr beginnt mit der folgenden Ausrüstung: einem Trank der Verholzung, einem +0 Kurzschwert und einer +2 Armbrust";
    show_result(msg, expected);

    // test mutant beasts
    msg = localise("the juvenile shock beast");
    expected = "das juvenile Schockbiest";
    show_result(msg, expected);

    msg = localise("an elder weird beast");
    expected = "ein älteres bizarres Biest";
    show_result(msg, expected);

    msg = localise_contextual("dat", "your mature fire beast");
    expected = "Eurem reifen Feuerbiest";
    show_result(msg, expected);

    // test derived undead
    msg = localise("the orc skeleton");
    expected = "das Ork-Skelett";
    show_result(msg, expected);

    msg = localise_contextual("acc", "your elf zombie");
    expected = "Euren Elf-Zombie";
    show_result(msg, expected);

    msg = localise("3 ogre simulacra");
    expected = "3 Oger-Simulacra";
    show_result(msg, expected);

    msg = localise("3 Maras (2 ally target)");
    expected = "3 Maras (2 Ziele Verbündeter)";
    show_result(msg, expected);

    msg = localise("Boghold the orc warlord (strong)");
    expected = "Boghold der Ork-Warlord (stark)";
    show_result(msg, expected);

    msg = localise("hydra");
    expected = "Hydra";
    show_result(msg, expected);

    msg = localise("a hydra");
    expected = "eine Hydra";
    show_result(msg, expected);

    msg = localise("a seven-headed hydra (sleeping)");
    expected = "eine siebenköpfige Hydra (schlafend)";
    show_result(msg, expected);

    msg = localise("the 27-headed Lernaean hydra (sleeping)");
    expected = "die 27-köpfige Lernäische Hydra (schlafend)";
    show_result(msg, expected);

    msg = localise("a seven-headed hydra zombie (dormant)");
    expected = "ein siebenköpfiger Hydra-Zombie (ruhend)";
    show_result(msg, expected);

    msg = localise("a 27-headed Lernaean hydra zombie (dormant)");
    expected = "ein 27-köpfiger Lernäischer Hydra-Zombie (ruhend)";
    show_result(msg, expected);

    msg = localise("You hit %s", "the 27-headed Lernaean hydra zombie");
    expected = "Ihr schlagt den 27-köpfigen Lernäischen Hydra-Zombie";
    show_result(msg, expected);

    msg = localise("an infernal demonspawn simulacrum (dormant)");
    expected = "ein höllisches Dämonenbrut-Simulacrum (ruhend)";
    show_result(msg, expected);

    msg = localise("You see here %s.", "an infernal demonspawn simulacrum");
    expected = "Ihr seht hier ein höllisches Dämonenbrut-Simulacrum.";
    show_result(msg, expected);

    msg = localise("%s %d times for up to %d damage each", "bite", 7, 10);
    expected = "7 Mal beißen für bis zu 10 Schaden pro Mal";
    show_result(msg, expected);

    you.position = coord_def(10, 10);
    env.grid.init(DNGN_FLOOR);
    env.pgrid.init(FPROP_NONE);
    env.level_map_ids.init(INVALID_MAP_INDEX);
    env.level_map_mask.init(INVALID_MAP_INDEX);
    init_monsters();

    monster *dowan = get_free_monster();
    dowan->type = MONS_DOWAN;
    dowan->hit_points = 30;
    dowan->position = coord_def(10, 9);

    monster *orc = get_free_monster();
    orc->type = MONS_ORC;
    orc->hit_points = 20;
    orc->position = coord_def(10, 11);

    dowan->foe = orc->mindex();
    msg = getSpeakString("Gozag permabribe");
    msg = do_mon_str_replacements(msg, *dowan);
    expected = "Dowan grinst den Ork gierig an und klimpert mit dem Geldbeutel.";
    show_result(msg, expected);

    dowan->foe = MHITYOU;
    msg = getSpeakString("Gozag permabribe");
    msg = do_mon_str_replacements(msg, *dowan);
    expected = "Dowan grinst Euch gierig an und klimpert mit dem Geldbeutel.";
    show_result(msg, expected);

    dowan->foe = orc->mindex();
    msg = getSpeakString("_Vashnia_rare_2_");
    msg = do_mon_str_replacements(msg, *dowan);
    expected = "Dowan sagt zu dem Ork, \"Du wirst ein schönes Nadelkissen abgeben.\"";
    show_result(msg, expected);

    dowan->foe = MHITYOU;
    msg = getSpeakString("_Vashnia_rare_2_");
    msg = do_mon_str_replacements(msg, *dowan);
    expected = "Dowan sagt, \"Du wirst ein schönes Nadelkissen abgeben.\"";
    show_result(msg, expected);

    msg = getSpeakString("fleeing Dowan");
    msg = do_mon_str_replacements(msg, *orc);
    expected = "VISUAL:Der Ork schreit entsetzt auf.";
    show_result(msg, expected);

    // test invisible monster
    orc->add_ench(mon_enchant(ENCH_INVIS));
    msg = getSpeakString("fleeing Dowan");
    msg = do_mon_str_replacements(msg, *orc);
    expected = "VISUAL:Etwas schreit entsetzt auf.";
    show_result(msg, expected);

    cout << "TEST INSULTS: " << endl;
    you.species = SP_MINOTAUR;
    orc->foe = MHITYOU;

    msg = getSpeakString("generic_insult");
    msg = do_mon_str_replacements(msg, *orc);
    cout << msg << endl;

    msg = getSpeakString("generic_insult");
    msg = do_mon_str_replacements(msg, *orc);
    cout << msg << endl;
}
