//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// compiler_specific.h


#ifndef _COMPILER_SPECIFIC_H
#define _COMPILER_SPECIFIC_H


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if 0 //defined(_MSC_VER)
// Microsoft Visual C++ 6.0

// C4061 enum 'identifier' is not handled by case label
// C4100 argument 'identifier' is not used
// C4132 const 'object' must be initialized
// C4552 'operator' : operator has no effect
// C4701 local variable 'name' may be uninitialized
// C4706 condition is a result of a assign
// C4786 identifier is truncated into 255 chars (in debug information)
 # pragma warning(disable : 4061 4100 4132 4552 4701 4706 4786)

 # define setmode _setmode
 # define for if (false) ; else for

 # define stati64_t _stati64


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#elif 0 //defined(__BORLANDC__)
// Borland C++ 5.5.1

// W8004 'identifier' is assigned a value that is never used in function
// W8022 'identifier' hides virtual function 'function'
// W8027 Functions containing ... are not expanded inline
// W8030 Temporary used for parameter 'identifier'
//       in call to 'function' in function
// W8060 Possibly incorrect assignment in function
// W8070 Function should return a value in function
// W8084 Suggest parentheses to clarify precedence in function
 # pragma warn -8004
 # pragma warn -8022
 # pragma warn -8027
 # pragma warn -8030
 # pragma warn -8060
 # pragma warn -8070
 # pragma warn -8084

 # ifdef UNICODE
extern wchar_t * *_wargv;
 # endif

 # ifdef _MBCS
  #  define _istcntrl iscntrl
 # endif

 # include <windows.h>
 # include <tchar.h>

extern "C"
{
int WINAPI _tWinMain(HINSTANCE i_hInstance, HINSTANCE i_hPrevInstance,
                     LPTSTR i_lpszCmdLine, int i_nCmdShow);
}

 # define stati64_t stati64


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#elif 0 //defined(__CYGWIN__)
// Cygwin 1.1 (gcc 2.95.2)
//#    error "I don't know the details of this compiler... Plz hack."
 # define stati64_t stat

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#elif 0 //defined(__WATCOMC__)
// Watcom C++
 # error "I don't know the details of this compiler... Plz hack."


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Linux GCC

#elif defined(__linux__)
 # define stati64_t stat


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Mac GCC

#elif defined(__APPLE__)
 # define stati64_t stat


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// unknown

#else
 # error "I don't know the details of this compiler... Plz hack."

#endif


#endif // _COMPILER_SPECIFIC_H
