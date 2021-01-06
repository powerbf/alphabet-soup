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
    LocalizationArg(const char* value);
    LocalizationArg(const string& value, const string& plural_val, const int count);
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
string localize(const vector<LocalizationArg>& args, const bool capitalize = false);

// localize using va_list (yuk!)
string vlocalize(const string& fmt_str, va_list args, const bool capitalize = false);

// convenience functions
// TODO: Rewrite using variadic template
string localize(const LocalizationArg& arg);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3, const LocalizationArg& arg4);
string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3, const LocalizationArg& arg4,
                const LocalizationArg& arg5);

/**
 * Get the localized equivalent of a single-character
 * (Mostly for prompt answers like Y/N)
 */
int localize_char(char ch);



