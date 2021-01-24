//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// compiler_specific_func.cpp


#include "compiler_specific_func.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if 0 //defined(_MSC_VER)
// Microsoft Visual C++ 6.0

// get compiler version string
tstring getCompilerVersionString()
{
    TCHAR buf[200];

    _sntprintf(buf, NUMBER_OF(buf),
               _T("Microsoft (R) 32-bit C/C++ Optimizing Compiler Version %d.%02d"),
               _MSC_VER / 100,
               _MSC_VER % 100
    );
    return tstring(buf);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#elif 0 //defined(__BORLANDC__)
// Borland C++ 5.5.1

// get compiler version string
tstring getCompilerVersionString()
{
    TCHAR buf[100];

    _sntprintf(buf, NUMBER_OF(buf), _T("Borland C++ %d.%d.%d"),
               __BORLANDC__ / 0x100,
               __BORLANDC__ / 0x10 % 0x10,
               __BORLANDC__ % 0x10
    );
    return tstring(buf);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#elif defined(__linux__) || defined(__APPLE__)
// Linux GCC

// get compiler version string
tstring getCompilerVersionString()
{
    TCHAR buf[200];

    _sntprintf(buf, NUMBER_OF(buf),
             #ifdef __clang__
               _T("clang")      // TODO:
             #else
               _T("GCC")        // TODO:
             #endif
    );
    return tstring(buf);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// unknown
#else
 #error "I don't know the details of this compiler... Plz hack."
#endif
