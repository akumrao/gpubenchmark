/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * Filename:        common.c
 * Description:     Implements common functions and global variables.
 *                  This file provides implementations for several core
 *                  functionalities used across the subsystems:
 *
 * Author:          Arvind Umrao <aumrao@google.com>
 * 
 */

#include "common.h"
#include <sys/time.h>

#if defined(__ANDROID__) 
#include <android/log.h>
#endif


/**
 * Function:        void log_message(int log_lvl, FILE *fp, const char *tag, const char *fmt, ...)
 * Description:     Logs messages based on the specified log level and log type.
 *                  Supports logging to ADB shell, Logcat, Serial, and file output.
 * Parameters:      log_lvl - int       : Log level (INFO, ERROR, DEBUG, WARN).
 *                  fp      - FILE*     : File pointer for logging output (if applicable).
 *                  tag     - char*     : Tag for categorizing log messages.
 *                  fmt     - char*     : Format string for the log message.
 * Returns:         void
 */

void log_message(int log_lvl, FILE *fp, const char *tag, const char *fmt, ...)
{
    if (log_lvl <= console_loglevel) {
        va_list args;
        va_start(args, fmt);

        /* Time and milliseconds for timestamp */
        time_t now = time(NULL);
        struct tm *local_time = localtime(&now);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int msec = tv.tv_usec / 1000;
        char timestamp[80];
        strftime(timestamp, sizeof(timestamp), "%b-%d %H:%M:%S", local_time);

        switch (log_type) {
            case LOG_TYPE_LOGCAT:
                switch (log_lvl) {
                    #if defined(__ANDROID__) 
                    case LOG_INFO: __android_log_vprint(ANDROID_LOG_INFO, tag, fmt, args); break;
                    case LOG_ERR: __android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, args); break;
                    case LOG_DEBUG: __android_log_vprint(ANDROID_LOG_WARN, tag, fmt, args); break;
                    default: __android_log_print(ANDROID_LOG_WARN, tag, "UNKNOWN: %s", fmt); break;
                    #endif    
                }
            case LOG_TYPE_ADBSHELL:
                switch (log_lvl) {
                    case LOG_INFO: printf(COLOR_BLUE "%s.%03d  INFO %s: ", timestamp, msec, tag); VAR_LOG; break;
                    case LOG_ERR: printf(COLOR_RED "%s.%03d ERROR %s: ", timestamp, msec, tag); VAR_LOG; break;
                    case LOG_DEBUG: printf(COLOR_YELLOW "%s.%03d DEBUG %s: ", timestamp, msec, tag); VAR_LOG; break;
                    default: printf(COLOR_GREEN "%s.%03d  WARN %s: ", timestamp, msec, tag); VAR_LOG; break;
                }
                break;
            case LOG_TYPE_SERIAL:
                break;
            default:
                break;
        }
        va_end(args);

        /* Log to file if the file pointer is not NULL */
        if (fp != NULL) {
            switch (log_lvl) {
                case LOG_INFO: fprintf(fp, "%s.%03d  INFO %s:  ", timestamp, msec, tag); break;
                case LOG_ERR: fprintf(fp, "%s.%03d ERROR %s:  ", timestamp, msec, tag); break;
                case LOG_DEBUG: fprintf(fp, "%s.%03d DEBUG %s:  ", timestamp, msec, tag); break;
                default: fprintf(fp, "%s.%03d  WARN %s:  ", timestamp, msec, tag); break;
            }
            va_start(args, fmt); /* Restart the variable argument list */
            vfprintf(fp, fmt, args); fprintf(fp, "\n"); fflush(fp);
            va_end(args);
        }
    }
}

/**
 * Function:        int set_log_type(const char *config_value)
 * Description:     Sets the logging type based on the provided configuration value.
 *                  Supports ADB shell, Logcat, and Serial logging modes.
 * Parameters:      config_value - char* : String specifying the log type.
 * Returns:         int - SUCCESS on setting log type.
 */

int set_log_type(const char *config_value) {
    if (strcmp(config_value, "adbshell") == 0) {
        log_type = LOG_TYPE_ADBSHELL;
    } else if (strcmp(config_value, "logcat_shell") == 0) {
        log_type = LOG_TYPE_LOGCAT;
    } else if (strcmp(config_value, "serial") == 0) {
        log_type = LOG_TYPE_SERIAL;
    } else {
        //log_message(LOG_WARN, log_fp,  "Invalid log mode: %s. Defaulting to console.",FL, config_value);
        log_type = LOG_TYPE_ADBSHELL;
    }
    return SUCCESS;
}



