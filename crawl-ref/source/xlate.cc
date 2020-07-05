/**
 * @file  xlate.cc
 * @brief Low-level translation routines.
 * This implementation uses gnu gettext, but references to gettext are deliberately kept
 * within this file to allow easy change to a different implementation.
 **/

#include "xlate.h"

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

static const char GETTEXT_CTXT_GLUE = '\004';
static const string DEFAULT_DOMAIN = "messages";

static string language;

// initialize
void init_xlate(const string &lang)
{
    // must do this to apply user's locale because C++ sets locale to "C" by default, which won't handle unicode
    // this also probably won't work if the user's locale is not unicode (TODO: test that)
    setlocale(LC_ALL, "");

    language = lang;
    setenv("LANGUAGE", language.c_str(), 1);

    bindtextdomain("context-map", "./locale");
    bindtextdomain("messages", "./locale");
    bindtextdomain("entities", "./locale");
    bindtextdomain("monsters", "./locale");

    // set default domain
    textdomain(DEFAULT_DOMAIN.c_str());
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
// domain = translation file (optional, default="messages")
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

    // if domain not specified then fall back to default by passing NULL
    const char *dom = (domain.empty() ? NULL : domain.c_str());

    string translation;

    if (!context.empty())
    {
        // check for translation in specific context
        string ctx_msgid = context + GETTEXT_CTXT_GLUE + msgid;
        const char *xlation = dgettext(dom, ctx_msgid.c_str());
        if (xlation != NULL && ctx_msgid != xlation)
        {
            translation = xlation;
        }
    }

    if (translation.empty())
    {
        // check for translation in global context
        const char *xlation = dgettext(dom, msgid.c_str());
        if (xlation != NULL)
        {
            translation = xlation;
        }
    }

    return translation;
}

// translate with domain, context and number
// select the plural form corresponding to number
//
// domain = translation file (optional, default="messages")
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

    // if domain not specified then fall back to default by passing NULL
    const char *dom = (domain.empty() ? NULL : domain.c_str());

    string translation;

    if (!context.empty())
    {
        // check for translation in specific context
        string ctx_msgid1 = context + GETTEXT_CTXT_GLUE + msgid1;
        string ctx_msgid2 = context + GETTEXT_CTXT_GLUE + msgid2;
        const char *xlation = dngettext(dom, ctx_msgid1.c_str(), ctx_msgid2.c_str(), n);
        if (xlation != NULL && ctx_msgid1 != xlation && ctx_msgid2 != xlation)
        {
            translation = xlation;
        }
    }

    if (translation.empty())
    {
        // look in global context
        const char *xlation = dngettext(dom, msgid1.c_str(), msgid2.c_str(), n);
        if (xlation != NULL)
        {
            translation = xlation;
        }
    }

    if (translation.empty())
    {
        // still no joy - fall back on English
        translation = (n == 1 ? msgid1 : msgid2);
    }

    return translation;
}

#endif
