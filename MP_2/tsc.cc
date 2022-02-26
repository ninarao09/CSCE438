#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include "sns.grpc.pb.h"
#include <google/protobuf/util/time_util.h>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>


using::google::protobuf::Timestamp;
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using csce438::Request;
using csce438::Reply;
using csce438::SNSService;
using csce438::Message;

std::string currentUser;


// std::string getSenderUsername(std::string username){
    
//     std::string temp = "";
   
//       for(int i=0; i<(int)username.size(); i++){

//          if(username[i] != ":"){
//             temp += username[i];
//          } else{
//              return temp;
//          }
          
//       }
// }

class Client : public IClient
{
    public:
        Client(const std::string& hname,
               const std::string& uname,
               const std::string& p)
            :hostname(hname), username(uname), port(p)
            {}
    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();
    private:
        std::string hostname;
        std::string username;
        std::string port;
        
        // You can have an instance of the client stub
        // as a member variable.
        std::unique_ptr<SNSService::Stub> stub_;
        IReply List(std::string &command);
        IReply Follow(std::string &command);
        IReply UnFollow(std::string &command);
        void Timeline(std::string& command);

};




int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string username = "default";
    std::string port = "3010";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                username = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    Client myc(hostname, username, port);
    // You MUST invoke "run_client" function to start business logic
    myc.run_client();

    return 0;
}

int Client::connectTo()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to create a stub so that
    // you call service methods in the processCommand/porcessTimeline
    // functions. That is, the stub should be accessible when you want
    // to call any service methods in those functions.
    // I recommend you to have the stub as
    // a member variable in your own Client class.
    // Please refer to gRpc tutorial how to create a stub.
	// ------------------------------------------------------------
	
	//Test case zero
	//check if username exists already
	//return -1 if true
	
	//creating the stub
	stub_ = std::unique_ptr<SNSService::Stub>(SNSService::NewStub(
	    grpc::CreateChannel("localhost:3010", grpc::InsecureChannelCredentials())));
	
	ClientContext context;
    Request request;
    Reply reply;
    
    
    request.set_username(username);
    currentUser = username;
    
    //call login on server side to check if username was already created
    grpc::Status status = stub_->Login(&context, request, &reply);
    
	if (status.ok()) {
	    if(reply.msg() == "Succesfull Login\n"){
	        
	        grpc::CreateChannel("localhost:3010", grpc::InsecureChannelCredentials());
            return 1; // return 1 if success, otherwise return -1
            
	    } else if (reply.msg() == "User already created\n"){
	        return -1;
	    }
        
    } else {
        return -1;
    }
	
	
	
	
}

IReply Client::processCommand(std::string& input)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse the given input
    // command and create your own message so that you call an 
    // appropriate service method. The input command will be one
    // of the followings:
	//
	// FOLLOW <username>
	// UNFOLLOW <username>
	// LIST
    // TIMELINE
	//
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
	// GUIDE 2:
	// Then, you should create a variable of IReply structure
	// provided by the client.h and initialize it according to
	// the result. Finally you can finish this function by returning
    // the IReply.
	// ------------------------------------------------------------
    
    IReply ire;
    
    if(input == "LIST"){
        
        ire = List(input);

        return ire;
        
    } else if(input.substr(0,6) == "FOLLOW"){
        
        ire = Follow(input);
         
        return ire;
        
    }else if(input.substr(0,8) == "UNFOLLOW"){
        
        ire = UnFollow(input);
        
        return ire;
        
    }else if(input == "TIMELINE"){
        ire.comm_status = SUCCESS;
        return ire;
    } else{
        ire.comm_status = FAILURE_INVALID;
        return ire;

    }
    
	// ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Follow" service method for FOLLOW command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Follow(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
    // ------------------------------------------------------------
    

}

void Client::processTimeline()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
	// ------------------------------------------------------------
    Timeline(username);
    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
	// ------------------------------------------------------------
}



IReply Client::List(std::string& command){
    
    //std::cout << "In list function" << std::endl;

    
    IReply ire;
    ClientContext context;
    Request request;
    Reply reply;
    
    request.set_username(currentUser);
    
    grpc::Status status = stub_->List(&context, request, &reply);
    ire.grpc_status = status;
    
    if (status.ok()) {
        ire.comm_status = SUCCESS;
        
        for(std::string s : reply.all_users()){
            ire.all_users.push_back(s);
        }
        
        for(std::string s : reply.following_users()){
            ire.following_users.push_back(s);
        }
        
        
    } else {
        ire.comm_status = FAILURE_NOT_EXISTS;
    }

    
    return ire;
}


