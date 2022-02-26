#include <ctime>

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <google/protobuf/util/time_util.h>
#include <grpc++/grpc++.h>

#include "sns.grpc.pb.h"

using google::protobuf::Timestamp;
using google::protobuf::Duration;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using csce438::Message;
using csce438::Request;
using csce438::Reply;
using csce438::SNSService;

//using namespace std;

int currentClientIndex;
int otherClientIndex;

std::string sender_username;

struct Client{
  std::string name;
  std::vector<Client*> followers;
  std::vector<Client*> following;
  ServerReaderWriter<Message, Message>* stream = 0;
  int timeline_size = 0;
  bool isConnected = true;
};

//global of all clients
std::vector<Client> clients_db;



int findClientByIndex(std::string username){
  for(int i=0; i<clients_db.size(); ++i){
    if(clients_db[i].name == username){
      //std::cout << "in index function" << clients_db[i].name << std::endl;
      return i;
    }
  }
  return -1;
}


void addFollowerToFile(std::string username, std::string follower){
  
  std::string filename = username + "_followers.txt";
  std::ofstream user_file(filename, std::ios::app|std::ios::out|std::ios::in);
  user_file<<follower<<std::endl;
  
}

void removeFollowerToFile(std::string username, std::string follower){
  
  std::string line;
  std::string filename = username+"_followers.txt";
  std::ifstream in(filename);
  std::ofstream user_file(filename, std::ios::app|std::ios::out|std::ios::in);


  // while(getline(in, line)){
    
  //     line.replace(line.find(follower),follower.length(),"");
  //     user_file << line;
    

  // }

}

class SNSServiceImpl final : public SNSService::Service {
  
  Status List(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // LIST request from the user. Ensure that both the fields
    // all_users & following_users are populated
    // ------------------------------------------------------------
    

    currentClientIndex = findClientByIndex(request->username());
    //std::cout << "currentClientIndex : " << currentClientIndex<< std::endl;


    for(int i=0; i<clients_db.size(); ++i){
        reply->add_all_users(clients_db[i].name);
    }
    
    
    reply->add_following_users(clients_db[currentClientIndex].name);
    for(int i=0; i<clients_db[currentClientIndex].followers.size(); ++i){
        reply->add_following_users(clients_db[currentClientIndex].followers[i]->name);
    }   
    

    return Status::OK;
  }

  Status Follow(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // request from a user to follow one of the existing
    // users
    // ------------------------------------------------------------
    

    
    //follow the user
    
    std::cout << "I am in follow server function" << std::endl;
    
    //username of client you want to follow
    std::string username1 = request->username();
    std::string username2 = request->arguments(0);
    
    std::cout << "U1: " << username1 << std::endl;
    std::cout << "U2: " << username2 << std::endl;
    
    
    currentClientIndex = findClientByIndex(username1);
    otherClientIndex = findClientByIndex(username2);


    
    //check if username is the same as the current user 
    if(username1 == username2){
        reply->set_msg("You cannot follow yourself");
        return Status::OK; 
    }
    
    //check if username is valid
    bool isValid = false;
    for(int i=0; i<clients_db.size(); ++i){
      if(clients_db[i].name==username2){
        isValid = true;
      }
    }
    
    if(!isValid){
        reply->set_msg("Username does not exist");
        return Status::OK;
    }
    
    
      //check if user1 is already followung user2
     for(int i=0; i<clients_db[currentClientIndex].following.size(); ++i){
        if(clients_db[currentClientIndex].following[i]->name == username2){
            reply->set_msg("Already following this user");
            return Status::OK;
        }
     }
     
     
     
     

     Client* clientToFollow;
     for(int i=0; i<clients_db.size(); ++i){
        if(clients_db[i].name==username2){
          clientToFollow = &clients_db[i];
        }
     }
     
     Client* clientToFollower;
     for(int i=0; i<clients_db.size(); ++i){
        if(clients_db[i].name==username1){
          clientToFollower = &clients_db[i];
        }
     }
    
    //std::cout << clientToFollow->name << std::endl;
    clients_db[currentClientIndex].following.push_back(clientToFollow);
    clients_db[otherClientIndex].followers.push_back(clientToFollower);
    addFollowerToFile(username2, username1);
    reply->set_msg("Follow Succesfull");

    return Status::OK; 
  }

