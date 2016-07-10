
#ifndef REAPER_HW_COMPAT_H
#define REAPER_HW_COMPAT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _WIN32

# ifndef WIN32
#  define WIN32
# endif

# ifdef _MSC_VER
#  define MSVC
# endif

# if defined(__ICL) || defined(__ICC)
#  define ICC
#  undef MSVC
# endif

# ifdef __MINGW32__
#  define GCC
#  define MINGW32
#  define WIN32THR
#  define WIN32WIN
# endif

# define WIN32THR
# define WIN32WIN

# define IS_LITTLE_ENDIAN

#endif

#ifdef __CYGWIN__
# define GCC
# define CYGWIN
# define WIN32
# define WIN32THR
# define WIN32WIN
# define IS_LITTLE_ENDIAN
#endif

#ifdef __linux__

# define LINUX
# define GCC
# define PTHREADS
# define X11
# define IS_LITTLE_ENDIAN

#endif

#ifdef __FreeBSD__
# define FREEBSD
# define GCC
# define PTHREADS
# define X11
# define IS_LITTLE_ENDIAN
# ifdef poll
#  undef poll
# endif
#endif

#ifdef __sun__
# define SOLARIS
# define GCC
# define PTHREADS
# define X11
# define IS_BIG_ENDIAN
#endif

#ifdef __SUNPRO_CC
# define SOLARIS
# define GCC
# define PTHREADS
# define X11
# define IS_BIG_ENDIAN
#endif


#if (__GNUC__ == 2) && !defined(USE_STLPORT) && defined(__cplusplus)

#include <streambuf.h>
#include <iostream>

#define __SGI_STL_USE_AUTO_PTR_CONVERSIONS

namespace std {

#ifdef FIX_MISSING_STL_TRAITS
# ifdef NO_STREAMSIZE_IN_IOSTREAM
typedef int streamsize;
# endif

template <class Category, class Tp,
	typename Distance = int,
	typename Pointer = Tp*,
	typename Reference = Tp&>
struct iterator {
  typedef Category  iterator_category;
  typedef Tp        value_type;
  typedef Distance  difference_type;
  typedef Pointer   pointer;
  typedef Reference reference;
};

template<class T>
struct char_traits { };

template<>
struct char_traits<char> {
	typedef int int_type;
	typedef char char_type;
	static int eof() {
		return -1;
	}
	static int not_eof(const int& ch) {
		if (ch != -1)
			return ch;
		else
			return 0;
	}
	static char to_char_type(const int& ch) {
		return static_cast<char>(ch);
	}
	static int to_int_type(int ch) {
		return ch;
	}
	static bool eq_int_type(int a, int b) {
		return a == b;
	}
};
#endif

template<class T>
struct basic_streambuf
 : public std::streambuf
{
};

template<class T>
struct basic_iostream
 : public std::iostream
{
	basic_iostream(basic_streambuf<T>* p)
	 : std::iostream(p)
	{ }
};


struct dummy_type_foo_right { };
static dummy_type_foo_right right;
struct dummy_type_foo_fixed { };
static dummy_type_foo_fixed fixed;
template<class T>
void dummy_type_foo_f() {
	dummy_type_foo_right r = right;
	dummy_type_foo_fixed f = fixed;
}

inline
std::ostream& operator<<(std::ostream& os, dummy_type_foo_right) {
	os.setf(ios::right, ios::floatfield);
	return os;
}

inline
std::ostream& operator<<(std::ostream& os, dummy_type_foo_fixed) {
	os.setf(ios::fixed, ios::floatfield);
	return os;
}

inline
void boolalpha(std::ostream&)
{ }

}

#endif


#ifdef MSVC
// const double->float downcast
#pragma warning(disable: 4305)
#pragma warning(disable: 4244)

// C++ exception specification ignored
#pragma warning(disable: 4290)

// Signed/unsigned mismatch
#pragma warning(disable: 4018)

#endif

#if defined(MSVC) || defined(ICC)

// Warns when debug symbols gets truncated to 255 chars
// Very disturbing when compiling with nested templates
#pragma warning(disable: 4786)

#define for if (false) ; else for

#ifndef __SGI_STL_PORT
#if _MSC_VER < 1300
#ifdef __cplusplus
namespace std
{
        template<class T>
        inline const T& min(const T& x, const T& y)
        {
                return y<x ? y : x;
        }

        template<class T>
        inline const T& max(const T& x, const T& y)
        {
                return x<y ? y : x;
        }
}
#endif
#endif
#endif
#endif

#ifdef WIN32

# if defined(MINGW32) || defined(CYGWIN)
#  ifndef ENUM_CURRENT_SETTINGS
#   define ENUM_CURRENT_SETTINGS ((DWORD)-1)
#  endif

#  ifndef CDS_FULLSCREEN
#   define CDS_FULLSCREEN 4
#  endif
#  ifndef PFD_GENERIC_ACCELERATED
#   define PFD_GENERIC_ACCELERATED 0x00001000
#  endif
# endif


# ifndef CYGWIN
#  ifdef MINGW32

#ifdef __cplusplus
extern "C" {
#endif
int _isnan(double);
double _copysign(double, double);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
inline int isnan(double x) { return _isnan(x); }
inline double copysign(double x, double y) { return _copysign(x, y); }
#ifdef __cplusplus
}
#endif
#  else

#   define isnan _isnan
#   define copysign _copysign

#  endif

# endif

#define DLL_EXPORT __declspec(dllexport)

#endif

#ifdef GCC
# ifndef DLL_EXPORT
#  define DLL_EXPORT
# endif
#endif

#if defined(__STRICT_ANSI__) && __GNUC__!=3
#ifdef __cplusplus
extern "C" {
#endif
double copysign(double, double);
int isnan(double x);
#ifdef __cplusplus
}
#endif
#endif


#ifdef NO_TIME_H_TIMESPEC
#include <time.h>
struct timespec
{
        time_t tv_sec;
	long tv_nsec;
};
#endif

#ifdef NO_TIME_H_NANOSLEEP

extern "C" int nanosleep(const struct timespec *req, struct timespec *rem) throw ();

#endif


#ifdef MSVC
# if _MSC_VER < 1300
#  define HASH_NAMESPACE_MISC
#  define USE_MAP_AS_HASH_MAP
# else
#  define HAVE_HASH_MAP
#  define HASH_NAMESPACE_STD
# endif
#endif

#ifdef GCC
# if (__GNUC__ == 2) || defined(USE_STLPORT)
#  define HASH_NAMESPACE_NONE
#  if !defined(HAVE_EXT_HASH_MAP) && !defined(HAVE_HASH_MAP)
#   define HAVE_HASH_MAP
#  endif
# else
#  if (__GNUC_MINOR__ == 0)
#   define HASH_NAMESPACE_STD
#  else
#   define HASH_NAMESPACE_GNU_CXX
#  endif
#  ifndef HAVE_EXT_HASH_MAP
#   define HAVE_EXT_HASH_MAP
#  endif
# endif
#endif

#endif

