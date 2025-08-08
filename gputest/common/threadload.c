/*
 * Copyright (c) 2024-25 akumrao@yahoo.com
 *
 * FileName:      threadload.h
 * Description:   Thread abstraction

 * Author:        Arvind Umrao <aumrao@google.com>
 *                Rajanee Kumbhar <rajaneek@google.com>
 */

#include "common.h"
#include "threadload.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>
#include <signal.h>
#include "condwait.h"

/*
For thread load
*/

#define TAG "TC_GPU_001"

/**
 * Function:        void close_thread_log(FILE* thread_log)
 * Description:     Closes the thread's log file after ensuring that the content
 *                  is properly read and formatted. If the file has data, it is
 *                  read into a buffer, formatted with newline characters, and
 *                  pushed as a message before closing the file.
 * Parameters:      thread_log: FILE* - Pointer to the thread's log file.
 * Returns:         void - This function does not return a value.
 */

void close_thread_log (FILE* thread_log) {
    long fsize = ftell(thread_log);
   
    if(fsize > 0)
    {
        fseek(thread_log, 0, SEEK_SET);

        char *string =  (char *)malloc(fsize + 1+2);
        if(string)
        {
           int sz = fread(string, 1, fsize , thread_log);

           string[sz-1]= '\n';
           string[sz-2]= '\n';

           pushMessage(string, sz );
        }
    }
    fclose ((FILE*) thread_log);
    thread_log = NULL;
}

/**
 * Function:        void* load_thread(void * arg)
 * Description:     Entry point for a new thread that manages GPU load execution.
 *                  It initializes log file handling, determines logging mode (individual or clubbed),
 *                  and runs the thread's main execution function.
 * Parameters:      arg: void* - Pointer to a ThLoader structure containing thread configuration.
 * Returns:         void* - Always returns NULL upon thread completion.
 */
void* load_thread(void * arg) {
    ThLoader *th = (ThLoader *) arg;
    //printf("load_thread started\n");
    
    char thread_log_filename[1024]={'\0'};
     
   /* Generate the filename for this thread.s log file. */

    if(strlen(th->logfile))
        th->localFile = true;
    else
        th->localFile = false;

    if(th->localFile )
        sprintf (thread_log_filename, "%s%s", th->logPath, th->logfile);
    else if(th->clubbed)
        sprintf (thread_log_filename, "%sthread%d.log", th->logPath, (int) pthread_self ());

    if(strlen(thread_log_filename)) {
        th->thread_log = fopen(thread_log_filename, "w");
        if (th->thread_log == NULL) {
            fprintf(stderr, "Failed to open file %s \n", thread_log_filename);
            return NULL;
        }
    }

    /* Store the file pointer in thread-specific data under thread_log_key. */
    /* pthread_setspecific (thread_log_key, thread_log); */
    /* write_to_thread_log ("Thread starting."); */                   
  
    th->run(th);
    if(th->thread_log)
    {
        //slog_message(LOG_DEBUG, TAG, "End of thread tid = %ld  ",  (long) pthread_self () );
        fclose (th->thread_log);
    }
    else if(th->clubbed)
    {
        //log_message(LOG_DEBUG, th->thread_log, TAG, "End of thread tid = %ld ", (long) pthread_self () );
        close_thread_log(th->thread_log);
    }
    else
    {
        slog_message(LOG_DEBUG, TAG, "End of thread tid = %ld  ",  (long) pthread_self () );
    }
    return NULL;
}

/**
 * Function:        void thload_start(ThLoader* th)
 * Description:     Initializes and starts a new thread that runs the `load_thread` function.
 *                  Sets the `keeprunning` flag to 1 to indicate the thread should keep running.
 * Parameters:      th: ThLoader* - Pointer to the thread loader structure that manages execution.
 * Returns:         void - This function does not return a value.
 */

void thload_start(ThLoader* th) {
    th->keeprunning = 1;
    pthread_create(&th->threads, NULL, load_thread, th);
}

/**
 * Function:        void thload_run(ThLoader* th)
 * Description:     Runs a loop that logs GPU load activity while the `keeprunning` flag
 *                  is set. It utilizes a conditional wait mechanism (`Condwait`) to control
 *                  execution timing, ensuring efficient CPU usage.
 * Parameters:      th: ThLoader* - Pointer to the thread loader structure that manages execution.
 * Returns:         void - This function does not return a value.
 */

void thload_run(ThLoader* th) {

    Condwait condwait = (Condwait){  condwait_int, condwait_wait, condwait_signal, condwait_stop };
    condwait.init(&condwait);
    
    int ncount = 0;
    while (atomic_load_explicit(&th->keeprunning, memory_order_relaxed))
    {
       
        if(th->clubbed || th->localFile)
          log_message(LOG_INFO, th->thread_log, TAG, "GPU001 load %d , tid = %ld ", ncount++,  (long) pthread_self () );
        else
          slog_message(LOG_INFO,  TAG, "GPU001 load %d , tid = %ld ",  ncount++,  (long) pthread_self () );

        condwait.wait(&condwait, 0, 100);
    }
    condwait.stop(&condwait);
} 

#if 0
/**
 * Function:        void thload_run_notused(ThLoader* th)
 * Description:     Executes a loop that continuously logs GPU load activity while the
 *                  `keeprunning` flag is set. It logs messages either to a thread-specific
 *                  log file or a global log, depending on the thread's configuration.
 * Parameters:      th: ThLoader* - Pointer to the thread loader structure that controls execution.
 * Returns:         void - This function does not return a value.
 */

void thload_run_notused(ThLoader* th){ 
    int ncount = 0;
    while (atomic_load_explicit(&th->keeprunning, memory_order_relaxed))
    {
        if(th->clubbed || th->localFile)
           log_message(LOG_INFO, th->thread_log, TAG, "GPU001 load %d , tid = %ld ", ncount++,  (long) pthread_self () );
        else
           slog_message(LOG_INFO, TAG, "GPU001 load %d , tid = %ld ",  ncount++,  (long) pthread_self () );
        
        usleep(10000);
    }
} 
#endif
 
/**
 * Function: void thload_stop(ThLoader* th)
 * Description: Stops the running thread by setting the `keeprunning` flag 
 *              to false and waits for the thread to finish using `pthread_join`.
 * Parameters:
 *   - th: ThLoader* - Pointer to a `ThLoader` structure that controls 
 *     the thread's execution.
 * Returns:
 *   void - No return value. The function halts the thread and waits for it to terminate.
 */

void thload_stop(ThLoader* th){ 

    //printf("thload_stop\n" );

    atomic_store_explicit(&th->keeprunning,0 , memory_order_relaxed);

    pthread_join(th->threads, NULL);
}


