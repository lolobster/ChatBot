#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<ctype.h>   // Char tolower() function

#define MAX_BUFFER 2048

// The thread function, that handles the connections
void *connection_handler(void *);

// Test message
char testMessage[] = "Hello client!";
char testMessage2[] = "I CAN'T UNDERSTAND THAT";

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

	//free(socket_desc);
    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
 //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size, counter = 0;
    char message, client_message[MAX_BUFFER];
    char *getMessage = client_message, *helloTest = "hello\n";

    //Receive a message from client
    while( (read_size = recv(sock , client_message , MAX_BUFFER, 0)) > 0 )
    {
        while(client_message[counter])
        {
            client_message[counter] = tolower(client_message[counter]);
            counter++;
        }
        counter = 0;
		
        printf("%s", client_message);
		//getMessage = client_message;
        printf("%s", getMessage);
		
		int result = strcmp(getMessage, helloTest);
        // Check if you can find the word
        if(result == 0)
        {
			printf("Result: %d\n", result);
            write(sock , testMessage, strlen(testMessage));
        }
        else
        {
            //Send the message back to client
            printf("Result: %d\n", result);
            write(sock , testMessage2, strlen(testMessage2));
		}
        //memset(testMessage, 0, MAX_BUFFER);
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}
