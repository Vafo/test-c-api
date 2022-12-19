#include "http/socket_linux.h"

struct http
{
    char hostname[64];
    in_port_t port;
};

int http_init(struct http *dst, char *host, in_port_t port);
int http_get(struct http *dst, char *route, char *dst_body, unsigned int buf_size);
int http_post(struct http *dst, char *route, char *src_body, unsigned int buf_size);