#include "http/socket_linux.h"

#define BUF_SIZE 1024
char buffer[BUF_SIZE];

int main(int argc, char *argv[])
{
    int server_sock, client_sock;
    struct sockaddr address;
    int addrlen = sizeof(&address);
    int port = atoi(argv[1]);
    int num_cons = atoi(argv[2]);

    server_sock = server_socket_create(port, num_cons);

    client_sock = accept(server_sock, &address, (socklen_t *)&addrlen);

    int valread;
    valread = read(client_sock, buffer, BUF_SIZE-1);
    buffer[valread] = '\0';
    printf("CLIENT HAS SENT\n%s", buffer);
    close(client_sock);
    shutdown(server_sock, SHUT_RDWR);

}