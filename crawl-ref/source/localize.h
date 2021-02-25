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

/**
 * Temporarily stop translating
 */
void pause_localization();

/**
 * Resume translating
 */
void unpause_localization();

// Get the current localization language
const string& get_localization_language();

/**
 *  Localize a list of args.
 *  @param args The list of arguments.
 *              If there's more than one, the first one is expected to be a printf-style format string.
 */
string localize(const vector<LocalizationArg>& args);

// localize using va_list (yuk!)
string vlocalize(const string& fmt_str, va_list args);

// convenience functions
template<typename... Ts>
static string localize(const string& first, Ts const &... rest)
{
    vector<LocalizationArg> v {first, rest...};
    return localize(v);
}

/**
 * Get the localized equivalent of a single-character
 * (Mostly for prompt answers like Y/N)
 */
int localize_char(char ch);



