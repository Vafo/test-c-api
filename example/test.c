#include <gtk/gtk.h>

#include "socket_client_proc.h"


SOCKET socket_server;


struct struct_to_gpointer
{
    GtkStack* stack_;
    GtkWidget* label_;
    GtkWidget *email_entry_;
    GtkWidget *password_entry_;
};

struct struct_to_gpointer login_button_struct;

static void
activate (GtkApplication *app,
          gpointer        user_data);

static void 
login_page(GtkWidget *widget,
			gpointer  data)
{
    GtkStack* stack = data;

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "login_page");
}

static void 
login_button_routine(GtkWidget *widget,
					  gpointer  data)
{
    struct struct_to_gpointer *temp_struct = data;

    const char *email_data = gtk_entry_get_text(GTK_ENTRY(temp_struct->email_entry_));
	const char *password_data = gtk_entry_get_text(GTK_ENTRY(temp_struct->password_entry_));

	printf("|%s| - |%s|\n", email_data, password_data);

	char to_server_buf[128];
	int pos_temp = strlen(email_data);
	strcpy(to_server_buf, email_data);
	to_server_buf[pos_temp++] = '\n';
	strcpy(to_server_buf + pos_temp, password_data);
	pos_temp += strlen(password_data);
	to_server_buf[pos_temp++] = '\n';
	to_server_buf[pos_temp++] = '\n';
	to_server_buf[pos_temp++] = '\0';

	printf("Message to server:\n|%s|\n", to_server_buf);

	if(send(socket_server, to_server_buf, pos_temp, 0) == -1){
		gtk_entry_set_text(GTK_ENTRY(temp_struct->email_entry_), "");
    	gtk_entry_set_text(GTK_ENTRY(temp_struct->password_entry_), "");
		return;
	}
	printf("Message is sent to server\n\n");
	
	printf("About to receive answer from server...\n");
	
	if(recv(socket_server, to_server_buf, 128, 0) < 1){
		printf("Connection closed by server\n");
		return;
	}
	printf("Message from server arrived: |%.*s|\n", 3, to_server_buf);
	
	if(strncmp(to_server_buf, "yes", 3) == 0)
		gtk_stack_set_visible_child_name(GTK_STACK(temp_struct->stack_), "login_succeed");		
	else
		gtk_entry_set_text(GTK_ENTRY(temp_struct->email_entry_), "");
    		gtk_entry_set_text(GTK_ENTRY(temp_struct->password_entry_), "");

	/*if(strcmp(email_data, "amir") == 0 && strcmp(password_data, "amir") == 0){
		// gtk_label_set_text(GTK_LABEL(temp_struct->label_), email_data);
        // GRID_LOGIN_SUCCEED(temp_struct->stack_);
        gtk_stack_set_visible_child_name(GTK_STACK(temp_struct->stack_), "login_succeed");
	}
	gtk_entry_set_text(GTK_ENTRY(temp_struct->email_entry_), "");
    gtk_entry_set_text(GTK_ENTRY(temp_struct->password_entry_), "");
	*/
}


static void
activate (GtkApplication *app,
          gpointer        user_data)
{
    GtkWidget *window;
    GtkStack *stack;

	GtkWidget *email_label;
	GtkWidget *password_label;
	GtkWidget *email_entry;
	GtkWidget *password_entry;
	GtkWidget *login_button;
    GtkWidget *box;
	GtkWidget *show_email;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Startup Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 500, 500);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	stack = GTK_STACK(gtk_stack_new());
    //gtk_container_add(GTK_CONTAINER(window), stack);

    email_label = gtk_label_new("Email:");
    email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Email");

	password_label = gtk_label_new("Password:");
    password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");

    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);

    login_button = gtk_button_new_with_label("Log in");

	show_email = gtk_label_new("");
	
	login_button_struct.stack_ = stack; 
	login_button_struct.label_ = show_email;
	login_button_struct.email_entry_ = email_entry;
	login_button_struct.password_entry_ = password_entry;
	
	//struct struct_to_gpointer login_button_struct = {stack, show_email, email_entry, password_entry};

    g_signal_connect(login_button, "clicked", G_CALLBACK(login_button_routine), &login_button_struct);

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL,20);
    gtk_box_pack_start(GTK_BOX(box), email_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), email_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), password_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), password_entry, FALSE, FALSE, 0); 
    gtk_box_pack_start(GTK_BOX(box), login_button, FALSE, FALSE, 0); 
    gtk_box_pack_start(GTK_BOX(box), show_email, FALSE, FALSE, 0); 

    gtk_stack_add_named(GTK_STACK(stack), box, "login_page");
    /////
    GtkWidget *succeed_msg = gtk_label_new("Successfully logged in! You are in the system");  
	GtkWidget *login_page_button = gtk_button_new_with_label("Back to login menu");  
	g_signal_connect (login_page_button, "clicked", G_CALLBACK(login_page), stack);  
	GtkWidget *grid = gtk_grid_new();                   
	gtk_grid_set_row_spacing(GTK_GRID(grid), 2);                   
	gtk_grid_attach(GTK_GRID(grid), succeed_msg, 0, 1, 1, 1);       
	gtk_grid_attach(GTK_GRID(grid), login_page_button, 0, 2, 1, 1);
	  
    gtk_stack_add_named(GTK_STACK(stack), grid, "login_succeed"); 
	/////
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(stack));

   	gtk_widget_show_all(window);	 
}



int
main(int    argc,
     char **argv)
{
	//SOCKET socket_server;
	/*struct addrinfo *server_addrinfo;

	if(conf_server_address(server_addrinfo, "localhost", "8080"))//127.0.0.1
		return 1;
	print_server_address(server_addrinfo);

	if((socket_server = create_socket(server_addrinfo)) == -1)
		return 2;
	
	if(connect_to_server(server_addrinfo, socket_server))
		return 3;
	*/	
	
	////////
	
	printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo("127.0.0.1", "8080", &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
///////
    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
            address_buffer, sizeof(address_buffer),
            service_buffer, sizeof(service_buffer),
            NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);
///////
    printf("Creating socket...\n");
    socket_server = socket(peer_address->ai_family,
            peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_server)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 2;
    }
///////
    printf("Connecting...\n");
    if (connect(socket_server,
                peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 3;
    }
    freeaddrinfo(peer_address);
///////
    printf("Connected.\n");
    printf("To send data, enter text followed by enter.\n");
	
	////////
		

    GtkApplication *app;

    int status;

    app = gtk_application_new("localhost.gtk.test", G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

	printf("Closing socket...\n");
	CLOSESOCKET(socket_server);

    return status;
}
