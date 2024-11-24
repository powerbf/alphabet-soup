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

static void simple_test(const string& english, const string& expected)
{
    string actual = localise(english);
    show_result(actual, expected);
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
    simple_test("the orc", "der Ork");
    simple_test("the ball python", "die Königspython");
    simple_test("the skeleton", "das Skelett");
    cout << endl;

    // with indefinite article
    simple_test("an orc", "ein Ork");
    simple_test("a ball python", "eine Königspython");
    simple_test("a skeleton", "ein Skelett");
    cout << endl;

    // with "your"
    simple_test("your orc", "dein Ork");
    simple_test("your ball python", "deine Königspython");
    simple_test("your skeleton", "dein Skelett");
    cout << endl;

    // with no determiner
    simple_test("orc", "Ork");
    simple_test("ball python", "Königspython");
    simple_test("skeleton", "Skelett");
    cout << endl;

    // with definite article and adjective
    simple_test("the helpless orc", "der hilflose Ork");
    simple_test("the helpless ball python", "die hilflose Königspython");
    simple_test("the helpless skeleton", "das hilflose Skelett");
    cout << endl;

    // with indefinite article and adjective
    simple_test("a helpless orc", "ein hilfloser Ork");
    simple_test("a helpless ball python", "eine hilflose Königspython");
    simple_test("a helpless skeleton", "ein hilfloses Skelett");
    cout << endl;

    // with "your" and adjective
    simple_test("your helpless orc", "dein hilfloser Ork");
    simple_test("your helpless ball python", "deine hilflose Königspython");
    simple_test("your helpless skeleton", "dein hilfloses Skelett");
    cout << endl;

    // with no determiner and adjective
    simple_test("helpless orc", "hilfloser Ork");
    simple_test("helpless ball python", "hilflose Königspython");
    simple_test("helpless skeleton", "hilfloses Skelett");
    cout << endl;

    // uniques
    simple_test("Prince Ribbit", "Prinz Ribbit");
    simple_test("Crazy Yiuf", "Verrückter Yiuf");
    simple_test("Natasha", "Natascha");
    cout << endl;

    // uniques with definite article
    simple_test("Blork the orc", "Blork der Ork");
    simple_test("the Enchantress", "die Zauberin");
    simple_test("the Lernaean hydra", "die Lernäische Hydra");
    simple_test("the Serpent of Hell", "die Höllenschlange");
    simple_test("the Royal Jelly", "das Gelée Royale");
    cout << endl;

    // uniques with definite article and adjective
    simple_test("the helpless Prince Ribbit", "der hilflose Prinz Ribbit");
    simple_test("the helpless Crazy Yiuf", "der hilflose Verrückte Yiuf");
    //simple_test("the helpless Blork the orc", "der hilflose Blork der Ork");
    simple_test("the helpless Enchantress", "die hilflose Zauberin");
    simple_test("the helpless 27-headed Lernaean hydra", "die hilflose 27-köpfige Lernäische Hydra");
    simple_test("the helpless Serpent of Hell", "die hilflose Höllenschlange");
    simple_test("the helpless Royal Jelly", "das hilflose Gelée Royale");
    cout << endl;

    // uniques with adjective only
    simple_test("helpless Prince Ribbit", "hilfloser Prinz Ribbit");
    simple_test("helpless Crazy Yiuf", "hilfloser Verrückter Yiuf");
    //simple_test("helpless Blork the orc", "hilfloser Blork der Ork");
    simple_test("helpless Enchantress", "hilflose Zauberin");
    simple_test("helpless 27-headed Lernaean hydra", "hilflose 27-köpfige Lernäische Hydra");
    simple_test("helpless Serpent of Hell", "hilflose Höllenschlange");
    simple_test("helpless Royal Jelly", "hilfloses Gelée Royale");
    cout << endl;

    // special cases
    simple_test("the helpless yellow draconian", "der hilflose gelbe Drakonier");
    simple_test("Boghold the orc warlord", "Boghold der Ork-Warlord");
    //simple_test("the helpless Boghold the orc warlord", "der hilflose Boghold der Ork-Warlord");
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