#define LOG_BUFFER_SIZE 1024

/**
 * Function:        void slog_message(int log_lvl, const char *tag, const char *fmt, ...)
 * Description:     Logs messages to different outputs based on log type and level.
 *                  Supports logging to Logcat (Android), ADB shell, serial, and file.
 * Parameters:      log_lvl  - int       : Logging level (INFO, ERROR, DEBUG, etc.).
 *                  tag      - char*     : Tag for log identification.
 *                  fmt      - char*     : Format string for log message.
 *                  ...      - va_list   : Variable arguments for formatted message.
 * Returns:         void
 */

void slog_message(int log_lvl, const char *tag, const char *fmt, ...) {

    if (log_lvl <= console_loglevel) {
        va_list args;
        va_start(args, fmt);

        /* Time and milliseconds for timestamp */
        time_t now = time(NULL);
        struct tm *local_time = localtime(&now);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int msec = tv.tv_usec / 1000;
        char timestamp[80];
        strftime(timestamp, sizeof(timestamp), "%b-%d %H:%M:%S", local_time);

        switch (log_type) {
            case LOG_TYPE_LOGCAT:
                switch (log_lvl) {
                    #if defined(__ANDROID__) 
                    case LOG_INFO: __android_log_vprint(ANDROID_LOG_INFO, tag, fmt, args); break;
                    case LOG_ERR: __android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, args); break;
                    case LOG_DEBUG: __android_log_vprint(ANDROID_LOG_WARN, tag, fmt, args); break;
                    default: __android_log_print(ANDROID_LOG_WARN, tag, "UNKNOWN: %s", fmt); break;
                    #endif    
                }
            case LOG_TYPE_ADBSHELL:
                switch (log_lvl) {
                    case LOG_INFO: printf(COLOR_BLUE "%s.%03d  INFO %s: ", timestamp, msec, tag); VAR_LOG; break;
                    case LOG_ERR: printf(COLOR_RED "%s.%03d ERROR %s: ", timestamp, msec, tag); VAR_LOG; break;
                    case LOG_DEBUG: printf(COLOR_YELLOW "%s.%03d DEBUG %s: ", timestamp, msec, tag); VAR_LOG; break;
                    default: printf(COLOR_GREEN "%s.%03d  WARN %s: ", timestamp, msec, tag); VAR_LOG; break;
                }
                break;
            case LOG_TYPE_SERIAL:
                break;
            default:
                break;
        }
        va_end(args);

         char *store = (char *)malloc(2048);
         if(!store)
              return;
         
        int ncount = 0;

        /* Log to file if the file pointer is not NULL */
       // if (fp != NULL) 
        {
            switch (log_lvl) {
                case LOG_INFO: ncount += sprintf(&store[ncount], "%s.%03d  INFO %s:  ", timestamp, msec, tag); break;
                case LOG_ERR: ncount += sprintf(&store[ncount], "%s.%03d ERROR %s:  ", timestamp, msec, tag); break;
                case LOG_DEBUG: ncount += sprintf(&store[ncount], "%s.%03d DEBUG %s:  ", timestamp, msec, tag); break;
                default: ncount += sprintf(&store[ncount], "%s.%03d  WARN %s:  ", timestamp, msec, tag); break;
            }
            va_start(args, fmt); /* Restart the variable argument list */
            ncount += vsprintf(&store[ncount], fmt, args);  ncount += sprintf(&store[ncount], "\n"); //fflush(fp);
            va_end(args);
        }
        pushMessage(store , strlen(store) );
    }
}

/**
 * Function:        char* trim(char *str)
 * Description:     Removes leading and trailing whitespace from a given string.
 * Parameters:      str  - char*  : Input string to be trimmed.
 * Returns:         char*  - Pointer to the trimmed string.
 */

char *trim(char *str) {
    char *start = str;
    char *end = NULL;

    /* Trim leading spaces */
    while (isspace((unsigned char)*start)) {
        start++;
    }

    /* Check for empty string (all spaces) */
    if (*start == '\0') {
        *str = '\0';
        return str;
    }

    /* Trim trailing spaces */
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    return start;
}
