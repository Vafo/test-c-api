#include "http/http-api.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    struct http serv;
    char *body = NULL;
    int body_size = 0;
    http_init(&serv, argv[1], atoi(argv[2]));
    http_get(&serv, argv[3], &body, &body_size);

    printf("%d\n", body_size);
    printf("%s", body);
}

// int main(int argc, char *argv[]){
//     int fd;
//     int pos = 0;
//     char inputBuffer[BUFFER_SIZE];
//     char buffer[BUFFER_SIZE];
//     if(argc < 4){
//         fprintf(stderr, "Usage: %s <hostname> <port> <other-args>\n", argv[0]);
//         exit(1);
//     }
//     fd = user_socket_connect(argv[1], atoi(argv[2]));
//     if(fd)
//     {
//         printf("socket success\n");
//     }

//     pos += snprintf(inputBuffer + pos, BUFFER_SIZE, "GET %s HTTP/1.0\r\n", argv[3]);
//     pos += snprintf(inputBuffer + pos, BUFFER_SIZE, "\r\n");

//     write(fd, inputBuffer, pos); // write(fd, char[]*, len);
//     bzero(buffer, BUFFER_SIZE);
//     printf("reading response JALAAAAAAAAAAAp\n");
//     while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
//         fprintf(stderr, "%s", buffer);
//         bzero(buffer, BUFFER_SIZE);
//     }
//     shutdown(fd, SHUT_RDWR);
//     close(fd);
//     return 0;
// }