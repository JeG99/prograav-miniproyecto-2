#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <stdlib.h> // free

int logged_in = 0;

int main(int argc , char *argv[])
{
	int server_socket;
	struct sockaddr_in server;
	char send_buffer[1000];
	char recv_buffer[2000];
	
	//Create socket
	server_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (server_socket == -1)
	{
		printf("Socket creation error");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(server_socket , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("Server connection failed");
		return 1;
	}
	
	puts("Server connection accepted\n");
	
	//keep communicating with server
	while(1)
    {
		memset(send_buffer, 0, strlen(send_buffer));
		memset(recv_buffer, 0, strlen(recv_buffer));
		if(logged_in) {
			size_t buffer_size = 0;
			char* query_buffer = NULL;
			printf("Enter send_buffer: ");
			//scanf("%s" , send_buffer);
			getline(&query_buffer, &buffer_size, stdin);
			query_buffer[buffer_size] = '\0';
			if(send(server_socket, query_buffer, strlen(query_buffer) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			
			if(strncmp(query_buffer, "exit", strlen("exit")) == 0){

				// Notify client exit to server
				if( send(server_socket , "CLIENT_EXIT" , strlen("CLIENT_EXIT") , 0) < 0)
				{
					puts("Send failed");
					return 1;
				}

				// Client socket closed
				close(server_socket);
				puts("Socket closed");
				return 1;
			}
			/*
			//Send some data
			if( send(server_socket , send_buffer , strlen(send_buffer) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			*/
			//Receive a reply from the server
			if( recv(server_socket, recv_buffer, 2000 , 0) < 0)
			{
				puts("recv failed");
				break;
			}
			
			puts("Server reply:");
			puts(recv_buffer);
			free(query_buffer);
			//memset(query_buffer, 0, strlen(query_buffer));

		}
		else {
			// Message to start server login
			if(send(server_socket, "LOGIN_REQUEST", strlen("LOGIN_REQUEST"), 0) < 0)
			{
				puts("Send failed");
			}
			// Waiting for server response (it should now ask for a username)
			while(!logged_in && recv(server_socket , recv_buffer , 2000 , 0)){
				printf("%s", recv_buffer); // User:
				scanf("%s", send_buffer);
				if(send(server_socket, send_buffer, strlen(send_buffer), 0) < 0)
				{
					puts("Send failed");
				}
				// Waiting for server response (it should now ask for a password)
				while(!logged_in && recv(server_socket , recv_buffer , 2000 , 0))
				{
					printf("%s", recv_buffer); // Password:
					scanf("%s", send_buffer);
					if(send(server_socket, send_buffer, strlen(send_buffer), 0) < 0)
					{
						puts("Send failed");
					}
					// Wait for server account validation
					while(!logged_in && recv(server_socket , recv_buffer , 2000 , 0))
					{
						puts(recv_buffer); // AUTHENTICATED / BAD_AUTH

						if(strncmp(recv_buffer, "AUTHENTICATED", strlen("AUTHENTICATED")) == 0)
						{
							// Client can now send more messages
							puts("(type <exit> to close session)");
							logged_in = 1;
						}
						else if(strncmp(recv_buffer, "BAD_AUTH", strlen("BAD_AUTH")) == 0)
						{
							// Notify client exit to server
							if( send(server_socket , "CLIENT_EXIT" , strlen("CLIENT_EXIT") , 0) < 0)
							{
								puts("Send failed");
								return 1;
							}

							// Client socket closed
							close(server_socket);
							return 1;
						}
					}
				}	
			}
		}
	}
	
	close(server_socket);
	return 0;
}