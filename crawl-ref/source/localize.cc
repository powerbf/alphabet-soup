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
#include <typeinfo>
using namespace std;

#include "localize.h"
#include "xlate.h"
#include "stringutil.h"

union arg_t
{
    int i;
    long l;
    long long ll;
    intmax_t im;
    double d;
    long double ld;
    size_t sz;
    ptrdiff_t pd;
    char* s;
    int* pi;
    void* pv;
};


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


// localize a single string
static string _localize_string(const string& context, const string& value, const string& plural_val, const int count)
{
    if (plural_val.empty())
    {
        return cxlate(context, value);
    }
    else
    {
        string result = cnxlate(context, value, plural_val, count);
        result = make_stringf(result.c_str(), count);
        return result;
    }
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

    // first argument is the format string
    LocalizationArg fmt_arg = args.at(0);

    // translate format string
    string fmt_xlated;
    if (fmt_arg.translate)
    {
        fmt_xlated = _localize_string("", fmt_arg.stringVal, fmt_arg.plural, fmt_arg.count);
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

                string s = fmt_spec;
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
                        argx = _localize_string(context, arg.stringVal, arg.plural, arg.count);
                    }
                    else
                    {
                        argx = arg.stringVal;
                    }
                    s = make_stringf(fmt_spec.c_str(), argx.c_str());
                }
                else if (*type == typeid(long double))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.longDoubleVal);
                }
                else if (*type == typeid(double))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.doubleVal);
                }
                else if (*type == typeid(long long) || *type == typeid(unsigned long long))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.longLongVal);
                }
                else if (*type == typeid(long) || *type == typeid(unsigned long))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.longVal);
                }
                else if (*type == typeid(int) || *type == typeid(unsigned int))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.intVal);
                }
                ss << s;
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
    if (capitalize)
    {
        result = uppercase_first(result);
    }
    return result;

}

// convenience function using va_args (yuk!)
string localize(const string& fmt_str, ...)
{
    va_list args;
    va_start(args, fmt_str);
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

    return localize(niceArgs);
}

string localize(const LocalizationArg& arg, const bool capitalize)
{
    vector<LocalizationArg> args;
    args.push_back(arg);
    return localize(args, capitalize);
}

string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const bool capitalize)
{
    vector<LocalizationArg> args;
    args.push_back(arg1);
    args.push_back(arg2);
    return localize(args, capitalize);
}

string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3, const bool capitalize)
{
    vector<LocalizationArg> args;
    args.push_back(arg1);
    args.push_back(arg2);
    args.push_back(arg3);
    return localize(args, capitalize);
}

string localize(const LocalizationArg& arg1, const LocalizationArg& arg2, const LocalizationArg& arg3,
                const LocalizationArg& arg4, const bool capitalize)
{
    vector<LocalizationArg> args;
    args.push_back(arg1);
    args.push_back(arg2);
    args.push_back(arg3);
    args.push_back(arg4);
    return localize(args, capitalize);
}
