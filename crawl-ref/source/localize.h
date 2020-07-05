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

/**
 *  Localize a list of args.
 *  @param args The list of arguments.
 *              If there's more than one, the first one is expected to be a printf-style format string.
 *  @capitalize Capitalize the first letter of the result? (true by default)
 */
string localize(const vector<LocalizationArg>& args, const bool capitalize = true);

// convenience function using va_args (yuk!)
string localize(const string& fmt_str, ...);

// more convenience functions
string localize(const LocalizationArg& arg, const bool capitalize = true);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const bool capitalize = true);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3, const bool capitalize = true);


