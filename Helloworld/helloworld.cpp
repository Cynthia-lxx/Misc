//Author:街角的猫_wjz
//Using C and C++ standard libraries
//Feel free to copy!!
// C
#ifndef _GLIBCXX_NO_ASSERT
#include <cassert>
#endif
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#if __cplusplus >= 201103L
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cwchar>
#include <cwctype>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#if __cplusplus >= 201103L
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#endif
/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

/* According to C99 standard (section 7.2) the assert
   macro shall be redefined each time assert.h gets
   included depending on the status of NDEBUG macro.  */
#undef assert

#ifndef __ASSERT_H_
#define __ASSERT_H_

#include <crtdefs.h>
#ifdef __cplusplus
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRT_TERMINATE_DEFINED
#define _CRT_TERMINATE_DEFINED
  void __cdecl __MINGW_NOTHROW exit(int _Code) __MINGW_ATTRIB_NORETURN;
  void __cdecl __MINGW_NOTHROW _exit(int _Code) __MINGW_ATTRIB_NORETURN;

#if !defined __NO_ISOCEXT /* extern stub in static libmingwex.a */
  /* C99 function name */
  void __cdecl _Exit(int) __MINGW_ATTRIB_NORETURN;
#ifndef __CRT__NO_INLINE
  __CRT_INLINE __MINGW_ATTRIB_NORETURN void  __cdecl _Exit(int status)
  {  _exit(status); }
#endif /* !__CRT__NO_INLINE */
#endif /* Not  __NO_ISOCEXT */

#pragma push_macro("abort")
#undef abort
  void __cdecl __declspec(noreturn) abort(void);
#pragma pop_macro("abort")

#endif /* _CRT_TERMINATE_DEFINED */

extern void __cdecl
_wassert(const wchar_t *_Message,const wchar_t *_File,unsigned _Line);
extern void __cdecl
_assert (const char *_Message, const char *_File, unsigned _Line);

#ifdef __cplusplus
}
#endif

#endif /* !defined (__ASSERT_H_) */

#if (defined _ISOC11_SOURCE \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L)) \
    && !defined (__cplusplus)
/* Static assertion.  Requires support in the compiler.  */
#undef static_assert
#define static_assert _Static_assert
#endif

#ifdef NDEBUG
#define assert(_Expression) ((void)0)
#else /* !defined (NDEBUG) */
#if defined(_UNICODE) || defined(UNICODE)
#define assert(_Expression) \
 (void) \
 ((!!(_Expression)) || \
  (_wassert(_CRT_WIDE(#_Expression),_CRT_WIDE(__FILE__),__LINE__),0))
#else /* not unicode */
#define assert(_Expression) \
 (void) \
 ((!!(_Expression)) || \
  (_assert(#_Expression,__FILE__,__LINE__),0))
#endif /* _UNICODE||UNICODE */
#endif /* !defined (NDEBUG) */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */

/* According to C99 standard (section 7.2) the assert
   macro shall be redefined each time assert.h gets
   included depending on the status of NDEBUG macro.  */
#undef assert

#ifndef __ASSERT_H_
#define __ASSERT_H_

#include <crtdefs.h>
#ifdef __cplusplus
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRT_TERMINATE_DEFINED
#define _CRT_TERMINATE_DEFINED
  void __cdecl __MINGW_NOTHROW exit(int _Code) __MINGW_ATTRIB_NORETURN;
  void __cdecl __MINGW_NOTHROW _exit(int _Code) __MINGW_ATTRIB_NORETURN;

#if !defined __NO_ISOCEXT /* extern stub in static libmingwex.a */
  /* C99 function name */
  void __cdecl _Exit(int) __MINGW_ATTRIB_NORETURN;
#ifndef __CRT__NO_INLINE
  __CRT_INLINE __MINGW_ATTRIB_NORETURN void  __cdecl _Exit(int status)
  {  _exit(status); }
#endif /* !__CRT__NO_INLINE */
#endif /* Not  __NO_ISOCEXT */

#pragma push_macro("abort")
#undef abort
  void __cdecl __declspec(noreturn) abort(void);
#pragma pop_macro("abort")

#endif /* _CRT_TERMINATE_DEFINED */

extern void __cdecl
_wassert(const wchar_t *_Message,const wchar_t *_File,unsigned _Line);
extern void __cdecl
_assert (const char *_Message, const char *_File, unsigned _Line);

#ifdef __cplusplus
}
#endif

#endif /* !defined (__ASSERT_H_) */

#if (defined _ISOC11_SOURCE \
     || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L)) \
    && !defined (__cplusplus)
/* Static assertion.  Requires support in the compiler.  */
#undef static_assert
#define static_assert _Static_assert
#endif

#ifdef NDEBUG
#define assert(_Expression) ((void)0)
#else /* !defined (NDEBUG) */
#if defined(_UNICODE) || defined(UNICODE)
#define assert(_Expression) \
 (void) \
 ((!!(_Expression)) || \
  (_wassert(_CRT_WIDE(#_Expression),_CRT_WIDE(__FILE__),__LINE__),0))
#else /* not unicode */
#define assert(_Expression) \
 (void) \
 ((!!(_Expression)) || \
  (_assert(#_Expression,__FILE__,__LINE__),0))
#endif /* _UNICODE||UNICODE */
#endif /* !defined (NDEBUG) */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_CTYPE
#define _INC_CTYPE

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifndef _CRT_CTYPEDATA_DEFINED
#define _CRT_CTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS

#ifndef __PCTYPE_FUNC
#define __PCTYPE_FUNC __pctype_func()
#ifdef _MSVCRT_
#define __pctype_func()	(_pctype)
#else
#define __pctype_func()	(* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#ifndef _pctype
#ifdef _MSVCRT_
  extern unsigned short *_pctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pctype);
#define _pctype (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#endif
#endif

#ifndef _CRT_WCTYPEDATA_DEFINED
#define _CRT_WCTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS
#ifndef _wctype
#ifdef _MSVCRT_
  extern unsigned short *_wctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_wctype);
#define _wctype (* __MINGW_IMP_SYMBOL(_wctype))
#endif
#endif
#ifdef _MSVCRT_
#define __pwctype_func() (_pwctype)
#ifndef _pwctype
  extern unsigned short *_pwctype;
#endif
#else
#define __pwctype_func() (* __MINGW_IMP_SYMBOL(_pwctype))
#ifndef _pwctype
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pwctype);
#define _pwctype (* __MINGW_IMP_SYMBOL(_pwctype))
#endif
#endif
#endif
#endif

  /* CRT stuff */
#if 1
  extern const unsigned char __newclmap[];
  extern const unsigned char __newcumap[];
  extern pthreadlocinfo __ptlocinfo;
  extern pthreadmbcinfo __ptmbcinfo;
  extern int __globallocalestatus;
  extern int __locale_changed;
  extern struct threadlocaleinfostruct __initiallocinfo;
  extern _locale_tstruct __initiallocalestructinfo;
  pthreadlocinfo __cdecl __updatetlocinfo(void);
  pthreadmbcinfo __cdecl __updatetmbcinfo(void);
#endif

#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8

#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80

#define _LEADBYTE 0x8000
#define _ALPHA (0x0100|_UPPER|_LOWER)

#ifndef _CTYPE_DEFINED
#define _CTYPE_DEFINED

  _CRTIMP int __cdecl _isctype(int _C,int _Type);
  _CRTIMP int __cdecl _isctype_l(int _C,int _Type,_locale_t _Locale);
  _CRTIMP int __cdecl isalpha(int _C);
  _CRTIMP int __cdecl _isalpha_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isupper(int _C);
  _CRTIMP int __cdecl _isupper_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl islower(int _C);
  _CRTIMP int __cdecl _islower_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isdigit(int _C);
  _CRTIMP int __cdecl _isdigit_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isxdigit(int _C);
  _CRTIMP int __cdecl _isxdigit_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isspace(int _C);
  _CRTIMP int __cdecl _isspace_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl ispunct(int _C);
  _CRTIMP int __cdecl _ispunct_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isalnum(int _C);
  _CRTIMP int __cdecl _isalnum_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isprint(int _C);
  _CRTIMP int __cdecl _isprint_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isgraph(int _C);
  _CRTIMP int __cdecl _isgraph_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl iscntrl(int _C);
  _CRTIMP int __cdecl _iscntrl_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl toupper(int _C);
  _CRTIMP int __cdecl tolower(int _C);
  _CRTIMP int __cdecl _tolower(int _C);
  _CRTIMP int __cdecl _tolower_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl _toupper(int _C);
  _CRTIMP int __cdecl _toupper_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl __isascii(int _C);
  _CRTIMP int __cdecl __toascii(int _C);
  _CRTIMP int __cdecl __iscsymf(int _C);
  _CRTIMP int __cdecl __iscsym(int _C);

#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES) || defined (__cplusplus)
int __cdecl isblank(int _C);
#endif
#endif

#ifndef _WCTYPE_DEFINED
#define _WCTYPE_DEFINED

  int __cdecl iswalpha(wint_t _C);
  _CRTIMP int __cdecl _iswalpha_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswupper(wint_t _C);
  _CRTIMP int __cdecl _iswupper_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswlower(wint_t _C);
  _CRTIMP int __cdecl _iswlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswdigit(wint_t _C);
  _CRTIMP int __cdecl _iswdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswxdigit(wint_t _C);
  _CRTIMP int __cdecl _iswxdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswspace(wint_t _C);
  _CRTIMP int __cdecl _iswspace_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswpunct(wint_t _C);
  _CRTIMP int __cdecl _iswpunct_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswalnum(wint_t _C);
  _CRTIMP int __cdecl _iswalnum_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswprint(wint_t _C);
  _CRTIMP int __cdecl _iswprint_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswgraph(wint_t _C);
  _CRTIMP int __cdecl _iswgraph_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswcntrl(wint_t _C);
  _CRTIMP int __cdecl _iswcntrl_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswascii(wint_t _C);
  int __cdecl isleadbyte(int _C);
  _CRTIMP int __cdecl _isleadbyte_l(int _C,_locale_t _Locale);
  wint_t __cdecl towupper(wint_t _C);
  _CRTIMP wint_t __cdecl _towupper_l(wint_t _C,_locale_t _Locale);
  wint_t __cdecl towlower(wint_t _C);
  _CRTIMP wint_t __cdecl _towlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswctype(wint_t _C,wctype_t _Type);
  _CRTIMP int __cdecl _iswctype_l(wint_t _C,wctype_t _Type,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsymf(wint_t _C);
  _CRTIMP int __cdecl _iswcsymf_l(wint_t _C,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsym(wint_t _C);
  _CRTIMP int __cdecl _iswcsym_l(wint_t _C,_locale_t _Locale);
  int __cdecl is_wctype(wint_t _C,wctype_t _Type);

#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES)
int __cdecl iswblank(wint_t _C);
#endif
#endif

#ifndef _CTYPE_DISABLE_MACROS

#ifndef MB_CUR_MAX
#define MB_CUR_MAX ___mb_cur_max_func()
#ifndef __mb_cur_max
#ifdef _MSVCRT_
  extern int __mb_cur_max;
#define __mb_cur_max	__mb_cur_max
#else
  extern int * __MINGW_IMP_SYMBOL(__mb_cur_max);
#define __mb_cur_max	(* __MINGW_IMP_SYMBOL(__mb_cur_max))
#endif
#endif
#define ___mb_cur_max_func() (__mb_cur_max)
#endif

#define __chvalidchk(a,b) (__PCTYPE_FUNC[(a)] & (b))
#define _chvalidchk_l(_Char,_Flag,_Locale) (!_Locale ? __chvalidchk(_Char,_Flag) : ((_locale_t)_Locale)->locinfo->pctype[_Char] & (_Flag))
#define _ischartype_l(_Char,_Flag,_Locale) (((_Locale)!=NULL && (((_locale_t)(_Locale))->locinfo->mb_cur_max) > 1) ? _isctype_l(_Char,(_Flag),_Locale) : _chvalidchk_l(_Char,_Flag,_Locale))
#define _isalpha_l(_Char,_Locale) _ischartype_l(_Char,_ALPHA,_Locale)
#define _isupper_l(_Char,_Locale) _ischartype_l(_Char,_UPPER,_Locale)
#define _islower_l(_Char,_Locale) _ischartype_l(_Char,_LOWER,_Locale)
#define _isdigit_l(_Char,_Locale) _ischartype_l(_Char,_DIGIT,_Locale)
#define _isxdigit_l(_Char,_Locale) _ischartype_l(_Char,_HEX,_Locale)
#define _isspace_l(_Char,_Locale) _ischartype_l(_Char,_SPACE,_Locale)
#define _ispunct_l(_Char,_Locale) _ischartype_l(_Char,_PUNCT,_Locale)
#define _isalnum_l(_Char,_Locale) _ischartype_l(_Char,_ALPHA|_DIGIT,_Locale)
#define _isprint_l(_Char,_Locale) _ischartype_l(_Char,_BLANK|_PUNCT|_ALPHA|_DIGIT,_Locale)
#define _isgraph_l(_Char,_Locale) _ischartype_l(_Char,_PUNCT|_ALPHA|_DIGIT,_Locale)
#define _iscntrl_l(_Char,_Locale) _ischartype_l(_Char,_CONTROL,_Locale)
#define _tolower(_Char) ((_Char)-'A'+'a')
#define _toupper(_Char) ((_Char)-'a'+'A')
#define __isascii(_Char) ((unsigned)(_Char) < 0x80)
#define __toascii(_Char) ((_Char) & 0x7f)

#ifndef _WCTYPE_INLINE_DEFINED
#define _WCTYPE_INLINE_DEFINED

#undef _CRT_WCTYPE_NOINLINE
#ifndef __cplusplus
#define iswalpha(_c) (iswctype(_c,_ALPHA))
#define iswupper(_c) (iswctype(_c,_UPPER))
#define iswlower(_c) (iswctype(_c,_LOWER))
#define iswdigit(_c) (iswctype(_c,_DIGIT))
#define iswxdigit(_c) (iswctype(_c,_HEX))
#define iswspace(_c) (iswctype(_c,_SPACE))
#define iswpunct(_c) (iswctype(_c,_PUNCT))
#define iswalnum(_c) (iswctype(_c,_ALPHA|_DIGIT))
#define iswprint(_c) (iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT))
#define iswgraph(_c) (iswctype(_c,_PUNCT|_ALPHA|_DIGIT))
#define iswcntrl(_c) (iswctype(_c,_CONTROL))
#define iswascii(_c) ((unsigned)(_c) < 0x80)
#define _iswalpha_l(_c,_p) (_iswctype_l(_c,_ALPHA,_p))
#define _iswupper_l(_c,_p) (_iswctype_l(_c,_UPPER,_p))
#define _iswlower_l(_c,_p) (_iswctype_l(_c,_LOWER,_p))
#define _iswdigit_l(_c,_p) (_iswctype_l(_c,_DIGIT,_p))
#define _iswxdigit_l(_c,_p) (_iswctype_l(_c,_HEX,_p))
#define _iswspace_l(_c,_p) (_iswctype_l(_c,_SPACE,_p))
#define _iswpunct_l(_c,_p) (_iswctype_l(_c,_PUNCT,_p))
#define _iswalnum_l(_c,_p) (_iswctype_l(_c,_ALPHA|_DIGIT,_p))
#define _iswprint_l(_c,_p) (_iswctype_l(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswgraph_l(_c,_p) (_iswctype_l(_c,_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswcntrl_l(_c,_p) (_iswctype_l(_c,_CONTROL,_p))
#endif
#endif

#define __iscsymf(_c) (isalpha(_c) || ((_c)=='_'))
#define __iscsym(_c) (isalnum(_c) || ((_c)=='_'))
#define __iswcsymf(_c) (iswalpha(_c) || ((_c)=='_'))
#define __iswcsym(_c) (iswalnum(_c) || ((_c)=='_'))
#define _iscsymf_l(_c,_p) (_isalpha_l(_c,_p) || ((_c)=='_'))
#define _iscsym_l(_c,_p) (_isalnum_l(_c,_p) || ((_c)=='_'))
#define _iswcsymf_l(_c,_p) (_iswalpha_l(_c,_p) || ((_c)=='_'))
#define _iswcsym_l(_c,_p) (_iswalnum_l(_c,_p) || ((_c)=='_'))
#endif

#ifndef	NO_OLDNAMES
#ifndef _CTYPE_DEFINED
  int __cdecl isascii(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl toascii(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl iscsymf(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl iscsym(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#else
#define isascii __isascii
#define toascii __toascii
#define iscsymf __iscsymf
#define iscsym __iscsym
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_CTYPE
#define _INC_CTYPE

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifndef _CRT_CTYPEDATA_DEFINED
#define _CRT_CTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS

#ifndef __PCTYPE_FUNC
#define __PCTYPE_FUNC __pctype_func()
#ifdef _MSVCRT_
#define __pctype_func()	(_pctype)
#else
#define __pctype_func()	(* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#ifndef _pctype
#ifdef _MSVCRT_
  extern unsigned short *_pctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pctype);
#define _pctype (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#endif
#endif

#ifndef _CRT_WCTYPEDATA_DEFINED
#define _CRT_WCTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS
#ifndef _wctype
#ifdef _MSVCRT_
  extern unsigned short *_wctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_wctype);
#define _wctype (* __MINGW_IMP_SYMBOL(_wctype))
#endif
#endif
#ifdef _MSVCRT_
#define __pwctype_func() (_pwctype)
#ifndef _pwctype
  extern unsigned short *_pwctype;
#endif
#else
#define __pwctype_func() (* __MINGW_IMP_SYMBOL(_pwctype))
#ifndef _pwctype
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pwctype);
#define _pwctype (* __MINGW_IMP_SYMBOL(_pwctype))
#endif
#endif
#endif
#endif

  /* CRT stuff */
#if 1
  extern const unsigned char __newclmap[];
  extern const unsigned char __newcumap[];
  extern pthreadlocinfo __ptlocinfo;
  extern pthreadmbcinfo __ptmbcinfo;
  extern int __globallocalestatus;
  extern int __locale_changed;
  extern struct threadlocaleinfostruct __initiallocinfo;
  extern _locale_tstruct __initiallocalestructinfo;
  pthreadlocinfo __cdecl __updatetlocinfo(void);
  pthreadmbcinfo __cdecl __updatetmbcinfo(void);
#endif

#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8

#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80

#define _LEADBYTE 0x8000
#define _ALPHA (0x0100|_UPPER|_LOWER)

#ifndef _CTYPE_DEFINED
#define _CTYPE_DEFINED

  _CRTIMP int __cdecl _isctype(int _C,int _Type);
  _CRTIMP int __cdecl _isctype_l(int _C,int _Type,_locale_t _Locale);
  _CRTIMP int __cdecl isalpha(int _C);
  _CRTIMP int __cdecl _isalpha_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isupper(int _C);
  _CRTIMP int __cdecl _isupper_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl islower(int _C);
  _CRTIMP int __cdecl _islower_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isdigit(int _C);
  _CRTIMP int __cdecl _isdigit_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isxdigit(int _C);
  _CRTIMP int __cdecl _isxdigit_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isspace(int _C);
  _CRTIMP int __cdecl _isspace_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl ispunct(int _C);
  _CRTIMP int __cdecl _ispunct_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isalnum(int _C);
  _CRTIMP int __cdecl _isalnum_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isprint(int _C);
  _CRTIMP int __cdecl _isprint_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl isgraph(int _C);
  _CRTIMP int __cdecl _isgraph_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl iscntrl(int _C);
  _CRTIMP int __cdecl _iscntrl_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl toupper(int _C);
  _CRTIMP int __cdecl tolower(int _C);
  _CRTIMP int __cdecl _tolower(int _C);
  _CRTIMP int __cdecl _tolower_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl _toupper(int _C);
  _CRTIMP int __cdecl _toupper_l(int _C,_locale_t _Locale);
  _CRTIMP int __cdecl __isascii(int _C);
  _CRTIMP int __cdecl __toascii(int _C);
  _CRTIMP int __cdecl __iscsymf(int _C);
  _CRTIMP int __cdecl __iscsym(int _C);

#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES) || defined (__cplusplus)
int __cdecl isblank(int _C);
#endif
#endif

#ifndef _WCTYPE_DEFINED
#define _WCTYPE_DEFINED

  int __cdecl iswalpha(wint_t _C);
  _CRTIMP int __cdecl _iswalpha_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswupper(wint_t _C);
  _CRTIMP int __cdecl _iswupper_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswlower(wint_t _C);
  _CRTIMP int __cdecl _iswlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswdigit(wint_t _C);
  _CRTIMP int __cdecl _iswdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswxdigit(wint_t _C);
  _CRTIMP int __cdecl _iswxdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswspace(wint_t _C);
  _CRTIMP int __cdecl _iswspace_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswpunct(wint_t _C);
  _CRTIMP int __cdecl _iswpunct_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswalnum(wint_t _C);
  _CRTIMP int __cdecl _iswalnum_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswprint(wint_t _C);
  _CRTIMP int __cdecl _iswprint_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswgraph(wint_t _C);
  _CRTIMP int __cdecl _iswgraph_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswcntrl(wint_t _C);
  _CRTIMP int __cdecl _iswcntrl_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswascii(wint_t _C);
  int __cdecl isleadbyte(int _C);
  _CRTIMP int __cdecl _isleadbyte_l(int _C,_locale_t _Locale);
  wint_t __cdecl towupper(wint_t _C);
  _CRTIMP wint_t __cdecl _towupper_l(wint_t _C,_locale_t _Locale);
  wint_t __cdecl towlower(wint_t _C);
  _CRTIMP wint_t __cdecl _towlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswctype(wint_t _C,wctype_t _Type);
  _CRTIMP int __cdecl _iswctype_l(wint_t _C,wctype_t _Type,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsymf(wint_t _C);
  _CRTIMP int __cdecl _iswcsymf_l(wint_t _C,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsym(wint_t _C);
  _CRTIMP int __cdecl _iswcsym_l(wint_t _C,_locale_t _Locale);
  int __cdecl is_wctype(wint_t _C,wctype_t _Type);

#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES)
int __cdecl iswblank(wint_t _C);
#endif
#endif

#ifndef _CTYPE_DISABLE_MACROS

#ifndef MB_CUR_MAX
#define MB_CUR_MAX ___mb_cur_max_func()
#ifndef __mb_cur_max
#ifdef _MSVCRT_
  extern int __mb_cur_max;
#define __mb_cur_max	__mb_cur_max
#else
  extern int * __MINGW_IMP_SYMBOL(__mb_cur_max);
#define __mb_cur_max	(* __MINGW_IMP_SYMBOL(__mb_cur_max))
#endif
#endif
#define ___mb_cur_max_func() (__mb_cur_max)
#endif

#define __chvalidchk(a,b) (__PCTYPE_FUNC[(a)] & (b))
#define _chvalidchk_l(_Char,_Flag,_Locale) (!_Locale ? __chvalidchk(_Char,_Flag) : ((_locale_t)_Locale)->locinfo->pctype[_Char] & (_Flag))
#define _ischartype_l(_Char,_Flag,_Locale) (((_Locale)!=NULL && (((_locale_t)(_Locale))->locinfo->mb_cur_max) > 1) ? _isctype_l(_Char,(_Flag),_Locale) : _chvalidchk_l(_Char,_Flag,_Locale))
#define _isalpha_l(_Char,_Locale) _ischartype_l(_Char,_ALPHA,_Locale)
#define _isupper_l(_Char,_Locale) _ischartype_l(_Char,_UPPER,_Locale)
#define _islower_l(_Char,_Locale) _ischartype_l(_Char,_LOWER,_Locale)
#define _isdigit_l(_Char,_Locale) _ischartype_l(_Char,_DIGIT,_Locale)
#define _isxdigit_l(_Char,_Locale) _ischartype_l(_Char,_HEX,_Locale)
#define _isspace_l(_Char,_Locale) _ischartype_l(_Char,_SPACE,_Locale)
#define _ispunct_l(_Char,_Locale) _ischartype_l(_Char,_PUNCT,_Locale)
#define _isalnum_l(_Char,_Locale) _ischartype_l(_Char,_ALPHA|_DIGIT,_Locale)
#define _isprint_l(_Char,_Locale) _ischartype_l(_Char,_BLANK|_PUNCT|_ALPHA|_DIGIT,_Locale)
#define _isgraph_l(_Char,_Locale) _ischartype_l(_Char,_PUNCT|_ALPHA|_DIGIT,_Locale)
#define _iscntrl_l(_Char,_Locale) _ischartype_l(_Char,_CONTROL,_Locale)
#define _tolower(_Char) ((_Char)-'A'+'a')
#define _toupper(_Char) ((_Char)-'a'+'A')
#define __isascii(_Char) ((unsigned)(_Char) < 0x80)
#define __toascii(_Char) ((_Char) & 0x7f)

#ifndef _WCTYPE_INLINE_DEFINED
#define _WCTYPE_INLINE_DEFINED

#undef _CRT_WCTYPE_NOINLINE
#ifndef __cplusplus
#define iswalpha(_c) (iswctype(_c,_ALPHA))
#define iswupper(_c) (iswctype(_c,_UPPER))
#define iswlower(_c) (iswctype(_c,_LOWER))
#define iswdigit(_c) (iswctype(_c,_DIGIT))
#define iswxdigit(_c) (iswctype(_c,_HEX))
#define iswspace(_c) (iswctype(_c,_SPACE))
#define iswpunct(_c) (iswctype(_c,_PUNCT))
#define iswalnum(_c) (iswctype(_c,_ALPHA|_DIGIT))
#define iswprint(_c) (iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT))
#define iswgraph(_c) (iswctype(_c,_PUNCT|_ALPHA|_DIGIT))
#define iswcntrl(_c) (iswctype(_c,_CONTROL))
#define iswascii(_c) ((unsigned)(_c) < 0x80)
#define _iswalpha_l(_c,_p) (_iswctype_l(_c,_ALPHA,_p))
#define _iswupper_l(_c,_p) (_iswctype_l(_c,_UPPER,_p))
#define _iswlower_l(_c,_p) (_iswctype_l(_c,_LOWER,_p))
#define _iswdigit_l(_c,_p) (_iswctype_l(_c,_DIGIT,_p))
#define _iswxdigit_l(_c,_p) (_iswctype_l(_c,_HEX,_p))
#define _iswspace_l(_c,_p) (_iswctype_l(_c,_SPACE,_p))
#define _iswpunct_l(_c,_p) (_iswctype_l(_c,_PUNCT,_p))
#define _iswalnum_l(_c,_p) (_iswctype_l(_c,_ALPHA|_DIGIT,_p))
#define _iswprint_l(_c,_p) (_iswctype_l(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswgraph_l(_c,_p) (_iswctype_l(_c,_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswcntrl_l(_c,_p) (_iswctype_l(_c,_CONTROL,_p))
#endif
#endif

#define __iscsymf(_c) (isalpha(_c) || ((_c)=='_'))
#define __iscsym(_c) (isalnum(_c) || ((_c)=='_'))
#define __iswcsymf(_c) (iswalpha(_c) || ((_c)=='_'))
#define __iswcsym(_c) (iswalnum(_c) || ((_c)=='_'))
#define _iscsymf_l(_c,_p) (_isalpha_l(_c,_p) || ((_c)=='_'))
#define _iscsym_l(_c,_p) (_isalnum_l(_c,_p) || ((_c)=='_'))
#define _iswcsymf_l(_c,_p) (_iswalpha_l(_c,_p) || ((_c)=='_'))
#define _iswcsym_l(_c,_p) (_iswalnum_l(_c,_p) || ((_c)=='_'))
#endif

#ifndef	NO_OLDNAMES
#ifndef _CTYPE_DEFINED
  int __cdecl isascii(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl toascii(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl iscsymf(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl iscsym(int _C) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#else
#define isascii __isascii
#define toascii __toascii
#define iscsymf __iscsymf
#define iscsym __iscsym
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_ERRNO
#define _INC_ERRNO

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRT_ERRNO_DEFINED
#define _CRT_ERRNO_DEFINED
_CRTIMP extern int *__cdecl _errno(void);
#define errno (*_errno())

errno_t __cdecl _set_errno(int _Value);
errno_t __cdecl _get_errno(int *_Value);
#endif /* _CRT_ERRNO_DEFINED */

#define EPERM 1
#define ENOENT 2
#define ENOFILE ENOENT
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define EDEADLK 36
#define ENAMETOOLONG 38
#define ENOLCK 39
#define ENOSYS 40
#define ENOTEMPTY 41

#ifndef RC_INVOKED
#if !defined(_SECURECRT_ERRCODE_VALUES_DEFINED)
#define _SECURECRT_ERRCODE_VALUES_DEFINED
#define EINVAL 22
#define ERANGE 34
#define EILSEQ 42
#define STRUNCATE 80
#endif
#endif

#define EDEADLOCK EDEADLK

/* Posix thread extensions.  */

#ifndef ENOTSUP
#define ENOTSUP         129
#endif

/* Extension defined as by report VC 10+ defines error-numbers.  */

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT 102
#endif

#ifndef EADDRINUSE
#define EADDRINUSE 100
#endif

#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL 101
#endif

#ifndef EISCONN
#define EISCONN 113
#endif

#ifndef ENOBUFS
#define ENOBUFS 119
#endif

#ifndef ECONNABORTED
#define ECONNABORTED 106
#endif

#ifndef EALREADY
#define EALREADY 103
#endif

#ifndef ECONNREFUSED
#define ECONNREFUSED 107
#endif

#ifndef ECONNRESET
#define ECONNRESET 108
#endif

#ifndef EDESTADDRREQ
#define EDESTADDRREQ 109
#endif

#ifndef EHOSTUNREACH
#define EHOSTUNREACH 110
#endif

#ifndef EMSGSIZE
#define EMSGSIZE 115
#endif

#ifndef ENETDOWN
#define ENETDOWN 116
#endif

#ifndef ENETRESET
#define ENETRESET 117
#endif

#ifndef ENETUNREACH
#define ENETUNREACH 118
#endif

#ifndef ENOPROTOOPT
#define ENOPROTOOPT 123
#endif

#ifndef ENOTSOCK
#define ENOTSOCK 128
#endif

#ifndef ENOTCONN
#define ENOTCONN 126
#endif

#ifndef ECANCELED
#define ECANCELED 105
#endif

#ifndef EINPROGRESS
#define EINPROGRESS 112
#endif

#ifndef EOPNOTSUPP
#define EOPNOTSUPP 130
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK 140
#endif

#ifndef EOWNERDEAD
#define EOWNERDEAD 133
#endif

#ifndef EPROTO
#define EPROTO 134
#endif

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT 135
#endif

/* Defined as WSAETIMEDOUT.  */
#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif

#ifndef ELOOP
#define ELOOP 114
#endif

#ifndef EPROTOTYPE
#define EPROTOTYPE 136
#endif

#ifndef EOVERFLOW
#define EOVERFLOW 132
#endif

#ifdef __cplusplus
}
#endif
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_ERRNO
#define _INC_ERRNO

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRT_ERRNO_DEFINED
#define _CRT_ERRNO_DEFINED
_CRTIMP extern int *__cdecl _errno(void);
#define errno (*_errno())

errno_t __cdecl _set_errno(int _Value);
errno_t __cdecl _get_errno(int *_Value);
#endif /* _CRT_ERRNO_DEFINED */

#define EPERM 1
#define ENOENT 2
#define ENOFILE ENOENT
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define EDEADLK 36
#define ENAMETOOLONG 38
#define ENOLCK 39
#define ENOSYS 40
#define ENOTEMPTY 41

#ifndef RC_INVOKED
#if !defined(_SECURECRT_ERRCODE_VALUES_DEFINED)
#define _SECURECRT_ERRCODE_VALUES_DEFINED
#define EINVAL 22
#define ERANGE 34
#define EILSEQ 42
#define STRUNCATE 80
#endif
#endif

#define EDEADLOCK EDEADLK

/* Posix thread extensions.  */

#ifndef ENOTSUP
#define ENOTSUP         129
#endif

/* Extension defined as by report VC 10+ defines error-numbers.  */

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT 102
#endif

#ifndef EADDRINUSE
#define EADDRINUSE 100
#endif

#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL 101
#endif

#ifndef EISCONN
#define EISCONN 113
#endif

#ifndef ENOBUFS
#define ENOBUFS 119
#endif

#ifndef ECONNABORTED
#define ECONNABORTED 106
#endif

#ifndef EALREADY
#define EALREADY 103
#endif

#ifndef ECONNREFUSED
#define ECONNREFUSED 107
#endif

#ifndef ECONNRESET
#define ECONNRESET 108
#endif

#ifndef EDESTADDRREQ
#define EDESTADDRREQ 109
#endif

#ifndef EHOSTUNREACH
#define EHOSTUNREACH 110
#endif

#ifndef EMSGSIZE
#define EMSGSIZE 115
#endif

#ifndef ENETDOWN
#define ENETDOWN 116
#endif

#ifndef ENETRESET
#define ENETRESET 117
#endif

#ifndef ENETUNREACH
#define ENETUNREACH 118
#endif

#ifndef ENOPROTOOPT
#define ENOPROTOOPT 123
#endif

#ifndef ENOTSOCK
#define ENOTSOCK 128
#endif

#ifndef ENOTCONN
#define ENOTCONN 126
#endif

#ifndef ECANCELED
#define ECANCELED 105
#endif

#ifndef EINPROGRESS
#define EINPROGRESS 112
#endif

#ifndef EOPNOTSUPP
#define EOPNOTSUPP 130
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK 140
#endif

#ifndef EOWNERDEAD
#define EOWNERDEAD 133
#endif

#ifndef EPROTO
#define EPROTO 134
#endif

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT 135
#endif

/* Defined as WSAETIMEDOUT.  */
#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif

#ifndef ELOOP
#define ELOOP 114
#endif

#ifndef EPROTOTYPE
#define EPROTOTYPE 136
#endif

#ifndef EOVERFLOW
#define EOVERFLOW 132
#endif

#ifdef __cplusplus
}
#endif
#endif

/*
 * float.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within the package.
 *
 * Constants related to floating point arithmetic.
 *
 * Also included here are some non-ANSI bits for accessing the floating
 * point controller.
 *
 */

#if (defined (__GNUC__) && defined (__GNUC_MINOR__)) \
    || (defined(__clang__) && defined(__clang_major__))
#if (__GNUC__ < 4  || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)) \
    || (__clang_major__ >=3)
#if !defined(_FLOAT_H___) && !defined(__FLOAT_H)
#include_next <float.h>
#endif
#elif !defined (_FLOAT_H___)
#if (__GNUC__ < 4)
#error Corrupt install of gcc-s internal headers, or search order was changed.
#else
	/* #include_next <float_ginclude.h> */
	
   	/* Number of decimal digits, q, such that any floating-point number with q
   	   decimal digits can be rounded into a floating-point number with p radix b
	   digits and back again without change to the q decimal digits,

	   p * log10(b)			if b is a power of 10
	   floor((p - 1) * log10(b))	otherwise
	*/
	#undef FLT_DIG
	#undef DBL_DIG
	#undef LDBL_DIG
	#define FLT_DIG		__FLT_DIG__
	#define DBL_DIG		__DBL_DIG__
	#define LDBL_DIG	__LDBL_DIG__
	
	
	/* Maximum representable finite floating-point number,

	   (1 - b**-p) * b**emax
	*/
	#undef FLT_MAX
	#undef DBL_MAX
	#undef LDBL_MAX
	#define FLT_MAX		__FLT_MAX__
	#define DBL_MAX		__DBL_MAX__
	#define LDBL_MAX	__LDBL_MAX__
	
	
	/* Minimum normalized positive floating-point number, b**(emin - 1).  */
	#undef FLT_MIN
	#undef DBL_MIN
	#undef LDBL_MIN
	#define FLT_MIN		__FLT_MIN__
	#define DBL_MIN		__DBL_MIN__
	#define LDBL_MIN	__LDBL_MIN__
	
	/* Needed for libjava building - Victor K. */

	/* Radix of exponent representation, b. */
    #undef FLT_RADIX
    #define FLT_RADIX	__FLT_RADIX__
    
    /* Minimum int x such that FLT_RADIX**(x-1) is a normalized float, emin */
	#undef FLT_MIN_EXP
	#undef DBL_MIN_EXP
	#undef LDBL_MIN_EXP
	#define FLT_MIN_EXP	__FLT_MIN_EXP__
	#define DBL_MIN_EXP	__DBL_MIN_EXP__
	#define LDBL_MIN_EXP	__LDBL_MIN_EXP__

	/* Minimum negative integer such that 10 raised to that power is in the
   	range of normalized floating-point numbers,

	ceil(log10(b) * (emin - 1))
	*/
	#undef FLT_MIN_10_EXP
	#undef DBL_MIN_10_EXP
	#undef LDBL_MIN_10_EXP
	#define FLT_MIN_10_EXP	__FLT_MIN_10_EXP__
	#define DBL_MIN_10_EXP	__DBL_MIN_10_EXP__
	#define LDBL_MIN_10_EXP	__LDBL_MIN_10_EXP__

	/* Maximum int x such that FLT_RADIX**(x-1) is a representable float, emax.  */
	#undef FLT_MAX_EXP
	#undef DBL_MAX_EXP
	#undef LDBL_MAX_EXP
	#define FLT_MAX_EXP	__FLT_MAX_EXP__
	#define DBL_MAX_EXP	__DBL_MAX_EXP__
	#define LDBL_MAX_EXP	__LDBL_MAX_EXP__

	/* Maximum integer such that 10 raised to that power is in the range of
   	representable finite floating-point numbers,

	floor(log10((1 - b**-p) * b**emax))
	*/
	#undef FLT_MAX_10_EXP
	#undef DBL_MAX_10_EXP
	#undef LDBL_MAX_10_EXP
	#define FLT_MAX_10_EXP	__FLT_MAX_10_EXP__
	#define DBL_MAX_10_EXP	__DBL_MAX_10_EXP__
	#define LDBL_MAX_10_EXP	__LDBL_MAX_10_EXP__

	/* Addition rounds to 0: zero, 1: nearest, 2: +inf, 3: -inf, -1: unknown.  */
	/* ??? This is supposed to change with calls to fesetround in <fenv.h>.  */
	#undef FLT_ROUNDS
	#define FLT_ROUNDS 1
    
	#define _FLOAT_H___
#endif
#endif
#endif

#ifndef _MINGW_FLOAT_H_
#define _MINGW_FLOAT_H_

/* All the headers include this file. */
#include <crtdefs.h>

/*
 * Functions and definitions for controlling the FPU.
 */
#ifndef	__STRICT_ANSI__

/* TODO: These constants are only valid for x86 machines */

/* Control word masks for unMask */
#define	_MCW_DN  	0x03000000  	/* Denormal control */
#define	_MCW_EM		0x0008001F	/* Error masks */
#define	_MCW_IC		0x00040000	/* Infinity */
#define	_MCW_RC		0x00000300	/* Rounding */
#define	_MCW_PC		0x00030000	/* Precision */

/* Number of base-FLT_RADIX digits in the significand, p.  */
#undef FLT_MANT_DIG
#undef DBL_MANT_DIG
#undef LDBL_MANT_DIG
#define FLT_MANT_DIG   __FLT_MANT_DIG__
#define DBL_MANT_DIG   __DBL_MANT_DIG__
#define LDBL_MANT_DIG  __LDBL_MANT_DIG__

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* The floating-point expression evaluation method.
      -1  indeterminate
       0  evaluate all operations and constants just to the range and
	  precision of the type
       1  evaluate operations and constants of type float and double
	  to the range and precision of the double type, evaluate
	  long double operations and constants to the range and
	  precision of the long double type
       2  evaluate all operations and constants to the range and
	  precision of the long double type

   ??? This ought to change with the setting of the fp control word;
   the value provided by the compiler assumes the widest setting.  */
#undef FLT_EVAL_METHOD
#define FLT_EVAL_METHOD	__FLT_EVAL_METHOD__

#endif /* C99 */


/* Control word values for unNew (use with related unMask above) */
#define	_DN_SAVE	0x00000000
#define	_DN_FLUSH	0x01000000
#define	_EM_INVALID	0x00000010
#define	_EM_DENORMAL	0x00080000
#define	_EM_ZERODIVIDE	0x00000008
#define	_EM_OVERFLOW	0x00000004
#define	_EM_UNDERFLOW	0x00000002
#define	_EM_INEXACT	0x00000001
#define	_IC_AFFINE	0x00040000
#define	_IC_PROJECTIVE	0x00000000
#define	_RC_CHOP	0x00000300
#define	_RC_UP		0x00000200
#define	_RC_DOWN	0x00000100
#define	_RC_NEAR	0x00000000
#define	_PC_24		0x00020000
#define	_PC_53		0x00010000
#define	_PC_64		0x00000000

/* These are also defined in Mingw math.h, needed to work around
   GCC build issues.  */
/* Return values for fpclass. */
#ifndef __MINGW_FPCLASS_DEFINED
#define __MINGW_FPCLASS_DEFINED 1
#define	_FPCLASS_SNAN	0x0001	/* Signaling "Not a Number" */
#define	_FPCLASS_QNAN	0x0002	/* Quiet "Not a Number" */
#define	_FPCLASS_NINF	0x0004	/* Negative Infinity */
#define	_FPCLASS_NN	0x0008	/* Negative Normal */
#define	_FPCLASS_ND	0x0010	/* Negative Denormal */
#define	_FPCLASS_NZ	0x0020	/* Negative Zero */
#define	_FPCLASS_PZ	0x0040	/* Positive Zero */
#define	_FPCLASS_PD	0x0080	/* Positive Denormal */
#define	_FPCLASS_PN	0x0100	/* Positive Normal */
#define	_FPCLASS_PINF	0x0200	/* Positive Infinity */
#endif /* __MINGW_FPCLASS_DEFINED */

/* invalid subconditions (_SW_INVALID also set) */
#define _SW_UNEMULATED		0x0040  /* unemulated instruction */
#define _SW_SQRTNEG		0x0080  /* square root of a neg number */
#define _SW_STACKOVERFLOW	0x0200  /* FP stack overflow */
#define _SW_STACKUNDERFLOW	0x0400  /* FP stack underflow */

/*  Floating point error signals and return codes */
#define _FPE_INVALID		0x81
#define _FPE_DENORMAL		0x82
#define _FPE_ZERODIVIDE		0x83
#define _FPE_OVERFLOW		0x84
#define _FPE_UNDERFLOW		0x85
#define _FPE_INEXACT		0x86
#define _FPE_UNEMULATED		0x87
#define _FPE_SQRTNEG		0x88
#define _FPE_STACKOVERFLOW	0x8a
#define _FPE_STACKUNDERFLOW	0x8b
#define _FPE_EXPLICITGEN	0x8c    /* raise( SIGFPE ); */

#define CW_DEFAULT _CW_DEFAULT
#define MCW_PC  _MCW_PC
#define PC_24   _PC_24
#define PC_53   _PC_53
#define PC_64   _PC_64

#if defined(_M_IX86)
#define _CW_DEFAULT (_RC_NEAR+_PC_53+_EM_INVALID+_EM_ZERODIVIDE+_EM_OVERFLOW+_EM_UNDERFLOW+_EM_INEXACT+_EM_DENORMAL)
#elif defined(_M_IA64)
#define _CW_DEFAULT (_RC_NEAR+_PC_64+_EM_INVALID+_EM_ZERODIVIDE+_EM_OVERFLOW+_EM_UNDERFLOW+_EM_INEXACT+_EM_DENORMAL)
#elif defined(_M_AMD64)
#define _CW_DEFAULT (_RC_NEAR+_EM_INVALID+_EM_ZERODIVIDE+_EM_OVERFLOW+_EM_UNDERFLOW+_EM_INEXACT+_EM_DENORMAL)
#endif

#ifndef RC_INVOKED

#ifdef	__cplusplus
extern "C" {
#endif

/* Set the FPU control word as cw = (cw & ~unMask) | (unNew & unMask),
 * i.e. change the bits in unMask to have the values they have in unNew,
 * leaving other bits unchanged. */
_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _controlfp (unsigned int unNew, unsigned int unMask) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
_CRTIMP errno_t __cdecl _controlfp_s(unsigned int *_CurrentState, unsigned int _NewValue, unsigned int _Mask);
_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _control87 (unsigned int unNew, unsigned int unMask);


_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _clearfp (void);	/* Clear the FPU status word */
_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _statusfp (void);	/* Report the FPU status word */
#define		_clear87	_clearfp
#define		_status87	_statusfp


/*
   MSVCRT.dll _fpreset initializes the control register to 0x27f,
   the status register to zero and the tag word to 0FFFFh.
   This differs from asm instruction finit/fninit which set control
   word to 0x37f (64 bit mantissa precison rather than 53 bit).
   By default, the mingw version of _fpreset sets fp control as
   per fninit. To use the MSVCRT.dll _fpreset, include CRT_fp8.o when
   building your application.	 
*/
void __cdecl __MINGW_NOTHROW _fpreset (void);
void __cdecl __MINGW_NOTHROW fpreset (void);

/* Global 'variable' for the current floating point error code. */
_CRTIMP int * __cdecl __MINGW_NOTHROW __fpecode(void);
#define	_fpecode	(*(__fpecode()))

/*
 * IEEE recommended functions.  MS puts them in float.h
 * but they really belong in math.h.
 */

#ifndef _SIGN_DEFINED
#define _SIGN_DEFINED
_CRTIMP double __cdecl __MINGW_NOTHROW _chgsign (double _X);
_CRTIMP double __cdecl __MINGW_NOTHROW _copysign (double _Number,double _Sign);
_CRTIMP double __cdecl __MINGW_NOTHROW _logb (double);
_CRTIMP double __cdecl __MINGW_NOTHROW _nextafter (double, double);
_CRTIMP double __cdecl __MINGW_NOTHROW _scalb (double, long);

_CRTIMP int __cdecl __MINGW_NOTHROW _finite (double);
_CRTIMP int __cdecl __MINGW_NOTHROW _fpclass (double);
_CRTIMP int __cdecl __MINGW_NOTHROW _isnan (double);

#define _copysignl copysignl
extern long double __cdecl _chgsignl (long double);
#endif /* _SIGN_DEFINED */

#ifdef	__cplusplus
}
#endif

#endif	/* Not RC_INVOKED */

#endif	/* Not __STRICT_ANSI__ */

#endif /* _MINGW_FLOAT_H_ */


/*
 * float.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within the package.
 *
 * Constants related to floating point arithmetic.
 *
 * Also included here are some non-ANSI bits for accessing the floating
 * point controller.
 *
 */

#if (defined (__GNUC__) && defined (__GNUC_MINOR__)) \
    || (defined(__clang__) && defined(__clang_major__))
#if (__GNUC__ < 4  || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)) \
    || (__clang_major__ >=3)
#if !defined(_FLOAT_H___) && !defined(__FLOAT_H)
#include_next <float.h>
#endif
#elif !defined (_FLOAT_H___)
#if (__GNUC__ < 4)
#error Corrupt install of gcc-s internal headers, or search order was changed.
#else
	/* #include_next <float_ginclude.h> */
	
   	/* Number of decimal digits, q, such that any floating-point number with q
   	   decimal digits can be rounded into a floating-point number with p radix b
	   digits and back again without change to the q decimal digits,

	   p * log10(b)			if b is a power of 10
	   floor((p - 1) * log10(b))	otherwise
	*/
	#undef FLT_DIG
	#undef DBL_DIG
	#undef LDBL_DIG
	#define FLT_DIG		__FLT_DIG__
	#define DBL_DIG		__DBL_DIG__
	#define LDBL_DIG	__LDBL_DIG__
	
	
	/* Maximum representable finite floating-point number,

	   (1 - b**-p) * b**emax
	*/
	#undef FLT_MAX
	#undef DBL_MAX
	#undef LDBL_MAX
	#define FLT_MAX		__FLT_MAX__
	#define DBL_MAX		__DBL_MAX__
	#define LDBL_MAX	__LDBL_MAX__
	
	
	/* Minimum normalized positive floating-point number, b**(emin - 1).  */
	#undef FLT_MIN
	#undef DBL_MIN
	#undef LDBL_MIN
	#define FLT_MIN		__FLT_MIN__
	#define DBL_MIN		__DBL_MIN__
	#define LDBL_MIN	__LDBL_MIN__
	
	/* Needed for libjava building - Victor K. */

	/* Radix of exponent representation, b. */
    #undef FLT_RADIX
    #define FLT_RADIX	__FLT_RADIX__
    
    /* Minimum int x such that FLT_RADIX**(x-1) is a normalized float, emin */
	#undef FLT_MIN_EXP
	#undef DBL_MIN_EXP
	#undef LDBL_MIN_EXP
	#define FLT_MIN_EXP	__FLT_MIN_EXP__
	#define DBL_MIN_EXP	__DBL_MIN_EXP__
	#define LDBL_MIN_EXP	__LDBL_MIN_EXP__

	/* Minimum negative integer such that 10 raised to that power is in the
   	range of normalized floating-point numbers,

	ceil(log10(b) * (emin - 1))
	*/
	#undef FLT_MIN_10_EXP
	#undef DBL_MIN_10_EXP
	#undef LDBL_MIN_10_EXP
	#define FLT_MIN_10_EXP	__FLT_MIN_10_EXP__
	#define DBL_MIN_10_EXP	__DBL_MIN_10_EXP__
	#define LDBL_MIN_10_EXP	__LDBL_MIN_10_EXP__

	/* Maximum int x such that FLT_RADIX**(x-1) is a representable float, emax.  */
	#undef FLT_MAX_EXP
	#undef DBL_MAX_EXP
	#undef LDBL_MAX_EXP
	#define FLT_MAX_EXP	__FLT_MAX_EXP__
	#define DBL_MAX_EXP	__DBL_MAX_EXP__
	#define LDBL_MAX_EXP	__LDBL_MAX_EXP__

	/* Maximum integer such that 10 raised to that power is in the range of
   	representable finite floating-point numbers,

	floor(log10((1 - b**-p) * b**emax))
	*/
	#undef FLT_MAX_10_EXP
	#undef DBL_MAX_10_EXP
	#undef LDBL_MAX_10_EXP
	#define FLT_MAX_10_EXP	__FLT_MAX_10_EXP__
	#define DBL_MAX_10_EXP	__DBL_MAX_10_EXP__
	#define LDBL_MAX_10_EXP	__LDBL_MAX_10_EXP__

	/* Addition rounds to 0: zero, 1: nearest, 2: +inf, 3: -inf, -1: unknown.  */
	/* ??? This is supposed to change with calls to fesetround in <fenv.h>.  */
	#undef FLT_ROUNDS
	#define FLT_ROUNDS 1
    
	#define _FLOAT_H___
#endif
#endif
#endif

#ifndef _MINGW_FLOAT_H_
#define _MINGW_FLOAT_H_

/* All the headers include this file. */
#include <crtdefs.h>

/*
 * Functions and definitions for controlling the FPU.
 */
#ifndef	__STRICT_ANSI__

/* TODO: These constants are only valid for x86 machines */

/* Control word masks for unMask */
#define	_MCW_DN  	0x03000000  	/* Denormal control */
#define	_MCW_EM		0x0008001F	/* Error masks */
#define	_MCW_IC		0x00040000	/* Infinity */
#define	_MCW_RC		0x00000300	/* Rounding */
#define	_MCW_PC		0x00030000	/* Precision */

/* Number of base-FLT_RADIX digits in the significand, p.  */
#undef FLT_MANT_DIG
#undef DBL_MANT_DIG
#undef LDBL_MANT_DIG
#define FLT_MANT_DIG   __FLT_MANT_DIG__
#define DBL_MANT_DIG   __DBL_MANT_DIG__
#define LDBL_MANT_DIG  __LDBL_MANT_DIG__

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* The floating-point expression evaluation method.
      -1  indeterminate
       0  evaluate all operations and constants just to the range and
	  precision of the type
       1  evaluate operations and constants of type float and double
	  to the range and precision of the double type, evaluate
	  long double operations and constants to the range and
	  precision of the long double type
       2  evaluate all operations and constants to the range and
	  precision of the long double type

   ??? This ought to change with the setting of the fp control word;
   the value provided by the compiler assumes the widest setting.  */
#undef FLT_EVAL_METHOD
#define FLT_EVAL_METHOD	__FLT_EVAL_METHOD__

#endif /* C99 */


/* Control word values for unNew (use with related unMask above) */
#define	_DN_SAVE	0x00000000
#define	_DN_FLUSH	0x01000000
#define	_EM_INVALID	0x00000010
#define	_EM_DENORMAL	0x00080000
#define	_EM_ZERODIVIDE	0x00000008
#define	_EM_OVERFLOW	0x00000004
#define	_EM_UNDERFLOW	0x00000002
#define	_EM_INEXACT	0x00000001
#define	_IC_AFFINE	0x00040000
#define	_IC_PROJECTIVE	0x00000000
#define	_RC_CHOP	0x00000300
#define	_RC_UP		0x00000200
#define	_RC_DOWN	0x00000100
#define	_RC_NEAR	0x00000000
#define	_PC_24		0x00020000
#define	_PC_53		0x00010000
#define	_PC_64		0x00000000

/* These are also defined in Mingw math.h, needed to work around
   GCC build issues.  */
/* Return values for fpclass. */
#ifndef __MINGW_FPCLASS_DEFINED
#define __MINGW_FPCLASS_DEFINED 1
#define	_FPCLASS_SNAN	0x0001	/* Signaling "Not a Number" */
#define	_FPCLASS_QNAN	0x0002	/* Quiet "Not a Number" */
#define	_FPCLASS_NINF	0x0004	/* Negative Infinity */
#define	_FPCLASS_NN	0x0008	/* Negative Normal */
#define	_FPCLASS_ND	0x0010	/* Negative Denormal */
#define	_FPCLASS_NZ	0x0020	/* Negative Zero */
#define	_FPCLASS_PZ	0x0040	/* Positive Zero */
#define	_FPCLASS_PD	0x0080	/* Positive Denormal */
#define	_FPCLASS_PN	0x0100	/* Positive Normal */
#define	_FPCLASS_PINF	0x0200	/* Positive Infinity */
#endif /* __MINGW_FPCLASS_DEFINED */

/* invalid subconditions (_SW_INVALID also set) */
#define _SW_UNEMULATED		0x0040  /* unemulated instruction */
#define _SW_SQRTNEG		0x0080  /* square root of a neg number */
#define _SW_STACKOVERFLOW	0x0200  /* FP stack overflow */
#define _SW_STACKUNDERFLOW	0x0400  /* FP stack underflow */

/*  Floating point error signals and return codes */
#define _FPE_INVALID		0x81
#define _FPE_DENORMAL		0x82
#define _FPE_ZERODIVIDE		0x83
#define _FPE_OVERFLOW		0x84
#define _FPE_UNDERFLOW		0x85
#define _FPE_INEXACT		0x86
#define _FPE_UNEMULATED		0x87
#define _FPE_SQRTNEG		0x88
#define _FPE_STACKOVERFLOW	0x8a
#define _FPE_STACKUNDERFLOW	0x8b
#define _FPE_EXPLICITGEN	0x8c    /* raise( SIGFPE ); */

#define CW_DEFAULT _CW_DEFAULT
#define MCW_PC  _MCW_PC
#define PC_24   _PC_24
#define PC_53   _PC_53
#define PC_64   _PC_64

#if defined(_M_IX86)
#define _CW_DEFAULT (_RC_NEAR+_PC_53+_EM_INVALID+_EM_ZERODIVIDE+_EM_OVERFLOW+_EM_UNDERFLOW+_EM_INEXACT+_EM_DENORMAL)
#elif defined(_M_IA64)
#define _CW_DEFAULT (_RC_NEAR+_PC_64+_EM_INVALID+_EM_ZERODIVIDE+_EM_OVERFLOW+_EM_UNDERFLOW+_EM_INEXACT+_EM_DENORMAL)
#elif defined(_M_AMD64)
#define _CW_DEFAULT (_RC_NEAR+_EM_INVALID+_EM_ZERODIVIDE+_EM_OVERFLOW+_EM_UNDERFLOW+_EM_INEXACT+_EM_DENORMAL)
#endif

#ifndef RC_INVOKED

#ifdef	__cplusplus
extern "C" {
#endif

/* Set the FPU control word as cw = (cw & ~unMask) | (unNew & unMask),
 * i.e. change the bits in unMask to have the values they have in unNew,
 * leaving other bits unchanged. */
_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _controlfp (unsigned int unNew, unsigned int unMask) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
_CRTIMP errno_t __cdecl _controlfp_s(unsigned int *_CurrentState, unsigned int _NewValue, unsigned int _Mask);
_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _control87 (unsigned int unNew, unsigned int unMask);


_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _clearfp (void);	/* Clear the FPU status word */
_CRTIMP unsigned int __cdecl __MINGW_NOTHROW _statusfp (void);	/* Report the FPU status word */
#define		_clear87	_clearfp
#define		_status87	_statusfp


/*
   MSVCRT.dll _fpreset initializes the control register to 0x27f,
   the status register to zero and the tag word to 0FFFFh.
   This differs from asm instruction finit/fninit which set control
   word to 0x37f (64 bit mantissa precison rather than 53 bit).
   By default, the mingw version of _fpreset sets fp control as
   per fninit. To use the MSVCRT.dll _fpreset, include CRT_fp8.o when
   building your application.	 
*/
void __cdecl __MINGW_NOTHROW _fpreset (void);
void __cdecl __MINGW_NOTHROW fpreset (void);

/* Global 'variable' for the current floating point error code. */
_CRTIMP int * __cdecl __MINGW_NOTHROW __fpecode(void);
#define	_fpecode	(*(__fpecode()))

/*
 * IEEE recommended functions.  MS puts them in float.h
 * but they really belong in math.h.
 */

#ifndef _SIGN_DEFINED
#define _SIGN_DEFINED
_CRTIMP double __cdecl __MINGW_NOTHROW _chgsign (double _X);
_CRTIMP double __cdecl __MINGW_NOTHROW _copysign (double _Number,double _Sign);
_CRTIMP double __cdecl __MINGW_NOTHROW _logb (double);
_CRTIMP double __cdecl __MINGW_NOTHROW _nextafter (double, double);
_CRTIMP double __cdecl __MINGW_NOTHROW _scalb (double, long);

_CRTIMP int __cdecl __MINGW_NOTHROW _finite (double);
_CRTIMP int __cdecl __MINGW_NOTHROW _fpclass (double);
_CRTIMP int __cdecl __MINGW_NOTHROW _isnan (double);

#define _copysignl copysignl
extern long double __cdecl _chgsignl (long double);
#endif /* _SIGN_DEFINED */

#ifdef	__cplusplus
}
#endif

#endif	/* Not RC_INVOKED */

#endif	/* Not __STRICT_ANSI__ */

#endif /* _MINGW_FLOAT_H_ */


/* Copyright (C) 2002-2014 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  5.2.4.2.2  Characteristics of floating types <float.h>
 */

#ifndef _FLOAT_H___
#define _FLOAT_H___

/* Radix of exponent representation, b. */
#undef FLT_RADIX
#define FLT_RADIX	__FLT_RADIX__

/* Number of base-FLT_RADIX digits in the significand, p.  */
#undef FLT_MANT_DIG
#undef DBL_MANT_DIG
#undef LDBL_MANT_DIG
#define FLT_MANT_DIG	__FLT_MANT_DIG__
#define DBL_MANT_DIG	__DBL_MANT_DIG__
#define LDBL_MANT_DIG	__LDBL_MANT_DIG__

/* Number of decimal digits, q, such that any floating-point number with q
   decimal digits can be rounded into a floating-point number with p radix b
   digits and back again without change to the q decimal digits,

	p * log10(b)			if b is a power of 10
	floor((p - 1) * log10(b))	otherwise
*/
#undef FLT_DIG
#undef DBL_DIG
#undef LDBL_DIG
#define FLT_DIG		__FLT_DIG__
#define DBL_DIG		__DBL_DIG__
#define LDBL_DIG	__LDBL_DIG__

/* Minimum int x such that FLT_RADIX**(x-1) is a normalized float, emin */
#undef FLT_MIN_EXP
#undef DBL_MIN_EXP
#undef LDBL_MIN_EXP
#define FLT_MIN_EXP	__FLT_MIN_EXP__
#define DBL_MIN_EXP	__DBL_MIN_EXP__
#define LDBL_MIN_EXP	__LDBL_MIN_EXP__

/* Minimum negative integer such that 10 raised to that power is in the
   range of normalized floating-point numbers,

	ceil(log10(b) * (emin - 1))
*/
#undef FLT_MIN_10_EXP
#undef DBL_MIN_10_EXP
#undef LDBL_MIN_10_EXP
#define FLT_MIN_10_EXP	__FLT_MIN_10_EXP__
#define DBL_MIN_10_EXP	__DBL_MIN_10_EXP__
#define LDBL_MIN_10_EXP	__LDBL_MIN_10_EXP__

/* Maximum int x such that FLT_RADIX**(x-1) is a representable float, emax.  */
#undef FLT_MAX_EXP
#undef DBL_MAX_EXP
#undef LDBL_MAX_EXP
#define FLT_MAX_EXP	__FLT_MAX_EXP__
#define DBL_MAX_EXP	__DBL_MAX_EXP__
#define LDBL_MAX_EXP	__LDBL_MAX_EXP__

/* Maximum integer such that 10 raised to that power is in the range of
   representable finite floating-point numbers,

	floor(log10((1 - b**-p) * b**emax))
*/
#undef FLT_MAX_10_EXP
#undef DBL_MAX_10_EXP
#undef LDBL_MAX_10_EXP
#define FLT_MAX_10_EXP	__FLT_MAX_10_EXP__
#define DBL_MAX_10_EXP	__DBL_MAX_10_EXP__
#define LDBL_MAX_10_EXP	__LDBL_MAX_10_EXP__

/* Maximum representable finite floating-point number,

	(1 - b**-p) * b**emax
*/
#undef FLT_MAX
#undef DBL_MAX
#undef LDBL_MAX
#define FLT_MAX		__FLT_MAX__
#define DBL_MAX		__DBL_MAX__
#define LDBL_MAX	__LDBL_MAX__

/* The difference between 1 and the least value greater than 1 that is
   representable in the given floating point type, b**1-p.  */
#undef FLT_EPSILON
#undef DBL_EPSILON
#undef LDBL_EPSILON
#define FLT_EPSILON	__FLT_EPSILON__
#define DBL_EPSILON	__DBL_EPSILON__
#define LDBL_EPSILON	__LDBL_EPSILON__

/* Minimum normalized positive floating-point number, b**(emin - 1).  */
#undef FLT_MIN
#undef DBL_MIN
#undef LDBL_MIN
#define FLT_MIN		__FLT_MIN__
#define DBL_MIN		__DBL_MIN__
#define LDBL_MIN	__LDBL_MIN__

/* Addition rounds to 0: zero, 1: nearest, 2: +inf, 3: -inf, -1: unknown.  */
/* ??? This is supposed to change with calls to fesetround in <fenv.h>.  */
#undef FLT_ROUNDS
#define FLT_ROUNDS 1

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/* The floating-point expression evaluation method.
        -1  indeterminate
         0  evaluate all operations and constants just to the range and
            precision of the type
         1  evaluate operations and constants of type float and double
            to the range and precision of the double type, evaluate
            long double operations and constants to the range and
            precision of the long double type
         2  evaluate all operations and constants to the range and
            precision of the long double type

   ??? This ought to change with the setting of the fp control word;
   the value provided by the compiler assumes the widest setting.  */
#undef FLT_EVAL_METHOD
#define FLT_EVAL_METHOD	__FLT_EVAL_METHOD__

/* Number of decimal digits, n, such that any floating-point number in the
   widest supported floating type with pmax radix b digits can be rounded
   to a floating-point number with n decimal digits and back again without
   change to the value,

	pmax * log10(b)			if b is a power of 10
	ceil(1 + pmax * log10(b))	otherwise
*/
#undef DECIMAL_DIG
#define DECIMAL_DIG	__DECIMAL_DIG__

#endif /* C99 */

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* Versions of DECIMAL_DIG for each floating-point type.  */
#undef FLT_DECIMAL_DIG
#undef DBL_DECIMAL_DIG
#undef LDBL_DECIMAL_DIG
#define FLT_DECIMAL_DIG		__FLT_DECIMAL_DIG__
#define DBL_DECIMAL_DIG		__DBL_DECIMAL_DIG__
#define LDBL_DECIMAL_DIG	__DECIMAL_DIG__

/* Whether types support subnormal numbers.  */
#undef FLT_HAS_SUBNORM
#undef DBL_HAS_SUBNORM
#undef LDBL_HAS_SUBNORM
#define FLT_HAS_SUBNORM		__FLT_HAS_DENORM__
#define DBL_HAS_SUBNORM		__DBL_HAS_DENORM__
#define LDBL_HAS_SUBNORM	__LDBL_HAS_DENORM__

/* Minimum positive values, including subnormals.  */
#undef FLT_TRUE_MIN
#undef DBL_TRUE_MIN
#undef LDBL_TRUE_MIN
#if __FLT_HAS_DENORM__
#define FLT_TRUE_MIN	__FLT_DENORM_MIN__
#else
#define FLT_TRUE_MIN	__FLT_MIN__
#endif
#if __DBL_HAS_DENORM__
#define DBL_TRUE_MIN	__DBL_DENORM_MIN__
#else
#define DBL_TRUE_MIN	__DBL_MIN__
#endif
#if __LDBL_HAS_DENORM__
#define LDBL_TRUE_MIN	__LDBL_DENORM_MIN__
#else
#define LDBL_TRUE_MIN	__LDBL_MIN__
#endif

#endif /* C11 */

#ifdef __STDC_WANT_DEC_FP__
/* Draft Technical Report 24732, extension for decimal floating-point
   arithmetic: Characteristic of decimal floating types <float.h>.  */

/* Number of base-FLT_RADIX digits in the significand, p.  */
#undef DEC32_MANT_DIG
#undef DEC64_MANT_DIG
#undef DEC128_MANT_DIG
#define DEC32_MANT_DIG	__DEC32_MANT_DIG__
#define DEC64_MANT_DIG	__DEC64_MANT_DIG__
#define DEC128_MANT_DIG	__DEC128_MANT_DIG__

/* Minimum exponent. */
#undef DEC32_MIN_EXP
#undef DEC64_MIN_EXP
#undef DEC128_MIN_EXP
#define DEC32_MIN_EXP	__DEC32_MIN_EXP__
#define DEC64_MIN_EXP	__DEC64_MIN_EXP__
#define DEC128_MIN_EXP	__DEC128_MIN_EXP__

/* Maximum exponent. */
#undef DEC32_MAX_EXP
#undef DEC64_MAX_EXP
#undef DEC128_MAX_EXP
#define DEC32_MAX_EXP	__DEC32_MAX_EXP__
#define DEC64_MAX_EXP	__DEC64_MAX_EXP__
#define DEC128_MAX_EXP	__DEC128_MAX_EXP__

/* Maximum representable finite decimal floating-point number
   (there are 6, 15, and 33 9s after the decimal points respectively). */
#undef DEC32_MAX
#undef DEC64_MAX
#undef DEC128_MAX
#define DEC32_MAX   __DEC32_MAX__
#define DEC64_MAX   __DEC64_MAX__
#define DEC128_MAX  __DEC128_MAX__

/* The difference between 1 and the least value greater than 1 that is
   representable in the given floating point type. */
#undef DEC32_EPSILON
#undef DEC64_EPSILON
#undef DEC128_EPSILON
#define DEC32_EPSILON	__DEC32_EPSILON__
#define DEC64_EPSILON	__DEC64_EPSILON__
#define DEC128_EPSILON	__DEC128_EPSILON__

/* Minimum normalized positive floating-point number. */
#undef DEC32_MIN
#undef DEC64_MIN
#undef DEC128_MIN
#define DEC32_MIN	__DEC32_MIN__
#define DEC64_MIN	__DEC64_MIN__
#define DEC128_MIN	__DEC128_MIN__

/* Minimum subnormal positive floating-point number. */
#undef DEC32_SUBNORMAL_MIN
#undef DEC64_SUBNORMAL_MIN
#undef DEC128_SUBNORMAL_MIN
#define DEC32_SUBNORMAL_MIN       __DEC32_SUBNORMAL_MIN__
#define DEC64_SUBNORMAL_MIN       __DEC64_SUBNORMAL_MIN__
#define DEC128_SUBNORMAL_MIN      __DEC128_SUBNORMAL_MIN__

/* The floating-point expression evaluation method.
         -1  indeterminate
         0  evaluate all operations and constants just to the range and
            precision of the type
         1  evaluate operations and constants of type _Decimal32 
	    and _Decimal64 to the range and precision of the _Decimal64 
            type, evaluate _Decimal128 operations and constants to the 
	    range and precision of the _Decimal128 type;
	 2  evaluate all operations and constants to the range and
	    precision of the _Decimal128 type.  */

#undef DEC_EVAL_METHOD
#define DEC_EVAL_METHOD	__DEC_EVAL_METHOD__

#endif /* __STDC_WANT_DEC_FP__ */

#endif /* _FLOAT_H___ */
#include_next <float.h>

// File based streams -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/fstream
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.8  File-based streams
//

#ifndef _GLIBCXX_FSTREAM
#define _GLIBCXX_FSTREAM 1

#pragma GCC system_header

#include <istream>
#include <ostream>
#include <bits/codecvt.h>
#include <cstdio>             // For BUFSIZ
#include <bits/basic_file.h>  // For __basic_file, __c_lock
#if __cplusplus >= 201103L
#include <string>             // For std::string overloads.
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // [27.8.1.1] template class basic_filebuf
  /**
   *  @brief  The actual work of input and output (for files).
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class associates both its input and output sequence with an
   *  external disk file, and maintains a joint file position for both
   *  sequences.  Many of its semantics are described in terms of similar
   *  behavior in the Standard C Library's @c FILE streams.
   *
   *  Requirements on traits_type, specific to this class:
   *  - traits_type::pos_type must be fpos<traits_type::state_type>
   *  - traits_type::off_type must be streamoff
   *  - traits_type::state_type must be Assignable and DefaultConstructible,
   *  - traits_type::state_type() must be the initial state for codecvt.
   */
  template<typename _CharT, typename _Traits>
    class basic_filebuf : public basic_streambuf<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT                     	        char_type;
      typedef _Traits                    	        traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      typedef basic_streambuf<char_type, traits_type>  	__streambuf_type;
      typedef basic_filebuf<char_type, traits_type>     __filebuf_type;
      typedef __basic_file<char>		        __file_type;
      typedef typename traits_type::state_type          __state_type;
      typedef codecvt<char_type, char, __state_type>    __codecvt_type;

      friend class ios_base; // For sync_with_stdio.

    protected:
      // Data Members:
      // MT lock inherited from libio or other low-level io library.
      __c_lock          	_M_lock;

      // External buffer.
      __file_type 		_M_file;

      /// Place to stash in || out || in | out settings for current filebuf.
      ios_base::openmode 	_M_mode;

      // Beginning state type for codecvt.
      __state_type 		_M_state_beg;

      // During output, the state that corresponds to pptr(),
      // during input, the state that corresponds to egptr() and
      // _M_ext_next.
      __state_type		_M_state_cur;

      // Not used for output. During input, the state that corresponds
      // to eback() and _M_ext_buf.
      __state_type		_M_state_last;

      /// Pointer to the beginning of internal buffer.
      char_type*		_M_buf; 	

      /**
       *  Actual size of internal buffer. This number is equal to the size
       *  of the put area + 1 position, reserved for the overflow char of
       *  a full area.
       */
      size_t			_M_buf_size;

      // Set iff _M_buf is allocated memory from _M_allocate_internal_buffer.
      bool			_M_buf_allocated;

      /**
       *  _M_reading == false && _M_writing == false for @b uncommitted mode;
       *  _M_reading == true for @b read mode;
       *  _M_writing == true for @b write mode;
       *
       *  NB: _M_reading == true && _M_writing == true is unused.
       */
      bool                      _M_reading;
      bool                      _M_writing;

      //@{
      /**
       *  Necessary bits for putback buffer management.
       *
       *  @note pbacks of over one character are not currently supported.
       */
      char_type			_M_pback;
      char_type*		_M_pback_cur_save;
      char_type*		_M_pback_end_save;
      bool			_M_pback_init;
      //@}

      // Cached codecvt facet.
      const __codecvt_type* 	_M_codecvt;

      /**
       *  Buffer for external characters. Used for input when
       *  codecvt::always_noconv() == false. When valid, this corresponds
       *  to eback().
       */
      char*			_M_ext_buf;

      /**
       *  Size of buffer held by _M_ext_buf.
       */
      streamsize		_M_ext_buf_size;

      /**
       *  Pointers into the buffer held by _M_ext_buf that delimit a
       *  subsequence of bytes that have been read but not yet converted.
       *  When valid, _M_ext_next corresponds to egptr().
       */
      const char*		_M_ext_next;
      char*			_M_ext_end;

      /**
       *  Initializes pback buffers, and moves normal buffers to safety.
       *  Assumptions:
       *  _M_in_cur has already been moved back
       */
      void
      _M_create_pback()
      {
	if (!_M_pback_init)
	  {
	    _M_pback_cur_save = this->gptr();
	    _M_pback_end_save = this->egptr();
	    this->setg(&_M_pback, &_M_pback, &_M_pback + 1);
	    _M_pback_init = true;
	  }
      }

      /**
       *  Deactivates pback buffer contents, and restores normal buffer.
       *  Assumptions:
       *  The pback buffer has only moved forward.
       */
      void
      _M_destroy_pback() throw()
      {
	if (_M_pback_init)
	  {
	    // Length _M_in_cur moved in the pback buffer.
	    _M_pback_cur_save += this->gptr() != this->eback();
	    this->setg(_M_buf, _M_pback_cur_save, _M_pback_end_save);
	    _M_pback_init = false;
	  }
      }

    public:
      // Constructors/destructor:
      /**
       *  @brief  Does not open any files.
       *
       *  The default constructor initializes the parent class using its
       *  own default ctor.
       */
      basic_filebuf();

      /**
       *  @brief  The destructor closes the file first.
       */
      virtual
      ~basic_filebuf()
      { this->close(); }

      // Members:
      /**
       *  @brief  Returns true if the external file is open.
       */
      bool
      is_open() const throw()
      { return _M_file.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *  @return  @c this on success, NULL on failure
       *
       *  If a file is already open, this function immediately fails.
       *  Otherwise it tries to open the file named @a __s using the flags
       *  given in @a __mode.
       *
       *  Table 92, adapted here, gives the relation between openmode
       *  combinations and the equivalent @c fopen() flags.
       *  (NB: lines app, in|out|app, in|app, binary|app, binary|in|out|app,
       *  and binary|in|app per DR 596)
       *  <pre>
       *  +---------------------------------------------------------+
       *  | ios_base Flag combination            stdio equivalent   |
       *  |binary  in  out  trunc  app                              |
       *  +---------------------------------------------------------+
       *  |             +                        w                  |
       *  |             +           +            a                  |
       *  |                         +            a                  |
       *  |             +     +                  w                  |
       *  |         +                            r                  |
       *  |         +   +                        r+                 |
       *  |         +   +     +                  w+                 |
       *  |         +   +           +            a+                 |
       *  |         +               +            a+                 |
       *  +---------------------------------------------------------+
       *  |   +         +                        wb                 |
       *  |   +         +           +            ab                 |
       *  |   +                     +            ab                 |
       *  |   +         +     +                  wb                 |
       *  |   +     +                            rb                 |
       *  |   +     +   +                        r+b                |
       *  |   +     +   +     +                  w+b                |
       *  |   +     +   +           +            a+b                |
       *  |   +     +               +            a+b                |
       *  +---------------------------------------------------------+
       *  </pre>
       */
      __filebuf_type*
      open(const char* __s, ios_base::openmode __mode);

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *  @return  @c this on success, NULL on failure
       */
      __filebuf_type*
      open(const std::string& __s, ios_base::openmode __mode)
      { return open(__s.c_str(), __mode); }
#endif

      /**
       *  @brief  Closes the currently associated file.
       *  @return  @c this on success, NULL on failure
       *
       *  If no file is currently open, this function immediately fails.
       *
       *  If a <em>put buffer area</em> exists, @c overflow(eof) is
       *  called to flush all the characters.  The file is then
       *  closed.
       *
       *  If any operations fail, this function also fails.
       */
      __filebuf_type*
      close();

    protected:
      void
      _M_allocate_internal_buffer();

      void
      _M_destroy_internal_buffer() throw();

      // [27.8.1.4] overridden virtual functions
      virtual streamsize
      showmanyc();

      // Stroustrup, 1998, p. 628
      // underflow() and uflow() functions are called to get the next
      // character from the real input source when the buffer is empty.
      // Buffered input uses underflow()

      virtual int_type
      underflow();

      virtual int_type
      pbackfail(int_type __c = _Traits::eof());

      // Stroustrup, 1998, p 648
      // The overflow() function is called to transfer characters to the
      // real output destination when the buffer is full. A call to
      // overflow(c) outputs the contents of the buffer plus the
      // character c.
      // 27.5.2.4.5
      // Consume some sequence of the characters in the pending sequence.
      virtual int_type
      overflow(int_type __c = _Traits::eof());

      // Convert internal byte sequence to external, char-based
      // sequence via codecvt.
      bool
      _M_convert_to_external(char_type*, streamsize);

      /**
       *  @brief  Manipulates the buffer.
       *  @param  __s  Pointer to a buffer area.
       *  @param  __n  Size of @a __s.
       *  @return  @c this
       *
       *  If no file has been opened, and both @a __s and @a __n are zero, then
       *  the stream becomes unbuffered.  Otherwise, @c __s is used as a
       *  buffer; see
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt11ch25s02.html
       *  for more.
       */
      virtual __streambuf_type*
      setbuf(char_type* __s, streamsize __n);

      virtual pos_type
      seekoff(off_type __off, ios_base::seekdir __way,
	      ios_base::openmode __mode = ios_base::in | ios_base::out);

      virtual pos_type
      seekpos(pos_type __pos,
	      ios_base::openmode __mode = ios_base::in | ios_base::out);

      // Common code for seekoff, seekpos, and overflow
      pos_type
      _M_seek(off_type __off, ios_base::seekdir __way, __state_type __state);
      
      int
      _M_get_ext_pos(__state_type &__state);

      virtual int
      sync();

      virtual void
      imbue(const locale& __loc);

      virtual streamsize
      xsgetn(char_type* __s, streamsize __n);

      virtual streamsize
      xsputn(const char_type* __s, streamsize __n);

      // Flushes output buffer, then writes unshift sequence.
      bool
      _M_terminate_output();

      /**
       *  This function sets the pointers of the internal buffer, both get
       *  and put areas. Typically:
       *
       *   __off == egptr() - eback() upon underflow/uflow (@b read mode);
       *   __off == 0 upon overflow (@b write mode);
       *   __off == -1 upon open, setbuf, seekoff/pos (@b uncommitted mode).
       *
       *  NB: epptr() - pbase() == _M_buf_size - 1, since _M_buf_size
       *  reflects the actual allocated memory and the last cell is reserved
       *  for the overflow char of a full put area.
       */
      void
      _M_set_buffer(streamsize __off)
      {
	const bool __testin = _M_mode & ios_base::in;
	const bool __testout = (_M_mode & ios_base::out
				|| _M_mode & ios_base::app);

	if (__testin && __off > 0)
	  this->setg(_M_buf, _M_buf, _M_buf + __off);
	else
	  this->setg(_M_buf, _M_buf, _M_buf);

	if (__testout && __off == 0 && _M_buf_size > 1 )
	  this->setp(_M_buf, _M_buf + _M_buf_size - 1);
	else
	  this->setp(0, 0);
      }
    };

  // [27.8.1.5] Template class basic_ifstream
  /**
   *  @brief  Controlling input for files.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class supports reading from named files, using the inherited
   *  functions from std::basic_istream.  To control the associated
   *  sequence, an instance of std::basic_filebuf is used, which this page
   *  refers to as @c sb.
   */
  template<typename _CharT, typename _Traits>
    class basic_ifstream : public basic_istream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_istream<char_type, traits_type>	__istream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors/Destructors:
      /**
       *  @brief  Default constructor.
       *
       *  Initializes @c sb using its default constructor, and passes
       *  @c &sb to the base class initializer.  Does not open any files
       *  (you haven't given it a filename to open).
       */
      basic_ifstream() : __istream_type(), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an input file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::in is automatically included in @a __mode.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      explicit
      basic_ifstream(const char* __s, ios_base::openmode __mode = ios_base::in)
      : __istream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Create an input file stream.
       *  @param  __s  std::string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::in is automatically included in @a __mode.
       */
      explicit
      basic_ifstream(const std::string& __s,
		     ios_base::openmode __mode = ios_base::in)
      : __istream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }
#endif

      /**
       *  @brief  The destructor does nothing.
       *
       *  The file is closed by the filebuf object, not the formatting
       *  stream.
       */
      ~basic_ifstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_filebuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
       */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief  Wrapper to test for an open file.
       *  @return  @c rdbuf()->is_open()
       */
      bool
      is_open()
      { return _M_filebuf.is_open(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 365. Lack of const-qualification in clause 27
      bool
      is_open() const
      { return _M_filebuf.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(s,__mode|in).  If that function
       *  fails, @c failbit is set in the stream's error state.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      void
      open(const char* __s, ios_base::openmode __mode = ios_base::in)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::in))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode|in).  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      open(const std::string& __s, ios_base::openmode __mode = ios_base::in)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::in))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }
#endif

      /**
       *  @brief  Close the file.
       *
       *  Calls @c std::basic_filebuf::close().  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };


  // [27.8.1.8] Template class basic_ofstream
  /**
   *  @brief  Controlling output for files.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class supports reading from named files, using the inherited
   *  functions from std::basic_ostream.  To control the associated
   *  sequence, an instance of std::basic_filebuf is used, which this page
   *  refers to as @c sb.
   */
  template<typename _CharT, typename _Traits>
    class basic_ofstream : public basic_ostream<_CharT,_Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_ostream<char_type, traits_type>	__ostream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors:
      /**
       *  @brief  Default constructor.
       *
       *  Initializes @c sb using its default constructor, and passes
       *  @c &sb to the base class initializer.  Does not open any files
       *  (you haven't given it a filename to open).
       */
      basic_ofstream(): __ostream_type(), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::out | @c ios_base::trunc is automatically included in
       *  @a __mode.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      explicit
      basic_ofstream(const char* __s,
		     ios_base::openmode __mode = ios_base::out|ios_base::trunc)
      : __ostream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Create an output file stream.
       *  @param  __s  std::string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::out | @c ios_base::trunc is automatically included in
       *  @a __mode.
       */
      explicit
      basic_ofstream(const std::string& __s,
		     ios_base::openmode __mode = ios_base::out|ios_base::trunc)
      : __ostream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }
#endif

      /**
       *  @brief  The destructor does nothing.
       *
       *  The file is closed by the filebuf object, not the formatting
       *  stream.
       */
      ~basic_ofstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_filebuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
       */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief  Wrapper to test for an open file.
       *  @return  @c rdbuf()->is_open()
       */
      bool
      is_open()
      { return _M_filebuf.is_open(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 365. Lack of const-qualification in clause 27
      bool
      is_open() const
      { return _M_filebuf.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode|out|trunc).  If that
       *  function fails, @c failbit is set in the stream's error state.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      void
      open(const char* __s,
	   ios_base::openmode __mode = ios_base::out | ios_base::trunc)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::out))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(s,mode|out|trunc).  If that
       *  function fails, @c failbit is set in the stream's error state.
       */
      void
      open(const std::string& __s,
	   ios_base::openmode __mode = ios_base::out | ios_base::trunc)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::out))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }
#endif

      /**
       *  @brief  Close the file.
       *
       *  Calls @c std::basic_filebuf::close().  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };


  // [27.8.1.11] Template class basic_fstream
  /**
   *  @brief  Controlling input and output for files.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class supports reading from and writing to named files, using
   *  the inherited functions from std::basic_iostream.  To control the
   *  associated sequence, an instance of std::basic_filebuf is used, which
   *  this page refers to as @c sb.
   */
  template<typename _CharT, typename _Traits>
    class basic_fstream : public basic_iostream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_ios<char_type, traits_type>		__ios_type;
      typedef basic_iostream<char_type, traits_type>	__iostream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors/destructor:
      /**
       *  @brief  Default constructor.
       *
       *  Initializes @c sb using its default constructor, and passes
       *  @c &sb to the base class initializer.  Does not open any files
       *  (you haven't given it a filename to open).
       */
      basic_fstream()
      : __iostream_type(), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an input/output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      explicit
      basic_fstream(const char* __s,
		    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(0), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Create an input/output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       */
      explicit
      basic_fstream(const std::string& __s,
		    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(0), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }
#endif

      /**
       *  @brief  The destructor does nothing.
       *
       *  The file is closed by the filebuf object, not the formatting
       *  stream.
       */
      ~basic_fstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_filebuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
       */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief  Wrapper to test for an open file.
       *  @return  @c rdbuf()->is_open()
       */
      bool
      is_open()
      { return _M_filebuf.is_open(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 365. Lack of const-qualification in clause 27
      bool
      is_open() const
      { return _M_filebuf.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode).  If that
       *  function fails, @c failbit is set in the stream's error state.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      void
      open(const char* __s,
	   ios_base::openmode __mode = ios_base::in | ios_base::out)
      {
	if (!_M_filebuf.open(__s, __mode))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode).  If that
       *  function fails, @c failbit is set in the stream's error state.
       */
      void
      open(const std::string& __s,
	   ios_base::openmode __mode = ios_base::in | ios_base::out)
      {
	if (!_M_filebuf.open(__s, __mode))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }
#endif

      /**
       *  @brief  Close the file.
       *
       *  Calls @c std::basic_filebuf::close().  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#include <bits/fstream.tcc>

#endif /* _GLIBCXX_FSTREAM */

// Standard stream manipulators -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/iomanip
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.6.3  Standard manipulators
//

#ifndef _GLIBCXX_IOMANIP
#define _GLIBCXX_IOMANIP 1

#pragma GCC system_header

#include <bits/c++config.h>
#include <iosfwd>
#include <bits/ios_base.h>

#if __cplusplus >= 201103L
#include <locale>
#if __cplusplus > 201103L
#include <sstream> // used in quoted.
#endif
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // [27.6.3] standard manipulators
  // Also see DR 183.

  struct _Resetiosflags { ios_base::fmtflags _M_mask; };

  /**
   *  @brief  Manipulator for @c setf.
   *  @param  __mask  A format flags mask.
   *
   *  Sent to a stream object, this manipulator resets the specified flags,
   *  via @e stream.setf(0,__mask).
  */
  inline _Resetiosflags 
  resetiosflags(ios_base::fmtflags __mask)
  { return { __mask }; }

  template<typename _CharT, typename _Traits>
    inline basic_istream<_CharT, _Traits>& 
    operator>>(basic_istream<_CharT, _Traits>& __is, _Resetiosflags __f)
    { 
      __is.setf(ios_base::fmtflags(0), __f._M_mask); 
      return __is; 
    }

  template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _Resetiosflags __f)
    { 
      __os.setf(ios_base::fmtflags(0), __f._M_mask); 
      return __os; 
    }


  struct _Setiosflags { ios_base::fmtflags _M_mask; };

  /**
   *  @brief  Manipulator for @c setf.
   *  @param  __mask  A format flags mask.
   *
   *  Sent to a stream object, this manipulator sets the format flags
   *  to @a __mask.
  */
  inline _Setiosflags 
  setiosflags(ios_base::fmtflags __mask)
  { return { __mask }; }

  template<typename _CharT, typename _Traits>
    inline basic_istream<_CharT, _Traits>& 
    operator>>(basic_istream<_CharT, _Traits>& __is, _Setiosflags __f)
    { 
      __is.setf(__f._M_mask); 
      return __is; 
    }

  template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _Setiosflags __f)
    { 
      __os.setf(__f._M_mask); 
      return __os; 
    }


  struct _Setbase { int _M_base; };

  /**
   *  @brief  Manipulator for @c setf.
   *  @param  __base  A numeric base.
   *
   *  Sent to a stream object, this manipulator changes the
   *  @c ios_base::basefield flags to @c oct, @c dec, or @c hex when @a base
   *  is 8, 10, or 16, accordingly, and to 0 if @a __base is any other value.
  */
  inline _Setbase 
  setbase(int __base)
  { return { __base }; }

  template<typename _CharT, typename _Traits>
    inline basic_istream<_CharT, _Traits>& 
    operator>>(basic_istream<_CharT, _Traits>& __is, _Setbase __f)
    {
      __is.setf(__f._M_base ==  8 ? ios_base::oct : 
		__f._M_base == 10 ? ios_base::dec : 
		__f._M_base == 16 ? ios_base::hex : 
		ios_base::fmtflags(0), ios_base::basefield);
      return __is; 
    }
  
  template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _Setbase __f)
    {
      __os.setf(__f._M_base ==  8 ? ios_base::oct : 
		__f._M_base == 10 ? ios_base::dec : 
		__f._M_base == 16 ? ios_base::hex : 
		ios_base::fmtflags(0), ios_base::basefield);
      return __os; 
    }
  

  template<typename _CharT>
    struct _Setfill { _CharT _M_c; };

  /**
   *  @brief  Manipulator for @c fill.
   *  @param  __c  The new fill character.
   *
   *  Sent to a stream object, this manipulator calls @c fill(__c) for that
   *  object.
  */
  template<typename _CharT>
    inline _Setfill<_CharT>
    setfill(_CharT __c)
    { return { __c }; }

  template<typename _CharT, typename _Traits>
    inline basic_istream<_CharT, _Traits>& 
    operator>>(basic_istream<_CharT, _Traits>& __is, _Setfill<_CharT> __f)
    { 
      __is.fill(__f._M_c); 
      return __is; 
    }

  template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _Setfill<_CharT> __f)
    { 
      __os.fill(__f._M_c); 
      return __os; 
    }


  struct _Setprecision { int _M_n; };

  /**
   *  @brief  Manipulator for @c precision.
   *  @param  __n  The new precision.
   *
   *  Sent to a stream object, this manipulator calls @c precision(__n) for
   *  that object.
  */
  inline _Setprecision 
  setprecision(int __n)
  { return { __n }; }

  template<typename _CharT, typename _Traits>
    inline basic_istream<_CharT, _Traits>& 
    operator>>(basic_istream<_CharT, _Traits>& __is, _Setprecision __f)
    { 
      __is.precision(__f._M_n); 
      return __is; 
    }

  template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _Setprecision __f)
    { 
      __os.precision(__f._M_n); 
      return __os; 
    }


  struct _Setw { int _M_n; };

  /**
   *  @brief  Manipulator for @c width.
   *  @param  __n  The new width.
   *
   *  Sent to a stream object, this manipulator calls @c width(__n) for
   *  that object.
  */
  inline _Setw 
  setw(int __n)
  { return { __n }; }

  template<typename _CharT, typename _Traits>
    inline basic_istream<_CharT, _Traits>& 
    operator>>(basic_istream<_CharT, _Traits>& __is, _Setw __f)
    {
      __is.width(__f._M_n);
      return __is; 
    }

  template<typename _CharT, typename _Traits>
    inline basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _Setw __f)
    {
      __os.width(__f._M_n);
      return __os; 
    }

#if __cplusplus >= 201103L
  
  template<typename _MoneyT>
    struct _Get_money { _MoneyT& _M_mon; bool _M_intl; };

  /**
   *  @brief  Extended manipulator for extracting money.
   *  @param  __mon  Either long double or a specialization of @c basic_string.
   *  @param  __intl A bool indicating whether international format 
   *                 is to be used.
   *
   *  Sent to a stream object, this manipulator extracts @a __mon.
  */
  template<typename _MoneyT>
    inline _Get_money<_MoneyT>
    get_money(_MoneyT& __mon, bool __intl = false)
    { return { __mon, __intl }; }

  template<typename _CharT, typename _Traits, typename _MoneyT>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __is, _Get_money<_MoneyT> __f)
    {
      typename basic_istream<_CharT, _Traits>::sentry __cerb(__is, false);
      if (__cerb)
	{
	  ios_base::iostate __err = ios_base::goodbit;
	  __try
	    {
	      typedef istreambuf_iterator<_CharT, _Traits>   _Iter;
	      typedef money_get<_CharT, _Iter>               _MoneyGet;

	      const _MoneyGet& __mg = use_facet<_MoneyGet>(__is.getloc());
	      __mg.get(_Iter(__is.rdbuf()), _Iter(), __f._M_intl,
		       __is, __err, __f._M_mon);
	    }
	  __catch(__cxxabiv1::__forced_unwind&)
	    {
	      __is._M_setstate(ios_base::badbit);
	      __throw_exception_again;
	    }
	  __catch(...)
	    { __is._M_setstate(ios_base::badbit); }
	  if (__err)
	    __is.setstate(__err);
	}
      return __is; 
    }


  template<typename _MoneyT>
    struct _Put_money { const _MoneyT& _M_mon; bool _M_intl; };

  /**
   *  @brief  Extended manipulator for inserting money.
   *  @param  __mon  Either long double or a specialization of @c basic_string.
   *  @param  __intl A bool indicating whether international format 
   *                 is to be used.
   *
   *  Sent to a stream object, this manipulator inserts @a __mon.
  */
  template<typename _MoneyT>
    inline _Put_money<_MoneyT>
    put_money(const _MoneyT& __mon, bool __intl = false)
    { return { __mon, __intl }; }

  template<typename _CharT, typename _Traits, typename _MoneyT>
    basic_ostream<_CharT, _Traits>& 
    operator<<(basic_ostream<_CharT, _Traits>& __os, _Put_money<_MoneyT> __f)
    {
      typename basic_ostream<_CharT, _Traits>::sentry __cerb(__os);
      if (__cerb)
	{
	  ios_base::iostate __err = ios_base::goodbit;
	  __try
	    {
	      typedef ostreambuf_iterator<_CharT, _Traits>   _Iter;
	      typedef money_put<_CharT, _Iter>               _MoneyPut;

	      const _MoneyPut& __mp = use_facet<_MoneyPut>(__os.getloc());
	      if (__mp.put(_Iter(__os.rdbuf()), __f._M_intl, __os,
			   __os.fill(), __f._M_mon).failed())
		__err |= ios_base::badbit;
	    }
	  __catch(__cxxabiv1::__forced_unwind&)
	    {
	      __os._M_setstate(ios_base::badbit);
	      __throw_exception_again;
	    }
	  __catch(...)
	    { __os._M_setstate(ios_base::badbit); }
	  if (__err)
	    __os.setstate(__err);
	}
      return __os; 
    }

#if __cplusplus > 201103L

#define __cpp_lib_quoted_string_io 201304

_GLIBCXX_END_NAMESPACE_VERSION
  namespace __detail {
  _GLIBCXX_BEGIN_NAMESPACE_VERSION

    /**
     * @brief Struct for delimited strings.
     */
    template<typename _String, typename _CharT>
      struct _Quoted_string
      {
	static_assert(is_reference<_String>::value
		   || is_pointer<_String>::value,
		      "String type must be pointer or reference");

	_Quoted_string(_String __str, _CharT __del, _CharT __esc)
	: _M_string(__str), _M_delim{__del}, _M_escape{__esc}
	{ }

	_Quoted_string&
	operator=(_Quoted_string&) = delete;

	_String _M_string;
	_CharT _M_delim;
	_CharT _M_escape;
      };

    /**
     * @brief Inserter for quoted strings.
     *
     *  _GLIBCXX_RESOLVE_LIB_DEFECTS
     *  DR 2344 quoted()'s interaction with padding is unclear
     */
    template<typename _CharT, typename _Traits>
      auto&
      operator<<(std::basic_ostream<_CharT, _Traits>& __os,
		 const _Quoted_string<const _CharT*, _CharT>& __str)
      {
	std::basic_ostringstream<_CharT, _Traits> __ostr;
	__ostr << __str._M_delim;
	for (const _CharT* __c = __str._M_string; *__c; ++__c)
	  {
	    if (*__c == __str._M_delim || *__c == __str._M_escape)
	      __ostr << __str._M_escape;
	    __ostr << *__c;
	  }
	__ostr << __str._M_delim;

	return __os << __ostr.str();
      }

    /**
     * @brief Inserter for quoted strings.
     *
     *  _GLIBCXX_RESOLVE_LIB_DEFECTS
     *  DR 2344 quoted()'s interaction with padding is unclear
     */
    template<typename _CharT, typename _Traits, typename _String>
      auto&
      operator<<(std::basic_ostream<_CharT, _Traits>& __os,
		 const _Quoted_string<_String, _CharT>& __str)
      {
	std::basic_ostringstream<_CharT, _Traits> __ostr;
	__ostr << __str._M_delim;
	for (auto& __c : __str._M_string)
	  {
	    if (__c == __str._M_delim || __c == __str._M_escape)
	      __ostr << __str._M_escape;
	    __ostr << __c;
	  }
	__ostr << __str._M_delim;

	return __os << __ostr.str();
      }

    /**
     * @brief Extractor for delimited strings.
     *        The left and right delimiters can be different.
     */
    template<typename _CharT, typename _Traits, typename _Alloc>
      auto&
      operator>>(std::basic_istream<_CharT, _Traits>& __is,
		 const _Quoted_string<basic_string<_CharT, _Traits, _Alloc>&,
				      _CharT>& __str)
      {
	_CharT __c;
	__is >> __c;
	if (!__is.good())
	  return __is;
	if (__c != __str._M_delim)
	  {
	    __is.unget();
	    __is >> __str._M_string;
	    return __is;
	  }
	__str._M_string.clear();
	std::ios_base::fmtflags __flags
	  = __is.flags(__is.flags() & ~std::ios_base::skipws);
	do
	  {
	    __is >> __c;
	    if (!__is.good())
	      break;
	    if (__c == __str._M_escape)
	      {
		__is >> __c;
		if (!__is.good())
		  break;
	      }
	    else if (__c == __str._M_delim)
	      break;
	    __str._M_string += __c;
	  }
	while (true);
	__is.setf(__flags);

	return __is;
      }
  _GLIBCXX_END_NAMESPACE_VERSION
  } // namespace __detail
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @brief Manipulator for quoted strings.
   * @param __str    String to quote.
   * @param __delim  Character to quote string with.
   * @param __escape Escape character to escape itself or quote character.
   */
  template<typename _CharT>
    inline auto
    quoted(const _CharT* __string,
	   _CharT __delim = _CharT('"'), _CharT __escape = _CharT('\\'))
    {
      return __detail::_Quoted_string<const _CharT*, _CharT>(__string, __delim,
							     __escape);
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline auto
    quoted(const basic_string<_CharT, _Traits, _Alloc>& __string,
	   _CharT __delim = _CharT('"'), _CharT __escape = _CharT('\\'))
    {
      return __detail::_Quoted_string<
			const basic_string<_CharT, _Traits, _Alloc>&, _CharT>(
				__string, __delim, __escape);
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline auto
    quoted(basic_string<_CharT, _Traits, _Alloc>& __string,
	   _CharT __delim = _CharT('"'), _CharT __escape = _CharT('\\'))
    {
      return __detail::_Quoted_string<
			basic_string<_CharT, _Traits, _Alloc>&, _CharT>(
				__string, __delim, __escape);
    }

#endif // __cplusplus > 201103L

#endif // __cplusplus >= 201103L

  // Inhibit implicit instantiations for required instantiations,
  // which are defined via explicit instantiations elsewhere.  
  // NB:  This syntax is a GNU extension.
#if _GLIBCXX_EXTERN_TEMPLATE
  extern template ostream& operator<<(ostream&, _Setfill<char>);
  extern template ostream& operator<<(ostream&, _Setiosflags);
  extern template ostream& operator<<(ostream&, _Resetiosflags);
  extern template ostream& operator<<(ostream&, _Setbase);
  extern template ostream& operator<<(ostream&, _Setprecision);
  extern template ostream& operator<<(ostream&, _Setw);
  extern template istream& operator>>(istream&, _Setfill<char>);
  extern template istream& operator>>(istream&, _Setiosflags);
  extern template istream& operator>>(istream&, _Resetiosflags);
  extern template istream& operator>>(istream&, _Setbase);
  extern template istream& operator>>(istream&, _Setprecision);
  extern template istream& operator>>(istream&, _Setw);

#ifdef _GLIBCXX_USE_WCHAR_T
  extern template wostream& operator<<(wostream&, _Setfill<wchar_t>);
  extern template wostream& operator<<(wostream&, _Setiosflags);
  extern template wostream& operator<<(wostream&, _Resetiosflags);
  extern template wostream& operator<<(wostream&, _Setbase);
  extern template wostream& operator<<(wostream&, _Setprecision);
  extern template wostream& operator<<(wostream&, _Setw);
  extern template wistream& operator>>(wistream&, _Setfill<wchar_t>);
  extern template wistream& operator>>(wistream&, _Setiosflags);
  extern template wistream& operator>>(wistream&, _Resetiosflags);
  extern template wistream& operator>>(wistream&, _Setbase);
  extern template wistream& operator>>(wistream&, _Setprecision);
  extern template wistream& operator>>(wistream&, _Setw);
#endif
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif /* _GLIBCXX_IOMANIP */

// Standard iostream objects -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/iostream
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.3  Standard iostream objects
//

#ifndef _GLIBCXX_IOSTREAM
#define _GLIBCXX_IOSTREAM 1

#pragma GCC system_header

#include <bits/c++config.h>
#include <ostream>
#include <istream>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  @name Standard Stream Objects
   *
   *  The &lt;iostream&gt; header declares the eight <em>standard stream
   *  objects</em>.  For other declarations, see
   *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/io.html
   *  and the @link iosfwd I/O forward declarations @endlink
   *
   *  They are required by default to cooperate with the global C
   *  library's @c FILE streams, and to be available during program
   *  startup and termination. For more information, see the section of the
   *  manual linked to above.
  */
  //@{
  extern istream cin;		/// Linked to standard input
  extern ostream cout;		/// Linked to standard output
  extern ostream cerr;		/// Linked to standard error (unbuffered)
  extern ostream clog;		/// Linked to standard error (buffered)

#ifdef _GLIBCXX_USE_WCHAR_T
  extern wistream wcin;		/// Linked to standard input
  extern wostream wcout;	/// Linked to standard output
  extern wostream wcerr;	/// Linked to standard error (unbuffered)
  extern wostream wclog;	/// Linked to standard error (buffered)
#endif
  //@}

  // For construction of filebuffers for cout, cin, cerr, clog et. al.
  static ios_base::Init __ioinit;

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif /* _GLIBCXX_IOSTREAM */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#include <crtdefs.h>

#ifndef _INC_LIMITS
#define _INC_LIMITS

/*
 * File system limits
 *
 * NOTE: Apparently the actual size of PATH_MAX is 260, but a space is
 *       required for the NUL. TODO: Test?
 * NOTE: PATH_MAX is the POSIX equivalent for Microsoft's MAX_PATH; the two
 *       are semantically identical, with a limit of 259 characters for the
 *       path name, plus one for a terminating NUL, for a total of 260.
 */
#define PATH_MAX	260

#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 0xff

#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX

#define MB_LEN_MAX 5
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 0xffffU
#define INT_MIN (-2147483647 - 1)
#define INT_MAX 2147483647
#define UINT_MAX 0xffffffffU
#define LONG_MIN (-2147483647L - 1)
#define LONG_MAX 2147483647L
#define ULONG_MAX 0xffffffffUL
#define LLONG_MAX 9223372036854775807ll
#define LLONG_MIN (-9223372036854775807ll - 1)
#define ULLONG_MAX 0xffffffffffffffffull

#define _I8_MIN (-127 - 1)
#define _I8_MAX 127
#define _UI8_MAX 0xffu

#define _I16_MIN (-32767 - 1)
#define _I16_MAX 32767
#define _UI16_MAX 0xffffu

#define _I32_MIN (-2147483647 - 1)
#define _I32_MAX 2147483647
#define _UI32_MAX 0xffffffffu

#if defined(__GNUC__)
#undef LONG_LONG_MAX
#define LONG_LONG_MAX 9223372036854775807ll
#undef LONG_LONG_MIN
#define LONG_LONG_MIN (-LONG_LONG_MAX-1)
#undef ULONG_LONG_MAX
#define ULONG_LONG_MAX (2ull * LONG_LONG_MAX + 1ull)
#endif

#define _I64_MIN (-9223372036854775807ll - 1)
#define _I64_MAX 9223372036854775807ll
#define _UI64_MAX 0xffffffffffffffffull

#ifndef SIZE_MAX
#ifdef _WIN64
#define SIZE_MAX _UI64_MAX
#else
#define SIZE_MAX UINT_MAX
#endif
#endif

#ifndef SSIZE_MAX
#ifdef _WIN64
#define SSIZE_MAX _I64_MAX
#else
#define SSIZE_MAX INT_MAX
#endif
#endif

#ifdef _POSIX_
#define _POSIX_ARG_MAX 4096
#define _POSIX_CHILD_MAX 6
#define _POSIX_LINK_MAX 8
#define _POSIX_MAX_CANON 255
#define _POSIX_MAX_INPUT 255
#define _POSIX_NAME_MAX 14
#define _POSIX_NGROUPS_MAX 0
#define _POSIX_OPEN_MAX 16
#define _POSIX_PATH_MAX 255
#define _POSIX_PIPE_BUF 512
#define _POSIX_SSIZE_MAX 32767
#define _POSIX_STREAM_MAX 8
#define _POSIX_TZNAME_MAX 3
#define ARG_MAX 14500
#define LINK_MAX 1024
#define MAX_CANON _POSIX_MAX_CANON
#define MAX_INPUT _POSIX_MAX_INPUT
#define NAME_MAX 255
#define NGROUPS_MAX 16
#define OPEN_MAX 32
#undef PATH_MAX
#define PATH_MAX 512
#define PIPE_BUF _POSIX_PIPE_BUF
/*#define SSIZE_MAX _POSIX_SSIZE_MAX*/
#define STREAM_MAX 20
#define TZNAME_MAX 10
#endif

#endif /* _INC_LIMITS */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#include <crtdefs.h>

#ifndef _INC_LIMITS
#define _INC_LIMITS

/*
 * File system limits
 *
 * NOTE: Apparently the actual size of PATH_MAX is 260, but a space is
 *       required for the NUL. TODO: Test?
 * NOTE: PATH_MAX is the POSIX equivalent for Microsoft's MAX_PATH; the two
 *       are semantically identical, with a limit of 259 characters for the
 *       path name, plus one for a terminating NUL, for a total of 260.
 */
#define PATH_MAX	260

#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 0xff

#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX

#define MB_LEN_MAX 5
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 0xffffU
#define INT_MIN (-2147483647 - 1)
#define INT_MAX 2147483647
#define UINT_MAX 0xffffffffU
#define LONG_MIN (-2147483647L - 1)
#define LONG_MAX 2147483647L
#define ULONG_MAX 0xffffffffUL
#define LLONG_MAX 9223372036854775807ll
#define LLONG_MIN (-9223372036854775807ll - 1)
#define ULLONG_MAX 0xffffffffffffffffull

#define _I8_MIN (-127 - 1)
#define _I8_MAX 127
#define _UI8_MAX 0xffu

#define _I16_MIN (-32767 - 1)
#define _I16_MAX 32767
#define _UI16_MAX 0xffffu

#define _I32_MIN (-2147483647 - 1)
#define _I32_MAX 2147483647
#define _UI32_MAX 0xffffffffu

#if defined(__GNUC__)
#undef LONG_LONG_MAX
#define LONG_LONG_MAX 9223372036854775807ll
#undef LONG_LONG_MIN
#define LONG_LONG_MIN (-LONG_LONG_MAX-1)
#undef ULONG_LONG_MAX
#define ULONG_LONG_MAX (2ull * LONG_LONG_MAX + 1ull)
#endif

#define _I64_MIN (-9223372036854775807ll - 1)
#define _I64_MAX 9223372036854775807ll
#define _UI64_MAX 0xffffffffffffffffull

#ifndef SIZE_MAX
#ifdef _WIN64
#define SIZE_MAX _UI64_MAX
#else
#define SIZE_MAX UINT_MAX
#endif
#endif

#ifndef SSIZE_MAX
#ifdef _WIN64
#define SSIZE_MAX _I64_MAX
#else
#define SSIZE_MAX INT_MAX
#endif
#endif

#ifdef _POSIX_
#define _POSIX_ARG_MAX 4096
#define _POSIX_CHILD_MAX 6
#define _POSIX_LINK_MAX 8
#define _POSIX_MAX_CANON 255
#define _POSIX_MAX_INPUT 255
#define _POSIX_NAME_MAX 14
#define _POSIX_NGROUPS_MAX 0
#define _POSIX_OPEN_MAX 16
#define _POSIX_PATH_MAX 255
#define _POSIX_PIPE_BUF 512
#define _POSIX_SSIZE_MAX 32767
#define _POSIX_STREAM_MAX 8
#define _POSIX_TZNAME_MAX 3
#define ARG_MAX 14500
#define LINK_MAX 1024
#define MAX_CANON _POSIX_MAX_CANON
#define MAX_INPUT _POSIX_MAX_INPUT
#define NAME_MAX 255
#define NGROUPS_MAX 16
#define OPEN_MAX 32
#undef PATH_MAX
#define PATH_MAX 512
#define PIPE_BUF _POSIX_PIPE_BUF
/*#define SSIZE_MAX _POSIX_SSIZE_MAX*/
#define STREAM_MAX 20
#define TZNAME_MAX 10
#endif

#endif /* _INC_LIMITS */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_LOCALE
#define _INC_LOCALE

#include <crtdefs.h>

#ifdef __cplusplus
#include <stdio.h>
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#define LC_ALL 0
#define LC_COLLATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5

#define LC_MIN LC_ALL
#define LC_MAX LC_TIME

#ifndef _LCONV_DEFINED
#define _LCONV_DEFINED
  struct lconv {
    char *decimal_point;
    char *thousands_sep;
    char *grouping;
    char *int_curr_symbol;
    char *currency_symbol;
    char *mon_decimal_point;
    char *mon_thousands_sep;
    char *mon_grouping;
    char *positive_sign;
    char *negative_sign;
    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
  };
#endif

#ifndef _CONFIG_LOCALE_SWT
#define _CONFIG_LOCALE_SWT

#define _ENABLE_PER_THREAD_LOCALE 0x1
#define _DISABLE_PER_THREAD_LOCALE 0x2
#define _ENABLE_PER_THREAD_LOCALE_GLOBAL 0x10
#define _DISABLE_PER_THREAD_LOCALE_GLOBAL 0x20
#define _ENABLE_PER_THREAD_LOCALE_NEW 0x100
#define _DISABLE_PER_THREAD_LOCALE_NEW 0x200

#endif

  int __cdecl _configthreadlocale(int _Flag);
  char *__cdecl setlocale(int _Category,const char *_Locale);
  _CRTIMP struct lconv *__cdecl localeconv(void);
  _locale_t __cdecl _get_current_locale(void);
  _locale_t __cdecl _create_locale(int _Category,const char *_Locale);
  void __cdecl _free_locale(_locale_t _Locale);
  _locale_t __cdecl __get_current_locale(void);
  _locale_t __cdecl __create_locale(int _Category,const char *_Locale);
  void __cdecl __free_locale(_locale_t _Locale);

#ifndef _WLOCALE_DEFINED
#define _WLOCALE_DEFINED
  _CRTIMP wchar_t *__cdecl _wsetlocale(int _Category,const wchar_t *_Locale);
#endif

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_LOCALE
#define _INC_LOCALE

#include <crtdefs.h>

#ifdef __cplusplus
#include <stdio.h>
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#define LC_ALL 0
#define LC_COLLATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5

#define LC_MIN LC_ALL
#define LC_MAX LC_TIME

#ifndef _LCONV_DEFINED
#define _LCONV_DEFINED
  struct lconv {
    char *decimal_point;
    char *thousands_sep;
    char *grouping;
    char *int_curr_symbol;
    char *currency_symbol;
    char *mon_decimal_point;
    char *mon_thousands_sep;
    char *mon_grouping;
    char *positive_sign;
    char *negative_sign;
    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
  };
#endif

#ifndef _CONFIG_LOCALE_SWT
#define _CONFIG_LOCALE_SWT

#define _ENABLE_PER_THREAD_LOCALE 0x1
#define _DISABLE_PER_THREAD_LOCALE 0x2
#define _ENABLE_PER_THREAD_LOCALE_GLOBAL 0x10
#define _DISABLE_PER_THREAD_LOCALE_GLOBAL 0x20
#define _ENABLE_PER_THREAD_LOCALE_NEW 0x100
#define _DISABLE_PER_THREAD_LOCALE_NEW 0x200

#endif

  int __cdecl _configthreadlocale(int _Flag);
  char *__cdecl setlocale(int _Category,const char *_Locale);
  _CRTIMP struct lconv *__cdecl localeconv(void);
  _locale_t __cdecl _get_current_locale(void);
  _locale_t __cdecl _create_locale(int _Category,const char *_Locale);
  void __cdecl _free_locale(_locale_t _Locale);
  _locale_t __cdecl __get_current_locale(void);
  _locale_t __cdecl __create_locale(int _Category,const char *_Locale);
  void __cdecl __free_locale(_locale_t _Locale);

#ifndef _WLOCALE_DEFINED
#define _WLOCALE_DEFINED
  _CRTIMP wchar_t *__cdecl _wsetlocale(int _Category,const wchar_t *_Locale);
#endif

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _MATH_H_
#define _MATH_H_

#if __GNUC__ >= 3
#pragma GCC system_header
#endif

#include <crtdefs.h>

struct _exception;

#pragma pack(push,_CRT_PACKING)

#define	_DOMAIN		1	/* domain error in argument */
#define	_SING		2	/* singularity */
#define	_OVERFLOW	3	/* range overflow */
#define	_UNDERFLOW	4	/* range underflow */
#define	_TLOSS		5	/* total loss of precision */
#define	_PLOSS		6	/* partial loss of precision */

#ifndef __STRICT_ANSI__
#ifndef	NO_OLDNAMES

#define	DOMAIN		_DOMAIN
#define	SING		_SING
#define	OVERFLOW	_OVERFLOW
#define	UNDERFLOW	_UNDERFLOW
#define	TLOSS		_TLOSS
#define	PLOSS		_PLOSS

#endif
#endif

#if !defined(__STRICT_ANSI__) || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(_USE_MATH_DEFINES)
#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		0.69314718055994530942
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#endif

#ifndef __STRICT_ANSI__
/* See also float.h  */
#ifndef __MINGW_FPCLASS_DEFINED
#define __MINGW_FPCLASS_DEFINED 1
/* IEEE 754 classication */
#define	_FPCLASS_SNAN	0x0001	/* Signaling "Not a Number" */
#define	_FPCLASS_QNAN	0x0002	/* Quiet "Not a Number" */
#define	_FPCLASS_NINF	0x0004	/* Negative Infinity */
#define	_FPCLASS_NN	0x0008	/* Negative Normal */
#define	_FPCLASS_ND	0x0010	/* Negative Denormal */
#define	_FPCLASS_NZ	0x0020	/* Negative Zero */
#define	_FPCLASS_PZ	0x0040	/* Positive Zero */
#define	_FPCLASS_PD	0x0080	/* Positive Denormal */
#define	_FPCLASS_PN	0x0100	/* Positive Normal */
#define	_FPCLASS_PINF	0x0200	/* Positive Infinity */
#endif
#endif

#ifndef RC_INVOKED

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MINGW_SOFTMATH
#define __MINGW_SOFTMATH

/* IEEE float/double type shapes.  */

  typedef union __mingw_dbl_type_t {
    double x;
    unsigned long long val;
    __C89_NAMELESS struct {
      unsigned int low, high;
    } lh;
  } __mingw_dbl_type_t;

  typedef union __mingw_flt_type_t {
    float x;
    unsigned int val;
  } __mingw_flt_type_t;

  typedef union __mingw_ldbl_type_t
  {
    long double x;
    __C89_NAMELESS struct {
      unsigned int low, high;
      int sign_exponent : 16;
      int res1 : 16;
      int res0 : 32;
    } lh;
  } __mingw_ldbl_type_t;

#endif

#ifndef _HUGE
  extern double * __MINGW_IMP_SYMBOL(_HUGE);
#define _HUGE	(* __MINGW_IMP_SYMBOL(_HUGE))
#endif

#if __MINGW_GNUC_PREREQ(3, 3)
#define	HUGE_VAL __builtin_huge_val()
#else
#define HUGE_VAL _HUGE
#endif

#ifndef _EXCEPTION_DEFINED
#define _EXCEPTION_DEFINED
  struct _exception {
    int type;
    const char *name;
    double arg1;
    double arg2;
    double retval;
  };

  void __mingw_raise_matherr (int typ, const char *name, double a1, double a2,
			      double rslt);
  void __mingw_setusermatherr (int (__cdecl *)(struct _exception *));
  _CRTIMP void __setusermatherr(int (__cdecl *)(struct _exception *));
  #define __setusermatherr __mingw_setusermatherr
#endif

  double __cdecl sin(double _X);
  double __cdecl cos(double _X);
  double __cdecl tan(double _X);
  double __cdecl sinh(double _X);
  double __cdecl cosh(double _X);
  double __cdecl tanh(double _X);
  double __cdecl asin(double _X);
  double __cdecl acos(double _X);
  double __cdecl atan(double _X);
  double __cdecl atan2(double _Y,double _X);
  double __cdecl exp(double _X);
  double __cdecl log(double _X);
  double __cdecl log10(double _X);
  double __cdecl pow(double _X,double _Y);
  double __cdecl sqrt(double _X);
  double __cdecl ceil(double _X);
  double __cdecl floor(double _X);

/* 7.12.7.2 The fabs functions: Double in C89 */
  extern  float __cdecl fabsf (float x);
  extern long double __cdecl fabsl (long double);
  extern double __cdecl fabs (double _X);

#ifndef __CRT__NO_INLINE
#if !defined (__ia64__)
  __CRT_INLINE float __cdecl fabsf (float x)
  {
#ifdef __x86_64__
    return __builtin_fabsf (x);
#else
    float res = 0.0F;
    __asm__ __volatile__ ("fabs;" : "=t" (res) : "0" (x));
    return res;
#endif
  }

  __CRT_INLINE long double __cdecl fabsl (long double x)
  {
    long double res = 0.0l;
    __asm__ __volatile__ ("fabs;" : "=t" (res) : "0" (x));
    return res;
  }

  __CRT_INLINE double __cdecl fabs (double x)
  {
#ifdef __x86_64__
    return __builtin_fabs (x);
#else
    double res = 0.0;
    __asm__ __volatile__ ("fabs;" : "=t" (res) : "0" (x));
    return res;
#endif
  }
#endif
#endif

  double __cdecl ldexp(double _X,int _Y);
  double __cdecl frexp(double _X,int *_Y);
  double __cdecl modf(double _X,double *_Y);
  double __cdecl fmod(double _X,double _Y);

  void __cdecl sincos (double __x, double *p_sin, double *p_cos);
  void __cdecl sincosl (long double __x, long double *p_sin, long double *p_cos);
  void __cdecl sincosf (float __x, float *p_sin, float *p_cos);

#ifndef _CRT_ABS_DEFINED
#define _CRT_ABS_DEFINED
  int __cdecl abs(int _X);
  long __cdecl labs(long _X);
#endif
#ifndef _CRT_ATOF_DEFINED
#define _CRT_ATOF_DEFINED
  double __cdecl atof(const char *_String);
  double __cdecl _atof_l(const char *_String,_locale_t _Locale);
#endif

#define EDOM 33
#define ERANGE 34

#ifndef __STRICT_ANSI__

#ifndef _COMPLEX_DEFINED
#define _COMPLEX_DEFINED
  struct _complex {
    double x;
    double y;
  };
#endif

  _CRTIMP double __cdecl _cabs(struct _complex _ComplexA);
  double __cdecl _hypot(double _X,double _Y);
  _CRTIMP double __cdecl _j0(double _X);
  _CRTIMP double __cdecl _j1(double _X);
  _CRTIMP double __cdecl _jn(int _X,double _Y);
  _CRTIMP double __cdecl _y0(double _X);
  _CRTIMP double __cdecl _y1(double _X);
  _CRTIMP double __cdecl _yn(int _X,double _Y);
#ifndef _CRT_MATHERR_DEFINED
#define _CRT_MATHERR_DEFINED
  _CRTIMP int __cdecl _matherr (struct _exception *);
#endif

/* These are also declared in Mingw float.h; needed here as well to work 
   around GCC build issues.  */
/* BEGIN FLOAT.H COPY */
/*
 * IEEE recommended functions
 */
#ifndef _SIGN_DEFINED
#define _SIGN_DEFINED
  _CRTIMP double __cdecl _chgsign (double _X);
  _CRTIMP double __cdecl _copysign (double _Number,double _Sign);
  _CRTIMP double __cdecl _logb (double);
  _CRTIMP double __cdecl _nextafter (double, double);
  _CRTIMP double __cdecl _scalb (double, long);
  _CRTIMP int __cdecl _finite (double);
  _CRTIMP int __cdecl _fpclass (double);
  _CRTIMP int __cdecl _isnan (double);
#endif

/* END FLOAT.H COPY */

#if !defined(NO_OLDNAMES)

_CRTIMP double __cdecl j0 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl j1 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl jn (int, double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl y0 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl y1 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl yn (int, double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;

_CRTIMP double __cdecl chgsign (double);
/*
 * scalb() is a GCC built-in.
 * Exclude this _scalb() stub; the semantics are incompatible
 * with the built-in implementation.
 *
_CRTIMP double __cdecl scalb (double, long);
 *
 */
  _CRTIMP int __cdecl finite (double);
  _CRTIMP int __cdecl fpclass (double);

#define FP_SNAN    _FPCLASS_SNAN
#define FP_QNAN    _FPCLASS_QNAN
#define FP_NINF    _FPCLASS_NINF
#define FP_PINF    _FPCLASS_PINF
#define FP_NDENORM _FPCLASS_ND
#define FP_PDENORM _FPCLASS_PD
#define FP_NZERO   _FPCLASS_NZ
#define FP_PZERO   _FPCLASS_PZ
#define FP_NNORM   _FPCLASS_NN
#define FP_PNORM   _FPCLASS_PN

#endif /* !defined (_NO_OLDNAMES) && !define (NO_OLDNAMES) */

#if(defined(_X86_) && !defined(__x86_64))
  _CRTIMP int __cdecl _set_SSE2_enable(int _Flag);
#endif

#endif

#ifndef __NO_ISOCEXT
#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
	|| !defined __STRICT_ANSI__ || defined __cplusplus

#if __MINGW_GNUC_PREREQ(3, 3)
#define HUGE_VALF	__builtin_huge_valf()
#define HUGE_VALL	__builtin_huge_vall()
#define INFINITY	__builtin_inf()
#define NAN		__builtin_nan("")
#else
extern const float __INFF;
#define HUGE_VALF __INFF
extern const long double  __INFL;
#define HUGE_VALL __INFL
#define INFINITY HUGE_VALF
extern const double __QNAN;
#define NAN __QNAN
#endif /* __MINGW_GNUC_PREREQ(3, 3) */

/* Use the compiler's builtin define for FLT_EVAL_METHOD to
   set float_t and double_t.  */
#if defined (__x86_64__) || defined(__FLT_EVAL_METHOD__)  
# if defined (__x86_64__) || ( __FLT_EVAL_METHOD__== 0)
typedef float float_t;
typedef double double_t;
# elif (__FLT_EVAL_METHOD__ == 1)
typedef double float_t;
typedef double double_t;
# else /* (__FLT_EVAL_METHOD__ == 2) default ix87 FPU */
typedef long double float_t;
typedef long double double_t;
#endif
#else /* ix87 FPU default */
typedef long double float_t;
typedef long double double_t;
#endif

/* 7.12.3.1 */
/*
   Return values for fpclassify.
   These are based on Intel x87 fpu condition codes
   in the high byte of status word and differ from
   the return values for MS IEEE 754 extension _fpclass()
*/
#define FP_NAN		0x0100
#define FP_NORMAL	0x0400
#define FP_INFINITE	(FP_NAN | FP_NORMAL)
#define FP_ZERO		0x4000
#define FP_SUBNORMAL	(FP_NORMAL | FP_ZERO)
/* 0x0200 is signbit mask */

/*
  We can't inline float or double, because we want to ensure truncation
  to semantic type before classification. 
  (A normal long double value might become subnormal when 
  converted to double, and zero when converted to float.)
*/

  extern int __cdecl __fpclassifyl (long double);
  extern int __cdecl __fpclassifyf (float);
  extern int __cdecl __fpclassify (double);

#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl __fpclassifyl (long double x) {
    unsigned short sw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;" : "=a" (sw): "t" (x));
    return sw & (FP_NAN | FP_NORMAL | FP_ZERO );
  }
  __CRT_INLINE int __cdecl __fpclassify (double x) {
#ifdef __x86_64__
    __mingw_dbl_type_t hlp;
    unsigned int l, h;

    hlp.x = x;
    h = hlp.lh.high;
    l = hlp.lh.low | (h & 0xfffff);
    h &= 0x7ff00000;
    if ((h | l) == 0)
      return FP_ZERO;
    if (!h)
      return FP_SUBNORMAL;
    if (h == 0x7ff00000)
      return (l ? FP_NAN : FP_INFINITE);
    return FP_NORMAL;
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;" : "=a" (sw): "t" (x));
    return sw & (FP_NAN | FP_NORMAL | FP_ZERO );
#endif
  }
  __CRT_INLINE int __cdecl __fpclassifyf (float x) {
#ifdef __x86_64__
    __mingw_flt_type_t hlp;

    hlp.x = x;
    hlp.val &= 0x7fffffff;
    if (hlp.val == 0)
      return FP_ZERO;
    if (hlp.val < 0x800000)
      return FP_SUBNORMAL;
    if (hlp.val >= 0x7f800000)
      return (hlp.val > 0x7f800000 ? FP_NAN : FP_INFINITE);
    return FP_NORMAL;
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;" : "=a" (sw): "t" (x));
    return sw & (FP_NAN | FP_NORMAL | FP_ZERO );
#endif
  }
#endif

#define fpclassify(x) (sizeof (x) == sizeof (float) ? __fpclassifyf (x)	  \
  : sizeof (x) == sizeof (double) ? __fpclassify (x) \
  : __fpclassifyl (x))

/* 7.12.3.2 */
#define isfinite(x) ((fpclassify(x) & FP_NAN) == 0)

/* 7.12.3.3 */
#define isinf(x) (fpclassify(x) == FP_INFINITE)

/* 7.12.3.4 */
/* We don't need to worry about truncation here:
   A NaN stays a NaN. */

  extern int __cdecl __isnan (double);
  extern int __cdecl __isnanf (float);
  extern int __cdecl __isnanl (long double);

#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl __isnan (double _x)
  {
#ifdef __x86_64__
    __mingw_dbl_type_t hlp;
    int l, h;

    hlp.x = _x;
    l = hlp.lh.low;
    h = hlp.lh.high & 0x7fffffff;
    h |= (unsigned int) (l | -l) >> 31;
    h = 0x7ff00000 - h;
    return (int) ((unsigned int) h) >> 31;
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam;"
      "fstsw %%ax": "=a" (sw) : "t" (_x));
    return (sw & (FP_NAN | FP_NORMAL | FP_INFINITE | FP_ZERO | FP_SUBNORMAL))
      == FP_NAN;
#endif
  }

  __CRT_INLINE int __cdecl __isnanf (float _x)
  {
#ifdef __x86_64__
    __mingw_flt_type_t hlp;
    int i;
    
    hlp.x = _x;
    i = hlp.val & 0x7fffffff;
    i = 0x7f800000 - i;
    return (int) (((unsigned int) i) >> 31);
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam;"
      "fstsw %%ax": "=a" (sw) : "t" (_x));
    return (sw & (FP_NAN | FP_NORMAL | FP_INFINITE | FP_ZERO | FP_SUBNORMAL))
      == FP_NAN;
#endif
  }

  __CRT_INLINE int __cdecl __isnanl (long double _x)
  {
    unsigned short sw;
    __asm__ __volatile__ ("fxam;"
      "fstsw %%ax": "=a" (sw) : "t" (_x));
    return (sw & (FP_NAN | FP_NORMAL | FP_INFINITE | FP_ZERO | FP_SUBNORMAL))
      == FP_NAN;
  }
#endif

#define isnan(x) (sizeof (x) == sizeof (float) ? __isnanf (x)	\
  : sizeof (x) == sizeof (double) ? __isnan (x)	\
  : __isnanl (x))

/* 7.12.3.5 */
#define isnormal(x) (fpclassify(x) == FP_NORMAL)

/* 7.12.3.6 The signbit macro */
  extern int __cdecl __signbit (double);
  extern int __cdecl __signbitf (float);
  extern int __cdecl __signbitl (long double);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl __signbit (double x) {
#ifdef __x86_64__
    __mingw_dbl_type_t hlp;
    
    hlp.x = x;
    return ((hlp.lh.high & 0x80000000) != 0);
#else
    unsigned short stw;
    __asm__ __volatile__ ( "fxam; fstsw %%ax;": "=a" (stw) : "t" (x));
    return stw & 0x0200;
#endif
  }

  __CRT_INLINE int __cdecl __signbitf (float x) {
#ifdef __x86_64__
    __mingw_flt_type_t hlp;
    hlp.x = x;
    return ((hlp.val & 0x80000000) != 0);
#else
    unsigned short stw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;": "=a" (stw) : "t" (x));
    return stw & 0x0200;
#endif
  }

  __CRT_INLINE int __cdecl __signbitl (long double x) {
    unsigned short stw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;": "=a" (stw) : "t" (x));
    return stw & 0x0200;
  }
#endif

#define signbit(x) (sizeof (x) == sizeof (float) ? __signbitf (x)	\
  : sizeof (x) == sizeof (double) ? __signbit (x)	\
  : __signbitl (x))

/* 7.12.4 Trigonometric functions: Double in C89 */
  extern float __cdecl sinf(float _X);
  extern long double __cdecl sinl(long double);

  extern float __cdecl cosf(float _X);
  extern long double __cdecl cosl(long double);

  extern float __cdecl tanf(float _X);
  extern long double __cdecl tanl(long double);
  extern float __cdecl asinf(float _X);
  extern long double __cdecl asinl(long double);

  extern float __cdecl acosf (float);
  extern long double __cdecl acosl (long double);

  extern float __cdecl atanf (float);
  extern long double __cdecl atanl (long double);

  extern float __cdecl atan2f (float, float);
  extern long double __cdecl atan2l (long double, long double);

/* 7.12.5 Hyperbolic functions: Double in C89  */
  extern float __cdecl sinhf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float sinhf(float _X) { return ((float)sinh((double)_X)); }
#endif
  extern long double __cdecl sinhl(long double);

  extern float __cdecl coshf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float coshf(float _X) { return ((float)cosh((double)_X)); }
#endif
  extern long double __cdecl coshl(long double);

  extern float __cdecl tanhf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float tanhf(float _X) { return ((float)tanh((double)_X)); }
#endif
  extern long double __cdecl tanhl(long double);

/* Inverse hyperbolic trig functions  */ 
/* 7.12.5.1 */
  extern double __cdecl acosh (double);
  extern float __cdecl acoshf (float);
  extern long double __cdecl acoshl (long double);

/* 7.12.5.2 */
  extern double __cdecl asinh (double);
  extern float __cdecl asinhf (float);
  extern long double __cdecl asinhl (long double);

/* 7.12.5.3 */
  extern double __cdecl atanh (double);
  extern float __cdecl atanhf  (float);
  extern long double __cdecl atanhl (long double);

/* Exponentials and logarithms  */
/* 7.12.6.1 Double in C89 */
  extern float __cdecl expf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float expf(float _X) { return ((float)exp((double)_X)); }
#endif
  extern long double __cdecl expl(long double);

/* 7.12.6.2 */
  extern double __cdecl exp2(double);
  extern float __cdecl exp2f(float);
  extern long double __cdecl exp2l(long double);

/* 7.12.6.3 The expm1 functions */
/* TODO: These could be inlined */
  extern double __cdecl expm1(double);
  extern float __cdecl expm1f(float);
  extern long double __cdecl expm1l(long double);

/* 7.12.6.4 Double in C89 */
  extern float frexpf(float _X,int *_Y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float frexpf(float _X,int *_Y) { return ((float)frexp((double)_X,_Y)); }
#endif
  extern long double __cdecl frexpl(long double,int *);

/* 7.12.6.5 */
#define FP_ILOGB0 ((int)0x80000000)
#define FP_ILOGBNAN ((int)0x80000000)
  extern int __cdecl ilogb (double);
  extern int __cdecl ilogbf (float);
  extern int __cdecl ilogbl (long double);

/* 7.12.6.6  Double in C89 */
  extern float __cdecl ldexpf(float _X,int _Y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float __cdecl ldexpf (float x, int expn) { return (float) ldexp ((double)x, expn); }
#endif
  extern long double __cdecl ldexpl (long double, int);

/* 7.12.6.7 Double in C89 */
  extern float __cdecl logf (float);
  extern long double __cdecl logl(long double);

/* 7.12.6.8 Double in C89 */
  extern float __cdecl log10f (float);
  extern long double __cdecl log10l(long double);

/* 7.12.6.9 */
  extern double __cdecl log1p(double);
  extern float __cdecl log1pf(float);
  extern long double __cdecl log1pl(long double);

/* 7.12.6.10 */
  extern double __cdecl log2 (double);
  extern float __cdecl log2f (float);
  extern long double __cdecl log2l (long double);

/* 7.12.6.11 */
  extern double __cdecl logb (double);
  extern float __cdecl logbf (float);
  extern long double __cdecl logbl (long double);

/* Inline versions.  GCC-4.0+ can do a better fast-math optimization
   with __builtins. */
#ifndef __CRT__NO_INLINE
#if !(__MINGW_GNUC_PREREQ (4, 0) && defined (__FAST_MATH__))
  __CRT_INLINE double __cdecl logb (double x)
  {
#ifdef __x86_64__
  __mingw_dbl_type_t hlp;
  int lx, hx;

  hlp.x = x;
  lx = hlp.lh.low;
  hx = hlp.lh.high & 0x7fffffff; /* high |x| */
  if ((hx | lx) == 0)
    return -1.0 / fabs (x);
  if (hx >= 0x7ff00000)
    return x * x;
  if ((hx >>= 20) == 0) /* IEEE 754 logb */
    return -1022.0;
  return (double) (hx - 1023);
#else
    double res = 0.0;
    __asm__ __volatile__ ("fxtract\n\t"
      "fstp	%%st" : "=t" (res) : "0" (x));
    return res;
#endif
  }

  __CRT_INLINE float __cdecl logbf (float x)
  {
#ifdef __x86_64__
    int v;
    __mingw_flt_type_t hlp;

    hlp.x = x;
    v = hlp.val & 0x7fffffff;                     /* high |x| */
    if (!v)
      return (float)-1.0 / fabsf (x);
    if (v >= 0x7f800000)
    return x * x;
  if ((v >>= 23) == 0) /* IEEE 754 logb */
    return -126.0;
  return (float) (v - 127);
#else
    float res = 0.0F;
    __asm__ __volatile__ ("fxtract\n\t"
      "fstp	%%st" : "=t" (res) : "0" (x));
    return res;
#endif
  }

  __CRT_INLINE long double __cdecl logbl (long double x)
  {
    long double res = 0.0l;
    __asm__ __volatile__ ("fxtract\n\t"
      "fstp	%%st" : "=t" (res) : "0" (x));
    return res;
  }
#endif /* !defined __FAST_MATH__ || !__MINGW_GNUC_PREREQ (4, 0) */
#endif /* __CRT__NO_INLINE */

/* 7.12.6.12  Double in C89 */
  extern float __cdecl modff (float, float*);
  extern long double __cdecl modfl (long double, long double*);

/* 7.12.6.13 */
  extern double __cdecl scalbn (double, int);
  extern float __cdecl scalbnf (float, int);
  extern long double __cdecl scalbnl (long double, int);

  extern double __cdecl scalbln (double, long);
  extern float __cdecl scalblnf (float, long);
  extern long double __cdecl scalblnl (long double, long);

/* 7.12.7.1 */
/* Implementations adapted from Cephes versions */ 
  extern double __cdecl cbrt (double);
  extern float __cdecl cbrtf (float);
  extern long double __cdecl cbrtl (long double);

/* 7.12.7.3  */
  extern double __cdecl hypot (double, double) __MINGW_ATTRIB_DEPRECATED_MSVC2005; /* in libmoldname.a */
  extern float __cdecl hypotf (float x, float y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float __cdecl hypotf (float x, float y) { return (float) hypot ((double)x, (double)y);}
#endif
  extern long double __cdecl hypotl (long double, long double);

/* 7.12.7.4 The pow functions. Double in C89 */
  extern float __cdecl powf(float _X,float _Y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float powf(float _X,float _Y) { return ((float)pow((double)_X,(double)_Y)); }
#endif
  extern long double __cdecl powl (long double, long double);

/* 7.12.7.5 The sqrt functions. Double in C89. */
  extern float __cdecl sqrtf (float);
  extern long double sqrtl(long double);

/* 7.12.8.1 The erf functions  */
  extern double __cdecl erf (double);
  extern float __cdecl erff (float);
  extern long double __cdecl erfl (long double);

/* 7.12.8.2 The erfc functions  */
  extern double __cdecl erfc (double);
  extern float __cdecl erfcf (float);
  extern long double __cdecl erfcl (long double);

/* 7.12.8.3 The lgamma functions */
  extern double __cdecl lgamma (double);
  extern float __cdecl lgammaf (float);
  extern long double __cdecl lgammal (long double);

  extern int signgam;

/* 7.12.8.4 The tgamma functions */
  extern double __cdecl tgamma (double);
  extern float __cdecl tgammaf (float);
  extern long double __cdecl tgammal (long double);

/* 7.12.9.1 Double in C89 */
  extern float __cdecl ceilf (float);
  extern long double __cdecl ceill (long double);

/* 7.12.9.2 Double in C89 */
  extern float __cdecl floorf (float);
  extern long double __cdecl floorl (long double);

/* 7.12.9.3 */
  extern double __cdecl nearbyint ( double);
  extern float __cdecl nearbyintf (float);
  extern long double __cdecl nearbyintl (long double);

/* 7.12.9.4 */
/* round, using fpu control word settings */
extern double __cdecl rint (double);
extern float __cdecl rintf (float);
extern long double __cdecl rintl (long double);

/* 7.12.9.5 */
extern long __cdecl lrint (double);
extern long __cdecl lrintf (float);
extern long __cdecl lrintl (long double);

__MINGW_EXTENSION long long __cdecl llrint (double);
__MINGW_EXTENSION long long __cdecl llrintf (float);
__MINGW_EXTENSION long long __cdecl llrintl (long double);

/* Inline versions of above. 
   GCC 4.0+ can do a better fast-math job with __builtins. */

#ifndef __CRT__NO_INLINE
#if !(__MINGW_GNUC_PREREQ (4, 0) && defined __FAST_MATH__ )
  __CRT_INLINE double __cdecl rint (double x)
  {
    double retval = 0.0;
    __asm__ __volatile__ ("frndint;": "=t" (retval) : "0" (x));
    return retval;
  }

  __CRT_INLINE float __cdecl rintf (float x)
  {
    float retval = 0.0;
    __asm__ __volatile__ ("frndint;" : "=t" (retval) : "0" (x) );
    return retval;
  }

  __CRT_INLINE long double __cdecl rintl (long double x)
  {
    long double retval = 0.0l;
    __asm__ __volatile__ ("frndint;" : "=t" (retval) : "0" (x) );
    return retval;
  }

  __CRT_INLINE long __cdecl lrint (double x) 
  {
    long retval = 0;
    __asm__ __volatile__							      \
      ("fistpl %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __CRT_INLINE long __cdecl lrintf (float x) 
  {
    long retval = 0;
    __asm__ __volatile__							      \
      ("fistpl %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __CRT_INLINE long __cdecl lrintl (long double x) 
  {
    long retval = 0;
    __asm__ __volatile__							      \
      ("fistpl %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llrint (double x) 
  {
    __MINGW_EXTENSION long long retval = 0ll;
    __asm__ __volatile__							      \
      ("fistpll %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llrintf (float x) 
  {
    __MINGW_EXTENSION long long retval = 0ll;
    __asm__ __volatile__							      \
      ("fistpll %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llrintl (long double x) 
  {
    __MINGW_EXTENSION long long retval = 0ll;
    __asm__ __volatile__							      \
      ("fistpll %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }
#endif /* !__FAST_MATH__ || !__MINGW_GNUC_PREREQ (4,0)  */
#endif /* !__CRT__NO_INLINE */

/* 7.12.9.6 */
/* round away from zero, regardless of fpu control word settings */
  extern double __cdecl round (double);
  extern float __cdecl roundf (float);
  extern long double __cdecl roundl (long double);

/* 7.12.9.7  */
  extern long __cdecl lround (double);
  extern long __cdecl lroundf (float);
  extern long __cdecl lroundl (long double);
  __MINGW_EXTENSION long long __cdecl llround (double);
  __MINGW_EXTENSION long long __cdecl llroundf (float);
  __MINGW_EXTENSION long long __cdecl llroundl (long double);
  
/* 7.12.9.8 */
/* round towards zero, regardless of fpu control word settings */
  extern double __cdecl trunc (double);
  extern float __cdecl truncf (float);
  extern long double __cdecl truncl (long double);

/* 7.12.10.1 Double in C89 */
  extern float __cdecl fmodf (float, float);
  extern long double __cdecl fmodl (long double, long double);

/* 7.12.10.2 */ 
  extern double __cdecl remainder (double, double);
  extern float __cdecl remainderf (float, float);
  extern long double __cdecl remainderl (long double, long double);

/* 7.12.10.3 */
  extern double __cdecl remquo(double, double, int *);
  extern float __cdecl remquof(float, float, int *);
  extern long double __cdecl remquol(long double, long double, int *);

/* 7.12.11.1 */
  extern double __cdecl copysign (double, double); /* in libmoldname.a */
  extern float __cdecl copysignf (float, float);
  extern long double __cdecl copysignl (long double, long double);

#ifndef __CRT__NO_INLINE
#if !defined (__ia64__)
  __CRT_INLINE double __cdecl copysign (double x, double y)
  {
    __mingw_dbl_type_t hx, hy;
    hx.x = x; hy.x = y;
    hx.lh.high = (hx.lh.high & 0x7fffffff) | (hy.lh.high & 0x80000000);
    return hx.x;
  }
  __CRT_INLINE float __cdecl copysignf (float x, float y)
  {
    __mingw_flt_type_t hx, hy;
    hx.x = x; hy.x = y;
    hx.val = (hx.val & 0x7fffffff) | (hy.val & 0x80000000);
    return hx.x;
  }
#endif
#endif

/* 7.12.11.2 Return a NaN */
  extern double __cdecl nan(const char *tagp);
  extern float __cdecl nanf(const char *tagp);
  extern long double __cdecl nanl(const char *tagp);

#ifndef __STRICT_ANSI__
#define _nan() nan("")
#define _nanf() nanf("")
#define _nanl() nanl("")
#endif

/* 7.12.11.3 */
  extern double __cdecl nextafter (double, double); /* in libmoldname.a */
  extern float __cdecl nextafterf (float, float);
  extern long double __cdecl nextafterl (long double, long double);

/* 7.12.11.4 The nexttoward functions */
  extern double __cdecl nexttoward (double,  long double);
  extern float __cdecl nexttowardf (float,  long double);
  extern long double __cdecl nexttowardl (long double, long double);

/* 7.12.12.1 */
/*  x > y ? (x - y) : 0.0  */
  extern double __cdecl fdim (double x, double y);
  extern float __cdecl fdimf (float x, float y);
  extern long double __cdecl fdiml (long double x, long double y);

/* fmax and fmin.
   NaN arguments are treated as missing data: if one argument is a NaN
   and the other numeric, then these functions choose the numeric
   value. */

/* 7.12.12.2 */
  extern double __cdecl fmax  (double, double);
  extern float __cdecl fmaxf (float, float);
  extern long double __cdecl fmaxl (long double, long double);

/* 7.12.12.3 */
  extern double __cdecl fmin (double, double);
  extern float __cdecl fminf (float, float);
  extern long double __cdecl fminl (long double, long double);

/* 7.12.13.1 */
/* return x * y + z as a ternary op */ 
  extern double __cdecl fma (double, double, double);
  extern float __cdecl fmaf (float, float, float);
  extern long double __cdecl fmal (long double, long double, long double);

/* 7.12.14 */
/* 
 *  With these functions, comparisons involving quiet NaNs set the FP
 *  condition code to "unordered".  The IEEE floating-point spec
 *  dictates that the result of floating-point comparisons should be
 *  false whenever a NaN is involved, with the exception of the != op, 
 *  which always returns true: yes, (NaN != NaN) is true).
 */

#if __GNUC__ >= 3

#define isgreater(x, y) __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isless(x, y) __builtin_isless(x, y)
#define islessequal(x, y) __builtin_islessequal(x, y)
#define islessgreater(x, y) __builtin_islessgreater(x, y)
#define isunordered(x, y) __builtin_isunordered(x, y)

#else
/*  helper  */
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int  __cdecl
    __fp_unordered_compare (long double x, long double y){
      unsigned short retval;
      __asm__ __volatile__ ("fucom %%st(1);"
	"fnstsw;": "=a" (retval) : "t" (x), "u" (y));
      return retval;
  }
#endif

#define isgreater(x, y) ((__fp_unordered_compare(x, y)  & 0x4500) == 0)
#define isless(x, y) ((__fp_unordered_compare (y, x)  & 0x4500) == 0)
#define isgreaterequal(x, y) ((__fp_unordered_compare (x, y)  & FP_INFINITE) == 0)
#define islessequal(x, y) ((__fp_unordered_compare(y, x)  & FP_INFINITE) == 0)
#define islessgreater(x, y) ((__fp_unordered_compare(x, y)  & FP_SUBNORMAL) == 0)
#define isunordered(x, y) ((__fp_unordered_compare(x, y)  & 0x4500) == 0x4500)

#endif

#endif /* __STDC_VERSION__ >= 199901L */
#endif /* __NO_ISOCEXT */

#if defined(_X86_) && !defined(__x86_64)
  _CRTIMP float __cdecl _hypotf(float _X,float _Y);
#endif

#if !defined(__ia64__)
   _CRTIMP float __cdecl _copysignf (float _Number,float _Sign);
   _CRTIMP float __cdecl _chgsignf (float _X);
   _CRTIMP float __cdecl _logbf(float _X);
   _CRTIMP float __cdecl _nextafterf(float _X,float _Y);
   _CRTIMP int __cdecl _finitef(float _X);
   _CRTIMP int __cdecl _isnanf(float _X);
   _CRTIMP int __cdecl _fpclassf(float _X);
#endif

#ifdef _SIGN_DEFINED
   extern long double __cdecl _chgsignl (long double);
#define _copysignl copysignl
#endif /* _SIGN_DEFINED */

#define _hypotl hypotl

#ifndef	NO_OLDNAMES
#define matherr _matherr
#define HUGE	_HUGE
#endif

#ifdef __cplusplus
}
#endif

#endif	/* Not RC_INVOKED */

#pragma pack(pop)

#endif /* End _MATH_H_ */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _MATH_H_
#define _MATH_H_

#if __GNUC__ >= 3
#pragma GCC system_header
#endif

#include <crtdefs.h>

struct _exception;

#pragma pack(push,_CRT_PACKING)

#define	_DOMAIN		1	/* domain error in argument */
#define	_SING		2	/* singularity */
#define	_OVERFLOW	3	/* range overflow */
#define	_UNDERFLOW	4	/* range underflow */
#define	_TLOSS		5	/* total loss of precision */
#define	_PLOSS		6	/* partial loss of precision */

#ifndef __STRICT_ANSI__
#ifndef	NO_OLDNAMES

#define	DOMAIN		_DOMAIN
#define	SING		_SING
#define	OVERFLOW	_OVERFLOW
#define	UNDERFLOW	_UNDERFLOW
#define	TLOSS		_TLOSS
#define	PLOSS		_PLOSS

#endif
#endif

#if !defined(__STRICT_ANSI__) || defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(_USE_MATH_DEFINES)
#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN2		0.69314718055994530942
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440
#endif

#ifndef __STRICT_ANSI__
/* See also float.h  */
#ifndef __MINGW_FPCLASS_DEFINED
#define __MINGW_FPCLASS_DEFINED 1
/* IEEE 754 classication */
#define	_FPCLASS_SNAN	0x0001	/* Signaling "Not a Number" */
#define	_FPCLASS_QNAN	0x0002	/* Quiet "Not a Number" */
#define	_FPCLASS_NINF	0x0004	/* Negative Infinity */
#define	_FPCLASS_NN	0x0008	/* Negative Normal */
#define	_FPCLASS_ND	0x0010	/* Negative Denormal */
#define	_FPCLASS_NZ	0x0020	/* Negative Zero */
#define	_FPCLASS_PZ	0x0040	/* Positive Zero */
#define	_FPCLASS_PD	0x0080	/* Positive Denormal */
#define	_FPCLASS_PN	0x0100	/* Positive Normal */
#define	_FPCLASS_PINF	0x0200	/* Positive Infinity */
#endif
#endif

#ifndef RC_INVOKED

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MINGW_SOFTMATH
#define __MINGW_SOFTMATH

/* IEEE float/double type shapes.  */

  typedef union __mingw_dbl_type_t {
    double x;
    unsigned long long val;
    __C89_NAMELESS struct {
      unsigned int low, high;
    } lh;
  } __mingw_dbl_type_t;

  typedef union __mingw_flt_type_t {
    float x;
    unsigned int val;
  } __mingw_flt_type_t;

  typedef union __mingw_ldbl_type_t
  {
    long double x;
    __C89_NAMELESS struct {
      unsigned int low, high;
      int sign_exponent : 16;
      int res1 : 16;
      int res0 : 32;
    } lh;
  } __mingw_ldbl_type_t;

#endif

#ifndef _HUGE
  extern double * __MINGW_IMP_SYMBOL(_HUGE);
#define _HUGE	(* __MINGW_IMP_SYMBOL(_HUGE))
#endif

#if __MINGW_GNUC_PREREQ(3, 3)
#define	HUGE_VAL __builtin_huge_val()
#else
#define HUGE_VAL _HUGE
#endif

#ifndef _EXCEPTION_DEFINED
#define _EXCEPTION_DEFINED
  struct _exception {
    int type;
    const char *name;
    double arg1;
    double arg2;
    double retval;
  };

  void __mingw_raise_matherr (int typ, const char *name, double a1, double a2,
			      double rslt);
  void __mingw_setusermatherr (int (__cdecl *)(struct _exception *));
  _CRTIMP void __setusermatherr(int (__cdecl *)(struct _exception *));
  #define __setusermatherr __mingw_setusermatherr
#endif

  double __cdecl sin(double _X);
  double __cdecl cos(double _X);
  double __cdecl tan(double _X);
  double __cdecl sinh(double _X);
  double __cdecl cosh(double _X);
  double __cdecl tanh(double _X);
  double __cdecl asin(double _X);
  double __cdecl acos(double _X);
  double __cdecl atan(double _X);
  double __cdecl atan2(double _Y,double _X);
  double __cdecl exp(double _X);
  double __cdecl log(double _X);
  double __cdecl log10(double _X);
  double __cdecl pow(double _X,double _Y);
  double __cdecl sqrt(double _X);
  double __cdecl ceil(double _X);
  double __cdecl floor(double _X);

/* 7.12.7.2 The fabs functions: Double in C89 */
  extern  float __cdecl fabsf (float x);
  extern long double __cdecl fabsl (long double);
  extern double __cdecl fabs (double _X);

#ifndef __CRT__NO_INLINE
#if !defined (__ia64__)
  __CRT_INLINE float __cdecl fabsf (float x)
  {
#ifdef __x86_64__
    return __builtin_fabsf (x);
#else
    float res = 0.0F;
    __asm__ __volatile__ ("fabs;" : "=t" (res) : "0" (x));
    return res;
#endif
  }

  __CRT_INLINE long double __cdecl fabsl (long double x)
  {
    long double res = 0.0l;
    __asm__ __volatile__ ("fabs;" : "=t" (res) : "0" (x));
    return res;
  }

  __CRT_INLINE double __cdecl fabs (double x)
  {
#ifdef __x86_64__
    return __builtin_fabs (x);
#else
    double res = 0.0;
    __asm__ __volatile__ ("fabs;" : "=t" (res) : "0" (x));
    return res;
#endif
  }
#endif
#endif

  double __cdecl ldexp(double _X,int _Y);
  double __cdecl frexp(double _X,int *_Y);
  double __cdecl modf(double _X,double *_Y);
  double __cdecl fmod(double _X,double _Y);

  void __cdecl sincos (double __x, double *p_sin, double *p_cos);
  void __cdecl sincosl (long double __x, long double *p_sin, long double *p_cos);
  void __cdecl sincosf (float __x, float *p_sin, float *p_cos);

#ifndef _CRT_ABS_DEFINED
#define _CRT_ABS_DEFINED
  int __cdecl abs(int _X);
  long __cdecl labs(long _X);
#endif
#ifndef _CRT_ATOF_DEFINED
#define _CRT_ATOF_DEFINED
  double __cdecl atof(const char *_String);
  double __cdecl _atof_l(const char *_String,_locale_t _Locale);
#endif

#define EDOM 33
#define ERANGE 34

#ifndef __STRICT_ANSI__

#ifndef _COMPLEX_DEFINED
#define _COMPLEX_DEFINED
  struct _complex {
    double x;
    double y;
  };
#endif

  _CRTIMP double __cdecl _cabs(struct _complex _ComplexA);
  double __cdecl _hypot(double _X,double _Y);
  _CRTIMP double __cdecl _j0(double _X);
  _CRTIMP double __cdecl _j1(double _X);
  _CRTIMP double __cdecl _jn(int _X,double _Y);
  _CRTIMP double __cdecl _y0(double _X);
  _CRTIMP double __cdecl _y1(double _X);
  _CRTIMP double __cdecl _yn(int _X,double _Y);
#ifndef _CRT_MATHERR_DEFINED
#define _CRT_MATHERR_DEFINED
  _CRTIMP int __cdecl _matherr (struct _exception *);
#endif

/* These are also declared in Mingw float.h; needed here as well to work 
   around GCC build issues.  */
/* BEGIN FLOAT.H COPY */
/*
 * IEEE recommended functions
 */
#ifndef _SIGN_DEFINED
#define _SIGN_DEFINED
  _CRTIMP double __cdecl _chgsign (double _X);
  _CRTIMP double __cdecl _copysign (double _Number,double _Sign);
  _CRTIMP double __cdecl _logb (double);
  _CRTIMP double __cdecl _nextafter (double, double);
  _CRTIMP double __cdecl _scalb (double, long);
  _CRTIMP int __cdecl _finite (double);
  _CRTIMP int __cdecl _fpclass (double);
  _CRTIMP int __cdecl _isnan (double);
#endif

/* END FLOAT.H COPY */

#if !defined(NO_OLDNAMES)

_CRTIMP double __cdecl j0 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl j1 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl jn (int, double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl y0 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl y1 (double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
_CRTIMP double __cdecl yn (int, double) __MINGW_ATTRIB_DEPRECATED_MSVC2005;

_CRTIMP double __cdecl chgsign (double);
/*
 * scalb() is a GCC built-in.
 * Exclude this _scalb() stub; the semantics are incompatible
 * with the built-in implementation.
 *
_CRTIMP double __cdecl scalb (double, long);
 *
 */
  _CRTIMP int __cdecl finite (double);
  _CRTIMP int __cdecl fpclass (double);

#define FP_SNAN    _FPCLASS_SNAN
#define FP_QNAN    _FPCLASS_QNAN
#define FP_NINF    _FPCLASS_NINF
#define FP_PINF    _FPCLASS_PINF
#define FP_NDENORM _FPCLASS_ND
#define FP_PDENORM _FPCLASS_PD
#define FP_NZERO   _FPCLASS_NZ
#define FP_PZERO   _FPCLASS_PZ
#define FP_NNORM   _FPCLASS_NN
#define FP_PNORM   _FPCLASS_PN

#endif /* !defined (_NO_OLDNAMES) && !define (NO_OLDNAMES) */

#if(defined(_X86_) && !defined(__x86_64))
  _CRTIMP int __cdecl _set_SSE2_enable(int _Flag);
#endif

#endif

#ifndef __NO_ISOCEXT
#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
	|| !defined __STRICT_ANSI__ || defined __cplusplus

#if __MINGW_GNUC_PREREQ(3, 3)
#define HUGE_VALF	__builtin_huge_valf()
#define HUGE_VALL	__builtin_huge_vall()
#define INFINITY	__builtin_inf()
#define NAN		__builtin_nan("")
#else
extern const float __INFF;
#define HUGE_VALF __INFF
extern const long double  __INFL;
#define HUGE_VALL __INFL
#define INFINITY HUGE_VALF
extern const double __QNAN;
#define NAN __QNAN
#endif /* __MINGW_GNUC_PREREQ(3, 3) */

/* Use the compiler's builtin define for FLT_EVAL_METHOD to
   set float_t and double_t.  */
#if defined (__x86_64__) || defined(__FLT_EVAL_METHOD__)  
# if defined (__x86_64__) || ( __FLT_EVAL_METHOD__== 0)
typedef float float_t;
typedef double double_t;
# elif (__FLT_EVAL_METHOD__ == 1)
typedef double float_t;
typedef double double_t;
# else /* (__FLT_EVAL_METHOD__ == 2) default ix87 FPU */
typedef long double float_t;
typedef long double double_t;
#endif
#else /* ix87 FPU default */
typedef long double float_t;
typedef long double double_t;
#endif

/* 7.12.3.1 */
/*
   Return values for fpclassify.
   These are based on Intel x87 fpu condition codes
   in the high byte of status word and differ from
   the return values for MS IEEE 754 extension _fpclass()
*/
#define FP_NAN		0x0100
#define FP_NORMAL	0x0400
#define FP_INFINITE	(FP_NAN | FP_NORMAL)
#define FP_ZERO		0x4000
#define FP_SUBNORMAL	(FP_NORMAL | FP_ZERO)
/* 0x0200 is signbit mask */

/*
  We can't inline float or double, because we want to ensure truncation
  to semantic type before classification. 
  (A normal long double value might become subnormal when 
  converted to double, and zero when converted to float.)
*/

  extern int __cdecl __fpclassifyl (long double);
  extern int __cdecl __fpclassifyf (float);
  extern int __cdecl __fpclassify (double);

#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl __fpclassifyl (long double x) {
    unsigned short sw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;" : "=a" (sw): "t" (x));
    return sw & (FP_NAN | FP_NORMAL | FP_ZERO );
  }
  __CRT_INLINE int __cdecl __fpclassify (double x) {
#ifdef __x86_64__
    __mingw_dbl_type_t hlp;
    unsigned int l, h;

    hlp.x = x;
    h = hlp.lh.high;
    l = hlp.lh.low | (h & 0xfffff);
    h &= 0x7ff00000;
    if ((h | l) == 0)
      return FP_ZERO;
    if (!h)
      return FP_SUBNORMAL;
    if (h == 0x7ff00000)
      return (l ? FP_NAN : FP_INFINITE);
    return FP_NORMAL;
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;" : "=a" (sw): "t" (x));
    return sw & (FP_NAN | FP_NORMAL | FP_ZERO );
#endif
  }
  __CRT_INLINE int __cdecl __fpclassifyf (float x) {
#ifdef __x86_64__
    __mingw_flt_type_t hlp;

    hlp.x = x;
    hlp.val &= 0x7fffffff;
    if (hlp.val == 0)
      return FP_ZERO;
    if (hlp.val < 0x800000)
      return FP_SUBNORMAL;
    if (hlp.val >= 0x7f800000)
      return (hlp.val > 0x7f800000 ? FP_NAN : FP_INFINITE);
    return FP_NORMAL;
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;" : "=a" (sw): "t" (x));
    return sw & (FP_NAN | FP_NORMAL | FP_ZERO );
#endif
  }
#endif

#define fpclassify(x) (sizeof (x) == sizeof (float) ? __fpclassifyf (x)	  \
  : sizeof (x) == sizeof (double) ? __fpclassify (x) \
  : __fpclassifyl (x))

/* 7.12.3.2 */
#define isfinite(x) ((fpclassify(x) & FP_NAN) == 0)

/* 7.12.3.3 */
#define isinf(x) (fpclassify(x) == FP_INFINITE)

/* 7.12.3.4 */
/* We don't need to worry about truncation here:
   A NaN stays a NaN. */

  extern int __cdecl __isnan (double);
  extern int __cdecl __isnanf (float);
  extern int __cdecl __isnanl (long double);

#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl __isnan (double _x)
  {
#ifdef __x86_64__
    __mingw_dbl_type_t hlp;
    int l, h;

    hlp.x = _x;
    l = hlp.lh.low;
    h = hlp.lh.high & 0x7fffffff;
    h |= (unsigned int) (l | -l) >> 31;
    h = 0x7ff00000 - h;
    return (int) ((unsigned int) h) >> 31;
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam;"
      "fstsw %%ax": "=a" (sw) : "t" (_x));
    return (sw & (FP_NAN | FP_NORMAL | FP_INFINITE | FP_ZERO | FP_SUBNORMAL))
      == FP_NAN;
#endif
  }

  __CRT_INLINE int __cdecl __isnanf (float _x)
  {
#ifdef __x86_64__
    __mingw_flt_type_t hlp;
    int i;
    
    hlp.x = _x;
    i = hlp.val & 0x7fffffff;
    i = 0x7f800000 - i;
    return (int) (((unsigned int) i) >> 31);
#else
    unsigned short sw;
    __asm__ __volatile__ ("fxam;"
      "fstsw %%ax": "=a" (sw) : "t" (_x));
    return (sw & (FP_NAN | FP_NORMAL | FP_INFINITE | FP_ZERO | FP_SUBNORMAL))
      == FP_NAN;
#endif
  }

  __CRT_INLINE int __cdecl __isnanl (long double _x)
  {
    unsigned short sw;
    __asm__ __volatile__ ("fxam;"
      "fstsw %%ax": "=a" (sw) : "t" (_x));
    return (sw & (FP_NAN | FP_NORMAL | FP_INFINITE | FP_ZERO | FP_SUBNORMAL))
      == FP_NAN;
  }
#endif

#define isnan(x) (sizeof (x) == sizeof (float) ? __isnanf (x)	\
  : sizeof (x) == sizeof (double) ? __isnan (x)	\
  : __isnanl (x))

/* 7.12.3.5 */
#define isnormal(x) (fpclassify(x) == FP_NORMAL)

/* 7.12.3.6 The signbit macro */
  extern int __cdecl __signbit (double);
  extern int __cdecl __signbitf (float);
  extern int __cdecl __signbitl (long double);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl __signbit (double x) {
#ifdef __x86_64__
    __mingw_dbl_type_t hlp;
    
    hlp.x = x;
    return ((hlp.lh.high & 0x80000000) != 0);
#else
    unsigned short stw;
    __asm__ __volatile__ ( "fxam; fstsw %%ax;": "=a" (stw) : "t" (x));
    return stw & 0x0200;
#endif
  }

  __CRT_INLINE int __cdecl __signbitf (float x) {
#ifdef __x86_64__
    __mingw_flt_type_t hlp;
    hlp.x = x;
    return ((hlp.val & 0x80000000) != 0);
#else
    unsigned short stw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;": "=a" (stw) : "t" (x));
    return stw & 0x0200;
#endif
  }

  __CRT_INLINE int __cdecl __signbitl (long double x) {
    unsigned short stw;
    __asm__ __volatile__ ("fxam; fstsw %%ax;": "=a" (stw) : "t" (x));
    return stw & 0x0200;
  }
#endif

#define signbit(x) (sizeof (x) == sizeof (float) ? __signbitf (x)	\
  : sizeof (x) == sizeof (double) ? __signbit (x)	\
  : __signbitl (x))

/* 7.12.4 Trigonometric functions: Double in C89 */
  extern float __cdecl sinf(float _X);
  extern long double __cdecl sinl(long double);

  extern float __cdecl cosf(float _X);
  extern long double __cdecl cosl(long double);

  extern float __cdecl tanf(float _X);
  extern long double __cdecl tanl(long double);
  extern float __cdecl asinf(float _X);
  extern long double __cdecl asinl(long double);

  extern float __cdecl acosf (float);
  extern long double __cdecl acosl (long double);

  extern float __cdecl atanf (float);
  extern long double __cdecl atanl (long double);

  extern float __cdecl atan2f (float, float);
  extern long double __cdecl atan2l (long double, long double);

/* 7.12.5 Hyperbolic functions: Double in C89  */
  extern float __cdecl sinhf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float sinhf(float _X) { return ((float)sinh((double)_X)); }
#endif
  extern long double __cdecl sinhl(long double);

  extern float __cdecl coshf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float coshf(float _X) { return ((float)cosh((double)_X)); }
#endif
  extern long double __cdecl coshl(long double);

  extern float __cdecl tanhf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float tanhf(float _X) { return ((float)tanh((double)_X)); }
#endif
  extern long double __cdecl tanhl(long double);

/* Inverse hyperbolic trig functions  */ 
/* 7.12.5.1 */
  extern double __cdecl acosh (double);
  extern float __cdecl acoshf (float);
  extern long double __cdecl acoshl (long double);

/* 7.12.5.2 */
  extern double __cdecl asinh (double);
  extern float __cdecl asinhf (float);
  extern long double __cdecl asinhl (long double);

/* 7.12.5.3 */
  extern double __cdecl atanh (double);
  extern float __cdecl atanhf  (float);
  extern long double __cdecl atanhl (long double);

/* Exponentials and logarithms  */
/* 7.12.6.1 Double in C89 */
  extern float __cdecl expf(float _X);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float expf(float _X) { return ((float)exp((double)_X)); }
#endif
  extern long double __cdecl expl(long double);

/* 7.12.6.2 */
  extern double __cdecl exp2(double);
  extern float __cdecl exp2f(float);
  extern long double __cdecl exp2l(long double);

/* 7.12.6.3 The expm1 functions */
/* TODO: These could be inlined */
  extern double __cdecl expm1(double);
  extern float __cdecl expm1f(float);
  extern long double __cdecl expm1l(long double);

/* 7.12.6.4 Double in C89 */
  extern float frexpf(float _X,int *_Y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float frexpf(float _X,int *_Y) { return ((float)frexp((double)_X,_Y)); }
#endif
  extern long double __cdecl frexpl(long double,int *);

/* 7.12.6.5 */
#define FP_ILOGB0 ((int)0x80000000)
#define FP_ILOGBNAN ((int)0x80000000)
  extern int __cdecl ilogb (double);
  extern int __cdecl ilogbf (float);
  extern int __cdecl ilogbl (long double);

/* 7.12.6.6  Double in C89 */
  extern float __cdecl ldexpf(float _X,int _Y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float __cdecl ldexpf (float x, int expn) { return (float) ldexp ((double)x, expn); }
#endif
  extern long double __cdecl ldexpl (long double, int);

/* 7.12.6.7 Double in C89 */
  extern float __cdecl logf (float);
  extern long double __cdecl logl(long double);

/* 7.12.6.8 Double in C89 */
  extern float __cdecl log10f (float);
  extern long double __cdecl log10l(long double);

/* 7.12.6.9 */
  extern double __cdecl log1p(double);
  extern float __cdecl log1pf(float);
  extern long double __cdecl log1pl(long double);

/* 7.12.6.10 */
  extern double __cdecl log2 (double);
  extern float __cdecl log2f (float);
  extern long double __cdecl log2l (long double);

/* 7.12.6.11 */
  extern double __cdecl logb (double);
  extern float __cdecl logbf (float);
  extern long double __cdecl logbl (long double);

/* Inline versions.  GCC-4.0+ can do a better fast-math optimization
   with __builtins. */
#ifndef __CRT__NO_INLINE
#if !(__MINGW_GNUC_PREREQ (4, 0) && defined (__FAST_MATH__))
  __CRT_INLINE double __cdecl logb (double x)
  {
#ifdef __x86_64__
  __mingw_dbl_type_t hlp;
  int lx, hx;

  hlp.x = x;
  lx = hlp.lh.low;
  hx = hlp.lh.high & 0x7fffffff; /* high |x| */
  if ((hx | lx) == 0)
    return -1.0 / fabs (x);
  if (hx >= 0x7ff00000)
    return x * x;
  if ((hx >>= 20) == 0) /* IEEE 754 logb */
    return -1022.0;
  return (double) (hx - 1023);
#else
    double res = 0.0;
    __asm__ __volatile__ ("fxtract\n\t"
      "fstp	%%st" : "=t" (res) : "0" (x));
    return res;
#endif
  }

  __CRT_INLINE float __cdecl logbf (float x)
  {
#ifdef __x86_64__
    int v;
    __mingw_flt_type_t hlp;

    hlp.x = x;
    v = hlp.val & 0x7fffffff;                     /* high |x| */
    if (!v)
      return (float)-1.0 / fabsf (x);
    if (v >= 0x7f800000)
    return x * x;
  if ((v >>= 23) == 0) /* IEEE 754 logb */
    return -126.0;
  return (float) (v - 127);
#else
    float res = 0.0F;
    __asm__ __volatile__ ("fxtract\n\t"
      "fstp	%%st" : "=t" (res) : "0" (x));
    return res;
#endif
  }

  __CRT_INLINE long double __cdecl logbl (long double x)
  {
    long double res = 0.0l;
    __asm__ __volatile__ ("fxtract\n\t"
      "fstp	%%st" : "=t" (res) : "0" (x));
    return res;
  }
#endif /* !defined __FAST_MATH__ || !__MINGW_GNUC_PREREQ (4, 0) */
#endif /* __CRT__NO_INLINE */

/* 7.12.6.12  Double in C89 */
  extern float __cdecl modff (float, float*);
  extern long double __cdecl modfl (long double, long double*);

/* 7.12.6.13 */
  extern double __cdecl scalbn (double, int);
  extern float __cdecl scalbnf (float, int);
  extern long double __cdecl scalbnl (long double, int);

  extern double __cdecl scalbln (double, long);
  extern float __cdecl scalblnf (float, long);
  extern long double __cdecl scalblnl (long double, long);

/* 7.12.7.1 */
/* Implementations adapted from Cephes versions */ 
  extern double __cdecl cbrt (double);
  extern float __cdecl cbrtf (float);
  extern long double __cdecl cbrtl (long double);

/* 7.12.7.3  */
  extern double __cdecl hypot (double, double) __MINGW_ATTRIB_DEPRECATED_MSVC2005; /* in libmoldname.a */
  extern float __cdecl hypotf (float x, float y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float __cdecl hypotf (float x, float y) { return (float) hypot ((double)x, (double)y);}
#endif
  extern long double __cdecl hypotl (long double, long double);

/* 7.12.7.4 The pow functions. Double in C89 */
  extern float __cdecl powf(float _X,float _Y);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE float powf(float _X,float _Y) { return ((float)pow((double)_X,(double)_Y)); }
#endif
  extern long double __cdecl powl (long double, long double);

/* 7.12.7.5 The sqrt functions. Double in C89. */
  extern float __cdecl sqrtf (float);
  extern long double sqrtl(long double);

/* 7.12.8.1 The erf functions  */
  extern double __cdecl erf (double);
  extern float __cdecl erff (float);
  extern long double __cdecl erfl (long double);

/* 7.12.8.2 The erfc functions  */
  extern double __cdecl erfc (double);
  extern float __cdecl erfcf (float);
  extern long double __cdecl erfcl (long double);

/* 7.12.8.3 The lgamma functions */
  extern double __cdecl lgamma (double);
  extern float __cdecl lgammaf (float);
  extern long double __cdecl lgammal (long double);

  extern int signgam;

/* 7.12.8.4 The tgamma functions */
  extern double __cdecl tgamma (double);
  extern float __cdecl tgammaf (float);
  extern long double __cdecl tgammal (long double);

/* 7.12.9.1 Double in C89 */
  extern float __cdecl ceilf (float);
  extern long double __cdecl ceill (long double);

/* 7.12.9.2 Double in C89 */
  extern float __cdecl floorf (float);
  extern long double __cdecl floorl (long double);

/* 7.12.9.3 */
  extern double __cdecl nearbyint ( double);
  extern float __cdecl nearbyintf (float);
  extern long double __cdecl nearbyintl (long double);

/* 7.12.9.4 */
/* round, using fpu control word settings */
extern double __cdecl rint (double);
extern float __cdecl rintf (float);
extern long double __cdecl rintl (long double);

/* 7.12.9.5 */
extern long __cdecl lrint (double);
extern long __cdecl lrintf (float);
extern long __cdecl lrintl (long double);

__MINGW_EXTENSION long long __cdecl llrint (double);
__MINGW_EXTENSION long long __cdecl llrintf (float);
__MINGW_EXTENSION long long __cdecl llrintl (long double);

/* Inline versions of above. 
   GCC 4.0+ can do a better fast-math job with __builtins. */

#ifndef __CRT__NO_INLINE
#if !(__MINGW_GNUC_PREREQ (4, 0) && defined __FAST_MATH__ )
  __CRT_INLINE double __cdecl rint (double x)
  {
    double retval = 0.0;
    __asm__ __volatile__ ("frndint;": "=t" (retval) : "0" (x));
    return retval;
  }

  __CRT_INLINE float __cdecl rintf (float x)
  {
    float retval = 0.0;
    __asm__ __volatile__ ("frndint;" : "=t" (retval) : "0" (x) );
    return retval;
  }

  __CRT_INLINE long double __cdecl rintl (long double x)
  {
    long double retval = 0.0l;
    __asm__ __volatile__ ("frndint;" : "=t" (retval) : "0" (x) );
    return retval;
  }

  __CRT_INLINE long __cdecl lrint (double x) 
  {
    long retval = 0;
    __asm__ __volatile__							      \
      ("fistpl %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __CRT_INLINE long __cdecl lrintf (float x) 
  {
    long retval = 0;
    __asm__ __volatile__							      \
      ("fistpl %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __CRT_INLINE long __cdecl lrintl (long double x) 
  {
    long retval = 0;
    __asm__ __volatile__							      \
      ("fistpl %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llrint (double x) 
  {
    __MINGW_EXTENSION long long retval = 0ll;
    __asm__ __volatile__							      \
      ("fistpll %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llrintf (float x) 
  {
    __MINGW_EXTENSION long long retval = 0ll;
    __asm__ __volatile__							      \
      ("fistpll %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }

  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llrintl (long double x) 
  {
    __MINGW_EXTENSION long long retval = 0ll;
    __asm__ __volatile__							      \
      ("fistpll %0"  : "=m" (retval) : "t" (x) : "st");				      \
      return retval;
  }
#endif /* !__FAST_MATH__ || !__MINGW_GNUC_PREREQ (4,0)  */
#endif /* !__CRT__NO_INLINE */

/* 7.12.9.6 */
/* round away from zero, regardless of fpu control word settings */
  extern double __cdecl round (double);
  extern float __cdecl roundf (float);
  extern long double __cdecl roundl (long double);

/* 7.12.9.7  */
  extern long __cdecl lround (double);
  extern long __cdecl lroundf (float);
  extern long __cdecl lroundl (long double);
  __MINGW_EXTENSION long long __cdecl llround (double);
  __MINGW_EXTENSION long long __cdecl llroundf (float);
  __MINGW_EXTENSION long long __cdecl llroundl (long double);
  
/* 7.12.9.8 */
/* round towards zero, regardless of fpu control word settings */
  extern double __cdecl trunc (double);
  extern float __cdecl truncf (float);
  extern long double __cdecl truncl (long double);

/* 7.12.10.1 Double in C89 */
  extern float __cdecl fmodf (float, float);
  extern long double __cdecl fmodl (long double, long double);

/* 7.12.10.2 */ 
  extern double __cdecl remainder (double, double);
  extern float __cdecl remainderf (float, float);
  extern long double __cdecl remainderl (long double, long double);

/* 7.12.10.3 */
  extern double __cdecl remquo(double, double, int *);
  extern float __cdecl remquof(float, float, int *);
  extern long double __cdecl remquol(long double, long double, int *);

/* 7.12.11.1 */
  extern double __cdecl copysign (double, double); /* in libmoldname.a */
  extern float __cdecl copysignf (float, float);
  extern long double __cdecl copysignl (long double, long double);

#ifndef __CRT__NO_INLINE
#if !defined (__ia64__)
  __CRT_INLINE double __cdecl copysign (double x, double y)
  {
    __mingw_dbl_type_t hx, hy;
    hx.x = x; hy.x = y;
    hx.lh.high = (hx.lh.high & 0x7fffffff) | (hy.lh.high & 0x80000000);
    return hx.x;
  }
  __CRT_INLINE float __cdecl copysignf (float x, float y)
  {
    __mingw_flt_type_t hx, hy;
    hx.x = x; hy.x = y;
    hx.val = (hx.val & 0x7fffffff) | (hy.val & 0x80000000);
    return hx.x;
  }
#endif
#endif

/* 7.12.11.2 Return a NaN */
  extern double __cdecl nan(const char *tagp);
  extern float __cdecl nanf(const char *tagp);
  extern long double __cdecl nanl(const char *tagp);

#ifndef __STRICT_ANSI__
#define _nan() nan("")
#define _nanf() nanf("")
#define _nanl() nanl("")
#endif

/* 7.12.11.3 */
  extern double __cdecl nextafter (double, double); /* in libmoldname.a */
  extern float __cdecl nextafterf (float, float);
  extern long double __cdecl nextafterl (long double, long double);

/* 7.12.11.4 The nexttoward functions */
  extern double __cdecl nexttoward (double,  long double);
  extern float __cdecl nexttowardf (float,  long double);
  extern long double __cdecl nexttowardl (long double, long double);

/* 7.12.12.1 */
/*  x > y ? (x - y) : 0.0  */
  extern double __cdecl fdim (double x, double y);
  extern float __cdecl fdimf (float x, float y);
  extern long double __cdecl fdiml (long double x, long double y);

/* fmax and fmin.
   NaN arguments are treated as missing data: if one argument is a NaN
   and the other numeric, then these functions choose the numeric
   value. */

/* 7.12.12.2 */
  extern double __cdecl fmax  (double, double);
  extern float __cdecl fmaxf (float, float);
  extern long double __cdecl fmaxl (long double, long double);

/* 7.12.12.3 */
  extern double __cdecl fmin (double, double);
  extern float __cdecl fminf (float, float);
  extern long double __cdecl fminl (long double, long double);

/* 7.12.13.1 */
/* return x * y + z as a ternary op */ 
  extern double __cdecl fma (double, double, double);
  extern float __cdecl fmaf (float, float, float);
  extern long double __cdecl fmal (long double, long double, long double);

/* 7.12.14 */
/* 
 *  With these functions, comparisons involving quiet NaNs set the FP
 *  condition code to "unordered".  The IEEE floating-point spec
 *  dictates that the result of floating-point comparisons should be
 *  false whenever a NaN is involved, with the exception of the != op, 
 *  which always returns true: yes, (NaN != NaN) is true).
 */

#if __GNUC__ >= 3

#define isgreater(x, y) __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isless(x, y) __builtin_isless(x, y)
#define islessequal(x, y) __builtin_islessequal(x, y)
#define islessgreater(x, y) __builtin_islessgreater(x, y)
#define isunordered(x, y) __builtin_isunordered(x, y)

#else
/*  helper  */
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int  __cdecl
    __fp_unordered_compare (long double x, long double y){
      unsigned short retval;
      __asm__ __volatile__ ("fucom %%st(1);"
	"fnstsw;": "=a" (retval) : "t" (x), "u" (y));
      return retval;
  }
#endif

#define isgreater(x, y) ((__fp_unordered_compare(x, y)  & 0x4500) == 0)
#define isless(x, y) ((__fp_unordered_compare (y, x)  & 0x4500) == 0)
#define isgreaterequal(x, y) ((__fp_unordered_compare (x, y)  & FP_INFINITE) == 0)
#define islessequal(x, y) ((__fp_unordered_compare(y, x)  & FP_INFINITE) == 0)
#define islessgreater(x, y) ((__fp_unordered_compare(x, y)  & FP_SUBNORMAL) == 0)
#define isunordered(x, y) ((__fp_unordered_compare(x, y)  & 0x4500) == 0x4500)

#endif

#endif /* __STDC_VERSION__ >= 199901L */
#endif /* __NO_ISOCEXT */

#if defined(_X86_) && !defined(__x86_64)
  _CRTIMP float __cdecl _hypotf(float _X,float _Y);
#endif

#if !defined(__ia64__)
   _CRTIMP float __cdecl _copysignf (float _Number,float _Sign);
   _CRTIMP float __cdecl _chgsignf (float _X);
   _CRTIMP float __cdecl _logbf(float _X);
   _CRTIMP float __cdecl _nextafterf(float _X,float _Y);
   _CRTIMP int __cdecl _finitef(float _X);
   _CRTIMP int __cdecl _isnanf(float _X);
   _CRTIMP int __cdecl _fpclassf(float _X);
#endif

#ifdef _SIGN_DEFINED
   extern long double __cdecl _chgsignl (long double);
#define _copysignl copysignl
#endif /* _SIGN_DEFINED */

#define _hypotl hypotl

#ifndef	NO_OLDNAMES
#define matherr _matherr
#define HUGE	_HUGE
#endif

#ifdef __cplusplus
}
#endif

#endif	/* Not RC_INVOKED */

#pragma pack(pop)

#endif /* End _MATH_H_ */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_STDIO
#define _INC_STDIO

#include <crtdefs.h>

#include <_mingw_print_push.h>

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#define BUFSIZ 512
#define _NFILE _NSTREAM_
#define _NSTREAM_ 512
#define _IOB_ENTRIES 20
#define EOF (-1)

#ifndef _FILE_DEFINED
  struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
  };
  typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif

#ifdef _POSIX_
#define _P_tmpdir "/"
#define _wP_tmpdir L"/"
#else
#define _P_tmpdir "\\"
#define _wP_tmpdir L"\\"
#endif

#define L_tmpnam (sizeof(_P_tmpdir) + 12)

#ifdef _POSIX_
#define L_ctermid 9
#define L_cuserid 32
#endif

#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

#define FILENAME_MAX 260
#define FOPEN_MAX 20
#define _SYS_OPEN 20
#define TMP_MAX 32767

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#include <_mingw_off_t.h>

#ifndef _STDIO_DEFINED
#ifdef _WIN64
  _CRTIMP FILE *__cdecl __iob_func(void);
#define _iob  __iob_func()
#else
#ifdef _MSVCRT_
extern FILE _iob[];	/* A pointer to an array of FILE */
#define __iob_func()	(_iob)
#else
extern FILE (* __MINGW_IMP_SYMBOL(_iob))[];	/* A pointer to an array of FILE */
#define __iob_func()	(* __MINGW_IMP_SYMBOL(_iob))
#define _iob __iob_func()
#endif
#endif
#endif

#ifndef _FPOS_T_DEFINED
#define _FPOS_T_DEFINED
#undef _FPOSOFF

#if (!defined(NO_OLDNAMES) || defined(__GNUC__))
  __MINGW_EXTENSION typedef __int64 fpos_t;
#define _FPOSOFF(fp) ((long)(fp))
#else
  __MINGW_EXTENSION typedef long long fpos_t;
#define _FPOSOFF(fp) ((long)(fp))
#endif

#endif

#ifndef _STDSTREAM_DEFINED
#define _STDSTREAM_DEFINED

#define stdin (&__iob_func()[0])
#define stdout (&__iob_func()[1])
#define stderr (&__iob_func()[2])
#endif

#define _IOREAD 0x0001
#define _IOWRT 0x0002

#define _IOFBF 0x0000
#define _IOLBF 0x0040
#define _IONBF 0x0004

#define _IOMYBUF 0x0008
#define _IOEOF 0x0010
#define _IOERR 0x0020
#define _IOSTRG 0x0040
#define _IORW 0x0080
#ifdef _POSIX_
#define _IOAPPEND 0x0200
#endif

#define _TWO_DIGIT_EXPONENT 0x1

#ifndef _STDIO_DEFINED
extern
  __attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vsscanf (const char * __restrict__ _Str,const char * __restrict__ Format,va_list argp);
extern
  __attribute__((__format__ (gnu_scanf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_scanf(const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vscanf(const char * __restrict__ Format, va_list argp);
extern
  __attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfscanf (FILE * __restrict__ fp, const char * __restrict__ Format,va_list argp);

extern
  __attribute__((__format__ (gnu_printf, 3, 0))) __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_vsnprintf(char * __restrict__ _DstBuf,size_t _MaxCount,const char * __restrict__ _Format,
                               va_list _ArgList);
extern
  __attribute__((__format__ (gnu_printf, 3, 4))) __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_snprintf(char * __restrict__ s, size_t n, const char * __restrict__  format, ...);
extern
  __attribute__((__format__ (gnu_printf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_printf(const char * __restrict__ , ... ) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vprintf (const char * __restrict__ , va_list) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fprintf (FILE * __restrict__ , const char * __restrict__ , ...) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfprintf (FILE * __restrict__ , const char * __restrict__ , va_list) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_sprintf (char * __restrict__ , const char * __restrict__ , ...) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vsprintf (char * __restrict__ , const char * __restrict__ , va_list) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __attribute__((nonnull (1,2)))
  int __cdecl __mingw_asprintf(char ** __restrict__ , const char * __restrict__ , ...) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __attribute__((nonnull (1,2)))
  int __cdecl __mingw_vasprintf(char ** __restrict__ , const char * __restrict__ , va_list) __MINGW_NOTHROW;

#if __USE_MINGW_ANSI_STDIO
/*
 * User has expressed a preference for C99 conformance...
 */
#undef __MINGW_PRINTF_FORMAT
#undef __MINGW_SCANF_FORMAT
#define __MINGW_PRINTF_FORMAT gnu_printf
#define __MINGW_SCANF_FORMAT  gnu_scanf

#ifdef _GNU_SOURCE
__mingw_ovr
__attribute__ ((__format__ (gnu_printf, 2, 3))) __attribute__((nonnull (1,2)))
int asprintf(char **__ret, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vasprintf( __ret, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__ ((__format__ (gnu_printf, 2, 0))) __attribute__((nonnull (1,2)))
int vasprintf(char **__ret, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vasprintf( __ret, __format, __local_argv );
}
#endif /* _GNU_SOURCE */

/* There seems to be a bug about builtins and static overrides of them
   in g++.  So we need to do here some trickery.  */
#ifdef __cplusplus
extern "C++" {
#endif

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int sscanf(const char *__source, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
int scanf(const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vscanf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int fscanf(FILE *__stream, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vsscanf (const char *__source, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsscanf( __source, __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
int vscanf(const char *__format,  __builtin_va_list __local_argv)
{
  return __mingw_vscanf( __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vfscanf (FILE *__stream,  const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfscanf( __stream, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */



__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int fprintf (FILE *__stream, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
int printf (const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vprintf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int sprintf (char *__stream, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vfprintf (FILE *__stream, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfprintf( __stream, __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
int vprintf (const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vprintf( __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vsprintf (char *__stream, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsprintf( __stream, __format, __local_argv );
}
/* #ifndef __NO_ISOCEXT */  /* externs in libmingwex.a */
__mingw_ovr
__attribute__((__format__ (gnu_printf, 3, 4))) __MINGW_ATTRIB_NONNULL(3)
int snprintf (char *__stream, size_t __n, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 3, 0))) __MINGW_ATTRIB_NONNULL(3)
int vsnprintf (char *__stream, size_t __n, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnprintf( __stream, __n, __format, __local_argv );
}

/* Override __builtin_printf-routines ... Kludge for libstdc++ ...*/
#define __builtin_vsnprintf __mingw_vsnprintf
#define __builtin_vsprintf __mingw_vsprintf

/* #endif */ /* __NO_ISOCEXT */

#ifdef __cplusplus
}
#endif

#else /* !__USE_MINGW_ANSI_STDIO */

#undef __MINGW_PRINTF_FORMAT
#undef __MINGW_SCANF_FORMAT
#define __MINGW_PRINTF_FORMAT ms_printf
#define __MINGW_SCANF_FORMAT  ms_scanf
#undef __builtin_vsnprintf
#undef __builtin_vsprintf

/*
 * Default configuration: simply direct all calls to MSVCRT...
 */
  int __cdecl fprintf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
  int __cdecl printf(const char * __restrict__ _Format,...);
  int __cdecl sprintf(char * __restrict__ _Dest,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  int __cdecl vfprintf(FILE * __restrict__ _File,const char * __restrict__ _Format,va_list _ArgList);
  int __cdecl vprintf(const char * __restrict__ _Format,va_list _ArgList);
  int __cdecl vsprintf(char * __restrict__ _Dest,const char * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  int __cdecl fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl scanf(const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifdef _GNU_SOURCE
  int __cdecl vasprintf(char ** __restrict__ ret,const char * __restrict__ format,va_list ap)  __attribute__ ((format (__MINGW_PRINTF_FORMAT, 2, 0)));;
  int __cdecl asprintf(char ** __restrict__ ret,const char * __restrict__ format,...) __attribute__ ((format (__MINGW_PRINTF_FORMAT, 2, 3)));
#endif /*_GNU_SOURCE*/
#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
  int __cdecl __ms_vscanf(const char * __restrict__ Format, va_list argp);
  int __cdecl __ms_vfscanf (FILE * __restrict__ fp, const char * __restrict__ Format,va_list argp);
  int __cdecl __ms_vsscanf (const char * __restrict__ _Str,const char * __restrict__ Format,va_list argp);

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vfscanf (FILE *__stream,  const char *__format, __builtin_va_list __local_argv)
  {
    return __ms_vfscanf (__stream, __format, __local_argv);
  }

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vsscanf (const char * __restrict__ __source, const char * __restrict__ __format, __builtin_va_list __local_argv)
  {
    return __ms_vsscanf( __source, __format, __local_argv );
  }
  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(1)
  int vscanf(const char *__format,  __builtin_va_list __local_argv)
  {
    return __ms_vscanf (__format, __local_argv);
  }

#endif /* __NO_ISOCEXT */
#endif /* __USE_MINGW_ANSI_STDIO */

  _CRTIMP int __cdecl _filbuf(FILE *_File);
  _CRTIMP int __cdecl _flsbuf(int _Ch,FILE *_File);
#ifdef _POSIX_
  _CRTIMP FILE *__cdecl _fsopen(const char *_Filename,const char *_Mode);
#else
  _CRTIMP FILE *__cdecl _fsopen(const char *_Filename,const char *_Mode,int _ShFlag);
#endif
  void __cdecl clearerr(FILE *_File);
  int __cdecl fclose(FILE *_File);
  _CRTIMP int __cdecl _fcloseall(void);
#ifdef _POSIX_
  FILE *__cdecl fdopen(int _FileHandle,const char *_Mode) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#else
  _CRTIMP FILE *__cdecl _fdopen(int _FileHandle,const char *_Mode);
#endif
  int __cdecl feof(FILE *_File);
  int __cdecl ferror(FILE *_File);
  int __cdecl fflush(FILE *_File);
  int __cdecl fgetc(FILE *_File);
  _CRTIMP int __cdecl _fgetchar(void);
  int __cdecl fgetpos(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos); /* 64bit only, no 32bit version */
  int __cdecl fgetpos64(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos); /* fgetpos already 64bit */
  char *__cdecl fgets(char * __restrict__ _Buf,int _MaxCount,FILE * __restrict__ _File);
  _CRTIMP int __cdecl _fileno(FILE *_File);
#ifdef _POSIX_
  int __cdecl fileno(FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
  _CRTIMP char *__cdecl _tempnam(const char *_DirName,const char *_FilePrefix);
  _CRTIMP int __cdecl _flushall(void);
  FILE *__cdecl fopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  FILE *fopen64(const char * __restrict__ filename,const char * __restrict__  mode);
  int __cdecl fputc(int _Ch,FILE *_File);
  _CRTIMP int __cdecl _fputchar(int _Ch);
  int __cdecl fputs(const char * __restrict__ _Str,FILE * __restrict__ _File);
  size_t __cdecl fread(void * __restrict__ _DstBuf,size_t _ElementSize,size_t _Count,FILE * __restrict__ _File);
  FILE *__cdecl freopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode,FILE * __restrict__ _File) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl fsetpos(FILE *_File,const fpos_t *_Pos);
  int __cdecl fsetpos64(FILE *_File,const fpos_t *_Pos); /* fsetpos already 64bit */
  int __cdecl fseek(FILE *_File,long _Offset,int _Origin);

  /* Shouldn't be any fseeko32 in glibc, 32bit to 64bit casting should be fine */
  /* int fseeko32(FILE* stream, _off_t offset, int whence);*/ /* fseeko32 redirects to fseeko64 */
  int fseeko64(FILE* stream, _off64_t offset, int whence);
  int fseeko(FILE* stream, _off_t offset, int whence);

#ifndef _FILE_OFFSET_BITS_SET_FSEEKO
#define _FILE_OFFSET_BITS_SET_FSEEKO
#if (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64))
#define fseeko fseeko64
#endif /* (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)) */
#endif /* _FILE_OFFSET_BITS_SET_FSEEKO */

  long __cdecl ftell(FILE *_File);
  /* Returns truncated 64bit off_t */
  _off_t ftello(FILE * stream);
  _off64_t ftello64(FILE * stream);

#ifndef _FILE_OFFSET_BITS_SET_FTELLO
#define _FILE_OFFSET_BITS_SET_FTELLO
#if (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64))
#define ftello ftello64
#endif /* (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)) */
#endif /* _FILE_OFFSET_BITS_SET_FTELLO */

  __MINGW_EXTENSION int __cdecl _fseeki64(FILE *_File,__int64 _Offset,int _Origin);
  __MINGW_EXTENSION __int64 __cdecl _ftelli64(FILE *_File);
  size_t __cdecl fwrite(const void * __restrict__ _Str,size_t _Size,size_t _Count,FILE * __restrict__ _File);
  int __cdecl getc(FILE *_File);
  int __cdecl getchar(void);
  _CRTIMP int __cdecl _getmaxstdio(void);
  char *__cdecl gets(char *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl _getw(FILE *_File);
#ifndef _CRT_PERROR_DEFINED
#define _CRT_PERROR_DEFINED
  void __cdecl perror(const char *_ErrMsg);
#endif
  _CRTIMP int __cdecl _pclose(FILE *_File);
  _CRTIMP FILE *__cdecl _popen(const char *_Command,const char *_Mode);
#if !defined(NO_OLDNAMES) && !defined(popen)
#define popen	_popen
#define pclose	_pclose
#endif
  int __cdecl putc(int _Ch,FILE *_File);
  int __cdecl putchar(int _Ch);
  int __cdecl puts(const char *_Str);
  _CRTIMP int __cdecl _putw(int _Word,FILE *_File);
#ifndef _CRT_DIRECTORY_DEFINED
#define _CRT_DIRECTORY_DEFINED
  int __cdecl remove(const char *_Filename);
  int __cdecl rename(const char *_OldFilename,const char *_NewFilename);
  _CRTIMP int __cdecl _unlink(const char *_Filename);
#ifndef	NO_OLDNAMES
  int __cdecl unlink(const char *_Filename) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif
  void __cdecl rewind(FILE *_File);
  _CRTIMP int __cdecl _rmtmp(void);
  void __cdecl setbuf(FILE * __restrict__ _File,char * __restrict__ _Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _setmaxstdio(int _Max);
  _CRTIMP unsigned int __cdecl _set_output_format(unsigned int _Format);
  _CRTIMP unsigned int __cdecl _get_output_format(void);
  int __cdecl setvbuf(FILE * __restrict__ _File,char * __restrict__ _Buf,int _Mode,size_t _Size);
  _CRTIMP int __cdecl _scprintf(const char * __restrict__ _Format,...);
  _CRTIMP int __cdecl _snscanf(const char * __restrict__ _Src,size_t _MaxCount,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  FILE *__cdecl tmpfile(void) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl tmpnam(char *_Buffer);
  int __cdecl ungetc(int _Ch,FILE *_File);

  _CRTIMP int __cdecl _snprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vsnprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#if !defined (__USE_MINGW_ANSI_STDIO) || __USE_MINGW_ANSI_STDIO == 0
/* this is here to deal with software defining
 * vsnprintf as _vsnprintf, eg. libxml2.  */
#pragma push_macro("snprintf")
#pragma push_macro("vsnprintf")
# undef snprintf
# undef vsnprintf
  int __cdecl __ms_vsnprintf(char * __restrict__ d,size_t n,const char * __restrict__ format,va_list arg)
    __MINGW_ATTRIB_DEPRECATED_MSVC2005 __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(3)
  int vsnprintf (char * __restrict__ __stream, size_t __n, const char * __restrict__ __format, va_list __local_argv)
  {
    return __ms_vsnprintf (__stream, __n, __format, __local_argv);
  }

  int __cdecl __ms_snprintf(char * __restrict__ s, size_t n, const char * __restrict__  format, ...);

#ifndef __NO_ISOCEXT
__mingw_ovr
__MINGW_ATTRIB_NONNULL(3)
int snprintf (char * __restrict__ __stream, size_t __n, const char * __restrict__ __format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __ms_vsnprintf (__stream, __n, __format, __local_argv);
  __builtin_va_end( __local_argv );
  return __retval;
}
#endif /* !__NO_ISOCEXT */

#pragma pop_macro ("vsnprintf")
#pragma pop_macro ("snprintf")
#endif

  _CRTIMP int __cdecl _vscprintf(const char * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _set_printf_count_output(int _Value);
  _CRTIMP int __cdecl _get_printf_count_output(void);

#ifndef _WSTDIO_DEFINED
#define _WSTDIO_DEFINED

/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswscanf (const wchar_t * __restrict__ _Str,const wchar_t * __restrict__ Format,va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wscanf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwscanf(const wchar_t * __restrict__ Format, va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwscanf (FILE * __restrict__ fp, const wchar_t * __restrict__ Format,va_list argp);

/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wprintf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */__MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
/*__attribute__((__format__ (gnu_wprintf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ , ...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ ,va_list);

#if __USE_MINGW_ANSI_STDIO
/*
 * User has expressed a preference for C99 conformance...
 */

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int swscanf(const wchar_t *__source, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vswscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wscanf(const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwscanf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwscanf(FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vswscanf (const wchar_t * __restrict__ __source, const wchar_t * __restrict__ __format, __builtin_va_list __local_argv)
{
  return __mingw_vswscanf( __source, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
{
  return __mingw_vwscanf( __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwscanf( __stream, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */



__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwprintf (FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wprintf (const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwprintf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwprintf (FILE *__stream, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwprintf( __stream, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwprintf (const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vwprintf( __format, __local_argv );
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
int snwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
int vsnwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */

#else /* !__USE_MINGW_ANSI_STDIO */

  int __cdecl fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wscanf(const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
  int __cdecl __ms_vwscanf (const wchar_t * __restrict__ , va_list);
  int __cdecl __ms_vfwscanf (FILE * __restrict__ ,const wchar_t * __restrict__ ,va_list);
  int __cdecl __ms_vswscanf (const wchar_t * __restrict__ ,const wchar_t * __restrict__ ,va_list);

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
  {
    return __ms_vfwscanf (__stream, __format, __local_argv);
  }

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vswscanf (const wchar_t * __restrict__ __source, const wchar_t * __restrict__ __format, __builtin_va_list __local_argv)
  {
    return __ms_vswscanf( __source, __format, __local_argv );
  }
  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(1)
  int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
  {
    return __ms_vwscanf (__format, __local_argv);
  }

#endif /* __NO_ISOCEXT */

  int __cdecl fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  int __cdecl wprintf(const wchar_t * __restrict__ _Format,...);
  int __cdecl vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  int __cdecl vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
#endif /* __USE_MINGW_ANSI_STDIO */

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifdef _POSIX_
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode);
#else
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);
#endif

  wint_t __cdecl fgetwc(FILE *_File);
  _CRTIMP wint_t __cdecl _fgetwchar(void);
  wint_t __cdecl fputwc(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _fputwchar(wchar_t _Ch);
  wint_t __cdecl getwc(FILE *_File);
  wint_t __cdecl getwchar(void);
  wint_t __cdecl putwc(wchar_t _Ch,FILE *_File);
  wint_t __cdecl putwchar(wchar_t _Ch);
  wint_t __cdecl ungetwc(wint_t _Ch,FILE *_File);
  wchar_t *__cdecl fgetws(wchar_t * __restrict__ _Dst,int _SizeInWords,FILE * __restrict__ _File);
  int __cdecl fputws(const wchar_t * __restrict__ _Str,FILE * __restrict__ _File);
  _CRTIMP wchar_t *__cdecl _getws(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _putws(const wchar_t *_Str);

  _CRTIMP int __cdecl _scwprintf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _swprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _snwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vsnwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */

#if !defined (__USE_MINGW_ANSI_STDIO) || __USE_MINGW_ANSI_STDIO == 0
#pragma push_macro("snwprintf")
#pragma push_macro("vsnwprintf")
# undef snwprintf
# undef vsnwprintf
  int __cdecl __ms_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
  int __cdecl __ms_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
  __mingw_ovr
  int snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...)
  {
    int r;
    va_list argp;
    __builtin_va_start (argp, format);
    r = _vsnwprintf (s, n, format, argp);
    __builtin_va_end (argp);
    return r;
  }
  __mingw_ovr
  int __cdecl vsnwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, va_list arg)
  {
    return _vsnwprintf(s,n,format,arg);
  }
#pragma pop_macro ("vsnwprintf")
#pragma pop_macro ("snwprintf")
#endif

#endif /* ! __NO_ISOCEXT */
  _CRTIMP int __cdecl _swprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,va_list _Args);

#ifndef RC_INVOKED
#include <swprintf.inl>
#endif

#ifdef _CRT_NON_CONFORMING_SWPRINTFS
#ifndef __cplusplus
#define _swprintf_l __swprintf_l
#define _vswprintf_l __vswprintf_l
#endif
#endif

  _CRTIMP wchar_t *__cdecl _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);
  _CRTIMP int __cdecl _vscwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _snwscanf(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  _CRTIMP FILE *__cdecl _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  _CRTIMP FILE *__cdecl _wfopen(const wchar_t * __restrict__ _Filename,const wchar_t *__restrict__  _Mode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP FILE *__cdecl _wfreopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode,FILE * __restrict__ _OldFile) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP FILE *__cdecl _wpopen(const wchar_t *_Command,const wchar_t *_Mode);
#if !defined(NO_OLDNAMES) && !defined(wpopen)
#define wpopen	_wpopen
#endif

  _CRTIMP int __cdecl _wremove(const wchar_t *_Filename);
  _CRTIMP wchar_t *__cdecl _wtmpnam(wchar_t *_Buffer);
  _CRTIMP wint_t __cdecl _fgetwc_nolock(FILE *_File);
  _CRTIMP wint_t __cdecl _fputwc_nolock(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _ungetwc_nolock(wint_t _Ch,FILE *_File);

#undef _CRT_GETPUTWCHAR_NOINLINE

#if !defined(__cplusplus) || defined(_CRT_GETPUTWCHAR_NOINLINE) || defined (__CRT__NO_INLINE)
#define getwchar() fgetwc(stdin)
#define putwchar(_c) fputwc((_c),stdout)
#else
  __CRT_INLINE wint_t __cdecl getwchar() {return (fgetwc(stdin)); }
  __CRT_INLINE wint_t __cdecl putwchar(wchar_t _C) {return (fputwc(_C,stdout)); }
#endif

#define getwc(_stm) fgetwc(_stm)
#define putwc(_c,_stm) fputwc(_c,_stm)
#define _putwc_nolock(_c,_stm) _fputwc_nolock(_c,_stm)
#define _getwc_nolock(_c) _fgetwc_nolock(_c)
#endif

#define _STDIO_DEFINED
#endif

#define _fgetc_nolock(_stream) (--(_stream)->_cnt >= 0 ? 0xff & *(_stream)->_ptr++ : _filbuf(_stream))
#define _fputc_nolock(_c,_stream) (--(_stream)->_cnt >= 0 ? 0xff & (*(_stream)->_ptr++ = (char)(_c)) : _flsbuf((_c),(_stream)))
#define _getc_nolock(_stream) _fgetc_nolock(_stream)
#define _putc_nolock(_c,_stream) _fputc_nolock(_c,_stream)
#define _getchar_nolock() _getc_nolock(stdin)
#define _putchar_nolock(_c) _putc_nolock((_c),stdout)
#define _getwchar_nolock() _getwc_nolock(stdin)
#define _putwchar_nolock(_c) _putwc_nolock((_c),stdout)

  _CRTIMP void __cdecl _lock_file(FILE *_File);
  _CRTIMP void __cdecl _unlock_file(FILE *_File);
  _CRTIMP int __cdecl _fclose_nolock(FILE *_File);
  _CRTIMP int __cdecl _fflush_nolock(FILE *_File);
  _CRTIMP size_t __cdecl _fread_nolock(void * __restrict__ _DstBuf,size_t _ElementSize,size_t _Count,FILE * __restrict__ _File);
  _CRTIMP int __cdecl _fseek_nolock(FILE *_File,long _Offset,int _Origin);
  _CRTIMP long __cdecl _ftell_nolock(FILE *_File);
  __MINGW_EXTENSION _CRTIMP int __cdecl _fseeki64_nolock(FILE *_File,__int64 _Offset,int _Origin);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _ftelli64_nolock(FILE *_File);
  _CRTIMP size_t __cdecl _fwrite_nolock(const void * __restrict__ _DstBuf,size_t _Size,size_t _Count,FILE * __restrict__ _File);
  _CRTIMP int __cdecl _ungetc_nolock(int _Ch,FILE *_File);

#if !defined(NO_OLDNAMES) || !defined(_POSIX)
#define P_tmpdir _P_tmpdir
#define SYS_OPEN _SYS_OPEN

  char *__cdecl tempnam(const char *_Directory,const char *_FilePrefix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fcloseall(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  FILE *__cdecl fdopen(int _FileHandle,const char *_Format) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fgetchar(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fileno(FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl flushall(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fputchar(int _Ch) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl getw(FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl putw(int _Ch,FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl rmtmp(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

#ifndef __MINGW_MBWC_CONVERT_DEFINED
#define __MINGW_MBWC_CONVERT_DEFINED

/**
 * __mingw_str_wide_utf8
 * Converts a null terminated UCS-2 string to a multibyte (UTF-8) equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[in] wptr Pointer to wide string.
 * @param[out] mbptr Pointer to multibyte string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return Number of characters converted, 0 for failure.
 *
 * WideCharToMultiByte - http://msdn.microsoft.com/en-us/library/dd374130(VS.85).aspx
 */
int __cdecl __mingw_str_wide_utf8 (const wchar_t * const wptr, char **mbptr, size_t * buflen);

/**
 * __mingw_str_utf8_wide
 * Converts a null terminated UTF-8 string to a UCS-2 equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[out] mbptr Pointer to multibyte string.
 * @param[in] wptr Pointer to wide string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return Number of characters converted, 0 for failure.
 *
 * MultiByteToWideChar - http://msdn.microsoft.com/en-us/library/dd319072(VS.85).aspx
 */

int __cdecl __mingw_str_utf8_wide (const char *const mbptr, wchar_t ** wptr, size_t * buflen);

/**
 * __mingw_str_free
 * Frees buffer create by __mingw_str_wide_utf8 and __mingw_str_utf8_wide.
 * @param[in] ptr memory block to free.
 *
 */

void __cdecl __mingw_str_free(void *ptr);

#endif /* __MINGW_MBWC_CONVERT_DEFINED */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/stdio_s.h>

#include <_mingw_print_pop.h>

#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_STDIO
#define _INC_STDIO

#include <crtdefs.h>

#include <_mingw_print_push.h>

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#define BUFSIZ 512
#define _NFILE _NSTREAM_
#define _NSTREAM_ 512
#define _IOB_ENTRIES 20
#define EOF (-1)

#ifndef _FILE_DEFINED
  struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
  };
  typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif

#ifdef _POSIX_
#define _P_tmpdir "/"
#define _wP_tmpdir L"/"
#else
#define _P_tmpdir "\\"
#define _wP_tmpdir L"\\"
#endif

#define L_tmpnam (sizeof(_P_tmpdir) + 12)

#ifdef _POSIX_
#define L_ctermid 9
#define L_cuserid 32
#endif

#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

#define FILENAME_MAX 260
#define FOPEN_MAX 20
#define _SYS_OPEN 20
#define TMP_MAX 32767

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#include <_mingw_off_t.h>

#ifndef _STDIO_DEFINED
#ifdef _WIN64
  _CRTIMP FILE *__cdecl __iob_func(void);
#define _iob  __iob_func()
#else
#ifdef _MSVCRT_
extern FILE _iob[];	/* A pointer to an array of FILE */
#define __iob_func()	(_iob)
#else
extern FILE (* __MINGW_IMP_SYMBOL(_iob))[];	/* A pointer to an array of FILE */
#define __iob_func()	(* __MINGW_IMP_SYMBOL(_iob))
#define _iob __iob_func()
#endif
#endif
#endif

#ifndef _FPOS_T_DEFINED
#define _FPOS_T_DEFINED
#undef _FPOSOFF

#if (!defined(NO_OLDNAMES) || defined(__GNUC__))
  __MINGW_EXTENSION typedef __int64 fpos_t;
#define _FPOSOFF(fp) ((long)(fp))
#else
  __MINGW_EXTENSION typedef long long fpos_t;
#define _FPOSOFF(fp) ((long)(fp))
#endif

#endif

#ifndef _STDSTREAM_DEFINED
#define _STDSTREAM_DEFINED

#define stdin (&__iob_func()[0])
#define stdout (&__iob_func()[1])
#define stderr (&__iob_func()[2])
#endif

#define _IOREAD 0x0001
#define _IOWRT 0x0002

#define _IOFBF 0x0000
#define _IOLBF 0x0040
#define _IONBF 0x0004

#define _IOMYBUF 0x0008
#define _IOEOF 0x0010
#define _IOERR 0x0020
#define _IOSTRG 0x0040
#define _IORW 0x0080
#ifdef _POSIX_
#define _IOAPPEND 0x0200
#endif

#define _TWO_DIGIT_EXPONENT 0x1

#ifndef _STDIO_DEFINED
extern
  __attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vsscanf (const char * __restrict__ _Str,const char * __restrict__ Format,va_list argp);
extern
  __attribute__((__format__ (gnu_scanf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_scanf(const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vscanf(const char * __restrict__ Format, va_list argp);
extern
  __attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfscanf (FILE * __restrict__ fp, const char * __restrict__ Format,va_list argp);

extern
  __attribute__((__format__ (gnu_printf, 3, 0))) __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_vsnprintf(char * __restrict__ _DstBuf,size_t _MaxCount,const char * __restrict__ _Format,
                               va_list _ArgList);
extern
  __attribute__((__format__ (gnu_printf, 3, 4))) __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_snprintf(char * __restrict__ s, size_t n, const char * __restrict__  format, ...);
extern
  __attribute__((__format__ (gnu_printf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_printf(const char * __restrict__ , ... ) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vprintf (const char * __restrict__ , va_list) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fprintf (FILE * __restrict__ , const char * __restrict__ , ...) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfprintf (FILE * __restrict__ , const char * __restrict__ , va_list) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_sprintf (char * __restrict__ , const char * __restrict__ , ...) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vsprintf (char * __restrict__ , const char * __restrict__ , va_list) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 3))) __attribute__((nonnull (1,2)))
  int __cdecl __mingw_asprintf(char ** __restrict__ , const char * __restrict__ , ...) __MINGW_NOTHROW;
extern
  __attribute__((__format__ (gnu_printf, 2, 0))) __attribute__((nonnull (1,2)))
  int __cdecl __mingw_vasprintf(char ** __restrict__ , const char * __restrict__ , va_list) __MINGW_NOTHROW;

#if __USE_MINGW_ANSI_STDIO
/*
 * User has expressed a preference for C99 conformance...
 */
#undef __MINGW_PRINTF_FORMAT
#undef __MINGW_SCANF_FORMAT
#define __MINGW_PRINTF_FORMAT gnu_printf
#define __MINGW_SCANF_FORMAT  gnu_scanf

#ifdef _GNU_SOURCE
__mingw_ovr
__attribute__ ((__format__ (gnu_printf, 2, 3))) __attribute__((nonnull (1,2)))
int asprintf(char **__ret, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vasprintf( __ret, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__ ((__format__ (gnu_printf, 2, 0))) __attribute__((nonnull (1,2)))
int vasprintf(char **__ret, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vasprintf( __ret, __format, __local_argv );
}
#endif /* _GNU_SOURCE */

/* There seems to be a bug about builtins and static overrides of them
   in g++.  So we need to do here some trickery.  */
#ifdef __cplusplus
extern "C++" {
#endif

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int sscanf(const char *__source, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
int scanf(const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vscanf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int fscanf(FILE *__stream, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vsscanf (const char *__source, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsscanf( __source, __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
int vscanf(const char *__format,  __builtin_va_list __local_argv)
{
  return __mingw_vscanf( __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_scanf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vfscanf (FILE *__stream,  const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfscanf( __stream, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */



__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int fprintf (FILE *__stream, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 1, 2))) __MINGW_ATTRIB_NONNULL(1)
int printf (const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vprintf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 3))) __MINGW_ATTRIB_NONNULL(2)
int sprintf (char *__stream, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vfprintf (FILE *__stream, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfprintf( __stream, __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 1, 0))) __MINGW_ATTRIB_NONNULL(1)
int vprintf (const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vprintf( __format, __local_argv );
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 2, 0))) __MINGW_ATTRIB_NONNULL(2)
int vsprintf (char *__stream, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsprintf( __stream, __format, __local_argv );
}
/* #ifndef __NO_ISOCEXT */  /* externs in libmingwex.a */
__mingw_ovr
__attribute__((__format__ (gnu_printf, 3, 4))) __MINGW_ATTRIB_NONNULL(3)
int snprintf (char *__stream, size_t __n, const char *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
__attribute__((__format__ (gnu_printf, 3, 0))) __MINGW_ATTRIB_NONNULL(3)
int vsnprintf (char *__stream, size_t __n, const char *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnprintf( __stream, __n, __format, __local_argv );
}

/* Override __builtin_printf-routines ... Kludge for libstdc++ ...*/
#define __builtin_vsnprintf __mingw_vsnprintf
#define __builtin_vsprintf __mingw_vsprintf

/* #endif */ /* __NO_ISOCEXT */

#ifdef __cplusplus
}
#endif

#else /* !__USE_MINGW_ANSI_STDIO */

#undef __MINGW_PRINTF_FORMAT
#undef __MINGW_SCANF_FORMAT
#define __MINGW_PRINTF_FORMAT ms_printf
#define __MINGW_SCANF_FORMAT  ms_scanf
#undef __builtin_vsnprintf
#undef __builtin_vsprintf

/*
 * Default configuration: simply direct all calls to MSVCRT...
 */
  int __cdecl fprintf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
  int __cdecl printf(const char * __restrict__ _Format,...);
  int __cdecl sprintf(char * __restrict__ _Dest,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  int __cdecl vfprintf(FILE * __restrict__ _File,const char * __restrict__ _Format,va_list _ArgList);
  int __cdecl vprintf(const char * __restrict__ _Format,va_list _ArgList);
  int __cdecl vsprintf(char * __restrict__ _Dest,const char * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  int __cdecl fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl scanf(const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifdef _GNU_SOURCE
  int __cdecl vasprintf(char ** __restrict__ ret,const char * __restrict__ format,va_list ap)  __attribute__ ((format (__MINGW_PRINTF_FORMAT, 2, 0)));;
  int __cdecl asprintf(char ** __restrict__ ret,const char * __restrict__ format,...) __attribute__ ((format (__MINGW_PRINTF_FORMAT, 2, 3)));
#endif /*_GNU_SOURCE*/
#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
  int __cdecl __ms_vscanf(const char * __restrict__ Format, va_list argp);
  int __cdecl __ms_vfscanf (FILE * __restrict__ fp, const char * __restrict__ Format,va_list argp);
  int __cdecl __ms_vsscanf (const char * __restrict__ _Str,const char * __restrict__ Format,va_list argp);

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vfscanf (FILE *__stream,  const char *__format, __builtin_va_list __local_argv)
  {
    return __ms_vfscanf (__stream, __format, __local_argv);
  }

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vsscanf (const char * __restrict__ __source, const char * __restrict__ __format, __builtin_va_list __local_argv)
  {
    return __ms_vsscanf( __source, __format, __local_argv );
  }
  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(1)
  int vscanf(const char *__format,  __builtin_va_list __local_argv)
  {
    return __ms_vscanf (__format, __local_argv);
  }

#endif /* __NO_ISOCEXT */
#endif /* __USE_MINGW_ANSI_STDIO */

  _CRTIMP int __cdecl _filbuf(FILE *_File);
  _CRTIMP int __cdecl _flsbuf(int _Ch,FILE *_File);
#ifdef _POSIX_
  _CRTIMP FILE *__cdecl _fsopen(const char *_Filename,const char *_Mode);
#else
  _CRTIMP FILE *__cdecl _fsopen(const char *_Filename,const char *_Mode,int _ShFlag);
#endif
  void __cdecl clearerr(FILE *_File);
  int __cdecl fclose(FILE *_File);
  _CRTIMP int __cdecl _fcloseall(void);
#ifdef _POSIX_
  FILE *__cdecl fdopen(int _FileHandle,const char *_Mode) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#else
  _CRTIMP FILE *__cdecl _fdopen(int _FileHandle,const char *_Mode);
#endif
  int __cdecl feof(FILE *_File);
  int __cdecl ferror(FILE *_File);
  int __cdecl fflush(FILE *_File);
  int __cdecl fgetc(FILE *_File);
  _CRTIMP int __cdecl _fgetchar(void);
  int __cdecl fgetpos(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos); /* 64bit only, no 32bit version */
  int __cdecl fgetpos64(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos); /* fgetpos already 64bit */
  char *__cdecl fgets(char * __restrict__ _Buf,int _MaxCount,FILE * __restrict__ _File);
  _CRTIMP int __cdecl _fileno(FILE *_File);
#ifdef _POSIX_
  int __cdecl fileno(FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
  _CRTIMP char *__cdecl _tempnam(const char *_DirName,const char *_FilePrefix);
  _CRTIMP int __cdecl _flushall(void);
  FILE *__cdecl fopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  FILE *fopen64(const char * __restrict__ filename,const char * __restrict__  mode);
  int __cdecl fputc(int _Ch,FILE *_File);
  _CRTIMP int __cdecl _fputchar(int _Ch);
  int __cdecl fputs(const char * __restrict__ _Str,FILE * __restrict__ _File);
  size_t __cdecl fread(void * __restrict__ _DstBuf,size_t _ElementSize,size_t _Count,FILE * __restrict__ _File);
  FILE *__cdecl freopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode,FILE * __restrict__ _File) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl fsetpos(FILE *_File,const fpos_t *_Pos);
  int __cdecl fsetpos64(FILE *_File,const fpos_t *_Pos); /* fsetpos already 64bit */
  int __cdecl fseek(FILE *_File,long _Offset,int _Origin);

  /* Shouldn't be any fseeko32 in glibc, 32bit to 64bit casting should be fine */
  /* int fseeko32(FILE* stream, _off_t offset, int whence);*/ /* fseeko32 redirects to fseeko64 */
  int fseeko64(FILE* stream, _off64_t offset, int whence);
  int fseeko(FILE* stream, _off_t offset, int whence);

#ifndef _FILE_OFFSET_BITS_SET_FSEEKO
#define _FILE_OFFSET_BITS_SET_FSEEKO
#if (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64))
#define fseeko fseeko64
#endif /* (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)) */
#endif /* _FILE_OFFSET_BITS_SET_FSEEKO */

  long __cdecl ftell(FILE *_File);
  /* Returns truncated 64bit off_t */
  _off_t ftello(FILE * stream);
  _off64_t ftello64(FILE * stream);

#ifndef _FILE_OFFSET_BITS_SET_FTELLO
#define _FILE_OFFSET_BITS_SET_FTELLO
#if (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64))
#define ftello ftello64
#endif /* (defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS == 64)) */
#endif /* _FILE_OFFSET_BITS_SET_FTELLO */

  __MINGW_EXTENSION int __cdecl _fseeki64(FILE *_File,__int64 _Offset,int _Origin);
  __MINGW_EXTENSION __int64 __cdecl _ftelli64(FILE *_File);
  size_t __cdecl fwrite(const void * __restrict__ _Str,size_t _Size,size_t _Count,FILE * __restrict__ _File);
  int __cdecl getc(FILE *_File);
  int __cdecl getchar(void);
  _CRTIMP int __cdecl _getmaxstdio(void);
  char *__cdecl gets(char *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl _getw(FILE *_File);
#ifndef _CRT_PERROR_DEFINED
#define _CRT_PERROR_DEFINED
  void __cdecl perror(const char *_ErrMsg);
#endif
  _CRTIMP int __cdecl _pclose(FILE *_File);
  _CRTIMP FILE *__cdecl _popen(const char *_Command,const char *_Mode);
#if !defined(NO_OLDNAMES) && !defined(popen)
#define popen	_popen
#define pclose	_pclose
#endif
  int __cdecl putc(int _Ch,FILE *_File);
  int __cdecl putchar(int _Ch);
  int __cdecl puts(const char *_Str);
  _CRTIMP int __cdecl _putw(int _Word,FILE *_File);
#ifndef _CRT_DIRECTORY_DEFINED
#define _CRT_DIRECTORY_DEFINED
  int __cdecl remove(const char *_Filename);
  int __cdecl rename(const char *_OldFilename,const char *_NewFilename);
  _CRTIMP int __cdecl _unlink(const char *_Filename);
#ifndef	NO_OLDNAMES
  int __cdecl unlink(const char *_Filename) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif
  void __cdecl rewind(FILE *_File);
  _CRTIMP int __cdecl _rmtmp(void);
  void __cdecl setbuf(FILE * __restrict__ _File,char * __restrict__ _Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _setmaxstdio(int _Max);
  _CRTIMP unsigned int __cdecl _set_output_format(unsigned int _Format);
  _CRTIMP unsigned int __cdecl _get_output_format(void);
  int __cdecl setvbuf(FILE * __restrict__ _File,char * __restrict__ _Buf,int _Mode,size_t _Size);
  _CRTIMP int __cdecl _scprintf(const char * __restrict__ _Format,...);
  _CRTIMP int __cdecl _snscanf(const char * __restrict__ _Src,size_t _MaxCount,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  FILE *__cdecl tmpfile(void) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl tmpnam(char *_Buffer);
  int __cdecl ungetc(int _Ch,FILE *_File);

  _CRTIMP int __cdecl _snprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vsnprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#if !defined (__USE_MINGW_ANSI_STDIO) || __USE_MINGW_ANSI_STDIO == 0
/* this is here to deal with software defining
 * vsnprintf as _vsnprintf, eg. libxml2.  */
#pragma push_macro("snprintf")
#pragma push_macro("vsnprintf")
# undef snprintf
# undef vsnprintf
  int __cdecl __ms_vsnprintf(char * __restrict__ d,size_t n,const char * __restrict__ format,va_list arg)
    __MINGW_ATTRIB_DEPRECATED_MSVC2005 __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(3)
  int vsnprintf (char * __restrict__ __stream, size_t __n, const char * __restrict__ __format, va_list __local_argv)
  {
    return __ms_vsnprintf (__stream, __n, __format, __local_argv);
  }

  int __cdecl __ms_snprintf(char * __restrict__ s, size_t n, const char * __restrict__  format, ...);

#ifndef __NO_ISOCEXT
__mingw_ovr
__MINGW_ATTRIB_NONNULL(3)
int snprintf (char * __restrict__ __stream, size_t __n, const char * __restrict__ __format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __ms_vsnprintf (__stream, __n, __format, __local_argv);
  __builtin_va_end( __local_argv );
  return __retval;
}
#endif /* !__NO_ISOCEXT */

#pragma pop_macro ("vsnprintf")
#pragma pop_macro ("snprintf")
#endif

  _CRTIMP int __cdecl _vscprintf(const char * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _set_printf_count_output(int _Value);
  _CRTIMP int __cdecl _get_printf_count_output(void);

#ifndef _WSTDIO_DEFINED
#define _WSTDIO_DEFINED

/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswscanf (const wchar_t * __restrict__ _Str,const wchar_t * __restrict__ Format,va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wscanf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwscanf(const wchar_t * __restrict__ Format, va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwscanf (FILE * __restrict__ fp, const wchar_t * __restrict__ Format,va_list argp);

/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wprintf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */__MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
/*__attribute__((__format__ (gnu_wprintf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ , ...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ ,va_list);

#if __USE_MINGW_ANSI_STDIO
/*
 * User has expressed a preference for C99 conformance...
 */

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int swscanf(const wchar_t *__source, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vswscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wscanf(const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwscanf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwscanf(FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vswscanf (const wchar_t * __restrict__ __source, const wchar_t * __restrict__ __format, __builtin_va_list __local_argv)
{
  return __mingw_vswscanf( __source, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
{
  return __mingw_vwscanf( __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwscanf( __stream, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */



__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwprintf (FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wprintf (const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwprintf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwprintf (FILE *__stream, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwprintf( __stream, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwprintf (const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vwprintf( __format, __local_argv );
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
int snwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
int vsnwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */

#else /* !__USE_MINGW_ANSI_STDIO */

  int __cdecl fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wscanf(const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
  int __cdecl __ms_vwscanf (const wchar_t * __restrict__ , va_list);
  int __cdecl __ms_vfwscanf (FILE * __restrict__ ,const wchar_t * __restrict__ ,va_list);
  int __cdecl __ms_vswscanf (const wchar_t * __restrict__ ,const wchar_t * __restrict__ ,va_list);

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
  {
    return __ms_vfwscanf (__stream, __format, __local_argv);
  }

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vswscanf (const wchar_t * __restrict__ __source, const wchar_t * __restrict__ __format, __builtin_va_list __local_argv)
  {
    return __ms_vswscanf( __source, __format, __local_argv );
  }
  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(1)
  int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
  {
    return __ms_vwscanf (__format, __local_argv);
  }

#endif /* __NO_ISOCEXT */

  int __cdecl fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  int __cdecl wprintf(const wchar_t * __restrict__ _Format,...);
  int __cdecl vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  int __cdecl vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
#endif /* __USE_MINGW_ANSI_STDIO */

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifdef _POSIX_
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode);
#else
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);
#endif

  wint_t __cdecl fgetwc(FILE *_File);
  _CRTIMP wint_t __cdecl _fgetwchar(void);
  wint_t __cdecl fputwc(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _fputwchar(wchar_t _Ch);
  wint_t __cdecl getwc(FILE *_File);
  wint_t __cdecl getwchar(void);
  wint_t __cdecl putwc(wchar_t _Ch,FILE *_File);
  wint_t __cdecl putwchar(wchar_t _Ch);
  wint_t __cdecl ungetwc(wint_t _Ch,FILE *_File);
  wchar_t *__cdecl fgetws(wchar_t * __restrict__ _Dst,int _SizeInWords,FILE * __restrict__ _File);
  int __cdecl fputws(const wchar_t * __restrict__ _Str,FILE * __restrict__ _File);
  _CRTIMP wchar_t *__cdecl _getws(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _putws(const wchar_t *_Str);

  _CRTIMP int __cdecl _scwprintf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _swprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _snwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vsnwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */

#if !defined (__USE_MINGW_ANSI_STDIO) || __USE_MINGW_ANSI_STDIO == 0
#pragma push_macro("snwprintf")
#pragma push_macro("vsnwprintf")
# undef snwprintf
# undef vsnwprintf
  int __cdecl __ms_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
  int __cdecl __ms_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
  __mingw_ovr
  int snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...)
  {
    int r;
    va_list argp;
    __builtin_va_start (argp, format);
    r = _vsnwprintf (s, n, format, argp);
    __builtin_va_end (argp);
    return r;
  }
  __mingw_ovr
  int __cdecl vsnwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, va_list arg)
  {
    return _vsnwprintf(s,n,format,arg);
  }
#pragma pop_macro ("vsnwprintf")
#pragma pop_macro ("snwprintf")
#endif

#endif /* ! __NO_ISOCEXT */
  _CRTIMP int __cdecl _swprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,va_list _Args);

#ifndef RC_INVOKED
#include <swprintf.inl>
#endif

#ifdef _CRT_NON_CONFORMING_SWPRINTFS
#ifndef __cplusplus
#define _swprintf_l __swprintf_l
#define _vswprintf_l __vswprintf_l
#endif
#endif

  _CRTIMP wchar_t *__cdecl _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);
  _CRTIMP int __cdecl _vscwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _snwscanf(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  _CRTIMP FILE *__cdecl _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  _CRTIMP FILE *__cdecl _wfopen(const wchar_t * __restrict__ _Filename,const wchar_t *__restrict__  _Mode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP FILE *__cdecl _wfreopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode,FILE * __restrict__ _OldFile) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP FILE *__cdecl _wpopen(const wchar_t *_Command,const wchar_t *_Mode);
#if !defined(NO_OLDNAMES) && !defined(wpopen)
#define wpopen	_wpopen
#endif

  _CRTIMP int __cdecl _wremove(const wchar_t *_Filename);
  _CRTIMP wchar_t *__cdecl _wtmpnam(wchar_t *_Buffer);
  _CRTIMP wint_t __cdecl _fgetwc_nolock(FILE *_File);
  _CRTIMP wint_t __cdecl _fputwc_nolock(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _ungetwc_nolock(wint_t _Ch,FILE *_File);

#undef _CRT_GETPUTWCHAR_NOINLINE

#if !defined(__cplusplus) || defined(_CRT_GETPUTWCHAR_NOINLINE) || defined (__CRT__NO_INLINE)
#define getwchar() fgetwc(stdin)
#define putwchar(_c) fputwc((_c),stdout)
#else
  __CRT_INLINE wint_t __cdecl getwchar() {return (fgetwc(stdin)); }
  __CRT_INLINE wint_t __cdecl putwchar(wchar_t _C) {return (fputwc(_C,stdout)); }
#endif

#define getwc(_stm) fgetwc(_stm)
#define putwc(_c,_stm) fputwc(_c,_stm)
#define _putwc_nolock(_c,_stm) _fputwc_nolock(_c,_stm)
#define _getwc_nolock(_c) _fgetwc_nolock(_c)
#endif

#define _STDIO_DEFINED
#endif

#define _fgetc_nolock(_stream) (--(_stream)->_cnt >= 0 ? 0xff & *(_stream)->_ptr++ : _filbuf(_stream))
#define _fputc_nolock(_c,_stream) (--(_stream)->_cnt >= 0 ? 0xff & (*(_stream)->_ptr++ = (char)(_c)) : _flsbuf((_c),(_stream)))
#define _getc_nolock(_stream) _fgetc_nolock(_stream)
#define _putc_nolock(_c,_stream) _fputc_nolock(_c,_stream)
#define _getchar_nolock() _getc_nolock(stdin)
#define _putchar_nolock(_c) _putc_nolock((_c),stdout)
#define _getwchar_nolock() _getwc_nolock(stdin)
#define _putwchar_nolock(_c) _putwc_nolock((_c),stdout)

  _CRTIMP void __cdecl _lock_file(FILE *_File);
  _CRTIMP void __cdecl _unlock_file(FILE *_File);
  _CRTIMP int __cdecl _fclose_nolock(FILE *_File);
  _CRTIMP int __cdecl _fflush_nolock(FILE *_File);
  _CRTIMP size_t __cdecl _fread_nolock(void * __restrict__ _DstBuf,size_t _ElementSize,size_t _Count,FILE * __restrict__ _File);
  _CRTIMP int __cdecl _fseek_nolock(FILE *_File,long _Offset,int _Origin);
  _CRTIMP long __cdecl _ftell_nolock(FILE *_File);
  __MINGW_EXTENSION _CRTIMP int __cdecl _fseeki64_nolock(FILE *_File,__int64 _Offset,int _Origin);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _ftelli64_nolock(FILE *_File);
  _CRTIMP size_t __cdecl _fwrite_nolock(const void * __restrict__ _DstBuf,size_t _Size,size_t _Count,FILE * __restrict__ _File);
  _CRTIMP int __cdecl _ungetc_nolock(int _Ch,FILE *_File);

#if !defined(NO_OLDNAMES) || !defined(_POSIX)
#define P_tmpdir _P_tmpdir
#define SYS_OPEN _SYS_OPEN

  char *__cdecl tempnam(const char *_Directory,const char *_FilePrefix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fcloseall(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  FILE *__cdecl fdopen(int _FileHandle,const char *_Format) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fgetchar(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fileno(FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl flushall(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl fputchar(int _Ch) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl getw(FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl putw(int _Ch,FILE *_File) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl rmtmp(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

#ifndef __MINGW_MBWC_CONVERT_DEFINED
#define __MINGW_MBWC_CONVERT_DEFINED

/**
 * __mingw_str_wide_utf8
 * Converts a null terminated UCS-2 string to a multibyte (UTF-8) equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[in] wptr Pointer to wide string.
 * @param[out] mbptr Pointer to multibyte string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return Number of characters converted, 0 for failure.
 *
 * WideCharToMultiByte - http://msdn.microsoft.com/en-us/library/dd374130(VS.85).aspx
 */
int __cdecl __mingw_str_wide_utf8 (const wchar_t * const wptr, char **mbptr, size_t * buflen);

/**
 * __mingw_str_utf8_wide
 * Converts a null terminated UTF-8 string to a UCS-2 equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[out] mbptr Pointer to multibyte string.
 * @param[in] wptr Pointer to wide string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return Number of characters converted, 0 for failure.
 *
 * MultiByteToWideChar - http://msdn.microsoft.com/en-us/library/dd319072(VS.85).aspx
 */

int __cdecl __mingw_str_utf8_wide (const char *const mbptr, wchar_t ** wptr, size_t * buflen);

/**
 * __mingw_str_free
 * Frees buffer create by __mingw_str_wide_utf8 and __mingw_str_utf8_wide.
 * @param[in] ptr memory block to free.
 *
 */

void __cdecl __mingw_str_free(void *ptr);

#endif /* __MINGW_MBWC_CONVERT_DEFINED */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/stdio_s.h>

#include <_mingw_print_pop.h>

#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_STDLIB
#define _INC_STDLIB

#include <crtdefs.h>
#include <limits.h>

#if defined (__USE_MINGW_ANSI_STDIO) && ((__USE_MINGW_ANSI_STDIO + 0) != 0) && !defined (__USE_MINGW_STRTOX)
#define __USE_MINGW_STRTOX 1
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifndef _ONEXIT_T_DEFINED
#define _ONEXIT_T_DEFINED

  typedef int (__cdecl *_onexit_t)(void);

#ifndef	NO_OLDNAMES
#define onexit_t _onexit_t
#endif
#endif

#ifndef _DIV_T_DEFINED
#define _DIV_T_DEFINED

  typedef struct _div_t {
    int quot;
    int rem;
  } div_t;

  typedef struct _ldiv_t {
    long quot;
    long rem;
  } ldiv_t;
#endif

#ifndef _CRT_DOUBLE_DEC
#define _CRT_DOUBLE_DEC

#pragma pack(4)
  typedef struct {
    unsigned char ld[10];
  } _LDOUBLE;
#pragma pack()

#define _PTR_LD(x) ((unsigned char *)(&(x)->ld))

  typedef struct {
    double x;
  } _CRT_DOUBLE;

  typedef struct {
    float f;
  } _CRT_FLOAT;

#pragma push_macro("long")
#undef long

  typedef struct {
    long double x;
  } _LONGDOUBLE;

#pragma pop_macro("long")

#pragma pack(4)
  typedef struct {
    unsigned char ld12[12];
  } _LDBL12;
#pragma pack()
#endif

#define RAND_MAX 0x7fff

#ifndef MB_CUR_MAX
#define MB_CUR_MAX ___mb_cur_max_func()
#ifndef __mb_cur_max
#ifdef _MSVCRT_
  extern int __mb_cur_max;
#define __mb_cur_max	__mb_cur_max
#else
  extern int * __MINGW_IMP_SYMBOL(__mb_cur_max);
#define __mb_cur_max	(* __MINGW_IMP_SYMBOL(__mb_cur_max))
#endif
#endif
#define ___mb_cur_max_func() (__mb_cur_max)
#endif

#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))

#define _MAX_PATH 260
#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define _MAX_FNAME 256
#define _MAX_EXT 256

#define _OUT_TO_DEFAULT 0
#define _OUT_TO_STDERR 1
#define _OUT_TO_MSGBOX 2
#define _REPORT_ERRMODE 3

#define _WRITE_ABORT_MSG 0x1
#define _CALL_REPORTFAULT 0x2

#define _MAX_ENV 32767

  typedef void (__cdecl *_purecall_handler)(void);

  _CRTIMP _purecall_handler __cdecl _set_purecall_handler(_purecall_handler _Handler);
  _CRTIMP _purecall_handler __cdecl _get_purecall_handler(void);

  typedef void (__cdecl *_invalid_parameter_handler)(const wchar_t *,const wchar_t *,const wchar_t *,unsigned int,uintptr_t);
  _CRTIMP _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(_invalid_parameter_handler _Handler);
  _CRTIMP _invalid_parameter_handler __cdecl _get_invalid_parameter_handler(void);

#ifndef _CRT_ERRNO_DEFINED
#define _CRT_ERRNO_DEFINED
  _CRTIMP extern int *__cdecl _errno(void);
#define errno (*_errno())
  errno_t __cdecl _set_errno(int _Value);
  errno_t __cdecl _get_errno(int *_Value);
#endif
  _CRTIMP unsigned long *__cdecl __doserrno(void);
#define _doserrno (*__doserrno())
  errno_t __cdecl _set_doserrno(unsigned long _Value);
  errno_t __cdecl _get_doserrno(unsigned long *_Value);
#ifdef _MSVCRT_
  extern char *_sys_errlist[];
  extern int _sys_nerr;
#else
  extern _CRTIMP char *_sys_errlist[1];
  extern _CRTIMP int _sys_nerr;
#endif
#if (defined(_X86_) && !defined(__x86_64))
  _CRTIMP int *__cdecl __p___argc(void);
  _CRTIMP char ***__cdecl __p___argv(void);
  _CRTIMP wchar_t ***__cdecl __p___wargv(void);
  _CRTIMP char ***__cdecl __p__environ(void);
  _CRTIMP wchar_t ***__cdecl __p__wenviron(void);
  _CRTIMP char **__cdecl __p__pgmptr(void);
  _CRTIMP wchar_t **__cdecl __p__wpgmptr(void);
#endif
#ifndef __argc
#ifdef _MSVCRT_
  extern int __argc;
#else
  extern int * __MINGW_IMP_SYMBOL(__argc);
#define __argc (* __MINGW_IMP_SYMBOL(__argc))
#endif
#endif
#ifndef __argv
#ifdef _MSVCRT_
  extern char **__argv;
#else
  extern char *** __MINGW_IMP_SYMBOL(__argv);
#define __argv	(* __MINGW_IMP_SYMBOL(__argv))
#endif
#endif
#ifndef __wargv
#ifdef _MSVCRT_
  extern wchar_t **__wargv;
#else
  extern wchar_t *** __MINGW_IMP_SYMBOL(__wargv);
#define __wargv (* __MINGW_IMP_SYMBOL(__wargv))
#endif
#endif

#ifdef _POSIX_
  extern char **environ;
#else
#ifndef _environ
#ifdef _MSVCRT_
  extern char **_environ;
#else
  extern char *** __MINGW_IMP_SYMBOL(_environ);
#define _environ (* __MINGW_IMP_SYMBOL(_environ))
#endif
#endif

#ifndef _wenviron
#ifdef _MSVCRT_
  extern wchar_t **_wenviron;
#else
  extern wchar_t *** __MINGW_IMP_SYMBOL(_wenviron);
#define _wenviron (* __MINGW_IMP_SYMBOL(_wenviron))
#endif
#endif
#endif
#ifndef _pgmptr
#ifdef _MSVCRT_
  extern char *_pgmptr;
#else
  extern char ** __MINGW_IMP_SYMBOL(_pgmptr);
#define _pgmptr	(* __MINGW_IMP_SYMBOL(_pgmptr))
#endif
#endif

#ifndef _wpgmptr
#ifdef _MSVCRT_
  extern wchar_t *_wpgmptr;
#else
  extern wchar_t ** __MINGW_IMP_SYMBOL(_wpgmptr);
#define _wpgmptr (* __MINGW_IMP_SYMBOL(_wpgmptr))
#endif
#endif
  errno_t __cdecl _get_pgmptr(char **_Value);
  errno_t __cdecl _get_wpgmptr(wchar_t **_Value);
#ifndef _fmode
#ifdef _MSVCRT_
  extern int _fmode;
#else
  extern int * __MINGW_IMP_SYMBOL(_fmode);
#define _fmode	(* __MINGW_IMP_SYMBOL(_fmode))
#endif
#endif
  _CRTIMP errno_t __cdecl _set_fmode(int _Mode);
  _CRTIMP errno_t __cdecl _get_fmode(int *_PMode);

#ifndef _osplatform
#ifdef _MSVCRT_
  extern unsigned int _osplatform;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_osplatform);
#define _osplatform (* __MINGW_IMP_SYMBOL(_osplatform))
#endif
#endif

#ifndef _osver
#ifdef _MSVCRT_
  extern unsigned int _osver;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_osver);
#define _osver	(* __MINGW_IMP_SYMBOL(_osver))
#endif
#endif

#ifndef _winver
#ifdef _MSVCRT_
  extern unsigned int _winver;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_winver);
#define _winver	(* __MINGW_IMP_SYMBOL(_winver))
#endif
#endif

#ifndef _winmajor
#ifdef _MSVCRT_
  extern unsigned int _winmajor;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_winmajor);
#define _winmajor (* __MINGW_IMP_SYMBOL(_winmajor))
#endif
#endif

#ifndef _winminor
#ifdef _MSVCRT_
  extern unsigned int _winminor;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_winminor);
#define _winminor (* __MINGW_IMP_SYMBOL(_winminor))
#endif
#endif

  errno_t __cdecl _get_osplatform(unsigned int *_Value);
  errno_t __cdecl _get_osver(unsigned int *_Value);
  errno_t __cdecl _get_winver(unsigned int *_Value);
  errno_t __cdecl _get_winmajor(unsigned int *_Value);
  errno_t __cdecl _get_winminor(unsigned int *_Value);
#ifndef _countof
#ifndef __cplusplus
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
  extern "C++" {
    template <typename _CountofType,size_t _SizeOfArray> char (*__countof_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
  }
#endif
#endif

#ifndef _CRT_TERMINATE_DEFINED
#define _CRT_TERMINATE_DEFINED
  void __cdecl __MINGW_NOTHROW exit(int _Code) __MINGW_ATTRIB_NORETURN;
  void __cdecl __MINGW_NOTHROW _exit(int _Code) __MINGW_ATTRIB_NORETURN;

#if !defined __NO_ISOCEXT /* extern stub in static libmingwex.a */
  /* C99 function name */
  void __cdecl _Exit(int) __MINGW_ATTRIB_NORETURN;
#ifndef __CRT__NO_INLINE
  __CRT_INLINE __MINGW_ATTRIB_NORETURN void  __cdecl _Exit(int status)
  {  _exit(status); }
#endif /* !__CRT__NO_INLINE */
#endif /* Not  __NO_ISOCEXT */

#pragma push_macro("abort")
#undef abort
  void __cdecl __declspec(noreturn) abort(void);
#pragma pop_macro("abort")

#endif /* _CRT_TERMINATE_DEFINED */

  _CRTIMP unsigned int __cdecl _set_abort_behavior(unsigned int _Flags,unsigned int _Mask);

#ifndef _CRT_ABS_DEFINED
#define _CRT_ABS_DEFINED
  int __cdecl abs(int _X);
  long __cdecl labs(long _X);
#endif

  __MINGW_EXTENSION __int64 __cdecl _abs64(__int64);
#ifdef __MINGW_INTRIN_INLINE
  __MINGW_INTRIN_INLINE __int64 __cdecl _abs64(__int64 x) {
    return __builtin_llabs(x);
  }
#endif

  int __cdecl atexit(void (__cdecl *)(void));
#ifndef _CRT_ATOF_DEFINED
#define _CRT_ATOF_DEFINED
  double __cdecl atof(const char *_String);
  double __cdecl _atof_l(const char *_String,_locale_t _Locale);
#endif
  int __cdecl atoi(const char *_Str);
  _CRTIMP int __cdecl _atoi_l(const char *_Str,_locale_t _Locale);
  long __cdecl atol(const char *_Str);
  _CRTIMP long __cdecl _atol_l(const char *_Str,_locale_t _Locale);
#ifndef _CRT_ALGO_DEFINED
#define _CRT_ALGO_DEFINED
  void *__cdecl bsearch(const void *_Key,const void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__cdecl *_PtFuncCompare)(const void *,const void *));
  void __cdecl qsort(void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__cdecl *_PtFuncCompare)(const void *,const void *));
#endif
  unsigned short __cdecl _byteswap_ushort(unsigned short _Short);
  unsigned long __cdecl _byteswap_ulong (unsigned long _Long);
  __MINGW_EXTENSION unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64 _Int64);
  div_t __cdecl div(int _Numerator,int _Denominator);
  char *__cdecl getenv(const char *_VarName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _itoa(int _Value,char *_Dest,int _Radix);
  __MINGW_EXTENSION _CRTIMP char *__cdecl _i64toa(__int64 _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP char *__cdecl _ui64toa(unsigned __int64 _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _atoi64(const char *_String);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _atoi64_l(const char *_String,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _strtoi64(const char *_String,char **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _strtoi64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _strtoui64(const char *_String,char **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _strtoui64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  ldiv_t __cdecl ldiv(long _Numerator,long _Denominator);
  _CRTIMP char *__cdecl _ltoa(long _Value,char *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl mblen(const char *_Ch,size_t _MaxCount);
  _CRTIMP int __cdecl _mblen_l(const char *_Ch,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP size_t __cdecl _mbstrlen(const char *_Str);
  _CRTIMP size_t __cdecl _mbstrlen_l(const char *_Str,_locale_t _Locale);
  _CRTIMP size_t __cdecl _mbstrnlen(const char *_Str,size_t _MaxCount);
  _CRTIMP size_t __cdecl _mbstrnlen_l(const char *_Str,size_t _MaxCount,_locale_t _Locale);
  int __cdecl mbtowc(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes);
  _CRTIMP int __cdecl _mbtowc_l(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes,_locale_t _Locale);
  size_t __cdecl mbstowcs(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount);
  _CRTIMP size_t __cdecl _mbstowcs_l(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale);
  int __cdecl rand(void);
  _CRTIMP int __cdecl _set_error_mode(int _Mode);
  void __cdecl srand(unsigned int _Seed);
#ifdef _CRT_RAND_S
  _CRTIMP errno_t __cdecl rand_s(unsigned int *randomValue);
#endif
#if defined(__USE_MINGW_STRTOX)
__mingw_ovr
double __cdecl __MINGW_NOTHROW strtod(const char * __restrict__ _Str,char ** __restrict__ _EndPtr)
{
  double __cdecl __mingw_strtod (const char * __restrict__, char ** __restrict__);
  return __mingw_strtod( _Str, _EndPtr);
}

__mingw_ovr
float __cdecl __MINGW_NOTHROW strtof(const char * __restrict__ _Str,char ** __restrict__ _EndPtr)
{
  float __cdecl __mingw_strtof (const char * __restrict__, char ** __restrict__);
  return __mingw_strtof( _Str, _EndPtr);
}

/* strtold is already an alias to __mingw_strtold */
#else
  double __cdecl __MINGW_NOTHROW strtod(const char * __restrict__ _Str,char ** __restrict__ _EndPtr);
  float __cdecl __MINGW_NOTHROW strtof(const char * __restrict__ nptr, char ** __restrict__ endptr);
#endif /* defined(__USE_MINGW_STRTOX) */
  long double __cdecl __MINGW_NOTHROW strtold(const char * __restrict__ , char ** __restrict__ );
#if !defined __NO_ISOCEXT
  /* libmingwex.a provides a c99-compliant strtod() exported as __strtod() */
  extern double __cdecl __MINGW_NOTHROW
  __strtod (const char * __restrict__ , char ** __restrict__);
#define strtod __strtod
#endif /* __NO_ISOCEXT */

#if !defined __NO_ISOCEXT  /* in libmingwex.a */
  float __cdecl __mingw_strtof (const char * __restrict__, char ** __restrict__);
  double __cdecl __mingw_strtod (const char * __restrict__, char ** __restrict__);
  long double __cdecl __mingw_strtold(const char * __restrict__, char ** __restrict__);
#endif /* __NO_ISOCEXT */
  _CRTIMP double __cdecl _strtod_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,_locale_t _Locale);
  long __cdecl strtol(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP long __cdecl _strtol_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __cdecl strtoul(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP unsigned long __cdecl _strtoul_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
#ifndef _CRT_SYSTEM_DEFINED
#define _CRT_SYSTEM_DEFINED
  int __cdecl system(const char *_Command);
#endif
  _CRTIMP char *__cdecl _ultoa(unsigned long _Value,char *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wctomb(char *_MbCh,wchar_t _WCh) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wctomb_l(char *_MbCh,wchar_t _WCh,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcstombs(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP size_t __cdecl _wcstombs_l(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef _CRT_ALLOCATION_DEFINED
#define _CRT_ALLOCATION_DEFINED
  void *__cdecl calloc(size_t _NumOfElements,size_t _SizeOfElements);
  void __cdecl free(void *_Memory);
  void *__cdecl malloc(size_t _Size);
  void *__cdecl realloc(void *_Memory,size_t _NewSize);
  _CRTIMP void *__cdecl _recalloc(void *_Memory,size_t _Count,size_t _Size);
/* Make sure that X86intrin.h doesn't produce here collisions.  */
#if (!defined (_XMMINTRIN_H_INCLUDED) && !defined (_MM_MALLOC_H_INCLUDED)) || defined(_aligned_malloc)
#pragma push_macro("_aligned_free")
#pragma push_macro("_aligned_malloc")
#undef _aligned_free
#undef _aligned_malloc
  _CRTIMP void __cdecl _aligned_free(void *_Memory);
  _CRTIMP void *__cdecl _aligned_malloc(size_t _Size,size_t _Alignment);
#pragma pop_macro("_aligned_free")
#pragma pop_macro("_aligned_malloc")
#endif
  _CRTIMP void *__cdecl _aligned_offset_malloc(size_t _Size,size_t _Alignment,size_t _Offset);
  _CRTIMP void *__cdecl _aligned_realloc(void *_Memory,size_t _Size,size_t _Alignment);
  _CRTIMP void *__cdecl _aligned_recalloc(void *_Memory,size_t _Count,size_t _Size,size_t _Alignment);
  _CRTIMP void *__cdecl _aligned_offset_realloc(void *_Memory,size_t _Size,size_t _Alignment,size_t _Offset);
  _CRTIMP void *__cdecl _aligned_offset_recalloc(void *_Memory,size_t _Count,size_t _Size,size_t _Alignment,size_t _Offset);
#endif

#ifndef _WSTDLIB_DEFINED
#define _WSTDLIB_DEFINED

  _CRTIMP wchar_t *__cdecl _itow(int _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ltow(long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ultow(unsigned long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  double __cdecl __mingw_wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl __mingw_wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
  long double __cdecl __mingw_wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);

#if defined(__USE_MINGW_STRTOX)
  __mingw_ovr
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstod(_Str,_EndPtr);
  }
  __mingw_ovr
  float __cdecl wcstof(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstof(_Str,_EndPtr);
  }
  /* wcstold is already a mingw implementation */
#else
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
#endif /* defined(__USE_MINGW_STRTOX) */
#if !defined __NO_ISOCEXT /* in libmingwex.a */
  long double __cdecl wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);
#endif /* __NO_ISOCEXT */
  _CRTIMP double __cdecl _wcstod_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,_locale_t _Locale);
  long __cdecl wcstol(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP long __cdecl _wcstol_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __cdecl wcstoul(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP unsigned long __cdecl _wcstoul_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wgetenv(const wchar_t *_VarName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef _CRT_WSYSTEM_DEFINED
#define _CRT_WSYSTEM_DEFINED
  _CRTIMP int __cdecl _wsystem(const wchar_t *_Command);
#endif
  _CRTIMP double __cdecl _wtof(const wchar_t *_Str);
  _CRTIMP double __cdecl _wtof_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _wtoi(const wchar_t *_Str);
  _CRTIMP int __cdecl _wtoi_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP long __cdecl _wtol(const wchar_t *_Str);
  _CRTIMP long __cdecl _wtol_l(const wchar_t *_Str,_locale_t _Locale);

  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _i64tow(__int64 _Val,wchar_t *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _ui64tow(unsigned __int64 _Val,wchar_t *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64(const wchar_t *_Str);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64_l(const wchar_t *_Str,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64_l(const wchar_t *_Str ,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
#endif

#ifndef _POSIX_
#define _CVTBUFSIZE (309+40)
  _CRTIMP char *__cdecl _fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes);
  _CRTIMP char *__cdecl _ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _gcvt(double _Val,int _NumOfDigits,char *_DstBuf) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _atodbl(_CRT_DOUBLE *_Result,char *_Str);
  _CRTIMP int __cdecl _atoldbl(_LDOUBLE *_Result,char *_Str);
  _CRTIMP int __cdecl _atoflt(_CRT_FLOAT *_Result,char *_Str);
  _CRTIMP int __cdecl _atodbl_l(_CRT_DOUBLE *_Result,char *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _atoldbl_l(_LDOUBLE *_Result,char *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _atoflt_l(_CRT_FLOAT *_Result,char *_Str,_locale_t _Locale);
#pragma push_macro ("_lrotr")
#pragma push_macro ("_lrotl")
#undef _lrotr
#undef _lrotl
#ifdef __x86_64__
  __MINGW_EXTENSION unsigned long long __cdecl _lrotl(unsigned long long _Val,int _Shift);
  __MINGW_EXTENSION unsigned long long __cdecl _lrotr(unsigned long long _Val,int _Shift);
#else
  unsigned long __cdecl _lrotl(unsigned long _Val,int _Shift);
  unsigned long __cdecl _lrotr(unsigned long _Val,int _Shift);
#endif
#pragma pop_macro ("_lrotl")
#pragma pop_macro ("_lrotr")

  _CRTIMP void __cdecl _makepath(char *_Path,const char *_Drive,const char *_Dir,const char *_Filename,const char *_Ext);
  _onexit_t __cdecl _onexit(_onexit_t _Func);

#ifndef _CRT_PERROR_DEFINED
#define _CRT_PERROR_DEFINED
  void __cdecl perror(const char *_ErrMsg);
#endif
  _CRTIMP int __cdecl _putenv(const char *_EnvString);
#pragma push_macro ("_rotr64")
#pragma push_macro ("_rotl64")
#undef _rotl64
#undef _rotr64
  __MINGW_EXTENSION unsigned __int64 __cdecl _rotl64(unsigned __int64 _Val,int _Shift);
  __MINGW_EXTENSION unsigned __int64 __cdecl _rotr64(unsigned __int64 Value,int Shift);
#pragma pop_macro ("_rotl64")
#pragma pop_macro ("_rotr64")
#pragma push_macro ("_rotr")
#pragma push_macro ("_rotl")
#undef _rotr
#undef _rotl
  unsigned int __cdecl _rotr(unsigned int _Val,int _Shift);
  unsigned int __cdecl _rotl(unsigned int _Val,int _Shift);
#pragma pop_macro ("_rotl")
#pragma pop_macro ("_rotr")
  __MINGW_EXTENSION unsigned __int64 __cdecl _rotr64(unsigned __int64 _Val,int _Shift);
  _CRTIMP void __cdecl _searchenv(const char *_Filename,const char *_EnvVar,char *_ResultPath) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _splitpath(const char *_FullPath,char *_Drive,char *_Dir,char *_Filename,char *_Ext) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _swab(char *_Buf1,char *_Buf2,int _SizeInBytes);

#ifndef _WSTDLIBP_DEFINED
#define _WSTDLIBP_DEFINED
  _CRTIMP wchar_t *__cdecl _wfullpath(wchar_t *_FullPath,const wchar_t *_Path,size_t _SizeInWords);
  _CRTIMP void __cdecl _wmakepath(wchar_t *_ResultPath,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);
#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP int __cdecl _wputenv(const wchar_t *_EnvString);
  _CRTIMP void __cdecl _wsearchenv(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _wsplitpath(const wchar_t *_FullPath,wchar_t *_Drive,wchar_t *_Dir,wchar_t *_Filename,wchar_t *_Ext) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#endif

  _CRTIMP void __cdecl _beep(unsigned _Frequency,unsigned _Duration) __MINGW_ATTRIB_DEPRECATED;
  /* Not to be confused with  _set_error_mode (int).  */
  _CRTIMP void __cdecl _seterrormode(int _Mode) __MINGW_ATTRIB_DEPRECATED;
  _CRTIMP void __cdecl _sleep(unsigned long _Duration) __MINGW_ATTRIB_DEPRECATED;
#endif

#ifndef	NO_OLDNAMES
#ifndef _POSIX_
#if 0
#ifndef __cplusplus
#ifndef NOMINMAX
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif
#endif
#endif

#define sys_errlist _sys_errlist
#define sys_nerr _sys_nerr
#define environ _environ
  char *__cdecl ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl gcvt(double _Val,int _NumOfDigits,char *_DstBuf) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl itoa(int _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl ltoa(long _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl putenv(const char *_EnvString) __MINGW_ATTRIB_DEPRECATED_MSVC2005;

#ifndef _CRT_SWAB_DEFINED
#define _CRT_SWAB_DEFINED  /* Also in unistd.h */
  void __cdecl swab(char *_Buf1,char *_Buf2,int _SizeInBytes) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

  char *__cdecl ultoa(unsigned long _Val,char *_Dstbuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  onexit_t __cdecl onexit(onexit_t _Func);
#endif
#endif

#if !defined __NO_ISOCEXT /* externs in static libmingwex.a */

  typedef struct { __MINGW_EXTENSION long long quot, rem; } lldiv_t;

  __MINGW_EXTENSION lldiv_t __cdecl lldiv(long long, long long);

  __MINGW_EXTENSION long long __cdecl llabs(long long);
#ifndef __CRT__NO_INLINE
  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llabs(long long _j) { return (_j >= 0 ? _j : -_j); }
#endif

  __MINGW_EXTENSION long long  __cdecl strtoll(const char * __restrict__, char ** __restrict, int);
  __MINGW_EXTENSION unsigned long long  __cdecl strtoull(const char * __restrict__, char ** __restrict__, int);

  /* these are stubs for MS _i64 versions */
  __MINGW_EXTENSION long long  __cdecl atoll (const char *);

#ifndef __STRICT_ANSI__
  __MINGW_EXTENSION long long  __cdecl wtoll (const wchar_t *);
  __MINGW_EXTENSION char *__cdecl lltoa (long long, char *, int);
  __MINGW_EXTENSION char *__cdecl ulltoa (unsigned long long , char *, int);
  __MINGW_EXTENSION wchar_t *__cdecl lltow (long long, wchar_t *, int);
  __MINGW_EXTENSION wchar_t *__cdecl ulltow (unsigned long long, wchar_t *, int);

  /* __CRT_INLINE using non-ansi functions */
#ifndef __CRT__NO_INLINE
  __MINGW_EXTENSION __CRT_INLINE long long  __cdecl atoll (const char * _c) { return _atoi64 (_c); }
  __MINGW_EXTENSION __CRT_INLINE char *__cdecl lltoa (long long _n, char * _c, int _i) { return _i64toa (_n, _c, _i); }
  __MINGW_EXTENSION __CRT_INLINE char *__cdecl ulltoa (unsigned long long _n, char * _c, int _i) { return _ui64toa (_n, _c, _i); }
  __MINGW_EXTENSION __CRT_INLINE long long  __cdecl wtoll (const wchar_t * _w) { return _wtoi64 (_w); }
  __MINGW_EXTENSION __CRT_INLINE wchar_t *__cdecl lltow (long long _n, wchar_t * _w, int _i) { return _i64tow (_n, _w, _i); }
  __MINGW_EXTENSION __CRT_INLINE wchar_t *__cdecl ulltow (unsigned long long _n, wchar_t * _w, int _i) { return _ui64tow (_n, _w, _i); }
#endif /* !__CRT__NO_INLINE */
#endif /* (__STRICT_ANSI__)  */

#endif /* !__NO_ISOCEXT */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/stdlib_s.h>
#include <malloc.h>

#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_STDLIB
#define _INC_STDLIB

#include <crtdefs.h>
#include <limits.h>

#if defined (__USE_MINGW_ANSI_STDIO) && ((__USE_MINGW_ANSI_STDIO + 0) != 0) && !defined (__USE_MINGW_STRTOX)
#define __USE_MINGW_STRTOX 1
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifndef _ONEXIT_T_DEFINED
#define _ONEXIT_T_DEFINED

  typedef int (__cdecl *_onexit_t)(void);

#ifndef	NO_OLDNAMES
#define onexit_t _onexit_t
#endif
#endif

#ifndef _DIV_T_DEFINED
#define _DIV_T_DEFINED

  typedef struct _div_t {
    int quot;
    int rem;
  } div_t;

  typedef struct _ldiv_t {
    long quot;
    long rem;
  } ldiv_t;
#endif

#ifndef _CRT_DOUBLE_DEC
#define _CRT_DOUBLE_DEC

#pragma pack(4)
  typedef struct {
    unsigned char ld[10];
  } _LDOUBLE;
#pragma pack()

#define _PTR_LD(x) ((unsigned char *)(&(x)->ld))

  typedef struct {
    double x;
  } _CRT_DOUBLE;

  typedef struct {
    float f;
  } _CRT_FLOAT;

#pragma push_macro("long")
#undef long

  typedef struct {
    long double x;
  } _LONGDOUBLE;

#pragma pop_macro("long")

#pragma pack(4)
  typedef struct {
    unsigned char ld12[12];
  } _LDBL12;
#pragma pack()
#endif

#define RAND_MAX 0x7fff

#ifndef MB_CUR_MAX
#define MB_CUR_MAX ___mb_cur_max_func()
#ifndef __mb_cur_max
#ifdef _MSVCRT_
  extern int __mb_cur_max;
#define __mb_cur_max	__mb_cur_max
#else
  extern int * __MINGW_IMP_SYMBOL(__mb_cur_max);
#define __mb_cur_max	(* __MINGW_IMP_SYMBOL(__mb_cur_max))
#endif
#endif
#define ___mb_cur_max_func() (__mb_cur_max)
#endif

#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))

#define _MAX_PATH 260
#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define _MAX_FNAME 256
#define _MAX_EXT 256

#define _OUT_TO_DEFAULT 0
#define _OUT_TO_STDERR 1
#define _OUT_TO_MSGBOX 2
#define _REPORT_ERRMODE 3

#define _WRITE_ABORT_MSG 0x1
#define _CALL_REPORTFAULT 0x2

#define _MAX_ENV 32767

  typedef void (__cdecl *_purecall_handler)(void);

  _CRTIMP _purecall_handler __cdecl _set_purecall_handler(_purecall_handler _Handler);
  _CRTIMP _purecall_handler __cdecl _get_purecall_handler(void);

  typedef void (__cdecl *_invalid_parameter_handler)(const wchar_t *,const wchar_t *,const wchar_t *,unsigned int,uintptr_t);
  _CRTIMP _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(_invalid_parameter_handler _Handler);
  _CRTIMP _invalid_parameter_handler __cdecl _get_invalid_parameter_handler(void);

#ifndef _CRT_ERRNO_DEFINED
#define _CRT_ERRNO_DEFINED
  _CRTIMP extern int *__cdecl _errno(void);
#define errno (*_errno())
  errno_t __cdecl _set_errno(int _Value);
  errno_t __cdecl _get_errno(int *_Value);
#endif
  _CRTIMP unsigned long *__cdecl __doserrno(void);
#define _doserrno (*__doserrno())
  errno_t __cdecl _set_doserrno(unsigned long _Value);
  errno_t __cdecl _get_doserrno(unsigned long *_Value);
#ifdef _MSVCRT_
  extern char *_sys_errlist[];
  extern int _sys_nerr;
#else
  extern _CRTIMP char *_sys_errlist[1];
  extern _CRTIMP int _sys_nerr;
#endif
#if (defined(_X86_) && !defined(__x86_64))
  _CRTIMP int *__cdecl __p___argc(void);
  _CRTIMP char ***__cdecl __p___argv(void);
  _CRTIMP wchar_t ***__cdecl __p___wargv(void);
  _CRTIMP char ***__cdecl __p__environ(void);
  _CRTIMP wchar_t ***__cdecl __p__wenviron(void);
  _CRTIMP char **__cdecl __p__pgmptr(void);
  _CRTIMP wchar_t **__cdecl __p__wpgmptr(void);
#endif
#ifndef __argc
#ifdef _MSVCRT_
  extern int __argc;
#else
  extern int * __MINGW_IMP_SYMBOL(__argc);
#define __argc (* __MINGW_IMP_SYMBOL(__argc))
#endif
#endif
#ifndef __argv
#ifdef _MSVCRT_
  extern char **__argv;
#else
  extern char *** __MINGW_IMP_SYMBOL(__argv);
#define __argv	(* __MINGW_IMP_SYMBOL(__argv))
#endif
#endif
#ifndef __wargv
#ifdef _MSVCRT_
  extern wchar_t **__wargv;
#else
  extern wchar_t *** __MINGW_IMP_SYMBOL(__wargv);
#define __wargv (* __MINGW_IMP_SYMBOL(__wargv))
#endif
#endif

#ifdef _POSIX_
  extern char **environ;
#else
#ifndef _environ
#ifdef _MSVCRT_
  extern char **_environ;
#else
  extern char *** __MINGW_IMP_SYMBOL(_environ);
#define _environ (* __MINGW_IMP_SYMBOL(_environ))
#endif
#endif

#ifndef _wenviron
#ifdef _MSVCRT_
  extern wchar_t **_wenviron;
#else
  extern wchar_t *** __MINGW_IMP_SYMBOL(_wenviron);
#define _wenviron (* __MINGW_IMP_SYMBOL(_wenviron))
#endif
#endif
#endif
#ifndef _pgmptr
#ifdef _MSVCRT_
  extern char *_pgmptr;
#else
  extern char ** __MINGW_IMP_SYMBOL(_pgmptr);
#define _pgmptr	(* __MINGW_IMP_SYMBOL(_pgmptr))
#endif
#endif

#ifndef _wpgmptr
#ifdef _MSVCRT_
  extern wchar_t *_wpgmptr;
#else
  extern wchar_t ** __MINGW_IMP_SYMBOL(_wpgmptr);
#define _wpgmptr (* __MINGW_IMP_SYMBOL(_wpgmptr))
#endif
#endif
  errno_t __cdecl _get_pgmptr(char **_Value);
  errno_t __cdecl _get_wpgmptr(wchar_t **_Value);
#ifndef _fmode
#ifdef _MSVCRT_
  extern int _fmode;
#else
  extern int * __MINGW_IMP_SYMBOL(_fmode);
#define _fmode	(* __MINGW_IMP_SYMBOL(_fmode))
#endif
#endif
  _CRTIMP errno_t __cdecl _set_fmode(int _Mode);
  _CRTIMP errno_t __cdecl _get_fmode(int *_PMode);

#ifndef _osplatform
#ifdef _MSVCRT_
  extern unsigned int _osplatform;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_osplatform);
#define _osplatform (* __MINGW_IMP_SYMBOL(_osplatform))
#endif
#endif

#ifndef _osver
#ifdef _MSVCRT_
  extern unsigned int _osver;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_osver);
#define _osver	(* __MINGW_IMP_SYMBOL(_osver))
#endif
#endif

#ifndef _winver
#ifdef _MSVCRT_
  extern unsigned int _winver;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_winver);
#define _winver	(* __MINGW_IMP_SYMBOL(_winver))
#endif
#endif

#ifndef _winmajor
#ifdef _MSVCRT_
  extern unsigned int _winmajor;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_winmajor);
#define _winmajor (* __MINGW_IMP_SYMBOL(_winmajor))
#endif
#endif

#ifndef _winminor
#ifdef _MSVCRT_
  extern unsigned int _winminor;
#else
  extern unsigned int * __MINGW_IMP_SYMBOL(_winminor);
#define _winminor (* __MINGW_IMP_SYMBOL(_winminor))
#endif
#endif

  errno_t __cdecl _get_osplatform(unsigned int *_Value);
  errno_t __cdecl _get_osver(unsigned int *_Value);
  errno_t __cdecl _get_winver(unsigned int *_Value);
  errno_t __cdecl _get_winmajor(unsigned int *_Value);
  errno_t __cdecl _get_winminor(unsigned int *_Value);
#ifndef _countof
#ifndef __cplusplus
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#else
  extern "C++" {
    template <typename _CountofType,size_t _SizeOfArray> char (*__countof_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
  }
#endif
#endif

#ifndef _CRT_TERMINATE_DEFINED
#define _CRT_TERMINATE_DEFINED
  void __cdecl __MINGW_NOTHROW exit(int _Code) __MINGW_ATTRIB_NORETURN;
  void __cdecl __MINGW_NOTHROW _exit(int _Code) __MINGW_ATTRIB_NORETURN;

#if !defined __NO_ISOCEXT /* extern stub in static libmingwex.a */
  /* C99 function name */
  void __cdecl _Exit(int) __MINGW_ATTRIB_NORETURN;
#ifndef __CRT__NO_INLINE
  __CRT_INLINE __MINGW_ATTRIB_NORETURN void  __cdecl _Exit(int status)
  {  _exit(status); }
#endif /* !__CRT__NO_INLINE */
#endif /* Not  __NO_ISOCEXT */

#pragma push_macro("abort")
#undef abort
  void __cdecl __declspec(noreturn) abort(void);
#pragma pop_macro("abort")

#endif /* _CRT_TERMINATE_DEFINED */

  _CRTIMP unsigned int __cdecl _set_abort_behavior(unsigned int _Flags,unsigned int _Mask);

#ifndef _CRT_ABS_DEFINED
#define _CRT_ABS_DEFINED
  int __cdecl abs(int _X);
  long __cdecl labs(long _X);
#endif

  __MINGW_EXTENSION __int64 __cdecl _abs64(__int64);
#ifdef __MINGW_INTRIN_INLINE
  __MINGW_INTRIN_INLINE __int64 __cdecl _abs64(__int64 x) {
    return __builtin_llabs(x);
  }
#endif

  int __cdecl atexit(void (__cdecl *)(void));
#ifndef _CRT_ATOF_DEFINED
#define _CRT_ATOF_DEFINED
  double __cdecl atof(const char *_String);
  double __cdecl _atof_l(const char *_String,_locale_t _Locale);
#endif
  int __cdecl atoi(const char *_Str);
  _CRTIMP int __cdecl _atoi_l(const char *_Str,_locale_t _Locale);
  long __cdecl atol(const char *_Str);
  _CRTIMP long __cdecl _atol_l(const char *_Str,_locale_t _Locale);
#ifndef _CRT_ALGO_DEFINED
#define _CRT_ALGO_DEFINED
  void *__cdecl bsearch(const void *_Key,const void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__cdecl *_PtFuncCompare)(const void *,const void *));
  void __cdecl qsort(void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__cdecl *_PtFuncCompare)(const void *,const void *));
#endif
  unsigned short __cdecl _byteswap_ushort(unsigned short _Short);
  unsigned long __cdecl _byteswap_ulong (unsigned long _Long);
  __MINGW_EXTENSION unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64 _Int64);
  div_t __cdecl div(int _Numerator,int _Denominator);
  char *__cdecl getenv(const char *_VarName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _itoa(int _Value,char *_Dest,int _Radix);
  __MINGW_EXTENSION _CRTIMP char *__cdecl _i64toa(__int64 _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP char *__cdecl _ui64toa(unsigned __int64 _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _atoi64(const char *_String);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _atoi64_l(const char *_String,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _strtoi64(const char *_String,char **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _strtoi64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _strtoui64(const char *_String,char **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _strtoui64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  ldiv_t __cdecl ldiv(long _Numerator,long _Denominator);
  _CRTIMP char *__cdecl _ltoa(long _Value,char *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl mblen(const char *_Ch,size_t _MaxCount);
  _CRTIMP int __cdecl _mblen_l(const char *_Ch,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP size_t __cdecl _mbstrlen(const char *_Str);
  _CRTIMP size_t __cdecl _mbstrlen_l(const char *_Str,_locale_t _Locale);
  _CRTIMP size_t __cdecl _mbstrnlen(const char *_Str,size_t _MaxCount);
  _CRTIMP size_t __cdecl _mbstrnlen_l(const char *_Str,size_t _MaxCount,_locale_t _Locale);
  int __cdecl mbtowc(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes);
  _CRTIMP int __cdecl _mbtowc_l(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes,_locale_t _Locale);
  size_t __cdecl mbstowcs(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount);
  _CRTIMP size_t __cdecl _mbstowcs_l(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale);
  int __cdecl rand(void);
  _CRTIMP int __cdecl _set_error_mode(int _Mode);
  void __cdecl srand(unsigned int _Seed);
#ifdef _CRT_RAND_S
  _CRTIMP errno_t __cdecl rand_s(unsigned int *randomValue);
#endif
#if defined(__USE_MINGW_STRTOX)
__mingw_ovr
double __cdecl __MINGW_NOTHROW strtod(const char * __restrict__ _Str,char ** __restrict__ _EndPtr)
{
  double __cdecl __mingw_strtod (const char * __restrict__, char ** __restrict__);
  return __mingw_strtod( _Str, _EndPtr);
}

__mingw_ovr
float __cdecl __MINGW_NOTHROW strtof(const char * __restrict__ _Str,char ** __restrict__ _EndPtr)
{
  float __cdecl __mingw_strtof (const char * __restrict__, char ** __restrict__);
  return __mingw_strtof( _Str, _EndPtr);
}

/* strtold is already an alias to __mingw_strtold */
#else
  double __cdecl __MINGW_NOTHROW strtod(const char * __restrict__ _Str,char ** __restrict__ _EndPtr);
  float __cdecl __MINGW_NOTHROW strtof(const char * __restrict__ nptr, char ** __restrict__ endptr);
#endif /* defined(__USE_MINGW_STRTOX) */
  long double __cdecl __MINGW_NOTHROW strtold(const char * __restrict__ , char ** __restrict__ );
#if !defined __NO_ISOCEXT
  /* libmingwex.a provides a c99-compliant strtod() exported as __strtod() */
  extern double __cdecl __MINGW_NOTHROW
  __strtod (const char * __restrict__ , char ** __restrict__);
#define strtod __strtod
#endif /* __NO_ISOCEXT */

#if !defined __NO_ISOCEXT  /* in libmingwex.a */
  float __cdecl __mingw_strtof (const char * __restrict__, char ** __restrict__);
  double __cdecl __mingw_strtod (const char * __restrict__, char ** __restrict__);
  long double __cdecl __mingw_strtold(const char * __restrict__, char ** __restrict__);
#endif /* __NO_ISOCEXT */
  _CRTIMP double __cdecl _strtod_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,_locale_t _Locale);
  long __cdecl strtol(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP long __cdecl _strtol_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __cdecl strtoul(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP unsigned long __cdecl _strtoul_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
#ifndef _CRT_SYSTEM_DEFINED
#define _CRT_SYSTEM_DEFINED
  int __cdecl system(const char *_Command);
#endif
  _CRTIMP char *__cdecl _ultoa(unsigned long _Value,char *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wctomb(char *_MbCh,wchar_t _WCh) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wctomb_l(char *_MbCh,wchar_t _WCh,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcstombs(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP size_t __cdecl _wcstombs_l(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef _CRT_ALLOCATION_DEFINED
#define _CRT_ALLOCATION_DEFINED
  void *__cdecl calloc(size_t _NumOfElements,size_t _SizeOfElements);
  void __cdecl free(void *_Memory);
  void *__cdecl malloc(size_t _Size);
  void *__cdecl realloc(void *_Memory,size_t _NewSize);
  _CRTIMP void *__cdecl _recalloc(void *_Memory,size_t _Count,size_t _Size);
/* Make sure that X86intrin.h doesn't produce here collisions.  */
#if (!defined (_XMMINTRIN_H_INCLUDED) && !defined (_MM_MALLOC_H_INCLUDED)) || defined(_aligned_malloc)
#pragma push_macro("_aligned_free")
#pragma push_macro("_aligned_malloc")
#undef _aligned_free
#undef _aligned_malloc
  _CRTIMP void __cdecl _aligned_free(void *_Memory);
  _CRTIMP void *__cdecl _aligned_malloc(size_t _Size,size_t _Alignment);
#pragma pop_macro("_aligned_free")
#pragma pop_macro("_aligned_malloc")
#endif
  _CRTIMP void *__cdecl _aligned_offset_malloc(size_t _Size,size_t _Alignment,size_t _Offset);
  _CRTIMP void *__cdecl _aligned_realloc(void *_Memory,size_t _Size,size_t _Alignment);
  _CRTIMP void *__cdecl _aligned_recalloc(void *_Memory,size_t _Count,size_t _Size,size_t _Alignment);
  _CRTIMP void *__cdecl _aligned_offset_realloc(void *_Memory,size_t _Size,size_t _Alignment,size_t _Offset);
  _CRTIMP void *__cdecl _aligned_offset_recalloc(void *_Memory,size_t _Count,size_t _Size,size_t _Alignment,size_t _Offset);
#endif

#ifndef _WSTDLIB_DEFINED
#define _WSTDLIB_DEFINED

  _CRTIMP wchar_t *__cdecl _itow(int _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ltow(long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ultow(unsigned long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

  double __cdecl __mingw_wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl __mingw_wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
  long double __cdecl __mingw_wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);

#if defined(__USE_MINGW_STRTOX)
  __mingw_ovr
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstod(_Str,_EndPtr);
  }
  __mingw_ovr
  float __cdecl wcstof(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstof(_Str,_EndPtr);
  }
  /* wcstold is already a mingw implementation */
#else
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
#endif /* defined(__USE_MINGW_STRTOX) */
#if !defined __NO_ISOCEXT /* in libmingwex.a */
  long double __cdecl wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);
#endif /* __NO_ISOCEXT */
  _CRTIMP double __cdecl _wcstod_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,_locale_t _Locale);
  long __cdecl wcstol(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP long __cdecl _wcstol_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __cdecl wcstoul(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP unsigned long __cdecl _wcstoul_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wgetenv(const wchar_t *_VarName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef _CRT_WSYSTEM_DEFINED
#define _CRT_WSYSTEM_DEFINED
  _CRTIMP int __cdecl _wsystem(const wchar_t *_Command);
#endif
  _CRTIMP double __cdecl _wtof(const wchar_t *_Str);
  _CRTIMP double __cdecl _wtof_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _wtoi(const wchar_t *_Str);
  _CRTIMP int __cdecl _wtoi_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP long __cdecl _wtol(const wchar_t *_Str);
  _CRTIMP long __cdecl _wtol_l(const wchar_t *_Str,_locale_t _Locale);

  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _i64tow(__int64 _Val,wchar_t *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _ui64tow(unsigned __int64 _Val,wchar_t *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64(const wchar_t *_Str);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64_l(const wchar_t *_Str,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64_l(const wchar_t *_Str ,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
#endif

#ifndef _POSIX_
#define _CVTBUFSIZE (309+40)
  _CRTIMP char *__cdecl _fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes);
  _CRTIMP char *__cdecl _ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _gcvt(double _Val,int _NumOfDigits,char *_DstBuf) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _atodbl(_CRT_DOUBLE *_Result,char *_Str);
  _CRTIMP int __cdecl _atoldbl(_LDOUBLE *_Result,char *_Str);
  _CRTIMP int __cdecl _atoflt(_CRT_FLOAT *_Result,char *_Str);
  _CRTIMP int __cdecl _atodbl_l(_CRT_DOUBLE *_Result,char *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _atoldbl_l(_LDOUBLE *_Result,char *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _atoflt_l(_CRT_FLOAT *_Result,char *_Str,_locale_t _Locale);
#pragma push_macro ("_lrotr")
#pragma push_macro ("_lrotl")
#undef _lrotr
#undef _lrotl
#ifdef __x86_64__
  __MINGW_EXTENSION unsigned long long __cdecl _lrotl(unsigned long long _Val,int _Shift);
  __MINGW_EXTENSION unsigned long long __cdecl _lrotr(unsigned long long _Val,int _Shift);
#else
  unsigned long __cdecl _lrotl(unsigned long _Val,int _Shift);
  unsigned long __cdecl _lrotr(unsigned long _Val,int _Shift);
#endif
#pragma pop_macro ("_lrotl")
#pragma pop_macro ("_lrotr")

  _CRTIMP void __cdecl _makepath(char *_Path,const char *_Drive,const char *_Dir,const char *_Filename,const char *_Ext);
  _onexit_t __cdecl _onexit(_onexit_t _Func);

#ifndef _CRT_PERROR_DEFINED
#define _CRT_PERROR_DEFINED
  void __cdecl perror(const char *_ErrMsg);
#endif
  _CRTIMP int __cdecl _putenv(const char *_EnvString);
#pragma push_macro ("_rotr64")
#pragma push_macro ("_rotl64")
#undef _rotl64
#undef _rotr64
  __MINGW_EXTENSION unsigned __int64 __cdecl _rotl64(unsigned __int64 _Val,int _Shift);
  __MINGW_EXTENSION unsigned __int64 __cdecl _rotr64(unsigned __int64 Value,int Shift);
#pragma pop_macro ("_rotl64")
#pragma pop_macro ("_rotr64")
#pragma push_macro ("_rotr")
#pragma push_macro ("_rotl")
#undef _rotr
#undef _rotl
  unsigned int __cdecl _rotr(unsigned int _Val,int _Shift);
  unsigned int __cdecl _rotl(unsigned int _Val,int _Shift);
#pragma pop_macro ("_rotl")
#pragma pop_macro ("_rotr")
  __MINGW_EXTENSION unsigned __int64 __cdecl _rotr64(unsigned __int64 _Val,int _Shift);
  _CRTIMP void __cdecl _searchenv(const char *_Filename,const char *_EnvVar,char *_ResultPath) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _splitpath(const char *_FullPath,char *_Drive,char *_Dir,char *_Filename,char *_Ext) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _swab(char *_Buf1,char *_Buf2,int _SizeInBytes);

#ifndef _WSTDLIBP_DEFINED
#define _WSTDLIBP_DEFINED
  _CRTIMP wchar_t *__cdecl _wfullpath(wchar_t *_FullPath,const wchar_t *_Path,size_t _SizeInWords);
  _CRTIMP void __cdecl _wmakepath(wchar_t *_ResultPath,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);
#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP int __cdecl _wputenv(const wchar_t *_EnvString);
  _CRTIMP void __cdecl _wsearchenv(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _wsplitpath(const wchar_t *_FullPath,wchar_t *_Drive,wchar_t *_Dir,wchar_t *_Filename,wchar_t *_Ext) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#endif

  _CRTIMP void __cdecl _beep(unsigned _Frequency,unsigned _Duration) __MINGW_ATTRIB_DEPRECATED;
  /* Not to be confused with  _set_error_mode (int).  */
  _CRTIMP void __cdecl _seterrormode(int _Mode) __MINGW_ATTRIB_DEPRECATED;
  _CRTIMP void __cdecl _sleep(unsigned long _Duration) __MINGW_ATTRIB_DEPRECATED;
#endif

#ifndef	NO_OLDNAMES
#ifndef _POSIX_
#if 0
#ifndef __cplusplus
#ifndef NOMINMAX
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif
#endif
#endif

#define sys_errlist _sys_errlist
#define sys_nerr _sys_nerr
#define environ _environ
  char *__cdecl ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl gcvt(double _Val,int _NumOfDigits,char *_DstBuf) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl itoa(int _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl ltoa(long _Val,char *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl putenv(const char *_EnvString) __MINGW_ATTRIB_DEPRECATED_MSVC2005;

#ifndef _CRT_SWAB_DEFINED
#define _CRT_SWAB_DEFINED  /* Also in unistd.h */
  void __cdecl swab(char *_Buf1,char *_Buf2,int _SizeInBytes) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

  char *__cdecl ultoa(unsigned long _Val,char *_Dstbuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  onexit_t __cdecl onexit(onexit_t _Func);
#endif
#endif

#if !defined __NO_ISOCEXT /* externs in static libmingwex.a */

  typedef struct { __MINGW_EXTENSION long long quot, rem; } lldiv_t;

  __MINGW_EXTENSION lldiv_t __cdecl lldiv(long long, long long);

  __MINGW_EXTENSION long long __cdecl llabs(long long);
#ifndef __CRT__NO_INLINE
  __MINGW_EXTENSION __CRT_INLINE long long __cdecl llabs(long long _j) { return (_j >= 0 ? _j : -_j); }
#endif

  __MINGW_EXTENSION long long  __cdecl strtoll(const char * __restrict__, char ** __restrict, int);
  __MINGW_EXTENSION unsigned long long  __cdecl strtoull(const char * __restrict__, char ** __restrict__, int);

  /* these are stubs for MS _i64 versions */
  __MINGW_EXTENSION long long  __cdecl atoll (const char *);

#ifndef __STRICT_ANSI__
  __MINGW_EXTENSION long long  __cdecl wtoll (const wchar_t *);
  __MINGW_EXTENSION char *__cdecl lltoa (long long, char *, int);
  __MINGW_EXTENSION char *__cdecl ulltoa (unsigned long long , char *, int);
  __MINGW_EXTENSION wchar_t *__cdecl lltow (long long, wchar_t *, int);
  __MINGW_EXTENSION wchar_t *__cdecl ulltow (unsigned long long, wchar_t *, int);

  /* __CRT_INLINE using non-ansi functions */
#ifndef __CRT__NO_INLINE
  __MINGW_EXTENSION __CRT_INLINE long long  __cdecl atoll (const char * _c) { return _atoi64 (_c); }
  __MINGW_EXTENSION __CRT_INLINE char *__cdecl lltoa (long long _n, char * _c, int _i) { return _i64toa (_n, _c, _i); }
  __MINGW_EXTENSION __CRT_INLINE char *__cdecl ulltoa (unsigned long long _n, char * _c, int _i) { return _ui64toa (_n, _c, _i); }
  __MINGW_EXTENSION __CRT_INLINE long long  __cdecl wtoll (const wchar_t * _w) { return _wtoi64 (_w); }
  __MINGW_EXTENSION __CRT_INLINE wchar_t *__cdecl lltow (long long _n, wchar_t * _w, int _i) { return _i64tow (_n, _w, _i); }
  __MINGW_EXTENSION __CRT_INLINE wchar_t *__cdecl ulltow (unsigned long long _n, wchar_t * _w, int _i) { return _ui64tow (_n, _w, _i); }
#endif /* !__CRT__NO_INLINE */
#endif /* (__STRICT_ANSI__)  */

#endif /* !__NO_ISOCEXT */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/stdlib_s.h>
#include <malloc.h>

#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_STRING
#define _INC_STRING

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NLSCMP_DEFINED
#define _NLSCMP_DEFINED
#define _NLSCMPERROR 2147483647
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#define _WConst_return _CONST_RETURN

#ifndef _CRT_MEMORY_DEFINED
#define _CRT_MEMORY_DEFINED
  _CRTIMP void *__cdecl _memccpy(void *_Dst,const void *_Src,int _Val,size_t _MaxCount);
  _CONST_RETURN void *__cdecl memchr(const void *_Buf ,int _Val,size_t _MaxCount);
  _CRTIMP int __cdecl _memicmp(const void *_Buf1,const void *_Buf2,size_t _Size);
  _CRTIMP int __cdecl _memicmp_l(const void *_Buf1,const void *_Buf2,size_t _Size,_locale_t _Locale);
  int __cdecl memcmp(const void *_Buf1,const void *_Buf2,size_t _Size);
  void * __cdecl memcpy(void * __restrict__ _Dst,const void * __restrict__ _Src,size_t _Size) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  void * __cdecl mempcpy (void *_Dst, const void *_Src, size_t _Size);
  void * __cdecl memset(void *_Dst,int _Val,size_t _Size);
#ifndef	NO_OLDNAMES
  void * __cdecl memccpy(void *_Dst,const void *_Src,int _Val,size_t _Size) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl memicmp(const void *_Buf1,const void *_Buf2,size_t _Size) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif
  char * __cdecl _strset(char *_Str,int _Val) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char * __cdecl _strset_l(char *_Str,int _Val,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char * __cdecl strcpy(char * __restrict__ _Dest,const char * __restrict__ _Source);
  char * __cdecl strcat(char * __restrict__ _Dest,const char * __restrict__ _Source);
  int __cdecl strcmp(const char *_Str1,const char *_Str2);
  size_t __cdecl strlen(const char *_Str);
  size_t __cdecl strnlen(const char *_Str,size_t _MaxCount);
  void *__cdecl memmove(void *_Dst,const void *_Src,size_t _Size) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strdup(const char *_Src);
  _CONST_RETURN char *__cdecl strchr(const char *_Str,int _Val);
  _CRTIMP int __cdecl _stricmp(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _strcmpi(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _stricmp_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  int __cdecl strcoll(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _strcoll_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _stricoll(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _stricoll_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _strncoll (const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strncoll_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP int __cdecl _strnicoll (const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strnicoll_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  size_t __cdecl strcspn(const char *_Str,const char *_Control);
  _CRTIMP char *__cdecl _strerror(const char *_ErrMsg) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl strerror(int) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strlwr(char *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *strlwr_l(char *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl strncat(char * __restrict__ _Dest,const char * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl strncmp(const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strnicmp(const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strnicmp_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  char *strncpy(char * __restrict__ _Dest,const char * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strnset(char *_Str,int _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strnset_l(char *str,int c,size_t count,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN char *__cdecl strpbrk(const char *_Str,const char *_Control);
  _CONST_RETURN char *__cdecl strrchr(const char *_Str,int _Ch);
  _CRTIMP char *__cdecl _strrev(char *_Str);
  size_t __cdecl strspn(const char *_Str,const char *_Control);
  _CONST_RETURN char *__cdecl strstr(const char *_Str,const char *_SubStr);
  char *__cdecl strtok(char * __restrict__ _Str,const char * __restrict__ _Delim) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#pragma push_macro("strtok_r")
#undef strtok_r
  char *strtok_r(char * __restrict__ _Str, const char * __restrict__ _Delim, char ** __restrict__ __last);
#pragma pop_macro("strtok_r")
  _CRTIMP char *__cdecl _strupr(char *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *_strupr_l(char *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl strxfrm(char * __restrict__ _Dst,const char * __restrict__ _Src,size_t _MaxCount);
  _CRTIMP size_t __cdecl _strxfrm_l(char * __restrict__ _Dst,const char * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);

#ifndef	NO_OLDNAMES
  char *__cdecl strdup(const char *_Src) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl strcmpi(const char *_Str1,const char *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl stricmp(const char *_Str1,const char *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strlwr(char *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl strnicmp(const char *_Str1,const char *_Str,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl strncasecmp (const char *, const char *, size_t);
  int __cdecl strcasecmp (const char *, const char *);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl strncasecmp (const char *__sz1, const char *__sz2, size_t __sizeMaxCompare) { return _strnicmp (__sz1, __sz2, __sizeMaxCompare); }
  __CRT_INLINE int __cdecl strcasecmp (const char *__sz1, const char *__sz2) { return _stricmp (__sz1, __sz2); }
#else
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif /* !__CRT__NO_INLINE */
  char *__cdecl strnset(char *_Str,int _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strrev(char *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strset(char *_Str,int _Val) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strupr(char *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

#ifndef _WSTRING_DEFINED
#define _WSTRING_DEFINED

  _CRTIMP wchar_t *__cdecl _wcsdup(const wchar_t *_Str);
  wchar_t *__cdecl wcscat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcschr(const wchar_t *_Str,wchar_t _Ch);
  int __cdecl wcscmp(const wchar_t *_Str1,const wchar_t *_Str2);
  wchar_t *__cdecl wcscpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcscspn(const wchar_t *_Str,const wchar_t *_Control);
  size_t __cdecl wcslen(const wchar_t *_Str);
  size_t __cdecl wcsnlen(const wchar_t *_Src,size_t _MaxCount);
  wchar_t *wcsncat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wcsncmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  wchar_t *wcsncpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  wchar_t *__cdecl _wcsncpy_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcspbrk(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsrchr(const wchar_t *_Str,wchar_t _Ch);
  size_t __cdecl wcsspn(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsstr(const wchar_t *_Str,const wchar_t *_SubStr);
  wchar_t *__cdecl wcstok(wchar_t * __restrict__ _Str,const wchar_t * __restrict__ _Delim) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcserror(int _ErrNum) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl __wcserror(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsrev(wchar_t *_Str);
  _CRTIMP wchar_t *__cdecl _wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcslwr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcslwr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsupr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcsupr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsxfrm(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount);
  _CRTIMP size_t __cdecl _wcsxfrm_l(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);
  int __cdecl wcscoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcscoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsncoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsncoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);

#ifndef	NO_OLDNAMES
  wchar_t *__cdecl wcsdup(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#define wcswcs wcsstr
  int __cdecl wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsrev(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcslwr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsupr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif

#ifdef __cplusplus
}
#endif

#include <sec_api/string_s.h>
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_STRING
#define _INC_STRING

#include <crtdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NLSCMP_DEFINED
#define _NLSCMP_DEFINED
#define _NLSCMPERROR 2147483647
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#define _WConst_return _CONST_RETURN

#ifndef _CRT_MEMORY_DEFINED
#define _CRT_MEMORY_DEFINED
  _CRTIMP void *__cdecl _memccpy(void *_Dst,const void *_Src,int _Val,size_t _MaxCount);
  _CONST_RETURN void *__cdecl memchr(const void *_Buf ,int _Val,size_t _MaxCount);
  _CRTIMP int __cdecl _memicmp(const void *_Buf1,const void *_Buf2,size_t _Size);
  _CRTIMP int __cdecl _memicmp_l(const void *_Buf1,const void *_Buf2,size_t _Size,_locale_t _Locale);
  int __cdecl memcmp(const void *_Buf1,const void *_Buf2,size_t _Size);
  void * __cdecl memcpy(void * __restrict__ _Dst,const void * __restrict__ _Src,size_t _Size) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  void * __cdecl mempcpy (void *_Dst, const void *_Src, size_t _Size);
  void * __cdecl memset(void *_Dst,int _Val,size_t _Size);
#ifndef	NO_OLDNAMES
  void * __cdecl memccpy(void *_Dst,const void *_Src,int _Val,size_t _Size) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl memicmp(const void *_Buf1,const void *_Buf2,size_t _Size) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif
  char * __cdecl _strset(char *_Str,int _Val) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char * __cdecl _strset_l(char *_Str,int _Val,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char * __cdecl strcpy(char * __restrict__ _Dest,const char * __restrict__ _Source);
  char * __cdecl strcat(char * __restrict__ _Dest,const char * __restrict__ _Source);
  int __cdecl strcmp(const char *_Str1,const char *_Str2);
  size_t __cdecl strlen(const char *_Str);
  size_t __cdecl strnlen(const char *_Str,size_t _MaxCount);
  void *__cdecl memmove(void *_Dst,const void *_Src,size_t _Size) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strdup(const char *_Src);
  _CONST_RETURN char *__cdecl strchr(const char *_Str,int _Val);
  _CRTIMP int __cdecl _stricmp(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _strcmpi(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _stricmp_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  int __cdecl strcoll(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _strcoll_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _stricoll(const char *_Str1,const char *_Str2);
  _CRTIMP int __cdecl _stricoll_l(const char *_Str1,const char *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _strncoll (const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strncoll_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP int __cdecl _strnicoll (const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strnicoll_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  size_t __cdecl strcspn(const char *_Str,const char *_Control);
  _CRTIMP char *__cdecl _strerror(const char *_ErrMsg) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl strerror(int) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strlwr(char *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *strlwr_l(char *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl strncat(char * __restrict__ _Dest,const char * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl strncmp(const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strnicmp(const char *_Str1,const char *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _strnicmp_l(const char *_Str1,const char *_Str2,size_t _MaxCount,_locale_t _Locale);
  char *strncpy(char * __restrict__ _Dest,const char * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strnset(char *_Str,int _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strnset_l(char *str,int c,size_t count,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN char *__cdecl strpbrk(const char *_Str,const char *_Control);
  _CONST_RETURN char *__cdecl strrchr(const char *_Str,int _Ch);
  _CRTIMP char *__cdecl _strrev(char *_Str);
  size_t __cdecl strspn(const char *_Str,const char *_Control);
  _CONST_RETURN char *__cdecl strstr(const char *_Str,const char *_SubStr);
  char *__cdecl strtok(char * __restrict__ _Str,const char * __restrict__ _Delim) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#pragma push_macro("strtok_r")
#undef strtok_r
  char *strtok_r(char * __restrict__ _Str, const char * __restrict__ _Delim, char ** __restrict__ __last);
#pragma pop_macro("strtok_r")
  _CRTIMP char *__cdecl _strupr(char *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *_strupr_l(char *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl strxfrm(char * __restrict__ _Dst,const char * __restrict__ _Src,size_t _MaxCount);
  _CRTIMP size_t __cdecl _strxfrm_l(char * __restrict__ _Dst,const char * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);

#ifndef	NO_OLDNAMES
  char *__cdecl strdup(const char *_Src) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl strcmpi(const char *_Str1,const char *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl stricmp(const char *_Str1,const char *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strlwr(char *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl strnicmp(const char *_Str1,const char *_Str,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl strncasecmp (const char *, const char *, size_t);
  int __cdecl strcasecmp (const char *, const char *);
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl strncasecmp (const char *__sz1, const char *__sz2, size_t __sizeMaxCompare) { return _strnicmp (__sz1, __sz2, __sizeMaxCompare); }
  __CRT_INLINE int __cdecl strcasecmp (const char *__sz1, const char *__sz2) { return _stricmp (__sz1, __sz2); }
#else
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif /* !__CRT__NO_INLINE */
  char *__cdecl strnset(char *_Str,int _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strrev(char *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strset(char *_Str,int _Val) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  char *__cdecl strupr(char *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

#ifndef _WSTRING_DEFINED
#define _WSTRING_DEFINED

  _CRTIMP wchar_t *__cdecl _wcsdup(const wchar_t *_Str);
  wchar_t *__cdecl wcscat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcschr(const wchar_t *_Str,wchar_t _Ch);
  int __cdecl wcscmp(const wchar_t *_Str1,const wchar_t *_Str2);
  wchar_t *__cdecl wcscpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcscspn(const wchar_t *_Str,const wchar_t *_Control);
  size_t __cdecl wcslen(const wchar_t *_Str);
  size_t __cdecl wcsnlen(const wchar_t *_Src,size_t _MaxCount);
  wchar_t *wcsncat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wcsncmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  wchar_t *wcsncpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  wchar_t *__cdecl _wcsncpy_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcspbrk(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsrchr(const wchar_t *_Str,wchar_t _Ch);
  size_t __cdecl wcsspn(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsstr(const wchar_t *_Str,const wchar_t *_SubStr);
  wchar_t *__cdecl wcstok(wchar_t * __restrict__ _Str,const wchar_t * __restrict__ _Delim) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcserror(int _ErrNum) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl __wcserror(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsrev(wchar_t *_Str);
  _CRTIMP wchar_t *__cdecl _wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcslwr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcslwr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsupr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcsupr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsxfrm(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount);
  _CRTIMP size_t __cdecl _wcsxfrm_l(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);
  int __cdecl wcscoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcscoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsncoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsncoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);

#ifndef	NO_OLDNAMES
  wchar_t *__cdecl wcsdup(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#define wcswcs wcsstr
  int __cdecl wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsrev(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcslwr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsupr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif

#ifdef __cplusplus
}
#endif

#include <sec_api/string_s.h>
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _TIME_H_
#define _TIME_H_

#include <crtdefs.h>

#ifndef _WIN32
#error Only Win32 target is supported!
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRTIMP
#define _CRTIMP __declspec(dllimport)
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
  typedef unsigned short wchar_t;
#endif

#ifndef _TIME32_T_DEFINED
#define _TIME32_T_DEFINED
  typedef long __time32_t;
#endif

#ifndef _TIME64_T_DEFINED
#define _TIME64_T_DEFINED
  __MINGW_EXTENSION typedef __int64 __time64_t;
#endif

#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
#ifdef _USE_32BIT_TIME_T
  typedef __time32_t time_t;
#else
  typedef __time64_t time_t;
#endif
#endif

#ifndef _CLOCK_T_DEFINED
#define _CLOCK_T_DEFINED
  typedef long clock_t;
#endif

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
#undef size_t
#ifdef _WIN64
  __MINGW_EXTENSION typedef unsigned __int64 size_t;
#else
  typedef unsigned int size_t;
#endif
#endif

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
#undef ssize_t
#ifdef _WIN64
  __MINGW_EXTENSION typedef __int64 ssize_t;
#else
  typedef int ssize_t;
#endif
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef _TM_DEFINED
#define _TM_DEFINED
  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };
#endif

#define CLOCKS_PER_SEC 1000

  __MINGW_IMPORT int _daylight;
  __MINGW_IMPORT long _dstbias;
  __MINGW_IMPORT long _timezone;
  __MINGW_IMPORT char * _tzname[2];

  _CRTIMP errno_t __cdecl _get_daylight(int *_Daylight);
  _CRTIMP errno_t __cdecl _get_dstbias(long *_Daylight_savings_bias);
  _CRTIMP errno_t __cdecl _get_timezone(long *_Timezone);
  _CRTIMP errno_t __cdecl _get_tzname(size_t *_ReturnValue,char *_Buffer,size_t _SizeInBytes,int _Index);
  char *__cdecl asctime(const struct tm *_Tm) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl _ctime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  clock_t __cdecl clock(void);
  double __cdecl _difftime32(__time32_t _Time1,__time32_t _Time2);
  struct tm *__cdecl _gmtime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  struct tm *__cdecl _localtime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl strftime(char * __restrict__ _Buf,size_t _SizeInBytes,const char * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  _CRTIMP size_t __cdecl _strftime_l(char * __restrict__ _Buf,size_t _Max_size,const char * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  _CRTIMP char *__cdecl _strdate(char *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strtime(char *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __time32_t __cdecl _time32(__time32_t *_Time);
  __time32_t __cdecl _mktime32(struct tm *_Tm);
  __time32_t __cdecl _mkgmtime32(struct tm *_Tm);
#if defined (_POSIX_) || defined(__GNUC__)
  void __cdecl tzset(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#if !defined (_POSIX_)
  _CRTIMP void __cdecl _tzset(void);
#endif

  double __cdecl _difftime64(__time64_t _Time1,__time64_t _Time2);
  _CRTIMP char *__cdecl _ctime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP struct tm *__cdecl _gmtime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP struct tm *__cdecl _localtime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP __time64_t __cdecl _mktime64(struct tm *_Tm);
  _CRTIMP __time64_t __cdecl _mkgmtime64(struct tm *_Tm);
  _CRTIMP __time64_t __cdecl _time64(__time64_t *_Time);
  unsigned __cdecl _getsystime(struct tm *_Tm);
  unsigned __cdecl _setsystime(struct tm *_Tm,unsigned _MilliSec);

#ifndef _WTIME_DEFINED
  _CRTIMP wchar_t *__cdecl _wasctime(const struct tm *_Tm);
  wchar_t *__cdecl _wctime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsftime(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  _CRTIMP size_t __cdecl _wcsftime_l(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wstrdate(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wstrtime(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wctime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#if !defined (RC_INVOKED) && !defined (_INC_WTIME_INL)
#define _INC_WTIME_INL
  wchar_t *__cdecl _wctime(const time_t *) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __CRT__NO_INLINE
#ifndef _USE_32BIT_TIME_T
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime64(_Time); }
#else
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime32(_Time); }
#endif
#endif /* __CRT__NO_INLINE */
#endif

#define _WTIME_DEFINED
#endif

#ifndef RC_INVOKED
double __cdecl difftime(time_t _Time1,time_t _Time2);
char *__cdecl ctime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
struct tm *__cdecl gmtime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
struct tm *__cdecl localtime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifdef _POSIX
#ifdef __GNUC__ /* FIXME: Other compilers that these macros work with? */
#ifndef localtime_r
#define localtime_r(_Time, _Tm)	({ struct tm *___tmp_tm =		\
						localtime((_Time));	\
						if (___tmp_tm) {	\
						  *(_Tm) = *___tmp_tm;	\
						  ___tmp_tm = (_Tm);	\
						}			\
						___tmp_tm;	})
#endif
#ifndef gmtime_r
#define gmtime_r(_Time,_Tm)	({ struct tm *___tmp_tm =		\
						gmtime((_Time));	\
						if (___tmp_tm) {	\
						  *(_Tm) = *___tmp_tm;	\
						  ___tmp_tm = (_Tm);	\
						}			\
						___tmp_tm;	})
#endif
#ifndef ctime_r
#define ctime_r(_Time,_Str)	({ char *___tmp_tm = ctime((_Time));	\
						if (___tmp_tm)		\
						 ___tmp_tm =		\
						   strcpy((_Str),___tmp_tm); \
						___tmp_tm;	})
#endif
#ifndef asctime_r
#define asctime_r(_Tm, _Buf)	({ char *___tmp_tm = asctime((_Tm));	\
						if (___tmp_tm)		\
						 ___tmp_tm =		\
						   strcpy((_Buf),___tmp_tm);\
						___tmp_tm;	})
#endif
#else /* NOT GCC: */
      /* FIXME: These are more generic but call the main function twice! */
#ifndef localtime_r
#define localtime_r(_Time, _Tm) (localtime ((_Time)) ? *(_Tm) = *localtime ((_Time),(_Tm)) : 0)
#endif
#ifndef gmtime_r
#define gmtime_r(_Time,_Tm) (gmtime ((_Time)) ? (*(_Tm) = *gmtime (_Time),(_Tm)) : 0)
#endif
#ifndef ctime_r
#define ctime_r(_Time,_Str) (ctime ((_Time)) ? (strcpy((_Str),ctime ((_Time))),(_Str)) : 0)
#endif
#endif /* __GNUC__ */
#endif /* _POSIX */

time_t __cdecl mktime(struct tm *_Tm);
time_t __cdecl _mkgmtime(struct tm *_Tm);
time_t __cdecl time(time_t *_Time);

#ifndef __CRT__NO_INLINE
#if !defined(_USE_32BIT_TIME_T)
__CRT_INLINE double __cdecl difftime(time_t _Time1,time_t _Time2)
  { return _difftime64(_Time1,_Time2); }
__CRT_INLINE char *__cdecl ctime(const time_t *_Time) { return _ctime64(_Time); }
__CRT_INLINE struct tm *__cdecl gmtime(const time_t *_Time) { return _gmtime64(_Time); }
__CRT_INLINE struct tm *__cdecl localtime(const time_t *_Time) { return _localtime64(_Time); }
__CRT_INLINE time_t __cdecl mktime(struct tm *_Tm) { return _mktime64(_Tm); }
__CRT_INLINE time_t __cdecl _mkgmtime(struct tm *_Tm) { return _mkgmtime64(_Tm); }
__CRT_INLINE time_t __cdecl time(time_t *_Time) { return _time64(_Time); }
#else
__CRT_INLINE double __cdecl difftime(time_t _Time1,time_t _Time2)
  { return _difftime32(_Time1,_Time2); }
__CRT_INLINE char *__cdecl ctime(const time_t *_Time) { return _ctime32(_Time); }
__CRT_INLINE struct tm *__cdecl localtime(const time_t *_Time) { return _localtime32(_Time); }
__CRT_INLINE time_t __cdecl mktime(struct tm *_Tm) { return _mktime32(_Tm); }
__CRT_INLINE struct tm *__cdecl gmtime(const time_t *_Time) { return _gmtime32(_Time); }
__CRT_INLINE time_t __cdecl _mkgmtime(struct tm *_Tm) { return _mkgmtime32(_Tm); }
__CRT_INLINE time_t __cdecl time(time_t *_Time) { return _time32(_Time); }
#endif /* !_USE_32BIT_TIME_T */
#endif /* !__CRT__NO_INLINE */
#endif /* !RC_INVOKED */

#if !defined(NO_OLDNAMES) || defined(_POSIX)
#define CLK_TCK CLOCKS_PER_SEC

  _CRTIMP extern int daylight;
  _CRTIMP extern long timezone;
  _CRTIMP extern char *tzname[2];
  void __cdecl tzset(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

#include <_timeval.h>

#ifndef _TIMEZONE_DEFINED /* also in sys/time.h */
#define _TIMEZONE_DEFINED
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

  extern int __cdecl mingw_gettimeofday (struct timeval *p, struct timezone *z);
#endif /* _TIMEZONE_DEFINED */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/time_s.h>

/* Adding timespec definition.  */
#include <sys/timeb.h>

/* POSIX 2008 says clock_gettime and timespec are defined in time.h header,
   but other systems - like Linux, Solaris, etc - tend to declare such
   recent extensions only if the following guards are met.  */
#if !defined(IN_WINPTHREAD) && \
	((!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX)) || \
	 (_POSIX_C_SOURCE > 2) || defined(__EXTENSIONS__))
#include <pthread_time.h>
#endif

#endif /* End _TIME_H_ */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _TIME_H_
#define _TIME_H_

#include <crtdefs.h>

#ifndef _WIN32
#error Only Win32 target is supported!
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRTIMP
#define _CRTIMP __declspec(dllimport)
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
  typedef unsigned short wchar_t;
#endif

#ifndef _TIME32_T_DEFINED
#define _TIME32_T_DEFINED
  typedef long __time32_t;
#endif

#ifndef _TIME64_T_DEFINED
#define _TIME64_T_DEFINED
  __MINGW_EXTENSION typedef __int64 __time64_t;
#endif

#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
#ifdef _USE_32BIT_TIME_T
  typedef __time32_t time_t;
#else
  typedef __time64_t time_t;
#endif
#endif

#ifndef _CLOCK_T_DEFINED
#define _CLOCK_T_DEFINED
  typedef long clock_t;
#endif

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
#undef size_t
#ifdef _WIN64
  __MINGW_EXTENSION typedef unsigned __int64 size_t;
#else
  typedef unsigned int size_t;
#endif
#endif

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
#undef ssize_t
#ifdef _WIN64
  __MINGW_EXTENSION typedef __int64 ssize_t;
#else
  typedef int ssize_t;
#endif
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef _TM_DEFINED
#define _TM_DEFINED
  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };
#endif

#define CLOCKS_PER_SEC 1000

  __MINGW_IMPORT int _daylight;
  __MINGW_IMPORT long _dstbias;
  __MINGW_IMPORT long _timezone;
  __MINGW_IMPORT char * _tzname[2];

  _CRTIMP errno_t __cdecl _get_daylight(int *_Daylight);
  _CRTIMP errno_t __cdecl _get_dstbias(long *_Daylight_savings_bias);
  _CRTIMP errno_t __cdecl _get_timezone(long *_Timezone);
  _CRTIMP errno_t __cdecl _get_tzname(size_t *_ReturnValue,char *_Buffer,size_t _SizeInBytes,int _Index);
  char *__cdecl asctime(const struct tm *_Tm) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  char *__cdecl _ctime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  clock_t __cdecl clock(void);
  double __cdecl _difftime32(__time32_t _Time1,__time32_t _Time2);
  struct tm *__cdecl _gmtime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  struct tm *__cdecl _localtime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl strftime(char * __restrict__ _Buf,size_t _SizeInBytes,const char * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  _CRTIMP size_t __cdecl _strftime_l(char * __restrict__ _Buf,size_t _Max_size,const char * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  _CRTIMP char *__cdecl _strdate(char *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP char *__cdecl _strtime(char *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __time32_t __cdecl _time32(__time32_t *_Time);
  __time32_t __cdecl _mktime32(struct tm *_Tm);
  __time32_t __cdecl _mkgmtime32(struct tm *_Tm);
#if defined (_POSIX_) || defined(__GNUC__)
  void __cdecl tzset(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#if !defined (_POSIX_)
  _CRTIMP void __cdecl _tzset(void);
#endif

  double __cdecl _difftime64(__time64_t _Time1,__time64_t _Time2);
  _CRTIMP char *__cdecl _ctime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP struct tm *__cdecl _gmtime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP struct tm *__cdecl _localtime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP __time64_t __cdecl _mktime64(struct tm *_Tm);
  _CRTIMP __time64_t __cdecl _mkgmtime64(struct tm *_Tm);
  _CRTIMP __time64_t __cdecl _time64(__time64_t *_Time);
  unsigned __cdecl _getsystime(struct tm *_Tm);
  unsigned __cdecl _setsystime(struct tm *_Tm,unsigned _MilliSec);

#ifndef _WTIME_DEFINED
  _CRTIMP wchar_t *__cdecl _wasctime(const struct tm *_Tm);
  wchar_t *__cdecl _wctime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsftime(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  _CRTIMP size_t __cdecl _wcsftime_l(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wstrdate(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wstrtime(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wctime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#if !defined (RC_INVOKED) && !defined (_INC_WTIME_INL)
#define _INC_WTIME_INL
  wchar_t *__cdecl _wctime(const time_t *) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __CRT__NO_INLINE
#ifndef _USE_32BIT_TIME_T
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime64(_Time); }
#else
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime32(_Time); }
#endif
#endif /* __CRT__NO_INLINE */
#endif

#define _WTIME_DEFINED
#endif

#ifndef RC_INVOKED
double __cdecl difftime(time_t _Time1,time_t _Time2);
char *__cdecl ctime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
struct tm *__cdecl gmtime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
struct tm *__cdecl localtime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifdef _POSIX
#ifdef __GNUC__ /* FIXME: Other compilers that these macros work with? */
#ifndef localtime_r
#define localtime_r(_Time, _Tm)	({ struct tm *___tmp_tm =		\
						localtime((_Time));	\
						if (___tmp_tm) {	\
						  *(_Tm) = *___tmp_tm;	\
						  ___tmp_tm = (_Tm);	\
						}			\
						___tmp_tm;	})
#endif
#ifndef gmtime_r
#define gmtime_r(_Time,_Tm)	({ struct tm *___tmp_tm =		\
						gmtime((_Time));	\
						if (___tmp_tm) {	\
						  *(_Tm) = *___tmp_tm;	\
						  ___tmp_tm = (_Tm);	\
						}			\
						___tmp_tm;	})
#endif
#ifndef ctime_r
#define ctime_r(_Time,_Str)	({ char *___tmp_tm = ctime((_Time));	\
						if (___tmp_tm)		\
						 ___tmp_tm =		\
						   strcpy((_Str),___tmp_tm); \
						___tmp_tm;	})
#endif
#ifndef asctime_r
#define asctime_r(_Tm, _Buf)	({ char *___tmp_tm = asctime((_Tm));	\
						if (___tmp_tm)		\
						 ___tmp_tm =		\
						   strcpy((_Buf),___tmp_tm);\
						___tmp_tm;	})
#endif
#else /* NOT GCC: */
      /* FIXME: These are more generic but call the main function twice! */
#ifndef localtime_r
#define localtime_r(_Time, _Tm) (localtime ((_Time)) ? *(_Tm) = *localtime ((_Time),(_Tm)) : 0)
#endif
#ifndef gmtime_r
#define gmtime_r(_Time,_Tm) (gmtime ((_Time)) ? (*(_Tm) = *gmtime (_Time),(_Tm)) : 0)
#endif
#ifndef ctime_r
#define ctime_r(_Time,_Str) (ctime ((_Time)) ? (strcpy((_Str),ctime ((_Time))),(_Str)) : 0)
#endif
#endif /* __GNUC__ */
#endif /* _POSIX */

time_t __cdecl mktime(struct tm *_Tm);
time_t __cdecl _mkgmtime(struct tm *_Tm);
time_t __cdecl time(time_t *_Time);

#ifndef __CRT__NO_INLINE
#if !defined(_USE_32BIT_TIME_T)
__CRT_INLINE double __cdecl difftime(time_t _Time1,time_t _Time2)
  { return _difftime64(_Time1,_Time2); }
__CRT_INLINE char *__cdecl ctime(const time_t *_Time) { return _ctime64(_Time); }
__CRT_INLINE struct tm *__cdecl gmtime(const time_t *_Time) { return _gmtime64(_Time); }
__CRT_INLINE struct tm *__cdecl localtime(const time_t *_Time) { return _localtime64(_Time); }
__CRT_INLINE time_t __cdecl mktime(struct tm *_Tm) { return _mktime64(_Tm); }
__CRT_INLINE time_t __cdecl _mkgmtime(struct tm *_Tm) { return _mkgmtime64(_Tm); }
__CRT_INLINE time_t __cdecl time(time_t *_Time) { return _time64(_Time); }
#else
__CRT_INLINE double __cdecl difftime(time_t _Time1,time_t _Time2)
  { return _difftime32(_Time1,_Time2); }
__CRT_INLINE char *__cdecl ctime(const time_t *_Time) { return _ctime32(_Time); }
__CRT_INLINE struct tm *__cdecl localtime(const time_t *_Time) { return _localtime32(_Time); }
__CRT_INLINE time_t __cdecl mktime(struct tm *_Tm) { return _mktime32(_Tm); }
__CRT_INLINE struct tm *__cdecl gmtime(const time_t *_Time) { return _gmtime32(_Time); }
__CRT_INLINE time_t __cdecl _mkgmtime(struct tm *_Tm) { return _mkgmtime32(_Tm); }
__CRT_INLINE time_t __cdecl time(time_t *_Time) { return _time32(_Time); }
#endif /* !_USE_32BIT_TIME_T */
#endif /* !__CRT__NO_INLINE */
#endif /* !RC_INVOKED */

#if !defined(NO_OLDNAMES) || defined(_POSIX)
#define CLK_TCK CLOCKS_PER_SEC

  _CRTIMP extern int daylight;
  _CRTIMP extern long timezone;
  _CRTIMP extern char *tzname[2];
  void __cdecl tzset(void) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif

#include <_timeval.h>

#ifndef _TIMEZONE_DEFINED /* also in sys/time.h */
#define _TIMEZONE_DEFINED
struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

  extern int __cdecl mingw_gettimeofday (struct timeval *p, struct timezone *z);
#endif /* _TIMEZONE_DEFINED */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/time_s.h>

/* Adding timespec definition.  */
#include <sys/timeb.h>

/* POSIX 2008 says clock_gettime and timespec are defined in time.h header,
   but other systems - like Linux, Solaris, etc - tend to declare such
   recent extensions only if the following guards are met.  */
#if !defined(IN_WINPTHREAD) && \
	((!defined(_STRICT_STDC) && !defined(__XOPEN_OR_POSIX)) || \
	 (_POSIX_C_SOURCE > 2) || defined(__EXTENSIONS__))
#include <pthread_time.h>
#endif

#endif /* End _TIME_H_ */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_WCHAR
#define _INC_WCHAR

#include <crtdefs.h>
#include <_mingw_print_push.h>

#if defined (__USE_MINGW_ANSI_STDIO) && ((__USE_MINGW_ANSI_STDIO + 0) != 0) && !defined (__USE_MINGW_STRTOX)
#define __USE_MINGW_STRTOX 1
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WCHAR_MIN  /* also in stdint.h */
#define WCHAR_MIN 0U
#define WCHAR_MAX 0xffffU
#endif

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifndef _FILE_DEFINED
  struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
  };
  typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif

#ifndef _STDIO_DEFINED
#ifdef _WIN64
  _CRTIMP FILE *__cdecl __iob_func(void);
#define _iob  __iob_func()
#else
#ifdef _MSVCRT_
extern FILE _iob[];	/* A pointer to an array of FILE */
#define __iob_func()	(_iob)
#else
extern FILE (* __MINGW_IMP_SYMBOL(_iob))[];	/* A pointer to an array of FILE */
#define __iob_func()	(* __MINGW_IMP_SYMBOL(_iob))
#endif
#endif

#define _iob __iob_func()
#endif

#ifndef _STDSTREAM_DEFINED
#define stdin (&__iob_func()[0])
#define stdout (&__iob_func()[1])
#define stderr (&__iob_func()[2])
#define _STDSTREAM_DEFINED
#endif

#ifndef _FSIZE_T_DEFINED
  typedef unsigned long _fsize_t;
#define _FSIZE_T_DEFINED
#endif

#ifndef _WFINDDATA_T_DEFINED
  struct _wfinddata32_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    _fsize_t size;
    wchar_t name[260];
  };

  struct _wfinddata32i64_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    __MINGW_EXTENSION __int64 size;
    wchar_t name[260];
  };

  struct _wfinddata64i32_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    _fsize_t size;
    wchar_t name[260];
  };

  struct _wfinddata64_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    __MINGW_EXTENSION __int64 size;
    wchar_t name[260];
  };
/* #endif */

#ifdef _USE_32BIT_TIME_T
#define _wfinddata_t _wfinddata32_t
#define _wfinddatai64_t _wfinddata32i64_t

#define _wfindfirst32 _wfindfirst
#define _wfindnext32 _wfindnext
#define _wfindfirst32i64 _wfindfirsti64
#define _wfindnext32i64 _wfindnexti64
#else
#define _wfinddata_t _wfinddata64i32_t
#define _wfinddatai64_t _wfinddata64_t

#define _wfindfirst _wfindfirst64i32
#define _wfindnext _wfindnext64i32
#define _wfindfirsti64 _wfindfirst64
#define _wfindnexti64 _wfindnext64
#endif

#define _WFINDDATA_T_DEFINED
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef _CONST_RETURN
#define _CONST_RETURN
#endif

#define _WConst_return _CONST_RETURN

#ifndef _CRT_CTYPEDATA_DEFINED
#define _CRT_CTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS

#ifndef __PCTYPE_FUNC
#define __PCTYPE_FUNC __pctype_func()
#ifdef _MSVCRT_
#define __pctype_func() (_pctype)
#else
#define __pctype_func() (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#ifndef _pctype
#ifdef _MSVCRT_
  extern unsigned short *_pctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pctype);
#define _pctype (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif
#endif
#endif

#ifndef _CRT_WCTYPEDATA_DEFINED
#define _CRT_WCTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS
#ifndef _wctype
#ifdef _MSVCRT_
  extern unsigned short *_wctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_wctype);
#define _wctype (* __MINGW_IMP_SYMBOL(_wctype))
#endif
#endif

#ifdef _MSVCRT_
#define __pwctype_func() (_pwctype)
#else
#define __pwctype_func() (* __MINGW_IMP_SYMBOL(_pwctype))
#endif

#ifndef _pwctype
#ifdef _MSVCRT_
  extern unsigned short *_pwctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pwctype);
#define _pwctype (* __MINGW_IMP_SYMBOL(_pwctype))
#endif
#endif

#endif
#endif

#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8

#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80

#define _LEADBYTE 0x8000
#define _ALPHA (0x0100|_UPPER|_LOWER)

#ifndef _WCTYPE_DEFINED
#define _WCTYPE_DEFINED

  int __cdecl iswalpha(wint_t _C);
  _CRTIMP int __cdecl _iswalpha_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswupper(wint_t _C);
  _CRTIMP int __cdecl _iswupper_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswlower(wint_t _C);
  _CRTIMP int __cdecl _iswlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswdigit(wint_t _C);
  _CRTIMP int __cdecl _iswdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswxdigit(wint_t _C);
  _CRTIMP int __cdecl _iswxdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswspace(wint_t _C);
  _CRTIMP int __cdecl _iswspace_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswpunct(wint_t _C);
  _CRTIMP int __cdecl _iswpunct_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswalnum(wint_t _C);
  _CRTIMP int __cdecl _iswalnum_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswprint(wint_t _C);
  _CRTIMP int __cdecl _iswprint_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswgraph(wint_t _C);
  _CRTIMP int __cdecl _iswgraph_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswcntrl(wint_t _C);
  _CRTIMP int __cdecl _iswcntrl_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswascii(wint_t _C);
  int __cdecl isleadbyte(int _C);
  _CRTIMP int __cdecl _isleadbyte_l(int _C,_locale_t _Locale);
  wint_t __cdecl towupper(wint_t _C);
  _CRTIMP wint_t __cdecl _towupper_l(wint_t _C,_locale_t _Locale);
  wint_t __cdecl towlower(wint_t _C);
  _CRTIMP wint_t __cdecl _towlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswctype(wint_t _C,wctype_t _Type);
  _CRTIMP int __cdecl _iswctype_l(wint_t _C,wctype_t _Type,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsymf(wint_t _C);
  _CRTIMP int __cdecl _iswcsymf_l(wint_t _C,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsym(wint_t _C);
  _CRTIMP int __cdecl _iswcsym_l(wint_t _C,_locale_t _Locale);
  int __cdecl is_wctype(wint_t _C,wctype_t _Type);

#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES)
  int __cdecl iswblank(wint_t _C);
#endif

#endif

#ifndef _WDIRECT_DEFINED
#define _WDIRECT_DEFINED

  _CRTIMP wchar_t *__cdecl _wgetcwd(wchar_t *_DstBuf,int _SizeInWords);
  _CRTIMP wchar_t *__cdecl _wgetdcwd(int _Drive,wchar_t *_DstBuf,int _SizeInWords);
  wchar_t *__cdecl _wgetdcwd_nolock(int _Drive,wchar_t *_DstBuf,int _SizeInWords);
  _CRTIMP int __cdecl _wchdir(const wchar_t *_Path);
  _CRTIMP int __cdecl _wmkdir(const wchar_t *_Path);
  _CRTIMP int __cdecl _wrmdir(const wchar_t *_Path);
#endif

#ifndef _WIO_DEFINED
#define _WIO_DEFINED

  _CRTIMP int __cdecl _waccess(const wchar_t *_Filename,int _AccessMode);
  _CRTIMP int __cdecl _wchmod(const wchar_t *_Filename,int _Mode);
  _CRTIMP int __cdecl _wcreat(const wchar_t *_Filename,int _PermissionMode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP intptr_t __cdecl _wfindfirst32(const wchar_t *_Filename,struct _wfinddata32_t *_FindData);
  _CRTIMP int __cdecl _wfindnext32(intptr_t _FindHandle,struct _wfinddata32_t *_FindData);
  _CRTIMP int __cdecl _wunlink(const wchar_t *_Filename);
  _CRTIMP int __cdecl _wrename(const wchar_t *_OldFilename,const wchar_t *_NewFilename);
  _CRTIMP wchar_t *__cdecl _wmktemp(wchar_t *_TemplateName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP intptr_t __cdecl _wfindfirst32i64(const wchar_t *_Filename,struct _wfinddata32i64_t *_FindData);
  intptr_t __cdecl _wfindfirst64i32(const wchar_t *_Filename,struct _wfinddata64i32_t *_FindData);
  _CRTIMP intptr_t __cdecl _wfindfirst64(const wchar_t *_Filename,struct _wfinddata64_t *_FindData);
  _CRTIMP int __cdecl _wfindnext32i64(intptr_t _FindHandle,struct _wfinddata32i64_t *_FindData);
  int __cdecl _wfindnext64i32(intptr_t _FindHandle,struct _wfinddata64i32_t *_FindData);
  _CRTIMP int __cdecl _wfindnext64(intptr_t _FindHandle,struct _wfinddata64_t *_FindData);
  _CRTIMP errno_t __cdecl _wsopen_s(int *_FileHandle,const wchar_t *_Filename,int _OpenFlag,int _ShareFlag,int _PermissionFlag);
  _CRTIMP int __cdecl _wopen(const wchar_t *_Filename,int _OpenFlag,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wsopen(const wchar_t *_Filename,int _OpenFlag,int _ShareFlag,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#endif

#ifndef _WLOCALE_DEFINED
#define _WLOCALE_DEFINED
  _CRTIMP wchar_t *__cdecl _wsetlocale(int _Category,const wchar_t *_Locale);
#endif

#ifndef _WPROCESS_DEFINED
#define _WPROCESS_DEFINED

  _CRTIMP intptr_t __cdecl _wexecl(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexecle(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexeclp(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexeclpe(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexecv(const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wexecve(const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  _CRTIMP intptr_t __cdecl _wexecvp(const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wexecvpe(const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  _CRTIMP intptr_t __cdecl _wspawnl(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnle(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnlp(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnlpe(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnv(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wspawnve(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  _CRTIMP intptr_t __cdecl _wspawnvp(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wspawnvpe(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
#ifndef _CRT_WSYSTEM_DEFINED
#define _CRT_WSYSTEM_DEFINED
  _CRTIMP int __cdecl _wsystem(const wchar_t *_Command);
#endif
#endif

#ifndef _WCTYPE_INLINE_DEFINED
#undef _CRT_WCTYPE_NOINLINE
#if !defined(__cplusplus) || defined(_CRT_WCTYPE_NOINLINE)
#define iswalpha(_c) (iswctype(_c,_ALPHA))
#define iswupper(_c) (iswctype(_c,_UPPER))
#define iswlower(_c) (iswctype(_c,_LOWER))
#define iswdigit(_c) (iswctype(_c,_DIGIT))
#define iswxdigit(_c) (iswctype(_c,_HEX))
#define iswspace(_c) (iswctype(_c,_SPACE))
#define iswpunct(_c) (iswctype(_c,_PUNCT))
#define iswalnum(_c) (iswctype(_c,_ALPHA|_DIGIT))
#define iswprint(_c) (iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT))
#define iswgraph(_c) (iswctype(_c,_PUNCT|_ALPHA|_DIGIT))
#define iswcntrl(_c) (iswctype(_c,_CONTROL))
#define iswascii(_c) ((unsigned)(_c) < 0x80)

#define _iswalpha_l(_c,_p) (_iswctype_l(_c,_ALPHA,_p))
#define _iswupper_l(_c,_p) (_iswctype_l(_c,_UPPER,_p))
#define _iswlower_l(_c,_p) (_iswctype_l(_c,_LOWER,_p))
#define _iswdigit_l(_c,_p) (_iswctype_l(_c,_DIGIT,_p))
#define _iswxdigit_l(_c,_p) (_iswctype_l(_c,_HEX,_p))
#define _iswspace_l(_c,_p) (_iswctype_l(_c,_SPACE,_p))
#define _iswpunct_l(_c,_p) (_iswctype_l(_c,_PUNCT,_p))
#define _iswalnum_l(_c,_p) (_iswctype_l(_c,_ALPHA|_DIGIT,_p))
#define _iswprint_l(_c,_p) (_iswctype_l(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswgraph_l(_c,_p) (_iswctype_l(_c,_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswcntrl_l(_c,_p) (_iswctype_l(_c,_CONTROL,_p))
#ifndef _CTYPE_DISABLE_MACROS
#define isleadbyte(_c) (__PCTYPE_FUNC[(unsigned char)(_c)] & _LEADBYTE)
#endif
#endif
#define _WCTYPE_INLINE_DEFINED
#endif

#if !defined(_POSIX_) || defined(__GNUC__)
#ifndef _INO_T_DEFINED
#define _INO_T_DEFINED
  typedef unsigned short _ino_t;
#ifndef	NO_OLDNAMES
  typedef unsigned short ino_t;
#endif
#endif

#ifndef _DEV_T_DEFINED
#define _DEV_T_DEFINED
  typedef unsigned int _dev_t;
#ifndef	NO_OLDNAMES
  typedef unsigned int dev_t;
#endif
#endif

#include <_mingw_off_t.h>
#include <_mingw_stat64.h>

#ifndef _WSTAT_DEFINED
#define _WSTAT_DEFINED

  _CRTIMP int __cdecl _wstat32(const wchar_t *_Name,struct _stat32 *_Stat);
  _CRTIMP int __cdecl _wstat32i64(const wchar_t *_Name,struct _stat32i64 *_Stat);
  int __cdecl _wstat64i32(const wchar_t *_Name,struct _stat64i32 *_Stat);
  _CRTIMP int __cdecl _wstat64(const wchar_t *_Name,struct _stat64 *_Stat);
#endif
#endif

#ifndef _WCONIO_DEFINED
#define _WCONIO_DEFINED

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

  _CRTIMP wchar_t *_cgetws(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wint_t __cdecl _getwch(void);
  _CRTIMP wint_t __cdecl _getwche(void);
  _CRTIMP wint_t __cdecl _putwch(wchar_t _WCh);
  _CRTIMP wint_t __cdecl _ungetwch(wint_t _WCh);
  _CRTIMP int __cdecl _cputws(const wchar_t *_String);
  _CRTIMP int __cdecl _cwprintf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _cwscanf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _cwscanf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vcwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _cwprintf_p(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vcwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);

  _CRTIMP int __cdecl _cwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vcwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _cwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vcwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  wint_t __cdecl _putwch_nolock(wchar_t _WCh);
  wint_t __cdecl _getwch_nolock(void);
  wint_t __cdecl _getwche_nolock(void);
  wint_t __cdecl _ungetwch_nolock(wint_t _WCh);
#endif

#ifndef _WSTDIO_DEFINED
#define _WSTDIO_DEFINED

/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswscanf (const wchar_t * __restrict__ _Str,const wchar_t * __restrict__ Format,va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wscanf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwscanf(const wchar_t * __restrict__ Format, va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwscanf (FILE * __restrict__ fp, const wchar_t * __restrict__ Format,va_list argp);


/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wprintf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
/* __attribute__((__format__ (gnu_printf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ , ...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ ,va_list);

#undef __mingw_ovr
#if defined (__GNUC__)
#define __mingw_ovr static __attribute__ ((__unused__)) __inline__ __cdecl
#elif defined(__cplusplus)
#define __mingw_ovr inline __cdecl
#else
#define __mingw_ovr static __cdecl
#endif

#if __USE_MINGW_ANSI_STDIO

/*
 * User has expressed a preference for C99 conformance...
 */
__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int swscanf(const wchar_t *__source, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vswscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wscanf(const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwscanf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwscanf(FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vswscanf (const wchar_t *__source, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vswscanf( __source, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
{
  return __mingw_vwscanf( __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwscanf( __stream, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */



__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwprintf (FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wprintf (const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwprintf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwprintf (FILE *__stream, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwprintf( __stream, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwprintf (const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vwprintf( __format, __local_argv );
}

/*#ifndef __NO_ISOCEXT */  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
int snwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
int vsnwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
}
/* #endif */ /* __NO_ISOCEXT */
#else /* !__USE_MINGW_ANSI_STDIO */

  int __cdecl fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wscanf(const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
  int __cdecl __ms_vwscanf (const wchar_t * __restrict__ , va_list);
  int __cdecl __ms_vfwscanf (FILE * __restrict__ ,const wchar_t * __restrict__ ,va_list);
  int __cdecl __ms_vswscanf (const wchar_t * __restrict__ ,const wchar_t * __restrict__ ,va_list);

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
  {
    return __ms_vfwscanf (__stream, __format, __local_argv);
  }

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vswscanf (const wchar_t * __restrict__ __source, const wchar_t * __restrict__ __format, __builtin_va_list __local_argv)
  {
    return __ms_vswscanf( __source, __format, __local_argv );
  }
  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(1)
  int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
  {
    return __ms_vwscanf (__format, __local_argv);
  }

#endif /* __NO_ISOCEXT */

  int __cdecl fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  int __cdecl wprintf(const wchar_t * __restrict__ _Format,...);
  int __cdecl vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  int __cdecl vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
#endif /* __USE_MINGW_ANSI_STDIO */


#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifdef _POSIX_
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode);
#else
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);
#endif

  wint_t __cdecl fgetwc(FILE *_File);
  _CRTIMP wint_t __cdecl _fgetwchar(void);
  wint_t __cdecl fputwc(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _fputwchar(wchar_t _Ch);
  wint_t __cdecl getwc(FILE *_File);
  wint_t __cdecl getwchar(void);
  wint_t __cdecl putwc(wchar_t _Ch,FILE *_File);
  wint_t __cdecl putwchar(wchar_t _Ch);
  wint_t __cdecl ungetwc(wint_t _Ch,FILE *_File);
  wchar_t *__cdecl fgetws(wchar_t * __restrict__ _Dst,int _SizeInWords,FILE * __restrict__ _File);
  int __cdecl fputws(const wchar_t * __restrict__ _Str,FILE * __restrict__ _File);
  _CRTIMP wchar_t *__cdecl _getws(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _putws(const wchar_t *_Str);
  _CRTIMP int __cdecl _scwprintf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _swprintf_l(wchar_t * __restrict__ ,size_t _SizeInWords,const wchar_t * __restrict__ _Format,_locale_t _Locale,... ) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _swprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _snwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vsnwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */

#if !defined (__USE_MINGW_ANSI_STDIO) || __USE_MINGW_ANSI_STDIO == 0
#pragma push_macro("snwprintf")
#pragma push_macro("vsnwprintf")
# undef snwprintf
# undef vsnwprintf
  int __cdecl __ms_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
  int __cdecl __ms_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
  __mingw_ovr
  int snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...)
  {
    int r;
    va_list argp;
    __builtin_va_start (argp, format);
    r = _vsnwprintf (s, n, format, argp);
    __builtin_va_end (argp);
    return r;
  }
  __mingw_ovr
  int __cdecl vsnwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, va_list arg)
  {
    return _vsnwprintf(s,n,format,arg);
  }
#pragma pop_macro ("vsnwprintf")
#pragma pop_macro ("snwprintf")
#endif

#endif

  _CRTIMP int __cdecl _fwprintf_p(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _wprintf_p(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vfwprintf_p(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _vwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _swprintf_p(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf_p(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _scwprintf_p(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vscwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _wprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _wprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _vwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _fwprintf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _fwprintf_p_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vfwprintf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _vfwprintf_p_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _swprintf_c_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _swprintf_p_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vswprintf_c_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _vswprintf_p_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _scwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _scwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vscwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _snwprintf_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vsnwprintf_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _swprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,va_list _Args);
  _CRTIMP int __cdecl __swprintf_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,_locale_t _Plocinfo,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vswprintf_l(wchar_t * __restrict__ _Dest,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl __vswprintf_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,_locale_t _Plocinfo,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef RC_INVOKED
#include <swprintf.inl>
#endif

#ifdef _CRT_NON_CONFORMING_SWPRINTFS
#ifndef __cplusplus
#define _swprintf_l __swprintf_l
#define _vswprintf_l __vswprintf_l
#endif
#endif

  _CRTIMP wchar_t *__cdecl _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);
  _CRTIMP int __cdecl _vscwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _vscwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _fwscanf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _swscanf_l(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,_locale_t _Locale,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _snwscanf(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _snwscanf_l(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _wscanf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP FILE *__cdecl _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  _CRTIMP FILE *__cdecl _wfopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP FILE *__cdecl _wfreopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode,FILE * __restrict__ _OldFile) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP FILE *__cdecl _wpopen(const wchar_t *_Command,const wchar_t *_Mode);
#if !defined(NO_OLDNAMES) && !defined(wpopen)
#define wpopen	_wpopen
#endif
  _CRTIMP int __cdecl _wremove(const wchar_t *_Filename);
  _CRTIMP wchar_t *__cdecl _wtmpnam(wchar_t *_Buffer);
  _CRTIMP wint_t __cdecl _fgetwc_nolock(FILE *_File);
  _CRTIMP wint_t __cdecl _fputwc_nolock(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _ungetwc_nolock(wint_t _Ch,FILE *_File);

#undef _CRT_GETPUTWCHAR_NOINLINE

#if !defined(__cplusplus) || defined(_CRT_GETPUTWCHAR_NOINLINE) || defined (__CRT__NO_INLINE)
#define getwchar() fgetwc(stdin)
#define putwchar(_c) fputwc((_c),stdout)
#else
  __CRT_INLINE wint_t __cdecl getwchar() {return (fgetwc(stdin)); }
  __CRT_INLINE wint_t __cdecl putwchar(wchar_t _C) {return (fputwc(_C,stdout)); }
#endif

#define getwc(_stm) fgetwc(_stm)
#define putwc(_c,_stm) fputwc(_c,_stm)
#define _putwc_nolock(_c,_stm) _fputwc_nolock(_c,_stm)
#define _getwc_nolock(_c) _fgetwc_nolock(_c)
#endif

#ifndef _WSTDLIB_DEFINED
#define _WSTDLIB_DEFINED

  _CRTIMP wchar_t *__cdecl _itow(int _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ltow(long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ultow(unsigned long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP double __cdecl _wcstod_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,_locale_t _Locale);

  double __cdecl __mingw_wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl __mingw_wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
  long double __cdecl __mingw_wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);

#if __USE_MINGW_ANSI_STDIO
  __mingw_ovr
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstod(_Str,_EndPtr);
  }
  __mingw_ovr
  float __cdecl wcstof(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstof(_Str,_EndPtr);
  }
  /* wcstold is already a mingw implementation */
#else
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
#endif /* defined(__USE_MINGW_STRTOX) */
#if !defined __NO_ISOCEXT /* in libmingwex.a */
  long double __cdecl wcstold (const wchar_t * __restrict__, wchar_t ** __restrict__);
#endif /* __NO_ISOCEXT */
  long __cdecl wcstol(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP long __cdecl _wcstol_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __cdecl wcstoul(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP unsigned long __cdecl _wcstoul_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wgetenv(const wchar_t *_VarName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef _CRT_WSYSTEM_DEFINED
#define _CRT_WSYSTEM_DEFINED
  _CRTIMP int __cdecl _wsystem(const wchar_t *_Command);
#endif
  _CRTIMP double __cdecl _wtof(const wchar_t *_Str);
  _CRTIMP double __cdecl _wtof_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _wtoi(const wchar_t *_Str);
  _CRTIMP int __cdecl _wtoi_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP long __cdecl _wtol(const wchar_t *_Str);
  _CRTIMP long __cdecl _wtol_l(const wchar_t *_Str,_locale_t _Locale);

  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _i64tow(__int64 _Val,wchar_t *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _ui64tow(unsigned __int64 _Val,wchar_t *_DstBuf,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64(const wchar_t *_Str);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64_l(const wchar_t *_Str,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
#endif

#ifndef _POSIX_
#ifndef _WSTDLIBP_DEFINED
#define _WSTDLIBP_DEFINED
  _CRTIMP wchar_t *__cdecl _wfullpath(wchar_t *_FullPath,const wchar_t *_Path,size_t _SizeInWords);
  _CRTIMP void __cdecl _wmakepath(wchar_t *_ResultPath,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);
#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP int __cdecl _wputenv(const wchar_t *_EnvString);
  _CRTIMP void __cdecl _wsearchenv(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _wsplitpath(const wchar_t *_FullPath,wchar_t *_Drive,wchar_t *_Dir,wchar_t *_Filename,wchar_t *_Ext) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#endif
#endif

#ifndef _WSTRING_DEFINED
#define _WSTRING_DEFINED
  _CRTIMP wchar_t *__cdecl _wcsdup(const wchar_t *_Str);
  wchar_t *__cdecl wcscat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcschr(const wchar_t *_Str,wchar_t _Ch);
  int __cdecl wcscmp(const wchar_t *_Str1,const wchar_t *_Str2);
  wchar_t *__cdecl wcscpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcscspn(const wchar_t *_Str,const wchar_t *_Control);
  size_t __cdecl wcslen(const wchar_t *_Str);
  size_t __cdecl wcsnlen(const wchar_t *_Src,size_t _MaxCount);
  wchar_t *__cdecl wcsncat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wcsncmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  wchar_t *__cdecl wcsncpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  wchar_t *__cdecl _wcsncpy_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcspbrk(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsrchr(const wchar_t *_Str,wchar_t _Ch);
  size_t __cdecl wcsspn(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsstr(const wchar_t *_Str,const wchar_t *_SubStr);
  wchar_t *__cdecl wcstok(wchar_t * __restrict__ _Str,const wchar_t * __restrict__ _Delim) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcserror(int _ErrNum) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl __wcserror(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsrev(wchar_t *_Str);
  _CRTIMP wchar_t *__cdecl _wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcslwr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcslwr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsupr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcsupr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsxfrm(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount);
  _CRTIMP size_t __cdecl _wcsxfrm_l(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);
  int __cdecl wcscoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcscoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsncoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsncoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);

#ifndef	NO_OLDNAMES
  wchar_t *__cdecl wcsdup(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#define wcswcs wcsstr
  int __cdecl wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsrev(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcslwr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsupr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif

#ifndef _TM_DEFINED
#define _TM_DEFINED
  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };
#endif

#ifndef _WTIME_DEFINED
#define _WTIME_DEFINED

  _CRTIMP wchar_t *__cdecl _wasctime(const struct tm *_Tm);
  wchar_t *__cdecl _wctime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsftime(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  _CRTIMP size_t __cdecl _wcsftime_l(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wstrdate(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wstrtime(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wctime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#if !defined (RC_INVOKED) && !defined (_INC_WTIME_INL)
#define _INC_WTIME_INL
  wchar_t *__cdecl _wctime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __CRT__NO_INLINE
#ifndef _USE_32BIT_TIME_T
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime64(_Time); }
#else
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime32(_Time); }
#endif
#endif /* __CRT__NO_INLINE */
#endif
#endif

  typedef int mbstate_t;
  typedef wchar_t _Wint_t;

  wint_t __cdecl btowc(int);
  size_t __cdecl mbrlen(const char * __restrict__ _Ch,size_t _SizeInBytes,mbstate_t * __restrict__ _State);
  size_t __cdecl mbrtowc(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SizeInBytes,mbstate_t * __restrict__ _State);
  size_t __cdecl mbsrtowcs(wchar_t * __restrict__ _Dest,const char ** __restrict__ _PSrc,size_t _Count,mbstate_t * __restrict__ _State) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcrtomb(char * __restrict__ _Dest,wchar_t _Source,mbstate_t * __restrict__ _State) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsrtombs(char * __restrict__ _Dest,const wchar_t ** __restrict__ _PSource,size_t _Count,mbstate_t * __restrict__ _State) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wctob(wint_t _WCh);

#ifndef __NO_ISOCEXT /* these need static lib libmingwex.a */
  wchar_t *__cdecl wmemset(wchar_t *s, wchar_t c, size_t n);
  _CONST_RETURN wchar_t *__cdecl wmemchr(const wchar_t *s, wchar_t c, size_t n);
  int __cdecl wmemcmp(const wchar_t *s1, const wchar_t *s2,size_t n);
  wchar_t *__cdecl wmemcpy(wchar_t * __restrict__ s1,const wchar_t * __restrict__ s2,size_t n) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  wchar_t * __cdecl wmempcpy (wchar_t *_Dst, const wchar_t *_Src, size_t _Size);
  wchar_t *__cdecl wmemmove(wchar_t *s1, const wchar_t *s2, size_t n) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl fwide(FILE *stream,int mode);
  int __cdecl mbsinit(const mbstate_t *ps);
  __MINGW_EXTENSION long long __cdecl wcstoll(const wchar_t * __restrict__ nptr,wchar_t ** __restrict__ endptr, int base);
  __MINGW_EXTENSION unsigned long long __cdecl wcstoull(const wchar_t * __restrict__ nptr,wchar_t ** __restrict__ endptr, int base);
#endif /* __NO_ISOCEXT */

  void *__cdecl memmove(void *_Dst,const void *_Src,size_t _MaxCount);
  void *__cdecl memcpy(void * __restrict__ _Dst,const void * __restrict__ _Src,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl fwide(FILE *_F,int _M) { (void)_F; return (_M); }
  __CRT_INLINE int __cdecl mbsinit(const mbstate_t *_P) { return (!_P || *_P==0); }
  __CRT_INLINE _CONST_RETURN wchar_t *__cdecl wmemchr(const wchar_t *_S,wchar_t _C,size_t _N) {
    if (_S) {
      for ( ; 0 < _N; ++_S, --_N)
	if (*_S == _C)
	  return (_CONST_RETURN wchar_t *)(_S);
    }
    return (_CONST_RETURN wchar_t *) NULL;
  }
  __CRT_INLINE int __cdecl wmemcmp(const wchar_t *_S1,const wchar_t *_S2,size_t _N) {
    if (_N == 0 || _S1 == _S2)
      return 0;	/* even for NULL pointers.  */
    if ((_S1 && !(_S2)) || (_S2 && !(_S1)))
      return !(_S2) ? 1 : -1; /* robust.  */
    for ( ; 0 < _N; ++_S1, ++_S2, --_N)
      if (*_S1 != *_S2)
	return (*_S1 < *_S2 ? -1 : +1);
    return 0;
  }
  __CRT_INLINE wchar_t *__cdecl wmemcpy(wchar_t * __restrict__ _S1,const wchar_t * __restrict__ _S2,size_t _N)
  {
    return (wchar_t *) memcpy (_S1,_S2,_N*sizeof(wchar_t));
  }
  __CRT_INLINE wchar_t *__cdecl wmemmove(wchar_t *_S1,const wchar_t *_S2,size_t _N) { return (wchar_t *)memmove(_S1,_S2,_N*sizeof(wchar_t)); }
  __CRT_INLINE wchar_t *__cdecl wmemset(wchar_t *_S,wchar_t _C,size_t _N) {
    wchar_t *_Su = _S;
    for (;0<_N;++_Su,--_N) {
      *_Su = _C;
    }
    return (_S);
  }
#endif /* !__CRT__NO_INLINE */

#ifndef __MINGW_MBWC_CONVERT_DEFINED
#define __MINGW_MBWC_CONVERT_DEFINED

/**
 * __mingw_str_wide_utf8
 * Converts a null terminated UCS-2 string to a multibyte (UTF-8) equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[in] wptr Pointer to wide string.
 * @param[out] mbptr Pointer to multibyte string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return bytes written by WideCharToMultiByte conversion, 0 for failure.
 *
 * WideCharToMultiByte - http://msdn.microsoft.com/en-us/library/dd374130(VS.85).aspx
 */
int __cdecl __mingw_str_wide_utf8 (const wchar_t * const wptr, char **mbptr, size_t * buflen);

/**
 * __mingw_str_utf8_wide
 * Converts a null terminated UTF-8 string to a UCS-2 equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[out] mbptr Pointer to multibyte string.
 * @param[in] wptr Pointer to wide string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return bytes written by WideCharToMultiByte conversion, 0 for failure.
 *
 * MultiByteToWideChar - http://msdn.microsoft.com/en-us/library/dd319072(VS.85).aspx
 */

int __cdecl __mingw_str_utf8_wide (const char *const mbptr, wchar_t ** wptr, size_t * buflen);

/**
 * __mingw_str_free
 * Frees buffer create by __mingw_str_wide_utf8 and __mingw_str_utf8_wide.
 * @param[in] ptr memory block to free.
 *
 */

void __cdecl __mingw_str_free(void *ptr);

#endif /* __MINGW_MBWC_CONVERT_DEFINED */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/wchar_s.h>

#include <_mingw_print_pop.h>

#endif /* _INC_WCHAR */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_WCHAR
#define _INC_WCHAR

#include <crtdefs.h>
#include <_mingw_print_push.h>

#if defined (__USE_MINGW_ANSI_STDIO) && ((__USE_MINGW_ANSI_STDIO + 0) != 0) && !defined (__USE_MINGW_STRTOX)
#define __USE_MINGW_STRTOX 1
#endif

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WCHAR_MIN  /* also in stdint.h */
#define WCHAR_MIN 0U
#define WCHAR_MAX 0xffffU
#endif

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifndef _FILE_DEFINED
  struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
  };
  typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif

#ifndef _STDIO_DEFINED
#ifdef _WIN64
  _CRTIMP FILE *__cdecl __iob_func(void);
#define _iob  __iob_func()
#else
#ifdef _MSVCRT_
extern FILE _iob[];	/* A pointer to an array of FILE */
#define __iob_func()	(_iob)
#else
extern FILE (* __MINGW_IMP_SYMBOL(_iob))[];	/* A pointer to an array of FILE */
#define __iob_func()	(* __MINGW_IMP_SYMBOL(_iob))
#endif
#endif

#define _iob __iob_func()
#endif

#ifndef _STDSTREAM_DEFINED
#define stdin (&__iob_func()[0])
#define stdout (&__iob_func()[1])
#define stderr (&__iob_func()[2])
#define _STDSTREAM_DEFINED
#endif

#ifndef _FSIZE_T_DEFINED
  typedef unsigned long _fsize_t;
#define _FSIZE_T_DEFINED
#endif

#ifndef _WFINDDATA_T_DEFINED
  struct _wfinddata32_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    _fsize_t size;
    wchar_t name[260];
  };

  struct _wfinddata32i64_t {
    unsigned attrib;
    __time32_t time_create;
    __time32_t time_access;
    __time32_t time_write;
    __MINGW_EXTENSION __int64 size;
    wchar_t name[260];
  };

  struct _wfinddata64i32_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    _fsize_t size;
    wchar_t name[260];
  };

  struct _wfinddata64_t {
    unsigned attrib;
    __time64_t time_create;
    __time64_t time_access;
    __time64_t time_write;
    __MINGW_EXTENSION __int64 size;
    wchar_t name[260];
  };
/* #endif */

#ifdef _USE_32BIT_TIME_T
#define _wfinddata_t _wfinddata32_t
#define _wfinddatai64_t _wfinddata32i64_t

#define _wfindfirst32 _wfindfirst
#define _wfindnext32 _wfindnext
#define _wfindfirst32i64 _wfindfirsti64
#define _wfindnext32i64 _wfindnexti64
#else
#define _wfinddata_t _wfinddata64i32_t
#define _wfinddatai64_t _wfinddata64_t

#define _wfindfirst _wfindfirst64i32
#define _wfindnext _wfindnext64i32
#define _wfindfirsti64 _wfindfirst64
#define _wfindnexti64 _wfindnext64
#endif

#define _WFINDDATA_T_DEFINED
#endif

#ifndef NULL
#ifdef __cplusplus
#ifndef _WIN64
#define NULL 0
#else
#define NULL 0LL
#endif  /* W64 */
#else
#define NULL ((void *)0)
#endif
#endif

#ifndef _CONST_RETURN
#define _CONST_RETURN
#endif

#define _WConst_return _CONST_RETURN

#ifndef _CRT_CTYPEDATA_DEFINED
#define _CRT_CTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS

#ifndef __PCTYPE_FUNC
#define __PCTYPE_FUNC __pctype_func()
#ifdef _MSVCRT_
#define __pctype_func() (_pctype)
#else
#define __pctype_func() (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#ifndef _pctype
#ifdef _MSVCRT_
  extern unsigned short *_pctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pctype);
#define _pctype (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif
#endif
#endif

#ifndef _CRT_WCTYPEDATA_DEFINED
#define _CRT_WCTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS
#ifndef _wctype
#ifdef _MSVCRT_
  extern unsigned short *_wctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_wctype);
#define _wctype (* __MINGW_IMP_SYMBOL(_wctype))
#endif
#endif

#ifdef _MSVCRT_
#define __pwctype_func() (_pwctype)
#else
#define __pwctype_func() (* __MINGW_IMP_SYMBOL(_pwctype))
#endif

#ifndef _pwctype
#ifdef _MSVCRT_
  extern unsigned short *_pwctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pwctype);
#define _pwctype (* __MINGW_IMP_SYMBOL(_pwctype))
#endif
#endif

#endif
#endif

#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8

#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80

#define _LEADBYTE 0x8000
#define _ALPHA (0x0100|_UPPER|_LOWER)

#ifndef _WCTYPE_DEFINED
#define _WCTYPE_DEFINED

  int __cdecl iswalpha(wint_t _C);
  _CRTIMP int __cdecl _iswalpha_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswupper(wint_t _C);
  _CRTIMP int __cdecl _iswupper_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswlower(wint_t _C);
  _CRTIMP int __cdecl _iswlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswdigit(wint_t _C);
  _CRTIMP int __cdecl _iswdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswxdigit(wint_t _C);
  _CRTIMP int __cdecl _iswxdigit_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswspace(wint_t _C);
  _CRTIMP int __cdecl _iswspace_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswpunct(wint_t _C);
  _CRTIMP int __cdecl _iswpunct_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswalnum(wint_t _C);
  _CRTIMP int __cdecl _iswalnum_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswprint(wint_t _C);
  _CRTIMP int __cdecl _iswprint_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswgraph(wint_t _C);
  _CRTIMP int __cdecl _iswgraph_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswcntrl(wint_t _C);
  _CRTIMP int __cdecl _iswcntrl_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswascii(wint_t _C);
  int __cdecl isleadbyte(int _C);
  _CRTIMP int __cdecl _isleadbyte_l(int _C,_locale_t _Locale);
  wint_t __cdecl towupper(wint_t _C);
  _CRTIMP wint_t __cdecl _towupper_l(wint_t _C,_locale_t _Locale);
  wint_t __cdecl towlower(wint_t _C);
  _CRTIMP wint_t __cdecl _towlower_l(wint_t _C,_locale_t _Locale);
  int __cdecl iswctype(wint_t _C,wctype_t _Type);
  _CRTIMP int __cdecl _iswctype_l(wint_t _C,wctype_t _Type,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsymf(wint_t _C);
  _CRTIMP int __cdecl _iswcsymf_l(wint_t _C,_locale_t _Locale);
  _CRTIMP int __cdecl __iswcsym(wint_t _C);
  _CRTIMP int __cdecl _iswcsym_l(wint_t _C,_locale_t _Locale);
  int __cdecl is_wctype(wint_t _C,wctype_t _Type);

#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES)
  int __cdecl iswblank(wint_t _C);
#endif

#endif

#ifndef _WDIRECT_DEFINED
#define _WDIRECT_DEFINED

  _CRTIMP wchar_t *__cdecl _wgetcwd(wchar_t *_DstBuf,int _SizeInWords);
  _CRTIMP wchar_t *__cdecl _wgetdcwd(int _Drive,wchar_t *_DstBuf,int _SizeInWords);
  wchar_t *__cdecl _wgetdcwd_nolock(int _Drive,wchar_t *_DstBuf,int _SizeInWords);
  _CRTIMP int __cdecl _wchdir(const wchar_t *_Path);
  _CRTIMP int __cdecl _wmkdir(const wchar_t *_Path);
  _CRTIMP int __cdecl _wrmdir(const wchar_t *_Path);
#endif

#ifndef _WIO_DEFINED
#define _WIO_DEFINED

  _CRTIMP int __cdecl _waccess(const wchar_t *_Filename,int _AccessMode);
  _CRTIMP int __cdecl _wchmod(const wchar_t *_Filename,int _Mode);
  _CRTIMP int __cdecl _wcreat(const wchar_t *_Filename,int _PermissionMode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP intptr_t __cdecl _wfindfirst32(const wchar_t *_Filename,struct _wfinddata32_t *_FindData);
  _CRTIMP int __cdecl _wfindnext32(intptr_t _FindHandle,struct _wfinddata32_t *_FindData);
  _CRTIMP int __cdecl _wunlink(const wchar_t *_Filename);
  _CRTIMP int __cdecl _wrename(const wchar_t *_OldFilename,const wchar_t *_NewFilename);
  _CRTIMP wchar_t *__cdecl _wmktemp(wchar_t *_TemplateName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP intptr_t __cdecl _wfindfirst32i64(const wchar_t *_Filename,struct _wfinddata32i64_t *_FindData);
  intptr_t __cdecl _wfindfirst64i32(const wchar_t *_Filename,struct _wfinddata64i32_t *_FindData);
  _CRTIMP intptr_t __cdecl _wfindfirst64(const wchar_t *_Filename,struct _wfinddata64_t *_FindData);
  _CRTIMP int __cdecl _wfindnext32i64(intptr_t _FindHandle,struct _wfinddata32i64_t *_FindData);
  int __cdecl _wfindnext64i32(intptr_t _FindHandle,struct _wfinddata64i32_t *_FindData);
  _CRTIMP int __cdecl _wfindnext64(intptr_t _FindHandle,struct _wfinddata64_t *_FindData);
  _CRTIMP errno_t __cdecl _wsopen_s(int *_FileHandle,const wchar_t *_Filename,int _OpenFlag,int _ShareFlag,int _PermissionFlag);
  _CRTIMP int __cdecl _wopen(const wchar_t *_Filename,int _OpenFlag,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wsopen(const wchar_t *_Filename,int _OpenFlag,int _ShareFlag,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#endif

#ifndef _WLOCALE_DEFINED
#define _WLOCALE_DEFINED
  _CRTIMP wchar_t *__cdecl _wsetlocale(int _Category,const wchar_t *_Locale);
#endif

#ifndef _WPROCESS_DEFINED
#define _WPROCESS_DEFINED

  _CRTIMP intptr_t __cdecl _wexecl(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexecle(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexeclp(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexeclpe(const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wexecv(const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wexecve(const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  _CRTIMP intptr_t __cdecl _wexecvp(const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wexecvpe(const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  _CRTIMP intptr_t __cdecl _wspawnl(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnle(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnlp(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnlpe(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  _CRTIMP intptr_t __cdecl _wspawnv(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wspawnve(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  _CRTIMP intptr_t __cdecl _wspawnvp(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  _CRTIMP intptr_t __cdecl _wspawnvpe(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
#ifndef _CRT_WSYSTEM_DEFINED
#define _CRT_WSYSTEM_DEFINED
  _CRTIMP int __cdecl _wsystem(const wchar_t *_Command);
#endif
#endif

#ifndef _WCTYPE_INLINE_DEFINED
#undef _CRT_WCTYPE_NOINLINE
#if !defined(__cplusplus) || defined(_CRT_WCTYPE_NOINLINE)
#define iswalpha(_c) (iswctype(_c,_ALPHA))
#define iswupper(_c) (iswctype(_c,_UPPER))
#define iswlower(_c) (iswctype(_c,_LOWER))
#define iswdigit(_c) (iswctype(_c,_DIGIT))
#define iswxdigit(_c) (iswctype(_c,_HEX))
#define iswspace(_c) (iswctype(_c,_SPACE))
#define iswpunct(_c) (iswctype(_c,_PUNCT))
#define iswalnum(_c) (iswctype(_c,_ALPHA|_DIGIT))
#define iswprint(_c) (iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT))
#define iswgraph(_c) (iswctype(_c,_PUNCT|_ALPHA|_DIGIT))
#define iswcntrl(_c) (iswctype(_c,_CONTROL))
#define iswascii(_c) ((unsigned)(_c) < 0x80)

#define _iswalpha_l(_c,_p) (_iswctype_l(_c,_ALPHA,_p))
#define _iswupper_l(_c,_p) (_iswctype_l(_c,_UPPER,_p))
#define _iswlower_l(_c,_p) (_iswctype_l(_c,_LOWER,_p))
#define _iswdigit_l(_c,_p) (_iswctype_l(_c,_DIGIT,_p))
#define _iswxdigit_l(_c,_p) (_iswctype_l(_c,_HEX,_p))
#define _iswspace_l(_c,_p) (_iswctype_l(_c,_SPACE,_p))
#define _iswpunct_l(_c,_p) (_iswctype_l(_c,_PUNCT,_p))
#define _iswalnum_l(_c,_p) (_iswctype_l(_c,_ALPHA|_DIGIT,_p))
#define _iswprint_l(_c,_p) (_iswctype_l(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswgraph_l(_c,_p) (_iswctype_l(_c,_PUNCT|_ALPHA|_DIGIT,_p))
#define _iswcntrl_l(_c,_p) (_iswctype_l(_c,_CONTROL,_p))
#ifndef _CTYPE_DISABLE_MACROS
#define isleadbyte(_c) (__PCTYPE_FUNC[(unsigned char)(_c)] & _LEADBYTE)
#endif
#endif
#define _WCTYPE_INLINE_DEFINED
#endif

#if !defined(_POSIX_) || defined(__GNUC__)
#ifndef _INO_T_DEFINED
#define _INO_T_DEFINED
  typedef unsigned short _ino_t;
#ifndef	NO_OLDNAMES
  typedef unsigned short ino_t;
#endif
#endif

#ifndef _DEV_T_DEFINED
#define _DEV_T_DEFINED
  typedef unsigned int _dev_t;
#ifndef	NO_OLDNAMES
  typedef unsigned int dev_t;
#endif
#endif

#include <_mingw_off_t.h>
#include <_mingw_stat64.h>

#ifndef _WSTAT_DEFINED
#define _WSTAT_DEFINED

  _CRTIMP int __cdecl _wstat32(const wchar_t *_Name,struct _stat32 *_Stat);
  _CRTIMP int __cdecl _wstat32i64(const wchar_t *_Name,struct _stat32i64 *_Stat);
  int __cdecl _wstat64i32(const wchar_t *_Name,struct _stat64i32 *_Stat);
  _CRTIMP int __cdecl _wstat64(const wchar_t *_Name,struct _stat64 *_Stat);
#endif
#endif

#ifndef _WCONIO_DEFINED
#define _WCONIO_DEFINED

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

  _CRTIMP wchar_t *_cgetws(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wint_t __cdecl _getwch(void);
  _CRTIMP wint_t __cdecl _getwche(void);
  _CRTIMP wint_t __cdecl _putwch(wchar_t _WCh);
  _CRTIMP wint_t __cdecl _ungetwch(wint_t _WCh);
  _CRTIMP int __cdecl _cputws(const wchar_t *_String);
  _CRTIMP int __cdecl _cwprintf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _cwscanf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _cwscanf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vcwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _cwprintf_p(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vcwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);

  _CRTIMP int __cdecl _cwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vcwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _cwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vcwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  wint_t __cdecl _putwch_nolock(wchar_t _WCh);
  wint_t __cdecl _getwch_nolock(void);
  wint_t __cdecl _getwche_nolock(void);
  wint_t __cdecl _ungetwch_nolock(wint_t _WCh);
#endif

#ifndef _WSTDIO_DEFINED
#define _WSTDIO_DEFINED

/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswscanf (const wchar_t * __restrict__ _Str,const wchar_t * __restrict__ Format,va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wscanf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwscanf(const wchar_t * __restrict__ Format, va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwscanf (FILE * __restrict__ fp, const wchar_t * __restrict__ Format,va_list argp);


/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_wprintf(const wchar_t * __restrict__ _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
/* __attribute__((__format__ (gnu_printf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
  int __cdecl __mingw_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
  int __cdecl __mingw_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_swprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ , ...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
  int __cdecl __mingw_vswprintf(wchar_t * __restrict__ , const wchar_t * __restrict__ ,va_list);

#undef __mingw_ovr
#if defined (__GNUC__)
#define __mingw_ovr static __attribute__ ((__unused__)) __inline__ __cdecl
#elif defined(__cplusplus)
#define __mingw_ovr inline __cdecl
#else
#define __mingw_ovr static __cdecl
#endif

#if __USE_MINGW_ANSI_STDIO

/*
 * User has expressed a preference for C99 conformance...
 */
__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int swscanf(const wchar_t *__source, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vswscanf( __source, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wscanf(const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwscanf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwscanf(FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwscanf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vswscanf (const wchar_t *__source, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vswscanf( __source, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
{
  return __mingw_vwscanf( __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwscanf( __stream, __format, __local_argv );
}
#endif /* __NO_ISOCEXT */



__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ __MINGW_ATTRIB_NONNULL(2)
int fwprintf (FILE *__stream, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vfwprintf( __stream, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ __MINGW_ATTRIB_NONNULL(1)
int wprintf (const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vwprintf( __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ __MINGW_ATTRIB_NONNULL(2)
int vfwprintf (FILE *__stream, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vfwprintf( __stream, __format, __local_argv );
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 1, 0))) */ __MINGW_ATTRIB_NONNULL(1)
int vwprintf (const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vwprintf( __format, __local_argv );
}

/*#ifndef __NO_ISOCEXT */  /* externs in libmingwex.a */
__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ __MINGW_ATTRIB_NONNULL(3)
int snwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, ...)
{
  register int __retval;
  __builtin_va_list __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

__mingw_ovr
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ __MINGW_ATTRIB_NONNULL(3)
int vsnwprintf (wchar_t *__stream, size_t __n, const wchar_t *__format, __builtin_va_list __local_argv)
{
  return __mingw_vsnwprintf( __stream, __n, __format, __local_argv );
}
/* #endif */ /* __NO_ISOCEXT */
#else /* !__USE_MINGW_ANSI_STDIO */

  int __cdecl fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wscanf(const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */
  int __cdecl __ms_vwscanf (const wchar_t * __restrict__ , va_list);
  int __cdecl __ms_vfwscanf (FILE * __restrict__ ,const wchar_t * __restrict__ ,va_list);
  int __cdecl __ms_vswscanf (const wchar_t * __restrict__ ,const wchar_t * __restrict__ ,va_list);

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vfwscanf (FILE *__stream,  const wchar_t *__format, __builtin_va_list __local_argv)
  {
    return __ms_vfwscanf (__stream, __format, __local_argv);
  }

  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(2)
  int vswscanf (const wchar_t * __restrict__ __source, const wchar_t * __restrict__ __format, __builtin_va_list __local_argv)
  {
    return __ms_vswscanf( __source, __format, __local_argv );
  }
  __mingw_ovr
  __MINGW_ATTRIB_NONNULL(1)
  int vwscanf(const wchar_t *__format,  __builtin_va_list __local_argv)
  {
    return __ms_vwscanf (__format, __local_argv);
  }

#endif /* __NO_ISOCEXT */

  int __cdecl fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  int __cdecl wprintf(const wchar_t * __restrict__ _Format,...);
  int __cdecl vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  int __cdecl vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
#endif /* __USE_MINGW_ANSI_STDIO */


#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifdef _POSIX_
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode);
#else
  _CRTIMP FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);
#endif

  wint_t __cdecl fgetwc(FILE *_File);
  _CRTIMP wint_t __cdecl _fgetwchar(void);
  wint_t __cdecl fputwc(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _fputwchar(wchar_t _Ch);
  wint_t __cdecl getwc(FILE *_File);
  wint_t __cdecl getwchar(void);
  wint_t __cdecl putwc(wchar_t _Ch,FILE *_File);
  wint_t __cdecl putwchar(wchar_t _Ch);
  wint_t __cdecl ungetwc(wint_t _Ch,FILE *_File);
  wchar_t *__cdecl fgetws(wchar_t * __restrict__ _Dst,int _SizeInWords,FILE * __restrict__ _File);
  int __cdecl fputws(const wchar_t * __restrict__ _Str,FILE * __restrict__ _File);
  _CRTIMP wchar_t *__cdecl _getws(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _putws(const wchar_t *_Str);
  _CRTIMP int __cdecl _scwprintf(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _swprintf_l(wchar_t * __restrict__ ,size_t _SizeInWords,const wchar_t * __restrict__ _Format,_locale_t _Locale,... ) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _swprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf_c(wchar_t * __restrict__ _DstBuf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _snwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vsnwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef __NO_ISOCEXT  /* externs in libmingwex.a */

#if !defined (__USE_MINGW_ANSI_STDIO) || __USE_MINGW_ANSI_STDIO == 0
#pragma push_macro("snwprintf")
#pragma push_macro("vsnwprintf")
# undef snwprintf
# undef vsnwprintf
  int __cdecl __ms_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
  int __cdecl __ms_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
  __mingw_ovr
  int snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...)
  {
    int r;
    va_list argp;
    __builtin_va_start (argp, format);
    r = _vsnwprintf (s, n, format, argp);
    __builtin_va_end (argp);
    return r;
  }
  __mingw_ovr
  int __cdecl vsnwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, va_list arg)
  {
    return _vsnwprintf(s,n,format,arg);
  }
#pragma pop_macro ("vsnwprintf")
#pragma pop_macro ("snwprintf")
#endif

#endif

  _CRTIMP int __cdecl _fwprintf_p(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _wprintf_p(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vfwprintf_p(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _vwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _swprintf_p(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf_p(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _scwprintf_p(const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vscwprintf_p(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _wprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _wprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _vwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _fwprintf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _fwprintf_p_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vfwprintf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _vfwprintf_p_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _swprintf_c_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _swprintf_p_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vswprintf_c_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _vswprintf_p_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _scwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _scwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vscwprintf_p_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _snwprintf_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _vsnwprintf_l(wchar_t * __restrict__ _DstBuf,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _swprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _vswprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,va_list _Args);
  _CRTIMP int __cdecl __swprintf_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,_locale_t _Plocinfo,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _vswprintf_l(wchar_t * __restrict__ _Dest,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl __vswprintf_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,_locale_t _Plocinfo,va_list _Args) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef RC_INVOKED
#include <swprintf.inl>
#endif

#ifdef _CRT_NON_CONFORMING_SWPRINTFS
#ifndef __cplusplus
#define _swprintf_l __swprintf_l
#define _vswprintf_l __vswprintf_l
#endif
#endif

  _CRTIMP wchar_t *__cdecl _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);
  _CRTIMP int __cdecl _vscwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  _CRTIMP int __cdecl _vscwprintf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,va_list _ArgList);
  _CRTIMP int __cdecl _fwscanf_l(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,_locale_t _Locale,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _swscanf_l(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,_locale_t _Locale,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _snwscanf(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  _CRTIMP int __cdecl _snwscanf_l(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,_locale_t _Locale,...);
  _CRTIMP int __cdecl _wscanf_l(const wchar_t * __restrict__ _Format,_locale_t _Locale,...) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP FILE *__cdecl _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  _CRTIMP FILE *__cdecl _wfopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP FILE *__cdecl _wfreopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode,FILE * __restrict__ _OldFile) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP FILE *__cdecl _wpopen(const wchar_t *_Command,const wchar_t *_Mode);
#if !defined(NO_OLDNAMES) && !defined(wpopen)
#define wpopen	_wpopen
#endif
  _CRTIMP int __cdecl _wremove(const wchar_t *_Filename);
  _CRTIMP wchar_t *__cdecl _wtmpnam(wchar_t *_Buffer);
  _CRTIMP wint_t __cdecl _fgetwc_nolock(FILE *_File);
  _CRTIMP wint_t __cdecl _fputwc_nolock(wchar_t _Ch,FILE *_File);
  _CRTIMP wint_t __cdecl _ungetwc_nolock(wint_t _Ch,FILE *_File);

#undef _CRT_GETPUTWCHAR_NOINLINE

#if !defined(__cplusplus) || defined(_CRT_GETPUTWCHAR_NOINLINE) || defined (__CRT__NO_INLINE)
#define getwchar() fgetwc(stdin)
#define putwchar(_c) fputwc((_c),stdout)
#else
  __CRT_INLINE wint_t __cdecl getwchar() {return (fgetwc(stdin)); }
  __CRT_INLINE wint_t __cdecl putwchar(wchar_t _C) {return (fputwc(_C,stdout)); }
#endif

#define getwc(_stm) fgetwc(_stm)
#define putwc(_c,_stm) fputwc(_c,_stm)
#define _putwc_nolock(_c,_stm) _fputwc_nolock(_c,_stm)
#define _getwc_nolock(_c) _fgetwc_nolock(_c)
#endif

#ifndef _WSTDLIB_DEFINED
#define _WSTDLIB_DEFINED

  _CRTIMP wchar_t *__cdecl _itow(int _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ltow(long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _ultow(unsigned long _Value,wchar_t *_Dest,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP double __cdecl _wcstod_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,_locale_t _Locale);

  double __cdecl __mingw_wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl __mingw_wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
  long double __cdecl __mingw_wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);

#if __USE_MINGW_ANSI_STDIO
  __mingw_ovr
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstod(_Str,_EndPtr);
  }
  __mingw_ovr
  float __cdecl wcstof(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr){
    return __mingw_wcstof(_Str,_EndPtr);
  }
  /* wcstold is already a mingw implementation */
#else
  double __cdecl wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __cdecl wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
#endif /* defined(__USE_MINGW_STRTOX) */
#if !defined __NO_ISOCEXT /* in libmingwex.a */
  long double __cdecl wcstold (const wchar_t * __restrict__, wchar_t ** __restrict__);
#endif /* __NO_ISOCEXT */
  long __cdecl wcstol(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP long __cdecl _wcstol_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __cdecl wcstoul(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  _CRTIMP unsigned long __cdecl _wcstoul_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wgetenv(const wchar_t *_VarName) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef _CRT_WSYSTEM_DEFINED
#define _CRT_WSYSTEM_DEFINED
  _CRTIMP int __cdecl _wsystem(const wchar_t *_Command);
#endif
  _CRTIMP double __cdecl _wtof(const wchar_t *_Str);
  _CRTIMP double __cdecl _wtof_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP int __cdecl _wtoi(const wchar_t *_Str);
  _CRTIMP int __cdecl _wtoi_l(const wchar_t *_Str,_locale_t _Locale);
  _CRTIMP long __cdecl _wtol(const wchar_t *_Str);
  _CRTIMP long __cdecl _wtol_l(const wchar_t *_Str,_locale_t _Locale);

  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _i64tow(__int64 _Val,wchar_t *_DstBuf,int _Radix) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  __MINGW_EXTENSION _CRTIMP wchar_t *__cdecl _ui64tow(unsigned __int64 _Val,wchar_t *_DstBuf,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64(const wchar_t *_Str);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wtoi64_l(const wchar_t *_Str,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP __int64 __cdecl _wcstoi64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __MINGW_EXTENSION _CRTIMP unsigned __int64 __cdecl _wcstoui64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
#endif

#ifndef _POSIX_
#ifndef _WSTDLIBP_DEFINED
#define _WSTDLIBP_DEFINED
  _CRTIMP wchar_t *__cdecl _wfullpath(wchar_t *_FullPath,const wchar_t *_Path,size_t _SizeInWords);
  _CRTIMP void __cdecl _wmakepath(wchar_t *_ResultPath,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);
#ifndef _CRT_WPERROR_DEFINED
#define _CRT_WPERROR_DEFINED
  _CRTIMP void __cdecl _wperror(const wchar_t *_ErrMsg);
#endif
  _CRTIMP int __cdecl _wputenv(const wchar_t *_EnvString);
  _CRTIMP void __cdecl _wsearchenv(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP void __cdecl _wsplitpath(const wchar_t *_FullPath,wchar_t *_Drive,wchar_t *_Dir,wchar_t *_Filename,wchar_t *_Ext) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#endif
#endif

#ifndef _WSTRING_DEFINED
#define _WSTRING_DEFINED
  _CRTIMP wchar_t *__cdecl _wcsdup(const wchar_t *_Str);
  wchar_t *__cdecl wcscat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcschr(const wchar_t *_Str,wchar_t _Ch);
  int __cdecl wcscmp(const wchar_t *_Str1,const wchar_t *_Str2);
  wchar_t *__cdecl wcscpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcscspn(const wchar_t *_Str,const wchar_t *_Control);
  size_t __cdecl wcslen(const wchar_t *_Str);
  size_t __cdecl wcsnlen(const wchar_t *_Src,size_t _MaxCount);
  wchar_t *__cdecl wcsncat(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wcsncmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  wchar_t *__cdecl wcsncpy(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  wchar_t *__cdecl _wcsncpy_l(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _Count,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CONST_RETURN wchar_t *__cdecl wcspbrk(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsrchr(const wchar_t *_Str,wchar_t _Ch);
  size_t __cdecl wcsspn(const wchar_t *_Str,const wchar_t *_Control);
  _CONST_RETURN wchar_t *__cdecl wcsstr(const wchar_t *_Str,const wchar_t *_SubStr);
  wchar_t *__cdecl wcstok(wchar_t * __restrict__ _Str,const wchar_t * __restrict__ _Delim) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcserror(int _ErrNum) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl __wcserror(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP int __cdecl _wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicmp_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsrev(wchar_t *_Str);
  _CRTIMP wchar_t *__cdecl _wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcslwr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcslwr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wcsupr(wchar_t *_String) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *_wcsupr_l(wchar_t *_String,_locale_t _Locale) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsxfrm(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount);
  _CRTIMP size_t __cdecl _wcsxfrm_l(wchar_t * __restrict__ _Dst,const wchar_t * __restrict__ _Src,size_t _MaxCount,_locale_t _Locale);
  int __cdecl wcscoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcscoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2);
  _CRTIMP int __cdecl _wcsicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsncoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsncoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);
  _CRTIMP int __cdecl _wcsnicoll(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount);
  _CRTIMP int __cdecl _wcsnicoll_l(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount,_locale_t _Locale);

#ifndef	NO_OLDNAMES
  wchar_t *__cdecl wcsdup(const wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#define wcswcs wcsstr
  int __cdecl wcsicmp(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsnicmp(const wchar_t *_Str1,const wchar_t *_Str2,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsnset(wchar_t *_Str,wchar_t _Val,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsrev(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsset(wchar_t *_Str,wchar_t _Val) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcslwr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  wchar_t *__cdecl wcsupr(wchar_t *_Str) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
  int __cdecl wcsicoll(const wchar_t *_Str1,const wchar_t *_Str2) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
#endif
#endif

#ifndef _TM_DEFINED
#define _TM_DEFINED
  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };
#endif

#ifndef _WTIME_DEFINED
#define _WTIME_DEFINED

  _CRTIMP wchar_t *__cdecl _wasctime(const struct tm *_Tm);
  wchar_t *__cdecl _wctime32(const __time32_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsftime(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  _CRTIMP size_t __cdecl _wcsftime_l(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  _CRTIMP wchar_t *__cdecl _wstrdate(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wstrtime(wchar_t *_Buffer) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  _CRTIMP wchar_t *__cdecl _wctime64(const __time64_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;

#if !defined (RC_INVOKED) && !defined (_INC_WTIME_INL)
#define _INC_WTIME_INL
  wchar_t *__cdecl _wctime(const time_t *_Time) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __CRT__NO_INLINE
#ifndef _USE_32BIT_TIME_T
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime64(_Time); }
#else
  __CRT_INLINE wchar_t *__cdecl _wctime(const time_t *_Time) { return _wctime32(_Time); }
#endif
#endif /* __CRT__NO_INLINE */
#endif
#endif

  typedef int mbstate_t;
  typedef wchar_t _Wint_t;

  wint_t __cdecl btowc(int);
  size_t __cdecl mbrlen(const char * __restrict__ _Ch,size_t _SizeInBytes,mbstate_t * __restrict__ _State);
  size_t __cdecl mbrtowc(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SizeInBytes,mbstate_t * __restrict__ _State);
  size_t __cdecl mbsrtowcs(wchar_t * __restrict__ _Dest,const char ** __restrict__ _PSrc,size_t _Count,mbstate_t * __restrict__ _State) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcrtomb(char * __restrict__ _Dest,wchar_t _Source,mbstate_t * __restrict__ _State) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  size_t __cdecl wcsrtombs(char * __restrict__ _Dest,const wchar_t ** __restrict__ _PSource,size_t _Count,mbstate_t * __restrict__ _State) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl wctob(wint_t _WCh);

#ifndef __NO_ISOCEXT /* these need static lib libmingwex.a */
  wchar_t *__cdecl wmemset(wchar_t *s, wchar_t c, size_t n);
  _CONST_RETURN wchar_t *__cdecl wmemchr(const wchar_t *s, wchar_t c, size_t n);
  int __cdecl wmemcmp(const wchar_t *s1, const wchar_t *s2,size_t n);
  wchar_t *__cdecl wmemcpy(wchar_t * __restrict__ s1,const wchar_t * __restrict__ s2,size_t n) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  wchar_t * __cdecl wmempcpy (wchar_t *_Dst, const wchar_t *_Src, size_t _Size);
  wchar_t *__cdecl wmemmove(wchar_t *s1, const wchar_t *s2, size_t n) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
  int __cdecl fwide(FILE *stream,int mode);
  int __cdecl mbsinit(const mbstate_t *ps);
  __MINGW_EXTENSION long long __cdecl wcstoll(const wchar_t * __restrict__ nptr,wchar_t ** __restrict__ endptr, int base);
  __MINGW_EXTENSION unsigned long long __cdecl wcstoull(const wchar_t * __restrict__ nptr,wchar_t ** __restrict__ endptr, int base);
#endif /* __NO_ISOCEXT */

  void *__cdecl memmove(void *_Dst,const void *_Src,size_t _MaxCount);
  void *__cdecl memcpy(void * __restrict__ _Dst,const void * __restrict__ _Src,size_t _MaxCount) __MINGW_ATTRIB_DEPRECATED_SEC_WARN;
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl fwide(FILE *_F,int _M) { (void)_F; return (_M); }
  __CRT_INLINE int __cdecl mbsinit(const mbstate_t *_P) { return (!_P || *_P==0); }
  __CRT_INLINE _CONST_RETURN wchar_t *__cdecl wmemchr(const wchar_t *_S,wchar_t _C,size_t _N) {
    if (_S) {
      for ( ; 0 < _N; ++_S, --_N)
	if (*_S == _C)
	  return (_CONST_RETURN wchar_t *)(_S);
    }
    return (_CONST_RETURN wchar_t *) NULL;
  }
  __CRT_INLINE int __cdecl wmemcmp(const wchar_t *_S1,const wchar_t *_S2,size_t _N) {
    if (_N == 0 || _S1 == _S2)
      return 0;	/* even for NULL pointers.  */
    if ((_S1 && !(_S2)) || (_S2 && !(_S1)))
      return !(_S2) ? 1 : -1; /* robust.  */
    for ( ; 0 < _N; ++_S1, ++_S2, --_N)
      if (*_S1 != *_S2)
	return (*_S1 < *_S2 ? -1 : +1);
    return 0;
  }
  __CRT_INLINE wchar_t *__cdecl wmemcpy(wchar_t * __restrict__ _S1,const wchar_t * __restrict__ _S2,size_t _N)
  {
    return (wchar_t *) memcpy (_S1,_S2,_N*sizeof(wchar_t));
  }
  __CRT_INLINE wchar_t *__cdecl wmemmove(wchar_t *_S1,const wchar_t *_S2,size_t _N) { return (wchar_t *)memmove(_S1,_S2,_N*sizeof(wchar_t)); }
  __CRT_INLINE wchar_t *__cdecl wmemset(wchar_t *_S,wchar_t _C,size_t _N) {
    wchar_t *_Su = _S;
    for (;0<_N;++_Su,--_N) {
      *_Su = _C;
    }
    return (_S);
  }
#endif /* !__CRT__NO_INLINE */

#ifndef __MINGW_MBWC_CONVERT_DEFINED
#define __MINGW_MBWC_CONVERT_DEFINED

/**
 * __mingw_str_wide_utf8
 * Converts a null terminated UCS-2 string to a multibyte (UTF-8) equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[in] wptr Pointer to wide string.
 * @param[out] mbptr Pointer to multibyte string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return bytes written by WideCharToMultiByte conversion, 0 for failure.
 *
 * WideCharToMultiByte - http://msdn.microsoft.com/en-us/library/dd374130(VS.85).aspx
 */
int __cdecl __mingw_str_wide_utf8 (const wchar_t * const wptr, char **mbptr, size_t * buflen);

/**
 * __mingw_str_utf8_wide
 * Converts a null terminated UTF-8 string to a UCS-2 equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[out] mbptr Pointer to multibyte string.
 * @param[in] wptr Pointer to wide string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return bytes written by WideCharToMultiByte conversion, 0 for failure.
 *
 * MultiByteToWideChar - http://msdn.microsoft.com/en-us/library/dd319072(VS.85).aspx
 */

int __cdecl __mingw_str_utf8_wide (const char *const mbptr, wchar_t ** wptr, size_t * buflen);

/**
 * __mingw_str_free
 * Frees buffer create by __mingw_str_wide_utf8 and __mingw_str_utf8_wide.
 * @param[in] ptr memory block to free.
 *
 */

void __cdecl __mingw_str_free(void *ptr);

#endif /* __MINGW_MBWC_CONVERT_DEFINED */

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#include <sec_api/wchar_s.h>

#include <_mingw_print_pop.h>

#endif /* _INC_WCHAR */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_WCTYPE
#define _INC_WCTYPE

#ifndef _WIN32
#error Only Win32 target is supported!
#endif

#include <crtdefs.h>

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRTIMP
#define _CRTIMP __declspec(dllimport)
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#ifndef __cplusplus
  typedef unsigned short wchar_t;
#endif /* C++ */
#endif /* _WCHAR_T_DEFINED */

#ifndef _WCTYPE_T_DEFINED
#define _WCTYPE_T_DEFINED
  typedef unsigned short wint_t;
  typedef unsigned short wctype_t;
#endif /* _WCTYPE_T_DEFINED */

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifndef _CRT_CTYPEDATA_DEFINED
#define _CRT_CTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS

#ifndef __PCTYPE_FUNC
#define __PCTYPE_FUNC __pctype_func()
#ifdef _MSVCRT_
#define __pctype_func() (_pctype)
#else
#define __pctype_func() (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#ifndef _pctype
#ifdef _MSVCRT_
  extern unsigned short *_pctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pctype);
#define _pctype (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#endif
#endif

#ifndef _CRT_WCTYPEDATA_DEFINED
#define _CRT_WCTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS
#ifndef _wctype
#ifdef _MSVCRT_
  extern unsigned short *_wctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_wctype);
#define _wctype (* __MINGW_IMP_SYMBOL(_wctype))
#endif
#endif

#ifndef _pwctype
#ifdef _MSVCRT_
  extern unsigned short *_pwctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pwctype);
#define _pwctype (* __MINGW_IMP_SYMBOL(_pwctype))
#define __pwctype_func() (* __MINGW_IMP_SYMBOL(_pwctype))
#endif
#endif
#endif
#endif

#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8

#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80

#define _LEADBYTE 0x8000
#define _ALPHA (0x0100|_UPPER|_LOWER)

#ifndef _WCTYPE_DEFINED
#define _WCTYPE_DEFINED

  int __cdecl iswalpha(wint_t);
  int __cdecl iswupper(wint_t);
  int __cdecl iswlower(wint_t);
  int __cdecl iswdigit(wint_t);
  int __cdecl iswxdigit(wint_t);
  int __cdecl iswspace(wint_t);
  int __cdecl iswpunct(wint_t);
  int __cdecl iswalnum(wint_t);
  int __cdecl iswprint(wint_t);
  int __cdecl iswgraph(wint_t);
  int __cdecl iswcntrl(wint_t);
  int __cdecl iswascii(wint_t);
  int __cdecl isleadbyte(int);
  wint_t __cdecl towupper(wint_t);
  wint_t __cdecl towlower(wint_t);
  int __cdecl iswctype(wint_t,wctype_t);
  _CRTIMP int __cdecl __iswcsymf(wint_t);
  _CRTIMP int __cdecl __iswcsym(wint_t);
  int __cdecl is_wctype(wint_t,wctype_t);
#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES) || defined (__cplusplus)
int __cdecl iswblank(wint_t _C);
#endif
#endif

#ifndef _WCTYPE_INLINE_DEFINED
#define _WCTYPE_INLINE_DEFINED
#ifndef __cplusplus
#define iswalpha(_c) (iswctype(_c,_ALPHA))
#define iswupper(_c) (iswctype(_c,_UPPER))
#define iswlower(_c) (iswctype(_c,_LOWER))
#define iswdigit(_c) (iswctype(_c,_DIGIT))
#define iswxdigit(_c) (iswctype(_c,_HEX))
#define iswspace(_c) (iswctype(_c,_SPACE))
#define iswpunct(_c) (iswctype(_c,_PUNCT))
#define iswalnum(_c) (iswctype(_c,_ALPHA|_DIGIT))
#define iswprint(_c) (iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT))
#define iswgraph(_c) (iswctype(_c,_PUNCT|_ALPHA|_DIGIT))
#define iswcntrl(_c) (iswctype(_c,_CONTROL))
#define iswascii(_c) ((unsigned)(_c) < 0x80)
#define isleadbyte(c) (__pctype_func()[(unsigned char)(c)] & _LEADBYTE)
#else
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl iswalpha(wint_t _C) {return (iswctype(_C,_ALPHA)); }
  __CRT_INLINE int __cdecl iswupper(wint_t _C) {return (iswctype(_C,_UPPER)); }
  __CRT_INLINE int __cdecl iswlower(wint_t _C) {return (iswctype(_C,_LOWER)); }
  __CRT_INLINE int __cdecl iswdigit(wint_t _C) {return (iswctype(_C,_DIGIT)); }
  __CRT_INLINE int __cdecl iswxdigit(wint_t _C) {return (iswctype(_C,_HEX)); }
  __CRT_INLINE int __cdecl iswspace(wint_t _C) {return (iswctype(_C,_SPACE)); }
  __CRT_INLINE int __cdecl iswpunct(wint_t _C) {return (iswctype(_C,_PUNCT)); }
  __CRT_INLINE int __cdecl iswalnum(wint_t _C) {return (iswctype(_C,_ALPHA|_DIGIT)); }
  __CRT_INLINE int __cdecl iswprint(wint_t _C) {return (iswctype(_C,_BLANK|_PUNCT|_ALPHA|_DIGIT)); }
  __CRT_INLINE int __cdecl iswgraph(wint_t _C) {return (iswctype(_C,_PUNCT|_ALPHA|_DIGIT)); }
  __CRT_INLINE int __cdecl iswcntrl(wint_t _C) {return (iswctype(_C,_CONTROL)); }
  __CRT_INLINE int __cdecl iswascii(wint_t _C) {return ((unsigned)(_C) < 0x80); }
  __CRT_INLINE int __cdecl isleadbyte(int _C) {return (__pctype_func()[(unsigned char)(_C)] & _LEADBYTE); }
#endif /* !__CRT__NO_INLINE */
#endif /* __cplusplus */
#endif

  typedef wchar_t wctrans_t;
  wint_t __cdecl towctrans(wint_t,wctrans_t);
  wctrans_t __cdecl wctrans(const char *);
  wctype_t __cdecl wctype(const char *);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _INC_WCTYPE
#define _INC_WCTYPE

#ifndef _WIN32
#error Only Win32 target is supported!
#endif

#include <crtdefs.h>

#pragma pack(push,_CRT_PACKING)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _CRTIMP
#define _CRTIMP __declspec(dllimport)
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#ifndef __cplusplus
  typedef unsigned short wchar_t;
#endif /* C++ */
#endif /* _WCHAR_T_DEFINED */

#ifndef _WCTYPE_T_DEFINED
#define _WCTYPE_T_DEFINED
  typedef unsigned short wint_t;
  typedef unsigned short wctype_t;
#endif /* _WCTYPE_T_DEFINED */

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

#ifndef _CRT_CTYPEDATA_DEFINED
#define _CRT_CTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS

#ifndef __PCTYPE_FUNC
#define __PCTYPE_FUNC __pctype_func()
#ifdef _MSVCRT_
#define __pctype_func() (_pctype)
#else
#define __pctype_func() (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#ifndef _pctype
#ifdef _MSVCRT_
  extern unsigned short *_pctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pctype);
#define _pctype (* __MINGW_IMP_SYMBOL(_pctype))
#endif
#endif

#endif
#endif

#ifndef _CRT_WCTYPEDATA_DEFINED
#define _CRT_WCTYPEDATA_DEFINED
#ifndef _CTYPE_DISABLE_MACROS
#ifndef _wctype
#ifdef _MSVCRT_
  extern unsigned short *_wctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_wctype);
#define _wctype (* __MINGW_IMP_SYMBOL(_wctype))
#endif
#endif

#ifndef _pwctype
#ifdef _MSVCRT_
  extern unsigned short *_pwctype;
#else
  extern unsigned short ** __MINGW_IMP_SYMBOL(_pwctype);
#define _pwctype (* __MINGW_IMP_SYMBOL(_pwctype))
#define __pwctype_func() (* __MINGW_IMP_SYMBOL(_pwctype))
#endif
#endif
#endif
#endif

#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8

#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80

#define _LEADBYTE 0x8000
#define _ALPHA (0x0100|_UPPER|_LOWER)

#ifndef _WCTYPE_DEFINED
#define _WCTYPE_DEFINED

  int __cdecl iswalpha(wint_t);
  int __cdecl iswupper(wint_t);
  int __cdecl iswlower(wint_t);
  int __cdecl iswdigit(wint_t);
  int __cdecl iswxdigit(wint_t);
  int __cdecl iswspace(wint_t);
  int __cdecl iswpunct(wint_t);
  int __cdecl iswalnum(wint_t);
  int __cdecl iswprint(wint_t);
  int __cdecl iswgraph(wint_t);
  int __cdecl iswcntrl(wint_t);
  int __cdecl iswascii(wint_t);
  int __cdecl isleadbyte(int);
  wint_t __cdecl towupper(wint_t);
  wint_t __cdecl towlower(wint_t);
  int __cdecl iswctype(wint_t,wctype_t);
  _CRTIMP int __cdecl __iswcsymf(wint_t);
  _CRTIMP int __cdecl __iswcsym(wint_t);
  int __cdecl is_wctype(wint_t,wctype_t);
#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined (NO_OLDNAMES) || defined (__cplusplus)
int __cdecl iswblank(wint_t _C);
#endif
#endif

#ifndef _WCTYPE_INLINE_DEFINED
#define _WCTYPE_INLINE_DEFINED
#ifndef __cplusplus
#define iswalpha(_c) (iswctype(_c,_ALPHA))
#define iswupper(_c) (iswctype(_c,_UPPER))
#define iswlower(_c) (iswctype(_c,_LOWER))
#define iswdigit(_c) (iswctype(_c,_DIGIT))
#define iswxdigit(_c) (iswctype(_c,_HEX))
#define iswspace(_c) (iswctype(_c,_SPACE))
#define iswpunct(_c) (iswctype(_c,_PUNCT))
#define iswalnum(_c) (iswctype(_c,_ALPHA|_DIGIT))
#define iswprint(_c) (iswctype(_c,_BLANK|_PUNCT|_ALPHA|_DIGIT))
#define iswgraph(_c) (iswctype(_c,_PUNCT|_ALPHA|_DIGIT))
#define iswcntrl(_c) (iswctype(_c,_CONTROL))
#define iswascii(_c) ((unsigned)(_c) < 0x80)
#define isleadbyte(c) (__pctype_func()[(unsigned char)(c)] & _LEADBYTE)
#else
#ifndef __CRT__NO_INLINE
  __CRT_INLINE int __cdecl iswalpha(wint_t _C) {return (iswctype(_C,_ALPHA)); }
  __CRT_INLINE int __cdecl iswupper(wint_t _C) {return (iswctype(_C,_UPPER)); }
  __CRT_INLINE int __cdecl iswlower(wint_t _C) {return (iswctype(_C,_LOWER)); }
  __CRT_INLINE int __cdecl iswdigit(wint_t _C) {return (iswctype(_C,_DIGIT)); }
  __CRT_INLINE int __cdecl iswxdigit(wint_t _C) {return (iswctype(_C,_HEX)); }
  __CRT_INLINE int __cdecl iswspace(wint_t _C) {return (iswctype(_C,_SPACE)); }
  __CRT_INLINE int __cdecl iswpunct(wint_t _C) {return (iswctype(_C,_PUNCT)); }
  __CRT_INLINE int __cdecl iswalnum(wint_t _C) {return (iswctype(_C,_ALPHA|_DIGIT)); }
  __CRT_INLINE int __cdecl iswprint(wint_t _C) {return (iswctype(_C,_BLANK|_PUNCT|_ALPHA|_DIGIT)); }
  __CRT_INLINE int __cdecl iswgraph(wint_t _C) {return (iswctype(_C,_PUNCT|_ALPHA|_DIGIT)); }
  __CRT_INLINE int __cdecl iswcntrl(wint_t _C) {return (iswctype(_C,_CONTROL)); }
  __CRT_INLINE int __cdecl iswascii(wint_t _C) {return ((unsigned)(_C) < 0x80); }
  __CRT_INLINE int __cdecl isleadbyte(int _C) {return (__pctype_func()[(unsigned char)(_C)] & _LEADBYTE); }
#endif /* !__CRT__NO_INLINE */
#endif /* __cplusplus */
#endif

  typedef wchar_t wctrans_t;
  wint_t __cdecl towctrans(wint_t,wctrans_t);
  wctrans_t __cdecl wctrans(const char *);
  wctype_t __cdecl wctype(const char *);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif

// <algorithm> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/algorithm
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_ALGORITHM
#define _GLIBCXX_ALGORITHM 1

#pragma GCC system_header

#include <utility> // UK-300.
#include <bits/stl_algobase.h>
#include <bits/stl_algo.h>

#ifdef _GLIBCXX_PARALLEL
# include <parallel/algorithm>
#endif

#endif /* _GLIBCXX_ALGORITHM */

// <bitset> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 * Copyright (c) 1998
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/bitset
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_BITSET
#define _GLIBCXX_BITSET 1

#pragma GCC system_header

#include <string>
#include <bits/functexcept.h>   // For invalid_argument, out_of_range,
                                // overflow_error
#include <iosfwd>
#include <bits/cxxabi_forced.h>

#define _GLIBCXX_BITSET_BITS_PER_WORD  (__CHAR_BIT__ * __SIZEOF_LONG__)
#define _GLIBCXX_BITSET_WORDS(__n) \
  ((__n) / _GLIBCXX_BITSET_BITS_PER_WORD + \
   ((__n) % _GLIBCXX_BITSET_BITS_PER_WORD == 0 ? 0 : 1))

#define _GLIBCXX_BITSET_BITS_PER_ULL (__CHAR_BIT__ * __SIZEOF_LONG_LONG__)

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_CONTAINER

  /**
   *  Base class, general case.  It is a class invariant that _Nw will be
   *  nonnegative.
   *
   *  See documentation for bitset.
  */
  template<size_t _Nw>
    struct _Base_bitset
    {
      typedef unsigned long _WordT;

      /// 0 is the least significant word.
      _WordT 		_M_w[_Nw];

      _GLIBCXX_CONSTEXPR _Base_bitset() _GLIBCXX_NOEXCEPT
      : _M_w() { }

#if __cplusplus >= 201103L
      constexpr _Base_bitset(unsigned long long __val) noexcept
      : _M_w{ _WordT(__val)
#if __SIZEOF_LONG_LONG__ > __SIZEOF_LONG__
	       , _WordT(__val >> _GLIBCXX_BITSET_BITS_PER_WORD)
#endif
       } { }
#else
      _Base_bitset(unsigned long __val)
      : _M_w()
      { _M_w[0] = __val; }
#endif

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichword(size_t __pos) _GLIBCXX_NOEXCEPT
      { return __pos / _GLIBCXX_BITSET_BITS_PER_WORD; }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichbyte(size_t __pos) _GLIBCXX_NOEXCEPT
      { return (__pos % _GLIBCXX_BITSET_BITS_PER_WORD) / __CHAR_BIT__; }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichbit(size_t __pos) _GLIBCXX_NOEXCEPT
      { return __pos % _GLIBCXX_BITSET_BITS_PER_WORD; }

      static _GLIBCXX_CONSTEXPR _WordT
      _S_maskbit(size_t __pos) _GLIBCXX_NOEXCEPT
      { return (static_cast<_WordT>(1)) << _S_whichbit(__pos); }

      _WordT&
      _M_getword(size_t __pos) _GLIBCXX_NOEXCEPT
      { return _M_w[_S_whichword(__pos)]; }

      _GLIBCXX_CONSTEXPR _WordT
      _M_getword(size_t __pos) const _GLIBCXX_NOEXCEPT
      { return _M_w[_S_whichword(__pos)]; }

#if __cplusplus >= 201103L
      const _WordT*
      _M_getdata() const noexcept
      { return _M_w; }
#endif

      _WordT&
      _M_hiword() _GLIBCXX_NOEXCEPT
      { return _M_w[_Nw - 1]; }

      _GLIBCXX_CONSTEXPR _WordT
      _M_hiword() const _GLIBCXX_NOEXCEPT
      { return _M_w[_Nw - 1]; }

      void
      _M_do_and(const _Base_bitset<_Nw>& __x) _GLIBCXX_NOEXCEPT
      {
	for (size_t __i = 0; __i < _Nw; __i++)
	  _M_w[__i] &= __x._M_w[__i];
      }

      void
      _M_do_or(const _Base_bitset<_Nw>& __x) _GLIBCXX_NOEXCEPT
      {
	for (size_t __i = 0; __i < _Nw; __i++)
	  _M_w[__i] |= __x._M_w[__i];
      }

      void
      _M_do_xor(const _Base_bitset<_Nw>& __x) _GLIBCXX_NOEXCEPT
      {
	for (size_t __i = 0; __i < _Nw; __i++)
	  _M_w[__i] ^= __x._M_w[__i];
      }

      void
      _M_do_left_shift(size_t __shift) _GLIBCXX_NOEXCEPT;

      void
      _M_do_right_shift(size_t __shift) _GLIBCXX_NOEXCEPT;

      void
      _M_do_flip() _GLIBCXX_NOEXCEPT
      {
	for (size_t __i = 0; __i < _Nw; __i++)
	  _M_w[__i] = ~_M_w[__i];
      }

      void
      _M_do_set() _GLIBCXX_NOEXCEPT
      {
	for (size_t __i = 0; __i < _Nw; __i++)
	  _M_w[__i] = ~static_cast<_WordT>(0);
      }

      void
      _M_do_reset() _GLIBCXX_NOEXCEPT
      { __builtin_memset(_M_w, 0, _Nw * sizeof(_WordT)); }

      bool
      _M_is_equal(const _Base_bitset<_Nw>& __x) const _GLIBCXX_NOEXCEPT
      {
	for (size_t __i = 0; __i < _Nw; ++__i)
	  if (_M_w[__i] != __x._M_w[__i])
	    return false;
	return true;
      }

      template<size_t _Nb>
        bool
        _M_are_all() const _GLIBCXX_NOEXCEPT
        {
	  for (size_t __i = 0; __i < _Nw - 1; __i++)
	    if (_M_w[__i] != ~static_cast<_WordT>(0))
	      return false;
	  return _M_hiword() == (~static_cast<_WordT>(0)
				 >> (_Nw * _GLIBCXX_BITSET_BITS_PER_WORD
				     - _Nb));
	}

      bool
      _M_is_any() const _GLIBCXX_NOEXCEPT
      {
	for (size_t __i = 0; __i < _Nw; __i++)
	  if (_M_w[__i] != static_cast<_WordT>(0))
	    return true;
	return false;
      }

      size_t
      _M_do_count() const _GLIBCXX_NOEXCEPT
      {
	size_t __result = 0;
	for (size_t __i = 0; __i < _Nw; __i++)
	  __result += __builtin_popcountl(_M_w[__i]);
	return __result;
      }

      unsigned long
      _M_do_to_ulong() const;

#if __cplusplus >= 201103L
      unsigned long long
      _M_do_to_ullong() const;
#endif

      // find first "on" bit
      size_t
      _M_do_find_first(size_t) const _GLIBCXX_NOEXCEPT;

      // find the next "on" bit that follows "prev"
      size_t
      _M_do_find_next(size_t, size_t) const _GLIBCXX_NOEXCEPT;
    };

  // Definitions of non-inline functions from _Base_bitset.
  template<size_t _Nw>
    void
    _Base_bitset<_Nw>::_M_do_left_shift(size_t __shift) _GLIBCXX_NOEXCEPT
    {
      if (__builtin_expect(__shift != 0, 1))
	{
	  const size_t __wshift = __shift / _GLIBCXX_BITSET_BITS_PER_WORD;
	  const size_t __offset = __shift % _GLIBCXX_BITSET_BITS_PER_WORD;

	  if (__offset == 0)
	    for (size_t __n = _Nw - 1; __n >= __wshift; --__n)
	      _M_w[__n] = _M_w[__n - __wshift];
	  else
	    {
	      const size_t __sub_offset = (_GLIBCXX_BITSET_BITS_PER_WORD 
					   - __offset);
	      for (size_t __n = _Nw - 1; __n > __wshift; --__n)
		_M_w[__n] = ((_M_w[__n - __wshift] << __offset)
			     | (_M_w[__n - __wshift - 1] >> __sub_offset));
	      _M_w[__wshift] = _M_w[0] << __offset;
	    }

	  std::fill(_M_w + 0, _M_w + __wshift, static_cast<_WordT>(0));
	}
    }

  template<size_t _Nw>
    void
    _Base_bitset<_Nw>::_M_do_right_shift(size_t __shift) _GLIBCXX_NOEXCEPT
    {
      if (__builtin_expect(__shift != 0, 1))
	{
	  const size_t __wshift = __shift / _GLIBCXX_BITSET_BITS_PER_WORD;
	  const size_t __offset = __shift % _GLIBCXX_BITSET_BITS_PER_WORD;
	  const size_t __limit = _Nw - __wshift - 1;

	  if (__offset == 0)
	    for (size_t __n = 0; __n <= __limit; ++__n)
	      _M_w[__n] = _M_w[__n + __wshift];
	  else
	    {
	      const size_t __sub_offset = (_GLIBCXX_BITSET_BITS_PER_WORD
					   - __offset);
	      for (size_t __n = 0; __n < __limit; ++__n)
		_M_w[__n] = ((_M_w[__n + __wshift] >> __offset)
			     | (_M_w[__n + __wshift + 1] << __sub_offset));
	      _M_w[__limit] = _M_w[_Nw-1] >> __offset;
	    }
	  
	  std::fill(_M_w + __limit + 1, _M_w + _Nw, static_cast<_WordT>(0));
	}
    }

  template<size_t _Nw>
    unsigned long
    _Base_bitset<_Nw>::_M_do_to_ulong() const
    {
      for (size_t __i = 1; __i < _Nw; ++__i)
	if (_M_w[__i])
	  __throw_overflow_error(__N("_Base_bitset::_M_do_to_ulong"));
      return _M_w[0];
    }

#if __cplusplus >= 201103L
  template<size_t _Nw>
    unsigned long long
    _Base_bitset<_Nw>::_M_do_to_ullong() const
    {
      const bool __dw = sizeof(unsigned long long) > sizeof(unsigned long);
      for (size_t __i = 1 + __dw; __i < _Nw; ++__i)
	if (_M_w[__i])
	  __throw_overflow_error(__N("_Base_bitset::_M_do_to_ullong"));

      if (__dw)
	return _M_w[0] + (static_cast<unsigned long long>(_M_w[1])
			  << _GLIBCXX_BITSET_BITS_PER_WORD);
      return _M_w[0];
    }
#endif

  template<size_t _Nw>
    size_t
    _Base_bitset<_Nw>::
    _M_do_find_first(size_t __not_found) const _GLIBCXX_NOEXCEPT
    {
      for (size_t __i = 0; __i < _Nw; __i++)
	{
	  _WordT __thisword = _M_w[__i];
	  if (__thisword != static_cast<_WordT>(0))
	    return (__i * _GLIBCXX_BITSET_BITS_PER_WORD
		    + __builtin_ctzl(__thisword));
	}
      // not found, so return an indication of failure.
      return __not_found;
    }

  template<size_t _Nw>
    size_t
    _Base_bitset<_Nw>::
    _M_do_find_next(size_t __prev, size_t __not_found) const _GLIBCXX_NOEXCEPT
    {
      // make bound inclusive
      ++__prev;

      // check out of bounds
      if (__prev >= _Nw * _GLIBCXX_BITSET_BITS_PER_WORD)
	return __not_found;

      // search first word
      size_t __i = _S_whichword(__prev);
      _WordT __thisword = _M_w[__i];

      // mask off bits below bound
      __thisword &= (~static_cast<_WordT>(0)) << _S_whichbit(__prev);

      if (__thisword != static_cast<_WordT>(0))
	return (__i * _GLIBCXX_BITSET_BITS_PER_WORD
		+ __builtin_ctzl(__thisword));

      // check subsequent words
      __i++;
      for (; __i < _Nw; __i++)
	{
	  __thisword = _M_w[__i];
	  if (__thisword != static_cast<_WordT>(0))
	    return (__i * _GLIBCXX_BITSET_BITS_PER_WORD
		    + __builtin_ctzl(__thisword));
	}
      // not found, so return an indication of failure.
      return __not_found;
    } // end _M_do_find_next

  /**
   *  Base class, specialization for a single word.
   *
   *  See documentation for bitset.
  */
  template<>
    struct _Base_bitset<1>
    {
      typedef unsigned long _WordT;
      _WordT _M_w;

      _GLIBCXX_CONSTEXPR _Base_bitset() _GLIBCXX_NOEXCEPT
      : _M_w(0)
      { }

#if __cplusplus >= 201103L
      constexpr _Base_bitset(unsigned long long __val) noexcept
#else
      _Base_bitset(unsigned long __val)
#endif
      : _M_w(__val)
      { }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichword(size_t __pos) _GLIBCXX_NOEXCEPT
      { return __pos / _GLIBCXX_BITSET_BITS_PER_WORD; }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichbyte(size_t __pos) _GLIBCXX_NOEXCEPT
      { return (__pos % _GLIBCXX_BITSET_BITS_PER_WORD) / __CHAR_BIT__; }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichbit(size_t __pos) _GLIBCXX_NOEXCEPT
      {  return __pos % _GLIBCXX_BITSET_BITS_PER_WORD; }

      static _GLIBCXX_CONSTEXPR _WordT
      _S_maskbit(size_t __pos) _GLIBCXX_NOEXCEPT
      { return (static_cast<_WordT>(1)) << _S_whichbit(__pos); }

      _WordT&
      _M_getword(size_t) _GLIBCXX_NOEXCEPT
      { return _M_w; }

      _GLIBCXX_CONSTEXPR _WordT
      _M_getword(size_t) const _GLIBCXX_NOEXCEPT
      { return _M_w; }

#if __cplusplus >= 201103L
      const _WordT*
      _M_getdata() const noexcept
      { return &_M_w; }
#endif

      _WordT&
      _M_hiword() _GLIBCXX_NOEXCEPT
      { return _M_w; }

      _GLIBCXX_CONSTEXPR _WordT
      _M_hiword() const _GLIBCXX_NOEXCEPT
      { return _M_w; }

      void
      _M_do_and(const _Base_bitset<1>& __x) _GLIBCXX_NOEXCEPT
      { _M_w &= __x._M_w; }

      void
      _M_do_or(const _Base_bitset<1>& __x) _GLIBCXX_NOEXCEPT
      { _M_w |= __x._M_w; }

      void
      _M_do_xor(const _Base_bitset<1>& __x) _GLIBCXX_NOEXCEPT
      { _M_w ^= __x._M_w; }

      void
      _M_do_left_shift(size_t __shift) _GLIBCXX_NOEXCEPT
      { _M_w <<= __shift; }

      void
      _M_do_right_shift(size_t __shift) _GLIBCXX_NOEXCEPT
      { _M_w >>= __shift; }

      void
      _M_do_flip() _GLIBCXX_NOEXCEPT
      { _M_w = ~_M_w; }

      void
      _M_do_set() _GLIBCXX_NOEXCEPT
      { _M_w = ~static_cast<_WordT>(0); }

      void
      _M_do_reset() _GLIBCXX_NOEXCEPT
      { _M_w = 0; }

      bool
      _M_is_equal(const _Base_bitset<1>& __x) const _GLIBCXX_NOEXCEPT
      { return _M_w == __x._M_w; }

      template<size_t _Nb>
        bool
        _M_are_all() const _GLIBCXX_NOEXCEPT
        { return _M_w == (~static_cast<_WordT>(0)
			  >> (_GLIBCXX_BITSET_BITS_PER_WORD - _Nb)); }

      bool
      _M_is_any() const _GLIBCXX_NOEXCEPT
      { return _M_w != 0; }

      size_t
      _M_do_count() const _GLIBCXX_NOEXCEPT
      { return __builtin_popcountl(_M_w); }

      unsigned long
      _M_do_to_ulong() const _GLIBCXX_NOEXCEPT
      { return _M_w; }

#if __cplusplus >= 201103L
      unsigned long long
      _M_do_to_ullong() const noexcept
      { return _M_w; }
#endif

      size_t
      _M_do_find_first(size_t __not_found) const _GLIBCXX_NOEXCEPT
      {
        if (_M_w != 0)
          return __builtin_ctzl(_M_w);
        else
          return __not_found;
      }

      // find the next "on" bit that follows "prev"
      size_t
      _M_do_find_next(size_t __prev, size_t __not_found) const
	_GLIBCXX_NOEXCEPT
      {
	++__prev;
	if (__prev >= ((size_t) _GLIBCXX_BITSET_BITS_PER_WORD))
	  return __not_found;

	_WordT __x = _M_w >> __prev;
	if (__x != 0)
	  return __builtin_ctzl(__x) + __prev;
	else
	  return __not_found;
      }
    };

  /**
   *  Base class, specialization for no storage (zero-length %bitset).
   *
   *  See documentation for bitset.
  */
  template<>
    struct _Base_bitset<0>
    {
      typedef unsigned long _WordT;

      _GLIBCXX_CONSTEXPR _Base_bitset() _GLIBCXX_NOEXCEPT
      { }

#if __cplusplus >= 201103L
      constexpr _Base_bitset(unsigned long long) noexcept
#else
      _Base_bitset(unsigned long)
#endif
      { }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichword(size_t __pos) _GLIBCXX_NOEXCEPT
      { return __pos / _GLIBCXX_BITSET_BITS_PER_WORD; }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichbyte(size_t __pos) _GLIBCXX_NOEXCEPT
      { return (__pos % _GLIBCXX_BITSET_BITS_PER_WORD) / __CHAR_BIT__; }

      static _GLIBCXX_CONSTEXPR size_t
      _S_whichbit(size_t __pos) _GLIBCXX_NOEXCEPT
      {  return __pos % _GLIBCXX_BITSET_BITS_PER_WORD; }

      static _GLIBCXX_CONSTEXPR _WordT
      _S_maskbit(size_t __pos) _GLIBCXX_NOEXCEPT
      { return (static_cast<_WordT>(1)) << _S_whichbit(__pos); }

      // This would normally give access to the data.  The bounds-checking
      // in the bitset class will prevent the user from getting this far,
      // but (1) it must still return an lvalue to compile, and (2) the
      // user might call _Unchecked_set directly, in which case this /needs/
      // to fail.  Let's not penalize zero-length users unless they actually
      // make an unchecked call; all the memory ugliness is therefore
      // localized to this single should-never-get-this-far function.
      _WordT&
      _M_getword(size_t) _GLIBCXX_NOEXCEPT
      {
	__throw_out_of_range(__N("_Base_bitset::_M_getword")); 
	return *new _WordT;
      }

      _GLIBCXX_CONSTEXPR _WordT
      _M_getword(size_t __pos) const _GLIBCXX_NOEXCEPT
      { return 0; }

      _GLIBCXX_CONSTEXPR _WordT
      _M_hiword() const _GLIBCXX_NOEXCEPT
      { return 0; }

      void
      _M_do_and(const _Base_bitset<0>&) _GLIBCXX_NOEXCEPT
      { }

      void
      _M_do_or(const _Base_bitset<0>&) _GLIBCXX_NOEXCEPT
      { }

      void
      _M_do_xor(const _Base_bitset<0>&) _GLIBCXX_NOEXCEPT
      { }

      void
      _M_do_left_shift(size_t) _GLIBCXX_NOEXCEPT
      { }

      void
      _M_do_right_shift(size_t) _GLIBCXX_NOEXCEPT
      { }

      void
      _M_do_flip() _GLIBCXX_NOEXCEPT
      { }

      void
      _M_do_set() _GLIBCXX_NOEXCEPT
      { }

      void
      _M_do_reset() _GLIBCXX_NOEXCEPT
      { }

      // Are all empty bitsets equal to each other?  Are they equal to
      // themselves?  How to compare a thing which has no state?  What is
      // the sound of one zero-length bitset clapping?
      bool
      _M_is_equal(const _Base_bitset<0>&) const _GLIBCXX_NOEXCEPT
      { return true; }

      template<size_t _Nb>
        bool
        _M_are_all() const _GLIBCXX_NOEXCEPT
        { return true; }

      bool
      _M_is_any() const _GLIBCXX_NOEXCEPT
      { return false; }

      size_t
      _M_do_count() const _GLIBCXX_NOEXCEPT
      { return 0; }

      unsigned long
      _M_do_to_ulong() const _GLIBCXX_NOEXCEPT
      { return 0; }

#if __cplusplus >= 201103L
      unsigned long long
      _M_do_to_ullong() const noexcept
      { return 0; }
#endif

      // Normally "not found" is the size, but that could also be
      // misinterpreted as an index in this corner case.  Oh well.
      size_t
      _M_do_find_first(size_t) const _GLIBCXX_NOEXCEPT
      { return 0; }

      size_t
      _M_do_find_next(size_t, size_t) const _GLIBCXX_NOEXCEPT
      { return 0; }
    };


  // Helper class to zero out the unused high-order bits in the highest word.
  template<size_t _Extrabits>
    struct _Sanitize
    {
      typedef unsigned long _WordT;

      static void
      _S_do_sanitize(_WordT& __val) _GLIBCXX_NOEXCEPT
      { __val &= ~((~static_cast<_WordT>(0)) << _Extrabits); }
    };

  template<>
    struct _Sanitize<0>
    {
      typedef unsigned long _WordT;

      static void
      _S_do_sanitize(_WordT) _GLIBCXX_NOEXCEPT { } 
    };

#if __cplusplus >= 201103L
  template<size_t _Nb, bool = _Nb < _GLIBCXX_BITSET_BITS_PER_ULL>
    struct _Sanitize_val
    {
      static constexpr unsigned long long
      _S_do_sanitize_val(unsigned long long __val)
      { return __val; }
    };

  template<size_t _Nb>
    struct _Sanitize_val<_Nb, true>
    {
      static constexpr unsigned long long
      _S_do_sanitize_val(unsigned long long __val)
      { return __val & ~((~static_cast<unsigned long long>(0)) << _Nb); }
    };
#endif

  /**
   *  The %bitset class represents a @e fixed-size sequence of bits.
   *
   *  @ingroup containers
   *
   *  (Note that %bitset does @e not meet the formal requirements of a
   *  <a href="tables.html#65">container</a>.  Mainly, it lacks iterators.)
   *
   *  The template argument, @a Nb, may be any non-negative number,
   *  specifying the number of bits (e.g., "0", "12", "1024*1024").
   *
   *  In the general unoptimized case, storage is allocated in word-sized
   *  blocks.  Let B be the number of bits in a word, then (Nb+(B-1))/B
   *  words will be used for storage.  B - Nb%B bits are unused.  (They are
   *  the high-order bits in the highest word.)  It is a class invariant
   *  that those unused bits are always zero.
   *
   *  If you think of %bitset as <em>a simple array of bits</em>, be
   *  aware that your mental picture is reversed: a %bitset behaves
   *  the same way as bits in integers do, with the bit at index 0 in
   *  the <em>least significant / right-hand</em> position, and the bit at
   *  index Nb-1 in the <em>most significant / left-hand</em> position.
   *  Thus, unlike other containers, a %bitset's index <em>counts from
   *  right to left</em>, to put it very loosely.
   *
   *  This behavior is preserved when translating to and from strings.  For
   *  example, the first line of the following program probably prints
   *  <em>b(&apos;a&apos;) is 0001100001</em> on a modern ASCII system.
   *
   *  @code
   *     #include <bitset>
   *     #include <iostream>
   *     #include <sstream>
   *
   *     using namespace std;
   *
   *     int main()
   *     {
   *         long         a = 'a';
   *         bitset<10>   b(a);
   *
   *         cout << "b('a') is " << b << endl;
   *
   *         ostringstream s;
   *         s << b;
   *         string  str = s.str();
   *         cout << "index 3 in the string is " << str[3] << " but\n"
   *              << "index 3 in the bitset is " << b[3] << endl;
   *     }
   *  @endcode
   *
   *  Also see:
   *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt12ch33s02.html
   *  for a description of extensions.
   *
   *  Most of the actual code isn't contained in %bitset<> itself, but in the
   *  base class _Base_bitset.  The base class works with whole words, not with
   *  individual bits.  This allows us to specialize _Base_bitset for the
   *  important special case where the %bitset is only a single word.
   *
   *  Extra confusion can result due to the fact that the storage for
   *  _Base_bitset @e is a regular array, and is indexed as such.  This is
   *  carefully encapsulated.
  */
  template<size_t _Nb>
    class bitset
    : private _Base_bitset<_GLIBCXX_BITSET_WORDS(_Nb)>
    {
    private:
      typedef _Base_bitset<_GLIBCXX_BITSET_WORDS(_Nb)> _Base;
      typedef unsigned long _WordT;

      template<class _CharT, class _Traits, class _Alloc>
      void
      _M_check_initial_position(const std::basic_string<_CharT, _Traits, _Alloc>& __s,
				size_t __position) const
      {
	if (__position > __s.size())
	  __throw_out_of_range_fmt(__N("bitset::bitset: __position "
				       "(which is %zu) > __s.size() "
				       "(which is %zu)"),
				   __position, __s.size());
      }

      void _M_check(size_t __position, const char *__s) const
      {
	if (__position >= _Nb)
	  __throw_out_of_range_fmt(__N("%s: __position (which is %zu) "
				       ">= _Nb (which is %zu)"),
				   __s, __position, _Nb);
      }

      void
      _M_do_sanitize() _GLIBCXX_NOEXCEPT
      { 
	typedef _Sanitize<_Nb % _GLIBCXX_BITSET_BITS_PER_WORD> __sanitize_type;
	__sanitize_type::_S_do_sanitize(this->_M_hiword());
      }

#if __cplusplus >= 201103L
      template<typename> friend struct hash;
#endif

    public:
      /**
       *  This encapsulates the concept of a single bit.  An instance of this
       *  class is a proxy for an actual bit; this way the individual bit
       *  operations are done as faster word-size bitwise instructions.
       *
       *  Most users will never need to use this class directly; conversions
       *  to and from bool are automatic and should be transparent.  Overloaded
       *  operators help to preserve the illusion.
       *
       *  (On a typical system, this <em>bit %reference</em> is 64
       *  times the size of an actual bit.  Ha.)
       */
      class reference
      {
	friend class bitset;

	_WordT*	_M_wp;
	size_t 	_M_bpos;
	
	// left undefined
	reference();
	
      public:
	reference(bitset& __b, size_t __pos) _GLIBCXX_NOEXCEPT
	{
	  _M_wp = &__b._M_getword(__pos);
	  _M_bpos = _Base::_S_whichbit(__pos);
	}

	~reference() _GLIBCXX_NOEXCEPT
	{ }

	// For b[i] = __x;
	reference&
	operator=(bool __x) _GLIBCXX_NOEXCEPT
	{
	  if (__x)
	    *_M_wp |= _Base::_S_maskbit(_M_bpos);
	  else
	    *_M_wp &= ~_Base::_S_maskbit(_M_bpos);
	  return *this;
	}

	// For b[i] = b[__j];
	reference&
	operator=(const reference& __j) _GLIBCXX_NOEXCEPT
	{
	  if ((*(__j._M_wp) & _Base::_S_maskbit(__j._M_bpos)))
	    *_M_wp |= _Base::_S_maskbit(_M_bpos);
	  else
	    *_M_wp &= ~_Base::_S_maskbit(_M_bpos);
	  return *this;
	}

	// Flips the bit
	bool
	operator~() const _GLIBCXX_NOEXCEPT
	{ return (*(_M_wp) & _Base::_S_maskbit(_M_bpos)) == 0; }

	// For __x = b[i];
	operator bool() const _GLIBCXX_NOEXCEPT
	{ return (*(_M_wp) & _Base::_S_maskbit(_M_bpos)) != 0; }

	// For b[i].flip();
	reference&
	flip() _GLIBCXX_NOEXCEPT
	{
	  *_M_wp ^= _Base::_S_maskbit(_M_bpos);
	  return *this;
	}
      };
      friend class reference;

      // 23.3.5.1 constructors:
      /// All bits set to zero.
      _GLIBCXX_CONSTEXPR bitset() _GLIBCXX_NOEXCEPT
      { }

      /// Initial bits bitwise-copied from a single word (others set to zero).
#if __cplusplus >= 201103L
      constexpr bitset(unsigned long long __val) noexcept
      : _Base(_Sanitize_val<_Nb>::_S_do_sanitize_val(__val)) { }
#else
      bitset(unsigned long __val)
      : _Base(__val)
      { _M_do_sanitize(); }
#endif

      /**
       *  Use a subset of a string.
       *  @param  __s  A string of @a 0 and @a 1 characters.
       *  @param  __position  Index of the first character in @a __s to use;
       *                    defaults to zero.
       *  @throw  std::out_of_range  If @a pos is bigger the size of @a __s.
       *  @throw  std::invalid_argument  If a character appears in the string
       *                                 which is neither @a 0 nor @a 1.
       */
      template<class _CharT, class _Traits, class _Alloc>
	explicit
	bitset(const std::basic_string<_CharT, _Traits, _Alloc>& __s,
	       size_t __position = 0)
	: _Base()
	{
	  _M_check_initial_position(__s, __position);
	  _M_copy_from_string(__s, __position,
			      std::basic_string<_CharT, _Traits, _Alloc>::npos,
			      _CharT('0'), _CharT('1'));
	}

      /**
       *  Use a subset of a string.
       *  @param  __s  A string of @a 0 and @a 1 characters.
       *  @param  __position  Index of the first character in @a __s to use.
       *  @param  __n    The number of characters to copy.
       *  @throw std::out_of_range If @a __position is bigger the size
       *  of @a __s.
       *  @throw  std::invalid_argument  If a character appears in the string
       *                                 which is neither @a 0 nor @a 1.
       */
      template<class _CharT, class _Traits, class _Alloc>
	bitset(const std::basic_string<_CharT, _Traits, _Alloc>& __s,
	       size_t __position, size_t __n)
	: _Base()
	{
	  _M_check_initial_position(__s, __position);
	  _M_copy_from_string(__s, __position, __n, _CharT('0'), _CharT('1'));
	}

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 396. what are characters zero and one.
      template<class _CharT, class _Traits, class _Alloc>
	bitset(const std::basic_string<_CharT, _Traits, _Alloc>& __s,
	       size_t __position, size_t __n,
	       _CharT __zero, _CharT __one = _CharT('1'))
	: _Base()
	{
	  _M_check_initial_position(__s, __position);
	  _M_copy_from_string(__s, __position, __n, __zero, __one);
	}

#if __cplusplus >= 201103L
      /**
       *  Construct from a character %array.
       *  @param  __str  An %array of characters @a zero and @a one.
       *  @param  __n    The number of characters to use.
       *  @param  __zero The character corresponding to the value 0.
       *  @param  __one  The character corresponding to the value 1.
       *  @throw  std::invalid_argument If a character appears in the string
       *                                which is neither @a __zero nor @a __one.
       */
      template<typename _CharT>
        explicit
        bitset(const _CharT* __str,
	       typename std::basic_string<_CharT>::size_type __n
	       = std::basic_string<_CharT>::npos,
	       _CharT __zero = _CharT('0'), _CharT __one = _CharT('1'))
        : _Base()
        {
	  if (!__str)
	    __throw_logic_error(__N("bitset::bitset(const _CharT*, ...)"));

	  if (__n == std::basic_string<_CharT>::npos)
	    __n = std::char_traits<_CharT>::length(__str);
	  _M_copy_from_ptr<_CharT, std::char_traits<_CharT>>(__str, __n, 0,
							     __n, __zero,
							     __one);
	}
#endif

      // 23.3.5.2 bitset operations:
      //@{
      /**
       *  Operations on bitsets.
       *  @param  __rhs  A same-sized bitset.
       *
       *  These should be self-explanatory.
       */
      bitset<_Nb>&
      operator&=(const bitset<_Nb>& __rhs) _GLIBCXX_NOEXCEPT
      {
	this->_M_do_and(__rhs);
	return *this;
      }

      bitset<_Nb>&
      operator|=(const bitset<_Nb>& __rhs) _GLIBCXX_NOEXCEPT
      {
	this->_M_do_or(__rhs);
	return *this;
      }

      bitset<_Nb>&
      operator^=(const bitset<_Nb>& __rhs) _GLIBCXX_NOEXCEPT
      {
	this->_M_do_xor(__rhs);
	return *this;
      }
      //@}
      
      //@{
      /**
       *  Operations on bitsets.
       *  @param  __position  The number of places to shift.
       *
       *  These should be self-explanatory.
       */
      bitset<_Nb>&
      operator<<=(size_t __position) _GLIBCXX_NOEXCEPT
      {
	if (__builtin_expect(__position < _Nb, 1))
	  {
	    this->_M_do_left_shift(__position);
	    this->_M_do_sanitize();
	  }
	else
	  this->_M_do_reset();
	return *this;
      }

      bitset<_Nb>&
      operator>>=(size_t __position) _GLIBCXX_NOEXCEPT
      {
	if (__builtin_expect(__position < _Nb, 1))
	  {
	    this->_M_do_right_shift(__position);
	    this->_M_do_sanitize();
	  }
	else
	  this->_M_do_reset();
	return *this;
      }
      //@}
      
      //@{
      /**
       *  These versions of single-bit set, reset, flip, and test are
       *  extensions from the SGI version.  They do no range checking.
       *  @ingroup SGIextensions
       */
      bitset<_Nb>&
      _Unchecked_set(size_t __pos) _GLIBCXX_NOEXCEPT
      {
	this->_M_getword(__pos) |= _Base::_S_maskbit(__pos);
	return *this;
      }

      bitset<_Nb>&
      _Unchecked_set(size_t __pos, int __val) _GLIBCXX_NOEXCEPT
      {
	if (__val)
	  this->_M_getword(__pos) |= _Base::_S_maskbit(__pos);
	else
	  this->_M_getword(__pos) &= ~_Base::_S_maskbit(__pos);
	return *this;
      }

      bitset<_Nb>&
      _Unchecked_reset(size_t __pos) _GLIBCXX_NOEXCEPT
      {
	this->_M_getword(__pos) &= ~_Base::_S_maskbit(__pos);
	return *this;
      }

      bitset<_Nb>&
      _Unchecked_flip(size_t __pos) _GLIBCXX_NOEXCEPT
      {
	this->_M_getword(__pos) ^= _Base::_S_maskbit(__pos);
	return *this;
      }

      _GLIBCXX_CONSTEXPR bool
      _Unchecked_test(size_t __pos) const _GLIBCXX_NOEXCEPT
      { return ((this->_M_getword(__pos) & _Base::_S_maskbit(__pos))
		!= static_cast<_WordT>(0)); }
      //@}
      
      // Set, reset, and flip.
      /**
       *  @brief Sets every bit to true.
       */
      bitset<_Nb>&
      set() _GLIBCXX_NOEXCEPT
      {
	this->_M_do_set();
	this->_M_do_sanitize();
	return *this;
      }

      /**
       *  @brief Sets a given bit to a particular value.
       *  @param  __position  The index of the bit.
       *  @param  __val  Either true or false, defaults to true.
       *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
       */
      bitset<_Nb>&
      set(size_t __position, bool __val = true)
      {
	this->_M_check(__position, __N("bitset::set"));
	return _Unchecked_set(__position, __val);
      }

      /**
       *  @brief Sets every bit to false.
       */
      bitset<_Nb>&
      reset() _GLIBCXX_NOEXCEPT
      {
	this->_M_do_reset();
	return *this;
      }

      /**
       *  @brief Sets a given bit to false.
       *  @param  __position  The index of the bit.
       *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
       *
       *  Same as writing @c set(pos,false).
       */
      bitset<_Nb>&
      reset(size_t __position)
      {
	this->_M_check(__position, __N("bitset::reset"));
	return _Unchecked_reset(__position);
      }
      
      /**
       *  @brief Toggles every bit to its opposite value.
       */
      bitset<_Nb>&
      flip() _GLIBCXX_NOEXCEPT
      {
	this->_M_do_flip();
	this->_M_do_sanitize();
	return *this;
      }

      /**
       *  @brief Toggles a given bit to its opposite value.
       *  @param  __position  The index of the bit.
       *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
       */
      bitset<_Nb>&
      flip(size_t __position)
      {
	this->_M_check(__position, __N("bitset::flip"));
	return _Unchecked_flip(__position);
      }
      
      /// See the no-argument flip().
      bitset<_Nb>
      operator~() const _GLIBCXX_NOEXCEPT
      { return bitset<_Nb>(*this).flip(); }

      //@{
      /**
       *  @brief  Array-indexing support.
       *  @param  __position  Index into the %bitset.
       *  @return A bool for a <em>const %bitset</em>.  For non-const
       *           bitsets, an instance of the reference proxy class.
       *  @note  These operators do no range checking and throw no exceptions,
       *         as required by DR 11 to the standard.
       *
       *  _GLIBCXX_RESOLVE_LIB_DEFECTS Note that this implementation already
       *  resolves DR 11 (items 1 and 2), but does not do the range-checking
       *  required by that DR's resolution.  -pme
       *  The DR has since been changed:  range-checking is a precondition
       *  (users' responsibility), and these functions must not throw.  -pme
       */
      reference
      operator[](size_t __position)
      { return reference(*this, __position); }

      _GLIBCXX_CONSTEXPR bool
      operator[](size_t __position) const
      { return _Unchecked_test(__position); }
      //@}
      
      /**
       *  @brief Returns a numerical interpretation of the %bitset.
       *  @return  The integral equivalent of the bits.
       *  @throw  std::overflow_error  If there are too many bits to be
       *                               represented in an @c unsigned @c long.
       */
      unsigned long
      to_ulong() const
      { return this->_M_do_to_ulong(); }

#if __cplusplus >= 201103L
      unsigned long long
      to_ullong() const
      { return this->_M_do_to_ullong(); }
#endif

      /**
       *  @brief Returns a character interpretation of the %bitset.
       *  @return  The string equivalent of the bits.
       *
       *  Note the ordering of the bits:  decreasing character positions
       *  correspond to increasing bit positions (see the main class notes for
       *  an example).
       */
      template<class _CharT, class _Traits, class _Alloc>
	std::basic_string<_CharT, _Traits, _Alloc>
	to_string() const
	{
	  std::basic_string<_CharT, _Traits, _Alloc> __result;
	  _M_copy_to_string(__result, _CharT('0'), _CharT('1'));
	  return __result;
	}

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 396. what are characters zero and one.
      template<class _CharT, class _Traits, class _Alloc>
	std::basic_string<_CharT, _Traits, _Alloc>
	to_string(_CharT __zero, _CharT __one = _CharT('1')) const
	{
	  std::basic_string<_CharT, _Traits, _Alloc> __result;
	  _M_copy_to_string(__result, __zero, __one);
	  return __result;
	}

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 434. bitset::to_string() hard to use.
      template<class _CharT, class _Traits>
	std::basic_string<_CharT, _Traits, std::allocator<_CharT> >
	to_string() const
	{ return to_string<_CharT, _Traits, std::allocator<_CharT> >(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 853. to_string needs updating with zero and one.
      template<class _CharT, class _Traits>
	std::basic_string<_CharT, _Traits, std::allocator<_CharT> >
	to_string(_CharT __zero, _CharT __one = _CharT('1')) const
	{ return to_string<_CharT, _Traits,
	                   std::allocator<_CharT> >(__zero, __one); }

      template<class _CharT>
	std::basic_string<_CharT, std::char_traits<_CharT>,
	                  std::allocator<_CharT> >
	to_string() const
	{
	  return to_string<_CharT, std::char_traits<_CharT>,
	                   std::allocator<_CharT> >();
	}

      template<class _CharT>
	std::basic_string<_CharT, std::char_traits<_CharT>,
	                  std::allocator<_CharT> >
	to_string(_CharT __zero, _CharT __one = _CharT('1')) const
	{
	  return to_string<_CharT, std::char_traits<_CharT>,
	                   std::allocator<_CharT> >(__zero, __one);
	}

      std::basic_string<char, std::char_traits<char>, std::allocator<char> >
      to_string() const
      {
	return to_string<char, std::char_traits<char>,
	                 std::allocator<char> >();
      }

      std::basic_string<char, std::char_traits<char>, std::allocator<char> >
      to_string(char __zero, char __one = '1') const
      {
	return to_string<char, std::char_traits<char>,
	                 std::allocator<char> >(__zero, __one);
      }

      // Helper functions for string operations.
      template<class _CharT, class _Traits>
        void
        _M_copy_from_ptr(const _CharT*, size_t, size_t, size_t,
			 _CharT, _CharT);

      template<class _CharT, class _Traits, class _Alloc>
	void
	_M_copy_from_string(const std::basic_string<_CharT,
			    _Traits, _Alloc>& __s, size_t __pos, size_t __n,
			    _CharT __zero, _CharT __one)
	{ _M_copy_from_ptr<_CharT, _Traits>(__s.data(), __s.size(), __pos, __n,
					    __zero, __one); }

      template<class _CharT, class _Traits, class _Alloc>
	void
        _M_copy_to_string(std::basic_string<_CharT, _Traits, _Alloc>&,
			  _CharT, _CharT) const;

      // NB: Backward compat.
      template<class _CharT, class _Traits, class _Alloc>
	void
	_M_copy_from_string(const std::basic_string<_CharT,
			    _Traits, _Alloc>& __s, size_t __pos, size_t __n)
	{ _M_copy_from_string(__s, __pos, __n, _CharT('0'), _CharT('1')); }

      template<class _CharT, class _Traits, class _Alloc>
	void
        _M_copy_to_string(std::basic_string<_CharT, _Traits,_Alloc>& __s) const
	{ _M_copy_to_string(__s, _CharT('0'), _CharT('1')); }

      /// Returns the number of bits which are set.
      size_t
      count() const _GLIBCXX_NOEXCEPT
      { return this->_M_do_count(); }

      /// Returns the total number of bits.
      _GLIBCXX_CONSTEXPR size_t
      size() const _GLIBCXX_NOEXCEPT
      { return _Nb; }

      //@{
      /// These comparisons for equality/inequality are, well, @e bitwise.
      bool
      operator==(const bitset<_Nb>& __rhs) const _GLIBCXX_NOEXCEPT
      { return this->_M_is_equal(__rhs); }

      bool
      operator!=(const bitset<_Nb>& __rhs) const _GLIBCXX_NOEXCEPT
      { return !this->_M_is_equal(__rhs); }
      //@}
      
      /**
       *  @brief Tests the value of a bit.
       *  @param  __position  The index of a bit.
       *  @return  The value at @a pos.
       *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
       */
      bool
      test(size_t __position) const
      {
	this->_M_check(__position, __N("bitset::test"));
	return _Unchecked_test(__position);
      }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 693. std::bitset::all() missing.
      /**
       *  @brief Tests whether all the bits are on.
       *  @return  True if all the bits are set.
       */
      bool
      all() const _GLIBCXX_NOEXCEPT
      { return this->template _M_are_all<_Nb>(); }

      /**
       *  @brief Tests whether any of the bits are on.
       *  @return  True if at least one bit is set.
       */
      bool
      any() const _GLIBCXX_NOEXCEPT
      { return this->_M_is_any(); }

      /**
       *  @brief Tests whether any of the bits are on.
       *  @return  True if none of the bits are set.
       */
      bool
      none() const _GLIBCXX_NOEXCEPT
      { return !this->_M_is_any(); }

      //@{
      /// Self-explanatory.
      bitset<_Nb>
      operator<<(size_t __position) const _GLIBCXX_NOEXCEPT
      { return bitset<_Nb>(*this) <<= __position; }

      bitset<_Nb>
      operator>>(size_t __position) const _GLIBCXX_NOEXCEPT
      { return bitset<_Nb>(*this) >>= __position; }
      //@}
      
      /**
       *  @brief  Finds the index of the first "on" bit.
       *  @return  The index of the first bit set, or size() if not found.
       *  @ingroup SGIextensions
       *  @sa  _Find_next
       */
      size_t
      _Find_first() const _GLIBCXX_NOEXCEPT
      { return this->_M_do_find_first(_Nb); }

      /**
       *  @brief  Finds the index of the next "on" bit after prev.
       *  @return  The index of the next bit set, or size() if not found.
       *  @param  __prev  Where to start searching.
       *  @ingroup SGIextensions
       *  @sa  _Find_first
       */
      size_t
      _Find_next(size_t __prev) const _GLIBCXX_NOEXCEPT
      { return this->_M_do_find_next(__prev, _Nb); }
    };

  // Definitions of non-inline member functions.
  template<size_t _Nb>
    template<class _CharT, class _Traits>
      void
      bitset<_Nb>::
      _M_copy_from_ptr(const _CharT* __s, size_t __len,
		       size_t __pos, size_t __n, _CharT __zero, _CharT __one)
      {
	reset();
	const size_t __nbits = std::min(_Nb, std::min(__n, size_t(__len - __pos)));
	for (size_t __i = __nbits; __i > 0; --__i)
	  {
	    const _CharT __c = __s[__pos + __nbits - __i];
	    if (_Traits::eq(__c, __zero))
	      ;
	    else if (_Traits::eq(__c, __one))
	      _Unchecked_set(__i - 1);
	    else
	      __throw_invalid_argument(__N("bitset::_M_copy_from_ptr"));
	  }
      }

  template<size_t _Nb>
    template<class _CharT, class _Traits, class _Alloc>
      void
      bitset<_Nb>::
      _M_copy_to_string(std::basic_string<_CharT, _Traits, _Alloc>& __s,
			_CharT __zero, _CharT __one) const
      {
	__s.assign(_Nb, __zero);
	for (size_t __i = _Nb; __i > 0; --__i)
	  if (_Unchecked_test(__i - 1))
	    _Traits::assign(__s[_Nb - __i], __one);
      }

  // 23.3.5.3 bitset operations:
  //@{
  /**
   *  @brief  Global bitwise operations on bitsets.
   *  @param  __x  A bitset.
   *  @param  __y  A bitset of the same size as @a __x.
   *  @return  A new bitset.
   *
   *  These should be self-explanatory.
  */
  template<size_t _Nb>
    inline bitset<_Nb>
    operator&(const bitset<_Nb>& __x, const bitset<_Nb>& __y) _GLIBCXX_NOEXCEPT
    {
      bitset<_Nb> __result(__x);
      __result &= __y;
      return __result;
    }

  template<size_t _Nb>
    inline bitset<_Nb>
    operator|(const bitset<_Nb>& __x, const bitset<_Nb>& __y) _GLIBCXX_NOEXCEPT
    {
      bitset<_Nb> __result(__x);
      __result |= __y;
      return __result;
    }

  template <size_t _Nb>
    inline bitset<_Nb>
    operator^(const bitset<_Nb>& __x, const bitset<_Nb>& __y) _GLIBCXX_NOEXCEPT
    {
      bitset<_Nb> __result(__x);
      __result ^= __y;
      return __result;
    }
  //@}

  //@{
  /**
   *  @brief Global I/O operators for bitsets.
   *
   *  Direct I/O between streams and bitsets is supported.  Output is
   *  straightforward.  Input will skip whitespace, only accept @a 0 and @a 1
   *  characters, and will only extract as many digits as the %bitset will
   *  hold.
  */
  template<class _CharT, class _Traits, size_t _Nb>
    std::basic_istream<_CharT, _Traits>&
    operator>>(std::basic_istream<_CharT, _Traits>& __is, bitset<_Nb>& __x)
    {
      typedef typename _Traits::char_type          char_type;
      typedef std::basic_istream<_CharT, _Traits>  __istream_type;
      typedef typename __istream_type::ios_base    __ios_base;

      std::basic_string<_CharT, _Traits> __tmp;
      __tmp.reserve(_Nb);

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 303. Bitset input operator underspecified
      const char_type __zero = __is.widen('0');
      const char_type __one = __is.widen('1');

      typename __ios_base::iostate __state = __ios_base::goodbit;
      typename __istream_type::sentry __sentry(__is);
      if (__sentry)
	{
	  __try
	    {
	      for (size_t __i = _Nb; __i > 0; --__i)
		{
		  static typename _Traits::int_type __eof = _Traits::eof();
		  
		  typename _Traits::int_type __c1 = __is.rdbuf()->sbumpc();
		  if (_Traits::eq_int_type(__c1, __eof))
		    {
		      __state |= __ios_base::eofbit;
		      break;
		    }
		  else
		    {
		      const char_type __c2 = _Traits::to_char_type(__c1);
		      if (_Traits::eq(__c2, __zero))
			__tmp.push_back(__zero);
		      else if (_Traits::eq(__c2, __one))
			__tmp.push_back(__one);
		      else if (_Traits::
			       eq_int_type(__is.rdbuf()->sputbackc(__c2),
					   __eof))
			{
			  __state |= __ios_base::failbit;
			  break;
			}
		    }
		}
	    }
	  __catch(__cxxabiv1::__forced_unwind&)
	    {
	      __is._M_setstate(__ios_base::badbit);		
	      __throw_exception_again;
	    }
	  __catch(...)
	    { __is._M_setstate(__ios_base::badbit); }
	}

      if (__tmp.empty() && _Nb)
	__state |= __ios_base::failbit;
      else
	__x._M_copy_from_string(__tmp, static_cast<size_t>(0), _Nb,
				__zero, __one);
      if (__state)
	__is.setstate(__state);
      return __is;
    }

  template <class _CharT, class _Traits, size_t _Nb>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
	       const bitset<_Nb>& __x)
    {
      std::basic_string<_CharT, _Traits> __tmp;

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 396. what are characters zero and one.
      const ctype<_CharT>& __ct = use_facet<ctype<_CharT> >(__os.getloc());
      __x._M_copy_to_string(__tmp, __ct.widen('0'), __ct.widen('1'));
      return __os << __tmp;
    }
  //@}

_GLIBCXX_END_NAMESPACE_CONTAINER
} // namespace std

#undef _GLIBCXX_BITSET_WORDS
#undef _GLIBCXX_BITSET_BITS_PER_WORD
#undef _GLIBCXX_BITSET_BITS_PER_ULL

#if __cplusplus >= 201103L

#include <bits/functional_hash.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // DR 1182.
  /// std::hash specialization for bitset.
  template<size_t _Nb>
    struct hash<_GLIBCXX_STD_C::bitset<_Nb>>
    : public __hash_base<size_t, _GLIBCXX_STD_C::bitset<_Nb>>
    {
      size_t
      operator()(const _GLIBCXX_STD_C::bitset<_Nb>& __b) const noexcept
      {
	const size_t __clength = (_Nb + __CHAR_BIT__ - 1) / __CHAR_BIT__;
	return std::_Hash_impl::hash(__b._M_getdata(), __clength);
      }
    };

  template<>
    struct hash<_GLIBCXX_STD_C::bitset<0>>
    : public __hash_base<size_t, _GLIBCXX_STD_C::bitset<0>>
    {
      size_t
      operator()(const _GLIBCXX_STD_C::bitset<0>&) const noexcept
      { return 0; }
    };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif // C++11

#ifdef _GLIBCXX_DEBUG
# include <debug/bitset>
#endif

#ifdef _GLIBCXX_PROFILE
# include <profile/bitset>
#endif

#endif /* _GLIBCXX_BITSET */

// -*- C++ -*- forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/cctype
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c ctype.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: <ccytpe>
//

#pragma GCC system_header

#include <bits/c++config.h>
#include <ctype.h>

#ifndef _GLIBCXX_CCTYPE
#define _GLIBCXX_CCTYPE 1

// Get rid of those macros defined in <ctype.h> in lieu of real functions.
#undef isalnum
#undef isalpha
#undef iscntrl
#undef isdigit
#undef isgraph
#undef islower
#undef isprint
#undef ispunct
#undef isspace
#undef isupper
#undef isxdigit
#undef tolower
#undef toupper

namespace std
{
  using ::isalnum;
  using ::isalpha;
  using ::iscntrl;
  using ::isdigit;
  using ::isgraph;
  using ::islower;
  using ::isprint;
  using ::ispunct;
  using ::isspace;
  using ::isupper;
  using ::isxdigit;
  using ::tolower;
  using ::toupper;
} // namespace std

#if __cplusplus >= 201103L

#ifdef _GLIBCXX_USE_C99_CTYPE_TR1

#undef isblank

namespace std
{
  using ::isblank;
} // namespace std

#endif // _GLIBCXX_USE_C99_CTYPE_TR1

#endif // C++11

#endif

// The -*- C++ -*- forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file cerrno
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c errno.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: 19.3  Error numbers
//

#pragma GCC system_header

#include <errno.h>

#ifndef _GLIBCXX_CERRNO
#define _GLIBCXX_CERRNO 1

// Adhere to section 17.4.1.2 clause 5 of ISO 14882:1998
#ifndef errno
#define errno errno
#endif

#endif

// -*- C++ -*- forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file clocale
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c locale.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: 18.2.2  Implementation properties: C library
//

#pragma GCC system_header

#include <bits/c++config.h>
#include <locale.h>

#ifndef _GLIBCXX_CLOCALE
#define _GLIBCXX_CLOCALE 1

// Get rid of those macros defined in <locale.h> in lieu of real functions.
#undef setlocale
#undef localeconv

namespace std
{
  using ::lconv;
  using ::setlocale;
  using ::localeconv;
} // namespace std

#endif

// -*- C++ -*- C forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/cmath
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c math.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: 26.5  C library
//

#pragma GCC system_header

#include <bits/c++config.h>
#include <bits/cpp_type_traits.h>
#include <ext/type_traits.h>
#include <math.h>

#ifndef _GLIBCXX_CMATH
#define _GLIBCXX_CMATH 1

// Get rid of those macros defined in <math.h> in lieu of real functions.
#undef abs
#undef div
#undef acos
#undef asin
#undef atan
#undef atan2
#undef ceil
#undef cos
#undef cosh
#undef exp
#undef fabs
#undef floor
#undef fmod
#undef frexp
#undef ldexp
#undef log
#undef log10
#undef modf
#undef pow
#undef sin
#undef sinh
#undef sqrt
#undef tan
#undef tanh

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR double
  abs(double __x)
  { return __builtin_fabs(__x); }
#endif

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  abs(float __x)
  { return __builtin_fabsf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  abs(long double __x)
  { return __builtin_fabsl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    abs(_Tp __x)
    { return __builtin_fabs(__x); }

  using ::acos;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  acos(float __x)
  { return __builtin_acosf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  acos(long double __x)
  { return __builtin_acosl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    acos(_Tp __x)
    { return __builtin_acos(__x); }

  using ::asin;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  asin(float __x)
  { return __builtin_asinf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  asin(long double __x)
  { return __builtin_asinl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    asin(_Tp __x)
    { return __builtin_asin(__x); }

  using ::atan;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  atan(float __x)
  { return __builtin_atanf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  atan(long double __x)
  { return __builtin_atanl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    atan(_Tp __x)
    { return __builtin_atan(__x); }

  using ::atan2;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  atan2(float __y, float __x)
  { return __builtin_atan2f(__y, __x); }

  inline _GLIBCXX_CONSTEXPR long double
  atan2(long double __y, long double __x)
  { return __builtin_atan2l(__y, __x); }
#endif

  template<typename _Tp, typename _Up>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    atan2(_Tp __y, _Up __x)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return atan2(__type(__y), __type(__x));
    }

  using ::ceil;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  ceil(float __x)
  { return __builtin_ceilf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  ceil(long double __x)
  { return __builtin_ceill(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    ceil(_Tp __x)
    { return __builtin_ceil(__x); }

  using ::cos;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  cos(float __x)
  { return __builtin_cosf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  cos(long double __x)
  { return __builtin_cosl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    cos(_Tp __x)
    { return __builtin_cos(__x); }

  using ::cosh;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  cosh(float __x)
  { return __builtin_coshf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  cosh(long double __x)
  { return __builtin_coshl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    cosh(_Tp __x)
    { return __builtin_cosh(__x); }

  using ::exp;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  exp(float __x)
  { return __builtin_expf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  exp(long double __x)
  { return __builtin_expl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    exp(_Tp __x)
    { return __builtin_exp(__x); }

  using ::fabs;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  fabs(float __x)
  { return __builtin_fabsf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  fabs(long double __x)
  { return __builtin_fabsl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    fabs(_Tp __x)
    { return __builtin_fabs(__x); }

  using ::floor;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  floor(float __x)
  { return __builtin_floorf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  floor(long double __x)
  { return __builtin_floorl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    floor(_Tp __x)
    { return __builtin_floor(__x); }

  using ::fmod;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  fmod(float __x, float __y)
  { return __builtin_fmodf(__x, __y); }

  inline _GLIBCXX_CONSTEXPR long double
  fmod(long double __x, long double __y)
  { return __builtin_fmodl(__x, __y); }
#endif

  template<typename _Tp, typename _Up>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    fmod(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return fmod(__type(__x), __type(__y));
    }

  using ::frexp;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline float
  frexp(float __x, int* __exp)
  { return __builtin_frexpf(__x, __exp); }

  inline long double
  frexp(long double __x, int* __exp)
  { return __builtin_frexpl(__x, __exp); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    frexp(_Tp __x, int* __exp)
    { return __builtin_frexp(__x, __exp); }

  using ::ldexp;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  ldexp(float __x, int __exp)
  { return __builtin_ldexpf(__x, __exp); }

  inline _GLIBCXX_CONSTEXPR long double
  ldexp(long double __x, int __exp)
  { return __builtin_ldexpl(__x, __exp); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    ldexp(_Tp __x, int __exp)
    { return __builtin_ldexp(__x, __exp); }

  using ::log;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  log(float __x)
  { return __builtin_logf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  log(long double __x)
  { return __builtin_logl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    log(_Tp __x)
    { return __builtin_log(__x); }

  using ::log10;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  log10(float __x)
  { return __builtin_log10f(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  log10(long double __x)
  { return __builtin_log10l(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                    double>::__type
    log10(_Tp __x)
    { return __builtin_log10(__x); }

  using ::modf;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline float
  modf(float __x, float* __iptr)
  { return __builtin_modff(__x, __iptr); }

  inline long double
  modf(long double __x, long double* __iptr)
  { return __builtin_modfl(__x, __iptr); }
#endif

  using ::pow;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  pow(float __x, float __y)
  { return __builtin_powf(__x, __y); }

  inline _GLIBCXX_CONSTEXPR long double
  pow(long double __x, long double __y)
  { return __builtin_powl(__x, __y); }

#if __cplusplus < 201103L
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // DR 550. What should the return type of pow(float,int) be?
  inline double
  pow(double __x, int __i)
  { return __builtin_powi(__x, __i); }

  inline float
  pow(float __x, int __n)
  { return __builtin_powif(__x, __n); }

  inline long double
  pow(long double __x, int __n)
  { return __builtin_powil(__x, __n); }
#endif
#endif

  template<typename _Tp, typename _Up>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    pow(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return pow(__type(__x), __type(__y));
    }

  using ::sin;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  sin(float __x)
  { return __builtin_sinf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  sin(long double __x)
  { return __builtin_sinl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    sin(_Tp __x)
    { return __builtin_sin(__x); }

  using ::sinh;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  sinh(float __x)
  { return __builtin_sinhf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  sinh(long double __x)
  { return __builtin_sinhl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    sinh(_Tp __x)
    { return __builtin_sinh(__x); }

  using ::sqrt;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  sqrt(float __x)
  { return __builtin_sqrtf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  sqrt(long double __x)
  { return __builtin_sqrtl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    sqrt(_Tp __x)
    { return __builtin_sqrt(__x); }

  using ::tan;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  tan(float __x)
  { return __builtin_tanf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  tan(long double __x)
  { return __builtin_tanl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    tan(_Tp __x)
    { return __builtin_tan(__x); }

  using ::tanh;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
  inline _GLIBCXX_CONSTEXPR float
  tanh(float __x)
  { return __builtin_tanhf(__x); }

  inline _GLIBCXX_CONSTEXPR long double
  tanh(long double __x)
  { return __builtin_tanhl(__x); }
#endif

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    double>::__type
    tanh(_Tp __x)
    { return __builtin_tanh(__x); }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#if _GLIBCXX_USE_C99_MATH
#if !_GLIBCXX_USE_C99_FP_MACROS_DYNAMIC

// These are possible macros imported from C99-land.
#undef fpclassify
#undef isfinite
#undef isinf
#undef isnan
#undef isnormal
#undef signbit
#undef isgreater
#undef isgreaterequal
#undef isless
#undef islessequal
#undef islessgreater
#undef isunordered

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#if __cplusplus >= 201103L
  constexpr int
  fpclassify(float __x)
  { return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL,
				FP_SUBNORMAL, FP_ZERO, __x); }

  constexpr int
  fpclassify(double __x)
  { return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL,
				FP_SUBNORMAL, FP_ZERO, __x); }

  constexpr int
  fpclassify(long double __x)
  { return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL,
				FP_SUBNORMAL, FP_ZERO, __x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                              int>::__type
    fpclassify(_Tp __x)
    { return __x != 0 ? FP_NORMAL : FP_ZERO; }

  constexpr bool
  isfinite(float __x)
  { return __builtin_isfinite(__x); }

  constexpr bool
  isfinite(double __x)
  { return __builtin_isfinite(__x); }

  constexpr bool
  isfinite(long double __x)
  { return __builtin_isfinite(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                              bool>::__type
    isfinite(_Tp __x)
    { return true; }

  constexpr bool
  isinf(float __x)
  { return __builtin_isinf(__x); }

  constexpr bool
  isinf(double __x)
  { return __builtin_isinf(__x); }

  constexpr bool
  isinf(long double __x)
  { return __builtin_isinf(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                              bool>::__type
    isinf(_Tp __x)
    { return false; }

  constexpr bool
  isnan(float __x)
  { return __builtin_isnan(__x); }

  constexpr bool
  isnan(double __x)
  { return __builtin_isnan(__x); }

  constexpr bool
  isnan(long double __x)
  { return __builtin_isnan(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                              bool>::__type
    isnan(_Tp __x)
    { return false; }

  constexpr bool
  isnormal(float __x)
  { return __builtin_isnormal(__x); }

  constexpr bool
  isnormal(double __x)
  { return __builtin_isnormal(__x); }

  constexpr bool
  isnormal(long double __x)
  { return __builtin_isnormal(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                              bool>::__type
    isnormal(_Tp __x)
    { return __x != 0 ? true : false; }

  // The front-end doesn't provide a type generic builtin (libstdc++/58625).
  constexpr bool
  signbit(float __x)
  { return __builtin_signbitf(__x); }

  constexpr bool
  signbit(double __x)
  { return __builtin_signbit(__x); }

  constexpr bool
  signbit(long double __x)
  { return __builtin_signbitl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value,
                                              bool>::__type
    signbit(_Tp __x)
    { return __x < 0 ? true : false; }

  constexpr bool
  isgreater(float __x, float __y)
  { return __builtin_isgreater(__x, __y); }

  constexpr bool
  isgreater(double __x, double __y)
  { return __builtin_isgreater(__x, __y); }

  constexpr bool
  isgreater(long double __x, long double __y)
  { return __builtin_isgreater(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename
    __gnu_cxx::__enable_if<(__is_arithmetic<_Tp>::__value
			    && __is_arithmetic<_Up>::__value), bool>::__type
    isgreater(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return __builtin_isgreater(__type(__x), __type(__y));
    }

  constexpr bool
  isgreaterequal(float __x, float __y)
  { return __builtin_isgreaterequal(__x, __y); }

  constexpr bool
  isgreaterequal(double __x, double __y)
  { return __builtin_isgreaterequal(__x, __y); }

  constexpr bool
  isgreaterequal(long double __x, long double __y)
  { return __builtin_isgreaterequal(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename
    __gnu_cxx::__enable_if<(__is_arithmetic<_Tp>::__value
			    && __is_arithmetic<_Up>::__value), bool>::__type
    isgreaterequal(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return __builtin_isgreaterequal(__type(__x), __type(__y));
    }

  constexpr bool
  isless(float __x, float __y)
  { return __builtin_isless(__x, __y); }

  constexpr bool
  isless(double __x, double __y)
  { return __builtin_isless(__x, __y); }

  constexpr bool
  isless(long double __x, long double __y)
  { return __builtin_isless(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename
    __gnu_cxx::__enable_if<(__is_arithmetic<_Tp>::__value
			    && __is_arithmetic<_Up>::__value), bool>::__type
    isless(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return __builtin_isless(__type(__x), __type(__y));
    }

  constexpr bool
  islessequal(float __x, float __y)
  { return __builtin_islessequal(__x, __y); }

  constexpr bool
  islessequal(double __x, double __y)
  { return __builtin_islessequal(__x, __y); }

  constexpr bool
  islessequal(long double __x, long double __y)
  { return __builtin_islessequal(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename
    __gnu_cxx::__enable_if<(__is_arithmetic<_Tp>::__value
			    && __is_arithmetic<_Up>::__value), bool>::__type
    islessequal(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return __builtin_islessequal(__type(__x), __type(__y));
    }

  constexpr bool
  islessgreater(float __x, float __y)
  { return __builtin_islessgreater(__x, __y); }

  constexpr bool
  islessgreater(double __x, double __y)
  { return __builtin_islessgreater(__x, __y); }

  constexpr bool
  islessgreater(long double __x, long double __y)
  { return __builtin_islessgreater(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename
    __gnu_cxx::__enable_if<(__is_arithmetic<_Tp>::__value
			    && __is_arithmetic<_Up>::__value), bool>::__type
    islessgreater(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return __builtin_islessgreater(__type(__x), __type(__y));
    }

  constexpr bool
  isunordered(float __x, float __y)
  { return __builtin_isunordered(__x, __y); }

  constexpr bool
  isunordered(double __x, double __y)
  { return __builtin_isunordered(__x, __y); }

  constexpr bool
  isunordered(long double __x, long double __y)
  { return __builtin_isunordered(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename
    __gnu_cxx::__enable_if<(__is_arithmetic<_Tp>::__value
			    && __is_arithmetic<_Up>::__value), bool>::__type
    isunordered(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return __builtin_isunordered(__type(__x), __type(__y));
    }

#else

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    fpclassify(_Tp __f)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL,
				  FP_SUBNORMAL, FP_ZERO, __type(__f));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isfinite(_Tp __f)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isfinite(__type(__f));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isinf(_Tp __f)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isinf(__type(__f));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isnan(_Tp __f)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isnan(__type(__f));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isnormal(_Tp __f)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isnormal(__type(__f));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    signbit(_Tp __f)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_signbit(__type(__f));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isgreater(_Tp __f1, _Tp __f2)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isgreater(__type(__f1), __type(__f2));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isgreaterequal(_Tp __f1, _Tp __f2)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isgreaterequal(__type(__f1), __type(__f2));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isless(_Tp __f1, _Tp __f2)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isless(__type(__f1), __type(__f2));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    islessequal(_Tp __f1, _Tp __f2)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_islessequal(__type(__f1), __type(__f2));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    islessgreater(_Tp __f1, _Tp __f2)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_islessgreater(__type(__f1), __type(__f2));
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__enable_if<__is_arithmetic<_Tp>::__value,
					   int>::__type
    isunordered(_Tp __f1, _Tp __f2)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __builtin_isunordered(__type(__f1), __type(__f2));
    }

#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif /* _GLIBCXX_USE_C99_FP_MACROS_DYNAMIC */
#endif

#if __cplusplus >= 201103L

#ifdef _GLIBCXX_USE_C99_MATH_TR1

#undef acosh
#undef acoshf
#undef acoshl
#undef asinh
#undef asinhf
#undef asinhl
#undef atanh
#undef atanhf
#undef atanhl
#undef cbrt
#undef cbrtf
#undef cbrtl
#undef copysign
#undef copysignf
#undef copysignl
#undef erf
#undef erff
#undef erfl
#undef erfc
#undef erfcf
#undef erfcl
#undef exp2
#undef exp2f
#undef exp2l
#undef expm1
#undef expm1f
#undef expm1l
#undef fdim
#undef fdimf
#undef fdiml
#undef fma
#undef fmaf
#undef fmal
#undef fmax
#undef fmaxf
#undef fmaxl
#undef fmin
#undef fminf
#undef fminl
#undef hypot
#undef hypotf
#undef hypotl
#undef ilogb
#undef ilogbf
#undef ilogbl
#undef lgamma
#undef lgammaf
#undef lgammal
#undef llrint
#undef llrintf
#undef llrintl
#undef llround
#undef llroundf
#undef llroundl
#undef log1p
#undef log1pf
#undef log1pl
#undef log2
#undef log2f
#undef log2l
#undef logb
#undef logbf
#undef logbl
#undef lrint
#undef lrintf
#undef lrintl
#undef lround
#undef lroundf
#undef lroundl
#undef nan
#undef nanf
#undef nanl
#undef nearbyint
#undef nearbyintf
#undef nearbyintl
#undef nextafter
#undef nextafterf
#undef nextafterl
#undef nexttoward
#undef nexttowardf
#undef nexttowardl
#undef remainder
#undef remainderf
#undef remainderl
#undef remquo
#undef remquof
#undef remquol
#undef rint
#undef rintf
#undef rintl
#undef round
#undef roundf
#undef roundl
#undef scalbln
#undef scalblnf
#undef scalblnl
#undef scalbn
#undef scalbnf
#undef scalbnl
#undef tgamma
#undef tgammaf
#undef tgammal
#undef trunc
#undef truncf
#undef truncl

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // types
  using ::double_t;
  using ::float_t;

  // functions
  using ::acosh;
  using ::acoshf;
  using ::acoshl;

  using ::asinh;
  using ::asinhf;
  using ::asinhl;

  using ::atanh;
  using ::atanhf;
  using ::atanhl;

  using ::cbrt;
  using ::cbrtf;
  using ::cbrtl;

  using ::copysign;
  using ::copysignf;
  using ::copysignl;

  using ::erf;
  using ::erff;
  using ::erfl;

  using ::erfc;
  using ::erfcf;
  using ::erfcl;

  using ::exp2;
  using ::exp2f;
  using ::exp2l;

  using ::expm1;
  using ::expm1f;
  using ::expm1l;

  using ::fdim;
  using ::fdimf;
  using ::fdiml;

  using ::fma;
  using ::fmaf;
  using ::fmal;

  using ::fmax;
  using ::fmaxf;
  using ::fmaxl;

  using ::fmin;
  using ::fminf;
  using ::fminl;

  using ::hypot;
  using ::hypotf;
  using ::hypotl;

  using ::ilogb;
  using ::ilogbf;
  using ::ilogbl;

  using ::lgamma;
  using ::lgammaf;
  using ::lgammal;

  using ::llrint;
  using ::llrintf;
  using ::llrintl;

  using ::llround;
  using ::llroundf;
  using ::llroundl;

  using ::log1p;
  using ::log1pf;
  using ::log1pl;

  using ::log2;
  using ::log2f;
  using ::log2l;

  using ::logb;
  using ::logbf;
  using ::logbl;

  using ::lrint;
  using ::lrintf;
  using ::lrintl;

  using ::lround;
  using ::lroundf;
  using ::lroundl;

  using ::nan;
  using ::nanf;
  using ::nanl;

  using ::nearbyint;
  using ::nearbyintf;
  using ::nearbyintl;

  using ::nextafter;
  using ::nextafterf;
  using ::nextafterl;

  using ::nexttoward;
  using ::nexttowardf;
  using ::nexttowardl;

  using ::remainder;
  using ::remainderf;
  using ::remainderl;

  using ::remquo;
  using ::remquof;
  using ::remquol;

  using ::rint;
  using ::rintf;
  using ::rintl;

  using ::round;
  using ::roundf;
  using ::roundl;

  using ::scalbln;
  using ::scalblnf;
  using ::scalblnl;

  using ::scalbn;
  using ::scalbnf;
  using ::scalbnl;

  using ::tgamma;
  using ::tgammaf;
  using ::tgammal;

  using ::trunc;
  using ::truncf;
  using ::truncl;

  /// Additional overloads.
  constexpr float
  acosh(float __x)
  { return __builtin_acoshf(__x); }

  constexpr long double
  acosh(long double __x)
  { return __builtin_acoshl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    acosh(_Tp __x)
    { return __builtin_acosh(__x); }

  constexpr float
  asinh(float __x)
  { return __builtin_asinhf(__x); }

  constexpr long double
  asinh(long double __x)
  { return __builtin_asinhl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    asinh(_Tp __x)
    { return __builtin_asinh(__x); }

  constexpr float
  atanh(float __x)
  { return __builtin_atanhf(__x); }

  constexpr long double
  atanh(long double __x)
  { return __builtin_atanhl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    atanh(_Tp __x)
    { return __builtin_atanh(__x); }

  constexpr float
  cbrt(float __x)
  { return __builtin_cbrtf(__x); }

  constexpr long double
  cbrt(long double __x)
  { return __builtin_cbrtl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    cbrt(_Tp __x)
    { return __builtin_cbrt(__x); }

  constexpr float
  copysign(float __x, float __y)
  { return __builtin_copysignf(__x, __y); }

  constexpr long double
  copysign(long double __x, long double __y)
  { return __builtin_copysignl(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    copysign(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return copysign(__type(__x), __type(__y));
    }

  constexpr float
  erf(float __x)
  { return __builtin_erff(__x); }

  constexpr long double
  erf(long double __x)
  { return __builtin_erfl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    erf(_Tp __x)
    { return __builtin_erf(__x); }

  constexpr float
  erfc(float __x)
  { return __builtin_erfcf(__x); }

  constexpr long double
  erfc(long double __x)
  { return __builtin_erfcl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    erfc(_Tp __x)
    { return __builtin_erfc(__x); }

  constexpr float
  exp2(float __x)
  { return __builtin_exp2f(__x); }

  constexpr long double
  exp2(long double __x)
  { return __builtin_exp2l(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    exp2(_Tp __x)
    { return __builtin_exp2(__x); }

  constexpr float
  expm1(float __x)
  { return __builtin_expm1f(__x); }

  constexpr long double
  expm1(long double __x)
  { return __builtin_expm1l(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    expm1(_Tp __x)
    { return __builtin_expm1(__x); }

  constexpr float
  fdim(float __x, float __y)
  { return __builtin_fdimf(__x, __y); }

  constexpr long double
  fdim(long double __x, long double __y)
  { return __builtin_fdiml(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    fdim(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return fdim(__type(__x), __type(__y));
    }

  constexpr float
  fma(float __x, float __y, float __z)
  { return __builtin_fmaf(__x, __y, __z); }

  constexpr long double
  fma(long double __x, long double __y, long double __z)
  { return __builtin_fmal(__x, __y, __z); }

  template<typename _Tp, typename _Up, typename _Vp>
    constexpr typename __gnu_cxx::__promote_3<_Tp, _Up, _Vp>::__type
    fma(_Tp __x, _Up __y, _Vp __z)
    {
      typedef typename __gnu_cxx::__promote_3<_Tp, _Up, _Vp>::__type __type;
      return fma(__type(__x), __type(__y), __type(__z));
    }

  constexpr float
  fmax(float __x, float __y)
  { return __builtin_fmaxf(__x, __y); }

  constexpr long double
  fmax(long double __x, long double __y)
  { return __builtin_fmaxl(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    fmax(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return fmax(__type(__x), __type(__y));
    }

  constexpr float
  fmin(float __x, float __y)
  { return __builtin_fminf(__x, __y); }

  constexpr long double
  fmin(long double __x, long double __y)
  { return __builtin_fminl(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    fmin(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return fmin(__type(__x), __type(__y));
    }

  constexpr float
  hypot(float __x, float __y)
  { return __builtin_hypotf(__x, __y); }

  constexpr long double
  hypot(long double __x, long double __y)
  { return __builtin_hypotl(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    hypot(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return hypot(__type(__x), __type(__y));
    }

  constexpr int
  ilogb(float __x)
  { return __builtin_ilogbf(__x); }

  constexpr int
  ilogb(long double __x)
  { return __builtin_ilogbl(__x); }

  template<typename _Tp>
    constexpr
    typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                    int>::__type
    ilogb(_Tp __x)
    { return __builtin_ilogb(__x); }

  constexpr float
  lgamma(float __x)
  { return __builtin_lgammaf(__x); }

  constexpr long double
  lgamma(long double __x)
  { return __builtin_lgammal(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    lgamma(_Tp __x)
    { return __builtin_lgamma(__x); }

  constexpr long long
  llrint(float __x)
  { return __builtin_llrintf(__x); }

  constexpr long long
  llrint(long double __x)
  { return __builtin_llrintl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              long long>::__type
    llrint(_Tp __x)
    { return __builtin_llrint(__x); }

  constexpr long long
  llround(float __x)
  { return __builtin_llroundf(__x); }

  constexpr long long
  llround(long double __x)
  { return __builtin_llroundl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              long long>::__type
    llround(_Tp __x)
    { return __builtin_llround(__x); }

  constexpr float
  log1p(float __x)
  { return __builtin_log1pf(__x); }

  constexpr long double
  log1p(long double __x)
  { return __builtin_log1pl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    log1p(_Tp __x)
    { return __builtin_log1p(__x); }

  // DR 568.
  constexpr float
  log2(float __x)
  { return __builtin_log2f(__x); }

  constexpr long double
  log2(long double __x)
  { return __builtin_log2l(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    log2(_Tp __x)
    { return __builtin_log2(__x); }

  constexpr float
  logb(float __x)
  { return __builtin_logbf(__x); }

  constexpr long double
  logb(long double __x)
  { return __builtin_logbl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    logb(_Tp __x)
    { return __builtin_logb(__x); }

  constexpr long
  lrint(float __x)
  { return __builtin_lrintf(__x); }

  constexpr long
  lrint(long double __x)
  { return __builtin_lrintl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              long>::__type
    lrint(_Tp __x)
    { return __builtin_lrint(__x); }

  constexpr long
  lround(float __x)
  { return __builtin_lroundf(__x); }

  constexpr long
  lround(long double __x)
  { return __builtin_lroundl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              long>::__type
    lround(_Tp __x)
    { return __builtin_lround(__x); }

  constexpr float
  nearbyint(float __x)
  { return __builtin_nearbyintf(__x); }

  constexpr long double
  nearbyint(long double __x)
  { return __builtin_nearbyintl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    nearbyint(_Tp __x)
    { return __builtin_nearbyint(__x); }

  constexpr float
  nextafter(float __x, float __y)
  { return __builtin_nextafterf(__x, __y); }

  constexpr long double
  nextafter(long double __x, long double __y)
  { return __builtin_nextafterl(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    nextafter(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return nextafter(__type(__x), __type(__y));
    }

  constexpr float
  nexttoward(float __x, long double __y)
  { return __builtin_nexttowardf(__x, __y); }

  constexpr long double
  nexttoward(long double __x, long double __y)
  { return __builtin_nexttowardl(__x, __y); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    nexttoward(_Tp __x, long double __y)
    { return __builtin_nexttoward(__x, __y); }

  constexpr float
  remainder(float __x, float __y)
  { return __builtin_remainderf(__x, __y); }

  constexpr long double
  remainder(long double __x, long double __y)
  { return __builtin_remainderl(__x, __y); }

  template<typename _Tp, typename _Up>
    constexpr typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    remainder(_Tp __x, _Up __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return remainder(__type(__x), __type(__y));
    }

  inline float
  remquo(float __x, float __y, int* __pquo)
  { return __builtin_remquof(__x, __y, __pquo); }

  inline long double
  remquo(long double __x, long double __y, int* __pquo)
  { return __builtin_remquol(__x, __y, __pquo); }

  template<typename _Tp, typename _Up>
    inline typename __gnu_cxx::__promote_2<_Tp, _Up>::__type
    remquo(_Tp __x, _Up __y, int* __pquo)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return remquo(__type(__x), __type(__y), __pquo);
    }

  constexpr float
  rint(float __x)
  { return __builtin_rintf(__x); }

  constexpr long double
  rint(long double __x)
  { return __builtin_rintl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    rint(_Tp __x)
    { return __builtin_rint(__x); }

  constexpr float
  round(float __x)
  { return __builtin_roundf(__x); }

  constexpr long double
  round(long double __x)
  { return __builtin_roundl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    round(_Tp __x)
    { return __builtin_round(__x); }

  constexpr float
  scalbln(float __x, long __ex)
  { return __builtin_scalblnf(__x, __ex); }

  constexpr long double
  scalbln(long double __x, long __ex)
  { return __builtin_scalblnl(__x, __ex); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    scalbln(_Tp __x, long __ex)
    { return __builtin_scalbln(__x, __ex); }
 
  constexpr float
  scalbn(float __x, int __ex)
  { return __builtin_scalbnf(__x, __ex); }

  constexpr long double
  scalbn(long double __x, int __ex)
  { return __builtin_scalbnl(__x, __ex); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    scalbn(_Tp __x, int __ex)
    { return __builtin_scalbn(__x, __ex); }

  constexpr float
  tgamma(float __x)
  { return __builtin_tgammaf(__x); }

  constexpr long double
  tgamma(long double __x)
  { return __builtin_tgammal(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    tgamma(_Tp __x)
    { return __builtin_tgamma(__x); }
 
  constexpr float
  trunc(float __x)
  { return __builtin_truncf(__x); }

  constexpr long double
  trunc(long double __x)
  { return __builtin_truncl(__x); }

  template<typename _Tp>
    constexpr typename __gnu_cxx::__enable_if<__is_integer<_Tp>::__value, 
                                              double>::__type
    trunc(_Tp __x)
    { return __builtin_trunc(__x); }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif // _GLIBCXX_USE_C99_MATH_TR1

#endif // C++11

#endif

// The template and inlines for the -*- C++ -*- complex number classes.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/complex
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 26.2  Complex Numbers
// Note: this is not a conforming implementation.
// Initially implemented by Ulrich Drepper <drepper@cygnus.com>
// Improved by Gabriel Dos Reis <dosreis@cmla.ens-cachan.fr>
//

#ifndef _GLIBCXX_COMPLEX
#define _GLIBCXX_COMPLEX 1

#pragma GCC system_header

#include <bits/c++config.h>
#include <bits/cpp_type_traits.h>
#include <ext/type_traits.h>
#include <cmath>
#include <sstream>

// Get rid of a macro possibly defined in <complex.h>
#undef complex

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @defgroup complex_numbers Complex Numbers
   * @ingroup numerics
   *
   * Classes and functions for complex numbers.
   * @{
   */

  // Forward declarations.
  template<typename _Tp> class complex;
  template<> class complex<float>;
  template<> class complex<double>;
  template<> class complex<long double>;

  ///  Return magnitude of @a z.
  template<typename _Tp> _Tp abs(const complex<_Tp>&);
  ///  Return phase angle of @a z.
  template<typename _Tp> _Tp arg(const complex<_Tp>&);
  ///  Return @a z magnitude squared.
  template<typename _Tp> _Tp norm(const complex<_Tp>&);

  ///  Return complex conjugate of @a z.
  template<typename _Tp> complex<_Tp> conj(const complex<_Tp>&);
  ///  Return complex with magnitude @a rho and angle @a theta.
  template<typename _Tp> complex<_Tp> polar(const _Tp&, const _Tp& = 0);

  // Transcendentals:
  /// Return complex cosine of @a z.
  template<typename _Tp> complex<_Tp> cos(const complex<_Tp>&);
  /// Return complex hyperbolic cosine of @a z.
  template<typename _Tp> complex<_Tp> cosh(const complex<_Tp>&);
  /// Return complex base e exponential of @a z.
  template<typename _Tp> complex<_Tp> exp(const complex<_Tp>&);
  /// Return complex natural logarithm of @a z.
  template<typename _Tp> complex<_Tp> log(const complex<_Tp>&);
  /// Return complex base 10 logarithm of @a z.
  template<typename _Tp> complex<_Tp> log10(const complex<_Tp>&);
  /// Return @a x to the @a y'th power.
  template<typename _Tp> complex<_Tp> pow(const complex<_Tp>&, int);
  /// Return @a x to the @a y'th power.
  template<typename _Tp> complex<_Tp> pow(const complex<_Tp>&, const _Tp&);
  /// Return @a x to the @a y'th power.
  template<typename _Tp> complex<_Tp> pow(const complex<_Tp>&, 
                                          const complex<_Tp>&);
  /// Return @a x to the @a y'th power.
  template<typename _Tp> complex<_Tp> pow(const _Tp&, const complex<_Tp>&);
  /// Return complex sine of @a z.
  template<typename _Tp> complex<_Tp> sin(const complex<_Tp>&);
  /// Return complex hyperbolic sine of @a z.
  template<typename _Tp> complex<_Tp> sinh(const complex<_Tp>&);
  /// Return complex square root of @a z.
  template<typename _Tp> complex<_Tp> sqrt(const complex<_Tp>&);
  /// Return complex tangent of @a z.
  template<typename _Tp> complex<_Tp> tan(const complex<_Tp>&);
  /// Return complex hyperbolic tangent of @a z.
  template<typename _Tp> complex<_Tp> tanh(const complex<_Tp>&);
    
    
  // 26.2.2  Primary template class complex
  /**
   *  Template to represent complex numbers.
   *
   *  Specializations for float, double, and long double are part of the
   *  library.  Results with any other type are not guaranteed.
   *
   *  @param  Tp  Type of real and imaginary values.
  */
  template<typename _Tp>
    struct complex
    {
      /// Value typedef.
      typedef _Tp value_type;
      
      ///  Default constructor.  First parameter is x, second parameter is y.
      ///  Unspecified parameters default to 0.
      _GLIBCXX_CONSTEXPR complex(const _Tp& __r = _Tp(), const _Tp& __i = _Tp())
      : _M_real(__r), _M_imag(__i) { }

      // Lets the compiler synthesize the copy constructor   
      // complex (const complex<_Tp>&);
      ///  Copy constructor.
      template<typename _Up>
        _GLIBCXX_CONSTEXPR complex(const complex<_Up>& __z)
	: _M_real(__z.real()), _M_imag(__z.imag()) { }

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      _GLIBCXX_ABI_TAG_CXX11
      constexpr _Tp 
      real() { return _M_real; }

      _GLIBCXX_ABI_TAG_CXX11
      constexpr _Tp 
      imag() { return _M_imag; }
#else
      ///  Return real part of complex number.
      _Tp& 
      real() { return _M_real; }

      ///  Return real part of complex number.
      const _Tp& 
      real() const { return _M_real; }

      ///  Return imaginary part of complex number.
      _Tp& 
      imag() { return _M_imag; }

      ///  Return imaginary part of complex number.
      const _Tp& 
      imag() const { return _M_imag; }
#endif

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      void 
      real(_Tp __val) { _M_real = __val; }

      void 
      imag(_Tp __val) { _M_imag = __val; }

      /// Assign this complex number to scalar @a t.
      complex<_Tp>& operator=(const _Tp&);
      
      /// Add @a t to this complex number.
      // 26.2.5/1
      complex<_Tp>&
      operator+=(const _Tp& __t)
      {
	_M_real += __t;
	return *this;
      }

      /// Subtract @a t from this complex number.
      // 26.2.5/3
      complex<_Tp>&
      operator-=(const _Tp& __t)
      {
	_M_real -= __t;
	return *this;
      }

      /// Multiply this complex number by @a t.
      complex<_Tp>& operator*=(const _Tp&);
      /// Divide this complex number by @a t.
      complex<_Tp>& operator/=(const _Tp&);

      // Lets the compiler synthesize the
      // copy and assignment operator
      // complex<_Tp>& operator= (const complex<_Tp>&);
      /// Assign this complex number to complex @a z.
      template<typename _Up>
        complex<_Tp>& operator=(const complex<_Up>&);
      /// Add @a z to this complex number.
      template<typename _Up>
        complex<_Tp>& operator+=(const complex<_Up>&);
      /// Subtract @a z from this complex number.
      template<typename _Up>
        complex<_Tp>& operator-=(const complex<_Up>&);
      /// Multiply this complex number by @a z.
      template<typename _Up>
        complex<_Tp>& operator*=(const complex<_Up>&);
      /// Divide this complex number by @a z.
      template<typename _Up>
        complex<_Tp>& operator/=(const complex<_Up>&);

      _GLIBCXX_USE_CONSTEXPR complex __rep() const
      { return *this; }

    private:
      _Tp _M_real;
      _Tp _M_imag;
    };

  template<typename _Tp>
    complex<_Tp>&
    complex<_Tp>::operator=(const _Tp& __t)
    {
     _M_real = __t;
     _M_imag = _Tp();
     return *this;
    } 

  // 26.2.5/5
  template<typename _Tp>
    complex<_Tp>&
    complex<_Tp>::operator*=(const _Tp& __t)
    {
      _M_real *= __t;
      _M_imag *= __t;
      return *this;
    }

  // 26.2.5/7
  template<typename _Tp>
    complex<_Tp>&
    complex<_Tp>::operator/=(const _Tp& __t)
    {
      _M_real /= __t;
      _M_imag /= __t;
      return *this;
    }

  template<typename _Tp>
    template<typename _Up>
    complex<_Tp>&
    complex<_Tp>::operator=(const complex<_Up>& __z)
    {
      _M_real = __z.real();
      _M_imag = __z.imag();
      return *this;
    }

  // 26.2.5/9
  template<typename _Tp>
    template<typename _Up>
    complex<_Tp>&
    complex<_Tp>::operator+=(const complex<_Up>& __z)
    {
      _M_real += __z.real();
      _M_imag += __z.imag();
      return *this;
    }

  // 26.2.5/11
  template<typename _Tp>
    template<typename _Up>
    complex<_Tp>&
    complex<_Tp>::operator-=(const complex<_Up>& __z)
    {
      _M_real -= __z.real();
      _M_imag -= __z.imag();
      return *this;
    }

  // 26.2.5/13
  // XXX: This is a grammar school implementation.
  template<typename _Tp>
    template<typename _Up>
    complex<_Tp>&
    complex<_Tp>::operator*=(const complex<_Up>& __z)
    {
      const _Tp __r = _M_real * __z.real() - _M_imag * __z.imag();
      _M_imag = _M_real * __z.imag() + _M_imag * __z.real();
      _M_real = __r;
      return *this;
    }

  // 26.2.5/15
  // XXX: This is a grammar school implementation.
  template<typename _Tp>
    template<typename _Up>
    complex<_Tp>&
    complex<_Tp>::operator/=(const complex<_Up>& __z)
    {
      const _Tp __r =  _M_real * __z.real() + _M_imag * __z.imag();
      const _Tp __n = std::norm(__z);
      _M_imag = (_M_imag * __z.real() - _M_real * __z.imag()) / __n;
      _M_real = __r / __n;
      return *this;
    }
    
  // Operators:
  //@{
  ///  Return new complex value @a x plus @a y.
  template<typename _Tp>
    inline complex<_Tp>
    operator+(const complex<_Tp>& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r = __x;
      __r += __y;
      return __r;
    }

  template<typename _Tp>
    inline complex<_Tp>
    operator+(const complex<_Tp>& __x, const _Tp& __y)
    {
      complex<_Tp> __r = __x;
      __r += __y;
      return __r;
    }

  template<typename _Tp>
    inline complex<_Tp>
    operator+(const _Tp& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r = __y;
      __r += __x;
      return __r;
    }
  //@}

  //@{
  ///  Return new complex value @a x minus @a y.
  template<typename _Tp>
    inline complex<_Tp>
    operator-(const complex<_Tp>& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r = __x;
      __r -= __y;
      return __r;
    }
    
  template<typename _Tp>
    inline complex<_Tp>
    operator-(const complex<_Tp>& __x, const _Tp& __y)
    {
      complex<_Tp> __r = __x;
      __r -= __y;
      return __r;
    }

  template<typename _Tp>
    inline complex<_Tp>
    operator-(const _Tp& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r(__x, -__y.imag());
      __r -= __y.real();
      return __r;
    }
  //@}

  //@{
  ///  Return new complex value @a x times @a y.
  template<typename _Tp>
    inline complex<_Tp>
    operator*(const complex<_Tp>& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r = __x;
      __r *= __y;
      return __r;
    }

  template<typename _Tp>
    inline complex<_Tp>
    operator*(const complex<_Tp>& __x, const _Tp& __y)
    {
      complex<_Tp> __r = __x;
      __r *= __y;
      return __r;
    }

  template<typename _Tp>
    inline complex<_Tp>
    operator*(const _Tp& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r = __y;
      __r *= __x;
      return __r;
    }
  //@}

  //@{
  ///  Return new complex value @a x divided by @a y.
  template<typename _Tp>
    inline complex<_Tp>
    operator/(const complex<_Tp>& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r = __x;
      __r /= __y;
      return __r;
    }
    
  template<typename _Tp>
    inline complex<_Tp>
    operator/(const complex<_Tp>& __x, const _Tp& __y)
    {
      complex<_Tp> __r = __x;
      __r /= __y;
      return __r;
    }

  template<typename _Tp>
    inline complex<_Tp>
    operator/(const _Tp& __x, const complex<_Tp>& __y)
    {
      complex<_Tp> __r = __x;
      __r /= __y;
      return __r;
    }
  //@}

  ///  Return @a x.
  template<typename _Tp>
    inline complex<_Tp>
    operator+(const complex<_Tp>& __x)
    { return __x; }

  ///  Return complex negation of @a x.
  template<typename _Tp>
    inline complex<_Tp>
    operator-(const complex<_Tp>& __x)
    {  return complex<_Tp>(-__x.real(), -__x.imag()); }

  //@{
  ///  Return true if @a x is equal to @a y.
  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR bool
    operator==(const complex<_Tp>& __x, const complex<_Tp>& __y)
    { return __x.real() == __y.real() && __x.imag() == __y.imag(); }

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR bool
    operator==(const complex<_Tp>& __x, const _Tp& __y)
    { return __x.real() == __y && __x.imag() == _Tp(); }

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR bool
    operator==(const _Tp& __x, const complex<_Tp>& __y)
    { return __x == __y.real() && _Tp() == __y.imag(); }
  //@}

  //@{
  ///  Return false if @a x is equal to @a y.
  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR bool
    operator!=(const complex<_Tp>& __x, const complex<_Tp>& __y)
    { return __x.real() != __y.real() || __x.imag() != __y.imag(); }

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR bool
    operator!=(const complex<_Tp>& __x, const _Tp& __y)
    { return __x.real() != __y || __x.imag() != _Tp(); }

  template<typename _Tp>
    inline _GLIBCXX_CONSTEXPR bool
    operator!=(const _Tp& __x, const complex<_Tp>& __y)
    { return __x != __y.real() || _Tp() != __y.imag(); }
  //@}

  ///  Extraction operator for complex values.
  template<typename _Tp, typename _CharT, class _Traits>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __is, complex<_Tp>& __x)
    {
      _Tp __re_x, __im_x;
      _CharT __ch;
      __is >> __ch;
      if (__ch == '(') 
	{
	  __is >> __re_x >> __ch;
	  if (__ch == ',') 
	    {
	      __is >> __im_x >> __ch;
	      if (__ch == ')') 
		__x = complex<_Tp>(__re_x, __im_x);
	      else
		__is.setstate(ios_base::failbit);
	    }
	  else if (__ch == ')') 
	    __x = __re_x;
	  else
	    __is.setstate(ios_base::failbit);
	}
      else 
	{
	  __is.putback(__ch);
	  __is >> __re_x;
	  __x = __re_x;
	}
      return __is;
    }

  ///  Insertion operator for complex values.
  template<typename _Tp, typename _CharT, class _Traits>
    basic_ostream<_CharT, _Traits>&
    operator<<(basic_ostream<_CharT, _Traits>& __os, const complex<_Tp>& __x)
    {
      basic_ostringstream<_CharT, _Traits> __s;
      __s.flags(__os.flags());
      __s.imbue(__os.getloc());
      __s.precision(__os.precision());
      __s << '(' << __x.real() << ',' << __x.imag() << ')';
      return __os << __s.str();
    }

  // Values
#if __cplusplus >= 201103L
  template<typename _Tp>
    constexpr _Tp
    real(const complex<_Tp>& __z)
    { return __z.real(); }

  template<typename _Tp>
    constexpr _Tp
    imag(const complex<_Tp>& __z)
    { return __z.imag(); }
#else
  template<typename _Tp>
    inline _Tp&
    real(complex<_Tp>& __z)
    { return __z.real(); }
    
  template<typename _Tp>
    inline const _Tp&
    real(const complex<_Tp>& __z)
    { return __z.real(); }
    
  template<typename _Tp>
    inline _Tp&
    imag(complex<_Tp>& __z)
    { return __z.imag(); }
    
  template<typename _Tp>
    inline const _Tp&
    imag(const complex<_Tp>& __z)
    { return __z.imag(); }
#endif

  // 26.2.7/3 abs(__z):  Returns the magnitude of __z.
  template<typename _Tp>
    inline _Tp
    __complex_abs(const complex<_Tp>& __z)
    {
      _Tp __x = __z.real();
      _Tp __y = __z.imag();
      const _Tp __s = std::max(abs(__x), abs(__y));
      if (__s == _Tp())  // well ...
        return __s;
      __x /= __s; 
      __y /= __s;
      return __s * sqrt(__x * __x + __y * __y);
    }

#if _GLIBCXX_USE_C99_COMPLEX
  inline float
  __complex_abs(__complex__ float __z) { return __builtin_cabsf(__z); }

  inline double
  __complex_abs(__complex__ double __z) { return __builtin_cabs(__z); }

  inline long double
  __complex_abs(const __complex__ long double& __z)
  { return __builtin_cabsl(__z); }

  template<typename _Tp>
    inline _Tp
    abs(const complex<_Tp>& __z) { return __complex_abs(__z.__rep()); }
#else
  template<typename _Tp>
    inline _Tp
    abs(const complex<_Tp>& __z) { return __complex_abs(__z); }
#endif  


  // 26.2.7/4: arg(__z): Returns the phase angle of __z.
  template<typename _Tp>
    inline _Tp
    __complex_arg(const complex<_Tp>& __z)
    { return  atan2(__z.imag(), __z.real()); }

#if _GLIBCXX_USE_C99_COMPLEX
  inline float
  __complex_arg(__complex__ float __z) { return __builtin_cargf(__z); }

  inline double
  __complex_arg(__complex__ double __z) { return __builtin_carg(__z); }

  inline long double
  __complex_arg(const __complex__ long double& __z)
  { return __builtin_cargl(__z); }

  template<typename _Tp>
    inline _Tp
    arg(const complex<_Tp>& __z) { return __complex_arg(__z.__rep()); }
#else
  template<typename _Tp>
    inline _Tp
    arg(const complex<_Tp>& __z) { return __complex_arg(__z); }
#endif

  // 26.2.7/5: norm(__z) returns the squared magnitude of __z.
  //     As defined, norm() is -not- a norm is the common mathematical
  //     sens used in numerics.  The helper class _Norm_helper<> tries to
  //     distinguish between builtin floating point and the rest, so as
  //     to deliver an answer as close as possible to the real value.
  template<bool>
    struct _Norm_helper
    {
      template<typename _Tp>
        static inline _Tp _S_do_it(const complex<_Tp>& __z)
        {
          const _Tp __x = __z.real();
          const _Tp __y = __z.imag();
          return __x * __x + __y * __y;
        }
    };

  template<>
    struct _Norm_helper<true>
    {
      template<typename _Tp>
        static inline _Tp _S_do_it(const complex<_Tp>& __z)
        {
          _Tp __res = std::abs(__z);
          return __res * __res;
        }
    };
  
  template<typename _Tp>
    inline _Tp
    norm(const complex<_Tp>& __z)
    {
      return _Norm_helper<__is_floating<_Tp>::__value 
	&& !_GLIBCXX_FAST_MATH>::_S_do_it(__z);
    }

  template<typename _Tp>
    inline complex<_Tp>
    polar(const _Tp& __rho, const _Tp& __theta)
    { return complex<_Tp>(__rho * cos(__theta), __rho * sin(__theta)); }

  template<typename _Tp>
    inline complex<_Tp>
    conj(const complex<_Tp>& __z)
    { return complex<_Tp>(__z.real(), -__z.imag()); }
  
  // Transcendentals

  // 26.2.8/1 cos(__z):  Returns the cosine of __z.
  template<typename _Tp>
    inline complex<_Tp>
    __complex_cos(const complex<_Tp>& __z)
    {
      const _Tp __x = __z.real();
      const _Tp __y = __z.imag();
      return complex<_Tp>(cos(__x) * cosh(__y), -sin(__x) * sinh(__y));
    }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_cos(__complex__ float __z) { return __builtin_ccosf(__z); }

  inline __complex__ double
  __complex_cos(__complex__ double __z) { return __builtin_ccos(__z); }

  inline __complex__ long double
  __complex_cos(const __complex__ long double& __z)
  { return __builtin_ccosl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    cos(const complex<_Tp>& __z) { return __complex_cos(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    cos(const complex<_Tp>& __z) { return __complex_cos(__z); }
#endif

  // 26.2.8/2 cosh(__z): Returns the hyperbolic cosine of __z.
  template<typename _Tp>
    inline complex<_Tp>
    __complex_cosh(const complex<_Tp>& __z)
    {
      const _Tp __x = __z.real();
      const _Tp __y = __z.imag();
      return complex<_Tp>(cosh(__x) * cos(__y), sinh(__x) * sin(__y));
    }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_cosh(__complex__ float __z) { return __builtin_ccoshf(__z); }

  inline __complex__ double
  __complex_cosh(__complex__ double __z) { return __builtin_ccosh(__z); }

  inline __complex__ long double
  __complex_cosh(const __complex__ long double& __z)
  { return __builtin_ccoshl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    cosh(const complex<_Tp>& __z) { return __complex_cosh(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    cosh(const complex<_Tp>& __z) { return __complex_cosh(__z); }
#endif

  // 26.2.8/3 exp(__z): Returns the complex base e exponential of x
  template<typename _Tp>
    inline complex<_Tp>
    __complex_exp(const complex<_Tp>& __z)
    { return std::polar(exp(__z.real()), __z.imag()); }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_exp(__complex__ float __z) { return __builtin_cexpf(__z); }

  inline __complex__ double
  __complex_exp(__complex__ double __z) { return __builtin_cexp(__z); }

  inline __complex__ long double
  __complex_exp(const __complex__ long double& __z)
  { return __builtin_cexpl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    exp(const complex<_Tp>& __z) { return __complex_exp(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    exp(const complex<_Tp>& __z) { return __complex_exp(__z); }
#endif

  // 26.2.8/5 log(__z): Returns the natural complex logarithm of __z.
  //                    The branch cut is along the negative axis.
  template<typename _Tp>
    inline complex<_Tp>
    __complex_log(const complex<_Tp>& __z)
    { return complex<_Tp>(log(std::abs(__z)), std::arg(__z)); }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_log(__complex__ float __z) { return __builtin_clogf(__z); }

  inline __complex__ double
  __complex_log(__complex__ double __z) { return __builtin_clog(__z); }

  inline __complex__ long double
  __complex_log(const __complex__ long double& __z)
  { return __builtin_clogl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    log(const complex<_Tp>& __z) { return __complex_log(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    log(const complex<_Tp>& __z) { return __complex_log(__z); }
#endif

  template<typename _Tp>
    inline complex<_Tp>
    log10(const complex<_Tp>& __z)
    { return std::log(__z) / log(_Tp(10.0)); }

  // 26.2.8/10 sin(__z): Returns the sine of __z.
  template<typename _Tp>
    inline complex<_Tp>
    __complex_sin(const complex<_Tp>& __z)
    {
      const _Tp __x = __z.real();
      const _Tp __y = __z.imag();
      return complex<_Tp>(sin(__x) * cosh(__y), cos(__x) * sinh(__y)); 
    }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_sin(__complex__ float __z) { return __builtin_csinf(__z); }

  inline __complex__ double
  __complex_sin(__complex__ double __z) { return __builtin_csin(__z); }

  inline __complex__ long double
  __complex_sin(const __complex__ long double& __z)
  { return __builtin_csinl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    sin(const complex<_Tp>& __z) { return __complex_sin(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    sin(const complex<_Tp>& __z) { return __complex_sin(__z); }
#endif

  // 26.2.8/11 sinh(__z): Returns the hyperbolic sine of __z.
  template<typename _Tp>
    inline complex<_Tp>
    __complex_sinh(const complex<_Tp>& __z)
    {
      const _Tp __x = __z.real();
      const _Tp  __y = __z.imag();
      return complex<_Tp>(sinh(__x) * cos(__y), cosh(__x) * sin(__y));
    }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_sinh(__complex__ float __z) { return __builtin_csinhf(__z); }      

  inline __complex__ double
  __complex_sinh(__complex__ double __z) { return __builtin_csinh(__z); }      

  inline __complex__ long double
  __complex_sinh(const __complex__ long double& __z)
  { return __builtin_csinhl(__z); }      

  template<typename _Tp>
    inline complex<_Tp>
    sinh(const complex<_Tp>& __z) { return __complex_sinh(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    sinh(const complex<_Tp>& __z) { return __complex_sinh(__z); }
#endif

  // 26.2.8/13 sqrt(__z): Returns the complex square root of __z.
  //                     The branch cut is on the negative axis.
  template<typename _Tp>
    complex<_Tp>
    __complex_sqrt(const complex<_Tp>& __z)
    {
      _Tp __x = __z.real();
      _Tp __y = __z.imag();

      if (__x == _Tp())
        {
          _Tp __t = sqrt(abs(__y) / 2);
          return complex<_Tp>(__t, __y < _Tp() ? -__t : __t);
        }
      else
        {
          _Tp __t = sqrt(2 * (std::abs(__z) + abs(__x)));
          _Tp __u = __t / 2;
          return __x > _Tp()
            ? complex<_Tp>(__u, __y / __t)
            : complex<_Tp>(abs(__y) / __t, __y < _Tp() ? -__u : __u);
        }
    }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_sqrt(__complex__ float __z) { return __builtin_csqrtf(__z); }

  inline __complex__ double
  __complex_sqrt(__complex__ double __z) { return __builtin_csqrt(__z); }

  inline __complex__ long double
  __complex_sqrt(const __complex__ long double& __z)
  { return __builtin_csqrtl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    sqrt(const complex<_Tp>& __z) { return __complex_sqrt(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    sqrt(const complex<_Tp>& __z) { return __complex_sqrt(__z); }
#endif

  // 26.2.8/14 tan(__z):  Return the complex tangent of __z.
  
  template<typename _Tp>
    inline complex<_Tp>
    __complex_tan(const complex<_Tp>& __z)
    { return std::sin(__z) / std::cos(__z); }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_tan(__complex__ float __z) { return __builtin_ctanf(__z); }

  inline __complex__ double
  __complex_tan(__complex__ double __z) { return __builtin_ctan(__z); }

  inline __complex__ long double
  __complex_tan(const __complex__ long double& __z)
  { return __builtin_ctanl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    tan(const complex<_Tp>& __z) { return __complex_tan(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    tan(const complex<_Tp>& __z) { return __complex_tan(__z); }
#endif


  // 26.2.8/15 tanh(__z):  Returns the hyperbolic tangent of __z.
  
  template<typename _Tp>
    inline complex<_Tp>
    __complex_tanh(const complex<_Tp>& __z)
    { return std::sinh(__z) / std::cosh(__z); }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_tanh(__complex__ float __z) { return __builtin_ctanhf(__z); }

  inline __complex__ double
  __complex_tanh(__complex__ double __z) { return __builtin_ctanh(__z); }

  inline __complex__ long double
  __complex_tanh(const __complex__ long double& __z)
  { return __builtin_ctanhl(__z); }

  template<typename _Tp>
    inline complex<_Tp>
    tanh(const complex<_Tp>& __z) { return __complex_tanh(__z.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    tanh(const complex<_Tp>& __z) { return __complex_tanh(__z); }
#endif


  // 26.2.8/9  pow(__x, __y): Returns the complex power base of __x
  //                          raised to the __y-th power.  The branch
  //                          cut is on the negative axis.
  template<typename _Tp>
    complex<_Tp>
    __complex_pow_unsigned(complex<_Tp> __x, unsigned __n)
    {
      complex<_Tp> __y = __n % 2 ? __x : complex<_Tp>(1);

      while (__n >>= 1)
        {
          __x *= __x;
          if (__n % 2)
            __y *= __x;
        }

      return __y;
    }

  // In C++11 mode we used to implement the resolution of
  // DR 844. complex pow return type is ambiguous.
  // thus the following overload was disabled in that mode.  However, doing
  // that causes all sorts of issues, see, for example:
  //   http://gcc.gnu.org/ml/libstdc++/2013-01/msg00058.html
  // and also PR57974.
  template<typename _Tp>
    inline complex<_Tp>
    pow(const complex<_Tp>& __z, int __n)
    {
      return __n < 0
	? complex<_Tp>(1) / std::__complex_pow_unsigned(__z, -(unsigned)__n)
        : std::__complex_pow_unsigned(__z, __n);
    }

  template<typename _Tp>
    complex<_Tp>
    pow(const complex<_Tp>& __x, const _Tp& __y)
    {
#ifndef _GLIBCXX_USE_C99_COMPLEX
      if (__x == _Tp())
	return _Tp();
#endif
      if (__x.imag() == _Tp() && __x.real() > _Tp())
        return pow(__x.real(), __y);

      complex<_Tp> __t = std::log(__x);
      return std::polar(exp(__y * __t.real()), __y * __t.imag());
    }

  template<typename _Tp>
    inline complex<_Tp>
    __complex_pow(const complex<_Tp>& __x, const complex<_Tp>& __y)
    { return __x == _Tp() ? _Tp() : std::exp(__y * std::log(__x)); }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_pow(__complex__ float __x, __complex__ float __y)
  { return __builtin_cpowf(__x, __y); }

  inline __complex__ double
  __complex_pow(__complex__ double __x, __complex__ double __y)
  { return __builtin_cpow(__x, __y); }

  inline __complex__ long double
  __complex_pow(const __complex__ long double& __x,
		const __complex__ long double& __y)
  { return __builtin_cpowl(__x, __y); }

  template<typename _Tp>
    inline complex<_Tp>
    pow(const complex<_Tp>& __x, const complex<_Tp>& __y)
    { return __complex_pow(__x.__rep(), __y.__rep()); }
#else
  template<typename _Tp>
    inline complex<_Tp>
    pow(const complex<_Tp>& __x, const complex<_Tp>& __y)
    { return __complex_pow(__x, __y); }
#endif

  template<typename _Tp>
    inline complex<_Tp>
    pow(const _Tp& __x, const complex<_Tp>& __y)
    {
      return __x > _Tp() ? std::polar(pow(__x, __y.real()),
				      __y.imag() * log(__x))
	                 : std::pow(complex<_Tp>(__x), __y);
    }

  /// 26.2.3  complex specializations
  /// complex<float> specialization
  template<>
    struct complex<float>
    {
      typedef float value_type;
      typedef __complex__ float _ComplexT;

      _GLIBCXX_CONSTEXPR complex(_ComplexT __z) : _M_value(__z) { }

      _GLIBCXX_CONSTEXPR complex(float __r = 0.0f, float __i = 0.0f)
#if __cplusplus >= 201103L
      : _M_value{ __r, __i } { }
#else
      {
	__real__ _M_value = __r;
	__imag__ _M_value = __i;
      }
#endif

      explicit _GLIBCXX_CONSTEXPR complex(const complex<double>&);
      explicit _GLIBCXX_CONSTEXPR complex(const complex<long double>&);	

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      __attribute ((__abi_tag__ ("cxx11")))
      constexpr float 
      real() const { return __real__ _M_value; }

      __attribute ((__abi_tag__ ("cxx11")))
      constexpr float 
      imag() const { return __imag__ _M_value; }
#else
      float& 
      real() { return __real__ _M_value; }

      const float& 
      real() const { return __real__ _M_value; }      

      float& 
      imag() { return __imag__ _M_value; }

      const float& 
      imag() const { return __imag__ _M_value; }
#endif

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      void 
      real(float __val) { __real__ _M_value = __val; }

      void 
      imag(float __val) { __imag__ _M_value = __val; }

      complex&
      operator=(float __f)
      {
	_M_value = __f;
	return *this;
      }

      complex&
      operator+=(float __f)
      {
	_M_value += __f;
	return *this;
      }

      complex&
      operator-=(float __f)
      {
	_M_value -= __f;
	return *this;
      }

      complex&
      operator*=(float __f)
      {
	_M_value *= __f;
	return *this;
      }

      complex&
      operator/=(float __f)
      {
	_M_value /= __f;
	return *this;
      }

      // Let the compiler synthesize the copy and assignment
      // operator.  It always does a pretty good job.
      // complex& operator=(const complex&);

      template<typename _Tp>
        complex&
        operator=(const complex<_Tp>&  __z)
	{
	  __real__ _M_value = __z.real();
	  __imag__ _M_value = __z.imag();
	  return *this;
	}

      template<typename _Tp>
        complex&
        operator+=(const complex<_Tp>& __z)
	{
	  __real__ _M_value += __z.real();
	  __imag__ _M_value += __z.imag();
	  return *this;
	}

      template<class _Tp>
        complex&
        operator-=(const complex<_Tp>& __z)
	{
	  __real__ _M_value -= __z.real();
	  __imag__ _M_value -= __z.imag();
	  return *this;
	}

      template<class _Tp>
        complex&
        operator*=(const complex<_Tp>& __z)
	{
	  _ComplexT __t;
	  __real__ __t = __z.real();
	  __imag__ __t = __z.imag();
	  _M_value *= __t;
	  return *this;
	}

      template<class _Tp>
        complex&
        operator/=(const complex<_Tp>& __z)
	{
	  _ComplexT __t;
	  __real__ __t = __z.real();
	  __imag__ __t = __z.imag();
	  _M_value /= __t;
	  return *this;
	}

      _GLIBCXX_USE_CONSTEXPR _ComplexT __rep() const { return _M_value; }

    private:
      _ComplexT _M_value;
    };

  /// 26.2.3  complex specializations
  /// complex<double> specialization
  template<>
    struct complex<double>
    {
      typedef double value_type;
      typedef __complex__ double _ComplexT;

      _GLIBCXX_CONSTEXPR complex(_ComplexT __z) : _M_value(__z) { }

      _GLIBCXX_CONSTEXPR complex(double __r = 0.0, double __i = 0.0)
#if __cplusplus >= 201103L
      : _M_value{ __r, __i } { }
#else
      {
	__real__ _M_value = __r;
	__imag__ _M_value = __i;
      }
#endif

      _GLIBCXX_CONSTEXPR complex(const complex<float>& __z)
      : _M_value(__z.__rep()) { }

      explicit _GLIBCXX_CONSTEXPR complex(const complex<long double>&);	

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      __attribute ((__abi_tag__ ("cxx11")))
      constexpr double 
      real() const { return __real__ _M_value; }

      __attribute ((__abi_tag__ ("cxx11")))
      constexpr double 
      imag() const { return __imag__ _M_value; }
#else
      double& 
      real() { return __real__ _M_value; }

      const double& 
      real() const { return __real__ _M_value; }

      double& 
      imag() { return __imag__ _M_value; }

      const double& 
      imag() const { return __imag__ _M_value; }
#endif

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      void 
      real(double __val) { __real__ _M_value = __val; }

      void 
      imag(double __val) { __imag__ _M_value = __val; }

      complex&
      operator=(double __d)
      {
	_M_value = __d;
	return *this;
      }

      complex&
      operator+=(double __d)
      {
	_M_value += __d;
	return *this;
      }
	
      complex&
      operator-=(double __d)
      {
	_M_value -= __d;
	return *this;
      }

      complex&
      operator*=(double __d)
      {
	_M_value *= __d;
	return *this;
      }

      complex&
      operator/=(double __d)
      {
	_M_value /= __d;
	return *this;
      }

      // The compiler will synthesize this, efficiently.
      // complex& operator=(const complex&);

      template<typename _Tp>
        complex&
        operator=(const complex<_Tp>& __z)
	{
	  __real__ _M_value = __z.real();
	  __imag__ _M_value = __z.imag();
	  return *this;
	}

      template<typename _Tp>
        complex&
        operator+=(const complex<_Tp>& __z)
	{
	  __real__ _M_value += __z.real();
	  __imag__ _M_value += __z.imag();
	  return *this;
	}

      template<typename _Tp>
        complex&
        operator-=(const complex<_Tp>& __z)
	{
	  __real__ _M_value -= __z.real();
	  __imag__ _M_value -= __z.imag();
	  return *this;
	}

      template<typename _Tp>
        complex&
        operator*=(const complex<_Tp>& __z)
	{
	  _ComplexT __t;
	  __real__ __t = __z.real();
	  __imag__ __t = __z.imag();
	  _M_value *= __t;
	  return *this;
	}

      template<typename _Tp>
        complex&
        operator/=(const complex<_Tp>& __z)
	{
	  _ComplexT __t;
	  __real__ __t = __z.real();
	  __imag__ __t = __z.imag();
	  _M_value /= __t;
	  return *this;
	}

      _GLIBCXX_USE_CONSTEXPR _ComplexT __rep() const { return _M_value; }

    private:
      _ComplexT _M_value;
    };

  /// 26.2.3  complex specializations
  /// complex<long double> specialization
  template<>
    struct complex<long double>
    {
      typedef long double value_type;
      typedef __complex__ long double _ComplexT;

      _GLIBCXX_CONSTEXPR complex(_ComplexT __z) : _M_value(__z) { }

      _GLIBCXX_CONSTEXPR complex(long double __r = 0.0L, 
				 long double __i = 0.0L)
#if __cplusplus >= 201103L
      : _M_value{ __r, __i } { }
#else
      {
	__real__ _M_value = __r;
	__imag__ _M_value = __i;
      }
#endif

      _GLIBCXX_CONSTEXPR complex(const complex<float>& __z)
      : _M_value(__z.__rep()) { }

      _GLIBCXX_CONSTEXPR complex(const complex<double>& __z)
      : _M_value(__z.__rep()) { }

#if __cplusplus >= 201103L
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      __attribute ((__abi_tag__ ("cxx11")))
      constexpr long double 
      real() const { return __real__ _M_value; }

      __attribute ((__abi_tag__ ("cxx11")))
      constexpr long double 
      imag() const { return __imag__ _M_value; }
#else
      long double& 
      real() { return __real__ _M_value; }

      const long double& 
      real() const { return __real__ _M_value; }

      long double& 
      imag() { return __imag__ _M_value; }

      const long double& 
      imag() const { return __imag__ _M_value; }
#endif

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // DR 387. std::complex over-encapsulated.
      void 
      real(long double __val) { __real__ _M_value = __val; }

      void 
      imag(long double __val) { __imag__ _M_value = __val; }

      complex&
      operator=(long double __r)
      {
	_M_value = __r;
	return *this;
      }

      complex&
      operator+=(long double __r)
      {
	_M_value += __r;
	return *this;
      }

      complex&
      operator-=(long double __r)
      {
	_M_value -= __r;
	return *this;
      }

      complex&
      operator*=(long double __r)
      {
	_M_value *= __r;
	return *this;
      }

      complex&
      operator/=(long double __r)
      {
	_M_value /= __r;
	return *this;
      }

      // The compiler knows how to do this efficiently
      // complex& operator=(const complex&);

      template<typename _Tp>
        complex&
        operator=(const complex<_Tp>& __z)
	{
	  __real__ _M_value = __z.real();
	  __imag__ _M_value = __z.imag();
	  return *this;
	}

      template<typename _Tp>
        complex&
	operator+=(const complex<_Tp>& __z)
	{
	  __real__ _M_value += __z.real();
	  __imag__ _M_value += __z.imag();
	  return *this;
	}

      template<typename _Tp>
        complex&
	operator-=(const complex<_Tp>& __z)
	{
	  __real__ _M_value -= __z.real();
	  __imag__ _M_value -= __z.imag();
	  return *this;
	}

      template<typename _Tp>
        complex&
	operator*=(const complex<_Tp>& __z)
	{
	  _ComplexT __t;
	  __real__ __t = __z.real();
	  __imag__ __t = __z.imag();
	  _M_value *= __t;
	  return *this;
	}

      template<typename _Tp>
        complex&
	operator/=(const complex<_Tp>& __z)
	{
	  _ComplexT __t;
	  __real__ __t = __z.real();
	  __imag__ __t = __z.imag();
	  _M_value /= __t;
	  return *this;
	}

      _GLIBCXX_USE_CONSTEXPR _ComplexT __rep() const { return _M_value; }

    private:
      _ComplexT _M_value;
    };

  // These bits have to be at the end of this file, so that the
  // specializations have all been defined.
  inline _GLIBCXX_CONSTEXPR
  complex<float>::complex(const complex<double>& __z)
  : _M_value(__z.__rep()) { }

  inline _GLIBCXX_CONSTEXPR
  complex<float>::complex(const complex<long double>& __z)
  : _M_value(__z.__rep()) { }

  inline _GLIBCXX_CONSTEXPR
  complex<double>::complex(const complex<long double>& __z)
  : _M_value(__z.__rep()) { }

  // Inhibit implicit instantiations for required instantiations,
  // which are defined via explicit instantiations elsewhere.
  // NB:  This syntax is a GNU extension.
#if _GLIBCXX_EXTERN_TEMPLATE
  extern template istream& operator>>(istream&, complex<float>&);
  extern template ostream& operator<<(ostream&, const complex<float>&);
  extern template istream& operator>>(istream&, complex<double>&);
  extern template ostream& operator<<(ostream&, const complex<double>&);
  extern template istream& operator>>(istream&, complex<long double>&);
  extern template ostream& operator<<(ostream&, const complex<long double>&);

#ifdef _GLIBCXX_USE_WCHAR_T
  extern template wistream& operator>>(wistream&, complex<float>&);
  extern template wostream& operator<<(wostream&, const complex<float>&);
  extern template wistream& operator>>(wistream&, complex<double>&);
  extern template wostream& operator<<(wostream&, const complex<double>&);
  extern template wistream& operator>>(wistream&, complex<long double>&);
  extern template wostream& operator<<(wostream&, const complex<long double>&);
#endif
#endif

  // @} group complex_numbers

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

namespace __gnu_cxx _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // See ext/type_traits.h for the primary template.
  template<typename _Tp, typename _Up>
    struct __promote_2<std::complex<_Tp>, _Up>
    {
    public:
      typedef std::complex<typename __promote_2<_Tp, _Up>::__type> __type;
    };

  template<typename _Tp, typename _Up>
    struct __promote_2<_Tp, std::complex<_Up> >
    {
    public:
      typedef std::complex<typename __promote_2<_Tp, _Up>::__type> __type;
    };
  
  template<typename _Tp, typename _Up>
    struct __promote_2<std::complex<_Tp>, std::complex<_Up> >
    {
    public:
      typedef std::complex<typename __promote_2<_Tp, _Up>::__type> __type;
    };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#if __cplusplus >= 201103L

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // Forward declarations.
  template<typename _Tp> std::complex<_Tp> acos(const std::complex<_Tp>&);
  template<typename _Tp> std::complex<_Tp> asin(const std::complex<_Tp>&);
  template<typename _Tp> std::complex<_Tp> atan(const std::complex<_Tp>&);

  template<typename _Tp> std::complex<_Tp> acosh(const std::complex<_Tp>&);
  template<typename _Tp> std::complex<_Tp> asinh(const std::complex<_Tp>&);
  template<typename _Tp> std::complex<_Tp> atanh(const std::complex<_Tp>&);
  // DR 595.
  template<typename _Tp> _Tp               fabs(const std::complex<_Tp>&);

  template<typename _Tp>
    inline std::complex<_Tp>
    __complex_acos(const std::complex<_Tp>& __z)
    {
      const std::complex<_Tp> __t = std::asin(__z);
      const _Tp __pi_2 = 1.5707963267948966192313216916397514L;
      return std::complex<_Tp>(__pi_2 - __t.real(), -__t.imag());
    }

#if _GLIBCXX_USE_C99_COMPLEX_TR1
  inline __complex__ float
  __complex_acos(__complex__ float __z)
  { return __builtin_cacosf(__z); }

  inline __complex__ double
  __complex_acos(__complex__ double __z)
  { return __builtin_cacos(__z); }

  inline __complex__ long double
  __complex_acos(const __complex__ long double& __z)
  { return __builtin_cacosl(__z); }

  template<typename _Tp>
    inline std::complex<_Tp>
    acos(const std::complex<_Tp>& __z)
    { return __complex_acos(__z.__rep()); }
#else
  /// acos(__z) [8.1.2].
  //  Effects:  Behaves the same as C99 function cacos, defined
  //            in subclause 7.3.5.1.
  template<typename _Tp>
    inline std::complex<_Tp>
    acos(const std::complex<_Tp>& __z)
    { return __complex_acos(__z); }
#endif

  template<typename _Tp>
    inline std::complex<_Tp>
    __complex_asin(const std::complex<_Tp>& __z)
    {
      std::complex<_Tp> __t(-__z.imag(), __z.real());
      __t = std::asinh(__t);
      return std::complex<_Tp>(__t.imag(), -__t.real());
    }

#if _GLIBCXX_USE_C99_COMPLEX_TR1
  inline __complex__ float
  __complex_asin(__complex__ float __z)
  { return __builtin_casinf(__z); }

  inline __complex__ double
  __complex_asin(__complex__ double __z)
  { return __builtin_casin(__z); }

  inline __complex__ long double
  __complex_asin(const __complex__ long double& __z)
  { return __builtin_casinl(__z); }

  template<typename _Tp>
    inline std::complex<_Tp>
    asin(const std::complex<_Tp>& __z)
    { return __complex_asin(__z.__rep()); }
#else
  /// asin(__z) [8.1.3].
  //  Effects:  Behaves the same as C99 function casin, defined
  //            in subclause 7.3.5.2.
  template<typename _Tp>
    inline std::complex<_Tp>
    asin(const std::complex<_Tp>& __z)
    { return __complex_asin(__z); }
#endif
  
  template<typename _Tp>
    std::complex<_Tp>
    __complex_atan(const std::complex<_Tp>& __z)
    {
      const _Tp __r2 = __z.real() * __z.real();
      const _Tp __x = _Tp(1.0) - __r2 - __z.imag() * __z.imag();

      _Tp __num = __z.imag() + _Tp(1.0);
      _Tp __den = __z.imag() - _Tp(1.0);

      __num = __r2 + __num * __num;
      __den = __r2 + __den * __den;

      return std::complex<_Tp>(_Tp(0.5) * atan2(_Tp(2.0) * __z.real(), __x),
			       _Tp(0.25) * log(__num / __den));
    }

#if _GLIBCXX_USE_C99_COMPLEX_TR1
  inline __complex__ float
  __complex_atan(__complex__ float __z)
  { return __builtin_catanf(__z); }

  inline __complex__ double
  __complex_atan(__complex__ double __z)
  { return __builtin_catan(__z); }

  inline __complex__ long double
  __complex_atan(const __complex__ long double& __z)
  { return __builtin_catanl(__z); }

  template<typename _Tp>
    inline std::complex<_Tp>
    atan(const std::complex<_Tp>& __z)
    { return __complex_atan(__z.__rep()); }
#else
  /// atan(__z) [8.1.4].
  //  Effects:  Behaves the same as C99 function catan, defined
  //            in subclause 7.3.5.3.
  template<typename _Tp>
    inline std::complex<_Tp>
    atan(const std::complex<_Tp>& __z)
    { return __complex_atan(__z); }
#endif

  template<typename _Tp>
    std::complex<_Tp>
    __complex_acosh(const std::complex<_Tp>& __z)
    {
      // Kahan's formula.
      return _Tp(2.0) * std::log(std::sqrt(_Tp(0.5) * (__z + _Tp(1.0)))
				 + std::sqrt(_Tp(0.5) * (__z - _Tp(1.0))));
    }

#if _GLIBCXX_USE_C99_COMPLEX_TR1
  inline __complex__ float
  __complex_acosh(__complex__ float __z)
  { return __builtin_cacoshf(__z); }

  inline __complex__ double
  __complex_acosh(__complex__ double __z)
  { return __builtin_cacosh(__z); }

  inline __complex__ long double
  __complex_acosh(const __complex__ long double& __z)
  { return __builtin_cacoshl(__z); }

  template<typename _Tp>
    inline std::complex<_Tp>
    acosh(const std::complex<_Tp>& __z)
    { return __complex_acosh(__z.__rep()); }
#else
  /// acosh(__z) [8.1.5].
  //  Effects:  Behaves the same as C99 function cacosh, defined
  //            in subclause 7.3.6.1.
  template<typename _Tp>
    inline std::complex<_Tp>
    acosh(const std::complex<_Tp>& __z)
    { return __complex_acosh(__z); }
#endif

  template<typename _Tp>
    std::complex<_Tp>
    __complex_asinh(const std::complex<_Tp>& __z)
    {
      std::complex<_Tp> __t((__z.real() - __z.imag())
			    * (__z.real() + __z.imag()) + _Tp(1.0),
			    _Tp(2.0) * __z.real() * __z.imag());
      __t = std::sqrt(__t);

      return std::log(__t + __z);
    }

#if _GLIBCXX_USE_C99_COMPLEX_TR1
  inline __complex__ float
  __complex_asinh(__complex__ float __z)
  { return __builtin_casinhf(__z); }

  inline __complex__ double
  __complex_asinh(__complex__ double __z)
  { return __builtin_casinh(__z); }

  inline __complex__ long double
  __complex_asinh(const __complex__ long double& __z)
  { return __builtin_casinhl(__z); }

  template<typename _Tp>
    inline std::complex<_Tp>
    asinh(const std::complex<_Tp>& __z)
    { return __complex_asinh(__z.__rep()); }
#else
  /// asinh(__z) [8.1.6].
  //  Effects:  Behaves the same as C99 function casin, defined
  //            in subclause 7.3.6.2.
  template<typename _Tp>
    inline std::complex<_Tp>
    asinh(const std::complex<_Tp>& __z)
    { return __complex_asinh(__z); }
#endif

  template<typename _Tp>
    std::complex<_Tp>
    __complex_atanh(const std::complex<_Tp>& __z)
    {
      const _Tp __i2 = __z.imag() * __z.imag();
      const _Tp __x = _Tp(1.0) - __i2 - __z.real() * __z.real();

      _Tp __num = _Tp(1.0) + __z.real();
      _Tp __den = _Tp(1.0) - __z.real();

      __num = __i2 + __num * __num;
      __den = __i2 + __den * __den;

      return std::complex<_Tp>(_Tp(0.25) * (log(__num) - log(__den)),
			       _Tp(0.5) * atan2(_Tp(2.0) * __z.imag(), __x));
    }

#if _GLIBCXX_USE_C99_COMPLEX_TR1
  inline __complex__ float
  __complex_atanh(__complex__ float __z)
  { return __builtin_catanhf(__z); }

  inline __complex__ double
  __complex_atanh(__complex__ double __z)
  { return __builtin_catanh(__z); }

  inline __complex__ long double
  __complex_atanh(const __complex__ long double& __z)
  { return __builtin_catanhl(__z); }

  template<typename _Tp>
    inline std::complex<_Tp>
    atanh(const std::complex<_Tp>& __z)
    { return __complex_atanh(__z.__rep()); }
#else
  /// atanh(__z) [8.1.7].
  //  Effects:  Behaves the same as C99 function catanh, defined
  //            in subclause 7.3.6.3.
  template<typename _Tp>
    inline std::complex<_Tp>
    atanh(const std::complex<_Tp>& __z)
    { return __complex_atanh(__z); }
#endif

  template<typename _Tp>
    inline _Tp
    /// fabs(__z) [8.1.8].
    //  Effects:  Behaves the same as C99 function cabs, defined
    //            in subclause 7.3.8.1.
    fabs(const std::complex<_Tp>& __z)
    { return std::abs(__z); }

  /// Additional overloads [8.1.9].
  template<typename _Tp>
    inline typename __gnu_cxx::__promote<_Tp>::__type
    arg(_Tp __x)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
#if (_GLIBCXX_USE_C99_MATH && !_GLIBCXX_USE_C99_FP_MACROS_DYNAMIC)
      return std::signbit(__x) ? __type(3.1415926535897932384626433832795029L)
	                       : __type();
#else
      return std::arg(std::complex<__type>(__x));
#endif
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__promote<_Tp>::__type
    imag(_Tp)
    { return _Tp(); }

  template<typename _Tp>
    inline typename __gnu_cxx::__promote<_Tp>::__type
    norm(_Tp __x)
    {
      typedef typename __gnu_cxx::__promote<_Tp>::__type __type;
      return __type(__x) * __type(__x);
    }

  template<typename _Tp>
    inline typename __gnu_cxx::__promote<_Tp>::__type
    real(_Tp __x)
    { return __x; }

  template<typename _Tp, typename _Up>
    inline std::complex<typename __gnu_cxx::__promote_2<_Tp, _Up>::__type>
    pow(const std::complex<_Tp>& __x, const _Up& __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return std::pow(std::complex<__type>(__x), __type(__y));
    }

  template<typename _Tp, typename _Up>
    inline std::complex<typename __gnu_cxx::__promote_2<_Tp, _Up>::__type>
    pow(const _Tp& __x, const std::complex<_Up>& __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return std::pow(__type(__x), std::complex<__type>(__y));
    }

  template<typename _Tp, typename _Up>
    inline std::complex<typename __gnu_cxx::__promote_2<_Tp, _Up>::__type>
    pow(const std::complex<_Tp>& __x, const std::complex<_Up>& __y)
    {
      typedef typename __gnu_cxx::__promote_2<_Tp, _Up>::__type __type;
      return std::pow(std::complex<__type>(__x),
		      std::complex<__type>(__y));
    }

  // Forward declarations.
  // DR 781.
  template<typename _Tp> std::complex<_Tp> proj(const std::complex<_Tp>&);

  template<typename _Tp>
    std::complex<_Tp>
    __complex_proj(const std::complex<_Tp>& __z)
    {
      const _Tp __den = (__z.real() * __z.real()
			 + __z.imag() * __z.imag() + _Tp(1.0));

      return std::complex<_Tp>((_Tp(2.0) * __z.real()) / __den,
			       (_Tp(2.0) * __z.imag()) / __den);
    }

#if _GLIBCXX_USE_C99_COMPLEX
  inline __complex__ float
  __complex_proj(__complex__ float __z)
  { return __builtin_cprojf(__z); }

  inline __complex__ double
  __complex_proj(__complex__ double __z)
  { return __builtin_cproj(__z); }

  inline __complex__ long double
  __complex_proj(const __complex__ long double& __z)
  { return __builtin_cprojl(__z); }

  template<typename _Tp>
    inline std::complex<_Tp>
    proj(const std::complex<_Tp>& __z)
    { return __complex_proj(__z.__rep()); }
#else
  template<typename _Tp>
    inline std::complex<_Tp>
    proj(const std::complex<_Tp>& __z)
    { return __complex_proj(__z); }
#endif

  // DR 1137.
  template<typename _Tp>
    inline typename __gnu_cxx::__promote<_Tp>::__type
    proj(_Tp __x)
    { return __x; }

  template<typename _Tp>
    inline typename __gnu_cxx::__promote<_Tp>::__type
    conj(_Tp __x)
    { return __x; }

#if __cplusplus > 201103L

inline namespace literals {
inline namespace complex_literals {

#define __cpp_lib_complex_udls 201309

  constexpr std::complex<float>
  operator""if(long double __num)
  { return std::complex<float>{0.0F, static_cast<float>(__num)}; }

  constexpr std::complex<float>
  operator""if(unsigned long long __num)
  { return std::complex<float>{0.0F, static_cast<float>(__num)}; }

  constexpr std::complex<double>
  operator""i(long double __num)
  { return std::complex<double>{0.0, static_cast<double>(__num)}; }

  constexpr std::complex<double>
  operator""i(unsigned long long __num)
  { return std::complex<double>{0.0, static_cast<double>(__num)}; }

  constexpr std::complex<long double>
  operator""il(long double __num)
  { return std::complex<long double>{0.0L, __num}; }

  constexpr std::complex<long double>
  operator""il(unsigned long long __num)
  { return std::complex<long double>{0.0L, static_cast<long double>(__num)}; }

} // inline namespace complex_literals
} // inline namespace literals

#endif // C++14

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif  // C++11

#endif  /* _GLIBCXX_COMPLEX */

// -*- C++ -*- forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/cstdio
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c stdio.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: 27.8.2  C Library files
//

#pragma GCC system_header

#include <bits/c++config.h>
#include <stdio.h>

#ifndef _GLIBCXX_CSTDIO
#define _GLIBCXX_CSTDIO 1

#ifndef _GLIBCXX_HAVE_GETS
extern "C" char* gets (char* __s) __attribute__((deprecated));
#endif

// Get rid of those macros defined in <stdio.h> in lieu of real functions.
#undef clearerr
#undef fclose
#undef feof
#undef ferror
#undef fflush
#undef fgetc
#undef fgetpos
#undef fgets
#undef fopen
#undef fprintf
#undef fputc
#undef fputs
#undef fread
#undef freopen
#undef fscanf
#undef fseek
#undef fsetpos
#undef ftell
#undef fwrite
#undef getc
#undef getchar
#if __cplusplus <= 201103L
# undef gets
#endif
#undef perror
#undef printf
#undef putc
#undef putchar
#undef puts
#undef remove
#undef rename
#undef rewind
#undef scanf
#undef setbuf
#undef setvbuf
#undef sprintf
#undef sscanf
#undef tmpfile
#undef tmpnam
#undef ungetc
#undef vfprintf
#undef vprintf
#undef vsprintf

namespace std
{
  using ::FILE;
  using ::fpos_t;

  using ::clearerr;
  using ::fclose;
  using ::feof;
  using ::ferror;
  using ::fflush;
  using ::fgetc;
  using ::fgetpos;
  using ::fgets;
  using ::fopen;
  using ::fprintf;
  using ::fputc;
  using ::fputs;
  using ::fread;
  using ::freopen;
  using ::fscanf;
  using ::fseek;
  using ::fsetpos;
  using ::ftell;
  using ::fwrite;
  using ::getc;
  using ::getchar;
#if __cplusplus <= 201103L
  // LWG 2249
  using ::gets;
#endif
  using ::perror;
  using ::printf;
  using ::putc;
  using ::putchar;
  using ::puts;
  using ::remove;
  using ::rename;
  using ::rewind;
  using ::scanf;
  using ::setbuf;
  using ::setvbuf;
  using ::sprintf;
  using ::sscanf;
  using ::tmpfile;
#if _GLIBCXX_USE_TMPNAM
  using ::tmpnam;
#endif
  using ::ungetc;
  using ::vfprintf;
  using ::vprintf;
  using ::vsprintf;
} // namespace

#if _GLIBCXX_USE_C99

#undef snprintf
#undef vfscanf
#undef vscanf
#undef vsnprintf
#undef vsscanf

namespace __gnu_cxx
{
#if _GLIBCXX_USE_C99_CHECK || _GLIBCXX_USE_C99_DYNAMIC
  extern "C" int
  (snprintf)(char * __restrict, std::size_t, const char * __restrict, ...)
  throw ();
  extern "C" int
  (vfscanf)(FILE * __restrict, const char * __restrict, __gnuc_va_list);
  extern "C" int (vscanf)(const char * __restrict, __gnuc_va_list);
  extern "C" int
  (vsnprintf)(char * __restrict, std::size_t, const char * __restrict,
	      __gnuc_va_list) throw ();
  extern "C" int
  (vsscanf)(const char * __restrict, const char * __restrict, __gnuc_va_list)
  throw ();
#endif

#if !_GLIBCXX_USE_C99_DYNAMIC
  using ::snprintf;
  using ::vfscanf;
  using ::vscanf;
  using ::vsnprintf;
  using ::vsscanf;
#endif
} // namespace __gnu_cxx

namespace std
{
  using ::__gnu_cxx::snprintf;
  using ::__gnu_cxx::vfscanf;
  using ::__gnu_cxx::vscanf;
  using ::__gnu_cxx::vsnprintf;
  using ::__gnu_cxx::vsscanf;
} // namespace std

#endif // _GLIBCXX_USE_C99

#endif

// -*- C++ -*- forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/cstdlib
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c stdlib.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: 20.4.6  C library
//

#pragma GCC system_header

#include <bits/c++config.h>

#ifndef _GLIBCXX_CSTDLIB
#define _GLIBCXX_CSTDLIB 1

#if !_GLIBCXX_HOSTED
// The C standard does not require a freestanding implementation to
// provide <stdlib.h>.  However, the C++ standard does still require
// <cstdlib> -- but only the functionality mentioned in
// [lib.support.start.term].

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

namespace std
{
  extern "C" void abort(void) throw () _GLIBCXX_NORETURN;
  extern "C" int atexit(void (*)(void)) throw ();
  extern "C" void exit(int) throw () _GLIBCXX_NORETURN;
#if __cplusplus >= 201103L
# ifdef _GLIBCXX_HAVE_AT_QUICK_EXIT
  extern "C" int at_quick_exit(void (*)(void)) throw ();
# endif
# ifdef _GLIBCXX_HAVE_QUICK_EXIT
  extern "C" void quick_exit(int) throw() _GLIBCXX_NORETURN;
# endif
#endif
} // namespace std

#else

#include <stdlib.h>

// Get rid of those macros defined in <stdlib.h> in lieu of real functions.
#undef abort
#undef abs
#undef atexit
#if __cplusplus >= 201103L
# ifdef _GLIBCXX_HAVE_AT_QUICK_EXIT
#  undef at_quick_exit
# endif
#endif
#undef atof
#undef atoi
#undef atol
#undef bsearch
#undef calloc
#undef div
#undef exit
#undef free
#undef getenv
#undef labs
#undef ldiv
#undef malloc
#undef mblen
#undef mbstowcs
#undef mbtowc
#undef qsort
#if __cplusplus >= 201103L
# ifdef _GLIBCXX_HAVE_QUICK_EXIT
#  undef quick_exit
# endif
#endif
#undef rand
#undef realloc
#undef srand
#undef strtod
#undef strtol
#undef strtoul
#undef system
#undef wcstombs
#undef wctomb

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  using ::div_t;
  using ::ldiv_t;

  using ::abort;
  using ::abs;
  using ::atexit;
#if __cplusplus >= 201103L
# ifdef _GLIBCXX_HAVE_AT_QUICK_EXIT
  using ::at_quick_exit;
# endif
#endif
  using ::atof;
  using ::atoi;
  using ::atol;
  using ::bsearch;
  using ::calloc;
  using ::div;
  using ::exit;
  using ::free;
  using ::getenv;
  using ::labs;
  using ::ldiv;
  using ::malloc;
#ifdef _GLIBCXX_HAVE_MBSTATE_T
  using ::mblen;
  using ::mbstowcs;
  using ::mbtowc;
#endif // _GLIBCXX_HAVE_MBSTATE_T
  using ::qsort;
#if __cplusplus >= 201103L
# ifdef _GLIBCXX_HAVE_QUICK_EXIT
  using ::quick_exit;
# endif
#endif
  using ::rand;
  using ::realloc;
  using ::srand;
  using ::strtod;
  using ::strtol;
  using ::strtoul;
  using ::system;
#ifdef _GLIBCXX_USE_WCHAR_T
  using ::wcstombs;
  using ::wctomb;
#endif // _GLIBCXX_USE_WCHAR_T

#ifndef __CORRECT_ISO_CPP_STDLIB_H_PROTO
  inline long
  abs(long __i) { return __builtin_labs(__i); }

  inline ldiv_t
  div(long __i, long __j) { return ldiv(__i, __j); }
#endif

#ifdef _GLIBCXX_USE_LONG_LONG
  inline long long
  abs(long long __x) { return __builtin_llabs (__x); }
#endif

#if !defined(__STRICT_ANSI__) && defined(_GLIBCXX_USE_INT128)
  inline __int128
  abs(__int128 __x) { return __x >= 0 ? __x : -__x; }
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#if _GLIBCXX_USE_C99

#undef _Exit
#undef llabs
#undef lldiv
#undef atoll
#undef strtoll
#undef strtoull
#undef strtof
#undef strtold

namespace __gnu_cxx _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

#if !_GLIBCXX_USE_C99_LONG_LONG_DYNAMIC
  using ::lldiv_t;
#endif
#if _GLIBCXX_USE_C99_CHECK || _GLIBCXX_USE_C99_DYNAMIC
  extern "C" void (_Exit)(int) throw () _GLIBCXX_NORETURN;
#endif
#if !_GLIBCXX_USE_C99_DYNAMIC
  using ::_Exit;
#endif

#if !_GLIBCXX_USE_C99_LONG_LONG_DYNAMIC
  using ::llabs;

  inline lldiv_t
  div(long long __n, long long __d)
  { lldiv_t __q; __q.quot = __n / __d; __q.rem = __n % __d; return __q; }

  using ::lldiv;
#endif

#if _GLIBCXX_USE_C99_LONG_LONG_CHECK || _GLIBCXX_USE_C99_LONG_LONG_DYNAMIC
  extern "C" long long int (atoll)(const char *) throw ();
  extern "C" long long int
    (strtoll)(const char * __restrict, char ** __restrict, int) throw ();
  extern "C" unsigned long long int
    (strtoull)(const char * __restrict, char ** __restrict, int) throw ();
#endif
#if !_GLIBCXX_USE_C99_LONG_LONG_DYNAMIC
  using ::atoll;
  using ::strtoll;
  using ::strtoull;
#endif
  using ::strtof;
  using ::strtold;

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace __gnu_cxx

namespace std
{
#if !_GLIBCXX_USE_C99_LONG_LONG_DYNAMIC
  using ::__gnu_cxx::lldiv_t;
#endif
  using ::__gnu_cxx::_Exit;
#if !_GLIBCXX_USE_C99_LONG_LONG_DYNAMIC
  using ::__gnu_cxx::llabs;
  using ::__gnu_cxx::div;
  using ::__gnu_cxx::lldiv;
#endif
  using ::__gnu_cxx::atoll;
  using ::__gnu_cxx::strtof;
  using ::__gnu_cxx::strtoll;
  using ::__gnu_cxx::strtoull;
  using ::__gnu_cxx::strtold;
} // namespace std

#endif // _GLIBCXX_USE_C99

#endif // !_GLIBCXX_HOSTED

#endif

// -*- C++ -*- forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file cstring
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c string.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: 20.4.6  C library
//

#pragma GCC system_header

#include <bits/c++config.h>
#include <string.h>

#ifndef _GLIBCXX_CSTRING
#define _GLIBCXX_CSTRING 1

// Get rid of those macros defined in <string.h> in lieu of real functions.
#undef memchr
#undef memcmp
#undef memcpy
#undef memmove
#undef memset
#undef strcat
#undef strchr
#undef strcmp
#undef strcoll
#undef strcpy
#undef strcspn
#undef strerror
#undef strlen
#undef strncat
#undef strncmp
#undef strncpy
#undef strpbrk
#undef strrchr
#undef strspn
#undef strstr
#undef strtok
#undef strxfrm

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  using ::memchr;
  using ::memcmp;
  using ::memcpy;
  using ::memmove;
  using ::memset;
  using ::strcat;
  using ::strcmp;
  using ::strcoll;
  using ::strcpy;
  using ::strcspn;
  using ::strerror;
  using ::strlen;
  using ::strncat;
  using ::strncmp;
  using ::strncpy;
  using ::strspn;
  using ::strtok;
  using ::strxfrm;
  using ::strchr;
  using ::strpbrk;
  using ::strrchr;
  using ::strstr;

#ifndef __CORRECT_ISO_CPP_STRING_H_PROTO
  inline void*
  memchr(void* __s, int __c, size_t __n)
  { return __builtin_memchr(__s, __c, __n); }

  inline char*
  strchr(char* __s, int __n)
  { return __builtin_strchr(__s, __n); }

  inline char*
  strpbrk(char* __s1, const char* __s2)
  { return __builtin_strpbrk(__s1, __s2); }

  inline char*
  strrchr(char* __s, int __n)
  { return __builtin_strrchr(__s, __n); }

  inline char*
  strstr(char* __s1, const char* __s2)
  { return __builtin_strstr(__s1, __s2); }
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif

// -*- C++ -*- forwarding header.

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/ctime
 *  This is a Standard C++ Library file.  You should @c \#include this file
 *  in your programs, rather than any of the @a *.h implementation files.
 *
 *  This is the C++ version of the Standard C Library header @c time.h,
 *  and its contents are (mostly) the same as that header, but are all
 *  contained in the namespace @c std (except for names which are defined
 *  as macros in C).
 */

//
// ISO C++ 14882: 20.5  Date and time
//

#pragma GCC system_header

#include <bits/c++config.h>
#include <time.h>

#ifndef _GLIBCXX_CTIME
#define _GLIBCXX_CTIME 1

// Get rid of those macros defined in <time.h> in lieu of real functions.
#undef clock
#undef difftime
#undef mktime
#undef time
#undef asctime
#undef ctime
#undef gmtime
#undef localtime
#undef strftime

namespace std
{
  using ::clock_t;
  using ::time_t;
  using ::tm;

  using ::clock;
  using ::difftime;
  using ::mktime;
  using ::time;
  using ::asctime;
  using ::ctime;
  using ::gmtime;
  using ::localtime;
  using ::strftime;
} // namespace

#endif

// <deque> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/deque
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_DEQUE
#define _GLIBCXX_DEQUE 1

#pragma GCC system_header

#include <bits/stl_algobase.h>
#include <bits/allocator.h>
#include <bits/stl_construct.h>
#include <bits/stl_uninitialized.h>
#include <bits/stl_deque.h>
#include <bits/range_access.h>
#include <bits/deque.tcc>

#ifdef _GLIBCXX_DEBUG
# include <debug/deque>
#endif

#ifdef _GLIBCXX_PROFILE
# include <profile/deque>
#endif

#endif /* _GLIBCXX_DEQUE */

// Exception Handling support header for -*- C++ -*-

// Copyright (C) 1995-2014 Free Software Foundation, Inc.
//
// This file is part of GCC.
//
// GCC is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// GCC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file exception
 *  This is a Standard C++ Library header.
 */

#ifndef __EXCEPTION__
#define __EXCEPTION__

#pragma GCC system_header

#pragma GCC visibility push(default)

#include <bits/c++config.h>
#include <bits/atomic_lockfree_defines.h>

extern "C++" {

namespace std
{
  /**
   * @defgroup exceptions Exceptions
   * @ingroup diagnostics
   *
   * Classes and functions for reporting errors via exception classes.
   * @{
   */

  /**
   *  @brief Base class for all library exceptions.
   *
   *  This is the base class for all exceptions thrown by the standard
   *  library, and by certain language expressions.  You are free to derive
   *  your own %exception classes, or use a different hierarchy, or to
   *  throw non-class data (e.g., fundamental types).
   */
  class exception
  {
  public:
    exception() _GLIBCXX_USE_NOEXCEPT { }
    virtual ~exception() _GLIBCXX_USE_NOEXCEPT;

    /** Returns a C-style character string describing the general cause
     *  of the current error.  */
    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;
  };

  /** If an %exception is thrown which is not listed in a function's
   *  %exception specification, one of these may be thrown.  */
  class bad_exception : public exception
  {
  public:
    bad_exception() _GLIBCXX_USE_NOEXCEPT { }

    // This declaration is not useless:
    // http://gcc.gnu.org/onlinedocs/gcc-3.0.2/gcc_6.html#SEC118
    virtual ~bad_exception() _GLIBCXX_USE_NOEXCEPT;

    // See comment in eh_exception.cc.
    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT;
  };

  /// If you write a replacement %terminate handler, it must be of this type.
  typedef void (*terminate_handler) ();

  /// If you write a replacement %unexpected handler, it must be of this type.
  typedef void (*unexpected_handler) ();

  /// Takes a new handler function as an argument, returns the old function.
  terminate_handler set_terminate(terminate_handler) _GLIBCXX_USE_NOEXCEPT;

#if __cplusplus >= 201103L
  /// Return the current terminate handler.
  terminate_handler get_terminate() noexcept;
#endif

  /** The runtime will call this function if %exception handling must be
   *  abandoned for any reason.  It can also be called by the user.  */
  void terminate() _GLIBCXX_USE_NOEXCEPT __attribute__ ((__noreturn__));

  /// Takes a new handler function as an argument, returns the old function.
  unexpected_handler set_unexpected(unexpected_handler) _GLIBCXX_USE_NOEXCEPT;

#if __cplusplus >= 201103L
  /// Return the current unexpected handler.
  unexpected_handler get_unexpected() noexcept;
#endif

  /** The runtime will call this function if an %exception is thrown which
   *  violates the function's %exception specification.  */
  void unexpected() __attribute__ ((__noreturn__));

  /** [18.6.4]/1:  'Returns true after completing evaluation of a
   *  throw-expression until either completing initialization of the
   *  exception-declaration in the matching handler or entering @c unexpected()
   *  due to the throw; or after entering @c terminate() for any reason
   *  other than an explicit call to @c terminate().  [Note: This includes
   *  stack unwinding [15.2].  end note]'
   *
   *  2: 'When @c uncaught_exception() is true, throwing an
   *  %exception can result in a call of @c terminate()
   *  (15.5.1).'
   */
  bool uncaught_exception() _GLIBCXX_USE_NOEXCEPT __attribute__ ((__pure__));

  // @} group exceptions
} // namespace std

namespace __gnu_cxx
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  @brief A replacement for the standard terminate_handler which
   *  prints more information about the terminating exception (if any)
   *  on stderr.
   *
   *  @ingroup exceptions
   *
   *  Call
   *   @code
   *     std::set_terminate(__gnu_cxx::__verbose_terminate_handler)
   *   @endcode
   *  to use.  For more info, see
   *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt02ch06s02.html
   *
   *  In 3.4 and later, this is on by default.
   */
  void __verbose_terminate_handler();

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

} // extern "C++"

#pragma GCC visibility pop

#if (__cplusplus >= 201103L) && (ATOMIC_INT_LOCK_FREE > 1)
#include <bits/exception_ptr.h>
#include <bits/nested_exception.h>
#endif

#endif

// File based streams -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/fstream
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.8  File-based streams
//

#ifndef _GLIBCXX_FSTREAM
#define _GLIBCXX_FSTREAM 1

#pragma GCC system_header

#include <istream>
#include <ostream>
#include <bits/codecvt.h>
#include <cstdio>             // For BUFSIZ
#include <bits/basic_file.h>  // For __basic_file, __c_lock
#if __cplusplus >= 201103L
#include <string>             // For std::string overloads.
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // [27.8.1.1] template class basic_filebuf
  /**
   *  @brief  The actual work of input and output (for files).
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class associates both its input and output sequence with an
   *  external disk file, and maintains a joint file position for both
   *  sequences.  Many of its semantics are described in terms of similar
   *  behavior in the Standard C Library's @c FILE streams.
   *
   *  Requirements on traits_type, specific to this class:
   *  - traits_type::pos_type must be fpos<traits_type::state_type>
   *  - traits_type::off_type must be streamoff
   *  - traits_type::state_type must be Assignable and DefaultConstructible,
   *  - traits_type::state_type() must be the initial state for codecvt.
   */
  template<typename _CharT, typename _Traits>
    class basic_filebuf : public basic_streambuf<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT                     	        char_type;
      typedef _Traits                    	        traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      typedef basic_streambuf<char_type, traits_type>  	__streambuf_type;
      typedef basic_filebuf<char_type, traits_type>     __filebuf_type;
      typedef __basic_file<char>		        __file_type;
      typedef typename traits_type::state_type          __state_type;
      typedef codecvt<char_type, char, __state_type>    __codecvt_type;

      friend class ios_base; // For sync_with_stdio.

    protected:
      // Data Members:
      // MT lock inherited from libio or other low-level io library.
      __c_lock          	_M_lock;

      // External buffer.
      __file_type 		_M_file;

      /// Place to stash in || out || in | out settings for current filebuf.
      ios_base::openmode 	_M_mode;

      // Beginning state type for codecvt.
      __state_type 		_M_state_beg;

      // During output, the state that corresponds to pptr(),
      // during input, the state that corresponds to egptr() and
      // _M_ext_next.
      __state_type		_M_state_cur;

      // Not used for output. During input, the state that corresponds
      // to eback() and _M_ext_buf.
      __state_type		_M_state_last;

      /// Pointer to the beginning of internal buffer.
      char_type*		_M_buf; 	

      /**
       *  Actual size of internal buffer. This number is equal to the size
       *  of the put area + 1 position, reserved for the overflow char of
       *  a full area.
       */
      size_t			_M_buf_size;

      // Set iff _M_buf is allocated memory from _M_allocate_internal_buffer.
      bool			_M_buf_allocated;

      /**
       *  _M_reading == false && _M_writing == false for @b uncommitted mode;
       *  _M_reading == true for @b read mode;
       *  _M_writing == true for @b write mode;
       *
       *  NB: _M_reading == true && _M_writing == true is unused.
       */
      bool                      _M_reading;
      bool                      _M_writing;

      //@{
      /**
       *  Necessary bits for putback buffer management.
       *
       *  @note pbacks of over one character are not currently supported.
       */
      char_type			_M_pback;
      char_type*		_M_pback_cur_save;
      char_type*		_M_pback_end_save;
      bool			_M_pback_init;
      //@}

      // Cached codecvt facet.
      const __codecvt_type* 	_M_codecvt;

      /**
       *  Buffer for external characters. Used for input when
       *  codecvt::always_noconv() == false. When valid, this corresponds
       *  to eback().
       */
      char*			_M_ext_buf;

      /**
       *  Size of buffer held by _M_ext_buf.
       */
      streamsize		_M_ext_buf_size;

      /**
       *  Pointers into the buffer held by _M_ext_buf that delimit a
       *  subsequence of bytes that have been read but not yet converted.
       *  When valid, _M_ext_next corresponds to egptr().
       */
      const char*		_M_ext_next;
      char*			_M_ext_end;

      /**
       *  Initializes pback buffers, and moves normal buffers to safety.
       *  Assumptions:
       *  _M_in_cur has already been moved back
       */
      void
      _M_create_pback()
      {
	if (!_M_pback_init)
	  {
	    _M_pback_cur_save = this->gptr();
	    _M_pback_end_save = this->egptr();
	    this->setg(&_M_pback, &_M_pback, &_M_pback + 1);
	    _M_pback_init = true;
	  }
      }

      /**
       *  Deactivates pback buffer contents, and restores normal buffer.
       *  Assumptions:
       *  The pback buffer has only moved forward.
       */
      void
      _M_destroy_pback() throw()
      {
	if (_M_pback_init)
	  {
	    // Length _M_in_cur moved in the pback buffer.
	    _M_pback_cur_save += this->gptr() != this->eback();
	    this->setg(_M_buf, _M_pback_cur_save, _M_pback_end_save);
	    _M_pback_init = false;
	  }
      }

    public:
      // Constructors/destructor:
      /**
       *  @brief  Does not open any files.
       *
       *  The default constructor initializes the parent class using its
       *  own default ctor.
       */
      basic_filebuf();

      /**
       *  @brief  The destructor closes the file first.
       */
      virtual
      ~basic_filebuf()
      { this->close(); }

      // Members:
      /**
       *  @brief  Returns true if the external file is open.
       */
      bool
      is_open() const throw()
      { return _M_file.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *  @return  @c this on success, NULL on failure
       *
       *  If a file is already open, this function immediately fails.
       *  Otherwise it tries to open the file named @a __s using the flags
       *  given in @a __mode.
       *
       *  Table 92, adapted here, gives the relation between openmode
       *  combinations and the equivalent @c fopen() flags.
       *  (NB: lines app, in|out|app, in|app, binary|app, binary|in|out|app,
       *  and binary|in|app per DR 596)
       *  <pre>
       *  +---------------------------------------------------------+
       *  | ios_base Flag combination            stdio equivalent   |
       *  |binary  in  out  trunc  app                              |
       *  +---------------------------------------------------------+
       *  |             +                        w                  |
       *  |             +           +            a                  |
       *  |                         +            a                  |
       *  |             +     +                  w                  |
       *  |         +                            r                  |
       *  |         +   +                        r+                 |
       *  |         +   +     +                  w+                 |
       *  |         +   +           +            a+                 |
       *  |         +               +            a+                 |
       *  +---------------------------------------------------------+
       *  |   +         +                        wb                 |
       *  |   +         +           +            ab                 |
       *  |   +                     +            ab                 |
       *  |   +         +     +                  wb                 |
       *  |   +     +                            rb                 |
       *  |   +     +   +                        r+b                |
       *  |   +     +   +     +                  w+b                |
       *  |   +     +   +           +            a+b                |
       *  |   +     +               +            a+b                |
       *  +---------------------------------------------------------+
       *  </pre>
       */
      __filebuf_type*
      open(const char* __s, ios_base::openmode __mode);

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *  @return  @c this on success, NULL on failure
       */
      __filebuf_type*
      open(const std::string& __s, ios_base::openmode __mode)
      { return open(__s.c_str(), __mode); }
#endif

      /**
       *  @brief  Closes the currently associated file.
       *  @return  @c this on success, NULL on failure
       *
       *  If no file is currently open, this function immediately fails.
       *
       *  If a <em>put buffer area</em> exists, @c overflow(eof) is
       *  called to flush all the characters.  The file is then
       *  closed.
       *
       *  If any operations fail, this function also fails.
       */
      __filebuf_type*
      close();

    protected:
      void
      _M_allocate_internal_buffer();

      void
      _M_destroy_internal_buffer() throw();

      // [27.8.1.4] overridden virtual functions
      virtual streamsize
      showmanyc();

      // Stroustrup, 1998, p. 628
      // underflow() and uflow() functions are called to get the next
      // character from the real input source when the buffer is empty.
      // Buffered input uses underflow()

      virtual int_type
      underflow();

      virtual int_type
      pbackfail(int_type __c = _Traits::eof());

      // Stroustrup, 1998, p 648
      // The overflow() function is called to transfer characters to the
      // real output destination when the buffer is full. A call to
      // overflow(c) outputs the contents of the buffer plus the
      // character c.
      // 27.5.2.4.5
      // Consume some sequence of the characters in the pending sequence.
      virtual int_type
      overflow(int_type __c = _Traits::eof());

      // Convert internal byte sequence to external, char-based
      // sequence via codecvt.
      bool
      _M_convert_to_external(char_type*, streamsize);

      /**
       *  @brief  Manipulates the buffer.
       *  @param  __s  Pointer to a buffer area.
       *  @param  __n  Size of @a __s.
       *  @return  @c this
       *
       *  If no file has been opened, and both @a __s and @a __n are zero, then
       *  the stream becomes unbuffered.  Otherwise, @c __s is used as a
       *  buffer; see
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt11ch25s02.html
       *  for more.
       */
      virtual __streambuf_type*
      setbuf(char_type* __s, streamsize __n);

      virtual pos_type
      seekoff(off_type __off, ios_base::seekdir __way,
	      ios_base::openmode __mode = ios_base::in | ios_base::out);

      virtual pos_type
      seekpos(pos_type __pos,
	      ios_base::openmode __mode = ios_base::in | ios_base::out);

      // Common code for seekoff, seekpos, and overflow
      pos_type
      _M_seek(off_type __off, ios_base::seekdir __way, __state_type __state);
      
      int
      _M_get_ext_pos(__state_type &__state);

      virtual int
      sync();

      virtual void
      imbue(const locale& __loc);

      virtual streamsize
      xsgetn(char_type* __s, streamsize __n);

      virtual streamsize
      xsputn(const char_type* __s, streamsize __n);

      // Flushes output buffer, then writes unshift sequence.
      bool
      _M_terminate_output();

      /**
       *  This function sets the pointers of the internal buffer, both get
       *  and put areas. Typically:
       *
       *   __off == egptr() - eback() upon underflow/uflow (@b read mode);
       *   __off == 0 upon overflow (@b write mode);
       *   __off == -1 upon open, setbuf, seekoff/pos (@b uncommitted mode).
       *
       *  NB: epptr() - pbase() == _M_buf_size - 1, since _M_buf_size
       *  reflects the actual allocated memory and the last cell is reserved
       *  for the overflow char of a full put area.
       */
      void
      _M_set_buffer(streamsize __off)
      {
	const bool __testin = _M_mode & ios_base::in;
	const bool __testout = (_M_mode & ios_base::out
				|| _M_mode & ios_base::app);

	if (__testin && __off > 0)
	  this->setg(_M_buf, _M_buf, _M_buf + __off);
	else
	  this->setg(_M_buf, _M_buf, _M_buf);

	if (__testout && __off == 0 && _M_buf_size > 1 )
	  this->setp(_M_buf, _M_buf + _M_buf_size - 1);
	else
	  this->setp(0, 0);
      }
    };

  // [27.8.1.5] Template class basic_ifstream
  /**
   *  @brief  Controlling input for files.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class supports reading from named files, using the inherited
   *  functions from std::basic_istream.  To control the associated
   *  sequence, an instance of std::basic_filebuf is used, which this page
   *  refers to as @c sb.
   */
  template<typename _CharT, typename _Traits>
    class basic_ifstream : public basic_istream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_istream<char_type, traits_type>	__istream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors/Destructors:
      /**
       *  @brief  Default constructor.
       *
       *  Initializes @c sb using its default constructor, and passes
       *  @c &sb to the base class initializer.  Does not open any files
       *  (you haven't given it a filename to open).
       */
      basic_ifstream() : __istream_type(), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an input file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::in is automatically included in @a __mode.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      explicit
      basic_ifstream(const char* __s, ios_base::openmode __mode = ios_base::in)
      : __istream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Create an input file stream.
       *  @param  __s  std::string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::in is automatically included in @a __mode.
       */
      explicit
      basic_ifstream(const std::string& __s,
		     ios_base::openmode __mode = ios_base::in)
      : __istream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }
#endif

      /**
       *  @brief  The destructor does nothing.
       *
       *  The file is closed by the filebuf object, not the formatting
       *  stream.
       */
      ~basic_ifstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_filebuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
       */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief  Wrapper to test for an open file.
       *  @return  @c rdbuf()->is_open()
       */
      bool
      is_open()
      { return _M_filebuf.is_open(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 365. Lack of const-qualification in clause 27
      bool
      is_open() const
      { return _M_filebuf.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(s,__mode|in).  If that function
       *  fails, @c failbit is set in the stream's error state.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      void
      open(const char* __s, ios_base::openmode __mode = ios_base::in)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::in))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode|in).  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      open(const std::string& __s, ios_base::openmode __mode = ios_base::in)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::in))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }
#endif

      /**
       *  @brief  Close the file.
       *
       *  Calls @c std::basic_filebuf::close().  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };


  // [27.8.1.8] Template class basic_ofstream
  /**
   *  @brief  Controlling output for files.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class supports reading from named files, using the inherited
   *  functions from std::basic_ostream.  To control the associated
   *  sequence, an instance of std::basic_filebuf is used, which this page
   *  refers to as @c sb.
   */
  template<typename _CharT, typename _Traits>
    class basic_ofstream : public basic_ostream<_CharT,_Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_ostream<char_type, traits_type>	__ostream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors:
      /**
       *  @brief  Default constructor.
       *
       *  Initializes @c sb using its default constructor, and passes
       *  @c &sb to the base class initializer.  Does not open any files
       *  (you haven't given it a filename to open).
       */
      basic_ofstream(): __ostream_type(), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::out | @c ios_base::trunc is automatically included in
       *  @a __mode.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      explicit
      basic_ofstream(const char* __s,
		     ios_base::openmode __mode = ios_base::out|ios_base::trunc)
      : __ostream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Create an output file stream.
       *  @param  __s  std::string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  @c ios_base::out | @c ios_base::trunc is automatically included in
       *  @a __mode.
       */
      explicit
      basic_ofstream(const std::string& __s,
		     ios_base::openmode __mode = ios_base::out|ios_base::trunc)
      : __ostream_type(), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }
#endif

      /**
       *  @brief  The destructor does nothing.
       *
       *  The file is closed by the filebuf object, not the formatting
       *  stream.
       */
      ~basic_ofstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_filebuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
       */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief  Wrapper to test for an open file.
       *  @return  @c rdbuf()->is_open()
       */
      bool
      is_open()
      { return _M_filebuf.is_open(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 365. Lack of const-qualification in clause 27
      bool
      is_open() const
      { return _M_filebuf.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode|out|trunc).  If that
       *  function fails, @c failbit is set in the stream's error state.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      void
      open(const char* __s,
	   ios_base::openmode __mode = ios_base::out | ios_base::trunc)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::out))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(s,mode|out|trunc).  If that
       *  function fails, @c failbit is set in the stream's error state.
       */
      void
      open(const std::string& __s,
	   ios_base::openmode __mode = ios_base::out | ios_base::trunc)
      {
	if (!_M_filebuf.open(__s, __mode | ios_base::out))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }
#endif

      /**
       *  @brief  Close the file.
       *
       *  Calls @c std::basic_filebuf::close().  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };


  // [27.8.1.11] Template class basic_fstream
  /**
   *  @brief  Controlling input and output for files.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This class supports reading from and writing to named files, using
   *  the inherited functions from std::basic_iostream.  To control the
   *  associated sequence, an instance of std::basic_filebuf is used, which
   *  this page refers to as @c sb.
   */
  template<typename _CharT, typename _Traits>
    class basic_fstream : public basic_iostream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_filebuf<char_type, traits_type> 	__filebuf_type;
      typedef basic_ios<char_type, traits_type>		__ios_type;
      typedef basic_iostream<char_type, traits_type>	__iostream_type;

    private:
      __filebuf_type	_M_filebuf;

    public:
      // Constructors/destructor:
      /**
       *  @brief  Default constructor.
       *
       *  Initializes @c sb using its default constructor, and passes
       *  @c &sb to the base class initializer.  Does not open any files
       *  (you haven't given it a filename to open).
       */
      basic_fstream()
      : __iostream_type(), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an input/output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      explicit
      basic_fstream(const char* __s,
		    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(0), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Create an input/output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       */
      explicit
      basic_fstream(const std::string& __s,
		    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(0), _M_filebuf()
      {
	this->init(&_M_filebuf);
	this->open(__s, __mode);
      }
#endif

      /**
       *  @brief  The destructor does nothing.
       *
       *  The file is closed by the filebuf object, not the formatting
       *  stream.
       */
      ~basic_fstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_filebuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
       */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief  Wrapper to test for an open file.
       *  @return  @c rdbuf()->is_open()
       */
      bool
      is_open()
      { return _M_filebuf.is_open(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 365. Lack of const-qualification in clause 27
      bool
      is_open() const
      { return _M_filebuf.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode).  If that
       *  function fails, @c failbit is set in the stream's error state.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      void
      open(const char* __s,
	   ios_base::openmode __mode = ios_base::in | ios_base::out)
      {
	if (!_M_filebuf.open(__s, __mode))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode).  If that
       *  function fails, @c failbit is set in the stream's error state.
       */
      void
      open(const std::string& __s,
	   ios_base::openmode __mode = ios_base::in | ios_base::out)
      {
	if (!_M_filebuf.open(__s, __mode))
	  this->setstate(ios_base::failbit);
	else
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 409. Closing an fstream should clear error state
	  this->clear();
      }
#endif

      /**
       *  @brief  Close the file.
       *
       *  Calls @c std::basic_filebuf::close().  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      close()
      {
	if (!_M_filebuf.close())
	  this->setstate(ios_base::failbit);
      }
    };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#include <bits/fstream.tcc>

#endif /* _GLIBCXX_FSTREAM */

// <functional> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 * Copyright (c) 1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/** @file include/functional
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_FUNCTIONAL
#define _GLIBCXX_FUNCTIONAL 1

#pragma GCC system_header

#include <bits/c++config.h>
#include <bits/stl_function.h>

#if __cplusplus >= 201103L

#include <typeinfo>
#include <new>
#include <tuple>
#include <type_traits>
#include <bits/functexcept.h>
#include <bits/functional_hash.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _MemberPointer>
    class _Mem_fn;
  template<typename _Tp, typename _Class>
    _Mem_fn<_Tp _Class::*>
    mem_fn(_Tp _Class::*) noexcept;

_GLIBCXX_HAS_NESTED_TYPE(result_type)

  /// If we have found a result_type, extract it.
  template<bool _Has_result_type, typename _Functor>
    struct _Maybe_get_result_type
    { };

  template<typename _Functor>
    struct _Maybe_get_result_type<true, _Functor>
    { typedef typename _Functor::result_type result_type; };

  /**
   *  Base class for any function object that has a weak result type, as
   *  defined in 3.3/3 of TR1.
  */
  template<typename _Functor>
    struct _Weak_result_type_impl
    : _Maybe_get_result_type<__has_result_type<_Functor>::value, _Functor>
    { };

  /// Retrieve the result type for a function type.
  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...) const>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......) const>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...) volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......) volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...) const volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......) const volatile>
    { typedef _Res result_type; };

  /// Retrieve the result type for a function reference.
  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(&)(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(&)(_ArgTypes......)>
    { typedef _Res result_type; };

  /// Retrieve the result type for a function pointer.
  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(*)(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(*)(_ArgTypes......)>
    { typedef _Res result_type; };

  /// Retrieve result type for a member function pointer.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......)>
    { typedef _Res result_type; };

  /// Retrieve result type for a const member function pointer.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...) const>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......) const>
    { typedef _Res result_type; };

  /// Retrieve result type for a volatile member function pointer.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...) volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......) volatile>
    { typedef _Res result_type; };

  /// Retrieve result type for a const volatile member function pointer.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...)
				  const volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......)
				  const volatile>
    { typedef _Res result_type; };

  /**
   *  Strip top-level cv-qualifiers from the function object and let
   *  _Weak_result_type_impl perform the real work.
  */
  template<typename _Functor>
    struct _Weak_result_type
    : _Weak_result_type_impl<typename remove_cv<_Functor>::type>
    { };

  /**
   * Invoke a function object, which may be either a member pointer or a
   * function object. The first parameter will tell which.
   */
  template<typename _Functor, typename... _Args>
    inline
    typename enable_if<
	     (!is_member_pointer<_Functor>::value
	      && !is_function<_Functor>::value
	      && !is_function<typename remove_pointer<_Functor>::type>::value),
	     typename result_of<_Functor&(_Args&&...)>::type
	   >::type
    __invoke(_Functor& __f, _Args&&... __args)
    {
      return __f(std::forward<_Args>(__args)...);
    }

  template<typename _Functor, typename... _Args>
    inline
    typename enable_if<
             (is_member_pointer<_Functor>::value
              && !is_function<_Functor>::value
              && !is_function<typename remove_pointer<_Functor>::type>::value),
             typename result_of<_Functor(_Args&&...)>::type
           >::type
    __invoke(_Functor& __f, _Args&&... __args)
    {
      return std::mem_fn(__f)(std::forward<_Args>(__args)...);
    }

  // To pick up function references (that will become function pointers)
  template<typename _Functor, typename... _Args>
    inline
    typename enable_if<
	     (is_pointer<_Functor>::value
	      && is_function<typename remove_pointer<_Functor>::type>::value),
	     typename result_of<_Functor(_Args&&...)>::type
	   >::type
    __invoke(_Functor __f, _Args&&... __args)
    {
      return __f(std::forward<_Args>(__args)...);
    }

  /**
   *  Knowing which of unary_function and binary_function _Tp derives
   *  from, derives from the same and ensures that reference_wrapper
   *  will have a weak result type. See cases below.
   */
  template<bool _Unary, bool _Binary, typename _Tp>
    struct _Reference_wrapper_base_impl;

  // None of the nested argument types.
  template<typename _Tp>
    struct _Reference_wrapper_base_impl<false, false, _Tp>
    : _Weak_result_type<_Tp>
    { };

  // Nested argument_type only.
  template<typename _Tp>
    struct _Reference_wrapper_base_impl<true, false, _Tp>
    : _Weak_result_type<_Tp>
    {
      typedef typename _Tp::argument_type argument_type;
    };

  // Nested first_argument_type and second_argument_type only.
  template<typename _Tp>
    struct _Reference_wrapper_base_impl<false, true, _Tp>
    : _Weak_result_type<_Tp>
    {
      typedef typename _Tp::first_argument_type first_argument_type;
      typedef typename _Tp::second_argument_type second_argument_type;
    };

  // All the nested argument types.
   template<typename _Tp>
    struct _Reference_wrapper_base_impl<true, true, _Tp>
    : _Weak_result_type<_Tp>
    {
      typedef typename _Tp::argument_type argument_type;
      typedef typename _Tp::first_argument_type first_argument_type;
      typedef typename _Tp::second_argument_type second_argument_type;
    };

  _GLIBCXX_HAS_NESTED_TYPE(argument_type)
  _GLIBCXX_HAS_NESTED_TYPE(first_argument_type)
  _GLIBCXX_HAS_NESTED_TYPE(second_argument_type)

  /**
   *  Derives from unary_function or binary_function when it
   *  can. Specializations handle all of the easy cases. The primary
   *  template determines what to do with a class type, which may
   *  derive from both unary_function and binary_function.
  */
  template<typename _Tp>
    struct _Reference_wrapper_base
    : _Reference_wrapper_base_impl<
      __has_argument_type<_Tp>::value,
      __has_first_argument_type<_Tp>::value
      && __has_second_argument_type<_Tp>::value,
      _Tp>
    { };

  // - a function type (unary)
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1)>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) const>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) volatile>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) const volatile>
    : unary_function<_T1, _Res>
    { };

  // - a function type (binary)
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2)>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) const>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) volatile>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) const volatile>
    : binary_function<_T1, _T2, _Res>
    { };

  // - a function pointer type (unary)
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(*)(_T1)>
    : unary_function<_T1, _Res>
    { };

  // - a function pointer type (binary)
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(*)(_T1, _T2)>
    : binary_function<_T1, _T2, _Res>
    { };

  // - a pointer to member function type (unary, no qualifiers)
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)()>
    : unary_function<_T1*, _Res>
    { };

  // - a pointer to member function type (binary, no qualifiers)
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2)>
    : binary_function<_T1*, _T2, _Res>
    { };

  // - a pointer to member function type (unary, const)
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)() const>
    : unary_function<const _T1*, _Res>
    { };

  // - a pointer to member function type (binary, const)
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2) const>
    : binary_function<const _T1*, _T2, _Res>
    { };

  // - a pointer to member function type (unary, volatile)
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)() volatile>
    : unary_function<volatile _T1*, _Res>
    { };

  // - a pointer to member function type (binary, volatile)
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2) volatile>
    : binary_function<volatile _T1*, _T2, _Res>
    { };

  // - a pointer to member function type (unary, const volatile)
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)() const volatile>
    : unary_function<const volatile _T1*, _Res>
    { };

  // - a pointer to member function type (binary, const volatile)
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2) const volatile>
    : binary_function<const volatile _T1*, _T2, _Res>
    { };

  /**
   *  @brief Primary class template for reference_wrapper.
   *  @ingroup functors
   *  @{
   */
  template<typename _Tp>
    class reference_wrapper
    : public _Reference_wrapper_base<typename remove_cv<_Tp>::type>
    {
      _Tp* _M_data;

    public:
      typedef _Tp type;

      reference_wrapper(_Tp& __indata) noexcept
      : _M_data(std::__addressof(__indata))
      { }

      reference_wrapper(_Tp&&) = delete;

      reference_wrapper(const reference_wrapper<_Tp>& __inref) noexcept
      : _M_data(__inref._M_data)
      { }

      reference_wrapper&
      operator=(const reference_wrapper<_Tp>& __inref) noexcept
      {
	_M_data = __inref._M_data;
	return *this;
      }

      operator _Tp&() const noexcept
      { return this->get(); }

      _Tp&
      get() const noexcept
      { return *_M_data; }

      template<typename... _Args>
	typename result_of<_Tp&(_Args&&...)>::type
	operator()(_Args&&... __args) const
	{
	  return __invoke(get(), std::forward<_Args>(__args)...);
	}
    };


  /// Denotes a reference should be taken to a variable.
  template<typename _Tp>
    inline reference_wrapper<_Tp>
    ref(_Tp& __t) noexcept
    { return reference_wrapper<_Tp>(__t); }

  /// Denotes a const reference should be taken to a variable.
  template<typename _Tp>
    inline reference_wrapper<const _Tp>
    cref(const _Tp& __t) noexcept
    { return reference_wrapper<const _Tp>(__t); }

  template<typename _Tp>
    void ref(const _Tp&&) = delete;

  template<typename _Tp>
    void cref(const _Tp&&) = delete;

  /// Partial specialization.
  template<typename _Tp>
    inline reference_wrapper<_Tp>
    ref(reference_wrapper<_Tp> __t) noexcept
    { return ref(__t.get()); }

  /// Partial specialization.
  template<typename _Tp>
    inline reference_wrapper<const _Tp>
    cref(reference_wrapper<_Tp> __t) noexcept
    { return cref(__t.get()); }

  // @} group functors

  template<typename... _Types>
    struct _Pack : integral_constant<size_t, sizeof...(_Types)>
    { };

  template<typename _From, typename _To, bool = _From::value == _To::value>
    struct _AllConvertible : false_type
    { };

  template<typename... _From, typename... _To>
    struct _AllConvertible<_Pack<_From...>, _Pack<_To...>, true>
    : __and_<is_convertible<_From, _To>...>
    { };

  template<typename _Tp1, typename _Tp2>
    using _NotSame = __not_<is_same<typename std::decay<_Tp1>::type,
				    typename std::decay<_Tp2>::type>>;

  /**
   * Derives from @c unary_function or @c binary_function, or perhaps
   * nothing, depending on the number of arguments provided. The
   * primary template is the basis case, which derives nothing.
   */
  template<typename _Res, typename... _ArgTypes>
    struct _Maybe_unary_or_binary_function { };

  /// Derives from @c unary_function, as appropriate.
  template<typename _Res, typename _T1>
    struct _Maybe_unary_or_binary_function<_Res, _T1>
    : std::unary_function<_T1, _Res> { };

  /// Derives from @c binary_function, as appropriate.
  template<typename _Res, typename _T1, typename _T2>
    struct _Maybe_unary_or_binary_function<_Res, _T1, _T2>
    : std::binary_function<_T1, _T2, _Res> { };

  /// Implementation of @c mem_fn for member function pointers.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...)>
    : public _Maybe_unary_or_binary_function<_Res, _Class*, _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...);

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      // Require each _Args to be convertible to corresponding _ArgTypes
      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      // Require each _Args to be convertible to corresponding _ArgTypes
      // and require _Tp is not _Class, _Class& or _Class*
      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>, _NotSame<_Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      // Require each _Args to be convertible to corresponding _ArgTypes
      // and require _Tp is _Class or derived from _Class
      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      // Handle objects
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(_Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(_Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      // Handle pointers
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(_Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      // Handle smart pointers, references and pointers to derived
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res
	operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };

  /// Implementation of @c mem_fn for const member function pointers.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...) const>
    : public _Maybe_unary_or_binary_function<_Res, const _Class*,
					     _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...) const;

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>, _NotSame<const _Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      // Handle objects
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const _Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const _Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      // Handle pointers
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const _Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      // Handle smart pointers, references and pointers to derived
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };

  /// Implementation of @c mem_fn for volatile member function pointers.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...) volatile>
    : public _Maybe_unary_or_binary_function<_Res, volatile _Class*,
					     _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...) volatile;

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>, _NotSame<volatile _Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      // Handle objects
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(volatile _Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(volatile _Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      // Handle pointers
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(volatile _Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      // Handle smart pointers, references and pointers to derived
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res
	operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };

  /// Implementation of @c mem_fn for const volatile member function pointers.
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...) const volatile>
    : public _Maybe_unary_or_binary_function<_Res, const volatile _Class*,
					     _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...) const volatile;

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>,
		     _NotSame<const volatile _Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      // Handle objects
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const volatile _Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const volatile _Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      // Handle pointers
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const volatile _Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      // Handle smart pointers, references and pointers to derived
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };


  template<typename _Tp, bool>
    struct _Mem_fn_const_or_non
    {
      typedef const _Tp& type;
    };

  template<typename _Tp>
    struct _Mem_fn_const_or_non<_Tp, false>
    {
      typedef _Tp& type;
    };

  template<typename _Res, typename _Class>
    class _Mem_fn<_Res _Class::*>
    {
      using __pm_type = _Res _Class::*;

      // This bit of genius is due to Peter Dimov, improved slightly by
      // Douglas Gregor.
      // Made less elegant to support perfect forwarding and noexcept.
      template<typename _Tp>
	auto
	_M_call(_Tp&& __object, const _Class *) const noexcept
	-> decltype(std::forward<_Tp>(__object).*std::declval<__pm_type&>())
	{ return std::forward<_Tp>(__object).*__pm; }

      template<typename _Tp, typename _Up>
	auto
	_M_call(_Tp&& __object, _Up * const *) const noexcept
	-> decltype((*std::forward<_Tp>(__object)).*std::declval<__pm_type&>())
	{ return (*std::forward<_Tp>(__object)).*__pm; }

      template<typename _Tp>
	auto
	_M_call(_Tp&& __ptr, const volatile void*) const
	noexcept(noexcept((*__ptr).*std::declval<__pm_type&>()))
	-> decltype((*__ptr).*std::declval<__pm_type&>())
	{ return (*__ptr).*__pm; }

    public:
      explicit
      _Mem_fn(_Res _Class::*__pm) noexcept : __pm(__pm) { }

      // Handle objects
      _Res&
      operator()(_Class& __object) const noexcept
      { return __object.*__pm; }

      const _Res&
      operator()(const _Class& __object) const noexcept
      { return __object.*__pm; }

      _Res&&
      operator()(_Class&& __object) const noexcept
      { return std::forward<_Class>(__object).*__pm; }

      const _Res&&
      operator()(const _Class&& __object) const noexcept
      { return std::forward<const _Class>(__object).*__pm; }

      // Handle pointers
      _Res&
      operator()(_Class* __object) const noexcept
      { return __object->*__pm; }

      const _Res&
      operator()(const _Class* __object) const noexcept
      { return __object->*__pm; }

      // Handle smart pointers and derived
      template<typename _Tp, typename _Req = _Require<_NotSame<_Class*, _Tp>>>
	auto
	operator()(_Tp&& __unknown) const
	noexcept(noexcept(std::declval<_Mem_fn*>()->_M_call
			  (std::forward<_Tp>(__unknown), &__unknown)))
	-> decltype(this->_M_call(std::forward<_Tp>(__unknown), &__unknown))
	{ return _M_call(std::forward<_Tp>(__unknown), &__unknown); }

      template<typename _Tp, typename _Req = _Require<is_base_of<_Class, _Tp>>>
	auto
	operator()(reference_wrapper<_Tp> __ref) const
	noexcept(noexcept(std::declval<_Mem_fn&>()(__ref.get())))
	-> decltype((*this)(__ref.get()))
	{ return (*this)(__ref.get()); }

    private:
      _Res _Class::*__pm;
    };

  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 2048.  Unnecessary mem_fn overloads
  /**
   *  @brief Returns a function object that forwards to the member
   *  pointer @a pm.
   *  @ingroup functors
   */
  template<typename _Tp, typename _Class>
    inline _Mem_fn<_Tp _Class::*>
    mem_fn(_Tp _Class::* __pm) noexcept
    {
      return _Mem_fn<_Tp _Class::*>(__pm);
    }

  /**
   *  @brief Determines if the given type _Tp is a function object
   *  should be treated as a subexpression when evaluating calls to
   *  function objects returned by bind(). [TR1 3.6.1]
   *  @ingroup binders
   */
  template<typename _Tp>
    struct is_bind_expression
    : public false_type { };

  /**
   *  @brief Determines if the given type _Tp is a placeholder in a
   *  bind() expression and, if so, which placeholder it is. [TR1 3.6.2]
   *  @ingroup binders
   */
  template<typename _Tp>
    struct is_placeholder
    : public integral_constant<int, 0>
    { };

  /** @brief The type of placeholder objects defined by libstdc++.
   *  @ingroup binders
   */
  template<int _Num> struct _Placeholder { };

  _GLIBCXX_END_NAMESPACE_VERSION

  /** @namespace std::placeholders
   *  @brief ISO C++11 entities sub-namespace for functional.
   *  @ingroup binders
   */
  namespace placeholders
  {
  _GLIBCXX_BEGIN_NAMESPACE_VERSION
  /* Define a large number of placeholders. There is no way to
   * simplify this with variadic templates, because we're introducing
   * unique names for each.
   */
    extern const _Placeholder<1> _1;
    extern const _Placeholder<2> _2;
    extern const _Placeholder<3> _3;
    extern const _Placeholder<4> _4;
    extern const _Placeholder<5> _5;
    extern const _Placeholder<6> _6;
    extern const _Placeholder<7> _7;
    extern const _Placeholder<8> _8;
    extern const _Placeholder<9> _9;
    extern const _Placeholder<10> _10;
    extern const _Placeholder<11> _11;
    extern const _Placeholder<12> _12;
    extern const _Placeholder<13> _13;
    extern const _Placeholder<14> _14;
    extern const _Placeholder<15> _15;
    extern const _Placeholder<16> _16;
    extern const _Placeholder<17> _17;
    extern const _Placeholder<18> _18;
    extern const _Placeholder<19> _19;
    extern const _Placeholder<20> _20;
    extern const _Placeholder<21> _21;
    extern const _Placeholder<22> _22;
    extern const _Placeholder<23> _23;
    extern const _Placeholder<24> _24;
    extern const _Placeholder<25> _25;
    extern const _Placeholder<26> _26;
    extern const _Placeholder<27> _27;
    extern const _Placeholder<28> _28;
    extern const _Placeholder<29> _29;
  _GLIBCXX_END_NAMESPACE_VERSION
  }

  _GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  Partial specialization of is_placeholder that provides the placeholder
   *  number for the placeholder objects defined by libstdc++.
   *  @ingroup binders
   */
  template<int _Num>
    struct is_placeholder<_Placeholder<_Num> >
    : public integral_constant<int, _Num>
    { };

  template<int _Num>
    struct is_placeholder<const _Placeholder<_Num> >
    : public integral_constant<int, _Num>
    { };

  /**
   * Used by _Safe_tuple_element to indicate that there is no tuple
   * element at this position.
   */
  struct _No_tuple_element;

  /**
   * Implementation helper for _Safe_tuple_element. This primary
   * template handles the case where it is safe to use @c
   * tuple_element.
   */
  template<std::size_t __i, typename _Tuple, bool _IsSafe>
    struct _Safe_tuple_element_impl
    : tuple_element<__i, _Tuple> { };

  /**
   * Implementation helper for _Safe_tuple_element. This partial
   * specialization handles the case where it is not safe to use @c
   * tuple_element. We just return @c _No_tuple_element.
   */
  template<std::size_t __i, typename _Tuple>
    struct _Safe_tuple_element_impl<__i, _Tuple, false>
    {
      typedef _No_tuple_element type;
    };

  /**
   * Like tuple_element, but returns @c _No_tuple_element when
   * tuple_element would return an error.
   */
 template<std::size_t __i, typename _Tuple>
   struct _Safe_tuple_element
   : _Safe_tuple_element_impl<__i, _Tuple,
			      (__i < tuple_size<_Tuple>::value)>
   { };

  /**
   *  Maps an argument to bind() into an actual argument to the bound
   *  function object [TR1 3.6.3/5]. Only the first parameter should
   *  be specified: the rest are used to determine among the various
   *  implementations. Note that, although this class is a function
   *  object, it isn't entirely normal because it takes only two
   *  parameters regardless of the number of parameters passed to the
   *  bind expression. The first parameter is the bound argument and
   *  the second parameter is a tuple containing references to the
   *  rest of the arguments.
   */
  template<typename _Arg,
	   bool _IsBindExp = is_bind_expression<_Arg>::value,
	   bool _IsPlaceholder = (is_placeholder<_Arg>::value > 0)>
    class _Mu;

  /**
   *  If the argument is reference_wrapper<_Tp>, returns the
   *  underlying reference. [TR1 3.6.3/5 bullet 1]
   */
  template<typename _Tp>
    class _Mu<reference_wrapper<_Tp>, false, false>
    {
    public:
      typedef _Tp& result_type;

      /* Note: This won't actually work for const volatile
       * reference_wrappers, because reference_wrapper::get() is const
       * but not volatile-qualified. This might be a defect in the TR.
       */
      template<typename _CVRef, typename _Tuple>
	result_type
	operator()(_CVRef& __arg, _Tuple&) const volatile
	{ return __arg.get(); }
    };

  /**
   *  If the argument is a bind expression, we invoke the underlying
   *  function object with the same cv-qualifiers as we are given and
   *  pass along all of our arguments (unwrapped). [TR1 3.6.3/5 bullet 2]
   */
  template<typename _Arg>
    class _Mu<_Arg, true, false>
    {
    public:
      template<typename _CVArg, typename... _Args>
	auto
	operator()(_CVArg& __arg,
		   tuple<_Args...>& __tuple) const volatile
	-> decltype(__arg(declval<_Args>()...))
	{
	  // Construct an index tuple and forward to __call
	  typedef typename _Build_index_tuple<sizeof...(_Args)>::__type
	    _Indexes;
	  return this->__call(__arg, __tuple, _Indexes());
	}

    private:
      // Invokes the underlying function object __arg by unpacking all
      // of the arguments in the tuple.
      template<typename _CVArg, typename... _Args, std::size_t... _Indexes>
	auto
	__call(_CVArg& __arg, tuple<_Args...>& __tuple,
	       const _Index_tuple<_Indexes...>&) const volatile
	-> decltype(__arg(declval<_Args>()...))
	{
	  return __arg(std::forward<_Args>(get<_Indexes>(__tuple))...);
	}
    };

  /**
   *  If the argument is a placeholder for the Nth argument, returns
   *  a reference to the Nth argument to the bind function object.
   *  [TR1 3.6.3/5 bullet 3]
   */
  template<typename _Arg>
    class _Mu<_Arg, false, true>
    {
    public:
      template<typename _Signature> class result;

      template<typename _CVMu, typename _CVArg, typename _Tuple>
	class result<_CVMu(_CVArg, _Tuple)>
	{
	  // Add a reference, if it hasn't already been done for us.
	  // This allows us to be a little bit sloppy in constructing
	  // the tuple that we pass to result_of<...>.
	  typedef typename _Safe_tuple_element<(is_placeholder<_Arg>::value
						- 1), _Tuple>::type
	    __base_type;

	public:
	  typedef typename add_rvalue_reference<__base_type>::type type;
	};

      template<typename _Tuple>
	typename result<_Mu(_Arg, _Tuple)>::type
	operator()(const volatile _Arg&, _Tuple& __tuple) const volatile
	{
	  return std::forward<typename result<_Mu(_Arg, _Tuple)>::type>(
	      ::std::get<(is_placeholder<_Arg>::value - 1)>(__tuple));
	}
    };

  /**
   *  If the argument is just a value, returns a reference to that
   *  value. The cv-qualifiers on the reference are the same as the
   *  cv-qualifiers on the _Mu object. [TR1 3.6.3/5 bullet 4]
   */
  template<typename _Arg>
    class _Mu<_Arg, false, false>
    {
    public:
      template<typename _Signature> struct result;

      template<typename _CVMu, typename _CVArg, typename _Tuple>
	struct result<_CVMu(_CVArg, _Tuple)>
	{
	  typedef typename add_lvalue_reference<_CVArg>::type type;
	};

      // Pick up the cv-qualifiers of the argument
      template<typename _CVArg, typename _Tuple>
	_CVArg&&
	operator()(_CVArg&& __arg, _Tuple&) const volatile
	{ return std::forward<_CVArg>(__arg); }
    };

  /**
   *  Maps member pointers into instances of _Mem_fn but leaves all
   *  other function objects untouched. Used by tr1::bind(). The
   *  primary template handles the non--member-pointer case.
   */
  template<typename _Tp>
    struct _Maybe_wrap_member_pointer
    {
      typedef _Tp type;

      static const _Tp&
      __do_wrap(const _Tp& __x)
      { return __x; }

      static _Tp&&
      __do_wrap(_Tp&& __x)
      { return static_cast<_Tp&&>(__x); }
    };

  /**
   *  Maps member pointers into instances of _Mem_fn but leaves all
   *  other function objects untouched. Used by tr1::bind(). This
   *  partial specialization handles the member pointer case.
   */
  template<typename _Tp, typename _Class>
    struct _Maybe_wrap_member_pointer<_Tp _Class::*>
    {
      typedef _Mem_fn<_Tp _Class::*> type;

      static type
      __do_wrap(_Tp _Class::* __pm)
      { return type(__pm); }
    };

  // Specialization needed to prevent "forming reference to void" errors when
  // bind<void>() is called, because argument deduction instantiates
  // _Maybe_wrap_member_pointer<void> outside the immediate context where
  // SFINAE applies.
  template<>
    struct _Maybe_wrap_member_pointer<void>
    {
      typedef void type;
    };

  // std::get<I> for volatile-qualified tuples
  template<std::size_t _Ind, typename... _Tp>
    inline auto
    __volget(volatile tuple<_Tp...>& __tuple)
    -> typename tuple_element<_Ind, tuple<_Tp...>>::type volatile&
    { return std::get<_Ind>(const_cast<tuple<_Tp...>&>(__tuple)); }

  // std::get<I> for const-volatile-qualified tuples
  template<std::size_t _Ind, typename... _Tp>
    inline auto
    __volget(const volatile tuple<_Tp...>& __tuple)
    -> typename tuple_element<_Ind, tuple<_Tp...>>::type const volatile&
    { return std::get<_Ind>(const_cast<const tuple<_Tp...>&>(__tuple)); }

  /// Type of the function object returned from bind().
  template<typename _Signature>
    struct _Bind;

   template<typename _Functor, typename... _Bound_args>
    class _Bind<_Functor(_Bound_args...)>
    : public _Weak_result_type<_Functor>
    {
      typedef _Bind __self_type;
      typedef typename _Build_index_tuple<sizeof...(_Bound_args)>::__type
	_Bound_indexes;

      _Functor _M_f;
      tuple<_Bound_args...> _M_bound_args;

      // Call unqualified
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>)
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as const
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call_c(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>) const
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as volatile
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call_v(tuple<_Args...>&& __args,
		 _Index_tuple<_Indexes...>) volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as const volatile
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call_c_v(tuple<_Args...>&& __args,
		   _Index_tuple<_Indexes...>) const volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

     public:
      template<typename... _Args>
	explicit _Bind(const _Functor& __f, _Args&&... __args)
	: _M_f(__f), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      template<typename... _Args>
	explicit _Bind(_Functor&& __f, _Args&&... __args)
	: _M_f(std::move(__f)), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      _Bind(const _Bind&) = default;

      _Bind(_Bind&& __b)
      : _M_f(std::move(__b._M_f)), _M_bound_args(std::move(__b._M_bound_args))
      { }

      // Call unqualified
      template<typename... _Args, typename _Result
	= decltype( std::declval<_Functor>()(
	      _Mu<_Bound_args>()( std::declval<_Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args)
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      // Call as const
      template<typename... _Args, typename _Result
	= decltype( std::declval<typename enable_if<(sizeof...(_Args) >= 0),
		       typename add_const<_Functor>::type>::type>()(
	      _Mu<_Bound_args>()( std::declval<const _Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args) const
	{
	  return this->__call_c<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      // Call as volatile
      template<typename... _Args, typename _Result
	= decltype( std::declval<typename enable_if<(sizeof...(_Args) >= 0),
                       typename add_volatile<_Functor>::type>::type>()(
	      _Mu<_Bound_args>()( std::declval<volatile _Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args) volatile
	{
	  return this->__call_v<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      // Call as const volatile
      template<typename... _Args, typename _Result
	= decltype( std::declval<typename enable_if<(sizeof...(_Args) >= 0),
                       typename add_cv<_Functor>::type>::type>()(
	      _Mu<_Bound_args>()( std::declval<const volatile _Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args) const volatile
	{
	  return this->__call_c_v<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}
    };

  /// Type of the function object returned from bind<R>().
  template<typename _Result, typename _Signature>
    struct _Bind_result;

  template<typename _Result, typename _Functor, typename... _Bound_args>
    class _Bind_result<_Result, _Functor(_Bound_args...)>
    {
      typedef _Bind_result __self_type;
      typedef typename _Build_index_tuple<sizeof...(_Bound_args)>::__type
	_Bound_indexes;

      _Functor _M_f;
      tuple<_Bound_args...> _M_bound_args;

      // sfinae types
      template<typename _Res>
	struct __enable_if_void : enable_if<is_void<_Res>::value, int> { };
      template<typename _Res>
	struct __disable_if_void : enable_if<!is_void<_Res>::value, int> { };

      // Call unqualified
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0)
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      // Call unqualified, return void
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0)
	{
	  _M_f(_Mu<_Bound_args>()
	       (get<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as const
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0) const
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as const, return void
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0) const
	{
	  _M_f(_Mu<_Bound_args>()
	       (get<_Indexes>(_M_bound_args),  __args)...);
	}

      // Call as volatile
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0) volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as volatile, return void
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0) volatile
	{
	  _M_f(_Mu<_Bound_args>()
	       (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as const volatile
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0) const volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      // Call as const volatile, return void
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args,
	       _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0) const volatile
	{
	  _M_f(_Mu<_Bound_args>()
	       (__volget<_Indexes>(_M_bound_args), __args)...);
	}

    public:
      typedef _Result result_type;

      template<typename... _Args>
	explicit _Bind_result(const _Functor& __f, _Args&&... __args)
	: _M_f(__f), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      template<typename... _Args>
	explicit _Bind_result(_Functor&& __f, _Args&&... __args)
	: _M_f(std::move(__f)), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      _Bind_result(const _Bind_result&) = default;

      _Bind_result(_Bind_result&& __b)
      : _M_f(std::move(__b._M_f)), _M_bound_args(std::move(__b._M_bound_args))
      { }

      // Call unqualified
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args)
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      // Call as const
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args) const
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      // Call as volatile
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args) volatile
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      // Call as const volatile
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args) const volatile
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}
    };

  /**
   *  @brief Class template _Bind is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Signature>
    struct is_bind_expression<_Bind<_Signature> >
    : public true_type { };

  /**
   *  @brief Class template _Bind is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Signature>
    struct is_bind_expression<const _Bind<_Signature> >
    : public true_type { };

  /**
   *  @brief Class template _Bind is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Signature>
    struct is_bind_expression<volatile _Bind<_Signature> >
    : public true_type { };

  /**
   *  @brief Class template _Bind is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Signature>
    struct is_bind_expression<const volatile _Bind<_Signature>>
    : public true_type { };

  /**
   *  @brief Class template _Bind_result is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Result, typename _Signature>
    struct is_bind_expression<_Bind_result<_Result, _Signature>>
    : public true_type { };

  /**
   *  @brief Class template _Bind_result is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Result, typename _Signature>
    struct is_bind_expression<const _Bind_result<_Result, _Signature>>
    : public true_type { };

  /**
   *  @brief Class template _Bind_result is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Result, typename _Signature>
    struct is_bind_expression<volatile _Bind_result<_Result, _Signature>>
    : public true_type { };

  /**
   *  @brief Class template _Bind_result is always a bind expression.
   *  @ingroup binders
   */
  template<typename _Result, typename _Signature>
    struct is_bind_expression<const volatile _Bind_result<_Result, _Signature>>
    : public true_type { };

  // Trait type used to remove std::bind() from overload set via SFINAE
  // when first argument has integer type, so that std::bind() will
  // not be a better match than ::bind() from the BSD Sockets API.
  template<typename _Tp, typename _Tp2 = typename decay<_Tp>::type>
    using __is_socketlike = __or_<is_integral<_Tp2>, is_enum<_Tp2>>;

  template<bool _SocketLike, typename _Func, typename... _BoundArgs>
    struct _Bind_helper
    {
      typedef _Maybe_wrap_member_pointer<typename decay<_Func>::type>
	__maybe_type;
      typedef typename __maybe_type::type __func_type;
      typedef _Bind<__func_type(typename decay<_BoundArgs>::type...)> type;
    };

  // Partial specialization for is_socketlike == true, does not define
  // nested type so std::bind() will not participate in overload resolution
  // when the first argument might be a socket file descriptor.
  template<typename _Func, typename... _BoundArgs>
    struct _Bind_helper<true, _Func, _BoundArgs...>
    { };

  /**
   *  @brief Function template for std::bind.
   *  @ingroup binders
   */
  template<typename _Func, typename... _BoundArgs>
    inline typename
    _Bind_helper<__is_socketlike<_Func>::value, _Func, _BoundArgs...>::type
    bind(_Func&& __f, _BoundArgs&&... __args)
    {
      typedef _Bind_helper<false, _Func, _BoundArgs...> __helper_type;
      typedef typename __helper_type::__maybe_type __maybe_type;
      typedef typename __helper_type::type __result_type;
      return __result_type(__maybe_type::__do_wrap(std::forward<_Func>(__f)),
			   std::forward<_BoundArgs>(__args)...);
    }

  template<typename _Result, typename _Func, typename... _BoundArgs>
    struct _Bindres_helper
    {
      typedef _Maybe_wrap_member_pointer<typename decay<_Func>::type>
	__maybe_type;
      typedef typename __maybe_type::type __functor_type;
      typedef _Bind_result<_Result,
			   __functor_type(typename decay<_BoundArgs>::type...)>
	type;
    };

  /**
   *  @brief Function template for std::bind<R>.
   *  @ingroup binders
   */
  template<typename _Result, typename _Func, typename... _BoundArgs>
    inline
    typename _Bindres_helper<_Result, _Func, _BoundArgs...>::type
    bind(_Func&& __f, _BoundArgs&&... __args)
    {
      typedef _Bindres_helper<_Result, _Func, _BoundArgs...> __helper_type;
      typedef typename __helper_type::__maybe_type __maybe_type;
      typedef typename __helper_type::type __result_type;
      return __result_type(__maybe_type::__do_wrap(std::forward<_Func>(__f)),
			   std::forward<_BoundArgs>(__args)...);
    }

  template<typename _Signature>
    struct _Bind_simple;

  template<typename _Callable, typename... _Args>
    struct _Bind_simple<_Callable(_Args...)>
    {
      typedef typename result_of<_Callable(_Args...)>::type result_type;

      template<typename... _Args2, typename = typename
               enable_if< sizeof...(_Args) == sizeof...(_Args2)>::type>
        explicit
        _Bind_simple(const _Callable& __callable, _Args2&&... __args)
        : _M_bound(__callable, std::forward<_Args2>(__args)...)
        { }

      template<typename... _Args2, typename = typename
               enable_if< sizeof...(_Args) == sizeof...(_Args2)>::type>
        explicit
        _Bind_simple(_Callable&& __callable, _Args2&&... __args)
        : _M_bound(std::move(__callable), std::forward<_Args2>(__args)...)
        { }

      _Bind_simple(const _Bind_simple&) = default;
      _Bind_simple(_Bind_simple&&) = default;

      result_type
      operator()()
      {
        typedef typename _Build_index_tuple<sizeof...(_Args)>::__type _Indices;
        return _M_invoke(_Indices());
      }

    private:

      template<std::size_t... _Indices>
        typename result_of<_Callable(_Args...)>::type
        _M_invoke(_Index_tuple<_Indices...>)
        {
	  // std::bind always forwards bound arguments as lvalues,
	  // but this type can call functions which only accept rvalues.
          return std::forward<_Callable>(std::get<0>(_M_bound))(
              std::forward<_Args>(std::get<_Indices+1>(_M_bound))...);
        }

      std::tuple<_Callable, _Args...> _M_bound;
    };

  template<typename _Func, typename... _BoundArgs>
    struct _Bind_simple_helper
    {
      typedef _Maybe_wrap_member_pointer<typename decay<_Func>::type>
        __maybe_type;
      typedef typename __maybe_type::type __func_type;
      typedef _Bind_simple<__func_type(typename decay<_BoundArgs>::type...)>
       	__type;
    };

  // Simplified version of std::bind for internal use, without support for
  // unbound arguments, placeholders or nested bind expressions.
  template<typename _Callable, typename... _Args>
    typename _Bind_simple_helper<_Callable, _Args...>::__type
    __bind_simple(_Callable&& __callable, _Args&&... __args)
    {
      typedef _Bind_simple_helper<_Callable, _Args...> __helper_type;
      typedef typename __helper_type::__maybe_type __maybe_type;
      typedef typename __helper_type::__type __result_type;
      return __result_type(
          __maybe_type::__do_wrap( std::forward<_Callable>(__callable)),
          std::forward<_Args>(__args)...);
    }

  /**
   *  @brief Exception class thrown when class template function's
   *  operator() is called with an empty target.
   *  @ingroup exceptions
   */
  class bad_function_call : public std::exception
  {
  public:
    virtual ~bad_function_call() noexcept;

    const char* what() const noexcept;
  };

  /**
   *  Trait identifying "location-invariant" types, meaning that the
   *  address of the object (or any of its members) will not escape.
   *  Also implies a trivial copy constructor and assignment operator.
   */
  template<typename _Tp>
    struct __is_location_invariant
    : integral_constant<bool, (is_pointer<_Tp>::value
			       || is_member_pointer<_Tp>::value)>
    { };

  class _Undefined_class;

  union _Nocopy_types
  {
    void*       _M_object;
    const void* _M_const_object;
    void (*_M_function_pointer)();
    void (_Undefined_class::*_M_member_pointer)();
  };

  union _Any_data
  {
    void*       _M_access()       { return &_M_pod_data[0]; }
    const void* _M_access() const { return &_M_pod_data[0]; }

    template<typename _Tp>
      _Tp&
      _M_access()
      { return *static_cast<_Tp*>(_M_access()); }

    template<typename _Tp>
      const _Tp&
      _M_access() const
      { return *static_cast<const _Tp*>(_M_access()); }

    _Nocopy_types _M_unused;
    char _M_pod_data[sizeof(_Nocopy_types)];
  };

  enum _Manager_operation
  {
    __get_type_info,
    __get_functor_ptr,
    __clone_functor,
    __destroy_functor
  };

  // Simple type wrapper that helps avoid annoying const problems
  // when casting between void pointers and pointers-to-pointers.
  template<typename _Tp>
    struct _Simple_type_wrapper
    {
      _Simple_type_wrapper(_Tp __value) : __value(__value) { }

      _Tp __value;
    };

  template<typename _Tp>
    struct __is_location_invariant<_Simple_type_wrapper<_Tp> >
    : __is_location_invariant<_Tp>
    { };

  // Converts a reference to a function object into a callable
  // function object.
  template<typename _Functor>
    inline _Functor&
    __callable_functor(_Functor& __f)
    { return __f; }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* &__p)
    { return std::mem_fn(__p); }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* const &__p)
    { return std::mem_fn(__p); }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* volatile &__p)
    { return std::mem_fn(__p); }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* const volatile &__p)
    { return std::mem_fn(__p); }

  template<typename _Signature>
    class function;

  /// Base class of all polymorphic function object wrappers.
  class _Function_base
  {
  public:
    static const std::size_t _M_max_size = sizeof(_Nocopy_types);
    static const std::size_t _M_max_align = __alignof__(_Nocopy_types);

    template<typename _Functor>
      class _Base_manager
      {
      protected:
	static const bool __stored_locally =
	(__is_location_invariant<_Functor>::value
	 && sizeof(_Functor) <= _M_max_size
	 && __alignof__(_Functor) <= _M_max_align
	 && (_M_max_align % __alignof__(_Functor) == 0));

	typedef integral_constant<bool, __stored_locally> _Local_storage;

	// Retrieve a pointer to the function object
	static _Functor*
	_M_get_pointer(const _Any_data& __source)
	{
	  const _Functor* __ptr =
	    __stored_locally? std::__addressof(__source._M_access<_Functor>())
	    /* have stored a pointer */ : __source._M_access<_Functor*>();
	  return const_cast<_Functor*>(__ptr);
	}

	// Clone a location-invariant function object that fits within
	// an _Any_data structure.
	static void
	_M_clone(_Any_data& __dest, const _Any_data& __source, true_type)
	{
	  new (__dest._M_access()) _Functor(__source._M_access<_Functor>());
	}

	// Clone a function object that is not location-invariant or
	// that cannot fit into an _Any_data structure.
	static void
	_M_clone(_Any_data& __dest, const _Any_data& __source, false_type)
	{
	  __dest._M_access<_Functor*>() =
	    new _Functor(*__source._M_access<_Functor*>());
	}

	// Destroying a location-invariant object may still require
	// destruction.
	static void
	_M_destroy(_Any_data& __victim, true_type)
	{
	  __victim._M_access<_Functor>().~_Functor();
	}

	// Destroying an object located on the heap.
	static void
	_M_destroy(_Any_data& __victim, false_type)
	{
	  delete __victim._M_access<_Functor*>();
	}

      public:
	static bool
	_M_manager(_Any_data& __dest, const _Any_data& __source,
		   _Manager_operation __op)
	{
	  switch (__op)
	    {
#ifdef __GXX_RTTI
	    case __get_type_info:
	      __dest._M_access<const type_info*>() = &typeid(_Functor);
	      break;
#endif
	    case __get_functor_ptr:
	      __dest._M_access<_Functor*>() = _M_get_pointer(__source);
	      break;

	    case __clone_functor:
	      _M_clone(__dest, __source, _Local_storage());
	      break;

	    case __destroy_functor:
	      _M_destroy(__dest, _Local_storage());
	      break;
	    }
	  return false;
	}

	static void
	_M_init_functor(_Any_data& __functor, _Functor&& __f)
	{ _M_init_functor(__functor, std::move(__f), _Local_storage()); }

	template<typename _Signature>
	  static bool
	  _M_not_empty_function(const function<_Signature>& __f)
	  { return static_cast<bool>(__f); }

	template<typename _Tp>
	  static bool
	  _M_not_empty_function(_Tp* const& __fp)
	  { return __fp; }

	template<typename _Class, typename _Tp>
	  static bool
	  _M_not_empty_function(_Tp _Class::* const& __mp)
	  { return __mp; }

	template<typename _Tp>
	  static bool
	  _M_not_empty_function(const _Tp&)
	  { return true; }

      private:
	static void
	_M_init_functor(_Any_data& __functor, _Functor&& __f, true_type)
	{ new (__functor._M_access()) _Functor(std::move(__f)); }

	static void
	_M_init_functor(_Any_data& __functor, _Functor&& __f, false_type)
	{ __functor._M_access<_Functor*>() = new _Functor(std::move(__f)); }
      };

    template<typename _Functor>
      class _Ref_manager : public _Base_manager<_Functor*>
      {
	typedef _Function_base::_Base_manager<_Functor*> _Base;

      public:
	static bool
	_M_manager(_Any_data& __dest, const _Any_data& __source,
		   _Manager_operation __op)
	{
	  switch (__op)
	    {
#ifdef __GXX_RTTI
	    case __get_type_info:
	      __dest._M_access<const type_info*>() = &typeid(_Functor);
	      break;
#endif
	    case __get_functor_ptr:
	      __dest._M_access<_Functor*>() = *_Base::_M_get_pointer(__source);
	      return is_const<_Functor>::value;
	      break;

	    default:
	      _Base::_M_manager(__dest, __source, __op);
	    }
	  return false;
	}

	static void
	_M_init_functor(_Any_data& __functor, reference_wrapper<_Functor> __f)
	{
	  _Base::_M_init_functor(__functor, std::__addressof(__f.get()));
	}
      };

    _Function_base() : _M_manager(0) { }

    ~_Function_base()
    {
      if (_M_manager)
	_M_manager(_M_functor, _M_functor, __destroy_functor);
    }


    bool _M_empty() const { return !_M_manager; }

    typedef bool (*_Manager_type)(_Any_data&, const _Any_data&,
				  _Manager_operation);

    _Any_data     _M_functor;
    _Manager_type _M_manager;
  };

  template<typename _Signature, typename _Functor>
    class _Function_handler;

  template<typename _Res, typename _Functor, typename... _ArgTypes>
    class _Function_handler<_Res(_ArgTypes...), _Functor>
    : public _Function_base::_Base_manager<_Functor>
    {
      typedef _Function_base::_Base_manager<_Functor> _Base;

    public:
      static _Res
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	return (*_Base::_M_get_pointer(__functor))(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Functor, typename... _ArgTypes>
    class _Function_handler<void(_ArgTypes...), _Functor>
    : public _Function_base::_Base_manager<_Functor>
    {
      typedef _Function_base::_Base_manager<_Functor> _Base;

     public:
      static void
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	(*_Base::_M_get_pointer(__functor))(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Res, typename _Functor, typename... _ArgTypes>
    class _Function_handler<_Res(_ArgTypes...), reference_wrapper<_Functor> >
    : public _Function_base::_Ref_manager<_Functor>
    {
      typedef _Function_base::_Ref_manager<_Functor> _Base;

     public:
      static _Res
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	return __callable_functor(**_Base::_M_get_pointer(__functor))(
	      std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Functor, typename... _ArgTypes>
    class _Function_handler<void(_ArgTypes...), reference_wrapper<_Functor> >
    : public _Function_base::_Ref_manager<_Functor>
    {
      typedef _Function_base::_Ref_manager<_Functor> _Base;

     public:
      static void
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	__callable_functor(**_Base::_M_get_pointer(__functor))(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Class, typename _Member, typename _Res,
	   typename... _ArgTypes>
    class _Function_handler<_Res(_ArgTypes...), _Member _Class::*>
    : public _Function_handler<void(_ArgTypes...), _Member _Class::*>
    {
      typedef _Function_handler<void(_ArgTypes...), _Member _Class::*>
	_Base;

     public:
      static _Res
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	return std::mem_fn(_Base::_M_get_pointer(__functor)->__value)(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Class, typename _Member, typename... _ArgTypes>
    class _Function_handler<void(_ArgTypes...), _Member _Class::*>
    : public _Function_base::_Base_manager<
		 _Simple_type_wrapper< _Member _Class::* > >
    {
      typedef _Member _Class::* _Functor;
      typedef _Simple_type_wrapper<_Functor> _Wrapper;
      typedef _Function_base::_Base_manager<_Wrapper> _Base;

    public:
      static bool
      _M_manager(_Any_data& __dest, const _Any_data& __source,
		 _Manager_operation __op)
      {
	switch (__op)
	  {
#ifdef __GXX_RTTI
	  case __get_type_info:
	    __dest._M_access<const type_info*>() = &typeid(_Functor);
	    break;
#endif
	  case __get_functor_ptr:
	    __dest._M_access<_Functor*>() =
	      &_Base::_M_get_pointer(__source)->__value;
	    break;

	  default:
	    _Base::_M_manager(__dest, __source, __op);
	  }
	return false;
      }

      static void
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	std::mem_fn(_Base::_M_get_pointer(__functor)->__value)(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _From, typename _To>
    using __check_func_return_type
      = __or_<is_void<_To>, is_convertible<_From, _To>>;

  /**
   *  @brief Primary class template for std::function.
   *  @ingroup functors
   *
   *  Polymorphic function wrapper.
   */
  template<typename _Res, typename... _ArgTypes>
    class function<_Res(_ArgTypes...)>
    : public _Maybe_unary_or_binary_function<_Res, _ArgTypes...>,
      private _Function_base
    {
      typedef _Res _Signature_type(_ArgTypes...);

      template<typename _Functor>
	using _Invoke = decltype(__callable_functor(std::declval<_Functor&>())
				 (std::declval<_ArgTypes>()...) );

      // Used so the return type convertibility checks aren't done when
      // performing overload resolution for copy construction/assignment.
      template<typename _Tp>
	using _NotSelf = __not_<is_same<_Tp, function>>;

      template<typename _Functor>
	using _Callable
	  = __and_<_NotSelf<_Functor>,
		   __check_func_return_type<_Invoke<_Functor>, _Res>>;

      template<typename _Cond, typename _Tp>
	using _Requires = typename enable_if<_Cond::value, _Tp>::type;

    public:
      typedef _Res result_type;

      // [3.7.2.1] construct/copy/destroy

      /**
       *  @brief Default construct creates an empty function call wrapper.
       *  @post @c !(bool)*this
       */
      function() noexcept
      : _Function_base() { }

      /**
       *  @brief Creates an empty function call wrapper.
       *  @post @c !(bool)*this
       */
      function(nullptr_t) noexcept
      : _Function_base() { }

      /**
       *  @brief %Function copy constructor.
       *  @param __x A %function object with identical call signature.
       *  @post @c bool(*this) == bool(__x)
       *
       *  The newly-created %function contains a copy of the target of @a
       *  __x (if it has one).
       */
      function(const function& __x);

      /**
       *  @brief %Function move constructor.
       *  @param __x A %function object rvalue with identical call signature.
       *
       *  The newly-created %function contains the target of @a __x
       *  (if it has one).
       */
      function(function&& __x) : _Function_base()
      {
	__x.swap(*this);
      }

      // TODO: needs allocator_arg_t

      /**
       *  @brief Builds a %function that targets a copy of the incoming
       *  function object.
       *  @param __f A %function object that is callable with parameters of
       *  type @c T1, @c T2, ..., @c TN and returns a value convertible
       *  to @c Res.
       *
       *  The newly-created %function object will target a copy of 
       *  @a __f. If @a __f is @c reference_wrapper<F>, then this function
       *  object will contain a reference to the function object @c
       *  __f.get(). If @a __f is a NULL function pointer or NULL
       *  pointer-to-member, the newly-created object will be empty.
       *
       *  If @a __f is a non-NULL function pointer or an object of type @c
       *  reference_wrapper<F>, this function will not throw.
       */
      template<typename _Functor,
	       typename = _Requires<_Callable<_Functor>, void>>
	function(_Functor);

      /**
       *  @brief %Function assignment operator.
       *  @param __x A %function with identical call signature.
       *  @post @c (bool)*this == (bool)x
       *  @returns @c *this
       *
       *  The target of @a __x is copied to @c *this. If @a __x has no
       *  target, then @c *this will be empty.
       *
       *  If @a __x targets a function pointer or a reference to a function
       *  object, then this operation will not throw an %exception.
       */
      function&
      operator=(const function& __x)
      {
	function(__x).swap(*this);
	return *this;
      }

      /**
       *  @brief %Function move-assignment operator.
       *  @param __x A %function rvalue with identical call signature.
       *  @returns @c *this
       *
       *  The target of @a __x is moved to @c *this. If @a __x has no
       *  target, then @c *this will be empty.
       *
       *  If @a __x targets a function pointer or a reference to a function
       *  object, then this operation will not throw an %exception.
       */
      function&
      operator=(function&& __x)
      {
	function(std::move(__x)).swap(*this);
	return *this;
      }

      /**
       *  @brief %Function assignment to zero.
       *  @post @c !(bool)*this
       *  @returns @c *this
       *
       *  The target of @c *this is deallocated, leaving it empty.
       */
      function&
      operator=(nullptr_t)
      {
	if (_M_manager)
	  {
	    _M_manager(_M_functor, _M_functor, __destroy_functor);
	    _M_manager = 0;
	    _M_invoker = 0;
	  }
	return *this;
      }

      /**
       *  @brief %Function assignment to a new target.
       *  @param __f A %function object that is callable with parameters of
       *  type @c T1, @c T2, ..., @c TN and returns a value convertible
       *  to @c Res.
       *  @return @c *this
       *
       *  This  %function object wrapper will target a copy of @a
       *  __f. If @a __f is @c reference_wrapper<F>, then this function
       *  object will contain a reference to the function object @c
       *  __f.get(). If @a __f is a NULL function pointer or NULL
       *  pointer-to-member, @c this object will be empty.
       *
       *  If @a __f is a non-NULL function pointer or an object of type @c
       *  reference_wrapper<F>, this function will not throw.
       */
      template<typename _Functor>
	_Requires<_Callable<typename decay<_Functor>::type>, function&>
	operator=(_Functor&& __f)
	{
	  function(std::forward<_Functor>(__f)).swap(*this);
	  return *this;
	}

      /// @overload
      template<typename _Functor>
	function&
	operator=(reference_wrapper<_Functor> __f) noexcept
	{
	  function(__f).swap(*this);
	  return *this;
	}

      // [3.7.2.2] function modifiers

      /**
       *  @brief Swap the targets of two %function objects.
       *  @param __x A %function with identical call signature.
       *
       *  Swap the targets of @c this function object and @a __f. This
       *  function will not throw an %exception.
       */
      void swap(function& __x)
      {
	std::swap(_M_functor, __x._M_functor);
	std::swap(_M_manager, __x._M_manager);
	std::swap(_M_invoker, __x._M_invoker);
      }

      // TODO: needs allocator_arg_t
      /*
      template<typename _Functor, typename _Alloc>
	void
	assign(_Functor&& __f, const _Alloc& __a)
	{
	  function(allocator_arg, __a,
		   std::forward<_Functor>(__f)).swap(*this);
	}
      */

      // [3.7.2.3] function capacity

      /**
       *  @brief Determine if the %function wrapper has a target.
       *
       *  @return @c true when this %function object contains a target,
       *  or @c false when it is empty.
       *
       *  This function will not throw an %exception.
       */
      explicit operator bool() const noexcept
      { return !_M_empty(); }

      // [3.7.2.4] function invocation

      /**
       *  @brief Invokes the function targeted by @c *this.
       *  @returns the result of the target.
       *  @throws bad_function_call when @c !(bool)*this
       *
       *  The function call operator invokes the target function object
       *  stored by @c this.
       */
      _Res operator()(_ArgTypes... __args) const;

#ifdef __GXX_RTTI
      // [3.7.2.5] function target access
      /**
       *  @brief Determine the type of the target of this function object
       *  wrapper.
       *
       *  @returns the type identifier of the target function object, or
       *  @c typeid(void) if @c !(bool)*this.
       *
       *  This function will not throw an %exception.
       */
      const type_info& target_type() const noexcept;

      /**
       *  @brief Access the stored target function object.
       *
       *  @return Returns a pointer to the stored target function object,
       *  if @c typeid(Functor).equals(target_type()); otherwise, a NULL
       *  pointer.
       *
       * This function will not throw an %exception.
       */
      template<typename _Functor>       _Functor* target() noexcept;

      /// @overload
      template<typename _Functor> const _Functor* target() const noexcept;
#endif

    private:
      typedef _Res (*_Invoker_type)(const _Any_data&, _ArgTypes...);
      _Invoker_type _M_invoker;
  };

  // Out-of-line member definitions.
  template<typename _Res, typename... _ArgTypes>
    function<_Res(_ArgTypes...)>::
    function(const function& __x)
    : _Function_base()
    {
      if (static_cast<bool>(__x))
	{
	  _M_invoker = __x._M_invoker;
	  _M_manager = __x._M_manager;
	  __x._M_manager(_M_functor, __x._M_functor, __clone_functor);
	}
    }

  template<typename _Res, typename... _ArgTypes>
    template<typename _Functor, typename>
      function<_Res(_ArgTypes...)>::
      function(_Functor __f)
      : _Function_base()
      {
	typedef _Function_handler<_Signature_type, _Functor> _My_handler;

	if (_My_handler::_M_not_empty_function(__f))
	  {
	    _My_handler::_M_init_functor(_M_functor, std::move(__f));
	    _M_invoker = &_My_handler::_M_invoke;
	    _M_manager = &_My_handler::_M_manager;
	  }
      }

  template<typename _Res, typename... _ArgTypes>
    _Res
    function<_Res(_ArgTypes...)>::
    operator()(_ArgTypes... __args) const
    {
      if (_M_empty())
	__throw_bad_function_call();
      return _M_invoker(_M_functor, std::forward<_ArgTypes>(__args)...);
    }

#ifdef __GXX_RTTI
  template<typename _Res, typename... _ArgTypes>
    const type_info&
    function<_Res(_ArgTypes...)>::
    target_type() const noexcept
    {
      if (_M_manager)
	{
	  _Any_data __typeinfo_result;
	  _M_manager(__typeinfo_result, _M_functor, __get_type_info);
	  return *__typeinfo_result._M_access<const type_info*>();
	}
      else
	return typeid(void);
    }

  template<typename _Res, typename... _ArgTypes>
    template<typename _Functor>
      _Functor*
      function<_Res(_ArgTypes...)>::
      target() noexcept
      {
	if (typeid(_Functor) == target_type() && _M_manager)
	  {
	    _Any_data __ptr;
	    if (_M_manager(__ptr, _M_functor, __get_functor_ptr)
		&& !is_const<_Functor>::value)
	      return 0;
	    else
	      return __ptr._M_access<_Functor*>();
	  }
	else
	  return 0;
      }

  template<typename _Res, typename... _ArgTypes>
    template<typename _Functor>
      const _Functor*
      function<_Res(_ArgTypes...)>::
      target() const noexcept
      {
	if (typeid(_Functor) == target_type() && _M_manager)
	  {
	    _Any_data __ptr;
	    _M_manager(__ptr, _M_functor, __get_functor_ptr);
	    return __ptr._M_access<const _Functor*>();
	  }
	else
	  return 0;
      }
#endif

  // [20.7.15.2.6] null pointer comparisons

  /**
   *  @brief Compares a polymorphic function object wrapper against 0
   *  (the NULL pointer).
   *  @returns @c true if the wrapper has no target, @c false otherwise
   *
   *  This function will not throw an %exception.
   */
  template<typename _Res, typename... _Args>
    inline bool
    operator==(const function<_Res(_Args...)>& __f, nullptr_t) noexcept
    { return !static_cast<bool>(__f); }

  /// @overload
  template<typename _Res, typename... _Args>
    inline bool
    operator==(nullptr_t, const function<_Res(_Args...)>& __f) noexcept
    { return !static_cast<bool>(__f); }

  /**
   *  @brief Compares a polymorphic function object wrapper against 0
   *  (the NULL pointer).
   *  @returns @c false if the wrapper has no target, @c true otherwise
   *
   *  This function will not throw an %exception.
   */
  template<typename _Res, typename... _Args>
    inline bool
    operator!=(const function<_Res(_Args...)>& __f, nullptr_t) noexcept
    { return static_cast<bool>(__f); }

  /// @overload
  template<typename _Res, typename... _Args>
    inline bool
    operator!=(nullptr_t, const function<_Res(_Args...)>& __f) noexcept
    { return static_cast<bool>(__f); }

  // [20.7.15.2.7] specialized algorithms

  /**
   *  @brief Swap the targets of two polymorphic function object wrappers.
   *
   *  This function will not throw an %exception.
   */
  template<typename _Res, typename... _Args>
    inline void
    swap(function<_Res(_Args...)>& __x, function<_Res(_Args...)>& __y)
    { __x.swap(__y); }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif // C++11

#endif // _GLIBCXX_FUNCTIONAL

// <list> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/list
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_LIST
#define _GLIBCXX_LIST 1

#pragma GCC system_header

#include <bits/stl_algobase.h>
#include <bits/allocator.h>
#include <bits/range_access.h>
#include <bits/stl_list.h>
#include <bits/list.tcc>

#ifdef _GLIBCXX_DEBUG
# include <debug/list>
#endif

#ifdef _GLIBCXX_PROFILE
# include <profile/list>
#endif

#endif /* _GLIBCXX_LIST */


// <map> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/map
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_MAP
#define _GLIBCXX_MAP 1

#pragma GCC system_header

#include <bits/stl_tree.h>
#include <bits/stl_map.h>
#include <bits/stl_multimap.h>
#include <bits/range_access.h>

#ifdef _GLIBCXX_DEBUG
# include <debug/map>
#endif

#ifdef _GLIBCXX_PROFILE
# include <profile/map>
#endif

#endif /* _GLIBCXX_MAP */

// Iostreams base classes -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/ios
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.4  Iostreams base classes
//

#ifndef _GLIBCXX_IOS
#define _GLIBCXX_IOS 1

#pragma GCC system_header

#include <iosfwd>
#include <exception> 		// For ios_base::failure
#include <bits/char_traits.h> 	// For char_traits, streamoff, streamsize, fpos
#include <bits/localefwd.h>	// For class locale
#include <bits/ios_base.h>	// For ios_base declarations.
#include <streambuf> 
#include <bits/basic_ios.h>

#endif	/* _GLIBCXX_IOS */

// <iosfwd> Forward declarations -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/iosfwd
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.2  Forward declarations
//

#ifndef _GLIBCXX_IOSFWD
#define _GLIBCXX_IOSFWD 1

#pragma GCC system_header

#include <bits/c++config.h>
#include <bits/stringfwd.h> 	// For string forward declarations.
#include <bits/postypes.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  @defgroup io I/O
   *
   *  Nearly all of the I/O classes are parameterized on the type of
   *  characters they read and write.  (The major exception is ios_base at
   *  the top of the hierarchy.)  This is a change from pre-Standard
   *  streams, which were not templates.
   *
   *  For ease of use and compatibility, all of the basic_* I/O-related
   *  classes are given typedef names for both of the builtin character
   *  widths (wide and narrow).  The typedefs are the same as the
   *  pre-Standard names, for example:
   *
   *  @code
   *     typedef basic_ifstream<char>  ifstream;
   *  @endcode
   *
   *  Because properly forward-declaring these classes can be difficult, you
   *  should not do it yourself.  Instead, include the &lt;iosfwd&gt;
   *  header, which contains only declarations of all the I/O classes as
   *  well as the typedefs.  Trying to forward-declare the typedefs
   *  themselves (e.g., <code>class ostream;</code>) is not valid ISO C++.
   *
   *  For more specific declarations, see
   *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt11ch24.html
   *
   *  @{
  */
  class ios_base;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ios;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_streambuf;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_istream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ostream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_iostream;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	    typename _Alloc = allocator<_CharT> >
    class basic_stringbuf;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	   typename _Alloc = allocator<_CharT> >
    class basic_istringstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	   typename _Alloc = allocator<_CharT> >
    class basic_ostringstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	   typename _Alloc = allocator<_CharT> >
    class basic_stringstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_filebuf;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ifstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ofstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_fstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class istreambuf_iterator;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class ostreambuf_iterator;


  /// Base class for @c char streams.
  typedef basic_ios<char> 		ios; 

  /// Base class for @c char buffers.
  typedef basic_streambuf<char> 	streambuf;

  /// Base class for @c char input streams.
  typedef basic_istream<char> 		istream;

  /// Base class for @c char output streams.
  typedef basic_ostream<char> 		ostream;

  /// Base class for @c char mixed input and output streams.
  typedef basic_iostream<char> 		iostream;

  /// Class for @c char memory buffers.
  typedef basic_stringbuf<char> 	stringbuf;

  /// Class for @c char input memory streams.
  typedef basic_istringstream<char> 	istringstream;

  /// Class for @c char output memory streams.
  typedef basic_ostringstream<char> 	ostringstream;

  /// Class for @c char mixed input and output memory streams.
  typedef basic_stringstream<char> 	stringstream;

  /// Class for @c char file buffers.
  typedef basic_filebuf<char> 		filebuf;

  /// Class for @c char input file streams.
  typedef basic_ifstream<char> 		ifstream;

  /// Class for @c char output file streams.
  typedef basic_ofstream<char> 		ofstream;

  /// Class for @c char mixed input and output file streams.
  typedef basic_fstream<char> 		fstream;

#ifdef _GLIBCXX_USE_WCHAR_T
  /// Base class for @c wchar_t streams.
  typedef basic_ios<wchar_t> 		wios;

  /// Base class for @c wchar_t buffers.
  typedef basic_streambuf<wchar_t> 	wstreambuf;

  /// Base class for @c wchar_t input streams.
  typedef basic_istream<wchar_t> 	wistream;

  /// Base class for @c wchar_t output streams.
  typedef basic_ostream<wchar_t> 	wostream;

  /// Base class for @c wchar_t mixed input and output streams.
  typedef basic_iostream<wchar_t> 	wiostream;

  /// Class for @c wchar_t memory buffers.
  typedef basic_stringbuf<wchar_t> 	wstringbuf;

  /// Class for @c wchar_t input memory streams.
  typedef basic_istringstream<wchar_t> 	wistringstream;

  /// Class for @c wchar_t output memory streams.
  typedef basic_ostringstream<wchar_t> 	wostringstream;

  /// Class for @c wchar_t mixed input and output memory streams.
  typedef basic_stringstream<wchar_t> 	wstringstream;

  /// Class for @c wchar_t file buffers.
  typedef basic_filebuf<wchar_t> 	wfilebuf;

  /// Class for @c wchar_t input file streams.
  typedef basic_ifstream<wchar_t> 	wifstream;

  /// Class for @c wchar_t output file streams.
  typedef basic_ofstream<wchar_t> 	wofstream;

  /// Class for @c wchar_t mixed input and output file streams.
  typedef basic_fstream<wchar_t> 	wfstream;
#endif
  /** @}  */

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif /* _GLIBCXX_IOSFWD */

// Standard iostream objects -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/iostream
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.3  Standard iostream objects
//

#ifndef _GLIBCXX_IOSTREAM
#define _GLIBCXX_IOSTREAM 1

#pragma GCC system_header

#include <bits/c++config.h>
#include <ostream>
#include <istream>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   *  @name Standard Stream Objects
   *
   *  The &lt;iostream&gt; header declares the eight <em>standard stream
   *  objects</em>.  For other declarations, see
   *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/io.html
   *  and the @link iosfwd I/O forward declarations @endlink
   *
   *  They are required by default to cooperate with the global C
   *  library's @c FILE streams, and to be available during program
   *  startup and termination. For more information, see the section of the
   *  manual linked to above.
  */
  //@{
  extern istream cin;		/// Linked to standard input
  extern ostream cout;		/// Linked to standard output
  extern ostream cerr;		/// Linked to standard error (unbuffered)
  extern ostream clog;		/// Linked to standard error (buffered)

#ifdef _GLIBCXX_USE_WCHAR_T
  extern wistream wcin;		/// Linked to standard input
  extern wostream wcout;	/// Linked to standard output
  extern wostream wcerr;	/// Linked to standard error (unbuffered)
  extern wostream wclog;	/// Linked to standard error (buffered)
#endif
  //@}

  // For construction of filebuffers for cout, cin, cerr, clog et. al.
  static ios_base::Init __ioinit;

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif /* _GLIBCXX_IOSTREAM */

// <queue> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/queue
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_QUEUE
#define _GLIBCXX_QUEUE 1

#pragma GCC system_header

#include <deque>
#include <vector>
#include <bits/stl_heap.h>
#include <bits/stl_function.h>
#include <bits/stl_queue.h>

#endif /* _GLIBCXX_QUEUE */

// <set> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/set
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_SET
#define _GLIBCXX_SET 1

#pragma GCC system_header

#include <bits/stl_tree.h>
#include <bits/stl_set.h>
#include <bits/stl_multiset.h>
#include <bits/range_access.h>

#ifdef _GLIBCXX_DEBUG
# include <debug/set>
#endif

#ifdef _GLIBCXX_PROFILE
# include <profile/set>
#endif

#endif /* _GLIBCXX_SET */

// String based streams -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/sstream
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.7  String-based streams
//

#ifndef _GLIBCXX_SSTREAM
#define _GLIBCXX_SSTREAM 1

#pragma GCC system_header

#include <istream>
#include <ostream>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // [27.7.1] template class basic_stringbuf
  /**
   *  @brief  The actual work of input and output (for std::string).
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *  @tparam _Alloc  Allocator type, defaults to allocator<_CharT>.
   *
   *  This class associates either or both of its input and output sequences
   *  with a sequence of characters, which can be initialized from, or made
   *  available as, a @c std::basic_string.  (Paraphrased from [27.7.1]/1.)
   *
   *  For this class, open modes (of type @c ios_base::openmode) have
   *  @c in set if the input sequence can be read, and @c out set if the
   *  output sequence can be written.
  */
  template<typename _CharT, typename _Traits, typename _Alloc>
    class basic_stringbuf : public basic_streambuf<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 251. basic_stringbuf missing allocator_type
      typedef _Alloc				       	allocator_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      typedef basic_streambuf<char_type, traits_type>  	__streambuf_type;
      typedef basic_string<char_type, _Traits, _Alloc> 	__string_type;
      typedef typename __string_type::size_type		__size_type;

    protected:
      /// Place to stash in || out || in | out settings for current stringbuf.
      ios_base::openmode 	_M_mode;

      // Data Members:
      __string_type 		_M_string;

    public:
      // Constructors:
      /**
       *  @brief  Starts with an empty string buffer.
       *  @param  __mode  Whether the buffer can read, or write, or both.
       *
       *  The default constructor initializes the parent class using its
       *  own default ctor.
      */
      explicit
      basic_stringbuf(ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __streambuf_type(), _M_mode(__mode), _M_string()
      { }

      /**
       *  @brief  Starts with an existing string buffer.
       *  @param  __str  A string to copy as a starting buffer.
       *  @param  __mode  Whether the buffer can read, or write, or both.
       *
       *  This constructor initializes the parent class using its
       *  own default ctor.
      */
      explicit
      basic_stringbuf(const __string_type& __str,
		      ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __streambuf_type(), _M_mode(), _M_string(__str.data(), __str.size())
      { _M_stringbuf_init(__mode); }

      // Get and set:
      /**
       *  @brief  Copying out the string buffer.
       *  @return  A copy of one of the underlying sequences.
       *
       *  <em>If the buffer is only created in input mode, the underlying
       *  character sequence is equal to the input sequence; otherwise, it
       *  is equal to the output sequence.</em> [27.7.1.2]/1
      */
      __string_type
      str() const
      {
	__string_type __ret;
	if (this->pptr())
	  {
	    // The current egptr() may not be the actual string end.
	    if (this->pptr() > this->egptr())
	      __ret = __string_type(this->pbase(), this->pptr());
	    else
 	      __ret = __string_type(this->pbase(), this->egptr());
	  }
	else
	  __ret = _M_string;
	return __ret;
      }

      /**
       *  @brief  Setting a new buffer.
       *  @param  __s  The string to use as a new sequence.
       *
       *  Deallocates any previous stored sequence, then copies @a s to
       *  use as a new one.
      */
      void
      str(const __string_type& __s)
      {
	// Cannot use _M_string = __s, since v3 strings are COW.
	_M_string.assign(__s.data(), __s.size());
	_M_stringbuf_init(_M_mode);
      }

    protected:
      // Common initialization code goes here.
      void
      _M_stringbuf_init(ios_base::openmode __mode)
      {
	_M_mode = __mode;
	__size_type __len = 0;
	if (_M_mode & (ios_base::ate | ios_base::app))
	  __len = _M_string.size();
	_M_sync(const_cast<char_type*>(_M_string.data()), 0, __len);
      }

      virtual streamsize
      showmanyc()
      { 
	streamsize __ret = -1;
	if (_M_mode & ios_base::in)
	  {
	    _M_update_egptr();
	    __ret = this->egptr() - this->gptr();
	  }
	return __ret;
      }

      virtual int_type
      underflow();

      virtual int_type
      pbackfail(int_type __c = traits_type::eof());

      virtual int_type
      overflow(int_type __c = traits_type::eof());

      /**
       *  @brief  Manipulates the buffer.
       *  @param  __s  Pointer to a buffer area.
       *  @param  __n  Size of @a __s.
       *  @return  @c this
       *
       *  If no buffer has already been created, and both @a __s and @a __n are
       *  non-zero, then @c __s is used as a buffer; see
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt11ch25s02.html
       *  for more.
      */
      virtual __streambuf_type*
      setbuf(char_type* __s, streamsize __n)
      {
	if (__s && __n >= 0)
	  {
	    // This is implementation-defined behavior, and assumes
	    // that an external char_type array of length __n exists
	    // and has been pre-allocated. If this is not the case,
	    // things will quickly blow up.
	    
	    // Step 1: Destroy the current internal array.
	    _M_string.clear();
	    
	    // Step 2: Use the external array.
	    _M_sync(__s, __n, 0);
	  }
	return this;
      }

      virtual pos_type
      seekoff(off_type __off, ios_base::seekdir __way,
	      ios_base::openmode __mode = ios_base::in | ios_base::out);

      virtual pos_type
      seekpos(pos_type __sp,
	      ios_base::openmode __mode = ios_base::in | ios_base::out);

      // Internal function for correctly updating the internal buffer
      // for a particular _M_string, due to initialization or re-sizing
      // of an existing _M_string.
      void
      _M_sync(char_type* __base, __size_type __i, __size_type __o);

      // Internal function for correctly updating egptr() to the actual
      // string end.
      void
      _M_update_egptr()
      {
	const bool __testin = _M_mode & ios_base::in;
	if (this->pptr() && this->pptr() > this->egptr())
	  {
	    if (__testin)
	      this->setg(this->eback(), this->gptr(), this->pptr());
	    else
	      this->setg(this->pptr(), this->pptr(), this->pptr());
	  }
      }

      // Works around the issue with pbump, part of the protected
      // interface of basic_streambuf, taking just an int.
      void
      _M_pbump(char_type* __pbeg, char_type* __pend, off_type __off);
    };


  // [27.7.2] Template class basic_istringstream
  /**
   *  @brief  Controlling input for std::string.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *  @tparam _Alloc  Allocator type, defaults to allocator<_CharT>.
   *
   *  This class supports reading from objects of type std::basic_string,
   *  using the inherited functions from std::basic_istream.  To control
   *  the associated sequence, an instance of std::basic_stringbuf is used,
   *  which this page refers to as @c sb.
  */
  template<typename _CharT, typename _Traits, typename _Alloc>
    class basic_istringstream : public basic_istream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 251. basic_stringbuf missing allocator_type
      typedef _Alloc				       	allocator_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_string<_CharT, _Traits, _Alloc> 	__string_type;
      typedef basic_stringbuf<_CharT, _Traits, _Alloc> 	__stringbuf_type;
      typedef basic_istream<char_type, traits_type>	__istream_type;

    private:
      __stringbuf_type	_M_stringbuf;

    public:
      // Constructors:
      /**
       *  @brief  Default constructor starts with an empty string buffer.
       *  @param  __mode  Whether the buffer can read, or write, or both.
       *
       *  @c ios_base::in is automatically included in @a __mode.
       *
       *  Initializes @c sb using @c __mode|in, and passes @c &sb to the base
       *  class initializer.  Does not allocate any buffer.
       *
       *  That's a lie.  We initialize the base class with NULL, because the
       *  string class does its own memory management.
      */
      explicit
      basic_istringstream(ios_base::openmode __mode = ios_base::in)
      : __istream_type(), _M_stringbuf(__mode | ios_base::in)
      { this->init(&_M_stringbuf); }

      /**
       *  @brief  Starts with an existing string buffer.
       *  @param  __str  A string to copy as a starting buffer.
       *  @param  __mode  Whether the buffer can read, or write, or both.
       *
       *  @c ios_base::in is automatically included in @a mode.
       *
       *  Initializes @c sb using @a str and @c mode|in, and passes @c &sb
       *  to the base class initializer.
       *
       *  That's a lie.  We initialize the base class with NULL, because the
       *  string class does its own memory management.
      */
      explicit
      basic_istringstream(const __string_type& __str,
			  ios_base::openmode __mode = ios_base::in)
      : __istream_type(), _M_stringbuf(__str, __mode | ios_base::in)
      { this->init(&_M_stringbuf); }

      /**
       *  @brief  The destructor does nothing.
       *
       *  The buffer is deallocated by the stringbuf object, not the
       *  formatting stream.
      */
      ~basic_istringstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_stringbuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
      */
      __stringbuf_type*
      rdbuf() const
      { return const_cast<__stringbuf_type*>(&_M_stringbuf); }

      /**
       *  @brief  Copying out the string buffer.
       *  @return  @c rdbuf()->str()
      */
      __string_type
      str() const
      { return _M_stringbuf.str(); }

      /**
       *  @brief  Setting a new buffer.
       *  @param  __s  The string to use as a new sequence.
       *
       *  Calls @c rdbuf()->str(s).
      */
      void
      str(const __string_type& __s)
      { _M_stringbuf.str(__s); }
    };


  // [27.7.3] Template class basic_ostringstream
  /**
   *  @brief  Controlling output for std::string.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *  @tparam _Alloc  Allocator type, defaults to allocator<_CharT>.
   *
   *  This class supports writing to objects of type std::basic_string,
   *  using the inherited functions from std::basic_ostream.  To control
   *  the associated sequence, an instance of std::basic_stringbuf is used,
   *  which this page refers to as @c sb.
  */
  template <typename _CharT, typename _Traits, typename _Alloc>
    class basic_ostringstream : public basic_ostream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 251. basic_stringbuf missing allocator_type
      typedef _Alloc				       	allocator_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard types:
      typedef basic_string<_CharT, _Traits, _Alloc> 	__string_type;
      typedef basic_stringbuf<_CharT, _Traits, _Alloc> 	__stringbuf_type;
      typedef basic_ostream<char_type, traits_type>	__ostream_type;

    private:
      __stringbuf_type	_M_stringbuf;

    public:
      // Constructors/destructor:
      /**
       *  @brief  Default constructor starts with an empty string buffer.
       *  @param  __mode  Whether the buffer can read, or write, or both.
       *
       *  @c ios_base::out is automatically included in @a mode.
       *
       *  Initializes @c sb using @c mode|out, and passes @c &sb to the base
       *  class initializer.  Does not allocate any buffer.
       *
       *  That's a lie.  We initialize the base class with NULL, because the
       *  string class does its own memory management.
      */
      explicit
      basic_ostringstream(ios_base::openmode __mode = ios_base::out)
      : __ostream_type(), _M_stringbuf(__mode | ios_base::out)
      { this->init(&_M_stringbuf); }

      /**
       *  @brief  Starts with an existing string buffer.
       *  @param  __str  A string to copy as a starting buffer.
       *  @param  __mode  Whether the buffer can read, or write, or both.
       *
       *  @c ios_base::out is automatically included in @a mode.
       *
       *  Initializes @c sb using @a str and @c mode|out, and passes @c &sb
       *  to the base class initializer.
       *
       *  That's a lie.  We initialize the base class with NULL, because the
       *  string class does its own memory management.
      */
      explicit
      basic_ostringstream(const __string_type& __str,
			  ios_base::openmode __mode = ios_base::out)
      : __ostream_type(), _M_stringbuf(__str, __mode | ios_base::out)
      { this->init(&_M_stringbuf); }

      /**
       *  @brief  The destructor does nothing.
       *
       *  The buffer is deallocated by the stringbuf object, not the
       *  formatting stream.
      */
      ~basic_ostringstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_stringbuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
      */
      __stringbuf_type*
      rdbuf() const
      { return const_cast<__stringbuf_type*>(&_M_stringbuf); }

      /**
       *  @brief  Copying out the string buffer.
       *  @return  @c rdbuf()->str()
      */
      __string_type
      str() const
      { return _M_stringbuf.str(); }

      /**
       *  @brief  Setting a new buffer.
       *  @param  __s  The string to use as a new sequence.
       *
       *  Calls @c rdbuf()->str(s).
      */
      void
      str(const __string_type& __s)
      { _M_stringbuf.str(__s); }
    };


  // [27.7.4] Template class basic_stringstream
  /**
   *  @brief  Controlling input and output for std::string.
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *  @tparam _Alloc  Allocator type, defaults to allocator<_CharT>.
   *
   *  This class supports reading from and writing to objects of type
   *  std::basic_string, using the inherited functions from
   *  std::basic_iostream.  To control the associated sequence, an instance
   *  of std::basic_stringbuf is used, which this page refers to as @c sb.
  */
  template <typename _CharT, typename _Traits, typename _Alloc>
    class basic_stringstream : public basic_iostream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 251. basic_stringbuf missing allocator_type
      typedef _Alloc				       	allocator_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;

      // Non-standard Types:
      typedef basic_string<_CharT, _Traits, _Alloc> 	__string_type;
      typedef basic_stringbuf<_CharT, _Traits, _Alloc> 	__stringbuf_type;
      typedef basic_iostream<char_type, traits_type>	__iostream_type;

    private:
      __stringbuf_type	_M_stringbuf;

    public:
      // Constructors/destructors
      /**
       *  @brief  Default constructor starts with an empty string buffer.
       *  @param  __m  Whether the buffer can read, or write, or both.
       *
       *  Initializes @c sb using the mode from @c __m, and passes @c
       *  &sb to the base class initializer.  Does not allocate any
       *  buffer.
       *
       *  That's a lie.  We initialize the base class with NULL, because the
       *  string class does its own memory management.
      */
      explicit
      basic_stringstream(ios_base::openmode __m = ios_base::out | ios_base::in)
      : __iostream_type(), _M_stringbuf(__m)
      { this->init(&_M_stringbuf); }

      /**
       *  @brief  Starts with an existing string buffer.
       *  @param  __str  A string to copy as a starting buffer.
       *  @param  __m  Whether the buffer can read, or write, or both.
       *
       *  Initializes @c sb using @a __str and @c __m, and passes @c &sb
       *  to the base class initializer.
       *
       *  That's a lie.  We initialize the base class with NULL, because the
       *  string class does its own memory management.
      */
      explicit
      basic_stringstream(const __string_type& __str,
			 ios_base::openmode __m = ios_base::out | ios_base::in)
      : __iostream_type(), _M_stringbuf(__str, __m)
      { this->init(&_M_stringbuf); }

      /**
       *  @brief  The destructor does nothing.
       *
       *  The buffer is deallocated by the stringbuf object, not the
       *  formatting stream.
      */
      ~basic_stringstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_stringbuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
      */
      __stringbuf_type*
      rdbuf() const
      { return const_cast<__stringbuf_type*>(&_M_stringbuf); }

      /**
       *  @brief  Copying out the string buffer.
       *  @return  @c rdbuf()->str()
      */
      __string_type
      str() const
      { return _M_stringbuf.str(); }

      /**
       *  @brief  Setting a new buffer.
       *  @param  __s  The string to use as a new sequence.
       *
       *  Calls @c rdbuf()->str(s).
      */
      void
      str(const __string_type& __s)
      { _M_stringbuf.str(__s); }
    };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#include <bits/sstream.tcc>

#endif /* _GLIBCXX_SSTREAM */

// <stack> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/stack
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_STACK
#define _GLIBCXX_STACK 1

#pragma GCC system_header

#include <deque>
#include <bits/stl_stack.h>

#endif /* _GLIBCXX_STACK */

// Standard exception classes  -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/stdexcept
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 19.1  Exception classes
//

#ifndef _GLIBCXX_STDEXCEPT
#define _GLIBCXX_STDEXCEPT 1

#pragma GCC system_header

#include <exception>
#include <string>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @addtogroup exceptions
   * @{
   */

  /** Logic errors represent problems in the internal logic of a program;
   *  in theory, these are preventable, and even detectable before the
   *  program runs (e.g., violations of class invariants).
   *  @brief One of two subclasses of exception.
   */
  class logic_error : public exception 
  {
    string _M_msg;

  public:
    /** Takes a character string describing the error.  */
    explicit 
    logic_error(const string& __arg);

    virtual ~logic_error() _GLIBCXX_USE_NOEXCEPT;

    /** Returns a C-style character string describing the general cause of
     *  the current error (the same string passed to the ctor).  */
    virtual const char* 
    what() const _GLIBCXX_USE_NOEXCEPT;
  };

  /** Thrown by the library, or by you, to report domain errors (domain in
   *  the mathematical sense).  */
  class domain_error : public logic_error 
  {
  public:
    explicit domain_error(const string& __arg);
    virtual ~domain_error() _GLIBCXX_USE_NOEXCEPT;
  };

  /** Thrown to report invalid arguments to functions.  */
  class invalid_argument : public logic_error 
  {
  public:
    explicit invalid_argument(const string& __arg);
    virtual ~invalid_argument() _GLIBCXX_USE_NOEXCEPT;
  };

  /** Thrown when an object is constructed that would exceed its maximum
   *  permitted size (e.g., a basic_string instance).  */
  class length_error : public logic_error 
  {
  public:
    explicit length_error(const string& __arg);
    virtual ~length_error() _GLIBCXX_USE_NOEXCEPT;
  };

  /** This represents an argument whose value is not within the expected
   *  range (e.g., boundary checks in basic_string).  */
  class out_of_range : public logic_error 
  {
  public:
    explicit out_of_range(const string& __arg);
    virtual ~out_of_range() _GLIBCXX_USE_NOEXCEPT;
  };

  /** Runtime errors represent problems outside the scope of a program;
   *  they cannot be easily predicted and can generally only be caught as
   *  the program executes.
   *  @brief One of two subclasses of exception.
   */
  class runtime_error : public exception 
  {
    string _M_msg;

  public:
    /** Takes a character string describing the error.  */
    explicit 
    runtime_error(const string& __arg);

    virtual ~runtime_error() _GLIBCXX_USE_NOEXCEPT;

    /** Returns a C-style character string describing the general cause of
     *  the current error (the same string passed to the ctor).  */
    virtual const char* 
    what() const _GLIBCXX_USE_NOEXCEPT;
  };

  /** Thrown to indicate range errors in internal computations.  */
  class range_error : public runtime_error 
  {
  public:
    explicit range_error(const string& __arg);
    virtual ~range_error() _GLIBCXX_USE_NOEXCEPT;
  };

  /** Thrown to indicate arithmetic overflow.  */
  class overflow_error : public runtime_error 
  {
  public:
    explicit overflow_error(const string& __arg);
    virtual ~overflow_error() _GLIBCXX_USE_NOEXCEPT;
  };

  /** Thrown to indicate arithmetic underflow.  */
  class underflow_error : public runtime_error 
  {
  public:
    explicit underflow_error(const string& __arg);
    virtual ~underflow_error() _GLIBCXX_USE_NOEXCEPT;
  };

  // @} group exceptions

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif /* _GLIBCXX_STDEXCEPT */

// Stream buffer classes -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/streambuf
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 27.5  Stream buffers
//

#ifndef _GLIBXX_STREAMBUF
#define _GLIBXX_STREAMBUF 1

#pragma GCC system_header

#include <bits/c++config.h>
#include <iosfwd>
#include <bits/localefwd.h>
#include <bits/ios_base.h>
#include <bits/cpp_type_traits.h>
#include <ext/type_traits.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<typename _CharT, typename _Traits>
    streamsize
    __copy_streambufs_eof(basic_streambuf<_CharT, _Traits>*,
			  basic_streambuf<_CharT, _Traits>*, bool&);

  /**
   *  @brief  The actual work of input and output (interface).
   *  @ingroup io
   *
   *  @tparam _CharT  Type of character stream.
   *  @tparam _Traits  Traits for character type, defaults to
   *                   char_traits<_CharT>.
   *
   *  This is a base class.  Derived stream buffers each control a
   *  pair of character sequences:  one for input, and one for output.
   *
   *  Section [27.5.1] of the standard describes the requirements and
   *  behavior of stream buffer classes.  That section (three paragraphs)
   *  is reproduced here, for simplicity and accuracy.
   *
   *  -# Stream buffers can impose various constraints on the sequences
   *     they control.  Some constraints are:
   *     - The controlled input sequence can be not readable.
   *     - The controlled output sequence can be not writable.
   *     - The controlled sequences can be associated with the contents of
   *       other representations for character sequences, such as external
   *       files.
   *     - The controlled sequences can support operations @e directly to or
   *       from associated sequences.
   *     - The controlled sequences can impose limitations on how the
   *       program can read characters from a sequence, write characters to
   *       a sequence, put characters back into an input sequence, or alter
   *       the stream position.
   *     .
   *  -# Each sequence is characterized by three pointers which, if non-null,
   *     all point into the same @c charT array object.  The array object
   *     represents, at any moment, a (sub)sequence of characters from the
   *     sequence.  Operations performed on a sequence alter the values
   *     stored in these pointers, perform reads and writes directly to or
   *     from associated sequences, and alter <em>the stream position</em> and
   *     conversion state as needed to maintain this subsequence relationship.
   *     The three pointers are:
   *     - the <em>beginning pointer</em>, or lowest element address in the
   *       array (called @e xbeg here);
   *     - the <em>next pointer</em>, or next element address that is a
   *       current candidate for reading or writing (called @e xnext here);
   *     - the <em>end pointer</em>, or first element address beyond the
   *       end of the array (called @e xend here).
   *     .
   *  -# The following semantic constraints shall always apply for any set
   *     of three pointers for a sequence, using the pointer names given
   *     immediately above:
   *     - If @e xnext is not a null pointer, then @e xbeg and @e xend shall
   *       also be non-null pointers into the same @c charT array, as
   *       described above; otherwise, @e xbeg and @e xend shall also be null.
   *     - If @e xnext is not a null pointer and @e xnext < @e xend for an
   *       output sequence, then a <em>write position</em> is available.
   *       In this case, @e *xnext shall be assignable as the next element
   *       to write (to put, or to store a character value, into the sequence).
   *     - If @e xnext is not a null pointer and @e xbeg < @e xnext for an
   *       input sequence, then a <em>putback position</em> is available.
   *       In this case, @e xnext[-1] shall have a defined value and is the
   *       next (preceding) element to store a character that is put back
   *       into the input sequence.
   *     - If @e xnext is not a null pointer and @e xnext< @e xend for an
   *       input sequence, then a <em>read position</em> is available.
   *       In this case, @e *xnext shall have a defined value and is the
   *       next element to read (to get, or to obtain a character value,
   *       from the sequence).
  */
  template<typename _CharT, typename _Traits>
    class basic_streambuf 
    {
    public:
      //@{
      /**
       *  These are standard types.  They permit a standardized way of
       *  referring to names of (or names dependent on) the template
       *  parameters, which are specific to the implementation.
      */
      typedef _CharT 					char_type;
      typedef _Traits 					traits_type;
      typedef typename traits_type::int_type 		int_type;
      typedef typename traits_type::pos_type 		pos_type;
      typedef typename traits_type::off_type 		off_type;
      //@}

      //@{
      /// This is a non-standard type.
      typedef basic_streambuf<char_type, traits_type>  	__streambuf_type;
      //@}
      
      friend class basic_ios<char_type, traits_type>;
      friend class basic_istream<char_type, traits_type>;
      friend class basic_ostream<char_type, traits_type>;
      friend class istreambuf_iterator<char_type, traits_type>;
      friend class ostreambuf_iterator<char_type, traits_type>;

      friend streamsize
      __copy_streambufs_eof<>(basic_streambuf*, basic_streambuf*, bool&);

      template<bool _IsMove, typename _CharT2>
        friend typename __gnu_cxx::__enable_if<__is_char<_CharT2>::__value, 
					       _CharT2*>::__type
        __copy_move_a2(istreambuf_iterator<_CharT2>,
		       istreambuf_iterator<_CharT2>, _CharT2*);

      template<typename _CharT2>
        friend typename __gnu_cxx::__enable_if<__is_char<_CharT2>::__value,
				  istreambuf_iterator<_CharT2> >::__type
        find(istreambuf_iterator<_CharT2>, istreambuf_iterator<_CharT2>,
	     const _CharT2&);

      template<typename _CharT2, typename _Traits2>
        friend basic_istream<_CharT2, _Traits2>&
        operator>>(basic_istream<_CharT2, _Traits2>&, _CharT2*);

      template<typename _CharT2, typename _Traits2, typename _Alloc>
        friend basic_istream<_CharT2, _Traits2>&
        operator>>(basic_istream<_CharT2, _Traits2>&,
		   basic_string<_CharT2, _Traits2, _Alloc>&);

      template<typename _CharT2, typename _Traits2, typename _Alloc>
        friend basic_istream<_CharT2, _Traits2>&
        getline(basic_istream<_CharT2, _Traits2>&,
		basic_string<_CharT2, _Traits2, _Alloc>&, _CharT2);

    protected:
      /*
       *  This is based on _IO_FILE, just reordered to be more consistent,
       *  and is intended to be the most minimal abstraction for an
       *  internal buffer.
       *  -  get == input == read
       *  -  put == output == write
      */
      char_type* 		_M_in_beg;     ///< Start of get area.
      char_type* 		_M_in_cur;     ///< Current read area.
      char_type* 		_M_in_end;     ///< End of get area.
      char_type* 		_M_out_beg;    ///< Start of put area.
      char_type* 		_M_out_cur;    ///< Current put area.
      char_type* 		_M_out_end;    ///< End of put area.

      /// Current locale setting.
      locale 			_M_buf_locale;	

  public:
      /// Destructor deallocates no buffer space.
      virtual 
      ~basic_streambuf() 
      { }

      // [27.5.2.2.1] locales
      /**
       *  @brief  Entry point for imbue().
       *  @param  __loc  The new locale.
       *  @return  The previous locale.
       *
       *  Calls the derived imbue(__loc).
      */
      locale 
      pubimbue(const locale& __loc)
      {
	locale __tmp(this->getloc());
	this->imbue(__loc);
	_M_buf_locale = __loc;
	return __tmp;
      }

      /**
       *  @brief  Locale access.
       *  @return  The current locale in effect.
       *
       *  If pubimbue(loc) has been called, then the most recent @c loc
       *  is returned.  Otherwise the global locale in effect at the time
       *  of construction is returned.
      */
      locale   
      getloc() const
      { return _M_buf_locale; } 

      // [27.5.2.2.2] buffer management and positioning
      //@{
      /**
       *  @brief  Entry points for derived buffer functions.
       *
       *  The public versions of @c pubfoo dispatch to the protected
       *  derived @c foo member functions, passing the arguments (if any)
       *  and returning the result unchanged.
      */
      basic_streambuf*
      pubsetbuf(char_type* __s, streamsize __n) 
      { return this->setbuf(__s, __n); }

      /**
       *  @brief  Alters the stream position.
       *  @param  __off  Offset.
       *  @param  __way  Value for ios_base::seekdir.
       *  @param  __mode Value for ios_base::openmode.
       *
       *  Calls virtual seekoff function.
      */
      pos_type 
      pubseekoff(off_type __off, ios_base::seekdir __way, 
		 ios_base::openmode __mode = ios_base::in | ios_base::out)
      { return this->seekoff(__off, __way, __mode); }

      /**
       *  @brief  Alters the stream position.
       *  @param  __sp  Position
       *  @param  __mode Value for ios_base::openmode.
       *
       *  Calls virtual seekpos function.
      */
      pos_type 
      pubseekpos(pos_type __sp,
		 ios_base::openmode __mode = ios_base::in | ios_base::out)
      { return this->seekpos(__sp, __mode); }

      /**
       *  @brief  Calls virtual sync function.
      */
      int 
      pubsync() { return this->sync(); }
      //@}

      // [27.5.2.2.3] get area
      /**
       *  @brief  Looking ahead into the stream.
       *  @return  The number of characters available.
       *
       *  If a read position is available, returns the number of characters
       *  available for reading before the buffer must be refilled.
       *  Otherwise returns the derived @c showmanyc().
      */
      streamsize 
      in_avail() 
      { 
	const streamsize __ret = this->egptr() - this->gptr();
	return __ret ? __ret : this->showmanyc();
      }

      /**
       *  @brief  Getting the next character.
       *  @return  The next character, or eof.
       *
       *  Calls @c sbumpc(), and if that function returns
       *  @c traits::eof(), so does this function.  Otherwise, @c sgetc().
      */
      int_type 
      snextc()
      {
	int_type __ret = traits_type::eof();
	if (__builtin_expect(!traits_type::eq_int_type(this->sbumpc(), 
						       __ret), true))
	  __ret = this->sgetc();
	return __ret;
      }

      /**
       *  @brief  Getting the next character.
       *  @return  The next character, or eof.
       *
       *  If the input read position is available, returns that character
       *  and increments the read pointer, otherwise calls and returns
       *  @c uflow().
      */
      int_type 
      sbumpc()
      {
	int_type __ret;
	if (__builtin_expect(this->gptr() < this->egptr(), true))
	  {
	    __ret = traits_type::to_int_type(*this->gptr());
	    this->gbump(1);
	  }
	else 
	  __ret = this->uflow();
	return __ret;
      }

      /**
       *  @brief  Getting the next character.
       *  @return  The next character, or eof.
       *
       *  If the input read position is available, returns that character,
       *  otherwise calls and returns @c underflow().  Does not move the 
       *  read position after fetching the character.
      */
      int_type 
      sgetc()
      {
	int_type __ret;
	if (__builtin_expect(this->gptr() < this->egptr(), true))
	  __ret = traits_type::to_int_type(*this->gptr());
	else 
	  __ret = this->underflow();
	return __ret;
      }

      /**
       *  @brief  Entry point for xsgetn.
       *  @param  __s  A buffer area.
       *  @param  __n  A count.
       *
       *  Returns xsgetn(__s,__n).  The effect is to fill @a __s[0] through
       *  @a __s[__n-1] with characters from the input sequence, if possible.
      */
      streamsize 
      sgetn(char_type* __s, streamsize __n)
      { return this->xsgetn(__s, __n); }

      // [27.5.2.2.4] putback
      /**
       *  @brief  Pushing characters back into the input stream.
       *  @param  __c  The character to push back.
       *  @return  The previous character, if possible.
       *
       *  Similar to sungetc(), but @a __c is pushed onto the stream
       *  instead of <em>the previous character.</em> If successful,
       *  the next character fetched from the input stream will be @a
       *  __c.
      */
      int_type 
      sputbackc(char_type __c)
      {
	int_type __ret;
	const bool __testpos = this->eback() < this->gptr();
	if (__builtin_expect(!__testpos || 
			     !traits_type::eq(__c, this->gptr()[-1]), false))
	  __ret = this->pbackfail(traits_type::to_int_type(__c));
	else 
	  {
	    this->gbump(-1);
	    __ret = traits_type::to_int_type(*this->gptr());
	  }
	return __ret;
      }

      /**
       *  @brief  Moving backwards in the input stream.
       *  @return  The previous character, if possible.
       *
       *  If a putback position is available, this function decrements
       *  the input pointer and returns that character.  Otherwise,
       *  calls and returns pbackfail().  The effect is to @a unget
       *  the last character @a gotten.
      */
      int_type 
      sungetc()
      {
	int_type __ret;
	if (__builtin_expect(this->eback() < this->gptr(), true))
	  {
	    this->gbump(-1);
	    __ret = traits_type::to_int_type(*this->gptr());
	  }
	else 
	  __ret = this->pbackfail();
	return __ret;
      }

      // [27.5.2.2.5] put area
      /**
       *  @brief  Entry point for all single-character output functions.
       *  @param  __c  A character to output.
       *  @return  @a __c, if possible.
       *
       *  One of two public output functions.
       *
       *  If a write position is available for the output sequence (i.e.,
       *  the buffer is not full), stores @a __c in that position, increments
       *  the position, and returns @c traits::to_int_type(__c).  If a write
       *  position is not available, returns @c overflow(__c).
      */
      int_type 
      sputc(char_type __c)
      {
	int_type __ret;
	if (__builtin_expect(this->pptr() < this->epptr(), true))
	  {
	    *this->pptr() = __c;
	    this->pbump(1);
	    __ret = traits_type::to_int_type(__c);
	  }
	else
	  __ret = this->overflow(traits_type::to_int_type(__c));
	return __ret;
      }

      /**
       *  @brief  Entry point for all single-character output functions.
       *  @param  __s  A buffer read area.
       *  @param  __n  A count.
       *
       *  One of two public output functions.
       *
       *
       *  Returns xsputn(__s,__n).  The effect is to write @a __s[0] through
       *  @a __s[__n-1] to the output sequence, if possible.
      */
      streamsize 
      sputn(const char_type* __s, streamsize __n)
      { return this->xsputn(__s, __n); }

    protected:
      /**
       *  @brief  Base constructor.
       *
       *  Only called from derived constructors, and sets up all the
       *  buffer data to zero, including the pointers described in the
       *  basic_streambuf class description.  Note that, as a result,
       *  - the class starts with no read nor write positions available,
       *  - this is not an error
      */
      basic_streambuf()
      : _M_in_beg(0), _M_in_cur(0), _M_in_end(0), 
      _M_out_beg(0), _M_out_cur(0), _M_out_end(0),
      _M_buf_locale(locale()) 
      { }

      // [27.5.2.3.1] get area access
      //@{
      /**
       *  @brief  Access to the get area.
       *
       *  These functions are only available to other protected functions,
       *  including derived classes.
       *
       *  - eback() returns the beginning pointer for the input sequence
       *  - gptr() returns the next pointer for the input sequence
       *  - egptr() returns the end pointer for the input sequence
      */
      char_type* 
      eback() const { return _M_in_beg; }

      char_type* 
      gptr()  const { return _M_in_cur;  }

      char_type* 
      egptr() const { return _M_in_end; }
      //@}

      /**
       *  @brief  Moving the read position.
       *  @param  __n  The delta by which to move.
       *
       *  This just advances the read position without returning any data.
      */
      void 
      gbump(int __n) { _M_in_cur += __n; }

      /**
       *  @brief  Setting the three read area pointers.
       *  @param  __gbeg  A pointer.
       *  @param  __gnext  A pointer.
       *  @param  __gend  A pointer.
       *  @post  @a __gbeg == @c eback(), @a __gnext == @c gptr(), and
       *         @a __gend == @c egptr()
      */
      void 
      setg(char_type* __gbeg, char_type* __gnext, char_type* __gend)
      {
	_M_in_beg = __gbeg;
	_M_in_cur = __gnext;
	_M_in_end = __gend;
      }

      // [27.5.2.3.2] put area access
      //@{
      /**
       *  @brief  Access to the put area.
       *
       *  These functions are only available to other protected functions,
       *  including derived classes.
       *
       *  - pbase() returns the beginning pointer for the output sequence
       *  - pptr() returns the next pointer for the output sequence
       *  - epptr() returns the end pointer for the output sequence
      */
      char_type* 
      pbase() const { return _M_out_beg; }

      char_type* 
      pptr() const { return _M_out_cur; }

      char_type* 
      epptr() const { return _M_out_end; }
      //@}

      /**
       *  @brief  Moving the write position.
       *  @param  __n  The delta by which to move.
       *
       *  This just advances the write position without returning any data.
      */
      void 
      pbump(int __n) { _M_out_cur += __n; }

      /**
       *  @brief  Setting the three write area pointers.
       *  @param  __pbeg  A pointer.
       *  @param  __pend  A pointer.
       *  @post  @a __pbeg == @c pbase(), @a __pbeg == @c pptr(), and
       *         @a __pend == @c epptr()
      */
      void 
      setp(char_type* __pbeg, char_type* __pend)
      { 
	_M_out_beg = _M_out_cur = __pbeg; 
	_M_out_end = __pend;
      }

      // [27.5.2.4] virtual functions
      // [27.5.2.4.1] locales
      /**
       *  @brief  Changes translations.
       *  @param  __loc  A new locale.
       *
       *  Translations done during I/O which depend on the current
       *  locale are changed by this call.  The standard adds,
       *  <em>Between invocations of this function a class derived
       *  from streambuf can safely cache results of calls to locale
       *  functions and to members of facets so obtained.</em>
       *
       *  @note  Base class version does nothing.
      */
      virtual void 
      imbue(const locale& __loc) 
      { }

      // [27.5.2.4.2] buffer management and positioning
      /**
       *  @brief  Manipulates the buffer.
       *
       *  Each derived class provides its own appropriate behavior.  See
       *  the next-to-last paragraph of 
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt11ch25s02.html
       *  for more on this function.
       *
       *  @note  Base class version does nothing, returns @c this.
      */
      virtual basic_streambuf<char_type,_Traits>* 
      setbuf(char_type*, streamsize)
      {	return this; }
      
      /**
       *  @brief  Alters the stream positions.
       *
       *  Each derived class provides its own appropriate behavior.
       *  @note  Base class version does nothing, returns a @c pos_type
       *         that represents an invalid stream position.
      */
      virtual pos_type 
      seekoff(off_type, ios_base::seekdir,
	      ios_base::openmode /*__mode*/ = ios_base::in | ios_base::out)
      { return pos_type(off_type(-1)); } 

      /**
       *  @brief  Alters the stream positions.
       *
       *  Each derived class provides its own appropriate behavior.
       *  @note  Base class version does nothing, returns a @c pos_type
       *         that represents an invalid stream position.
      */
      virtual pos_type 
      seekpos(pos_type, 
	      ios_base::openmode /*__mode*/ = ios_base::in | ios_base::out)
      { return pos_type(off_type(-1)); } 

      /**
       *  @brief  Synchronizes the buffer arrays with the controlled sequences.
       *  @return  -1 on failure.
       *
       *  Each derived class provides its own appropriate behavior,
       *  including the definition of @a failure.
       *  @note  Base class version does nothing, returns zero.
      */
      virtual int 
      sync() { return 0; }

      // [27.5.2.4.3] get area
      /**
       *  @brief  Investigating the data available.
       *  @return  An estimate of the number of characters available in the
       *           input sequence, or -1.
       *
       *  <em>If it returns a positive value, then successive calls to
       *  @c underflow() will not return @c traits::eof() until at
       *  least that number of characters have been supplied.  If @c
       *  showmanyc() returns -1, then calls to @c underflow() or @c
       *  uflow() will fail.</em> [27.5.2.4.3]/1
       *
       *  @note  Base class version does nothing, returns zero.
       *  @note  The standard adds that <em>the intention is not only that the
       *         calls [to underflow or uflow] will not return @c eof() but
       *         that they will return immediately.</em>
       *  @note  The standard adds that <em>the morphemes of @c showmanyc are
       *         @b es-how-many-see, not @b show-manic.</em>
      */
      virtual streamsize 
      showmanyc() { return 0; }

      /**
       *  @brief  Multiple character extraction.
       *  @param  __s  A buffer area.
       *  @param  __n  Maximum number of characters to assign.
       *  @return  The number of characters assigned.
       *
       *  Fills @a __s[0] through @a __s[__n-1] with characters from the input
       *  sequence, as if by @c sbumpc().  Stops when either @a __n characters
       *  have been copied, or when @c traits::eof() would be copied.
       *
       *  It is expected that derived classes provide a more efficient
       *  implementation by overriding this definition.
      */
      virtual streamsize 
      xsgetn(char_type* __s, streamsize __n);

      /**
       *  @brief  Fetches more data from the controlled sequence.
       *  @return  The first character from the <em>pending sequence</em>.
       *
       *  Informally, this function is called when the input buffer is
       *  exhausted (or does not exist, as buffering need not actually be
       *  done).  If a buffer exists, it is @a refilled.  In either case, the
       *  next available character is returned, or @c traits::eof() to
       *  indicate a null pending sequence.
       *
       *  For a formal definition of the pending sequence, see a good text
       *  such as Langer & Kreft, or [27.5.2.4.3]/7-14.
       *
       *  A functioning input streambuf can be created by overriding only
       *  this function (no buffer area will be used).  For an example, see
       *  http://gcc.gnu.org/onlinedocs/libstdc++/manual/bk01pt11ch25.html
       *
       *  @note  Base class version does nothing, returns eof().
      */
      virtual int_type 
      underflow()
      { return traits_type::eof(); }

      /**
       *  @brief  Fetches more data from the controlled sequence.
       *  @return  The first character from the <em>pending sequence</em>.
       *
       *  Informally, this function does the same thing as @c underflow(),
       *  and in fact is required to call that function.  It also returns
       *  the new character, like @c underflow() does.  However, this
       *  function also moves the read position forward by one.
      */
      virtual int_type 
      uflow() 
      {
	int_type __ret = traits_type::eof();
	const bool __testeof = traits_type::eq_int_type(this->underflow(), 
							__ret);
	if (!__testeof)
	  {
	    __ret = traits_type::to_int_type(*this->gptr());
	    this->gbump(1);
	  }
	return __ret;    
      }

      // [27.5.2.4.4] putback
      /**
       *  @brief  Tries to back up the input sequence.
       *  @param  __c  The character to be inserted back into the sequence.
       *  @return  eof() on failure, <em>some other value</em> on success
       *  @post  The constraints of @c gptr(), @c eback(), and @c pptr()
       *         are the same as for @c underflow().
       *
       *  @note  Base class version does nothing, returns eof().
      */
      virtual int_type 
      pbackfail(int_type __c  = traits_type::eof())
      { return traits_type::eof(); }

      // Put area:
      /**
       *  @brief  Multiple character insertion.
       *  @param  __s  A buffer area.
       *  @param  __n  Maximum number of characters to write.
       *  @return  The number of characters written.
       *
       *  Writes @a __s[0] through @a __s[__n-1] to the output sequence, as if
       *  by @c sputc().  Stops when either @a n characters have been
       *  copied, or when @c sputc() would return @c traits::eof().
       *
       *  It is expected that derived classes provide a more efficient
       *  implementation by overriding this definition.
      */
      virtual streamsize 
      xsputn(const char_type* __s, streamsize __n);

      /**
       *  @brief  Consumes data from the buffer; writes to the
       *          controlled sequence.
       *  @param  __c  An additional character to consume.
       *  @return  eof() to indicate failure, something else (usually
       *           @a __c, or not_eof())
       *
       *  Informally, this function is called when the output buffer
       *  is full (or does not exist, as buffering need not actually
       *  be done).  If a buffer exists, it is @a consumed, with
       *  <em>some effect</em> on the controlled sequence.
       *  (Typically, the buffer is written out to the sequence
       *  verbatim.)  In either case, the character @a c is also
       *  written out, if @a __c is not @c eof().
       *
       *  For a formal definition of this function, see a good text
       *  such as Langer & Kreft, or [27.5.2.4.5]/3-7.
       *
       *  A functioning output streambuf can be created by overriding only
       *  this function (no buffer area will be used).
       *
       *  @note  Base class version does nothing, returns eof().
      */
      virtual int_type 
      overflow(int_type __c  = traits_type::eof())
      { return traits_type::eof(); }

#if _GLIBCXX_USE_DEPRECATED
    // Annex D.6
    public:
      /**
       *  @brief  Tosses a character.
       *
       *  Advances the read pointer, ignoring the character that would have
       *  been read.
       *
       *  See http://gcc.gnu.org/ml/libstdc++/2002-05/msg00168.html
       */
      void 
      stossc() 
      {
	if (this->gptr() < this->egptr()) 
	  this->gbump(1);
	else 
	  this->uflow();
      }
#endif

      // Also used by specializations for char and wchar_t in src.
      void 
      __safe_gbump(streamsize __n) { _M_in_cur += __n; }

      void
      __safe_pbump(streamsize __n) { _M_out_cur += __n; }

    private:
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // Side effect of DR 50. 
      basic_streambuf(const basic_streambuf& __sb)
      : _M_in_beg(__sb._M_in_beg), _M_in_cur(__sb._M_in_cur), 
      _M_in_end(__sb._M_in_end), _M_out_beg(__sb._M_out_beg), 
      _M_out_cur(__sb._M_out_cur), _M_out_end(__sb._M_out_cur),
      _M_buf_locale(__sb._M_buf_locale) 
      { }

      basic_streambuf&
      operator=(const basic_streambuf&) { return *this; };
    };

  // Explicit specialization declarations, defined in src/streambuf.cc.
  template<>
    streamsize
    __copy_streambufs_eof(basic_streambuf<char>* __sbin,
			  basic_streambuf<char>* __sbout, bool& __ineof);
#ifdef _GLIBCXX_USE_WCHAR_T
  template<>
    streamsize
    __copy_streambufs_eof(basic_streambuf<wchar_t>* __sbin,
			  basic_streambuf<wchar_t>* __sbout, bool& __ineof);
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#include <bits/streambuf.tcc>

#endif /* _GLIBCXX_STREAMBUF */

// Components for manipulating sequences of characters -*- C++ -*-

// Copyright (C) 1997-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/string
 *  This is a Standard C++ Library header.
 */

//
// ISO C++ 14882: 21  Strings library
//

#ifndef _GLIBCXX_STRING
#define _GLIBCXX_STRING	1

#pragma GCC system_header

#include <bits/c++config.h>
#include <bits/stringfwd.h>
#include <bits/char_traits.h>  // NB: In turn includes stl_algobase.h
#include <bits/allocator.h>
#include <bits/cpp_type_traits.h>
#include <bits/localefwd.h>    // For operators >>, <<, and getline.
#include <bits/ostream_insert.h>
#include <bits/stl_iterator_base_types.h>
#include <bits/stl_iterator_base_funcs.h>
#include <bits/stl_iterator.h>
#include <bits/stl_function.h> // For less
#include <ext/numeric_traits.h> 
#include <bits/stl_algobase.h> 
#include <bits/range_access.h>
#include <bits/basic_string.h>
#include <bits/basic_string.tcc> 

#endif /* _GLIBCXX_STRING */

// <utility> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/utility
 *  This is a Standard C++ Library header. 
 */

#ifndef _GLIBCXX_UTILITY
#define _GLIBCXX_UTILITY 1

#pragma GCC system_header

/**
 * @defgroup utilities Utilities
 *
 * Components deemed generally useful. Includes pair, tuple,
 * forward/move helpers, ratio, function object, metaprogramming and
 * type traits, time, date, and memory functions.
 */

#include <bits/c++config.h>
#include <bits/stl_relops.h>
#include <bits/stl_pair.h>

#if __cplusplus >= 201103L

#include <bits/move.h>
#include <initializer_list>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<class _Tp>
    class tuple_size;

  template<std::size_t _Int, class _Tp>
    class tuple_element;

   // Various functions which give std::pair a tuple-like interface.
  template<class _Tp1, class _Tp2>
    struct tuple_size<std::pair<_Tp1, _Tp2>>
    : public integral_constant<std::size_t, 2> { };

  template<class _Tp1, class _Tp2>
    struct tuple_element<0, std::pair<_Tp1, _Tp2>>
    { typedef _Tp1 type; };
 
  template<class _Tp1, class _Tp2>
    struct tuple_element<1, std::pair<_Tp1, _Tp2>>
    { typedef _Tp2 type; };

  template<std::size_t _Int>
    struct __pair_get;

  template<>
    struct __pair_get<0>
    {
      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp1&
        __get(std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.first; }

      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp1&&
        __move_get(std::pair<_Tp1, _Tp2>&& __pair) noexcept
        { return std::forward<_Tp1>(__pair.first); }

      template<typename _Tp1, typename _Tp2>
        static constexpr const _Tp1&
        __const_get(const std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.first; }
    };

  template<>
    struct __pair_get<1>
    {
      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp2&
        __get(std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.second; }

      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp2&&
        __move_get(std::pair<_Tp1, _Tp2>&& __pair) noexcept
        { return std::forward<_Tp2>(__pair.second); }

      template<typename _Tp1, typename _Tp2>
        static constexpr const _Tp2&
        __const_get(const std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.second; }
    };

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&
    get(std::pair<_Tp1, _Tp2>& __in) noexcept
    { return __pair_get<_Int>::__get(__in); }

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&&
    get(std::pair<_Tp1, _Tp2>&& __in) noexcept
    { return __pair_get<_Int>::__move_get(std::move(__in)); }

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr const typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&
    get(const std::pair<_Tp1, _Tp2>& __in) noexcept
    { return __pair_get<_Int>::__const_get(__in); }

#if __cplusplus > 201103L

#define __cpp_lib_tuples_by_type 201304

  template <typename _Tp, typename _Up>
    constexpr _Tp&
    get(pair<_Tp, _Up>& __p) noexcept
    { return __p.first; }

  template <typename _Tp, typename _Up>
    constexpr const _Tp&
    get(const pair<_Tp, _Up>& __p) noexcept
    { return __p.first; }

  template <typename _Tp, typename _Up>
    constexpr _Tp&&
    get(pair<_Tp, _Up>&& __p) noexcept
    { return std::move(__p.first); }

  template <typename _Tp, typename _Up>
    constexpr _Tp&
    get(pair<_Up, _Tp>& __p) noexcept
    { return __p.second; }

  template <typename _Tp, typename _Up>
    constexpr const _Tp&
    get(const pair<_Up, _Tp>& __p) noexcept
    { return __p.second; }

  template <typename _Tp, typename _Up>
    constexpr _Tp&&
    get(pair<_Up, _Tp>&& __p) noexcept
    { return std::move(__p.second); }

#define __cpp_lib_exchange_function 201304

  /// Assign @p __new_val to @p __obj and return its previous value.
  template <typename _Tp, typename _Up = _Tp>
    inline _Tp
    exchange(_Tp& __obj, _Up&& __new_val)
    {
      _Tp __old_val = std::move(__obj);
      __obj = std::forward<_Up>(__new_val);
      return __old_val;
    }
#endif

  // Stores a tuple of indices.  Used by tuple and pair, and by bind() to
  // extract the elements in a tuple.
  template<size_t... _Indexes>
    struct _Index_tuple
    {
      typedef _Index_tuple<_Indexes..., sizeof...(_Indexes)> __next;
    };

  // Builds an _Index_tuple<0, 1, 2, ..., _Num-1>.
  template<size_t _Num>
    struct _Build_index_tuple
    {
      typedef typename _Build_index_tuple<_Num - 1>::__type::__next __type;
    };

  template<>
    struct _Build_index_tuple<0>
    {
      typedef _Index_tuple<> __type;
    };

#if __cplusplus > 201103L

#define __cpp_lib_integer_sequence 201304

  /// Class template integer_sequence
  template<typename _Tp, _Tp... _Idx>
    struct integer_sequence
    {
      typedef _Tp value_type;
      static constexpr size_t size() { return sizeof...(_Idx); }
    };

  template<typename _Tp, _Tp _Num,
	   typename _ISeq = typename _Build_index_tuple<_Num>::__type>
    struct _Make_integer_sequence;

  template<typename _Tp, _Tp _Num,  size_t... _Idx>
    struct _Make_integer_sequence<_Tp, _Num, _Index_tuple<_Idx...>>
    {
      static_assert( _Num >= 0,
		     "Cannot make integer sequence of negative length" );

      typedef integer_sequence<_Tp, static_cast<_Tp>(_Idx)...> __type;
    };

  /// Alias template make_integer_sequence
  template<typename _Tp, _Tp _Num>
    using make_integer_sequence
      = typename _Make_integer_sequence<_Tp, _Num>::__type;

  /// Alias template index_sequence
  template<size_t... _Idx>
    using index_sequence = integer_sequence<size_t, _Idx...>;

  /// Alias template make_index_sequence
  template<size_t _Num>
    using make_index_sequence = make_integer_sequence<size_t, _Num>;

  /// Alias template index_sequence_for
  template<typename... _Types>
    using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif

#endif /* _GLIBCXX_UTILITY */

// <vector> -*- C++ -*-

// Copyright (C) 2001-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file include/vector
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_VECTOR
#define _GLIBCXX_VECTOR 1

#pragma GCC system_header

#include <bits/stl_algobase.h>
#include <bits/allocator.h>
#include <bits/stl_construct.h>
#include <bits/stl_uninitialized.h>
#include <bits/stl_vector.h>
#include <bits/stl_bvector.h> 
#include <bits/range_access.h>

#ifndef _GLIBCXX_EXPORT_TEMPLATE
# include <bits/vector.tcc>
#endif

#ifdef _GLIBCXX_DEBUG
# include <debug/vector>
#endif

#ifdef _GLIBCXX_PROFILE
# include <profile/vector>
#endif

#endif /* _GLIBCXX_VECTOR */


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
/*
 * complex.h
 *
 * This file is part of the Mingw32 package.
 *
 * Contributors:
 *  Created by Danny Smith <dannysmith@users.sourceforge.net>
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _COMPLEX_H_
#define _COMPLEX_H_

/* All the headers include this file. */
#include <crtdefs.h>

/* These macros are specified by C99 standard */

#ifndef __cplusplus
#define complex _Complex
#endif

#define _Complex_I  (0.0F +  1.0iF)

/* GCC doesn't support _Imaginary type yet, so we don't
   define _Imaginary_I */

#define I _Complex_I

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef RC_INVOKED

double __MINGW_ATTRIB_CONST creal (double _Complex);
double __MINGW_ATTRIB_CONST cimag (double _Complex);
double __MINGW_ATTRIB_CONST carg (double _Complex);
double __MINGW_ATTRIB_CONST cabs (double _Complex) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
double _Complex __MINGW_ATTRIB_CONST conj (double _Complex);
double _Complex  cacos (double _Complex);
double _Complex  casin (double _Complex);
double _Complex  catan (double _Complex);
double _Complex  ccos (double _Complex);
double _Complex  csin (double _Complex);
double _Complex  ctan (double _Complex);
double _Complex  cacosh (double _Complex);
double _Complex  casinh (double _Complex);
double _Complex  catanh (double _Complex);
double _Complex  ccosh (double _Complex);
double _Complex  csinh (double _Complex);
double _Complex  ctanh (double _Complex);
double _Complex  cexp (double _Complex);
double _Complex  clog (double _Complex);
double _Complex  cpow (double _Complex, double _Complex);
double _Complex  csqrt (double _Complex);
double _Complex __MINGW_ATTRIB_CONST cproj (double _Complex);

float __MINGW_ATTRIB_CONST crealf (float _Complex);
float __MINGW_ATTRIB_CONST cimagf (float _Complex);
float __MINGW_ATTRIB_CONST cargf (float _Complex);
float __MINGW_ATTRIB_CONST cabsf (float _Complex);
float _Complex __MINGW_ATTRIB_CONST conjf (float _Complex);
float _Complex  cacosf (float _Complex);
float _Complex  casinf (float _Complex);
float _Complex  catanf (float _Complex);
float _Complex  ccosf (float _Complex);
float _Complex  csinf (float _Complex);
float _Complex  ctanf (float _Complex);
float _Complex  cacoshf (float _Complex);
float _Complex  casinhf (float _Complex);
float _Complex  catanhf (float _Complex);
float _Complex  ccoshf (float _Complex);
float _Complex  csinhf (float _Complex);
float _Complex  ctanhf (float _Complex);
float _Complex  cexpf (float _Complex);
float _Complex  clogf (float _Complex);
float _Complex  cpowf (float _Complex, float _Complex);
float _Complex  csqrtf (float _Complex);
float _Complex __MINGW_ATTRIB_CONST cprojf (float _Complex);

long double __MINGW_ATTRIB_CONST creall (long double _Complex);
long double __MINGW_ATTRIB_CONST cimagl (long double _Complex);
long double __MINGW_ATTRIB_CONST cargl (long double _Complex);
long double __MINGW_ATTRIB_CONST cabsl (long double _Complex);
long double _Complex __MINGW_ATTRIB_CONST conjl (long double _Complex);
long double _Complex  cacosl (long double _Complex);
long double _Complex  casinl (long double _Complex);
long double _Complex  catanl (long double _Complex);
long double _Complex  ccosl (long double _Complex);
long double _Complex  csinl (long double _Complex);
long double _Complex  ctanl (long double _Complex);
long double _Complex  cacoshl (long double _Complex);
long double _Complex  casinhl (long double _Complex);
long double _Complex  catanhl (long double _Complex);
long double _Complex  ccoshl (long double _Complex);
long double _Complex  csinhl (long double _Complex);
long double _Complex  ctanhl (long double _Complex);
long double _Complex  cexpl (long double _Complex);
long double _Complex  clogl (long double _Complex);
long double _Complex  cpowl (long double _Complex, long double _Complex);
long double _Complex  csqrtl (long double _Complex);
long double _Complex __MINGW_ATTRIB_CONST cprojl (long double _Complex);

#ifdef __GNUC__
#if !defined (__CRT__NO_INLINE) && defined (_MATH_H_)
/* double */
__CRT_INLINE double __MINGW_ATTRIB_CONST creal (double _Complex _Z)
{
  return __real__ _Z;
}

__CRT_INLINE double __MINGW_ATTRIB_CONST cimag (double _Complex _Z)
{
  return __imag__ _Z;
}

__CRT_INLINE double _Complex __MINGW_ATTRIB_CONST conj (double _Complex _Z)
{
  return __extension__ ~_Z;
}

__CRT_INLINE  double __MINGW_ATTRIB_CONST carg (double _Complex _Z)
{
  return atan2 (__imag__ _Z, __real__ _Z);
}

__CRT_INLINE double __MINGW_ATTRIB_CONST cabs (double _Complex _Z)
{
  return hypot (__real__ _Z, __imag__ _Z);
}

/* float */
__CRT_INLINE float __MINGW_ATTRIB_CONST crealf (float _Complex _Z)
{
  return __real__ _Z;
}

__CRT_INLINE float __MINGW_ATTRIB_CONST cimagf (float _Complex _Z)
{
  return __imag__ _Z;
}

__CRT_INLINE float _Complex __MINGW_ATTRIB_CONST conjf (float _Complex _Z)
{
  return __extension__ ~_Z;
}

__CRT_INLINE  float __MINGW_ATTRIB_CONST cargf (float _Complex _Z)
{
  return atan2f (__imag__ _Z, __real__ _Z);
}

__CRT_INLINE float __MINGW_ATTRIB_CONST cabsf (float _Complex _Z)
{
  return hypotf (__real__ _Z, __imag__ _Z);
}

/* long double */
__CRT_INLINE long double __MINGW_ATTRIB_CONST creall (long double _Complex _Z)
{
  return __real__ _Z;
}

__CRT_INLINE long double __MINGW_ATTRIB_CONST cimagl (long double _Complex _Z)
{
  return __imag__ _Z;
}

__CRT_INLINE long double _Complex __MINGW_ATTRIB_CONST conjl (long double _Complex _Z)
{
  return __extension__ ~_Z;
}

__CRT_INLINE  long double __MINGW_ATTRIB_CONST cargl (long double _Complex _Z)
{
  return atan2l (__imag__ _Z, __real__ _Z);
}

__CRT_INLINE long double __MINGW_ATTRIB_CONST cabsl (long double _Complex _Z)
{
  return hypotl (__real__ _Z, __imag__ _Z);
}
#endif /* !__CRT__NO_INLINE */
#endif /* __GNUC__ */


#endif /* RC_INVOKED */

#ifdef __cplusplus
}
#endif 

#endif /* _COMPLEX_H */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
/*
 * complex.h
 *
 * This file is part of the Mingw32 package.
 *
 * Contributors:
 *  Created by Danny Smith <dannysmith@users.sourceforge.net>
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _COMPLEX_H_
#define _COMPLEX_H_

/* All the headers include this file. */
#include <crtdefs.h>

/* These macros are specified by C99 standard */

#ifndef __cplusplus
#define complex _Complex
#endif

#define _Complex_I  (0.0F +  1.0iF)

/* GCC doesn't support _Imaginary type yet, so we don't
   define _Imaginary_I */

#define I _Complex_I

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef RC_INVOKED

double __MINGW_ATTRIB_CONST creal (double _Complex);
double __MINGW_ATTRIB_CONST cimag (double _Complex);
double __MINGW_ATTRIB_CONST carg (double _Complex);
double __MINGW_ATTRIB_CONST cabs (double _Complex) __MINGW_ATTRIB_DEPRECATED_MSVC2005;
double _Complex __MINGW_ATTRIB_CONST conj (double _Complex);
double _Complex  cacos (double _Complex);
double _Complex  casin (double _Complex);
double _Complex  catan (double _Complex);
double _Complex  ccos (double _Complex);
double _Complex  csin (double _Complex);
double _Complex  ctan (double _Complex);
double _Complex  cacosh (double _Complex);
double _Complex  casinh (double _Complex);
double _Complex  catanh (double _Complex);
double _Complex  ccosh (double _Complex);
double _Complex  csinh (double _Complex);
double _Complex  ctanh (double _Complex);
double _Complex  cexp (double _Complex);
double _Complex  clog (double _Complex);
double _Complex  cpow (double _Complex, double _Complex);
double _Complex  csqrt (double _Complex);
double _Complex __MINGW_ATTRIB_CONST cproj (double _Complex);

float __MINGW_ATTRIB_CONST crealf (float _Complex);
float __MINGW_ATTRIB_CONST cimagf (float _Complex);
float __MINGW_ATTRIB_CONST cargf (float _Complex);
float __MINGW_ATTRIB_CONST cabsf (float _Complex);
float _Complex __MINGW_ATTRIB_CONST conjf (float _Complex);
float _Complex  cacosf (float _Complex);
float _Complex  casinf (float _Complex);
float _Complex  catanf (float _Complex);
float _Complex  ccosf (float _Complex);
float _Complex  csinf (float _Complex);
float _Complex  ctanf (float _Complex);
float _Complex  cacoshf (float _Complex);
float _Complex  casinhf (float _Complex);
float _Complex  catanhf (float _Complex);
float _Complex  ccoshf (float _Complex);
float _Complex  csinhf (float _Complex);
float _Complex  ctanhf (float _Complex);
float _Complex  cexpf (float _Complex);
float _Complex  clogf (float _Complex);
float _Complex  cpowf (float _Complex, float _Complex);
float _Complex  csqrtf (float _Complex);
float _Complex __MINGW_ATTRIB_CONST cprojf (float _Complex);

long double __MINGW_ATTRIB_CONST creall (long double _Complex);
long double __MINGW_ATTRIB_CONST cimagl (long double _Complex);
long double __MINGW_ATTRIB_CONST cargl (long double _Complex);
long double __MINGW_ATTRIB_CONST cabsl (long double _Complex);
long double _Complex __MINGW_ATTRIB_CONST conjl (long double _Complex);
long double _Complex  cacosl (long double _Complex);
long double _Complex  casinl (long double _Complex);
long double _Complex  catanl (long double _Complex);
long double _Complex  ccosl (long double _Complex);
long double _Complex  csinl (long double _Complex);
long double _Complex  ctanl (long double _Complex);
long double _Complex  cacoshl (long double _Complex);
long double _Complex  casinhl (long double _Complex);
long double _Complex  catanhl (long double _Complex);
long double _Complex  ccoshl (long double _Complex);
long double _Complex  csinhl (long double _Complex);
long double _Complex  ctanhl (long double _Complex);
long double _Complex  cexpl (long double _Complex);
long double _Complex  clogl (long double _Complex);
long double _Complex  cpowl (long double _Complex, long double _Complex);
long double _Complex  csqrtl (long double _Complex);
long double _Complex __MINGW_ATTRIB_CONST cprojl (long double _Complex);

#ifdef __GNUC__
#if !defined (__CRT__NO_INLINE) && defined (_MATH_H_)
/* double */
__CRT_INLINE double __MINGW_ATTRIB_CONST creal (double _Complex _Z)
{
  return __real__ _Z;
}

__CRT_INLINE double __MINGW_ATTRIB_CONST cimag (double _Complex _Z)
{
  return __imag__ _Z;
}

__CRT_INLINE double _Complex __MINGW_ATTRIB_CONST conj (double _Complex _Z)
{
  return __extension__ ~_Z;
}

__CRT_INLINE  double __MINGW_ATTRIB_CONST carg (double _Complex _Z)
{
  return atan2 (__imag__ _Z, __real__ _Z);
}

__CRT_INLINE double __MINGW_ATTRIB_CONST cabs (double _Complex _Z)
{
  return hypot (__real__ _Z, __imag__ _Z);
}

/* float */
__CRT_INLINE float __MINGW_ATTRIB_CONST crealf (float _Complex _Z)
{
  return __real__ _Z;
}

__CRT_INLINE float __MINGW_ATTRIB_CONST cimagf (float _Complex _Z)
{
  return __imag__ _Z;
}

__CRT_INLINE float _Complex __MINGW_ATTRIB_CONST conjf (float _Complex _Z)
{
  return __extension__ ~_Z;
}

__CRT_INLINE  float __MINGW_ATTRIB_CONST cargf (float _Complex _Z)
{
  return atan2f (__imag__ _Z, __real__ _Z);
}

__CRT_INLINE float __MINGW_ATTRIB_CONST cabsf (float _Complex _Z)
{
  return hypotf (__real__ _Z, __imag__ _Z);
}

/* long double */
__CRT_INLINE long double __MINGW_ATTRIB_CONST creall (long double _Complex _Z)
{
  return __real__ _Z;
}

__CRT_INLINE long double __MINGW_ATTRIB_CONST cimagl (long double _Complex _Z)
{
  return __imag__ _Z;
}

__CRT_INLINE long double _Complex __MINGW_ATTRIB_CONST conjl (long double _Complex _Z)
{
  return __extension__ ~_Z;
}

__CRT_INLINE  long double __MINGW_ATTRIB_CONST cargl (long double _Complex _Z)
{
  return atan2l (__imag__ _Z, __real__ _Z);
}

__CRT_INLINE long double __MINGW_ATTRIB_CONST cabsl (long double _Complex _Z)
{
  return hypotl (__real__ _Z, __imag__ _Z);
}
#endif /* !__CRT__NO_INLINE */
#endif /* __GNUC__ */


#endif /* RC_INVOKED */

#ifdef __cplusplus
}
#endif 

#endif /* _COMPLEX_H */

// -*- C++ -*- compatibility header.

// Copyright (C) 2007-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file complex.h
 *  This is a Standard C++ Library header.
 */

#include <bits/c++config.h>

#if __cplusplus >= 201103L
# include <ccomplex>
#endif

#if _GLIBCXX_HAVE_COMPLEX_H
# include_next <complex.h>
# ifdef _GLIBCXX_COMPLEX
// See PR56111, keep the macro in C++03 if possible.
#  undef complex
# endif
#endif

#ifndef _GLIBCXX_COMPLEX_H
#define _GLIBCXX_COMPLEX_H 1

#endif

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _FENV_H_
#define _FENV_H_

#include <crtdefs.h>

/* FPU status word exception flags */
#define FE_INVALID	0x01
#define FE_DENORMAL	0x02
#define FE_DIVBYZERO	0x04
#define FE_OVERFLOW	0x08
#define FE_UNDERFLOW	0x10
#define FE_INEXACT	0x20
#define FE_ALL_EXCEPT (FE_INVALID | FE_DENORMAL | FE_DIVBYZERO \
		       | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)

/* FPU control word rounding flags */
#define FE_TONEAREST	0x0000
#define FE_DOWNWARD	0x0400
#define FE_UPWARD	0x0800
#define FE_TOWARDZERO	0x0c00

/* The MXCSR exception flags are the same as the
   FE flags. */
#define __MXCSR_EXCEPT_FLAG_SHIFT 0

/* How much to shift FE status word exception flags
   to get the MXCSR exeptions masks,  */
#define __MXCSR_EXCEPT_MASK_SHIFT 7

/* How much to shift FE status word exception flags
   to get MXCSR rounding flags,  */
#define __MXCSR_ROUND_FLAG_SHIFT 3

#ifndef RC_INVOKED
/*
  For now, support only for the basic abstraction of flags that are
  either set or clear. fexcept_t could be  structure that holds more
  info about the fp environment.
*/
typedef unsigned short fexcept_t;

/* This 32-byte struct represents the entire floating point
   environment as stored by fnstenv or fstenv, augmented by
   the  contents of the MXCSR register, as stored by stmxcsr
   (if CPU supports it). */
typedef struct
{
  unsigned short __control_word;
  unsigned short __unused0;
  unsigned short __status_word;
  unsigned short __unused1;
  unsigned short __tag_word;
  unsigned short __unused2;  
  unsigned int	 __ip_offset;    /* instruction pointer offset */
  unsigned short __ip_selector;  
  unsigned short __opcode;
  unsigned int	 __data_offset;
  unsigned short __data_selector;  
  unsigned short __unused3;
  unsigned int   __mxcsr; /* contents of the MXCSR register  */
} fenv_t;


/*The C99 standard (7.6.9) allows us to define implementation-specific macros for
  different fp environments */
  
/* The default Intel x87 floating point environment (64-bit mantissa) */
#define FE_PC64_ENV ((const fenv_t *)-1)

/* The floating point environment set by MSVCRT _fpreset (53-bit mantissa) */
#define FE_PC53_ENV ((const fenv_t *)-2)

/* The FE_DFL_ENV macro is required by standard.
  fesetenv will use the environment set at app startup.*/
#define FE_DFL_ENV ((const fenv_t *) 0)

#ifdef __cplusplus
extern "C" {
#endif

/*TODO: Some of these could be inlined */
/* 7.6.2 Exception */

extern int __cdecl feclearexcept (int);
extern int __cdecl fegetexceptflag (fexcept_t * flagp, int excepts);
extern int __cdecl feraiseexcept (int excepts );
extern int __cdecl fesetexceptflag (const fexcept_t *, int);
extern int __cdecl fetestexcept (int excepts);

/* 7.6.3 Rounding */

extern int __cdecl fegetround (void);
extern int __cdecl fesetround (int mode);

/* 7.6.4 Environment */

extern int __cdecl fegetenv(fenv_t * envp);
extern int __cdecl fesetenv(const fenv_t * );
extern int __cdecl feupdateenv(const fenv_t *);
extern int __cdecl feholdexcept(fenv_t *);

#ifdef __cplusplus
}
#endif
#endif	/* Not RC_INVOKED */

#endif /* ndef _FENV_H */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#ifndef _FENV_H_
#define _FENV_H_

#include <crtdefs.h>

/* FPU status word exception flags */
#define FE_INVALID	0x01
#define FE_DENORMAL	0x02
#define FE_DIVBYZERO	0x04
#define FE_OVERFLOW	0x08
#define FE_UNDERFLOW	0x10
#define FE_INEXACT	0x20
#define FE_ALL_EXCEPT (FE_INVALID | FE_DENORMAL | FE_DIVBYZERO \
		       | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)

/* FPU control word rounding flags */
#define FE_TONEAREST	0x0000
#define FE_DOWNWARD	0x0400
#define FE_UPWARD	0x0800
#define FE_TOWARDZERO	0x0c00

/* The MXCSR exception flags are the same as the
   FE flags. */
#define __MXCSR_EXCEPT_FLAG_SHIFT 0

/* How much to shift FE status word exception flags
   to get the MXCSR exeptions masks,  */
#define __MXCSR_EXCEPT_MASK_SHIFT 7

/* How much to shift FE status word exception flags
   to get MXCSR rounding flags,  */
#define __MXCSR_ROUND_FLAG_SHIFT 3

#ifndef RC_INVOKED
/*
  For now, support only for the basic abstraction of flags that are
  either set or clear. fexcept_t could be  structure that holds more
  info about the fp environment.
*/
typedef unsigned short fexcept_t;

/* This 32-byte struct represents the entire floating point
   environment as stored by fnstenv or fstenv, augmented by
   the  contents of the MXCSR register, as stored by stmxcsr
   (if CPU supports it). */
typedef struct
{
  unsigned short __control_word;
  unsigned short __unused0;
  unsigned short __status_word;
  unsigned short __unused1;
  unsigned short __tag_word;
  unsigned short __unused2;  
  unsigned int	 __ip_offset;    /* instruction pointer offset */
  unsigned short __ip_selector;  
  unsigned short __opcode;
  unsigned int	 __data_offset;
  unsigned short __data_selector;  
  unsigned short __unused3;
  unsigned int   __mxcsr; /* contents of the MXCSR register  */
} fenv_t;


/*The C99 standard (7.6.9) allows us to define implementation-specific macros for
  different fp environments */
  
/* The default Intel x87 floating point environment (64-bit mantissa) */
#define FE_PC64_ENV ((const fenv_t *)-1)

/* The floating point environment set by MSVCRT _fpreset (53-bit mantissa) */
#define FE_PC53_ENV ((const fenv_t *)-2)

/* The FE_DFL_ENV macro is required by standard.
  fesetenv will use the environment set at app startup.*/
#define FE_DFL_ENV ((const fenv_t *) 0)

#ifdef __cplusplus
extern "C" {
#endif

/*TODO: Some of these could be inlined */
/* 7.6.2 Exception */

extern int __cdecl feclearexcept (int);
extern int __cdecl fegetexceptflag (fexcept_t * flagp, int excepts);
extern int __cdecl feraiseexcept (int excepts );
extern int __cdecl fesetexceptflag (const fexcept_t *, int);
extern int __cdecl fetestexcept (int excepts);

/* 7.6.3 Rounding */

extern int __cdecl fegetround (void);
extern int __cdecl fesetround (int mode);

/* 7.6.4 Environment */

extern int __cdecl fegetenv(fenv_t * envp);
extern int __cdecl fesetenv(const fenv_t * );
extern int __cdecl feupdateenv(const fenv_t *);
extern int __cdecl feholdexcept(fenv_t *);

#ifdef __cplusplus
}
#endif
#endif	/* Not RC_INVOKED */

#endif /* ndef _FENV_H */

// -*- C++ -*- compatibility header.

// Copyright (C) 2007-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file fenv.h
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_FENV_H
#define _GLIBCXX_FENV_H 1

#pragma GCC system_header

#include <bits/c++config.h>
#if _GLIBCXX_HAVE_FENV_H
# include_next <fenv.h>
#endif

#if __cplusplus >= 201103L

#if _GLIBCXX_USE_C99_FENV_TR1

#undef feclearexcept
#undef fegetexceptflag
#undef feraiseexcept
#undef fesetexceptflag
#undef fetestexcept
#undef fegetround
#undef fesetround
#undef fegetenv
#undef feholdexcept
#undef fesetenv
#undef feupdateenv

namespace std
{
  // types
  using ::fenv_t;
  using ::fexcept_t;

  // functions
  using ::feclearexcept;
  using ::fegetexceptflag;
  using ::feraiseexcept;
  using ::fesetexceptflag;
  using ::fetestexcept;

  using ::fegetround;
  using ::fesetround;

  using ::fegetenv;
  using ::feholdexcept;
  using ::fesetenv;
  using ::feupdateenv;
} // namespace

#endif // _GLIBCXX_USE_C99_FENV_TR1

#endif // C++11

#endif // _GLIBCXX_FENV_H

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
/* 7.8 Format conversion of integer types <inttypes.h> */

#ifndef _INTTYPES_H_
#define _INTTYPES_H_

#include <crtdefs.h>
#include <stdint.h>
#define __need_wchar_t
#include <stddef.h>

#ifdef	__cplusplus
extern	"C"	{
#endif

typedef struct {
	intmax_t quot;
	intmax_t rem;
	} imaxdiv_t;

/* Restore to !define(__USE_MINGW_ANSI_STDIO) */
#include <_mingw_print_push.h>

#if !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS)

/* 7.8.1 Macros for format specifiers
 * 
 * MS runtime does not yet understand C9x standard "ll"
 * length specifier. It appears to treat "ll" as "l".
 * The non-standard I64 length specifier causes warning in GCC,
 * but understood by MS runtime functions.
 */

/* fprintf macros for signed types */
#define PRId8 "d"
#define PRId16 "d"
#define PRId32 "d"
#define PRId64 "I64d"

#define PRIdLEAST8 "d"
#define PRIdLEAST16 "d"
#define PRIdLEAST32 "d"
#define PRIdLEAST64 "I64d"

#define PRIdFAST8 "d"
#define PRIdFAST16 "d"
#define PRIdFAST32 "d"
#define PRIdFAST64 "I64d"

#define PRIdMAX "I64d"

#define PRIi8 "i"
#define PRIi16 "i"
#define PRIi32 "i"
#define PRIi64 "I64i"

#define PRIiLEAST8 "i"
#define PRIiLEAST16 "i"
#define PRIiLEAST32 "i"
#define PRIiLEAST64 "I64i"

#define PRIiFAST8 "i"
#define PRIiFAST16 "i"
#define PRIiFAST32 "i"
#define PRIiFAST64 "I64i"

#define PRIiMAX "I64i"

#define PRIo8 "o"
#define PRIo16 "o"
#define PRIo32 "o"
#define PRIo64 "I64o"

#define PRIoLEAST8 "o"
#define PRIoLEAST16 "o"
#define PRIoLEAST32 "o"
#define PRIoLEAST64 "I64o"

#define PRIoFAST8 "o"
#define PRIoFAST16 "o"
#define PRIoFAST32 "o"
#define PRIoFAST64 "I64o"

#define PRIoMAX "I64o"

/* fprintf macros for unsigned types */
#define PRIu8 "u"
#define PRIu16 "u"
#define PRIu32 "u"
#define PRIu64 "I64u"


#define PRIuLEAST8 "u"
#define PRIuLEAST16 "u"
#define PRIuLEAST32 "u"
#define PRIuLEAST64 "I64u"

#define PRIuFAST8 "u"
#define PRIuFAST16 "u"
#define PRIuFAST32 "u"
#define PRIuFAST64 "I64u"

#define PRIuMAX "I64u"

#define PRIx8 "x"
#define PRIx16 "x"
#define PRIx32 "x"
#define PRIx64 "I64x"

#define PRIxLEAST8 "x"
#define PRIxLEAST16 "x"
#define PRIxLEAST32 "x"
#define PRIxLEAST64 "I64x"

#define PRIxFAST8 "x"
#define PRIxFAST16 "x"
#define PRIxFAST32 "x"
#define PRIxFAST64 "I64x"

#define PRIxMAX "I64x"

#define PRIX8 "X"
#define PRIX16 "X"
#define PRIX32 "X"
#define PRIX64 "I64X"

#define PRIXLEAST8 "X"
#define PRIXLEAST16 "X"
#define PRIXLEAST32 "X"
#define PRIXLEAST64 "I64X"

#define PRIXFAST8 "X"
#define PRIXFAST16 "X"
#define PRIXFAST32 "X"
#define PRIXFAST64 "I64X"

#define PRIXMAX "I64X"

/*
 *   fscanf macros for signed int types
 *   NOTE: if 32-bit int is used for int_fast8_t and int_fast16_t
 *   (see stdint.h, 7.18.1.3), FAST8 and FAST16 should have
 *   no length identifiers
 */

#define SCNd16 "hd"
#define SCNd32 "d"
#define SCNd64 "I64d"

#define SCNdLEAST16 "hd"
#define SCNdLEAST32 "d"
#define SCNdLEAST64 "I64d"

#define SCNdFAST16 "hd"
#define SCNdFAST32 "d"
#define SCNdFAST64 "I64d"

#define SCNdMAX "I64d"

#define SCNi16 "hi"
#define SCNi32 "i"
#define SCNi64 "I64i"

#define SCNiLEAST16 "hi"
#define SCNiLEAST32 "i"
#define SCNiLEAST64 "I64i"

#define SCNiFAST16 "hi"
#define SCNiFAST32 "i"
#define SCNiFAST64 "I64i"

#define SCNiMAX "I64i"

#define SCNo16 "ho"
#define SCNo32 "o"
#define SCNo64 "I64o"

#define SCNoLEAST16 "ho"
#define SCNoLEAST32 "o"
#define SCNoLEAST64 "I64o"

#define SCNoFAST16 "ho"
#define SCNoFAST32 "o"
#define SCNoFAST64 "I64o"

#define SCNoMAX "I64o"

#define SCNx16 "hx"
#define SCNx32 "x"
#define SCNx64 "I64x"

#define SCNxLEAST16 "hx"
#define SCNxLEAST32 "x"
#define SCNxLEAST64 "I64x"

#define SCNxFAST16 "hx"
#define SCNxFAST32 "x"
#define SCNxFAST64 "I64x"

#define SCNxMAX "I64x"

/* fscanf macros for unsigned int types */

#define SCNu16 "hu"
#define SCNu32 "u"
#define SCNu64 "I64u"

#define SCNuLEAST16 "hu"
#define SCNuLEAST32 "u"
#define SCNuLEAST64 "I64u"

#define SCNuFAST16 "hu"
#define SCNuFAST32 "u"
#define SCNuFAST64 "I64u"

#define SCNuMAX "I64u"

#ifdef _WIN64
#define PRIdPTR "I64d"
#define PRIiPTR "I64i"
#define PRIoPTR "I64o"
#define PRIuPTR "I64u"
#define PRIxPTR "I64x"
#define PRIXPTR "I64X"
#define SCNdPTR "I64d"
#define SCNiPTR "I64i"
#define SCNoPTR "I64o"
#define SCNxPTR "I64x"
#define SCNuPTR "I64u"
#else
#define PRIdPTR "d"
#define PRIiPTR "i"
#define PRIoPTR "o"
#define PRIuPTR "u"
#define PRIxPTR "x"
#define PRIXPTR "X"
#define SCNdPTR "d"
#define SCNiPTR "i"
#define SCNoPTR "o"
#define SCNxPTR "x"
 #define SCNuPTR "u"
#endif

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/*
 * no length modifier for char types prior to C9x
 * MS runtime  scanf appears to treat "hh" as "h" 
 */

/* signed char */
#define SCNd8 "hhd"
#define SCNdLEAST8 "hhd"
#define SCNdFAST8 "hhd"

#define SCNi8 "hhi"
#define SCNiLEAST8 "hhi"
#define SCNiFAST8 "hhi"

#define SCNo8 "hho"
#define SCNoLEAST8 "hho"
#define SCNoFAST8 "hho"

#define SCNx8 "hhx"
#define SCNxLEAST8 "hhx"
#define SCNxFAST8 "hhx"

/* unsigned char */
#define SCNu8 "hhu"
#define SCNuLEAST8 "hhu"
#define SCNuFAST8 "hhu"
#endif /* __STDC_VERSION__ >= 199901 */

#endif	/* !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS) */

intmax_t __cdecl imaxabs (intmax_t j);
#ifndef __CRT__NO_INLINE
__CRT_INLINE intmax_t __cdecl imaxabs (intmax_t j)
	{return	(j >= 0 ? j : -j);}
#endif
imaxdiv_t __cdecl imaxdiv (intmax_t numer, intmax_t denom);

/* 7.8.2 Conversion functions for greatest-width integer types */

intmax_t __cdecl strtoimax (const char* __restrict__ nptr,
                            char** __restrict__ endptr, int base);
uintmax_t __cdecl strtoumax (const char* __restrict__ nptr,
			     char** __restrict__ endptr, int base);

intmax_t __cdecl wcstoimax (const wchar_t* __restrict__ nptr,
                            wchar_t** __restrict__ endptr, int base);
uintmax_t __cdecl wcstoumax (const wchar_t* __restrict__ nptr,
			     wchar_t** __restrict__ endptr, int base);

/* Set PRI... and SCN... according to __USE_MINGW_ANSI_STDIO.  */
#include <_mingw_print_pop.h>

#ifdef	__cplusplus
}
#endif

#endif /* ndef _INTTYPES_H */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
/* 7.8 Format conversion of integer types <inttypes.h> */

#ifndef _INTTYPES_H_
#define _INTTYPES_H_

#include <crtdefs.h>
#include <stdint.h>
#define __need_wchar_t
#include <stddef.h>

#ifdef	__cplusplus
extern	"C"	{
#endif

typedef struct {
	intmax_t quot;
	intmax_t rem;
	} imaxdiv_t;

/* Restore to !define(__USE_MINGW_ANSI_STDIO) */
#include <_mingw_print_push.h>

#if !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS)

/* 7.8.1 Macros for format specifiers
 * 
 * MS runtime does not yet understand C9x standard "ll"
 * length specifier. It appears to treat "ll" as "l".
 * The non-standard I64 length specifier causes warning in GCC,
 * but understood by MS runtime functions.
 */

/* fprintf macros for signed types */
#define PRId8 "d"
#define PRId16 "d"
#define PRId32 "d"
#define PRId64 "I64d"

#define PRIdLEAST8 "d"
#define PRIdLEAST16 "d"
#define PRIdLEAST32 "d"
#define PRIdLEAST64 "I64d"

#define PRIdFAST8 "d"
#define PRIdFAST16 "d"
#define PRIdFAST32 "d"
#define PRIdFAST64 "I64d"

#define PRIdMAX "I64d"

#define PRIi8 "i"
#define PRIi16 "i"
#define PRIi32 "i"
#define PRIi64 "I64i"

#define PRIiLEAST8 "i"
#define PRIiLEAST16 "i"
#define PRIiLEAST32 "i"
#define PRIiLEAST64 "I64i"

#define PRIiFAST8 "i"
#define PRIiFAST16 "i"
#define PRIiFAST32 "i"
#define PRIiFAST64 "I64i"

#define PRIiMAX "I64i"

#define PRIo8 "o"
#define PRIo16 "o"
#define PRIo32 "o"
#define PRIo64 "I64o"

#define PRIoLEAST8 "o"
#define PRIoLEAST16 "o"
#define PRIoLEAST32 "o"
#define PRIoLEAST64 "I64o"

#define PRIoFAST8 "o"
#define PRIoFAST16 "o"
#define PRIoFAST32 "o"
#define PRIoFAST64 "I64o"

#define PRIoMAX "I64o"

/* fprintf macros for unsigned types */
#define PRIu8 "u"
#define PRIu16 "u"
#define PRIu32 "u"
#define PRIu64 "I64u"


#define PRIuLEAST8 "u"
#define PRIuLEAST16 "u"
#define PRIuLEAST32 "u"
#define PRIuLEAST64 "I64u"

#define PRIuFAST8 "u"
#define PRIuFAST16 "u"
#define PRIuFAST32 "u"
#define PRIuFAST64 "I64u"

#define PRIuMAX "I64u"

#define PRIx8 "x"
#define PRIx16 "x"
#define PRIx32 "x"
#define PRIx64 "I64x"

#define PRIxLEAST8 "x"
#define PRIxLEAST16 "x"
#define PRIxLEAST32 "x"
#define PRIxLEAST64 "I64x"

#define PRIxFAST8 "x"
#define PRIxFAST16 "x"
#define PRIxFAST32 "x"
#define PRIxFAST64 "I64x"

#define PRIxMAX "I64x"

#define PRIX8 "X"
#define PRIX16 "X"
#define PRIX32 "X"
#define PRIX64 "I64X"

#define PRIXLEAST8 "X"
#define PRIXLEAST16 "X"
#define PRIXLEAST32 "X"
#define PRIXLEAST64 "I64X"

#define PRIXFAST8 "X"
#define PRIXFAST16 "X"
#define PRIXFAST32 "X"
#define PRIXFAST64 "I64X"

#define PRIXMAX "I64X"

/*
 *   fscanf macros for signed int types
 *   NOTE: if 32-bit int is used for int_fast8_t and int_fast16_t
 *   (see stdint.h, 7.18.1.3), FAST8 and FAST16 should have
 *   no length identifiers
 */

#define SCNd16 "hd"
#define SCNd32 "d"
#define SCNd64 "I64d"

#define SCNdLEAST16 "hd"
#define SCNdLEAST32 "d"
#define SCNdLEAST64 "I64d"

#define SCNdFAST16 "hd"
#define SCNdFAST32 "d"
#define SCNdFAST64 "I64d"

#define SCNdMAX "I64d"

#define SCNi16 "hi"
#define SCNi32 "i"
#define SCNi64 "I64i"

#define SCNiLEAST16 "hi"
#define SCNiLEAST32 "i"
#define SCNiLEAST64 "I64i"

#define SCNiFAST16 "hi"
#define SCNiFAST32 "i"
#define SCNiFAST64 "I64i"

#define SCNiMAX "I64i"

#define SCNo16 "ho"
#define SCNo32 "o"
#define SCNo64 "I64o"

#define SCNoLEAST16 "ho"
#define SCNoLEAST32 "o"
#define SCNoLEAST64 "I64o"

#define SCNoFAST16 "ho"
#define SCNoFAST32 "o"
#define SCNoFAST64 "I64o"

#define SCNoMAX "I64o"

#define SCNx16 "hx"
#define SCNx32 "x"
#define SCNx64 "I64x"

#define SCNxLEAST16 "hx"
#define SCNxLEAST32 "x"
#define SCNxLEAST64 "I64x"

#define SCNxFAST16 "hx"
#define SCNxFAST32 "x"
#define SCNxFAST64 "I64x"

#define SCNxMAX "I64x"

/* fscanf macros for unsigned int types */

#define SCNu16 "hu"
#define SCNu32 "u"
#define SCNu64 "I64u"

#define SCNuLEAST16 "hu"
#define SCNuLEAST32 "u"
#define SCNuLEAST64 "I64u"

#define SCNuFAST16 "hu"
#define SCNuFAST32 "u"
#define SCNuFAST64 "I64u"

#define SCNuMAX "I64u"

#ifdef _WIN64
#define PRIdPTR "I64d"
#define PRIiPTR "I64i"
#define PRIoPTR "I64o"
#define PRIuPTR "I64u"
#define PRIxPTR "I64x"
#define PRIXPTR "I64X"
#define SCNdPTR "I64d"
#define SCNiPTR "I64i"
#define SCNoPTR "I64o"
#define SCNxPTR "I64x"
#define SCNuPTR "I64u"
#else
#define PRIdPTR "d"
#define PRIiPTR "i"
#define PRIoPTR "o"
#define PRIuPTR "u"
#define PRIxPTR "x"
#define PRIXPTR "X"
#define SCNdPTR "d"
#define SCNiPTR "i"
#define SCNoPTR "o"
#define SCNxPTR "x"
 #define SCNuPTR "u"
#endif

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
/*
 * no length modifier for char types prior to C9x
 * MS runtime  scanf appears to treat "hh" as "h" 
 */

/* signed char */
#define SCNd8 "hhd"
#define SCNdLEAST8 "hhd"
#define SCNdFAST8 "hhd"

#define SCNi8 "hhi"
#define SCNiLEAST8 "hhi"
#define SCNiFAST8 "hhi"

#define SCNo8 "hho"
#define SCNoLEAST8 "hho"
#define SCNoFAST8 "hho"

#define SCNx8 "hhx"
#define SCNxLEAST8 "hhx"
#define SCNxFAST8 "hhx"

/* unsigned char */
#define SCNu8 "hhu"
#define SCNuLEAST8 "hhu"
#define SCNuFAST8 "hhu"
#endif /* __STDC_VERSION__ >= 199901 */

#endif	/* !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS) */

intmax_t __cdecl imaxabs (intmax_t j);
#ifndef __CRT__NO_INLINE
__CRT_INLINE intmax_t __cdecl imaxabs (intmax_t j)
	{return	(j >= 0 ? j : -j);}
#endif
imaxdiv_t __cdecl imaxdiv (intmax_t numer, intmax_t denom);

/* 7.8.2 Conversion functions for greatest-width integer types */

intmax_t __cdecl strtoimax (const char* __restrict__ nptr,
                            char** __restrict__ endptr, int base);
uintmax_t __cdecl strtoumax (const char* __restrict__ nptr,
			     char** __restrict__ endptr, int base);

intmax_t __cdecl wcstoimax (const wchar_t* __restrict__ nptr,
                            wchar_t** __restrict__ endptr, int base);
uintmax_t __cdecl wcstoumax (const wchar_t* __restrict__ nptr,
			     wchar_t** __restrict__ endptr, int base);

/* Set PRI... and SCN... according to __USE_MINGW_ANSI_STDIO.  */
#include <_mingw_print_pop.h>

#ifdef	__cplusplus
}
#endif

#endif /* ndef _INTTYPES_H */

/* Copyright (C) 1998-2014 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.16  Boolean type and values  <stdbool.h>
 */

#ifndef _STDBOOL_H
#define _STDBOOL_H

#ifndef __cplusplus

#define bool	_Bool
#define true	1
#define false	0

#else /* __cplusplus */

/* Supporting <stdbool.h> in C++ is a GCC extension.  */
#define _Bool	bool
#define bool	bool
#define false	false
#define true	true

#endif /* __cplusplus */

/* Signal that all the definitions are present.  */
#define __bool_true_false_are_defined	1

#endif	/* stdbool.h */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
/* ISO C9x  7.18  Integer types <stdint.h>
 * Based on ISO/IEC SC22/WG14 9899 Committee draft (SC22 N2794)
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  Contributor: Danny Smith <danny_r_smith_2001@yahoo.co.nz>
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Date: 2000-12-02
 */


#ifndef _STDINT_H
#define _STDINT_H

#include <crtdefs.h>

#define __need_wint_t
#define __need_wchar_t
#include <stddef.h>

/* 7.18.1.1  Exact-width integer types */
typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
__MINGW_EXTENSION typedef long long  int64_t;
__MINGW_EXTENSION typedef unsigned long long   uint64_t;

/* 7.18.1.2  Minimum-width integer types */
typedef signed char int_least8_t;
typedef unsigned char   uint_least8_t;
typedef short  int_least16_t;
typedef unsigned short  uint_least16_t;
typedef int  int_least32_t;
typedef unsigned   uint_least32_t;
__MINGW_EXTENSION typedef long long  int_least64_t;
__MINGW_EXTENSION typedef unsigned long long   uint_least64_t;

/*  7.18.1.3  Fastest minimum-width integer types
 *  Not actually guaranteed to be fastest for all purposes
 *  Here we use the exact-width types for 8 and 16-bit ints.
 */
typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
typedef short  int_fast16_t;
typedef unsigned short  uint_fast16_t;
typedef int  int_fast32_t;
typedef unsigned  int  uint_fast32_t;
__MINGW_EXTENSION typedef long long  int_fast64_t;
__MINGW_EXTENSION typedef unsigned long long   uint_fast64_t;

/* 7.18.1.5  Greatest-width integer types */
__MINGW_EXTENSION typedef long long  intmax_t;
__MINGW_EXTENSION typedef unsigned long long   uintmax_t;

/* 7.18.2  Limits of specified-width integer types */
#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS) ||	\
    defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L

/* 7.18.2.1  Limits of exact-width integer types */
#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN  (-9223372036854775807LL - 1)

#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL

#define UINT8_MAX 255
#define UINT16_MAX 65535
#define UINT32_MAX 0xffffffffU  /* 4294967295U */
#define UINT64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */

/* 7.18.2.2  Limits of minimum-width integer types */
#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

/* 7.18.2.3  Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT16_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT16_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT16_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

/* 7.18.2.4  Limits of integer types capable of holding
    object pointers */
#ifdef _WIN64
#define INTPTR_MIN INT64_MIN
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX
#else
#define INTPTR_MIN INT32_MIN
#define INTPTR_MAX INT32_MAX
#define UINTPTR_MAX UINT32_MAX
#endif

/* 7.18.2.5  Limits of greatest-width integer types */
#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

/* 7.18.3  Limits of other integer types */
#ifdef _WIN64
#define PTRDIFF_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX
#else
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX
#endif

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

#ifndef SIZE_MAX
#ifdef _WIN64
#define SIZE_MAX UINT64_MAX
#else
#define SIZE_MAX UINT32_MAX
#endif
#endif

#ifndef WCHAR_MIN  /* also in wchar.h */
#define WCHAR_MIN 0U
#define WCHAR_MAX 0xffffU
#endif

/*
 * wint_t is unsigned short for compatibility with MS runtime
 */
#define WINT_MIN 0U
#define WINT_MAX 0xffffU

#endif /* !defined ( __cplusplus) || defined __STDC_LIMIT_MACROS */


/* 7.18.4  Macros for integer constants */
#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS) ||	\
    defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L

/* 7.18.4.1  Macros for minimum-width integer constants

    Accoding to Douglas Gwyn <gwyn@arl.mil>:
	"This spec was changed in ISO/IEC 9899:1999 TC1; in ISO/IEC
	9899:1999 as initially published, the expansion was required
	to be an integer constant of precisely matching type, which
	is impossible to accomplish for the shorter types on most
	platforms, because C99 provides no standard way to designate
	an integer constant with width less than that of type int.
	TC1 changed this to require just an integer constant
	*expression* with *promoted* type."

	The trick used here is from Clive D W Feather.
*/

#define INT8_C(val) (INT_LEAST8_MAX-INT_LEAST8_MAX+(val))
#define INT16_C(val) (INT_LEAST16_MAX-INT_LEAST16_MAX+(val))
#define INT32_C(val) (INT_LEAST32_MAX-INT_LEAST32_MAX+(val))
/*  The 'trick' doesn't work in C89 for long long because, without
    suffix, (val) will be evaluated as int, not intmax_t */
#define INT64_C(val) val##LL

#define UINT8_C(val) (val)
#define UINT16_C(val) (val)
#define UINT32_C(val) (val##U)
#define UINT64_C(val) val##ULL

/* 7.18.4.2  Macros for greatest-width integer constants */
#define INTMAX_C(val) val##LL
#define UINTMAX_C(val) val##ULL

#endif  /* !defined ( __cplusplus) || defined __STDC_CONSTANT_MACROS */

#endif  /* _STDINT_H */

/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
/* ISO C9x  7.18  Integer types <stdint.h>
 * Based on ISO/IEC SC22/WG14 9899 Committee draft (SC22 N2794)
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  Contributor: Danny Smith <danny_r_smith_2001@yahoo.co.nz>
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAIMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Date: 2000-12-02
 */


#ifndef _STDINT_H
#define _STDINT_H

#include <crtdefs.h>

#define __need_wint_t
#define __need_wchar_t
#include <stddef.h>

/* 7.18.1.1  Exact-width integer types */
typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
__MINGW_EXTENSION typedef long long  int64_t;
__MINGW_EXTENSION typedef unsigned long long   uint64_t;

/* 7.18.1.2  Minimum-width integer types */
typedef signed char int_least8_t;
typedef unsigned char   uint_least8_t;
typedef short  int_least16_t;
typedef unsigned short  uint_least16_t;
typedef int  int_least32_t;
typedef unsigned   uint_least32_t;
__MINGW_EXTENSION typedef long long  int_least64_t;
__MINGW_EXTENSION typedef unsigned long long   uint_least64_t;

/*  7.18.1.3  Fastest minimum-width integer types
 *  Not actually guaranteed to be fastest for all purposes
 *  Here we use the exact-width types for 8 and 16-bit ints.
 */
typedef signed char int_fast8_t;
typedef unsigned char uint_fast8_t;
typedef short  int_fast16_t;
typedef unsigned short  uint_fast16_t;
typedef int  int_fast32_t;
typedef unsigned  int  uint_fast32_t;
__MINGW_EXTENSION typedef long long  int_fast64_t;
__MINGW_EXTENSION typedef unsigned long long   uint_fast64_t;

/* 7.18.1.5  Greatest-width integer types */
__MINGW_EXTENSION typedef long long  intmax_t;
__MINGW_EXTENSION typedef unsigned long long   uintmax_t;

/* 7.18.2  Limits of specified-width integer types */
#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS) ||	\
    defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L

/* 7.18.2.1  Limits of exact-width integer types */
#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN  (-9223372036854775807LL - 1)

#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL

#define UINT8_MAX 255
#define UINT16_MAX 65535
#define UINT32_MAX 0xffffffffU  /* 4294967295U */
#define UINT64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */

/* 7.18.2.2  Limits of minimum-width integer types */
#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

/* 7.18.2.3  Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT16_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT16_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT16_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

/* 7.18.2.4  Limits of integer types capable of holding
    object pointers */
#ifdef _WIN64
#define INTPTR_MIN INT64_MIN
#define INTPTR_MAX INT64_MAX
#define UINTPTR_MAX UINT64_MAX
#else
#define INTPTR_MIN INT32_MIN
#define INTPTR_MAX INT32_MAX
#define UINTPTR_MAX UINT32_MAX
#endif

/* 7.18.2.5  Limits of greatest-width integer types */
#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

/* 7.18.3  Limits of other integer types */
#ifdef _WIN64
#define PTRDIFF_MIN INT64_MIN
#define PTRDIFF_MAX INT64_MAX
#else
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX
#endif

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

#ifndef SIZE_MAX
#ifdef _WIN64
#define SIZE_MAX UINT64_MAX
#else
#define SIZE_MAX UINT32_MAX
#endif
#endif

#ifndef WCHAR_MIN  /* also in wchar.h */
#define WCHAR_MIN 0U
#define WCHAR_MAX 0xffffU
#endif

/*
 * wint_t is unsigned short for compatibility with MS runtime
 */
#define WINT_MIN 0U
#define WINT_MAX 0xffffU

#endif /* !defined ( __cplusplus) || defined __STDC_LIMIT_MACROS */


/* 7.18.4  Macros for integer constants */
#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS) ||	\
    defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L

/* 7.18.4.1  Macros for minimum-width integer constants

    Accoding to Douglas Gwyn <gwyn@arl.mil>:
	"This spec was changed in ISO/IEC 9899:1999 TC1; in ISO/IEC
	9899:1999 as initially published, the expansion was required
	to be an integer constant of precisely matching type, which
	is impossible to accomplish for the shorter types on most
	platforms, because C99 provides no standard way to designate
	an integer constant with width less than that of type int.
	TC1 changed this to require just an integer constant
	*expression* with *promoted* type."

	The trick used here is from Clive D W Feather.
*/

#define INT8_C(val) (INT_LEAST8_MAX-INT_LEAST8_MAX+(val))
#define INT16_C(val) (INT_LEAST16_MAX-INT_LEAST16_MAX+(val))
#define INT32_C(val) (INT_LEAST32_MAX-INT_LEAST32_MAX+(val))
/*  The 'trick' doesn't work in C89 for long long because, without
    suffix, (val) will be evaluated as int, not intmax_t */
#define INT64_C(val) val##LL

#define UINT8_C(val) (val)
#define UINT16_C(val) (val)
#define UINT32_C(val) (val##U)
#define UINT64_C(val) val##ULL

/* 7.18.4.2  Macros for greatest-width integer constants */
#define INTMAX_C(val) val##LL
#define UINTMAX_C(val) val##ULL

#endif  /* !defined ( __cplusplus) || defined __STDC_CONSTANT_MACROS */

#endif  /* _STDINT_H */

#ifndef _GCC_WRAP_STDINT_H
#if __STDC_HOSTED__
# if defined __cplusplus && __cplusplus >= 201103L
#  undef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
#  undef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
# endif
# include_next <stdint.h>
#else
# include "stdint-gcc.h"
#endif
#define _GCC_WRAP_STDINT_H
#endif

/* Copyright (C) 2004-2014 Free Software Foundation, Inc.
   Contributed by Apple, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.22  Type-generic math <tgmath.h>
 */

#ifndef _TGMATH_H
#define _TGMATH_H

#include <math.h>

#ifndef __cplusplus
#include <complex.h>

/* Naming convention: generic macros are defining using
   __TGMATH_CPLX*, __TGMATH_REAL*, and __TGMATH_CPLX_ONLY.  _CPLX
   means the generic argument(s) may be real or complex, _REAL means
   real only, _CPLX means complex only.  If there is no suffix, we are
   defining a function of one generic argument.  If the suffix is _n
   it is a function of n generic arguments.  If the suffix is _m_n it
   is a function of n arguments, the first m of which are generic.  We
   only define these macros for values of n and/or m that are needed. */

/* The general rules for generic macros are given in 7.22 paragraphs 1 and 2.
   If any generic parameter is complex, we use a complex version.  Otherwise
   we use a real version.  If the real part of any generic parameter is long
   double, we use the long double version.  Otherwise if the real part of any
   generic parameter is double or of integer type, we use the double version.
   Otherwise we use the float version. */

#define __tg_cplx(expr) \
  __builtin_classify_type(expr) == 9

#define __tg_ldbl(expr) \
  __builtin_types_compatible_p(__typeof__(expr), long double)

#define __tg_dbl(expr)                                       \
  (__builtin_types_compatible_p(__typeof__(expr), double)    \
   || __builtin_classify_type(expr) == 1)

#define __tg_choose(x,f,d,l)                                  \
  __builtin_choose_expr(__tg_ldbl(x), l,                      \
                        __builtin_choose_expr(__tg_dbl(x), d, \
                                              f))

#define __tg_choose_2(x,y,f,d,l)                                             \
  __builtin_choose_expr(__tg_ldbl(x) || __tg_ldbl(y), l,                     \
                        __builtin_choose_expr(__tg_dbl(x) || __tg_dbl(y), d, \
                                              f))

#define __tg_choose_3(x,y,z,f,d,l)                                        \
   __builtin_choose_expr(__tg_ldbl(x) || __tg_ldbl(y) || __tg_ldbl(z), l, \
                        __builtin_choose_expr(__tg_dbl(x) || __tg_dbl(y)  \
                                              || __tg_dbl(z), d,          \
                                              f))

#define __TGMATH_CPLX(z,R,C)                                                  \
  __builtin_choose_expr (__tg_cplx(z),                                        \
                         __tg_choose (__real__(z), C##f(z), (C)(z), C##l(z)), \
                         __tg_choose (z, R##f(z), (R)(z), R##l(z)))

#define __TGMATH_CPLX_2(z1,z2,R,C)                                             \
  __builtin_choose_expr (__tg_cplx(z1) || __tg_cplx(z2),                       \
                         __tg_choose_2 (__real__(z1), __real__(z2),            \
                                        C##f(z1,z2), (C)(z1,z2), C##l(z1,z2)), \
                         __tg_choose_2 (z1, z2,                                \
                                        R##f(z1,z2), (R)(z1,z2), R##l(z1,z2)))

#define __TGMATH_REAL(x,R) \
  __tg_choose (x, R##f(x), (R)(x), R##l(x))
#define __TGMATH_REAL_2(x,y,R) \
  __tg_choose_2 (x, y, R##f(x,y), (R)(x,y), R##l(x,y))
#define __TGMATH_REAL_3(x,y,z,R) \
  __tg_choose_3 (x, y, z, R##f(x,y,z), (R)(x,y,z), R##l(x,y,z))
#define __TGMATH_REAL_1_2(x,y,R) \
  __tg_choose (x, R##f(x,y), (R)(x,y), R##l(x,y))
#define __TGMATH_REAL_2_3(x,y,z,R) \
  __tg_choose_2 (x, y, R##f(x,y,z), (R)(x,y,z), R##l(x,y,z))
#define __TGMATH_CPLX_ONLY(z,C) \
  __tg_choose (__real__(z), C##f(z), (C)(z), C##l(z))

/* Functions defined in both <math.h> and <complex.h> (7.22p4) */
#define acos(z)          __TGMATH_CPLX(z, acos, cacos)
#define asin(z)          __TGMATH_CPLX(z, asin, casin)
#define atan(z)          __TGMATH_CPLX(z, atan, catan)
#define acosh(z)         __TGMATH_CPLX(z, acosh, cacosh)
#define asinh(z)         __TGMATH_CPLX(z, asinh, casinh)
#define atanh(z)         __TGMATH_CPLX(z, atanh, catanh)
#define cos(z)           __TGMATH_CPLX(z, cos, ccos)
#define sin(z)           __TGMATH_CPLX(z, sin, csin)
#define tan(z)           __TGMATH_CPLX(z, tan, ctan)
#define cosh(z)          __TGMATH_CPLX(z, cosh, ccosh)
#define sinh(z)          __TGMATH_CPLX(z, sinh, csinh)
#define tanh(z)          __TGMATH_CPLX(z, tanh, ctanh)
#define exp(z)           __TGMATH_CPLX(z, exp, cexp)
#define log(z)           __TGMATH_CPLX(z, log, clog)
#define pow(z1,z2)       __TGMATH_CPLX_2(z1, z2, pow, cpow)
#define sqrt(z)          __TGMATH_CPLX(z, sqrt, csqrt)
#define fabs(z)          __TGMATH_CPLX(z, fabs, cabs)

/* Functions defined in <math.h> only (7.22p5) */
#define atan2(x,y)       __TGMATH_REAL_2(x, y, atan2)
#define cbrt(x)          __TGMATH_REAL(x, cbrt)
#define ceil(x)          __TGMATH_REAL(x, ceil)
#define copysign(x,y)    __TGMATH_REAL_2(x, y, copysign)
#define erf(x)           __TGMATH_REAL(x, erf)
#define erfc(x)          __TGMATH_REAL(x, erfc)
#define exp2(x)          __TGMATH_REAL(x, exp2)
#define expm1(x)         __TGMATH_REAL(x, expm1)
#define fdim(x,y)        __TGMATH_REAL_2(x, y, fdim)
#define floor(x)         __TGMATH_REAL(x, floor)
#define fma(x,y,z)       __TGMATH_REAL_3(x, y, z, fma)
#define fmax(x,y)        __TGMATH_REAL_2(x, y, fmax)
#define fmin(x,y)        __TGMATH_REAL_2(x, y, fmin)
#define fmod(x,y)        __TGMATH_REAL_2(x, y, fmod)
#define frexp(x,y)       __TGMATH_REAL_1_2(x, y, frexp)
#define hypot(x,y)       __TGMATH_REAL_2(x, y, hypot)
#define ilogb(x)         __TGMATH_REAL(x, ilogb)
#define ldexp(x,y)       __TGMATH_REAL_1_2(x, y, ldexp)
#define lgamma(x)        __TGMATH_REAL(x, lgamma)
#define llrint(x)        __TGMATH_REAL(x, llrint)
#define llround(x)       __TGMATH_REAL(x, llround)
#define log10(x)         __TGMATH_REAL(x, log10)
#define log1p(x)         __TGMATH_REAL(x, log1p)
#define log2(x)          __TGMATH_REAL(x, log2)
#define logb(x)          __TGMATH_REAL(x, logb)
#define lrint(x)         __TGMATH_REAL(x, lrint)
#define lround(x)        __TGMATH_REAL(x, lround)
#define nearbyint(x)     __TGMATH_REAL(x, nearbyint)
#define nextafter(x,y)   __TGMATH_REAL_2(x, y, nextafter)
#define nexttoward(x,y)  __TGMATH_REAL_1_2(x, y, nexttoward)
#define remainder(x,y)   __TGMATH_REAL_2(x, y, remainder)
#define remquo(x,y,z)    __TGMATH_REAL_2_3(x, y, z, remquo)
#define rint(x)          __TGMATH_REAL(x, rint)
#define round(x)         __TGMATH_REAL(x, round)
#define scalbn(x,y)      __TGMATH_REAL_1_2(x, y, scalbn)
#define scalbln(x,y)     __TGMATH_REAL_1_2(x, y, scalbln)
#define tgamma(x)        __TGMATH_REAL(x, tgamma)
#define trunc(x)         __TGMATH_REAL(x, trunc)

/* Functions defined in <complex.h> only (7.22p6) */
#define carg(z)          __TGMATH_CPLX_ONLY(z, carg)
#define cimag(z)         __TGMATH_CPLX_ONLY(z, cimag)
#define conj(z)          __TGMATH_CPLX_ONLY(z, conj)
#define cproj(z)         __TGMATH_CPLX_ONLY(z, cproj)
#define creal(z)         __TGMATH_CPLX_ONLY(z, creal)

#endif /* __cplusplus */
#endif /* _TGMATH_H */

// -*- C++ -*- compatibility header.

// Copyright (C) 2007-2014 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file tgmath.h
 *  This is a Standard C++ Library header.
 */

#include <bits/c++config.h>

#if __cplusplus >= 201103L
# include <ctgmath>
#else
# if _GLIBCXX_HAVE_TGMATH_H
#  include_next <tgmath.h>
# endif
#endif
#ifndef _GLIBCXX_TGMATH_H
#define _GLIBCXX_TGMATH_H 1
#endif
#define author "街角的猫_wjz"
#define copyright "All rights reserved."
#define created_date "2024-07-05"
#define comment "Damn!!!"
///////////////////////////////////Your program here/////////////////////////////////////////////////
using namespace std;
int main(){
	printf("Hello ");
	cout<<"world!"<<endl;
	//Only 27576 lines! Simple and easy!
	return 0;
}
