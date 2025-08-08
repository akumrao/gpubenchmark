/*
 * FileName:      condwait.c
 * Description:   Implements functions for condition variable synchronization,
 *                including `condwait_wait` for waiting with a timeout, 
 *                `condwait_signal` for signaling, and `condwait_stop` for cleanup.

 * Author:        Arvind Umrao <aumrao@google.com>
 *                Rajanee Kumbhar <rajaneek@google.com>
 */

#include "condwait.h"
#include "common.h"

/**
 * Function:        void condwait_int(Condwait* th)
 * Description:     Initializes the condition variable and mutex for synchronization.
 * Parameters:      th  - Condwait*  : Pointer to the Condwait structure.
 * Returns:         void
 */

void condwait_int(Condwait* th) {
    pthread_mutex_init(&th->mutex, NULL);
    pthread_cond_init(&th->cond, NULL);
    clock_gettime(CLOCK_REALTIME, &th->timeout);
}

/**
 * Function: int condwait_wait(Condwait* th, int timeInSec, int timeInMs)
 * Description: Waits for a condition variable to be signaled, with a specified 
 *              timeout (in seconds and milliseconds). Returns 0 if signaled, 
 *              or ETIMEDOUT if the timeout occurs.
 * Parameters:
 *   - th: Condwait* - Pointer to a `Condwait` structure containing the mutex 
 *     and condition variable.
 *   - timeInSec: int - Timeout duration in seconds.
 *   - timeInMs: int - Timeout duration in milliseconds.
 * Returns:
 *   int - 0 if the condition was signaled, or ETIMEDOUT if the timeout occurred.
 */

int condwait_wait(Condwait* th, int timeInSec, int timeInMs) {

    /* Initialize mutex and condition variable */
    u_int64_t future_ns = th->timeout.tv_nsec + timeInMs * 1000000L;
    th->timeout.tv_nsec = future_ns % 1000000000;
    /* Timeout of seconds */
    th->timeout.tv_sec += timeInSec + future_ns / 1000000000;

    pthread_mutex_lock(&th->mutex);

    int result = pthread_cond_timedwait(&th->cond, &th->mutex, &th->timeout);

    pthread_mutex_unlock(&th->mutex);

    if (result == 0) {
       // printf("Condition signaled.\n");
    } else if (result == ETIMEDOUT) {
        //printf("Timeout occurred.\n");
    }
    return result;
}

/**
 * Function:        void condwait_signal(Condwait* th)
 * Description:     Signals the condition variable to wake up a waiting thread.
 * Parameters:      th  - Condwait*  : Pointer to the Condwait structure.
 * Returns:         void  - No return value.
 */

void condwait_signal(Condwait* th) {
    pthread_mutex_unlock(&th->mutex);
    pthread_cond_signal(&th->cond);
    pthread_mutex_unlock(&th->mutex);
}

/**
 * Function:        void condwait_stop(Condwait* th)
 * Description:     Stops the condition wait mechanism by destroying the condition variable.
 * Parameters:      th  - Condwait*  : Pointer to the Condwait structure.
 * Returns:         void  - No return value.
 */

void condwait_stop(Condwait* th) {
    pthread_mutex_lock(&th->mutex);
    pthread_cond_destroy(&th->cond);
    pthread_mutex_unlock(&th->mutex);
}
