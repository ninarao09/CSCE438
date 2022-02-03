#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "interface.h"


/*
 * TODO: IMPLEMENT BELOW THREE FUNCTIONS
 */
 
char  list[256]; // list of rooms
int connect_to(const char *host, const int port);
struct Reply process_command(const int sd, char* command);
void process_chatmode(const char* host, const int port);
void recv_chatmode(int sd);
void send_chatmode(int sd);

int main(int argc, char** argv) 
{
	if (argc != 3) {
		fprintf(stderr,
				"usage: enter host address and port number\n");
		exit(1);
	}

    display_title();
    
	while (1) {
	
		int sd = connect_to(argv[1], atoi(argv[2]));
		
		//Delete this
		//printf("Connected to Socket!!!\n");
    
		char command[MAX_DATA];
        get_command(command, MAX_DATA);

		struct Reply reply = process_command(sd, command);
		display_reply(command, reply);
		
		//delete thi
		
		touppercase(command, strlen(command) - 1);
		if (strncmp(command, "JOIN", 4) == 0) {
			printf("Now you are in the chatmode\n");
			process_chatmode(argv[1], reply.port);
		}
	
		close(sd);
    }

    return 0;
}

/*
 * Connect to the server using given host and port information
 *
 * @parameter host    host address given by command line argument
 * @parameter port    port given by command line argument
 * 
 * @return socket fildescriptor
 */
int connect_to(const char *host, const int port)
{
	// ------------------------------------------------------------
	// GUIDE :
	// In this function, you are suppose to connect to the server.
	// After connection is established, you are ready to send or
	// receive the message to/from the server.
	// 
	// Finally, you should return the socket fildescriptor
	// so that other functions such as "process_command" can use it
	// ------------------------------------------------------------

	int sd;
	struct sockaddr_in sin;

	//Create the socket
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0){
		perror("Error creating socket");
		exit(-1);
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(host);

	//Establish a connection with server - connect
	if(connect(sd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("Connection Failed");
		exit(-1);
	} else if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))==0){
		printf("Connection Successful;\n");
	}
	
	return sd;
}

/* 
 * Send an input command to the server and return the result
 *
 * @parameter sockfd   socket file descriptor to commnunicate
 *                     with the server
 * @parameter command  command will be sent to the server
 *
 * @return    Reply    
 */
struct Reply process_command(const int sd, char* command)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse a given command
	// and create your own message in order to communicate with
	// the server. Surely, you can use the input command without
	// any changes if your server understand it. The given command
    // will be one of the followings:
	//
	// CREATE <name>
	// DELETE <name>
	// JOIN <name>
    // LIST
	//
	// -  "<name>" is a chatroom name that you want to create, delete,
	// or join.
	// 
	// - CREATE/DELETE/JOIN and "<name>" are separated by one space.
	// ------------------------------------------------------------
	
	char message[256];
	strcpy(message, command);
	
	//printf("Testing message buffer");
	//printf(message); // message is the command typd in the terminal


	// ------------------------------------------------------------
	// GUIDE 2:
	// After you create the message, you need to send it to the
	// server and receive a result from the server.
	// ------------------------------------------------------------

	if(send(sd, message, sizeof(message), 0) < 0){
		perror("Sending Socket Failed");
	} 
	
	//printf("message was sent to the server\n");

	// ------------------------------------------------------------
	// GUIDE 3:
	// Then, you should create a variable of Reply structure
	// provided by the interface and initialize it according to
	// the result.
	//
	// For example, if a given command is "JOIN room1"
	// and the server successfully created the chatroom,
	// the server will reply a message including information about
	// success/failure, the number of members and port number.
	// By using this information, you should set the Reply variable.
	// the variable will be set as following:
	//
	// Reply reply;
	// reply.status = SUCCESS;
	// reply.num_member = number;
	// reply.port = port;
	// 
	// "number" and "port" variables are just an integer variable
	// and can be initialized using the message fomr the server.
	//
	// For another example, if a given command is "CREATE room1"
	// and the server failed to create the chatroom becuase it
	// already exists, the Reply varible will be set as following:
	//
	// Reply reply;
	// reply.status = FAILURE_ALREADY_EXISTS;
    // 
    // For the "LIST" command,
    // You are suppose to copy the list of chatroom to the list_room
    // variable. Each room name should be seperated by comma ','.
    // For example, if given command is "LIST", the Reply variable
    // will be set as following.
    //
    // Reply reply;
    // reply.status = SUCCESS;
    // strcpy(reply.list_room, list);
    // 
    // "list" is a string that contains a list of chat rooms such 
    // as "r1,r2,r3,"
	// ------------------------------------------------------------

	// REMOVE below code and write your own Reply.
	struct Reply reply;
	int offset;
	
	if(recv(sd, &reply, sizeof(reply), 0)<0){
			perror("Failed to recieve socket");
			exit(-1);

	}

	return reply;
	
}

