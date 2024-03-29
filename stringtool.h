//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// stringtool.h

#ifndef _STRINGTOOL_H
#define _STRINGTOOL_H

// 現状windowやfunctionに未対応で正規表現使ってないので利用しないようにする定義.
// 一応 動くようだが影響わかんないのでとりあえず未定義.
//#define UNUSE_REGEX

#include "misc.h"
//#  include <tchar.h>
#include "_tchar.h"
#include <cctype>
#include <string>
#include <iosfwd>
#if !defined(UNUSE_REGEX)
#include <regex>
#endif
#include <stdio.h>      // for snprintf
#include <string.h>     // for stricmp

#if 0 //def __CYGWIN__
namespace std
{
    typedef basic_string<wchar_t>   wstring;    // !!
}
#endif
#if defined(__linux__) || defined(__APPLE__)
 # define _tgetenv(env) getenv(env)
#endif

/// string for generic international text
typedef std::basic_string<_TCHAR>           tstring;
/// istream for generic international text
typedef std::basic_istream<_TCHAR>          tistream;
/// ostream for generic international text
typedef std::basic_ostream<_TCHAR>          tostream;
/// streambuf for for generic international text
typedef std::basic_streambuf<_TCHAR>        tstreambuf;
/// stringstream for generic international text
typedef std::basic_stringstream<_TCHAR>     tstringstream;
/// ifstream for generic international text
typedef std::basic_ifstream<_TCHAR>         tifstream;

#if !defined(UNUSE_REGEX)
/// basic_regex for generic international text
//typedef boost::basic_regex<_TCHAR> tregex;
class tregex : public std::basic_regex<_TCHAR>
{
    typedef std::basic_regex<_TCHAR>        base_type;
public:
    tregex() {}
    tregex(tregex const &rhs) : base_type(rhs), m_pattern(rhs.m_pattern) {}
    tregex(_TCHAR const *pattern) : base_type(pattern), m_pattern(pattern) {}
    tregex(std::string const &pattern) : base_type(pattern), m_pattern(pattern) {}
    tregex & operator =(tregex const &rhs) { tregex(rhs).swap(*this); return *this; }
    tregex & operator =(std::string const &rhs) { tregex(rhs).swap(*this); return *this; }
    tregex & operator =(char const *rhs) { tregex(rhs).swap(*this); return *this; }

    void assign(std::string const &pattern, std::regex_constants::syntax_option_type f)
    {
        base_type::assign(pattern, f);
        m_pattern = pattern;
    }

    tstring const &str() const { return m_pattern; }

    void swap(tregex &rhs)
    {
        std::swap(*(base_type *) this, *(base_type *) &rhs);
        std::swap(m_pattern, rhs.m_pattern);
    }

private:
    tstring m_pattern;
};

/// match_results for generic international text
typedef std::match_results<tstring::const_iterator>  tcmatch;

#else
class tregex : public std::basic_string<_TCHAR>
{
    typedef std::basic_string<_TCHAR>   base_type;
public:
    tregex() {}
    tregex(tregex const &rhs) : base_type(rhs) {}
    tregex(_TCHAR const *pattern) : base_type(pattern) {}
    tregex(std::string const &pattern) : base_type(pattern) {}
    tregex & operator =(tregex const &rhs) { tregex(rhs).swap(*this); return *this; }
    tregex & operator =(std::string const &rhs) { tregex(rhs).swap(*this); return *this; }
    tregex & operator =(char const *rhs) { tregex(rhs).swap(*this); return *this; }
    void swap(tregex &rhs) { std::swap(*(base_type *) this, *(base_type *) &rhs); }
    tstring const &str() const { return *(base_type*)this; }
    void assign(tstring const &pattern) { *(base_type*)this = pattern; }
};
typedef tstring tcmatch;
#endif

/// string with custom stream output
class tstringq : public tstring
{
public:
    ///
    tstringq() { }
    ///
    tstringq(const tstringq &i_str) : tstring(i_str) { }
    ///
    tstringq(const tstring &i_str) : tstring(i_str) { }
    ///
    tstringq(const _TCHAR *i_str) : tstring(i_str) { }
    ///
    tstringq(const _TCHAR *i_str, size_t i_n) : tstring(i_str, i_n) { }
    ///
    tstringq(const _TCHAR *i_str, size_t i_pos, size_t i_n)
        : tstring(i_str, i_pos, i_n) { }
    ///
    tstringq(size_t i_n, _TCHAR i_c) : tstring(i_n, i_c) { }
};


/// stream output
extern tostream &operator <<(tostream &i_ost, const tstringq &i_data);


/// identical to tcmatch except for str()
class tcmatch_results : public tcmatch
{
public:
 #if !defined(UNUSE_REGEX)
    /** returns match result as tstring.
        match_results<const _TCHAR *>::operator[]() returns a instance of
        sub_mtch<const _TCHAR *>.  So, we convert sub_mtch<const _TCHAR *> to
        tstring.
     */
    tstring str(tcmatch::size_type i_n) const
    {
        return static_cast<tstring>(static_cast<const tcmatch *>(this)->operator [](i_n) );
    }
 #endif
};

/// interpret meta characters such as \n
tstring interpretMetaCharacters(const _TCHAR *i_str, size_t i_len,
                                const _TCHAR *i_quote = NULL,
                                bool i_doesUseRegexpBackReference = false);

