#include "socket_macro.h"

#include <libpq-fe.h>

#include <stdlib.h>
#include <ctype.h>

void do_exit(PGconn *conn, PGresult *res) {
    
    fprintf(stderr, "%s\n", PQerrorMessage(conn));    

    PQclear(res);
    PQfinish(conn);    
    
    exit(1);
}

int main() {

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);


    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);


    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
	/////////////////
	PGconn *conn = PQconnectdb("user=poset_psql password=Berserk_25 host=localhost dbname=poset_db");

    if (PQstatus(conn) == CONNECTION_BAD) {
        
        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
            
        PQfinish(conn);
        exit(1);
    }
	/////////////////
    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");


    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; ++i) {
            if (FD_ISSET(i, &reads)) {

                if (i == socket_listen) {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address,
                            &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address,
                            client_len,
                            address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);

                } else {
                    char read[1024];
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    
                    printf("Message from client:\n|%.*s|\n", bytes_received, read);
			
		     	 	char *newline_char;
		     	 	char *newline_char_old = read;
		     	 	char email_buf[64];
		     	 	char password_buf[64];
		     	 	
		     	 	newline_char = strchr(read, '\n');
					
                    int j;
                    for (j = 0; newline_char != NULL; ++j){
                    	if(j == 0){
                        	strncpy(email_buf, newline_char_old, newline_char - newline_char_old);
                    		email_buf[newline_char - newline_char_old] = '\0';
                    	}
                    	else{
                    		strncpy(password_buf, newline_char_old, newline_char - newline_char_old);
                    		password_buf[newline_char - newline_char_old] = '\0';
                    	}
                    	newline_char_old = newline_char + 1;
                    	newline_char = strchr(newline_char + 1, '\n');
                    	if((newline_char - newline_char_old) == 0)
                    		break;
                    }
                    
                    printf("Email extracted: |%s|\nPassword extracted: |%s|\n", email_buf, password_buf);
                    /////////////////
					sprintf(read, "select exists(select 1 from users where email='%s' and password='%s')", email_buf, password_buf);
					PGresult *res = PQexec(conn, read);
    				if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        				do_exit(conn, res);
    				}
					/////////////////
                   if(strcmp(PQgetvalue(res, 0, 0), "t") == 0)
                    	strcpy(read, "yes"); 	// length is constant
                   else
                    	strcpy(read, "nil");	// length is constant
        	
                    PQclear(res);
                    send(i, read, 3, 0);
                }

            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)


	PQfinish(conn);
    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);


    printf("Finished.\n");

    return 0;
}
