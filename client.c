#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

int logged_in = 0;

int main(int argc , char *argv[])
{
	int server_socket;
	struct sockaddr_in server;
	char send_buffer[1000] , recv_buffer[2000];
	
	//Create socket
	server_socket = socket(AF_INET , SOCK_STREAM , 0);
	if (server_socket == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(server_socket , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");
	
	//keep communicating with server
	while(1)
    {

		if(logged_in) {
			memset(send_buffer, 0, strlen(send_buffer));
			memset(recv_buffer, 0, strlen(recv_buffer));
			printf("Enter send_buffer : ");
			scanf("%s" , send_buffer);
			
			//Send some data
			if( send(server_socket , send_buffer , strlen(send_buffer) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			
			//Receive a reply from the server
			if( recv(server_socket , recv_buffer , 2000 , 0) < 0)
			{
				puts("recv failed");
				break;
			}
			
			puts("Server reply :");
			puts(recv_buffer);
		}
		else {
			printf("HOLA\n");
			if(send(server_socket, "LOGIN_REQUEST", strlen("LOGIN_REQUEST"), 0) < 0){
				puts("Send failed");
			}
			while(recv(server_socket , recv_buffer , 2000 , 0)){
				puts(recv_buffer);
				scanf("%s", send_buffer);
				if(send(server_socket, send_buffer, strlen(send_buffer), 0) < 0){
					puts("Send failed");
				}
				while(recv(server_socket , recv_buffer , 2000 , 0))
				{
					puts(recv_buffer);
					scanf("%s", send_buffer);
					if(send(server_socket, send_buffer, strlen(send_buffer), 0) < 0){
						puts("Send failed");
					}
					while(recv(server_socket , recv_buffer , 2000 , 0)){
						puts(recv_buffer);

						if(strncmp(recv_buffer, "AUTHENTICATED", strlen("AUTHENTICATED")) == 0)
						{
							logged_in = 1;
						}
						else  if(strncmp(recv_buffer, "BAD_AUTH", strlen("BAD_AUTH")) == 0)
						{
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