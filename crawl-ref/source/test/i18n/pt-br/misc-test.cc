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
    if (actual != expected)
        cout << "   expected: \"" << expected << "\"" << endl;
}

int main()
{
    Options.lang_name = "pt-br";
    SysEnv.crawl_dir = ".";
    setlocale(LC_ALL, "");
    databaseSystemInit(true);
    init_localisation("pt-br");

    string msg;

    msg = localise("the door");
    show_result(msg, "a porta");

    msg = localise("a door");
    show_result(msg, "uma porta");

    msg = localise("a large door");
    show_result(msg, "uma porta grande");

    msg = localise("a very large door");
    show_result(msg, "uma porta muito grande");

    //msg = localise("2 very large doors");
    //show_result(msg, "2 portas muito grandes");

    msg = localise("the gate");
    show_result(msg, "o portão");

    msg = localise("a gate");
    show_result(msg, "um portão");

    msg = localise("a large gate");
    show_result(msg, "um portão grande");

    msg = localise("a very large gate");
    show_result(msg, "um portão muito grande");

    // msg = localise("2 very large doors");
    // show_result(msg, "2 portãos muito grandes");

    return num_fails;
}
