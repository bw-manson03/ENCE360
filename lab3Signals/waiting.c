#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>  /* defines pid_t */
#include <unistd.h>	 /* defines fork() */
#include <sys/wait.h>   /* defines the wait() system call. */
#include <signal.h>

 /* Function prototypes */
void sighup(int sigNum); /* routines child will call upon sigtrap */
void sigint(int sigNum);
void sigquit(int sigNum);

int main(void) {
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
		signal(SIGHUP, &sighup);
		signal(SIGINT, &sigint);
		signal(SIGQUIT, &sigquit);

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
	signal(SIGHUP, &sighup); // Reset signal. 
	char* s = "CHILD: I have received a SIGHUP\n";
	write(1, s, strlen(s)); // Use write() instead of printf() inside signal handlers, since it guarantees no interruptions. 
}

void sigint(int sigNum) {
	signal(SIGINT, &sigint); // Reset signal. 
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

