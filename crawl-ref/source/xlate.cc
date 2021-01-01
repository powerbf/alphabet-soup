/**
 * @file  xlate.cc
 * @brief Low-level translation routines.
 * This implementation uses gnu gettext, but references to gettext are deliberately kept
 * within this file to allow easy change to a different implementation.
 **/

#include "AppHdr.h"
#include "xlate.h"
#include "clua.h"
#include "database.h"
#include "stringutil.h"

#include <cstring>
using namespace std;

#ifdef NO_TRANSLATE
//// compile without translation logic ////

void init_xlate(const string &lang)
{
}

string get_xlate_language()
{
    return "";
}

string dcxlate(const string &domain, const string &context, const string &msgid)
{
    return msgid;
}

string dcnxlate(const string &domain, const string &context,
        const string &msgid1, const string &msgid2, unsigned long n)
{
    return (n == 1 ? msgid1 : msgid2);
}

#else
//// compile with translation logic ////

#include <clocale>
#include <libintl.h>

// markers for embedded expressions
const string exp_start = "((";
const string exp_end = "))";

static string language;

// initialize
void init_xlate(const string &lang)
{
    language = lang;
}

const string& get_xlate_language()
{
    return language;
}

// skip translation if language is English (or unspecified which implies English)
static inline bool skip_translation()
{
    return (language.empty() || language == "en");
}

// translate with domain and context
//
// domain = translation file (optional, default="strings")
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid = English text to be translated
//
// NOTE: this is different to dpgettext in two ways:
//  1) dpgettext can only handles string literals (not variables). This can handle either.
//  2) if context is NULL or empty then this falls back to contextless gettext
string dcxlate(const string &domain, const string &context, const string &msgid)
{
    if (skip_translation() || msgid.empty())
    {
        return msgid;
    }

    string translation;

    if (!context.empty())
    {
        // check for translation in specific context
        string ctx_msgid = string("{") + context + "}" + msgid;
        translation = getTranslatedString(ctx_msgid);
    }

    if (translation.empty())
    {
        // check for translation in global context
        translation = getTranslatedString(msgid);
    }

    if (translation.empty())
        return msgid;
    else
        return translation;
}

// translate with domain, context and number
// select the plural form corresponding to number
//
// domain = translation file (optional, default="strings")
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid1 = English singular text
// msgid2 = English plural text
// n = the count of whatever it is
//
// NOTE: this is different to dpngettext in two ways:
//  1) dpngettext can only handles string literals (not variables). This can handle either.
//  2) if context is NULL or empty then this falls back to contextless ngettext
string dcnxlate(const string &domain, const string &context,
        const string &msgid1, const string &msgid2, unsigned long n)
{
    if (skip_translation() || msgid1.empty() || msgid2.empty())
    {
        // apply English rules
        return (n == 1 ? msgid1 : msgid2);
    }

    if (n == 1)
    {
        return dcxlate(domain, context, msgid1);
    }
    else
    {
        string result = dcxlate(domain, context, msgid2);

        if (result.substr(0, exp_start.length()) != exp_start)
            // single plural form
           return result;

        // pass in the value of n. Surely there's a better way to do this.
        string lua_prefix = make_stringf("n=%ld\nreturn ", n);

        vector<string> lines = split_string("\n", result, false, false);
        for (const string& line: lines)
        {
            size_t pos1 = line.find(exp_start);
            size_t pos2 = line.rfind(exp_end);
            if (pos1 != string::npos && pos2 != string::npos)
            {
                string condition = line.substr(pos1 + exp_start.length(), pos2 - pos1 - exp_start.length());
                if (condition == "")
                {
                    // unconditional
                    return line.substr(pos2 + exp_end.length());
                }

                // evaluate the condition
                string lua = lua_prefix + condition;
                if (clua.execstring(lua.c_str(), "plural_lua", 1))
                {
                    // error
                    continue;
                }

                bool res;
                clua.fnreturns(">b", &res);
                if (res)
                    return line.substr(pos2 + exp_end.length());
            }
        }

        return msgid2;
    }
}

#endif
