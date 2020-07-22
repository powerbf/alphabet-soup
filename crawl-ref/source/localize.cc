/*
 * localize.h
 * High-level localization functions
 */

// don't need this, but forced to include because of stuff included by stringutil.h
#include "AppHdr.h"

#include <sstream>
#include <vector>
#include <cstdarg>
#include <cstdlib>
#include <regex>
#include <typeinfo>
using namespace std;

#include "localize.h"
#include "xlate.h"
#include "stringutil.h"
#include "unicode.h"
#include "english.h"

// check if string contains the char
static inline bool _contains(const std::string& s, char c)
{
    return (s.find(c) != string::npos);
}

// is this char a printf typespec (i.e. the end of %<something><char>)?
static inline bool _is_type_spec(char c)
{
    static const string type_specs = "diufFeEgGxXoscpaAn";
    return (type_specs.find(c) != string::npos);
}

// Extract arg id from format specifier
// (e.g. for the specifier "%2$d", return 2)
// Returns 0 if no positional specifier
static int _get_arg_id(const string& fmt)
{
    size_t dollar_pos = fmt.find('$');
    if (dollar_pos == string::npos)
    {
        return 0;
    }
    size_t pos_len = dollar_pos-1;
    if (pos_len < 1)
    {
        return 0;
    }
    string pos_str = fmt.substr(1, pos_len);
    int result = atoi(pos_str.c_str());
    if (errno != 0)
    {
        return 0;
    }
    return result;
}

// Remove arg id from arg format specifier
// (e.g. for "%1$d", return "%d")
static string _remove_arg_id(const string& fmt)
{
    size_t dollar_pos = fmt.find('$');
    if (dollar_pos == string::npos)
    {
        return fmt;
    }
    string result("%");
    if (dollar_pos < fmt.length()-1)
    {
        result += fmt.substr(dollar_pos+1, string::npos);
    }
    return result;
}

// translate a format spec like %d to a type like int
static const type_info* _format_spec_to_type(const string& fmt)
{
    if (fmt.length() < 2 || fmt.at(0) != '%')
    {
        return NULL;
    }

    char last_char = fmt.back();
    if (last_char == 'p')
    {
        return &typeid(void*);
    }
    else if (last_char == 's')
    {
        return &typeid(char*);
    }
    else if (last_char == 'n')
    {
        return &typeid(int*);
    }
    else if (_contains("aAeEfFgG", last_char))
    {
        if (fmt.at(fmt.length()-2) == 'L')
        {
            return &typeid(long double);
        }
        else
        {
            return &typeid(double);
        }
    }
    else if (_contains("uoxX", last_char))
    {
        // unsigned
        char penultimate = fmt.at(fmt.length()-2);
        if (penultimate == 'p')
        {
            return &typeid(ptrdiff_t);
        }
        else if (penultimate == 'z')
        {
            return &typeid(size_t);
        }
        else if (penultimate == 'j')
        {
            return &typeid(uintmax_t);
        }
        else if (penultimate == 'l')
        {
            if (fmt.length() > 2 && fmt.at(fmt.length()-3) == 'l')
            {
                return &typeid(unsigned long long);
            }
            else
            {
                return &typeid(unsigned long);
            }
        }
        else
        {
            return &typeid(unsigned);
        }
    }
    else if (_contains("cid", last_char))
    {
        // signed int
        char penultimate = fmt.at(fmt.length()-2);
        if (penultimate == 'p')
        {
            return &typeid(ptrdiff_t);
        }
        else if (penultimate == 'z')
        {
            return &typeid(size_t);
        }
        else if (penultimate == 'j')
        {
            return &typeid(intmax_t);
        }
        else if (penultimate == 'l')
        {
            if (fmt.length() > 2 && fmt.at(fmt.length()-3) == 'l')
            {
                return &typeid(long long);
            }
            else
            {
                return &typeid(long);
            }
        }
        else
        {
            return &typeid(int);
        }
    }

    return NULL;
}

