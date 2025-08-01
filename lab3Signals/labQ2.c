#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>  /* defines pid_t */
#include <unistd.h>	 /* defines fork() */
#include <sys/wait.h>   /* defines the wait() system call. */
#include <signal.h>

//returns true if the current signal handler for
//sigNum is func, and false otherwise
bool testHandler(int sigNum, void (*func)(int))
{
    struct sigaction sa;
    sigaction(sigNum, NULL, &sa);
    return (sa.sa_handler == func);
}