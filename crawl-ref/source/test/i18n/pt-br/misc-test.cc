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

    // nouns with definite article
    msg = localise("the gate");
    show_result(msg, "o portão");
    msg = localise("the door");
    show_result(msg, "a porta");

    // nouns with indefinite article
    msg = localise("a gate");
    show_result(msg, "um portão");
    msg = localise("a door");
    show_result(msg, "uma porta");

    // nouns with adjective that changes with gender
    msg = localise("a closed gate");
    show_result(msg, "um portão fechado");
    msg = localise("a closed door");
    show_result(msg, "uma porta fechada");
    msg = localise("an open gate");
    show_result(msg, "um portão aberto");
    msg = localise("an open door");
    show_result(msg, "uma porta aberta");

    // nouns with adjective that doesn't change with gender
    msg = localise("a cyan gate");
    show_result(msg, "um portão ciano");
    msg = localise("a cyan door");
    show_result(msg, "uma porta ciano");

    msg = localise("a large gate");
    show_result(msg, "um portão grande");
    msg = localise("a large door");
    show_result(msg, "uma porta grande");

    // nouns with adverb and adjective
    msg = localise("a very large gate");
    show_result(msg, "um portão muito grande");
    msg = localise("a very large door");
    show_result(msg, "uma porta muito grande");

    // nouns with adjective that becomes "with <whatever>"
    msg = localise("a runed gate");
    show_result(msg, "um portão com runas");
    msg = localise("a runed door");
    show_result(msg, "uma porta com runas");

    msg = localise("a topaz-encrusted gate");
    show_result(msg, "um portão incrustado com topázio");
    msg = localise("a topaz-encrusted door");
    show_result(msg, "uma porta incrustada com topázio");

    msg = localise("a emerald-encrusted gate");
    show_result(msg, "um portão incrustado com esmeralda");
    msg = localise("a emerald-encrusted door");
    show_result(msg, "uma porta incrustada com esmeralda");

    //msg = localise("2 very large doors");
    //show_result(msg, "2 portas muito grandes");
    // msg = localise("2 very large doors");
    // show_result(msg, "2 portãos muito grandes");

    // book titles

    msg = localise("Easy Casting");
    show_result(msg, "Conjuração Fácil");
    msg = localise("Advanced Casting");
    show_result(msg, "Conjuração Avançado");
    msg = localise("Sophisticated Casting");
    show_result(msg, "Conjuração Sofisticado");

    msg = localise("Easy Magic");
    show_result(msg, "Magia Fácil");
    msg = localise("Advanced Magic");
    show_result(msg, "Magia Avançada");
    msg = localise("Sophisticated Magic");
    show_result(msg, "Magia Sofisticada");

    msg = localise("Easy Rites and Rhymes");
    show_result(msg, "Ritos e Rimas Fáceis");
    msg = localise("Advanced Rites and Rhymes");
    show_result(msg, "Ritos e Rimas Avançadas");
    msg = localise("Sophisticated Rites and Rhymes");
    show_result(msg, "Ritos e Rimas Sofisticadas");

    return num_fails;
}