// split format string into constants and format specifiers
static vector<string> _split_format(const string& fmt_str)
{
    vector<string> results;

    int token_start = 0;
    int token_len = 0;
    const char* ch = fmt_str.c_str();

    while (*ch != '\0')
    {
        token_start += token_len;
        token_len = 1;
        bool finished = false;

        if (*ch == '%' && *(ch+1) != '%')
        {
            // read format specifier
            ++ch;
            while (!finished)
            {
                finished = (*ch == '\0' || _is_type_spec(*ch));
                if (*ch != '\0')
                {
                    ++token_len;
                    ++ch;
                }
            }
        }
        else if (*ch == '{')
        {
            // read context specifier
            ++ch;
            while (!finished)
            {
                finished = (*ch == '\0' || *ch == '}');
                if (*ch != '\0')
                {
                    ++token_len;
                    ++ch;
                }
            }
        }
        else
        {
            // read literal string
            while (!finished)
            {
                bool escaped = (*ch == '\\' || (*ch == '%' && *(ch+1) == '%'));
                ++ch;
                if (*ch == '\0')
                {
                    finished = true;
                }
                else if (escaped)
                {
                    // ignore character
                    ++token_len;
                    finished = false;
                }
                else
                {
                    finished = (*ch == '%' || *ch == '{');
                    if (!finished)
                    {
                        ++token_len;
                    }
                }
            }
        }

        // extract token
        results.push_back(fmt_str.substr(token_start, token_len));
    }
    return results;
}

/**
 * Get arg types from format string.
 * Returns a map indexed by argument id, beginning with 1.
 */
static map<int, const type_info*> _get_arg_types(const string& fmt)
{
    map<int, const type_info*> results;
    vector<string> strings = _split_format(fmt);
    int arg_count = 0;
    for (vector<string>::iterator it = strings.begin() ; it != strings.end(); ++it)
    {
        if (it->at(0) == '%' && it->length() > 1 && it->at(1) != '%')
        {
            ++arg_count;
            const type_info* ti = _format_spec_to_type(*it);
            int arg_id = _get_arg_id(*it);
            if (arg_id == 0)
            {
                arg_id = arg_count;
            }
            results[arg_id] = ti;
        }
    }
    return results;
}

// replace all instances of given substring
// std::regex_replace would do this, but not sure if available on all platforms
static void _replace_all(std::string& str, const std::string& patt, const std::string& replace)
{
    std::string::size_type pos = 0u;
    while((pos = str.find(patt, pos)) != std::string::npos){
        str.replace(pos, patt.length(), replace);
        pos += replace.length();
    }
}

static void _resolve_escapes(string& str)
{
    _replace_all(str, "%%", "%");
    _replace_all(str, "\\{", "{");
    _replace_all(str, "\\}", "}");
}


static string _localize_annotation(const string& s)
{
    string result = xlate(s);
    if (result != s)
        return result;

    // TODO: break it down

    return s;
}

static list<string> _localize_annotations(const list<string>& input)
{
    list<string> output;

    list<string>::const_iterator iter;
    for (iter = input.begin(); iter != input.end(); ++iter)
    {
        output.push_back(_localize_annotation(*iter));
    }

    return output;
}


/**
 * strip annotations in curly or round brackets from end of string
 *
 * @param  s            the string
 * @param  annotations  stripped annotations will be returned in this list
 * @return the string with the annotations stripped off
 */
static string _strip_annotations(const string& s, list<string>& annotations)
{
    string rest = s;
    size_t pos;

    while ((pos = rest.find_last_of("{(")) != string::npos)
    {
        // get the leading whitespace as well
        while(pos > 0 && rest[pos-1] == ' ')
            pos--;

        annotations.push_front(rest.substr(pos));
        rest = rest.substr(0, pos);
    }
    return rest;
}

/*
 * Add annotations to the end of a string
 * No brackets or whitespace are added because it's expected that the
 * annotations contain those already
 */
static string _add_annotations(const string& s, const list<string>& annotations)
{
    string result = s;

    list<string>::const_iterator iter;
    for (iter = annotations.begin(); iter != annotations.end(); ++iter)
    {
        result += *iter;
    }

    return result;
}

/*
 * Strip count from a string like "27 arrows"
 *
 * @param  s      the string
 * @param  count  the count will be returned here
 * @return the string with the count and following spaces removed
 */
static string _strip_count(const string& s, int& count)
{
    if (s.empty() || !isdigit(s[0]))
    {
        return s;
    }

    size_t pos;
    count = stoi(s, &pos);

    string rest = s.substr(pos);
    return trim_string_left(rest);
}

// forward declarations
static string _localize_string(const string& context, const string& value);
static string _localize_counted_string(const string& context, const string& singular,
                                       const string& plural, const int count);
// localize counted string when you only have the plural
static string _localize_counted_string(const string& context, const string& value);
static string _localize_list(const string& context, const string& value);

