#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

int check_auth (char* checkAuth) {
  FILE *configFile;
  configFile = fopen ("config.txt", "r");
  if (configFile == NULL) {
    puts("Error openning config file");
    exit(EXIT_FAILURE);
  }

  char user[100];
  char pass[100];

  fscanf(configFile, "%s", &user);
  fscanf(configFile, "%s", &pass);

  fclose(configFile);

  char auth[201];

  strcat(auth, user);
  strcat(auth, ":");
  strcat(auth, pass);

  return strcmp (auth, checkAuth) == 0;
}

int main() {
    printf("Starting server\n");

/* ------------------------------ Socket config ----------------------------- */

    int listenfd = 0, connfd = 0, socket_desc, c, client_sock, read_size;
    struct sockaddr_in server, client; 
    char client_message[2000], server_response[2000];

    char sendBuff[1025];
    time_t ticks; 

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
      printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 5001 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
      //print the error message
      perror("bind failed. Error");
      return 1;
    }
    puts("bind done");

    //Listen
	  listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
	  client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0) {
      perror("accept failed");
      return 1;
    }

    puts("Connection accepted");

/* ---------------------------- Socket connection --------------------------- */

  int num_message = 0;
  int isAuthenticated = 0;


  //Receive a message from client
  bzero(client_message, 2000);
  while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ) {
    if (!isAuthenticated) {
      puts(client_message);
      if(!check_auth(client_message)) {
        strcpy(server_response, "401");
        isAuthenticated = 0;
      } else {
        strcpy(server_response, "200");
        isAuthenticated = 1;
      }
      strcpy(client_message, "");
      write(client_sock , server_response , strlen(server_response));
    }
    bzero(client_message, 2000);
    //Send the message back to client
    // write(client_sock , client_message , strlen(client_message));
  }

  if (read_size == 0) {
    puts("Client disconnected");
    fflush(stdout);
  } else if(read_size == -1) {
    perror("recv failed");
  }
  
  return EXIT_SUCCESS;
}
