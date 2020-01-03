/************************************************************
 * PROJECT: KLEIN
 * FILE:    LOG.H
 * PURPOSE: LOGGING MECHANISM
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef _LOG_H_INCLUDED
#define _LOG_H_INCLUDED

#include <syslog.h>
#include "const.h"
#include "typedefs.h"
#include "macros.h"

#if defined(DEBUG_MODE_ENABLED)
#define LOG_IDENT                                       VERSION
#define LOG_OPT                                         (LOG_PID | LOG_CONS)
#define LOG_FACILITY                                    LOG_USER
#define LOG_PRIORITY                                    LOG_DEBUG
#define DEFAULT_LOG_MSG                                 errtype_to_str(es_malloc_failed)

#if (COMPILER_IS_GNUC)
#define _OPEN_LOG() {                                                   \
    openlog(LOG_IDENT, LOG_OPT, LOG_FACILITY);                          \
}
#define _CLOSE_LOG() {                                                  \
    closelog();                                                         \
}

#define SYSLOGF(fmt_, ...) {                                            \
    CHAR8 log[MAX_MESSAGE_SIZE];                                        \
    INT32 rc = sprintf(log,                                             \
                        "[%s: %s:%d] **DBG: " fmt_ "\n",                \
                        LOG_IDENT,                                      \
                        __FILE__,                                       \
                        __LINE__,                                       \
                        ##__VA_ARGS__);                                 \
    _OPEN_LOG();                                                        \
    syslog(LOG_PRIORITY, IS_EQUAL(rc, EOF) ? DEFAULT_LOG_MSG : log);    \
    _CLOSE_LOG();                                                       \
}
#else
#define SYSLOGF(fmt_, ...) {                                            \
    CHAR8 log[MAX_MESSAGE_SIZE];                                        \
    INT32 rc = sprintf(log,                                             \
                        "[%s: %s:%d] **DBG: " fmt_ "\n",                \
                        LOG_IDENT,                                      \
                        __FILE__,                                       \
                        __LINE__,                                       \
                        ##__VA_ARGS__);                                 \
    _OPEN_LOG();                                                        \
    syslog(LOG_PRIORITY, IS_EQUAL(rc, EOF) ? DEFAULT_LOG_MSG : log);    \
    _CLOSE_LOG();                                                       \
}
#endif
#else
#define SYSLOGF(fmt_, ...)
#endif

#if (COMPILER_IS_GNUC)
#define DIEF(fmt_, ...) {                                               \
    fprintf(stderr,                                                     \
            "[%s: %s:%d] **ERR: " fmt_ "\n",                            \
            VERSION,                                                    \
            __FILE__,                                                   \
            __LINE__,                                                   \
            ##__VA_ARGS__);                                             \
    fflush(stderr);                                                     \
    abort_klein();                                                      \
}
#else
#define DIEF(fmt_, ...) {                                               \
    fprintf(stderr,                                                     \
            "[%s: %s:%d] **ERR: " fmt_ "\n",                            \
            VERSION,                                                    \
            __FILE__,                                                   \
            __LINE__,                                                   \
            __VA_ARGS__);                                               \
    fflush(stderr);                                                     \
    abort_klein();                                                      \
}
#endif

#endif