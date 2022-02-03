#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "chatroom.h"
#include <fcntl.h>


#define BUFFER_LENGTH 256
#define MAX_ROOM 10
#define MAX_MEMBER 10
#define FALSE 0
/*
 * TODO: IMPLEMENT BELOW THREE FUNCTIONS
*/


// typedef struct chatrooms{
// 	char* room_name;
// 	int port_num;
// 	int num_members;
// 	int slave_socket;    //int slave_socket[MAX_MEMBER];
// }roo

struct ChatRoom room[MAX_DATA];
struct Reply reply;

//room room_db[MAX_ROOM];


char* return_message;
int isEmpty = 1;
int count = 0;
int inChatmode = 0;


int main(int argc, char** argv) 
{

   int    sd=-1, new_sd=-1;
   struct sockaddr_in sin;
   int    rc, length, on=1;
   char   buffer[BUFFER_LENGTH];
   fd_set read_fd;
   struct timeval timeout;

   
    sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0){
			perror("Error creating socket on server side");
			exit(-1);
	}
		
		//possibly setsocktopt
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(argv[1]));
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
		
	if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))<0){
		perror("Error binding socket");
		exit(-1);

	}
		
	if(listen(sd, 10)<0){
		perror("Error listening to socket");
		exit(-1);
	}
	

    while(1){
    	printf("I AM HERE 3\n");
    	
    	
		new_sd =  accept(sd, NULL, NULL);
		if(new_sd < 0){
			perror("Error accepting socket");
			exit(-1);
		}
		
		printf("I AM HERE 4\n");

		
		//timeout.tv_sec  = 30;
        //timeout.tv_usec = 0;
        
        FD_ZERO(&read_fd);
        FD_SET(new_sd, &read_fd);
        
        fcntl(new_sd, F_SETFL, fcntl(new_sd, F_GETFL, 0) | O_NONBLOCK);

        
        if(select(new_sd+1, &read_fd, NULL, NULL, NULL)<0){
        	perror("Error selecting socket");
        	exit(-1);
        }
        

		//memset(&buffer, 0, sizeof(buffer));
        if(recv(new_sd, buffer, sizeof(buffer), 0)<0){
        	perror("Error Recieving socket");
        	exit(-1);
        } //else{
        	printf("Message from a client\n");

        	printf(buffer);
        //}
        
        // here I should proccess the command recieved 
        if(inChatmode == 0 ){
        	processing_command_from_client(&buffer, new_sd);
        } else{
        	processing_messages_from_client(&buffer, new_sd);
        }
        
        
        int n = send(new_sd, &reply, sizeof(reply), 0);
	    if(n<0){
	      	perror("Error sending socket");
	       	exit(-1);
	    }
	    
	    //messages for the clinet
	    // int m = send(new_sd, &buffer, sizeof(buffer), 0);
	    // if(m<0){
	    //   	perror("Error sending socket");
	    //   	exit(-1);
	    // }



        printf("I AM HERE 2\n");

    }
    
    if(sd = -1){
    	close(sd);
    }
    
    if(new_sd = -1){
    	close(new_sd);
    }
		

    return 0;
}


