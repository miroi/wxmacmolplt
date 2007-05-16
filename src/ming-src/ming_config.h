/* src/ming_config.h.  Generated from ming_config.h.in by configure.  */
/* src/ming_config.h.in.  Generated from configure.in by autoheader.  */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 0

/* Define to 1 if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define to 1 if you have the <zlib.h> header file. */
#ifdef WIN32
#define HAVE_ZLIB_H 0
#else
#define HAVE_ZLIB_H 1
#endif

/* Name of package */
#define PACKAGE "ming"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "ming"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ming 0.4.0.beta4"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ming"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.4.0.beta4"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define this if you want Ming to track all objects allocations. Ming will
   mantain a doubly linked list of allocated objects, call
   Ming_collectGarbage() to get rid of them all */
#define TRACK_ALLOCS 1

/* Use c++ */
#define USE_CXX 1

/* Use freetype library */
#define USE_FREETYPE 1

/* Use gif library */
/* #undef USE_GIF */

/* Use png library */
#define USE_PNG 0

/* Use zlib */
#ifdef WIN32
#define USE_ZLIB 0
#else
#define USE_ZLIB 0
#endif

/* Version number of package */
#define VERSION "0.4.0.beta4"

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#define YYTEXT_POINTER 1

#ifdef WIN32
#ifndef __STRING
#define __STRING(x) #x
#endif
#endif