IReply Client::Follow(std::string& command){
    //check username to see if it exists
    //if not add it to the list
    
    //std::cout << "In follow function" << std::endl;


    IReply ire;
    ClientContext context;
    Request request;
    Reply reply;
    
    
    std::string username2 = command.substr(7, command.length()-1);
    //std::cout << username << std::endl;
    //std::cout << username2 << std::endl;
    request.set_username(username);
    request.add_arguments(username2);
    
    
    grpc::Status status = stub_->Follow(&context, request, &reply);
    ire.grpc_status = status;
    
    
    if (status.ok()) {
        if(reply.msg()=="You cannot follow yourself"){
            ire.comm_status = FAILURE_ALREADY_EXISTS;
        } else if(reply.msg()=="Username does not exist"){
            ire.comm_status = FAILURE_INVALID_USERNAME;
        }else if (reply.msg()=="Already following this user"){
            ire.comm_status = FAILURE_ALREADY_EXISTS;

        }else if (reply.msg()=="Follow Succesfull"){
            ire.comm_status = SUCCESS;

        }
        
        
    } else {
        ire.comm_status = FAILURE_NOT_EXISTS;
    }
    
    return ire;
}

IReply Client::UnFollow(std::string& command){
    
    IReply ire;
    ClientContext context;
    Request request;
    Reply reply;
    
    
    std::string username2 = command.substr(9, command.length()-1);
    
    request.set_username(username); // current user
    request.add_arguments(username2); //user to unfollow
    
    
    grpc::Status status = stub_->UnFollow(&context, request, &reply);
    ire.grpc_status = status;
    
    
    if (status.ok()) {
        if(reply.msg()=="You cannot unfollow yourself"){
            ire.comm_status = FAILURE_INVALID_USERNAME;
        } else if(reply.msg()=="Username does not exist"){
            ire.comm_status = FAILURE_INVALID_USERNAME;
        } else if (reply.msg()=="UnFollow Succesfull"){
            ire.comm_status = SUCCESS;

        }else if (reply.msg()=="UnFollow unsuccesfull - you are not following"){
            ire.comm_status = FAILURE_INVALID_USERNAME;

        }
    }
    
    return ire;
}

void Client::Timeline(std::string& username){
    
    ClientContext context;

    
    
    std::shared_ptr<grpc::ClientReaderWriter<Message, Message>> stream(
        stub_->Timeline(&context));
        
    //start thread to write to followers
    std::thread writer([username, stream](){

        Message m;
        std::string input = "Stream Initialized";
        m.set_msg(input);
        m.set_username(username);
            
        google::protobuf::Timestamp timestamp = Timestamp();
        timestamp = google::protobuf::util::TimeUtil::GetCurrentTime();
        m.set_allocated_timestamp(&timestamp);
        
        stream->Write(m);
        while(1){
            input = getPostMessage();
            m.set_msg(input);
            m.set_username(username);
            
            m.release_timestamp();
            google::protobuf::Timestamp timestamp = Timestamp();
            timestamp = google::protobuf::util::TimeUtil::GetCurrentTime();
            m.set_allocated_timestamp(&timestamp);
            
            stream->Write(m);
        }
        stream->WritesDone();
    });
    
    
    //start thread to recive posts from
    std::thread reader([username, stream](){

        Message m;

        while(stream->Read(&m)){
            
            
       
            std::time_t time1 = time(0);
            std::string token = m.msg().substr(0, m.msg().find(":"));
            std::string token2 = m.msg().substr(m.msg().find(":")+1, m.msg().length());
            
            //std::cout << "MSG: " << m.msg() << std::endl;
                        //std::cout << "token: " << token << std::endl;

            //std::cout << "token2: " << token2 << std::endl;
            
            if(m.username() == ""){
                 displayPostMessage(token, token2, time1);
            }else{
                displayPostMessage(m.username(), token2, time1);
            }


            
        }
    });
        
    writer.join();
    reader.join();
}