/// add session id to i_str
tstring addSessionId(const _TCHAR *i_str);
/// copy
size_t  strlcpy(char *o_dest, const char *i_src, size_t i_destSize);
/// copy
size_t  mbslcpy(unsigned char *o_dest, const unsigned char *i_src, size_t i_destSize);
/// copy
size_t  wcslcpy(wchar_t *o_dest, const wchar_t *i_src, size_t i_destSize);
/// copy
inline size_t tcslcpy(char *o_dest, const char *i_src, size_t i_destSize) { return strlcpy(o_dest,i_src,i_destSize); }
/// copy
inline size_t tcslcpy(unsigned char *o_dest, const unsigned char *i_src, size_t i_destSize)
{
    return mbslcpy(o_dest, i_src, i_destSize);
}
/// copy
inline size_t tcslcpy(wchar_t *o_dest, const wchar_t *i_src, size_t i_destSize)
{
    return wcslcpy(o_dest, i_src, i_destSize);
}
#if 0 //def _MBCS // escape regexp special characters in MBCS trail bytes
std::string     guardRegexpFromMbcs(const char *i_str);
#endif
/// converter
std::wstring    to_wstring(const std::string &i_str);
/// converter
std::string     to_string(const std::wstring &i_str);
// convert wstring to UTF-8
std::string     to_UTF_8(const std::wstring &i_str);

/// case insensitive string
class tstringi : public tstring
{
public:
    ///
    tstringi() { }
    ///
    tstringi(const tstringi &i_str) : tstring(i_str) { }
    ///
    tstringi(const tstring &i_str) : tstring(i_str) { }
    ///
    tstringi(const _TCHAR *i_str) : tstring(i_str) { }
    ///
    tstringi(const _TCHAR *i_str, size_t i_n) : tstring(i_str, i_n) { }
    ///
    tstringi(const _TCHAR *i_str, size_t i_pos, size_t i_n)
        : tstring(i_str, i_pos, i_n) { }
    ///
    tstringi(size_t i_n, _TCHAR i_c) : tstring(i_n, i_c) { }
    ///
    int compare(const tstringi &i_str) const { return compare( i_str.c_str() ); }
    ///
    int compare(const tstring &i_str) const { return compare( i_str.c_str() ); }
    ///
    int compare(const _TCHAR *i_str) const { return _tcsicmp(c_str(), i_str); }
    ///
    tstring &getString() { return *this; }
    ///
    const tstring &getString() const { return *this; }
};

/// case insensitive string comparison
inline bool operator <(const tstringi &i_str1, const _TCHAR *i_str2)
{
    return i_str1.compare(i_str2) < 0;
}
/// case insensitive string comparison
inline bool operator <(const _TCHAR *i_str1, const tstringi &i_str2)
{
    return 0 < i_str2.compare(i_str1);
}
/// case insensitive string comparison
inline bool operator <(const tstringi &i_str1, const tstring &i_str2)
{
    return i_str1.compare(i_str2) < 0;
}
/// case insensitive string comparison
inline bool operator <(const tstring &i_str1, const tstringi &i_str2)
{
    return 0 < i_str2.compare(i_str1);
}
/// case insensitive string comparison
inline bool operator <(const tstringi &i_str1, const tstringi &i_str2)
{
    return i_str1.compare(i_str2) < 0;
}

/// case insensitive string comparison
inline bool operator ==(const _TCHAR *i_str1, const tstringi &i_str2)
{
    return i_str2.compare(i_str1) == 0;
}
/// case insensitive string comparison
inline bool operator ==(const tstringi &i_str1, const _TCHAR *i_str2)
{
    return i_str1.compare(i_str2) == 0;
}
/// case insensitive string comparison
inline bool operator ==(const tstring &i_str1, const tstringi &i_str2)
{
    return i_str2.compare(i_str1) == 0;
}
/// case insensitive string comparison
inline bool operator ==(const tstringi &i_str1, const tstring &i_str2)
{
    return i_str1.compare(i_str2) == 0;
}
/// case insensitive string comparison
inline bool operator ==(const tstringi &i_str1, const tstringi &i_str2)
{
    return i_str1.compare(i_str2) == 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// workarounds for Borland C++

/// case insensitive string comparison
inline bool operator !=(const _TCHAR *i_str1, const tstringi &i_str2)
{
    return i_str2.compare(i_str1) != 0;
}
/// case insensitive string comparison
inline bool operator !=(const tstringi &i_str1, const _TCHAR *i_str2)
{
    return i_str1.compare(i_str2) != 0;
}
/// case insensitive string comparison
inline bool operator !=(const tstring &i_str1, const tstringi &i_str2)
{
    return i_str2.compare(i_str1) != 0;
}
/// case insensitive string comparison
inline bool operator !=(const tstringi &i_str1, const tstring &i_str2)
{
    return i_str1.compare(i_str2) != 0;
}
/// case insensitive string comparison
inline bool operator !=(const tstringi &i_str1, const tstringi &i_str2)
{
    return i_str1.compare(i_str2) != 0;
}

/// stream output
extern tostream &operator <<(tostream &i_ost, const tregex &i_data);

/// get lower string
extern tstring              toLower(const tstring &i_str);

#endif // !_STRINGTOOL_H
