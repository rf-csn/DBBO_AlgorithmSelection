/**
 * @file IOHprofiler_platform.h
 * @brief Automatic platform-dependent configuration of the IOHprofiler framework.
 *
 * Some platforms and standard conforming compilers require extra defines or includes to provide some
 * functionality.
 *
 * Because most feature defines need to be set before the first system header is included and we do not
 * know when a system header is included for the first time in the amalgamation, all internal files
 * that need these definitions should include this file before any system headers.
 */

#ifndef __COCO_PLATFORM__
#define __COCO_PLATFORM__

#include <stddef.h>

/* Definitions of IOHprofiler_PATH_MAX, IOHprofiler_path_separator, HAVE_GFA and HAVE_STAT heavily used by functions in
 * IOHprofiler_utilities.c */
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__) || defined(__CYGWIN__)
#include <windows.h>
static const char *IOHprofiler_path_separator = "\\";
#define IOHprofiler_PATH_MAX MAX_PATH
#define HAVE_GFA 1
#elif defined(__gnu_linux__)
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
static const char *IOHprofiler_path_separator = "/";
#define HAVE_STAT 1
#define IOHprofiler_PATH_MAX PATH_MAX
#elif defined(__APPLE__)
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <sys/types.h>
static const char *IOHprofiler_path_separator = "/";
#define HAVE_STAT 1
#define IOHprofiler_PATH_MAX PATH_MAX
#elif defined(__FreeBSD__)
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
static const char *IOHprofiler_path_separator = "/";
#define HAVE_STAT 1
#define IOHprofiler_PATH_MAX PATH_MAX
#elif (defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__))
/* Solaris */
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
static const char *IOHprofiler_path_separator = "/";
#define HAVE_STAT 1
#define IOHprofiler_PATH_MAX PATH_MAX
#else
#error Unknown platform
#endif
#if !defined(IOHprofiler_PATH_MAX)
#error IOHprofiler_PATH_MAX undefined
#endif

/* Definitions needed for creating and removing directories */
/* Separately handle the special case of Microsoft Visual Studio 2008 with x86_64-w64-mingw32-gcc */
#if _MSC_VER
#include <direct.h>
#elif defined(__MINGW32__) || defined(__MINGW64__)
#include <dirent.h>
#else
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

/* To silence the compiler (implicit-function-declaration warning). */
/** @cond */
int rmdir(const char *pathname);
int unlink(const char *file_name);
int mkdir(const char *pathname, mode_t mode);
/** @endcond */
#endif

/* Definition of the S_IRWXU constant needed to set file permissions */
#if defined(HAVE_GFA)
#define S_IRWXU 0700
#endif

/* To silence the Visual Studio compiler (C4996 warnings in the python build). */
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#ifdef __cplusplus
}
#endif

#endif
