#pragma once

#if 0 //defined(WIN32)
 #include <tchar.h>
#elif 1 //defined(__GNUC__)
 #if 0 //defined(UNICODE)
  typedef wchar_t       _TCHAR;
  #define _T(x)         L ## x
 #elif 0 //defined(_MBCS)
  typedef char          _TCHAR;
  #define USE_MBS
 #else //contain        _SBCS
  typedef char          _TCHAR;
  typedef _TCHAR        TCHAR;
  #define _tcsicmp      strcasecmp
  #define _T(x)         x
  #define _sntprintf    snprintf
  //#define USE_MBS
 #endif

// _T functions

// TODO:
#if 0 //def USE_MBS
#if 1
// UTF8 環境のみ対応ということにして、文字の2バイト目以降は0x80以上なので とりあえず falseで十分...
inline bool _ismbblead(unsigned char /*c*/)
{
    return false;
}
#else
// eucとsjisのみ対応 とりあえず、UTF-8とか wchar_t は後回し.
inline bool _ismbblead(unsigned char c)
{
    return    (0xFE >= c && c >= 0xA1) // euc
           || (0x9F >= c && c >= 0x81) // sjis
           || (0xEF >= c && c >= 0xE0) // sjis
    ;
}
#endif
#endif

 #define _istalpha(c)   isalpha(c)
 #define _istdigit(c)   isdigit(c)
 #define _istpunct(c)   ispunct(c)
 #define _istspace      isspace
 #define _tcschr        strchr
 #define _istgraph      isgraph
 #define _tcsnicmp      strncasecmp
 #define _istprint      isprint
 #define _tcstol        strtol
 #define _tfopen        fopen
 #define _tstati64      stat
 #if 0 //def USE_MBS
  #define _istlead      _ismbblead
 #endif
#endif
