#include "AppHdr.h"
#include "fake-main.hpp"
#include "localise.h"
#include "database.h"
#include "initfile.h"
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
    databaseSystemInit();
    init_localisation("de");

    string s = "Eure Zaubersprüche";
    cout << "strwidth(\"" << s << "\") = " << strwidth(s) << endl;
    /*char32_t c;
    utf8towc(&c, "ü");
    printf("%02X\n", c);*/
    string result = localise("%-34.34s", "Your Spells");
    show_result(result, "Eure Zaubersprüche                ");

    string msg;

    msg = localise("%s is %s.", "a gray horse", "happy");
    msg = uppercase_first(msg);
    show_result(msg, "Ein graues Pferd ist glücklich.");

    msg = localise("%s is %s.", "a gray cow", "happy");
    msg = uppercase_first(msg);
    show_result(msg, "Eine graue Kuh ist glücklich.");

    // test with @foo@ style placeholders

    map<string, string> params;
    params["The_monster"] = "the orc";
    msg = localise("@The_monster@ hits you.", params);
    show_result(msg, "Der Ork schlägt Euch.");

    params.clear();
    params["the_monster"] = "the orc";
    msg = localise("You hit @the_monster@.", params);
    show_result(msg, "Ihr schlagt den Ork.");
}
