#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <sstream>

using namespace std;
// TCP for server (cited Professor's given material: Beej)
#define TCP_serverM_PORT 25259 // the port users will be connecting to
#define UDP_serverM_PORT 24259
#define BACKLOG 10 // how many pending connections queue will hold
#define MAXBUFFER 100
#define serverC_Port 21259
#define serverEE_Port 23259
#define serverCS_Port 22259
struct sockaddr_in serverC_addr, serverEE_addr, serverCS_addr, client_addr, serverM_TCP_addr;
int TCP_socketCreated(void){
    int parentSocket;
    parentSocket = socket(AF_INET, SOCK_STREAM, 0);
    return parentSocket;
}

// TCP bind
int TCP_bindProcess(int parentSocket){
    memset(&serverM_TCP_addr, '\0', sizeof(serverM_TCP_addr));
    serverM_TCP_addr.sin_family = AF_INET;
    serverM_TCP_addr.sin_addr.s_addr = INADDR_ANY;
    serverM_TCP_addr.sin_port = htons(TCP_serverM_PORT);
    return bind(parentSocket, (struct sockaddr *)&serverM_TCP_addr, sizeof(serverM_TCP_addr));
}


int UDP_socketCreated(void){
    int udp_socket;
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket == -1){
        printf("Fail to create UDP socket. \n");
        exit(1);
    }
    struct sockaddr_in udp_addr;
    memset(&udp_addr, '\0', sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    udp_addr.sin_port = htons(UDP_serverM_PORT);

    // bind socket
    if(::bind(udp_socket, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) == -1){
        printf("Fail to bind UDP socket. \n");
        exit(1);
    }

    return udp_socket;
}

void initialServerC(){
    memset(&serverC_addr, 0, sizeof(serverC_addr));
    serverC_addr.sin_family = AF_INET;
    serverC_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverC_addr.sin_port = htons(serverC_Port);
}

void initialServerEE(){
    memset(&serverEE_addr, 0, sizeof(serverEE_addr));
    serverEE_addr.sin_family = AF_INET;
    serverEE_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverEE_addr.sin_port = htons(serverEE_Port);
}

void initialServerCS(){
    memset(&serverCS_addr, 0, sizeof(serverCS_addr));
    serverCS_addr.sin_family = AF_INET;
    serverCS_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverCS_addr.sin_port = htons(serverCS_Port);
}

