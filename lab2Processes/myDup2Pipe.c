#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define INP 1
#define OUTP 0

int main(void) {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    pipe(pipe1);
    pipe(pipe2);

    if ((pid1 = fork()) == 0) { /* Child code: Runs ls */
        dup2(pipe1[INP], STDOUT_FILENO);

        close(pipe1[OUTP]);
        close(pipe1[INP]);
        close(pipe2[INP]);
        close(pipe2[OUTP]);

        execl("/bin/ls", "ls", "-l", NULL);
        perror("The exec of ls failed");
        exit(1);
    }
    
    else if ((pid2 = fork()) == 0) { /* Parent code: Runs sort */
        dup2(pipe1[OUTP], STDIN_FILENO);
        dup2(pipe2[INP], STDOUT_FILENO);
        
        close(pipe1[OUTP]);
        close(pipe1[INP]);
        close(pipe2[INP]);
        close(pipe2[OUTP]);

        execl("/usr/bin/sort", "sort", "-k", "+8", NULL);
        perror("The exec of sort failed");
        exit(1);
    }
    
    else { /* Parent code: Runs head -5 */
        dup2(pipe2[OUTP], STDIN_FILENO);
        
        close(pipe1[OUTP]);
        close(pipe1[INP]);
        close(pipe2[INP]);
        close(pipe2[OUTP]);

        execl("/usr/bin/head", "head", "-5", NULL);
        perror("The exec of head failed");
        exit(1);
    }

    exit(0);
}
