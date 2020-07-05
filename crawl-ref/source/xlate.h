/**
 * @file  xlate.h
 * @brief Low-level translation routines.
 **/

#pragma once

#include <stddef.h>
#include <string>
using std::string;

// initialize
void init_xlate(const string &lang);

const string& get_xlate_language();

// translate with domain and context
//
// domain = translation file (optional, default="messages")
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid = English text to be translated
string dcxlate(const string &domain, const string &context, const string &msgid);

// translate with domain, context and number
// returns the plural form corresponding to number
//
// domain = translation file
// context = the context in which the text is being used (optional, default=none)
//  (for disambiguating same English text used in different contexts which may require different translations)
//  if no translation is found in the specified context, will look for translation at global (no) context
// msgid1 = English singular text
// msgid2 = English plural text
// n = the count of whatever it is
string dcnxlate(const string &domain, const string &context,
        const string &msgid1, const string &msgid2, unsigned long n);

// translate with context (use default domain)
static inline string cxlate(const string &context, const string &msgid)
{
    return dcxlate("", context, msgid);
}

// translate with domain (no context)
static inline string dxlate(const string &domain, const string &msgid)
{
    return dcxlate(domain, "", msgid);
}

// translate with default domain and no context
static inline string xlate(const string &msgid)
{
    return dcxlate("", "", msgid);
}

// translate with context and number (use default domain)
static inline string cnxlate(const string &context,
        const string &msgid1, const string &msgid2, unsigned long n)
{
    return dcnxlate("", context, msgid1, msgid2, n);
}

// translate with domain and number (no context)
static inline string dnxlate(const string &domain,
        const string &msgid1, const string &msgid2, unsigned long n)
{
    return dcnxlate(domain, "", msgid1, msgid2, n);
}

// translate with number (use default domain and no context)
static inline string nxlate(const string &msgid1, const string &msgid2, unsigned long n)
{
    return dcnxlate("", "", msgid1, msgid2, n);
}