string stringEncrypted(string str){
    for(int i = 0; i < str.size(); i++){
        if(str[i] >= '0' && str[i] <= '9'){
            // integer
            str[i] = str[i] + 4;
            int tempNum = (int)(str[i] - '0');
            tempNum = tempNum % 10;
            str[i] = (char)('0' + tempNum);
        }else if(str[i] >= 'a' && str[i] <= 'z'){
            str[i] = str[i] + 4;
            int tempChar = (int)(str[i] - 'a');
            tempChar = tempChar % 26;
            str[i] = (char)('a' + tempChar);
        }else if(str[i] >= 'A' && str[i] <= 'Z'){
            str[i] = str[i] + 4;
            int tempLarger = (int)(str[i] - 'A');
            tempLarger = tempLarger % 26;
            str[i] = (char)('A' + tempLarger);
        }else{
            continue;
        }
    }
    return str;
}
bool flag;
int main(int agrc, char *argv[]){
    int parent;
    string hostname;
    string password;  
    struct sockaddr_in server, client;
    parent = TCP_socketCreated();
    // create parent socket
    if(parent == -1){
        printf("Parent socket is not ready. \n");
        exit(1);
    }

    while(1){
    // bind
    if(TCP_bindProcess(parent) == -1){
        printf("bind process failed \n");
        exit(1);
    }

    // listen process
    if(listen(parent, BACKLOG) == -1){
        printf("listen failed \n");
        exit(1);
    }

    int UDP_socket;
    UDP_socket = UDP_socketCreated();
    // accept process and incoming sending and receiving process
    cout << "The main server is up and running." << endl;
    int clientLength;
    
    while(1){
        int countForAttempt = 1;
        int childSocket;
        clientLength = sizeof(struct sockaddr_in);
        // accept process
        
        if((childSocket = accept(parent, (struct sockaddr *)&client, (socklen_t*)&clientLength)) < 0){
                
            printf("accept failed \n");
            return 1;
        }
        while(countForAttempt <= 3){
            //out<<"counter for attempt" << countForAttempt << endl;
            //authenticated
            char* buffer = new char[1000];
            //memset(buffer, '\0', 1000);

            // receive through child socket
                memset(buffer, '\0', 1000);
                int rec;
                rec = recv(childSocket, buffer, 1000, 0);
                if(rec <= 0){
                    printf("fail to receive the data from client \n");
                    break;
                }
                
            
            string temp = buffer;
            stringstream ss;
            ss << temp;
            int count = 0;
            string str;
            while(getline(ss, str, ' ') && count < 2){
                if(count == 0){
                    hostname = str;
                }else{
                    password = str;
                }
                count++;
            }
            cout << "The main server received the authentication for " << hostname << " using TCP over port " << TCP_serverM_PORT << endl;
            // get host name and password
            // encrypted the info
            
            string encrypted_hostname;
            //encrypted_hostname.clear();
            string encrypted_password;
            //encrypted_password.clear();
            encrypted_hostname = stringEncrypted(hostname);
            encrypted_password = stringEncrypted(password);

            // cout<< encrypted_hostname << endl;
            // cout<< encrypted_password << endl;
            //udp to transmit these two parameters to serverC 
            // int UDP_socket;
            // UDP_socket = UDP_socketCreated();
            initialServerC();
            // send to serverC by using UDP
            string send ="";
            send = encrypted_hostname + ' ' + encrypted_password;
            //cout << send << endl;
            char UDP_send[send.length()];
            // string udpsend;
            // udpsend.clear();
            // strcpy(UDP_send, udpsend.c_str());
            //strcpy(UDP_send, send.c_str());
            strcpy(UDP_send, send.data());
            //UDP_send = send;
            //memset(UDP_send, '\0', 1000);
            // cout << UDP_send <<endl;
            int sendToServerC = sendto(UDP_socket, UDP_send, sizeof(UDP_send), 0, (const struct sockaddr *) &serverC_addr, sizeof(serverC_addr));
            //cout << sendToServerC << endl;
            if(sendToServerC == -1){
                printf("fail to send info to credential server \n");
                exit(1);
            }else{
                cout << "The main server sent an authentication request to serverC." << endl;
            }
            
            
            memset(buffer, '\0', 1000);
            encrypted_hostname.clear();
            encrypted_password.clear();
            send.clear();
            memset(UDP_send, '\0', sizeof(UDP_send));
            //receive the result from serverC by UDP
            // cout << buffer << send << UDP_send <<endl;
            char loginResult[1000];
            socklen_t serverC_size = sizeof(serverC_addr);
            int receiveFromServerC = recvfrom(UDP_socket, loginResult, sizeof(loginResult), 0, (struct sockaddr *) & serverC_addr, &serverC_size);
            //cout << receiveFromServerC << endl;
            if(receiveFromServerC == -1){
                printf("receiving process has been failed \n");
                exit(1);
            }else{
                cout << "The main server received the result of the authentication request from serverC using UDP over port " << UDP_serverM_PORT << endl;
            }
            string res = loginResult;
            //cout << res << endl;
            //send the result to the client by using TCP
            char* sendToClient = new char[1000];
            //memset(sendToClient, '\0', 1000);
            // res = "sdffgvrgsv";
            //strcpy(sendToClient, res.c_str());
            strcpy(sendToClient, res.data());
            //int TCP_childSocketToClient = 0;
            if(sendto(childSocket, sendToClient, strlen(sendToClient), 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1){
                printf("fail to send the result to the client \n");
                exit(1);
            }else{
                cout << "The main server sent the authentication result to the client. " << endl;
            }
            cout << sendToClient << endl;
            if(res != "Match"){
                countForAttempt++;
                hostname.clear();
                password.clear();                
                //cout<< buffer << endl;
                //cout << encrypted_hostname << endl;
                //cout<< encrypted_password << endl;
                // cout <<"send:"<< send << endl;
                // cout <<"UDP:" << UDP_send << endl;
                //close(childSocket);
                //continue;
            }else{
                //close(childSocket);
                flag = true;
                break;
            }

            
        }


        
        
        if(flag == true){
            while(1){
            // proceed course code and request
            char* category_coursecode = new char[1000];
            //memset(buffer, '\0', 1000);

            // receive through child socket
            //while(1){
                memset(category_coursecode, '\0', 1000);
                int rec;
                rec = recv(childSocket, category_coursecode, 1000, 0);
                //cout << "receive bytes " << rec << endl;
                if(rec <= 0){
                    //printf("fail to receive the data from client \n");
                    break;
                }

                string middle;
                middle.clear();
                middle = category_coursecode;
                //cout<< buffer2 << endl;
                // if(middle.find("EE") != 0 && middle.find("CS") != 0){
                //     continue;
                // }else{
                //     break;
                // }
                
            //}
            string getCourseCategory = category_coursecode;
            string CourseCode;
            string category;
            CourseCode.clear();
            category.clear();
            stringstream getCC;
            getCC << getCourseCategory;
            int split = 0;
            string substring;
            substring.clear();
            while(getline(getCC, substring, ' ') && split < 2){
                if(split == 0){
                    CourseCode = substring;
                }else{
                    category = substring;
                }
                split++;
            }
            getCourseCategory.clear();
            cout << "The main server received from " << hostname << " to query course " << CourseCode << " about " << category << " using TCP over port " << UDP_serverM_PORT << endl;
            
            // int requestFromClient = recv(childSocket, category_coursecode, strlen(category_coursecode), 0);
            //     if(requestFromClient == -1){
            //         printf("fail to receive request \n");
            //         exit(1);
            //     }
            cout << "receive request successfully" <<endl;
            cout << category_coursecode << endl;
            string request;
            request.clear();
            request = category_coursecode;
            // char* searchResult = new char[1000];
            //memset(searchResult, '\0', 1000);
            if(request.find("EE") == 0){
                initialServerEE();
                int sendToServerEE = sendto(UDP_socket, category_coursecode, strlen(category_coursecode), 0, (const struct sockaddr *) &serverEE_addr, sizeof(serverEE_addr));
                if(sendToServerEE == -1){
                    cout << "fail to send to server EE" << endl;
                    exit(1);
                }
                cout << "The main server sent to the request to serverEE." << endl;
                socklen_t serverEE_size = sizeof(serverEE_addr);
                char receiveEEResult[1000];
                memset(receiveEEResult, '\0', 1000);
                int receiveFromServerEE = recvfrom(UDP_socket, receiveEEResult, sizeof(receiveEEResult), 0, (struct sockaddr *) & serverEE_addr, &serverEE_size);
                if(receiveFromServerEE == -1){
                    cout << "fail to receive the result" << endl;
                    exit(1);
                }else{
                    cout << "The main server received the response from serverEE using UDP over port " << UDP_serverM_PORT << endl;
                }
                //cout << receiveEEResult << endl;
                // send to the client
                if(sendto(childSocket, receiveEEResult, strlen(receiveEEResult), 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1){
                    printf("fail to send the result to the client \n");
                    exit(1);
                }else{
                    printf("The main server sent the query information to the client. \n");
                }
                //cout << receiveEEResult << endl;
                
            }else if(request.find("CS") == 0){
                initialServerCS();
                int sendToServerCS = sendto(UDP_socket, category_coursecode, strlen(category_coursecode), 0, (const struct sockaddr *) &serverCS_addr, sizeof(serverCS_addr));
                if(sendToServerCS == -1){
                    cout << "fail to send to server CS" << endl;
                    exit(1);
                }
                cout << "The main server sent to the request to serverCS." << endl;
                socklen_t serverCS_size = sizeof(serverCS_addr);
                char receiveCSResult[1000];
                memset(receiveCSResult, '\0', 1000);
                int receiveFromServerCS = recvfrom(UDP_socket, receiveCSResult, sizeof(receiveCSResult), 0, (struct sockaddr *) & serverCS_addr, &serverCS_size);
                if(receiveFromServerCS == -1){
                    cout << "fail to receive the result" << endl;
                    exit(1);
                }else{
                    cout << "The main server received the response from serverCS using UDP over port " << UDP_serverM_PORT << endl;
                }
                //cout << receiveCSResult <<endl;
                //send to the client
                if(sendto(childSocket, receiveCSResult, strlen(receiveCSResult), 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1){
                    printf("fail to send the result to the client \n");
                    exit(1);
                }else{
                    printf("The main server sent the query information to the client. \n");
                }
                //cout << receiveCSResult <<endl;
            }
        }
        }
    }      

    

    }



    return 0;
}
