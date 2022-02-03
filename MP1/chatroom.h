#ifndef CHATROOM_H_
#define CHATROOM_H_
#include <ctype.h>
#include "interface.h"

struct ChatRoom{
    char room_name[MAX_DATA];
	int port_num;
	int num_members;
	int slave_socket[MAX_DATA];  
};

#endif