static string _localize_labeled_scroll(const string& context, const string& name)
{
    static const string pattern = " labeled ";

    size_t pos = name.find(pattern);
    if (pos == string::npos)
    {
        // not a labeled scroll
        return name;
    }

    // separate the label from the rest
    pos += pattern.length();
    string label = name.substr(pos);
    label = cxlate(context, label);

    string rest = name.substr(0, pos);
    rest += "%s";

    string result;
    if (isdigit(rest[0]))
    {
        // plural
        int count;
        string plural = _strip_count(rest, count);
        string singular = replace_first(plural, "scrolls", "scroll");
        singular = article_a(singular);
        plural = "%d " + plural;
        result = cnxlate(context, singular, plural, count);
        return make_stringf(result.c_str(), count, label.c_str());
    }
    else
    {
        // singular
        result = cxlate(context, rest);
        return make_stringf(result.c_str(), label.c_str());
    }
}

// try to localize complex item name
//
// Some examples:
//   a +0 broad axe
//   a +7 broad axe of flaming
//   the +11 broad axe "Jetioplo" (weapon) {vorpal, Str+4}
//   the +18 shield of the Gong (worn) {rElec rN+ MR+ EV-5}
//   the +2 pair of boots of Boqauskewui (worn) {*Contam rN+++ rCorr SInv}
//
//
static string _localize_complex_item_name(const string& context, const string& item)
{
    if (item.empty())
    {
        return item;
    }

    // break it up
    string determiner, count, base, name_or_brand;
    bool named_owner = false;

    vector<string> tokens = split_string(" ", item, true);
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const string& token = tokens[i];
        if (i == 0)
        {
            string tok_lower = lowercase_string(token);
            if (tok_lower == "a" || tok_lower == "an" || tok_lower == "the"
                     || tok_lower == "his" || tok_lower == "her" || tok_lower == "its"
                     || tok_lower == "their" || tok_lower == "your")
            {
                determiner = tok_lower;
                continue;
            }
            else if (ends_with(token,"'s"))
            {
                // determiner is a possessive like "the orc's" or "Sigmund's"
                determiner = token;
                named_owner = true;
                continue;
            }
        }

        if (!name_or_brand.empty())
        {
            // add to the brand or artefact name
            name_or_brand += " " + token;
        }
        else if (token[0] == '"' || (token == "of" && i > 0 && tokens[i-1] != "pair"))
        {
            // start of brand or artefact name
            name_or_brand = " " + token;
        }
        else if (regex_match(token, regex("^[0-9]+$")))
        {
            count = token;
        }
        else if (!base.empty())
        {
            base += " " + token;
        }
        else
        {
            base = token;
        }
    }

    string result;
    bool success = false;

    // change his/her/its/their to a/an for ease of translation
    // (owner may not have the same gender in other languages)
    if (determiner == "his" || determiner == "her" || determiner == "its" || determiner == "their")
    {
        determiner = base.find_first_of("aeiou") == 0 ? "an" : "a";
    }

    // try to construct a string that can be translated

    vector<string> words = split_string(" ", base, true);
    for (size_t i = 0; i < words.size() && !success; i++)
    {
        string noun;

        if (!determiner.empty())
        {
            noun = named_owner ? "%s" : determiner;
            noun += " ";
        }
        if (!count.empty())
        {
            noun += count + " ";
        }
        // placeholder for adjectives
        noun += i > 0 ? "%s" : "";

        for (size_t j = i; j < words.size(); j++)
        {
            noun += j == i ? "" : " ";
            noun += words[j];
        }

        string branded_noun = noun + name_or_brand;

        // first, try with brand attached
        result = count.empty()
                 ? cxlate(context, branded_noun)
                 : _localize_counted_string(context, branded_noun);
        success = (result != branded_noun);


        if (!success)
        {
            // now try without brand attached
            result = count.empty()
                     ? cxlate(context, noun)
                     : _localize_counted_string(context, noun);
            success = (result != noun);

            if (success)
            {
                // TODO: make this better
                result += cxlate(context, name_or_brand);
            }
        }

        if (success)
        {
            if (named_owner)
            {
                string owner = cxlate(context, determiner);
                result = replace_first(result, "%s", owner);
            }

            if (i != 0)
            {
                size_t pos = result.find("%s");
                if (pos != string:: npos)
                {
                    // find the context for the adjectives
                    string new_context = context;
                    size_t ctx_pos = result.rfind("{", pos);
                    if (ctx_pos != string::npos)
                    {
                        size_t ctx_end = result.find("}", ctx_pos);
                        if (ctx_end != string::npos)
                        {
                            new_context = result.substr(ctx_pos + 1, ctx_end - ctx_pos - 1);
                            result.erase(ctx_pos, ctx_end - ctx_pos +1);
                        }
                    }
                    // localize adjectives
                    string adjectives;
                    for (size_t k = 0; k < i; k++)
                    {
                        adjectives += cxlate(new_context, words[k] + " ");
                    }
                    result = replace_first(result, "%s", adjectives);
                }
            }

            return result;
        }

    }

    // failed
    return item;
}

