#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    if (argc < 2) {
        fprintf(stderr,"usage %s hostname\n", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) 
        error("ERROR on inet_pton");

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    printf("Connected to server. Enter messages:\n");
    while (1) {
        printf("Please enter the message: ");
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);

        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");

        memset(buffer, 0, BUFFER_SIZE);
        n = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) 
             error("ERROR reading from socket");
        printf("%s\n", buffer);
    }

    close(sockfd);
    return 0;
}
