/*!
 * \file   ModbusPlatform.h
 * \brief  Definition of platform specific macros.
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUSPLATFORM_H
#define MODBUSPLATFORM_H

#if defined (_WIN32)|| defined(_WIN64)|| defined(__WIN32__) || defined(__WINDOWS__)
#define MB_OS_WINDOWS
#endif

// Linux, BSD and Solaris define "unix", OSX doesn't, even though it derives from BSD
#if defined(unix) || defined(__unix__) || defined(__unix)
#define MB_PLATFORM_UNIX
#endif

#if BSD>=0
#define MB_OS_BSD
#endif

#if __APPLE__
#define MB_OS_OSX
#endif


#ifdef _MSC_VER

#define MB_DECL_IMPORT __declspec (dllimport)
#define MB_DECL_EXPORT __declspec (dllexport)

#else

#define MB_DECL_IMPORT
#define MB_DECL_EXPORT

#endif

#endif // MODBUSPLATFORM_H
