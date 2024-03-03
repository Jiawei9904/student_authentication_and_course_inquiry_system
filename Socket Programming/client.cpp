#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include<sstream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

 
#define TCP_serverM_PORT 25259 // the port client will be connecting to
#define MAXDATASIZE 1000 // max number of bytes we can get at once
struct sockaddr_in serverM_addr, client_addr;

int socketCreated(void){
    int parentSocket;
    // printf("create the socket \n");
    parentSocket = socket(AF_INET, SOCK_STREAM, 0);
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_addr.sin_port = 0; // let the system choose the port number
    return parentSocket;
}

int socketConnect(int socket){
    int res;
    struct sockaddr_in remote = {0};

    //memset(&remote, 0, sizeof(remote));
    memset(&serverM_addr, '\0', sizeof(serverM_addr));
    serverM_addr.sin_family = AF_INET;
    serverM_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverM_addr.sin_port = htons(TCP_serverM_PORT);
    res = connect(socket, (struct sockaddr *)&serverM_addr, sizeof(struct sockaddr_in));
    return res;
}

int socketSend(int socket, char* request, int requestLen){
    int res = -1;
    res = send(socket, request, requestLen, 0);
    return res;
}

bool flag;

int main(int argc, char *argv[])
{
    int socket;
    struct sockaddr_in server;
    string password;
    string hostname;
    string combine;
    char clientSent[1000];
    memset(clientSent, '\0', 1000);
    unsigned short client_port;
    

    int count = 1;
    socket = socketCreated();
    if(socket == -1){
        printf("socket is not ready \n");
        return 1;
    }
    
    if(socketConnect(socket) < 0){
        printf("connection failed \n");
        exit(1);
    }

    socklen_t client_size = sizeof(client_addr);
    getsockname(socket, (struct sockaddr *)&client_addr, &client_size);
    client_port = ntohs(client_addr.sin_port);
    cout << "The client is up and running." << endl;
    while(count <= 3){

        //input host name and password
        printf("Please enter the username: \n" );
        getline(cin, hostname);
        printf("Please enter the password: \n");
        getline(cin, password);
        if(hostname.length() < 5 || hostname.length() > 50){
            cout << "Invaild Username." << endl;
            hostname.clear();
            password.clear();
            count++;
            continue;
        }

        if(password.length() < 5 || password.length() > 50){
            cout << "Invalid Password." << endl;
            password.clear();
            hostname.clear();
            count++;
            continue;
        }
        combine = hostname + " " + password;
        strcpy(clientSent, combine.data());
        int sentToServerM = socketSend(socket, clientSent, 1000);
        if(sentToServerM == -1){
            cout << "fail to send the request. " << endl;
        }else{
            cout << hostname << " sent an authentication request to the main server." << endl;
        }
        // receive the result from the main server
        socklen_t client_size = sizeof(client_addr);
        char* serverReply = new char[1000];
        memset(serverReply, '\0', 1000);
        if(recv(socket, serverReply, MAXDATASIZE, 0) == -1){
            printf("fail to receive the result of login \n");
            exit(1);
        }else{
            cout << hostname << " received the result of authentication using TCP over port " << client_port << endl;
            string str = serverReply;
            if(str != "Match"){
                hostname.clear();
                password.clear();
                cout << "Authentication failed: " << serverReply << endl;
                if(count <= 3){
                    cout << "Attempts remaining: " << 3-count << endl;
                    count++;
                    continue;
                }
                
            }else{
                cout << "Authentication is successful." << endl;
                flag = true;
                break;
            }
        }
        

    }
    if(count > 3){
        cout << "Authentication failed for 3 attempts. Client will shut down" << endl;
        exit(1);
    }
    

    if(flag == true){
        while(1){
        string CourseCode;
        string category;
        CourseCode.clear();
        category.clear();
        char sendToServerM[1000];
        memset(sendToServerM, '\0', 1000);
        // send the request and info to the serverM
            printf("Please enter the course code to query: \n");
            getline(cin, CourseCode);
            printf("Please enter the category (Credit / Professor / Days / CourseName): \n");
            getline(cin, category);

            string search;
            search.clear();
            search = CourseCode + " " + category;
            strcpy(sendToServerM, search.c_str());
            if(send(socket, sendToServerM, 1000, 0) == -1){
                cout << "fail to send the request" <<endl;
                exit(1);
            }else{
                cout <<hostname << " sent a request to the main server." << endl;
            }
            // receive the result from the serverM
            char* searchResult = new char[1000];
            memset(searchResult, '\0', 1000);
            int receive = recv(socket, searchResult, MAXDATASIZE, 0);
            if(receive == -1){
                printf("fail to receive the result from main server \n");
                exit(1);
            }else{
                cout << "The client received the response from the main server using TCP over port " << client_port << endl;
                string temp = searchResult;
                if(temp == "No find"){
                    cout << "Didn't find the course: " << CourseCode << endl;
                }else{
                    cout << "The " << category << " of " << CourseCode << " is " << searchResult << endl;
                }
            } 
        
        printf("-----------Start a new request---------------- \n");
    }
    }
    close(socket);
    
    return 0;
}