void processing_command_from_client(char buffer[BUFFER_LENGTH], int sd){
	
	
	int offset;
	int port = 3004;
	
	//printf("in processmode func\n");
	//printf(buffer);

	if(strncmp(buffer, "CREATE", 6)==0){
	
		char roomName[sizeof(buffer)];
	
		
		isEmpty = 0;
		
		printf("in create\n");


		// place room name from buffer into roomName variable
		offset = 7;
		strcpy(roomName, &buffer[offset]);
		printf(roomName);

		//check if rooms exists already
		for(int i=0; i<count; ++i){
			// printf("databbase: %s\n", room[i].room_name);
			// printf("roomName: %s\n", roomName);
			// printf("count: %i\n", count);
			if(strncmp(room[i].room_name, roomName, strlen(roomName)) == 0){
				return_message = "FAIL1\n";
				reply.status = FAILURE_ALREADY_EXISTS;
				return 0;
				
			}
		}
		
		//create a new master socket and new entry in the local database
		

		struct sockaddr_in sin;

		int master_sd = socket(AF_INET, SOCK_STREAM, 0);
		if(master_sd < 0){
			perror("Error creating socket");
			exit(-1);
		}
		
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.s_addr = inet_addr("127.0.0.1");

		strcpy(room[count].room_name, roomName);
		room[count].port_num = port;
		room[count].num_members = 0;
		room[count].slave_socket[count] = master_sd;
		

		//should send the return message to client
		reply.status = SUCCESS;

		port++;
		count++;

		//pthread_create();
		return 0;


	}else if(strncmp(buffer, "DELETE", 6) == 0){
		printf("in delete\n");
		
		char roomName[sizeof(buffer)];
		// place room name from buffer into roomName variable
		offset = 7;
		strcpy(roomName, &buffer[offset]);
		
		
		char delete_warning[] = "Warning: chat room being deleted, shutting down connection;";
		//check if room exists or not
		for(int i=0; i<count; ++i){
			if(strncmp(room[i].room_name, roomName, strlen(roomName)) == 0){
				
				//Delete the chat room and close the connection
				int index = i;
				
				close(room[i].slave_socket);
				room[i].num_members = 0;
				
				for(int j=index; j<count-1; ++j){
					room[j] = room[j+1];
				}
				
				count--;
				
				reply.status = SUCCESS;
				return 0;
			}
		}
		
		// if not exist the return a FAILURE_NOT_EXISTS
		reply.status = FAILURE_NOT_EXISTS;
		return 0;
		
		
		
	}else if(strncmp(buffer, "JOIN", 4) == 0){


		//I need to send the message out to the rest of the clients
		printf("in join\n");
		//do join things here
		char roomName[sizeof(buffer)];
		// place room name from buffer into roomName variable
		offset = 5;
		strcpy(roomName, &buffer[offset]);

		for(int i=0; i<count; ++i){
			if(strncmp(room[i].room_name, roomName, strlen(roomName)) == 0){

				room[i].num_members++;
				reply.status = SUCCESS;
				reply.num_member = room[i].num_members;
				reply.port = room[i].port_num;
				
				//process messages
				
				//processing_messages_from_client(port, sd, i);

				inChatmode = 1;
				return 0;
				
			}
		}
	
		
		//room does not exist
		reply.status = FAILURE_NOT_EXISTS;
		return 0;
		
		
	}else if(strncmp(buffer, "LIST", 4) == 0){
		
		printf("in list\n");
		//struct Reply reply;
		
		
		//If the list is empty
		memset(reply.list_room ,0 , sizeof(reply.list_room));
		if(count > 0){
			for(int i=0; i<count; ++i){
				strcat(reply.list_room, room[i].room_name);
			 	strcat(reply.list_room, ",");
			 	printf("list - : %s\n", room[i].room_name);
			}
			
		} else{
			strcat(reply.list_room, "empty");
		}
		
		printf("list - retun: %s\n", reply.list_room);
		reply.status = SUCCESS;

		return 0;
		
	} else{
		reply.status = FAILURE_INVALID;
		return 0;
	}
	
	return 0;
}

void processing_messages_from_client(char buffer[BUFFER_LENGTH], int sd){
	
	// struct sockaddr_storage sin;
	// char buffer[MAX_DATA];
	// socklen_t sin_size;
	
	// sin_size = sizeof(sin);
	
	
	// int client_socket =  accept(sd,(struct sockaddr* )&sin, &sin_size);
	// if(client_socket < 0){
	// 	perror("Error accepting socket in p_m");
	// 	exit(-1);
	// }

	// 	memset(&buffer, 0, sizeof(buffer));
 //       if(recv(sd, buffer, sizeof(buffer), 0)<0){
 //       	perror("Error Recieng socket");
 //       	exit(-1);
 //       } 
        
        
  //   int n = send(sd, &buffer, sizeof(buffer), 0);
	 //   if(n<0){
	 //     	perror("Error sending socket");
	 //     	exit(-1);
		// }
	
	//printf(buffer);
	
	
	return 0;
}

