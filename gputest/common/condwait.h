/*
 * FileName:      condwait.h
 * Description:   Defines the `Condwait` structure and function prototypes
 *                for handling condition variable synchronization. It includes
 *                functions to wait for a condition with a timeout, signal
 *                a waiting thread, and clean up condition variables and mutexes.
 *
 * Author:        Arvind Umrao <aumrao@google.com>
 *                Rajanee Kumbhar <rajaneek@google.com>
 */

#include <pthread.h>

#ifndef CONDWAIT_H
#define CONDWAIT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _condwait { 

  void (*init)( struct _condwait* ); 
  int (*wait)( struct _condwait*, int, int ); 
  void (*signal)( struct _condwait*); 
  void (*stop)( struct _condwait*); 

  pthread_mutex_t mutex;
  pthread_cond_t cond;
  struct timespec timeout;

} Condwait;
void condwait_int(Condwait* th);
int condwait_wait(Condwait* th, int timeInSec, int timeInMs);
void condwait_signal(Condwait* th);
void condwait_stop(Condwait* th);

#ifdef __cplusplus
}
#endif

#endif /* CONDWAIT_H */