// localize a string containing a list of things (joined by commas, "and", "or")
// does nothing if input is not a list
static string _localize_list(const string& context, const string& s)
{
    static vector<string> separators = {",", " or ", " and "};

    // annotations can contain commas, so remove them for the moment
    list<string> annotations;
    string value = _strip_annotations(s, annotations);

    std::vector<string>::iterator it;
    for (it = separators.begin(); it != separators.end(); ++it)
    {
        string sep = *it;
        // split on the first separator only
        vector<string> tokens = split_string(sep, value, true, true, 1);
        // there should only ever be 1 or 2 tokens
        if (tokens.size() == 2)
        {
            // restore annotations
            tokens[1] = _add_annotations(tokens[1], annotations);

            string fmt = "%s" + sep + (sep == "," ? " " : "") + "%s";
            fmt = cxlate(context, fmt);
            // the tokens could be lists themselves
            string tok0 = _localize_string(context, tokens[0]);
            string tok1 = _localize_string(context, tokens[1]);
            return make_stringf(fmt.c_str(), tok0.c_str(), tok1.c_str());
        }
    }

    // not a list
    return s;
}


// localize a string with count
static string _localize_counted_string(const string& context, const string& singular,
                                       const string& plural, const int count)
{
    string result;
    result = cnxlate(context, singular, plural, count);
    ostringstream cnt;
    cnt << count;
    result = replace_first(result, "%d", cnt.str());
    return result;
}

// localize counted string when you only have the plural
static string _localize_counted_string(const string& context, const string& value)
{
    if (value.empty() || !isdigit(value[0]))
    {
        return value;
    }

    int count;
    string plural = _strip_count(value, count);
    string singular = article_a(singularise(plural));
    plural = "%d " + plural;

    return _localize_counted_string(context, singular, plural, count);
}

// localize a singular string
static string _localize_string(const string& context, const string& value)
{
    if (value.empty())
    {
        return value;
    }

    string result = cxlate(context, value);
    if (result != value)
    {
        // got a hit
        return result;
    }

    string trimmed = trimmed_string(value);
    if (trimmed.length() < 8 || count_occurrences(trimmed, " ") < 2)
    {
        // There's no point trying anything further
        return value;
    }

    // remove annotations
    list<string> annotations;
    string rest = _strip_annotations(value, annotations);

    if (!annotations.empty())
    {
        annotations = _localize_annotations(annotations);

        if (rest.empty())
        {
            return _add_annotations(rest, annotations);
        }

        result = cxlate(context, rest);
        if (result != rest)
        {
            return _add_annotations(result, annotations);
        }
    }

    if (contains(rest, "labeled") && contains(rest, "scroll"))
    {
        result = _localize_labeled_scroll(context, rest);
        return _add_annotations(result, annotations);
    }

    if (isdigit(rest[0]))
    {
        // probably a plural
        int count;
        string plural = _strip_count(rest, count);
        string singular = article_a(singularise(plural));
        plural = "%d " + plural;

        result = _localize_counted_string(context, singular, plural, count);
        if (result != rest)
        {
            return _add_annotations(result, annotations);
        }
    }

    // try treating it as a complex item name
    result = _localize_complex_item_name(context, rest);
    if (result != rest)
    {
        return _add_annotations(result, annotations);
    }

    if (contains(value, ",") || contains(value, " and ") or contains(value, " or "))
    {
        // try treating it as a list
        return _localize_list(context, value);
    }

    return value;
}

static string _localize_string(const string& context, const LocalizationArg& arg)
{
    if (arg.plural.empty())
        return _localize_string(context, arg.stringVal);
    else
        return _localize_counted_string(context, arg.stringVal,
                                        arg.plural, arg.count);
}

void LocalizationArg::init()
{
    intVal = 0;
    longVal = 0L;
    longLongVal = 0L;
    doubleVal = 0.0;
    longDoubleVal = 0.0;
    count = 1;
    translate = true;
}