/* 
 * Get into the chat mode
 * 
 * @parameter host     host address
 * @parameter port     port
 */
void process_chatmode(const char* host, const int port)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In order to join the chatroom, you are supposed to connect
	// to the server using host and port.
	// You may re-use the function "connect_to".
	// ------------------------------------------------------------
	int sd = connect_to(host, port);
	//int r_sd = connect_to(host, port);

	// char msg[MAX_DATA];
	// fd_set rd;
	
	// //fcntl(s_sd, F_SETFL, O_NONBLOCK);
	// //fcntl(0, F_SETFL, O_NONBLOCK);
	
	// fcntl(s_sd, F_SETFL, fcntl(s_sd, F_GETFL, 0) | O_NONBLOCK);
	
	// for(;;){
	// 	FD_ZERO(&rd);
 //       FD_SET(0, &rd);
 //       FD_SET(s_sd, &rd);
        
 //       if(select(s_sd+1, &rd, NULL, NULL, NULL)<0){
 //       	perror("Error selecting socket");
 //       	exit(-1);
 //       }
        
 //       if(FD_ISSET(0, &rd)){
 //       	get_message(msg, sizeof(msg));
		
	// 		if(send(s_sd, msg, sizeof(msg), 0)<0){
	// 			perror("Sending Socket Failed");
	// 			exit(-1);
	// 		} 
 //       }
        
 //       if(FD_ISSET(s_sd, &rd)){
 //       	int n = recv(s_sd, msg, sizeof(msg), 0);
	// 		if(n>0){
	// 			display_message(msg);
	// 		} 
        	
 //       }
	// }
	
	
	char s_msg[MAX_DATA];
	char r_msg[MAX_DATA];

	//printf("in recv\n");
	while(1){
		memset(s_msg, 0, sizeof(s_msg));
		memset(r_msg, 0, sizeof(r_msg));

		

		//read(sd, msg, MAX_DATA);
		get_message(s_msg, sizeof(s_msg));
		
		if(send(sd, s_msg, sizeof(s_msg), 0)<0){
			perror("Sending Socket Failed");
			exit(-1);
		} 
		
		
		
		int n = recv(sd, r_msg, sizeof(r_msg), 0);
		//printf("n: %i\n", n);
		// printf("msg: %s\n", msg);hi
		

		if(n>0){
			display_message(r_msg);
		} 
	

	}
	// ------------------------------------------------------------
	// GUIDE 2:
	// Once the client have been connected to the server, we need
	// to get a message from the user and send it to server.
	// At the same time, the client should wait for a message from
	// the server.
	// ------------------------------------------------------------
	
	// pthread_t send;
	// pthread_t recv;
	
	// pthread_create(&send, NULL, send_chatmode, s_sd);
	// pthread_create(&recv, NULL, recv_chatmode, s_sd);
	
	// pthread_join(send, NULL);
	// pthread_join(recv, NULL);
	
	//send_chatmode(s_sd);
	//recv_chatmode(r_sd);
	
	//close(send);
	//close(recv);
	

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    // 1. To get a message from a user, you should use a function
    // "void get_message(char*, int);" in the interface.h file
    // 
    // 2. To print the messages from other members, you should use
    // the function "void display_message(char*)" in the interface.h
    //
    // 3. Once a user entered to one of chatrooms, there is no way
    //    to command mode where the user  enter other commands
    //    such as CREATE,DELETE,LIST.
    //    Don't have to worry about this situation, and you can 
    //    terminate the client program by pressing CTRL-C (SIGINT)
	// -------------------------------------------------------------
	
}

void send_chatmode(int sd){
	char msg[MAX_DATA];
	printf("in send\n");

	while(1){
		memset(msg, 0, sizeof(msg));
		//printf("In send\n");

		get_message(msg, sizeof(msg));
		
		if(send(sd, msg, sizeof(msg), 0)<0){
			perror("Sending Socket Failed");
			exit(-1);
		} 
		
		//printf("MEssage sent: %s\n", msg);
		
	}
}

void recv_chatmode(int sd){
	char msg[MAX_DATA];
	printf("in recv\n");
	while(1){
		memset(msg, 0, sizeof(msg));

		//read(sd, msg, MAX_DATA);
		int n = recv(sd, msg, sizeof(msg), 0);
		// printf("n: %i\n", n);
		// printf("msg: %s\n", msg);

		if(n>0){
			display_message(msg);
		} 
	

	}
	
}

