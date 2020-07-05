/*
 * localize.h
 * High-level localization functions
 */

#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;
/*
 * Structure describing a localization argument
 */
struct LocalizationArg
{
public:
    string domain;
    string stringVal;
    string plural;

    // count of items, etc.
    int count;

    int intVal;
    long longVal;
    long long longLongVal;
    double doubleVal;
    long double longDoubleVal;

    // should this argument be translated? (defaults to true)
    bool translate;

    LocalizationArg();
    LocalizationArg(const string& value);
    LocalizationArg(const string& domain, const string& value);
    LocalizationArg(const string& value, const string& plural_val, const int count);
    LocalizationArg(const string& domain, const string& value, const string& plural_val, const int count);
    LocalizationArg(const int value);
    LocalizationArg(const long value);
    LocalizationArg(const long long value);
    LocalizationArg(const double value);
    LocalizationArg(const long double value);

private:
    void init();
};

/**
 * Initialize the localization system
 */
void init_localization(const string& lang);


// Get the current localization language
const string& get_localization_language();

// Localize a format string and a list of args.
// If there are multiple args, expects the first arg to be a format string.
// It is expected that any input strings are in English.
// If you pre-translate some strings it will probably still work in most cases.
// However, there's a remote chance that the pre-translated string may match an English
// string that we've provided a translation for and be erroneously translated again.
// One theoretical case would be: "poison"(en) -> "Gift"(de) -> "Gift"(en) -> "Geschenk"(de).
string localize(const vector<LocalizationArg>& args);

// same as localize except it capitalizes first letter
string localize_sentence(const vector<LocalizationArg>& args);

// convenience function using va_args (yuk!)
string localize(const string& fmt_str, ...);

// more convenience functions
string localize(const LocalizationArg& arg);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3);