LocalizationArg::LocalizationArg()
{
    init();
}

LocalizationArg::LocalizationArg(const string& value)
    : stringVal(value)
{
    init();
}

LocalizationArg::LocalizationArg(const string& value, const string& plural_val, const int num)
    : stringVal(value), plural(plural_val)
{
    init();
    count = num;
}

LocalizationArg::LocalizationArg(const char* value)
{
    init();
    if (value != nullptr)
    {
        stringVal = value;
    }
}

LocalizationArg::LocalizationArg(const int value)
{
    init();
    intVal = value;
}

LocalizationArg::LocalizationArg(const long value)
{
    init();
    longVal = value;
}

LocalizationArg::LocalizationArg(const long long value)
{
    init();
    longLongVal = value;
}

LocalizationArg::LocalizationArg(const double value)
{
    init();
    doubleVal = value;
}

LocalizationArg::LocalizationArg(const long double value)
{
    init();
    longDoubleVal = value;
}

void init_localization(const string& lang)
{
    init_xlate(lang);
}

const string& get_localization_language()
{
    return get_xlate_language();
}

string localize(const vector<LocalizationArg>& args, const bool capitalize)
{
    if (args.empty())
    {
        return "";
    }

    bool success = false;

    // first argument is the format string
    LocalizationArg fmt_arg = args.at(0);

    // translate format string
    string fmt_xlated;
    if (fmt_arg.translate)
    {
        fmt_xlated = _localize_string("", fmt_arg);
        success = (fmt_xlated != fmt_arg.stringVal);
    }
    else
    {
        fmt_xlated = fmt_arg.stringVal;
    }

    if (args.size() == 1 || fmt_xlated.empty())
    {
        // We're done here
        return fmt_xlated;
    }

    // get arg types for original English string
    map<int, const type_info*> arg_types = _get_arg_types(fmt_arg.stringVal);

    // now tokenize the translated string
    vector<string> strings = _split_format(fmt_xlated);

    ostringstream ss;

    string context;
    int arg_count = 0;
    for (vector<string>::iterator it = strings.begin() ; it != strings.end(); ++it)
    {
        if (it->at(0) == '{' && it->length() > 1)
        {
            context = it->substr(1, it->length() - 2); // strip curly brackets
        }
        else if (it->length() > 1 && it->at(0) == '%' && it->at(1) != '%')
        {
            // this is a format specifier like %s, %d, etc.

            ++arg_count;
            int arg_id = _get_arg_id(*it);
            arg_id = (arg_id == 0 ? arg_count : arg_id);

            map<int, const type_info*>::iterator type_entry = arg_types.find(arg_id);

            // range check arg id
            if (type_entry == arg_types.end() || (size_t)arg_id >= args.size())
            {
                // argument id is out of range - just regurgitate the original string
                ss << *it;
            }
            else
            {
                const LocalizationArg& arg = args.at(arg_id);

                string fmt_spec = _remove_arg_id(*it);
                const type_info* type = _format_spec_to_type(fmt_spec);
                const type_info* expected_type = type_entry->second;

                if (expected_type == NULL || type == NULL || *type != *expected_type)
                {
                    // something's wrong - skip this arg
                }
                else if (*type == typeid(char*))
                {
                    // arg is string
                    string argx;
                    if (arg.translate)
                    {
                        if (!arg.stringVal.empty()
                            && arg.stringVal.find_first_not_of("!") == string::npos)
                        {
                            // Special handling for attack strength exclamation marks:
                            // Some languages (e.g. Spanish) may not simply append exclamation marks at the end,
                            // so we switch it around and make the sentence the argument for a format string containing the punctuation.
                            string excl_fmt = cxlate(context, "%s" + arg.stringVal);
                            string sentence = ss.str();
                            ss.str("");
                            ss << make_stringf(excl_fmt.c_str(), sentence.c_str());
                        }
                        else
                        {
                            argx = _localize_string(context, arg);
                            ss << make_stringf(fmt_spec.c_str(), argx.c_str());
                            if (argx != arg.stringVal)
                                success = true;
                        }
                    }
                    else
                    {
                        argx = arg.stringVal;
                        ss << make_stringf(fmt_spec.c_str(), argx.c_str());
                    }
                }
                else {
                    if (*type == typeid(long double))
                    {
                        ss << make_stringf(fmt_spec.c_str(), arg.longDoubleVal);
                    }
                    else if (*type == typeid(double))
                    {
                        ss << make_stringf(fmt_spec.c_str(), arg.doubleVal);
                    }
                    else if (*type == typeid(long long) || *type == typeid(unsigned long long))
                    {
                        ss << make_stringf(fmt_spec.c_str(), arg.longLongVal);
                    }
                    else if (*type == typeid(long) || *type == typeid(unsigned long))
                    {
                        ss << make_stringf(fmt_spec.c_str(), arg.longVal);
                    }
                    else if (*type == typeid(int) || *type == typeid(unsigned int))
                    {
                        ss << make_stringf(fmt_spec.c_str(), arg.intVal);
                    }
                }
            }
         }
        else
        {
            // plain string (but could have escapes)
            string str = *it;
            _resolve_escapes(str);
            ss << str;
        }
    }

    string result = ss.str();

    if (args.size() > 1 && fmt_arg.translate && !success)
    {
        // there may be a translation for the completed string
        result = localize(result);
    }

    if (capitalize)
    {
        result = uppercase_first(result);
    }
    return result;

}

