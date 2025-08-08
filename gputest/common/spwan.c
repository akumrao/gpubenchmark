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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h> 

#include "common.h"

#include "spwan.h"

#define TAG "TOP "

#define READ   0
#define WRITE  1

/**
 * Function:       FILE* fork_dup2_exec(char* cArgv[], char type, int *pid)
 * Description:    Forks a child process, redirects input/output, and executes a command.
 * Parameters:     cArgv  - char*[]  : Array of command arguments.
 *                 type   - char     : 'r' for reading output, 'w' for writing input.
 *                 pid    - int*     : Pointer to store the child process ID.
 * Returns:        FILE*   - File pointer to the pipe for communication with the child process.
 */

FILE * fork_dup2_exec(char* cArgv[], char type, int *pid) {
    pid_t child_pid;
    int fd[2];
    pipe(fd);

    if((child_pid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    /* child process */
    if (child_pid == 0) {

        /* Don't inherit parent signal handlers */
        signal(SIGINT,  SIG_IGN);
        signal(SIGTERM, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        if (type == 'r') {
            /* Close the READ end of the pipe since the child's fd is write-only */
            close(fd[READ]);
            /* Redirect stdout to pipe */
            dup2(fd[WRITE], 1);
        }
        else {
            /* Close the WRITE end of the pipe since the child's fd is read-only */
            close(fd[WRITE]);
            /* Redirect stdin to pipe */
            dup2(fd[READ], 0);
        }

        
        if(execvp(cArgv[0], cArgv) == -1) {
            perror("execvp failed: ");
            _Exit(EXIT_FAILURE);
        }
        _Exit(EXIT_SUCCESS);
    }
    else {
        if (type == 'r') {
            /* Close the WRITE end of the pipe since parent's fd is read-only */
            close(fd[WRITE]);
        }
        else {
            /* Close the READ end of the pipe since parent's fd is write-only */
            close(fd[READ]);
        }
    }

    *pid = child_pid;

    if (type == 'r')
    {
        return fdopen(fd[READ], "r");
    }

    return fdopen(fd[WRITE], "w");
}

/**
 * Function:        int fork_dup2_exec_result(FILE *fp, pid_t child_pid)
 * Description:     Waits for the child process to terminate and captures its exit status.
 * Parameters:      fp         - FILE*   : File pointer to be closed before waiting.
 *                  child_pid  - pid_t   : Process ID of the child process.
 * Returns:         int        - Exit status of the child process or -1 on failure.
 */

int fork_dup2_exec_result(FILE * fp, pid_t child_pid) {
    int status;
    pid_t wpid;
    fclose(fp);
    do {
        wpid = waitpid(child_pid, &status, WUNTRACED
#ifdef WCONTINUED
                /* Not all implementations support this */
                | WCONTINUED
#endif
                  );
        /*
        if (errno != EINTR) {
            stat = -1;
            break;
        }
        */
        if(wpid == -1) {
            perror("error waitpid call");
            return -1;
        }
        if (WIFEXITED(status)) {
           // printf("child exited, status=%d\n", WEXITSTATUS(status));

        } else if (WIFSIGNALED(status)) {
            //printf("child killed (signal %d)\n", WTERMSIG(status));

        } else if (WIFSTOPPED(status)) {
            //printf("child stopped (signal %d)\n", WSTOPSIG(status));
                        kill(child_pid, SIGCONT); 

#ifdef WIFCONTINUED     /* Not all implementations support this */
        } else if (WIFCONTINUED(status)) {
            //printf("child continued\n");
#endif
        } else {
            /* Non-standard case -- may never happen */
            //printf("Unexpected status (0x%x)\n", status);
        }

    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return status;
}

#define USE_FORK_DUP2_EXEC

#ifdef USE_FORK_DUP2_EXEC

/**
 * Function:        int exec(char* argv[], char **out, int* outsize)
 * Description:     Executes a command, captures its output, and returns the output size.
 * Parameters:      argv       - char*[]  : Array of command arguments.
 *                 out        - char**   : Pointer to store the command output.
 *                 outsize    - int*     : Pointer to store the size of the output.
 * Returns:        int        - Size of the output on success, -1 on failure.
 */
int exec(char* argv[] ,  char **out, int* outsize   ) {
    int pid;

    FILE * fp = fork_dup2_exec(argv, 'r', &pid);
    char command_out[100] = {0};

        int sz = 0;
        *out = NULL;
        *outsize = 0;

    while ( (sz = read(fileno(fp), command_out, sizeof(command_out)-1)) != 0)
    {   

            char *tmp = (char *)realloc(*out, *outsize + sz );
            if(tmp)
            {
               memcpy( &tmp[ *outsize], command_out, sz );
               
               *out = tmp;
            }
            *outsize += sz;
            
    }
        
    if(fork_dup2_exec_result(fp, pid) != 0) {
        return -1;
    }
    return *outsize;

}

/**
 * Function:        void* load_exec(void * arg)
 * Description:     Handles the execution of a process in a separate thread.
 *                  It opens a log file, runs the execution routine, and logs the end of the thread.
 * Parameters:      arg: void* - Pointer to the Exec structure containing execution details.
 * Returns:         void* - Always returns NULL after execution completion.
 */

void* load_exec(void * arg) {
    Exec *th = (Exec *) arg;
    char thread_log_filename[256];

    sprintf (thread_log_filename, "%s%s", th->logPath, th->logfile); 

    th->thread_log = fopen (thread_log_filename, "w");
    if (th->thread_log == NULL) {
          fprintf(stderr, "Failed to open file %s\n", th->logfile);
          return NULL;
    }

    th->run(th);

    //slog_message(LOG_DEBUG, TAG, "End of thread %s ",  (long) pthread_self () );

    fclose (th->thread_log);
    return NULL;
}

/**
 * Function:        void exec_start(Exec* th)
 * Description:     Initializes and starts the execution of a process in a separate thread.
 *                  This function forks a new process, executes the given command, and
 *                  redirects its output. It then starts a new thread to handle execution monitoring.
 * Parameters:      th: Exec* - Pointer to the execution structure containing process details.
 * Returns:         void - This function does not return a value.
 */

void exec_start(Exec* th) {
    th->fp = fork_dup2_exec(th->argv, 'r', &th->pid);
    th->keeprunning = 1;
    pthread_create(&th->threads, NULL, load_exec, th);

}

/**
 * Function:        void exec_run(Exec* th)
 * Description:     Reads output from the running process and writes it to the thread log.
 *                  The function continuously reads data while the process is running and
 *                  appends it to the log file, ensuring real-time monitoring of execution output.
 * Parameters:      th: Exec* - Pointer to the execution structure containing process details.
 * Returns:         void - This function does not return a value.
 */

void exec_run(Exec* th) {
    char command_out[100] = {0};
    int sz = 0;

    while ( atomic_load_explicit(&th->keeprunning, memory_order_relaxed)  &&  ((sz = read(fileno(th->fp), command_out, sizeof(command_out)-1)) != 0  )) {
       fwrite(command_out, 1, sz, th->thread_log);
    }
    return ;
}

/**
 * Function:        void exec_stop(Exec* th)
 * Description:     Stops the execution of a running process by sending a SIGINT signal.
 *                  If the process is non-blocking, it updates an atomic flag to indicate
 *                  termination and kills the process. The function then ensures proper
 *                  cleanup by handling process output and joining the thread.
 * Parameters:      th: Exec* - Pointer to the execution structure containing process details.
 * Returns:         void - This function does not return a value.
 */

void exec_stop(Exec* th) {

    //printf("exec_stop\n" );
    if(!th->blocking )
    {
        atomic_store_explicit(&th->keeprunning,0 , memory_order_relaxed);

        kill(th->pid, SIGINT);
    }
    if(fork_dup2_exec_result(th->fp, th->pid) != 0) {
            return;
    }
    pthread_join(th->threads, NULL);
    //printf("exec_joined\n" );
}


#else
/**
 * Function:        int exec(const std::string &in_cmd, std::string &out)
 * Description:     Executes a shell command and captures its output.
 *                  The function uses `popen` to run the command and reads
 *                  the output into a string buffer.
 * Parameters:      in_cmd: const std::string& - The command to be executed.
 *                  out: std::string& - A reference to a string where the output will be stored.
 * Returns:         int - Returns 0 on success, or -1 if the command execution fails.
 */

int exec(const std::string &in_cmd, std::string &out) {
     FILE *stream =  popen(in_cmd.c_str(), "r");;
     if (!stream) return -1;
     char buffer[128];
     out = "";
     while (!feof(stream))
     {
          if (fgets(buffer, 128, stream) != NULL)
               out += buffer;
     }
     pclose(stream);
     return 0;
}
#endif
