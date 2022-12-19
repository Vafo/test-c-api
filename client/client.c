#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int socket_connect(char *host, in_port_t port){
    struct hostent *hp;
    struct sockaddr_in addr;
    int on = 1, sock;
    if((hp = gethostbyname(host)) == NULL){
    herror("gethostbyname");
    exit(1);
}
    bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    if(sock == -1){
        perror("setsockopt");
        exit(1);
    }

    if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        perror("connect");
        exit(1);
    }
    return sock;
}

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    int fd;
    int cx;
    char inputBuffer[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    if(argc < 4){
        fprintf(stderr, "Usage: %s <hostname> <port> <other-args>\n", argv[0]);
        exit(1);
    }
    fd = socket_connect(argv[1], atoi(argv[2]));
    if(fd)
    {
        printf("socket success\n");
    }

    cx = snprintf(inputBuffer, BUFFER_SIZE, "GET %s\r\n", argv[3]);

    write(fd, inputBuffer, cx); // write(fd, char[]*, len);
    bzero(buffer, BUFFER_SIZE);
    printf("reading response\n");
    while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
        fprintf(stderr, "%s", buffer);
        bzero(buffer, BUFFER_SIZE);
    }
    shutdown(fd, SHUT_RDWR);
    close(fd);
    return 0;
}