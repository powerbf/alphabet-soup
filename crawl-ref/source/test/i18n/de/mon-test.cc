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

    // with definite article
    test("der Ork", "the orc");
    test("die Königspython", "the ball python");
    test("das Skelett", "the skeleton");
    cout << endl;

    // with indefinite article
    test("ein Ork", "an orc");
    test("eine Königspython", "a ball python");
    test("ein Skelett", "a skeleton");
    cout << endl;

    // with "your"
    test("dein Ork", "your orc");
    test("deine Königspython", "your ball python");
    test("dein Skelett", "your skeleton");
    cout << endl;

    // with no determiner
    test("Ork", "orc");
    test("Königspython", "ball python");
    test("Skelett", "skeleton");
    cout << endl;

    // with definite article and adjective
    test("der hilflose Ork", "the helpless orc");
    test("die hilflose Königspython", "the helpless ball python");
    test("das hilflose Skelett", "the helpless skeleton");
    cout << endl;

    // with indefinite article and adjective
    test("ein hilfloser Ork", "a helpless orc");
    test("eine hilflose Königspython", "a helpless ball python");
    test("ein hilfloses Skelett", "a helpless skeleton");
    cout << endl;

    // with "your" and adjective
    test("dein hilfloser Ork", "your helpless orc");
    test("deine hilflose Königspython", "your helpless ball python");
    test("dein hilfloses Skelett", "your helpless skeleton");
    cout << endl;

    // with no determiner and adjective
    test("hilfloser Ork", "helpless orc");
    test("hilflose Königspython", "helpless ball python");
    test("hilfloses Skelett", "helpless skeleton");
    cout << endl;

    // uniques
    test("Prinz Ribbit", "Prince Ribbit");
    test("Verrückter Yiuf", "Crazy Yiuf");
    test("Natascha", "Natasha");
    cout << endl;

    // uniques with definite article
    test("Blork der Ork", "Blork the orc");
    test("die Zauberin", "the Enchantress");
    test("die Lernäische Hydra", "the Lernaean hydra");
    test("die Höllenschlange", "the Serpent of Hell");
    test("das Gelée Royale", "the Royal Jelly");
    cout << endl;

    // uniques with definite article and adjective
    test("der hilflose Prinz Ribbit", "the helpless Prince Ribbit");
    test("der hilflose Verrückte Yiuf", "the helpless Crazy Yiuf");
    //test("der hilflose Blork der Ork", "the helpless Blork the orc");
    test("die hilflose Zauberin", "the helpless Enchantress");
    test("die hilflose 27-köpfige Lernäische Hydra", "the helpless 27-headed Lernaean hydra");
    test("die hilflose Höllenschlange", "the helpless Serpent of Hell");
    test("das hilflose Gelée Royale", "the helpless Royal Jelly");
    cout << endl;

    // uniques with adjective only
    test("hilfloser Prinz Ribbit", "helpless Prince Ribbit");
    test("hilfloser Verrückter Yiuf", "helpless Crazy Yiuf");
    //test("hilfloser Blork der Ork", "helpless Blork the orc");
    test("hilflose Zauberin", "helpless Enchantress");
    test("hilflose 27-köpfige Lernäische Hydra", "helpless 27-headed Lernaean hydra");
    test("hilflose Höllenschlange", "helpless Serpent of Hell");
    test("hilfloses Gelée Royale", "helpless Royal Jelly");
    cout << endl;

    // special cases
    test("der hilflose gelbe Drakonier", "the helpless yellow draconian");
    test("Boghold der Ork-Warlord", "Boghold the orc warlord");
    //test("der hilflose Boghold der Ork-Warlord", "the helpless Boghold the orc warlord");
    cout << endl;

    // accusative case
    test("Du triffst den Ork", "You hit %s", "the orc");
    test("Du triffst die Königspython", "You hit %s", "the ball python");
    test("Du triffst das Skelett", "You hit %s", "the skeleton");
    test("Du triffst den hilflosen Ork", "You hit %s", "the helpless orc");
    test("Du triffst die 27-köpfige Lernäische Hydra", "You hit %s", "the 27-headed Lernaean hydra");
    test("Du triffst den hilflosen Verrückten Yiuf", "You hit %s", "the helpless Crazy Yiuf");
    test("Du triffst den hilflosen gelben Drakonier", "You hit %s", "the helpless yellow draconian");
    cout << endl;

    // dative case
    test("Du kollidierst mit dem Ork!", "You collide with %s!", "the orc");
    test("Du kollidierst mit der Königspython!", "You collide with %s!", "the ball python");
    test("Du kollidierst mit dem Skelett!", "You collide with %s!", "the skeleton");
    test("Du kollidierst mit der 27-köpfigen Lernäischen Hydra!", "You collide with %s!", "the 27-headed Lernaean hydra");
    cout << endl;

    // show results summary
    cout << num_passes << " TESTS PASSED" << endl;
    if (num_fails > 0)
        cout << "**** " << num_fails << " TESTS FAILED ****" << endl;

    return num_fails;
}
