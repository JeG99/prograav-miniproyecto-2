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

    bzero(server_reply, 2000);
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

  int numRows, numCols;

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
    
    if (strcmp(message, "exit") == 0) {
      return 0;
    }
		
		//Receive a reply from the server
    memset(server_reply,0, 2000);
		if( recv(sock , server_reply , 2000 , 0) < 0) {
			puts("recv failed");
			break;
		}

    if (strcmp(server_reply, "200") == 0) {
      memset(server_reply,0, 2000);
      if( recv(sock , server_reply , 2000 , 0) < 0) {
        puts("recv failed");
        break;
      }
    }

    // printf("server res %s\n", server_reply);
    numRows = strtol(server_reply, (char **)NULL, 10);
    // printf("num rows %d\n", numRows);

    for (int i = 0; i < numRows; i++) {
      memset(server_reply,0, 2000);
      if( recv(sock , server_reply , 2000 , 0) < 0) {
        puts("recv failed");
        break;
      }

      // printf("server res %s\n", server_reply);
      numCols = strtol(server_reply, (char **)NULL, 10);
      // printf("num cols %d\n", numCols);

      for (int c = 0; c < numCols; c++) {
        memset(server_reply,0, 2000);
        if( recv(sock , server_reply , 2000 , 0) < 0) {
          puts("recv failed");
          break;
        }

        printf("%s ", server_reply);
      }
      printf("\n");
    }
	}
	
	close(sock);
	return 0;
}
