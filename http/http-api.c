#include "http/http-api.h"

#define BUF_SIZE 1024

int http_init(struct http *dst, char *host, in_port_t port)
{
    strncpy(dst->hostname, host, 64);
    dst->port = port;
}

int http_get(struct http *dst, char *route, char *dst_body, unsigned int buf_size)
{
    int fd = user_socket_connect(dst->hostname, dst->port);
    char buffer[BUF_SIZE];
    int pos = 0;
    int bytes;

    if(fd == -1)
    {
        fprintf(stderr, "Socket falure\n");
    }

    bzero(buffer, BUF_SIZE);
    pos += snprintf(buffer + pos, BUF_SIZE, "GET %s HTTP/1.0\r\n", route);
    pos += snprintf(buffer + pos, BUF_SIZE - pos, "\r\n");
    
    bytes = write(fd, buffer, pos);
    if(bytes != pos)
    {
        fprintf(stderr, "Write func failure");
    }
    // bzero(dst_body, buf_size);
    printf("REQUEST\n%s\n", buffer);

    while(read(fd, buffer, BUF_SIZE - 1) != 0){
        fprintf(stderr, "%s", buffer);
        bzero(buffer, BUF_SIZE);
    }
    shutdown(fd, SHUT_RDWR);

    close(fd);
}
