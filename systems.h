/************************************************************
 * PROJECT: KLEIN
 * FILE:    SYSTEMS.H
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef SYSTEMS_H
#define SYSTEMS_H

#undef IS_LINUX
#if defined(linux) || defined(__linux__) || defined(__linux)
#define IS_LINUX 1
#endif
#if !(IS_LINUX)
#error "[UNSUPPORTED_OS]"
#endif
#undef COMPILER_IS_GNUC
#if defined(__GNUC__)
#define COMPILER_IS_GNUC 1
#endif
#if !(COMPILER_IS_GNUC)
#define NORETURN_ATTRIB
#define CONST_ATTRIB
#define ALWAYS_INLINE_ATTRIB
#define MACRO_START             do {
#define MACRO_END             } while(0x0)
#else
#undef _GNU_SOURCE
#define _GNU_SOURCE
#define NORETURN_ATTRIB         __attribute__ ((noreturn))
#define CONST_ATTRIB            __attribute__((const))
#define ALWAYS_INLINE_ATTRIB    __attribute__((always_inline))
#define MACRO_START             {
#define MACRO_END             }
#endif

#endif