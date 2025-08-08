/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * Filename:        common.h
 * Description:     Common functionality defines here to use across the modules.
 *                  It consist of common functionality which used across the
 *                  all the subsystem test cases.
 *
 * Author:          Arvind Umrao <aumrao@google.com>
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/utsname.h>

#include <unistd.h>
#include <signal.h>

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define ETAG TAG, "[%s:%d] "
#define FL __FILE__,__LINE__


/* Define the Macro */
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 1024
#define VER_BUFFER_SIZE 64
#define SUCCESS 0
#define FAILURE 1

#define COLOR_RESET "\033[0m\n"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"

#define LOG_ERR     0   /* error conditions */
#define LOG_WARN    1   /* warning conditions */
#define LOG_INFO    2   /* informational */
#define LOG_DEBUG   3   /* debug-level messages */

#define VAR_LOG vprintf(fmt, args); printf(COLOR_RESET); fflush(stdout)

/* Global config for logging type */
typedef enum {
    LOG_TYPE_ADBSHELL,
    LOG_TYPE_LOGCAT,
    LOG_TYPE_SERIAL
} log_type_t;

extern log_type_t log_type;
extern int console_loglevel;

/*Log messages. */
void log_message(int log_lvl, FILE* fp,  const char *tag, const char *fmt, ...) ;

void pushMessage(const char *str , long size );

/* Removes leading and trailing whitespace from a string. */
char *trim(char *str);

/*Thread safe log messages */
void slog_message(int log_lvl, const char *tag, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* SLOGGER_H */

