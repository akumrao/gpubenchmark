/*
 * FileName:      slogger.c
 * Description:   Implements a logging system with a producer-consumer model,
 *                using a queue and a thread to write log messages to a file.
 *                The system supports message enqueueing, logging, and thread
 *                management with a shutdown mechanism.
 *
 * Author:        Arvind Umrao <aumrao@google.com>
 *                Rajanee Kumbhar <rajaneek@google.com>
 */

#include "slogger.h"
#include "common.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>
#include <signal.h>
#include <stdatomic.h>

#define CAPACITY  2048

/*
 * Structure for Queue Data
 */
typedef struct {
    char *str;
    long size;
} QueueData;

typedef struct {
    
    int front, size;
    //int size;
    int shutdown;
    QueueData** data;
    size_t capacity;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;

Queue queue = {
    .front = 0,
    .size = 0,
    .shutdown = 0,
    .data = NULL,
    .capacity = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER
};

/**
 * Function:        void enqueue(QueueData* data)
 * Description:     Adds a log message to the queue for processing.
 * Parameters:      data  - QueueData*  : Pointer to the log message structure.
 * Returns:         void  - No return value.
 */
void enqueue(QueueData* data) {
    pthread_mutex_lock(&queue.mutex);
    if (queue.size == queue.capacity || queue.shutdown) {
        pthread_mutex_unlock(&queue.mutex);
        if(queue.size == queue.capacity)\
        /* This condition will not hit until test code is wrong. */
        fprintf(stderr, "Logger buffer overflow. \n");
        return;
    }
    int rear = (queue.front + queue.size) % queue.capacity;
    queue.data[rear] = data; 
    queue.size++;

    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);
}

/**
 * Function:        QueueData* dequeue()
 * Description:     Retrieves and removes the next log message from the queue.
 * Parameters:      None.
 * Returns:         QueueData*  - Pointer to the retrieved log message, or NULL if shutdown.
 */

QueueData* dequeue() {
    pthread_mutex_lock(&queue.mutex);
    while (queue.size == 0 && !queue.shutdown  ) {
        pthread_cond_wait(&queue.cond, &queue.mutex);
    }
    if( queue.size == 0 && queue.shutdown )
    {
        pthread_mutex_unlock(&queue.mutex);
        return NULL;
    }
    QueueData* data = queue.data[queue.front];
    queue.front = (queue.front + 1) % queue.capacity;
    queue.size--;

    pthread_mutex_unlock(&queue.mutex);
    return data;
}

/**
 * Function:        void* logging_thread(void* arg)
 * Description:     Handles logging in a separate thread by writing queued messages to a file.
 * Parameters:      arg   - void*   : Pointer to the Slogger structure containing log details.
 * Returns:        void*   - Always returns NULL after completing the logging process.
 */

void* logging_thread(void* arg) {

    Slogger *th = (Slogger *) arg;

    char thread_log_filename[1024]={'\0'};
    sprintf (thread_log_filename, "%s%s", th->logPath, th->logfile);
    FILE* file = fopen(thread_log_filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s \n", th->logfile);
        return NULL;
    }

    while (1)
    {
        /* get next chunk from queue */
        QueueData* qdata = dequeue();
        if(!qdata)
            break;
        fwrite(qdata->str, 1, qdata->size, file);
        fflush(file);
        free(qdata->str);
        free(qdata);
    }
    fclose(file);
    //printf("End of logging_thread \n");
    fflush(stdout);
    return NULL;
}

/**
 * Function:        void pushMessage(const char *str, long size)
 * Description:     Allocates a new queue task with the given string and size, then enqueues it.
 * Parameters:      str   - const char*  : Pointer to the message string.
 *                  size  - long         : Size of the message.
 * Returns:        void   - No return value.
 */

void pushMessage(const char *str , long size ) {
    QueueData* tasks =  (QueueData*) malloc( sizeof(QueueData));
    if(tasks) {
        if(str ) {
            tasks->str = (char *)str;
            tasks->size = size;
            /* enqueue task */
            enqueue(tasks);
        } else
             fprintf(stderr, "%s", "Out of Memory \n");
    } else
        fprintf(stderr, "%s", "Out of Memory \n");  
}

/**
 * Function:        void slog_start(Slogger* th)
 * Description:     Initializes and starts the logging thread, allocating memory for the log queue.
 * Parameters:      th  - Slogger*  : Pointer to the Slogger structure.
 * Returns:        void  - No return value.
 */

void slog_start(Slogger* th) {
    pthread_create(&th->threads, NULL, logging_thread, th);
    /* initialize queue */
    queue.capacity = CAPACITY;
    queue.data = (QueueData**) malloc(queue.capacity * sizeof(QueueData*));
}

/**
 * Function:        void slog_stop(Slogger* th)
 * Description:     Stops the logging thread, signals shutdown, and cleans up resources.
 * Parameters:      th  - Slogger*  : Pointer to the Slogger structure.
 * Returns:        void  - No return value.
 */

void slog_stop(Slogger* th) {

    //printf("slog_stop\n" );
    queue.shutdown = 1;
    //atomic_store_explicit(&keeprunning,0 , memory_order_relaxed);
    pthread_mutex_lock(&queue.mutex);
    pthread_cond_signal(&queue.cond);
    pthread_cond_destroy(&queue.cond);
    pthread_mutex_unlock(&queue.mutex);
    fflush(stdout);
    pthread_join(th->threads, NULL);
    free(queue.data);
} 
