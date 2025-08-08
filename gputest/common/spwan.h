/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      threadload.c
 * Description:   File have execvp run with thread.
 *
 * Author:        Arvind Umrao <aumrao@google.com>
 *

Fixed race condition with exec and waitpid and signal handler
Removed extern variables
Removed usleep
Generic reusable common structure with init, start, run and stop like java apis.
Reusable code for Execvp
Absolute log time with millisecond precision
One Entry one exit of program flow
removed all _exit
Fixed all the V2 issues for all.
Code is very thin now

*/

#ifndef EXEC_H
#define EXEC_H

#include <pthread.h>
#include <stdatomic.h>

#define MAX_PATH_LENGTH 256

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _execload { 

  void (*start)( struct _execload*);
  void (*run)( struct _execload*); 
  void (*stop)( struct _execload*); 
  
  char logPath[MAX_PATH_LENGTH]; //for eg /tmp/
  char logfile[MAX_PATH_LENGTH]; //UFS001.txt  
  
  char **argv;
  
  /* For command which are non blocking and finishes fast for example ls set it true. For command like top set it false */
  bool blocking;
   
  atomic_bool keeprunning; 
  pthread_t threads; 
  
  FILE *thread_log;
  
  FILE *fp;
  
  int pid;
  
} Exec ; 

int exec(char* arg_list[], char **out, int *retSize  );
int exec_log(char* arg_list[] );

void exec_start(Exec* th);

void exec_run(Exec* th);

void exec_stop(Exec* th);



#ifdef __cplusplus
}
#endif

#endif  /* EXEC_H */


