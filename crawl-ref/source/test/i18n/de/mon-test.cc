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
    if (actual != expected)
        cout << "   expected: \"" << expected << "\"" << endl;
}

static void test(const string& expected, const char* fmt...)
{
    va_list args;
    va_start(args, fmt);

    string actual = vlocalise(fmt, args);
    show_result(actual, expected);

    va_end(args);
}

int main()
{
    Options.lang_name = "de";
    SysEnv.crawl_dir = ".";
    setlocale(LC_ALL, "");
    databaseSystemInit(true);
    init_localisation("de");

    // simple masculine
    test("der Ork", "the orc");
    test("ein Ork", "an orc");
    test("dein Ork", "your orc");
    test("Ork", "orc");
    test("Du triffst den Ork", "You hit %s", "the orc");
    test("Du kollidierst mit dem Ork!", "You collide with %s!", "the orc");
    test("Du blockst den Angriff des Orks.", "You block %s's attack.", "the orc");
    test("Die Wunden des Orks heilen von selbst!", "%s's wounds heal themselves!", "the orc");
    cout << endl;

    // simple feminine
    test("die Königspython", "the ball python");
    test("eine Königspython", "a ball python");
    test("deine Königspython", "your ball python");
    test("Königspython", "ball python");
    test("Du triffst die Königspython", "You hit %s", "the ball python");
    test("Du kollidierst mit der Königspython!", "You collide with %s!", "the ball python");
    test("Du blockst den Angriff der Königspython.", "You block %s's attack.", "the ball python");
    test("Die Wunden der Königspython heilen von selbst!", "%s's wounds heal themselves!", "the ball python");
    cout << endl;

    // simple neuter
    test("das Skelett", "the skeleton");
    test("ein Skelett", "a skeleton");
    test("dein Skelett", "your skeleton");
    test("Skelett", "skeleton");
    test("Du triffst das Skelett", "You hit %s", "the skeleton");
    test("Du kollidierst mit dem Skelett!", "You collide with %s!", "the skeleton");
    test("Du blockst den Angriff des Skeletts.", "You block %s's attack.", "the skeleton");
    test("Die Wunden des Skeletts heilen von selbst!", "%s's wounds heal themselves!", "the skeleton");
     cout << endl;

    // masculine with adjective
    test("der hilflose Ork", "the helpless orc");
    test("ein hilfloser Ork", "a helpless orc");
    test("dein hilfloser Ork", "your helpless orc");
    test("hilfloser Ork", "helpless orc");
    test("Du triffst den hilflosen Ork", "You hit %s", "the helpless orc");
    test("Du kollidierst mit dem hilflosen Ork!", "You collide with %s!", "the helpless orc");
    test("Du blockst den Angriff des spektralen Orks.", "You block %s's attack.", "the spectral orc");
    test("Die Wunden des spektralen Orks heilen von selbst!", "%s's wounds heal themselves!", "the spectral orc");
    cout << endl;

    // feminine with adjective
    test("die hilflose Königspython", "the helpless ball python");
    test("eine hilflose Königspython", "a helpless ball python");
    test("deine hilflose Königspython", "your helpless ball python");
    test("hilflose Königspython", "helpless ball python");
    test("Du triffst die hilflose Königspython", "You hit %s", "the helpless ball python");
    test("Du kollidierst mit der hilflosen Königspython!", "You collide with %s!", "the helpless ball python");
    test("Du blockst den Angriff der spektralen Königspython.", "You block %s's attack.", "the spectral ball python");
    test("Die Wunden der spektralen Königspython heilen von selbst!", "%s's wounds heal themselves!", "the spectral ball python");
    cout << endl;

    // neuter with adjective
    test("das hilflose Skelett", "the helpless skeleton");
    test("ein hilfloses Skelett", "a helpless skeleton");
    test("dein hilfloses Skelett", "your helpless skeleton");
    test("hilfloses Skelett", "helpless skeleton");
    test("Du triffst das hilflose Skelett", "You hit %s", "the helpless skeleton");
    test("Du kollidierst mit dem hilflosen Skelett!", "You collide with %s!", "the helpless skeleton");
    test("Du blockst den Angriff des spektralen Schweins.", "You block %s's attack.", "the spectral hog");
    test("Die Wunden des spektralen Schweins heilen von selbst!", "%s's wounds heal themselves!", "the spectral hog");
    cout << endl;

    // masculine with weak declension
    test("der Feuerdrache", "the fire dragon");
    test("ein Feuerdrache", "a fire dragon");
    test("dein Feuerdrache", "your fire dragon");
    test("Feuerdrache", "fire dragon");
    test("der hilflose Feuerdrache", "the helpless fire dragon");
    test("ein hilfloser Feuerdrache", "a helpless fire dragon");
    test("dein hilfloser Feuerdrache", "your helpless fire dragon");
    test("hilfloser Feuerdrache", "helpless fire dragon");
    test("Du triffst den Feuerdrachen", "You hit %s", "the fire dragon");
    test("Du triffst den hilflosen Feuerdrachen", "You hit %s", "the helpless fire dragon");
    test("Du kollidierst mit dem Feuerdrachen!", "You collide with %s!", "the fire dragon");
    test("Du kollidierst mit dem hilflosen Feuerdrachen!", "You collide with %s!", "the helpless fire dragon");
    test("Du blockst den Angriff des spektralen Feuerdrachen.", "You block %s's attack.", "the spectral fire dragon");
    test("Die Wunden des spektralen Feuerdrachen heilen von selbst!", "%s's wounds heal themselves!", "the spectral fire dragon");
    cout << endl;

    // unique with simple name
    test("Natascha", "Natasha");
    test("Du triffst Natascha", "You hit %s", "Natasha");
    test("Du triffst die hilflose Natascha", "You hit %s", "the helpless Natasha");
    test("Du kollidierst mit Natascha!", "You collide with %s!", "Natascha");
    test("Du kollidierst mit der hilflosen Natascha!", "You collide with %s!", "the helpless Natasha");
    test("Du blockst den Angriff von Natascha.", "You block %s's attack.", "Natasha");
    test("Die Wunden von Natascha heilen von selbst!", "%s's wounds heal themselves!", "Natasha");
    cout << endl;

    // unique with definite article in name
    test("die Zauberin", "the Enchantress");
    test("Zauberin", "Enchantress");
    test("Du triffst die Zauberin", "You hit %s", "the Enchantress");
    test("Du triffst die hilflose Zauberin", "You hit %s", "the helpless Enchantress");
    test("Du kollidierst mit der Zauberin!", "You collide with %s!", "the Enchantress");
    test("Du kollidierst mit der hilflosen Zauberin!", "You collide with %s!", "the helpless Enchantress");
    test("Du blockst den Angriff der Zauberin.", "You block %s's attack.", "the Enchantress");
    test("Die Wunden der Zauberin heilen von selbst!", "%s's wounds heal themselves!", "the Enchantress");
    cout << endl;

    // unique with weak declension
    test("Prinz Quak", "Prince Ribbit");
    test("der hilflose Prinz Quak", "the helpless Prince Ribbit");
    test("hilfloser Prinz Quak", "helpless Prince Ribbit");
    test("Du triffst Prinz Quak", "You hit %s", "Prince Ribbit");
    test("Du triffst den hilflosen Prinz Quak", "You hit %s", "the helpless Prince Ribbit");
    test("Du kollidierst mit Prinz Quak!", "You collide with %s!", "Prince Ribbit");
    test("Du kollidierst mit dem hilflosen Prinz Quak!", "You collide with %s!", "the helpless Prince Ribbit");
    test("Du blockst den Angriff von Prinz Quak.", "You block %s's attack.", "Prince Ribbit");
    test("Die Wunden von Prinz Quak heilen von selbst!", "%s's wounds heal themselves!", "Prince Ribbit");
    cout << endl;

    // unique with a capitalised adjective in the name
    test("Verrückter Yiuf", "Crazy Yiuf");
    test("der hilflose Verrückte Yiuf", "the helpless Crazy Yiuf");
    test("hilfloser Verrückter Yiuf", "helpless Crazy Yiuf");
    test("Du triffst Verrückten Yiuf", "You hit %s", "Crazy Yiuf");
    test("Du triffst den hilflosen Verrückten Yiuf", "You hit %s", "the helpless Crazy Yiuf");
    test("Du kollidierst mit Verrücktem Yiuf!", "You collide with %s!", "Crazy Yiuf");
    test("Du kollidierst mit dem hilflosen Verrückten Yiuf!", "You collide with %s!", "the helpless Crazy Yiuf");
    test("Du blockst den Angriff vom Verrückten Yiuf.", "You block %s's attack.", "Crazy Yiuf");
    test("Die Wunden vom Verrückten Yiuf heilen von selbst!", "%s's wounds heal themselves!", "Crazy Yiuf");
    cout << endl;

    // another unique with a capitalised adjective in the name
    test("die Lernäische Hydra", "the Lernaean hydra");
    test("die hilflose Lernäische Hydra", "the helpless Lernaean hydra");
    test("hilflose Lernäische Hydra", "helpless Lernaean hydra");
    test("die 27-köpfige Lernäische Hydra", "the 27-headed Lernaean hydra");
    test("die hilflose 27-köpfige Lernäische Hydra", "the helpless 27-headed Lernaean hydra");
    test("Du triffst die hilflose 27-köpfige Lernäische Hydra", "You hit %s", "the helpless 27-headed Lernaean hydra");
    test("Du kollidierst mit der hilflosen 27-köpfigen Lernäischen Hydra!", "You collide with %s!", "the helpless 27-headed Lernaean hydra");
    test("Du blockst den Angriff der 27-köpfigen Lernäischen Hydra.", "You block %s's attack.", "the 27-headed Lernaean hydra");
    test("Die Wunden der 27-köpfigen Lernäischen Hydra heilen von selbst!", "%s's wounds heal themselves!", "the 27-headed Lernaean hydra");
    cout << endl;

    // unique with "of" in the English name (this could mess things up)
    test("die Höllenschlange", "the Serpent of Hell");
    test("Höllenschlange", "Serpent of Hell");
    test("die hilflose Höllenschlange", "the helpless Serpent of Hell");
    test("Du triffst die Höllenschlange", "You hit %s", "the Serpent of Hell");
    test("Du triffst die hilflose Höllenschlange", "You hit %s", "the helpless Serpent of Hell");
    test("Du kollidierst mit der Höllenschlange!", "You collide with %s!", "the Serpent of Hell");
    test("Du kollidierst mit der hilflosen Höllenschlange!", "You collide with %s!", "the helpless Serpent of Hell");
    test("Du blockst den Angriff der Höllenschlange.", "You block %s's attack.", "the Serpent of Hell");
    test("Die Wunden der Höllenschlange heilen von selbst!", "%s's wounds heal themselves!", "the Serpent of Hell");
    cout << endl;

    // unique with definite article in the middle
    test("Blork der Ork", "Blork the orc");
    // these ones currently fail
    //test("der hilflose Blork der Ork", "the helpless Blork the orc");
    //test("hilfloser Blork der Ork", "helpless Blork the orc");
    test("Du triffst Blork den Ork", "You hit %s", "Blork the orc");
    test("Du kollidierst mit Blork dem Ork!", "You collide with %s!", "Blork the orc");
    test("Du blockst den Angriff von Blork dem Ork.", "You block %s's attack.", "Blork the orc");
    test("Die Wunden von Blork dem Ork heilen von selbst!", "%s's wounds heal themselves!", "Blork the orc");
    cout << endl;

    // named ally
    test("Boghold der Ork-Warlord", "Boghold the orc warlord");
    //test("der hilflose Boghold der Ork-Warlord", "the helpless Boghold the orc warlord");
    test("Du triffst Boghold den Ork-Warlord", "You hit %s", "Boghold the orc warlord");
    test("Du kollidierst mit Boghold dem Ork-Warlord!", "You collide with %s!", "Boghold the orc warlord");
    // TODO: Change possessive to von + dative like Blork
    test("Du blockst den Angriff Boghold des Ork-Warlords.", "You block %s's attack.", "Boghold the orc warlord");
    test("Die Wunden Boghold des Ork-Warlords heilen von selbst!", "%s's wounds heal themselves!", "Boghold the orc warlord");
    cout << endl;

    // derived monsters (should have the gender of the derived monster, not the original)
    test("Du triffst den hilflosen Zombie eines Meerwesens", "You hit %s", "the helpless merfolk zombie");
    test("Du triffst das hilflose Skelett eines Orks", "You hit %s", "the helpless orc skeleton");
    test("Du triffst das hilflose Simulacrum eines Orks", "You hit %s", "the helpless orc simulacrum");
    cout << endl;
    test("eine Salzsäule in Form eines Orks", "an orc shaped pillar of salt");
    test("eine Salzsäule in Form von Dowan", "a Dowan shaped pillar of salt");
    test("eine Salzsäule in Form der Zauberin", "an Enchantress shaped pillar of salt");
    test("eine Salzsäule in Form der Lernäischen Hydra", "a Lernaean hydra shaped pillar of salt");
    test("eine Salzsäule in Form der Höllenschlange", "a Serpent of Hell shaped pillar of salt");
    test("eine Salzsäule in Form des Gelée Royale", "a Royal Jelly shaped pillar of salt");
    test("ein Eisblock in Form einer schwarzen Mamba", "a black mamba shaped block of ice");
    test("ein Gestaltwandler in Form eines Dampfdrachen", "a steam dragon shaped shifter");
    test("das Gelée Royale in Form eines Ogers", "the ogre shaped Royal Jelly");
    test("die Lernäische Hydra in Form eines Wolfs", "the wolf shaped Lernaean hydra");
    test("die Höllenschlange in Form eines Ogers", "the ogre shaped Serpent of Hell");
    test("Du siehst hier eine Salzsäule in Form von Mennas.", "You see here %s.", "a Mennas shaped pillar of salt");
    test("Du siehst hier einen Eisblock in Form einer Tarantella.", "You see here %s.", "a tarantella shaped block of ice");
    test("Dowan die Himmelsbestie", "Dowan the sky beast");
    test("die Zauberin der Wolf", "the Enchantress the wolf");
    cout << endl;

    // player ghost
    test("der Geist von MrDizzy", "MrDizzy's ghost");
    test("der hilflose Geist von MrDizzy", "the helpless MrDizzy's ghost");
    test("Du triffst den Geist von MrDizzy", "You hit %s", "MrDizzy's ghost");
    test("Du triffst den hilflosen Geist von MrDizzy", "You hit %s", "the helpless MrDizzy's ghost");
    test("Du kollidierst mit dem Geist von MrDizzy!", "You collide with %s!", "MrDizzy's ghost");
    test("Du kollidierst mit dem hilflosen Geist von MrDizzy!", "You collide with %s!", "the helpless MrDizzy's ghost");
    test("Du blockst den Angriff des Geistes von MrDizzy.", "You block %s's attack.", "MrDizzy's ghost");
    test("Die Wunden des Geistes von MrDizzy heilen von selbst!", "%s's wounds heal themselves!", "MrDizzy's ghost");
    // long-form description
    test("Die Erscheinung von MrDizzy der Minotaurischen Barrikade, einem legendären Minotaur Kämpfer von Beogh.",
         "The apparition of %s.", "MrDizzy the Minotaur Barricade, a legendary Minotaur Fighter of Beogh");
    test("Die Erscheinung von Fuonemn dem Unbezwingbaren, einem erfahrenen Tiefenzwerg Berserker von Trog.",
         "The apparition of %s.", "Fuonemn the Impregnable, a veteran Deep Dwarf Berserker of Trog");
    cout << endl;
    // level annotation short form
    test("der Geist von MrDizzy, durchschnittlichem MiKä", "MrDizzy's ghost, journeyman MiFi");

    // player illusion
    test("die Illusion von MrDizzy", "MrDizzy's illusion");
    test("die hilflose Illusion von MrDizzy", "the helpless MrDizzy's illusion");
    test("Du triffst die Illusion von MrDizzy", "You hit %s", "MrDizzy's illusion");
    test("Du triffst die hilflose Illusion von MrDizzy", "You hit %s", "the helpless MrDizzy's illusion");
    test("Du kollidierst mit der Illusion von MrDizzy!", "You collide with %s!", "MrDizzy's illusion");
    test("Du kollidierst mit der hilflosen Illusion von MrDizzy!", "You collide with %s!", "the helpless MrDizzy's illusion");
    test("Du blockst den Angriff der Illusion von MrDizzy.", "You block %s's attack.", "MrDizzy's illusion");
    test("Die Wunden der Illusion von MrDizzy heilen von selbst!", "%s's wounds heal themselves!", "MrDizzy's illusion");
    // long-form description
    test("Eine Illusion von MrDizzy der Minotaurischen Barrikade, einem legendären Minotaur Kämpfer von Beogh.",
         "An illusion of %s.", "MrDizzy the Minotaur Barricade, a legendary Minotaur Fighter of Beogh");
    cout << endl;

    // mutant beasts
    // these have one "tier" and two "facets"
    test("das larvale Fledermausfeuerbiest", "the larval batfire beast");
    test("ein juveniles Feuerochsenbiest", "a juvenile fireox beast");
    test("dein reifes Ochsenschockbiest", "your mature oxshock beast");
    test("älteres Schockstichbiest", "elder shocksting beast");
    test("des urzeitlichen Stichfeuerbiestes", "the primal stingfire beast's");
    //test("eines urzeitlichen Stichfeuerbiestes", "a primal stingfire beast's");
    cout << endl;

    test("Du punktierst etwas", "You puncture %s", "something");
    test("Du blockst den Angriff von etwas.", "You block %s's attack.", "something");
    test("Du ziehst Lebenskraft aus dem unsichtbaren Schrecken!",
         "You draw life force from %s%s", "the unseen horror", "!");
    // note: this will be capitalised by mprf
    test("etwas zieht Lebenskraft aus dir!!!",
         "%s draws life force from %s%s", "something", "you", "!!!");
    cout << endl;

    // show results summary
    cout << num_passes << " TESTS PASSED" << endl;
    if (num_fails > 0)
        cout << "**** " << num_fails << " TESTS FAILED ****" << endl;

    return num_fails;
}
