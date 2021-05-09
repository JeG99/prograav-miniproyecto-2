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

int MAX_LINE = 1024;

int account_validation(char* user, char* password)
{
    int val = 0;
    char line[MAX_LINE], *token;
    FILE* users = fopen("config.txt", "r");
    if(users != NULL) 
    {   /*
        puts(user);
        puts(password);*/
        while(!feof(users)){
            fgets(line, MAX_LINE, users);
            token = strtok(line, ":");
            if(strncmp(token, user, strlen(user)) == 0){
                token = strtok(NULL, ":");
                //puts("hola");
                if(strncmp(token, password, strlen(password)) == 0) 
                {
                    val = 1;
                    break;
                }
            }
        }
    }
    return val;
}

void server_init_test() 
{
    int server_socket, client_socket, c, read_size;
	struct sockaddr_in server, client;
	char recv_buffer[2000];
	
	//Create socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(server_socket,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
	}
	puts("bind done");
	
	//Listen
	listen(server_socket, 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	
	//accept connection from an incoming client
	client_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t*)&c);
	if (client_socket < 0)
	{
		perror("accept failed");
	}
	puts("Connection accepted");
	
	//Receive a message from client
	//while( (read_size = recv(client_socket, recv_buffer, 2000, 0)) > 0 )
	do {
		//Send the message back to client
        memset(recv_buffer, 0, strlen(recv_buffer));
        read_size = recv(client_socket, recv_buffer, 2000, 0);
        //write(client_socket, recv_buffer, strlen(recv_buffer));
        if(strncmp(recv_buffer, "LOGIN_REQUEST", strlen("LOGIN_REQUEST")) == 0){
            int validation = 0;
            char user[1024], password[1024];
            memset(user, 0, strlen(user));
            memset(password, 0, strlen(password));
            write(client_socket, "Usuario: ", strlen("Usuario: "));
            while(strlen(user) == 0)
            {
                memset(recv_buffer, 0, strlen(recv_buffer));
                recv(client_socket, recv_buffer, 2000, 0);
                strncpy(user, recv_buffer, strlen(recv_buffer));
            }

            write(client_socket, "Contraseña: ", strlen("Contraseña: "));
            while(strlen(password) == 0)
            {
                memset(recv_buffer, 0, strlen(recv_buffer));
                recv(client_socket, recv_buffer, 2000, 0);
                strncpy(password, recv_buffer, strlen(recv_buffer));
            }
            validation = account_validation(user, password);
            printf("%d\n", account_validation(user, password));
            write(client_socket, validation ? "AUTHENTICATED":"BAD_AUTH", validation ? strlen("AUTHENTICATED"):strlen("BAD_AUTH"));

        } else if(read_size > 0) {
            write(client_socket, recv_buffer, strlen(recv_buffer));
        }else if(read_size == -1) {
            write(client_socket, "Error en la lectura", strlen("Error en la lectura"));
        }

    } while(1);
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
        return (void)c;
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
    
}

void server_init() 
{
    
    int listenfd = 0, connfd = 0, n = 0;
    socklen_t c = 0;
    struct sockaddr_in serv_addr, client; 

    char sendBuff[1025];
    char recvBuff[1024];
    //time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //memset(&serv_addr, '0', sizeof(serv_addr));
    //memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10); 

    c = sizeof(struct sockaddr_in);
    //connfd = accept(listenfd, (struct sockaddr*)&client, &c);
    /*while(1){
        recv(listenfd, recvBuff, strlen(recvBuff), 0);
        if(strcmp(recvBuff, "hello") == 0)
        {
            strcpy(sendBuff, "hola de vuelta");
            send(listenfd, sendBuff, strlen(sendBuff), 0);
        }
    }*/

    //close(connfd);
    //sleep(1);
    
    
    while(1)
    {
        //connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        connfd = accept(listenfd, (struct sockaddr*)&client, &c);
        snprintf(sendBuff, sizeof(sendBuff), "%s ", "Usuario:");
        //write(connfd, sendBuff, strlen(sendBuff));
        send(listenfd, sendBuff, strlen(sendBuff), 0);
        while( (n = read(listenfd, recvBuff, sizeof(recvBuff)-1)) > 0 ) {
            if(strcmp(recvBuff, "user") == 0) 
            {
                recvBuff[c] = 0;
                snprintf(sendBuff, sizeof(sendBuff), "%s ", "QUE ONDA MAMONES");
                send(listenfd, sendBuff, strlen(sendBuff), 0);
                //write(connfd, sendBuff, strlen(sendBuff));
            }
            if(fputs(recvBuff, stdout) == EOF)
            {
                printf("\n Error : Fputs error\n");
            }
        }
        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        

        close(connfd);
        sleep(1);
     }    
}

static void daemonize()
{
    pid_t pid;
    
    /* Fork off the parent process */
    pid = fork();
    
    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    
     /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    
    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);
    
    /* Catch, ignore and handle signals */
    /*TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();
    
    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    
    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    
    /* Set new file permissions */
    umask(027);
    
    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/home/jeg99/prograav-miniproyecto-2");
    
    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }
    
    /* Open the log file */
    openlog ("firstdaemon", LOG_PID, LOG_DAEMON);
}

int main()
{   
    printf("Starting daemonize\n");
    daemonize();
    

    while (1)
    {
        server_init_test();        
    }

    syslog (LOG_NOTICE, "First daemon terminated.");
    closelog();
    
    return EXIT_SUCCESS;
}