  Status UnFollow(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // request from a user to unfollow one of his/her existing
    // followers
    // ------------------------------------------------------------
    
    std::string username1 = request->username();
    std::string username2 = request->arguments(0);
    
    //check if you try to unfollow yourself
    if(username1 == username2){
        reply->set_msg("You cannot unfollow yourself");
        return Status::OK; 
    }
    
    
    //check if username is valid
    bool isValid = false;
    for(int i=0; i<clients_db.size(); ++i){
      if(clients_db[i].name==username2){
        isValid = true;
      }
    }
    
    if(!isValid){
        reply->set_msg("Username does not exist");
        return Status::OK;
    }
    
    
    
    int followersIndex, followingIndex;
    currentClientIndex = findClientByIndex(username1);
    otherClientIndex = findClientByIndex(username2);
    
    //check if user is following the the one they want ot unfollow
    bool isFollowingAlready=false;
    for(int i=0; i<clients_db[currentClientIndex].following.size(); ++i){
      if(clients_db[currentClientIndex].following[i]->name==username2){
        isFollowingAlready = true;
      }
    }
    
    if(!isFollowingAlready){
        reply->set_msg("UnFollow unsuccesfull - you are not following");
        return Status::OK;
    }
    
    
    //parse through following and followers and save index 
    
    //finding index of the users to update for unfollowing them
     for(int i=0; i<clients_db[currentClientIndex].following.size(); ++i){
        if(clients_db[currentClientIndex].following[i]->name == username2){
          followingIndex = i;
        }
     }
     
     for(int i=0; i<clients_db[otherClientIndex].followers.size(); ++i){
        if(clients_db[otherClientIndex].followers[i]->name == username1){
          followersIndex = i;
        }
     }
     
     

    clients_db[currentClientIndex].following.erase(clients_db[currentClientIndex].following.begin()+followingIndex);
    clients_db[otherClientIndex].followers.erase(clients_db[otherClientIndex].followers.begin()+followersIndex);

    removeFollowerToFile(username2, username1);


    reply->set_msg("UnFollow Succesfull");
    return Status::OK;
  }
  
  Status Login(ServerContext* context, const Request* request, Reply* reply) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // a new user and verify if the username is available
    // or already taken
    // ------------------------------------------------------------
    std::string username1 = request->username();
    std::cout << "U1: " << username1 << std::endl;
    
    //after cpomple
    Client newClient;
    newClient.name = username1;
    clients_db.push_back(newClient);
    
    
    //check if user was created already clients cannot have the same name
    for(int i=0; i<clients_db.size()-1; ++i){
      if(clients_db[i].name == username1){
          reply->set_msg("User already created\n");
          clients_db.pop_back();
          return Status::OK;
      }
    }
    
    //follow yourself
    //createClientFollowerFile(username1);
    addFollowerToFile(username1, username1);

    reply->set_msg("Succesfull Login\n");
    return Status::OK;
  }

  Status Timeline(ServerContext* context, ServerReaderWriter<Message, Message>* stream) override {
    // ------------------------------------------------------------
    // In this function, you are to write code that handles 
    // receiving a message/post from a user, recording it in a file
    // and then making it available on his/her follower's streams
    // ------------------------------------------------------------
    Message m;
    Client *user;
    std::string sender = m.username();
    //receiving message
    
    int currentClientIndex = findClientByIndex(sender);

      //create a list of followers and samve them in a full
      //reade the followers files display the proper timeline data
      // std::string followers_list_filename = username + "_followers.txt";
      // std::ofstream followers_list_file(followers_list_filename, std::ios::app|std::ios::out|std::ios::in);
      // followers_list_file << username;
      // for(int i=0; i<clients_db[currentClientIndex].followers.size(); ++i){
      //     followers_list_file << clients_db[currentClientIndex].followers[i]->name;
      // }
    
    
    
    while(stream->Read(&m)){
      
      std::string username = m.username();
      int client_index = findClientByIndex(username);
      user = &clients_db[client_index];
      
      //create file with user posts
      std::string filename = username + ".txt";
      std::ofstream user_file(filename, std::ios::app|std::ios::out|std::ios::in);
      std::string fileinput = m.username() + ":" + m.msg();
      
      

      if(m.msg() != "Stream Initialized"){
        user_file << fileinput;
      } else{

        if(user->stream == 0){
          user->stream = stream;
        } 
        
        //user timline file
        std::string line;
        std::vector<std::string> recent_twenty;
        std::ifstream in(username+"_timeline.txt");
        int count = 0;
        
        while(getline(in, line)){
          if(user->timeline_size > 20){
            if(count < user->timeline_size-20){
              count++;
              continue;
            }
          }
          recent_twenty.push_back(line);

        }
        
        Message new_m;
        for(int i=recent_twenty.size()-1; i>=0; --i){
          new_m.set_msg(recent_twenty[i]);
          new_m.set_username(sender);
          stream->Write(new_m);
        }
        
        continue;
        
      }
      
      
      //send message to each my followers stream

      for(int i=0; i<user->followers.size(); ++i){
        Client* temp_user = user->followers[i];


        if(temp_user->stream != 0 && temp_user->isConnected){
          temp_user->stream->Write(m);
        }
        
        std::string temp_file = temp_user->name + "_timeline.txt";
        std::ofstream following_file(temp_file, std::ios::app|std::ios::out|std::ios::in);
        following_file<<fileinput;
        temp_user->timeline_size++;
        std::ofstream user_file(temp_user->name, std::ios::app|std::ios::out|std::ios::in);
        user_file<<fileinput;
        
        
      }
    }

    user->isConnected = false;
    return Status::OK;
  }

};

void RunServer(std::string port_no) {
  // ------------------------------------------------------------
  // In this function, you are to write code 
  // which would start the server, make it listen on a particular
  // port number.
  // ------------------------------------------------------------
  
  
  
  std::string server_address("localhost:" + port_no);
  SNSServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
  
}

int main(int argc, char** argv) {
  
  std::string port = "3010";
  int opt = 0;
  while ((opt = getopt(argc, argv, "p:")) != -1){
    switch(opt) {
      case 'p':
          port = optarg;
          break;
      default:
	         std::cerr << "Invalid Command Line Argument\n";
    }
  }
  RunServer(port);
  return 0;
}


