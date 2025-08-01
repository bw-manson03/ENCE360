#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>  /* defines pid_t */
#include <unistd.h>	 /* defines fork() */
#include <sys/wait.h>   /* defines the wait() system call. */
#include <signal.h>

struct sigaction sa_hup, sa_int, sa_quit;

/* Function prototypes */
void sighup(int sigNum); /* routines child will call upon sigtrap */
void sigint(int sigNum);
void sigquit(int sigNum); 

int main(void) {
	sa_hup.sa_handler = sighup;
	sigemptyset(&sa_hup.sa_mask);
	sa_hup.sa_flags = 0;
	sigaction(SIGHUP, &sa_hup, NULL);

	sa_int.sa_handler = sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);

	sa_quit.sa_handler = sigquit;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);

	pid_t pid = 0;
	pid = fork();

	printf("pid is %i\n", pid);

	// Check for errors (we skip this in most examples, but you should always do it). 
	if (pid < 0) { 
		perror("fork"); // Print to stderr, "standard error". 
		exit(EXIT_FAILURE);

	} else if (pid == 0) {
		// Child code. 

		// Register signal handlers. 

		while (true) {
			sleep(1); // Block forever. 
		}

	} else {
		// Parent code. 
		printf("Parent processing starts\n");
		
		int childStatus = 0; // This will hold return code. 

		printf("\nPARENT: sending SIGHUP\n\n");
		sleep(1); // Give the child some time to set up its signal handlers. 
		kill(pid, SIGHUP);
		sleep(3); // Pause for 3 seconds
		
		printf("\nPARENT: sending SIGINT\n\n");
		kill(pid, SIGINT);
		sleep(3); // Pause for 3 seconds. 
		
		printf("\nPARENT: sending SIGQUIT\n\n");
		kill(pid, SIGQUIT);
		wait(&childStatus);
		
		printf("\nPARENT: doing something\n\n");
		sleep(3);
	}

	return EXIT_SUCCESS;
}

void sighup(int sigNum) {
	sigaction(SIGHUP, &sa_hup, NULL); // Reset signal. 
	char* s = "CHILD: I have received a SIGHUP\n";
	write(1, s, strlen(s)); // Use write() instead of printf() inside signal handlers, since it guarantees no interruptions. 
}

void sigint(int sigNum) {
	sigaction(SIGINT, &sa_int, NULL); // Reset signal. 
	char* s = "CHILD: I have received a SIGINT\n";
	write(1, s, strlen(s)); // Use write() instead of printf() inside signal handlers, since it guarantees no interruptions. 
}

void sigquit(int sigNum) {
	// We'll be exiting, so no need to reset signal in this case. 
	char* s = "CHILD: My DADDY has Killed me!!!\nCHILD: cleaning up...\n";
	write(1, s, strlen(s));
	sleep(2);
	exit(0);
}