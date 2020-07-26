#include "AppHdr.h"
#include "fake-main.hpp"
#include "localize.h"

#include <iostream>
#include <string>
#include <vector>
using namespace std;


const vector<string> cases = {
    "nom"
};

const vector<string> test_items = {

    "a scroll labeled BLAHDEY BLAH",
    "3 scrolls labeled FOO",

    "a scroll of blinking",
    "2 scrolls of remove curse",

    "a green potion",
    "10 smoky black potions",

    "a potion of heal wounds",
    "5 potions of curing",

    "a +4 ring of protection (468 gold)",
    "a +6 ring of intelligence (760 gold)",
    "a cursed -4 ring of dexterity (26 gold)",

    "an orc corpse (skeletalised by now)",
    "a goblin corpse (skeletalised by now)",

    "a wand of digging (18)",
    "a lightning rod (3/4)",
    "2 boxes of beasts",

    "the ring of Fedhas's Hope {MP+9 Int+4 Dex+2}",

    "a knobbly runed staff",

    "club",
    "+9 mace",
    "an arrow",
    "27 arrows",
    "a broad axe",
    "a glowing broad axe",
    "a runed whip",
    "a - a +9 broad axe of holy wrath",
    "a +0 giant spiked club",
    "a cursed -1 giant club",
    "a +0 vorpal battleaxe",
    "a +0 broad axe of flaming",
    "a -1 broad axe of freezing",
    "a cursed -2 broad axe of draining",
    "the +11 broad axe \"Jetioplo\" (weapon) {vorpal, Str+4}",
    "the +9 trident of the Crushed Allies {vorpal, Fragile +Inv Str+3 Int+3}",


    "a long sword, 30 bolts and an arbalest"
};

const vector<string> expected = {

    "eine Schriftrolle beschriftet mit BLAHDEY BLAH",
    "3 Schriftrollen beschriftet mit FOO",

    "eine Schriftrolle des Flimmerns",
    "2 Schriftrollen des Segnens",

    "ein grüner Zaubertrank",
    "10 rauchige schwarze Zaubertränke",

    "ein Heiltrank",
    "5 Tränke des Gegenmittels",

    "ein +4 Ring des Schutzes (468 Gold)",
    "ein +6 Eing der Intelligenz (760 Gold)",
    "ein verfluchter -4 Ring der Geschicklichkeit (26 Gold)",

    "eine Leiche eines Orkes (inzwischen skelettiert)",
    "eine Leiche eines Goblins (inzwischen skelettiert)",

    "ein Zauberstab des Grabens (18)",
    "eine Blitzstange (3/4)",
    "2 Kisten der Bestien",

    "der Ring von \"Fedhas's Hope\"  {MP+9 Int+4 Ges+2}",

    "??? a knobbly runed staff ???",

    "Keule",
    "+9 Streitkolben",
    "ein Pfeil",
    "27 Pfeile",
    "eine Breitaxt",
    "eine glühende Breitaxt",
    "eine runenbesetzte Peitsche",
    "a - eine +9 Breitaxt des heiligen Zorns",
    "eine +0 riesige Stachelkeule",
    "eine verfluchte -1 Riesenkeule",
    "eine +0 Vorpal-Streitaxt",
    "eine +0 Breitaxt des Flammens",
    "eine -1 Breitaxt des Einfrierens",
    "eine verfluchte -2 Breitaxt des Entleerens",
    "die +11 Breitaxt \"Jetioplo\" (Waffe) {Vorpal, Stä+4}",
    "der +9 Dreizack von \"the Crushed Allies\" {vorpal, zerbrechlich +Uns Stä+3 Int+3}",


    "ein Langschwert, 30 Bolzen und eine Armbrust"
};

const vector<string> armour_en =
{
    "a leather armour",
    "a +1 scale mail",
    "an uncursed robe",
    "a cursed -1 chain mail",
    "a pair of gloves",
    "an enchanted pair of boots",
    "a pair of runed gloves",
    "a cursed pair of runed gloves",
    "an uncursed +0 pair of boots",
    "a cursed +2 pair of glowing boots",
    "an uncursed +2 pair of gloves of archery",
    "the +1 plate armour of Dice, Bag, and Bottle {Str+6 Int-3 Dex+4 SInv} (5390 gold)"
};

const vector<string> armour_de =
{
    "eine Lederrüstung",
    "ein +1 Schuppenpanzer",
    "eine unverfluchte Robe",
    "ein verfluchter -1 Kettenpanzer",
    "ein Paar Handschuhe",
    "ein verzaubertes Paar Stiefel",
    "ein Paar runenbesetzte Handschuhe",
    "ein verfluchtes Paar runenbesetzte Handschuhe",
    "ein unverfluchtes +0 Paar Stiefel",
    "ein verfluchtes +2 Paar glühende Stiefel",
    "ein unverfluchtes +2 Paar Handschuhe der Schießkunst",
    "der +1 Plattenpanzer von Würfel, Beutel, und Flasche {Stä+6 Int-3 Ges+4 UnsS} (5390 Gold)"
};

const vector<string> wands_en =
{
        "an ivory wand",
        "a cursed iron wand",
        "a wand of flame (13)"
};

const vector<string> wands_de =
{
        "ein Elfenbeinzauberstab",
        "ein verfluchter Eisenzauberstab",
        "ein Zauberstab des Flammens (13)"
};


int num_passes = 0;
int num_fails = 0;

static void test(const string& context, const string& item, const string& expect)
{
    string fmt = "{" + context + "}%s";
    LocalizationArg f = LocalizationArg(fmt);
    LocalizationArg it = LocalizationArg(item);
    string actual = localize(f, it);

    string status;
    if (actual == expect)
    {
        num_passes++;
        status = "PASS";
    }
    else
    {
        num_fails++;
        status = "*FAIL*";
    }

    cout << status << ": \"" << item << "\" (" << context << ")" << " -> \"" << actual << "\"" << endl;

}

int main()
{
    init_localization("de");

    const int num_cases = cases.size();
    const int num_items = test_items.size();

    for (int i = 0; i < num_cases; i++)
    {
        string cse = cases[i];
        for (int j = 0; j < num_items; j++)
        {
            test(cse, test_items[j], expected[j]);
        }

        cout << endl << "ARMOUR:" << endl;
        for (size_t j = 0; j < armour_en.size(); j++)
        {
            test(cse, armour_en[j], armour_de[j]);
        }

        cout << endl << "WANDS:" << endl;
        for (size_t j = 0; j < wands_en.size(); j++)
        {
            test(cse, wands_en[j], wands_de[j]);
        }
    }

    cout << endl << num_passes << " passed, "<< num_fails << " failed" << endl;
    return 0;
}
