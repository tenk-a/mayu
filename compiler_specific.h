// ////////////////////////////////////////////////////////////////////////////
// compiler_specific.h


#ifndef __compiler_specific_h__
#define __compiler_specific_h__


// ////////////////////////////////////////////////////////////////////////////
// Microsoft Visual C++ 6.0

#if defined(_MSC_VER)

// C4061 enum 'identifier' is not handled by case label
// C4100 argument 'identifier' is not used
// C4132 const 'object' must be initialized
// C4701 local variable 'name' may be uninitialized
// C4706 condition is a result of a assign
// C4786 identifier is truncated into 255 chars (in debug information)
// C4552 'operator' : operator has no effect
#  pragma warning(disable : 4061 4100 4132 4552 4701 4706 4786)

#  define snprintf _snprintf
#  define setmode _setmode


// ////////////////////////////////////////////////////////////////////////////
// Borland C++ 5.5 

#elif defined(__BORLANDC__)

// W8004 'identifier' is assigned a value that is never used in function
// W8022 'identifier' hides virtual function 'function'
// W8027 Functions containing ... are not expanded inline
// W8030 Temporary used for parameter 'identifier'
//       in call to 'function' in function
// W8060 Possibly incorrect assignment in function
// W8070 Function should return a value in function
// W8084 Suggest parentheses to clarify precedence in function
#pragma warn -8004
#pragma warn -8022
#pragma warn -8027
#pragma warn -8030
#pragma warn -8060
#pragma warn -8070
#pragma warn -8084


// ////////////////////////////////////////////////////////////////////////////
// Cygwin 1.1 (gcc 2.95.2)

#elif defined(__CYGWIN__)
#  error "I don't know the details of this compiler... Plz hack."


// ////////////////////////////////////////////////////////////////////////////
// Watcom C++

#elif defined(__WATCOMC__)
#  error "I don't know the details of this compiler... Plz hack."


// ////////////////////////////////////////////////////////////////////////////
// unknown

#else
#  error "I don't know the details of this compiler... Plz hack."

#endif


#endif // __compiler_specific_h__
