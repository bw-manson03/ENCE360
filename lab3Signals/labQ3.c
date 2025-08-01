#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int main(void)
{
    int sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    struct sockaddr_un name;
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, "a.sock", 5);

    bind(sockfd, (struct sockaddr *)&name, sizeof(struct sockaddr));

    listen(sockfd, 1); 

    char buffer[20] = {'\0'};
    ssize_t numBytes = read(sockfd, buffer, 20);
    buffer[numBytes] = '\0';
    write(sockfd, buffer, numBytes+1); // Echo message.  

    close(sock);
    unlink("a.sock"); 
}