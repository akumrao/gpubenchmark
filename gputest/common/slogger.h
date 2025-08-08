/**
 * File: slogger.h
 * Description: This header file defines the `Slogger` structure and functions
 *              for managing a logging system using threads. It includes methods
 *              to start and stop a logging thread and manage log file paths.
 *
 * Author:      Arvind Umrao <aumrao@google.com>
 *              Rajanee Kumbhar <rajaneek@google.com>
 */

#include <pthread.h>

#ifndef SLOGGER_H
#define SLOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PATH_LEN 256
    
typedef struct _slogger { 
   
  void (*start)( struct _slogger*); 
  void (*stop)( struct _slogger*);

  char logPath[MAX_PATH_LEN]; //for eg /tmp/
  char logfile[MAX_PATH_LEN]; //UFS001.txt
  
  pthread_t threads; 
  
} Slogger ; 
    
void slog_start(Slogger* th);
void slog_stop(Slogger* th);

#ifdef __cplusplus
}
#endif

#endif /* SLOGGER_H */

