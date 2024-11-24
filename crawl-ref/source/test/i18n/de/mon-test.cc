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

    // show results summary
    cout << num_passes << " TESTS PASSED" << endl;
    if (num_fails > 0)
        cout << "**** " << num_fails << " TESTS FAILED ****" << endl;

    return num_fails;
}