string localize(const string& fmt_str, va_list argp, const bool capitalize)
{
    va_list args;
    va_copy(args, argp);

    vector<LocalizationArg> niceArgs;

    niceArgs.push_back(LocalizationArg(fmt_str));

    // get arg types for original English string
    map<int, const type_info*> arg_types = _get_arg_types(fmt_str);

    int last_arg_id = 0;
    map<int, const type_info*>::iterator it;
    for (it = arg_types.begin(); it != arg_types.end(); ++it)
    {
        const int arg_id = it->first;
        const type_info& arg_type = *(it->second);

        if (arg_id != last_arg_id + 1)
        {
            // something went wrong
            break;
        }

        if (arg_type == typeid(char*))
        {
            niceArgs.push_back(LocalizationArg(va_arg(args, char*)));
        }
        else if (arg_type == typeid(long double))
        {
            niceArgs.push_back(LocalizationArg(va_arg(args, long double)));
        }
        else if (arg_type == typeid(double))
        {
            niceArgs.push_back(LocalizationArg(va_arg(args, double)));
        }
        else if (arg_type == typeid(long long) || arg_type == typeid(unsigned long long))
        {
            niceArgs.push_back(LocalizationArg(va_arg(args, long long)));
        }
        else if (arg_type == typeid(long) || arg_type == typeid(unsigned long))
        {
            niceArgs.push_back(LocalizationArg(va_arg(args, long)));
        }
        else if (arg_type == typeid(int) || arg_type == typeid(unsigned int))
        {
            niceArgs.push_back(LocalizationArg(va_arg(args, int)));
        }
        else if (arg_type == typeid(ptrdiff_t))
        {
            va_arg(args, ptrdiff_t);
            niceArgs.push_back(LocalizationArg());
        }
        else if (arg_type == typeid(size_t))
        {
            va_arg(args, size_t);
            niceArgs.push_back(LocalizationArg());
        }
        else if (arg_type == typeid(intmax_t) || arg_type == typeid(uintmax_t))
        {
            va_arg(args, intmax_t);
            niceArgs.push_back(LocalizationArg());
        }
        else
        {
            va_arg(args, void*);
            niceArgs.push_back(LocalizationArg());
        }

        last_arg_id = arg_id;
    }

    va_end(args);

    return localize(niceArgs, capitalize);
}

string localize(const LocalizationArg& arg)
{
    vector<LocalizationArg> args;
    args.push_back(arg);
    return localize(args);
}

string localize(const LocalizationArg& arg1, const LocalizationArg& arg2)
{
    vector<LocalizationArg> args;
    args.push_back(arg1);
    args.push_back(arg2);
    return localize(args);
}

string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3)
{
    vector<LocalizationArg> args;
    args.push_back(arg1);
    args.push_back(arg2);
    args.push_back(arg3);
    return localize(args);
}

string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3,
                const LocalizationArg& arg4)
{
    vector<LocalizationArg> args;
    args.push_back(arg1);
    args.push_back(arg2);
    args.push_back(arg3);
    args.push_back(arg4);
    return localize(args);
}

/**
 * Get the localized equivalent of a single-character
 * (Mostly for prompt answers like Y/N)
 */
int localize_char(char ch)
{
    char en[2];
    en[0] = ch;
    en[2] = '\0';

    string loc = xlate(en);

    char32_t res;
    if (utf8towc(&res, loc.c_str()))
    {
        return (int)res;
    }
    else
    {
        // failed
        return (int)ch;
    }
}


