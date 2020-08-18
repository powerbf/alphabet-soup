#include "AppHdr.h"
#include "fake-main.hpp"
#include "localize.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
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
    "the +7 pair of quick blades \"Gyre\" and \"Gimble\"",


    "a long sword, 30 bolts and an arbalest",
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
    "ein +6 Ring der Intelligenz (760 Gold)",
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
    "das +7 Paar Schnellklingen \"Gyre\" und \"Gimble\"",


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

vector<map<string, string>> wands =
{
    {
        {"en", "an ivory wand"},
        {"nom", "ein Elfenbeinzauberstab"},
        {"acc", "einen Elfenbeinzauberstab"}
    },
    {
        {"en", "a cursed iron wand"},
        {"nom", "ein verfluchter Eisenzauberstab"},
        {"acc", "einen verfluchten Eisenzauberstab"}
    },
    {
        {"en", "a wand of flame (13)"},
        {"nom", "ein Zauberstab des Flammens (13)"},
        {"acc", "einen Zauberstab des Flammens (13)"}
    },
};

vector<map<string, string>> rings =
{
    {
        {"en", "a diamond ring"},
        {"nom", "ein Diamantring"},
        {"acc", "einen Diamantring"}
    },
    {
        {"en", "an ivory ring"},
        {"nom", "ein Elfenbeinring"},
        {"acc", "einen Elfenbeinring"}
    },
    {
        {"en", "an cursed ivory ring"},
        {"nom", "ein verfluchter Elfenbeinring"},
        {"acc", "einen verfluchten Elfenbeinring"}
    },
    {
        {"en", "a small silver ring"},
        {"nom", "ein kleiner Silberring"},
        {"acc", "einen kleinen Silberring"}
    },
    {
        {"en", "a ring of wizardry"},
        {"nom", "ein Ring der Zauberei"},
        {"acc", "einen Ring der Zauberei"}
    },
    {
        {"en", "an uncursed ring of protection from fire"},
        {"nom", "ein unverfluchter Ring der Feuerresistenz"},
        {"acc", "einen unverfluchten Ring der Feuerresistenz"}
    },
    {
        {"en", "an uncursed +4 ring of protection"},
        {"nom", "ein unverfluchter +4 Ring des Schutzes"},
        {"acc", "einen unverfluchten +4 Ring des Schutzes"}
    },
    {
        {"en", "the ring \"Cuti\" {rF+ MR+ Dex+2}"},
        {"nom", "der Ring \"Cuti\" {rF+ MR+ Ges+2}"},
        {"acc", "der Ring \"Cuti\" {rF+ MR+ Ges+2}"}
    },
};

vector<map<string, string>> amulets =
{
    {
        {"en", "a filigree amulet"},
        {"nom", "ein Filigranamulett"},
        {"acc", "ein Filigranamulett"}
    },
    {
        {"en", "a dented ruby amulet"},
        {"nom", "ein verbeultes Rubinamulett"},
        {"acc", "ein verbeultes Rubinamulett"}
    },
    {
        {"en", "a cursed thin beryl amulet"},
        {"nom", "ein verfluchtes dünnes Beryllamulett"},
        {"acc", "ein verfluchtes dünnes Beryllamulett"}
    },
    {
        {"en", "an amulet of the acrobat"},
        {"nom", "ein Amulett des Akrobaten"},
        {"acc", "ein Amulett des Akrobaten"}
    },
    {
        {"en", "a cursed amulet of inaccuracy"},
        {"nom", "ein verfluchtes Amulett der Ungenauigkeit"},
        {"acc", "ein verfluchtes Amulett der Ungenauigkeit"}
    },
};

vector<map<string, string>> runes =
{
    {
        {"en", "the demonic rune"},
        {"nom", "die dämonische Rune"},
        {"acc", "die dämonische Rune"}
    },
    {
        {"en", "the golden rune of Zot"},
        {"nom", "die goldene Rune von Zot"},
        {"acc", "die goldene Rune von Zot"}
    },
    {
        {"en", "an obsidian rune of Zot"},
        {"nom", "eine Obsidianrune von Zot"},
        {"acc", "eine Obsidianrune von Zot"}
    },
    {
        {"en", "silver rune of Zot"},
        {"nom", "silberne Rune von Zot"},
        {"acc", "silberne Rune von Zot"}
    }
};

vector<map<string, string>> staves =
{
    {
        {"en", "a weird smoking staff"},
        {"nom", "ein seltsamer rauchiger Stecken"},
        {"acc", "einen seltsamen rauchigen Stecken"}
    },
    {
        {"en", "a cursed weird smoking staff"},
        {"nom", "ein verfluchter seltsamer rauchiger Stecken"},
        {"acc", "einen verfluchten seltsamen rauchigen Stecken"}
    },
    {
        {"en", "a staff of earth"},
        {"nom", "ein Stecken der Erde"},
        {"acc", "einen Stecken der Erde"}
    },
    {
        {"en", "an uncursed staff of fire"},
        {"nom", "ein unverfluchter Stecken des Feuers"},
        {"acc", "einen unverfluchten Stecken des Feuers"}
    },
};

vector<map<string, string>> books =
{
    {
        {"en", "a hardcover book"},
        {"nom", "ein gebundenes Buch"},
        {"acc", "ein gebundenes Buch"}
    },
    {
        {"en", "a dog-eared paperback book"},
        {"nom", "ein schäbiges Taschenbuch"},
        {"acc", "ein schäbiges Taschenbuch"}
    },
    {
        {"en", "a book of Clouds"},
        {"nom", "ein Buch der Wolken"},
        {"acc", "ein Buch der Wolken"}
    },
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

static void test_group(const string& casus, const string& group_name, vector<map<string, string>>& items)
{
    cout << endl << group_name << ":" << endl;
    for (size_t i = 0; i < items.size(); i++)
    {
        map<string, string>& item = items.at(i);
        if (item.count(casus) != 0)
            test(casus, item["en"], item[casus]);
    }
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

        test_group(cse, "RINGS", rings);
        test_group(cse, "AMULETS", amulets);
        test_group(cse, "WANDS", wands);
        test_group(cse, "STAVES", staves);
        test_group(cse, "BOOKS", books);
        test_group(cse, "RUNES", runes);

    }

    cout << endl << num_passes << " passed, "<< num_fails << " failed" << endl;
    return 0;
}
