/**
 * @file  xlate.cc
 * @brief Low-level translation routines.
 * This works similarly to gnu gettext, except that it uses plain text files
 * (so there's no need to compile .po to .mo every time you change some text).
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

string cxlate(const string &context, const string &text_en)
{
    return text_en;
}

string cnxlate(const string &context,
        const string &singular_en, const string &plural_en, unsigned long n)
{
    return (n == 1 ? singular_en : plural_en);
}

#else
//// compile with translation logic ////

#include <clocale>
#include <libintl.h>

// markers for embedded expressions
const string exp_start = "((";
const string exp_end = "))";


// translate with context
//
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// text_en = English text to be translated
// fallback_en = fall back to English if no translation is found?
string cxlate(const string &context, const string &text_en, bool fallback_en)
{
    if (text_en.empty())
    {
        return text_en;
    }

    string translation;

    if (!context.empty())
    {
        // check for translation in specific context
        string ctx_text_en = string("{") + context + "}" + text_en;
        translation = getTranslatedString(ctx_text_en);
    }

    if (translation.empty())
    {
        // check for translation in global context
        translation = getTranslatedString(text_en);
    }

    if (translation.empty() && fallback_en)
        return text_en;
    else
        return translation;
}

// translate with context and number
// select the plural form corresponding to number
//
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// singular_en = English singular text
// plural_en = English plural text
// n = the count of whatever it is
string cnxlate(const string &context,
        const string &singular_en, const string &plural_en, unsigned long n)
{
    if (n == 1)
    {
        // translate English singular form
        return cxlate(context, singular_en);
    }
    else
    {
        // translate English plural form
        string result = cxlate(context, plural_en);

        // if the target language has multiple plural forms,
        // there will be an embedded expression for choosing the right one
        if (result.substr(0, exp_start.length()) != exp_start)
        {
            // target language has only one plural form
           return result;
        }

        // Pass in the value of n to the experssion. 
        // Surely there's a better way to do this.
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

        return plural_en;
    }
}

#endif