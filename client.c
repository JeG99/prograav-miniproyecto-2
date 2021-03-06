#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[])
{
  int sock;
	struct sockaddr_in server;
	char message[2000] , server_reply[2000];

  //Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");

  server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 5001 );

  //Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("connect failed. Error");
		return 1;
	}

  puts("Connected\n");

  int isAuthenticated = 0;
  do {
    printf("Enter auth (user:pass): ");
    scanf("%s" , message);

    if( send(sock , message , strlen(message) , 0) < 0) {
      puts("Send failed");
      return 1;
    }

    if( recv(sock , server_reply , 2000 , 0) < 0) {
      puts("recv failed");
      break;
    }

    if (strcmp("401", server_reply) == 0) {
      printf("Bad auth, please try again\n");
    } else {
      isAuthenticated = 1;
    }

  } while (!isAuthenticated);

  //keep communicating with server
	while(1) {
    bzero(message, 2000);
		//Send some data
		printf("Enter operation : ");
		scanf("%s" , message);

    if( send(sock , message , strlen(message) , 0) < 0) {
			puts("Send failed");
			return 1;
		}
		
		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0) {
			puts("recv failed");
			break;
		}

		//Receive a reply from the server
		puts("Server reply :");
		puts(server_reply);
    bzero(server_reply, 2000);
	}
	
	close(sock);
	return 0;
}
