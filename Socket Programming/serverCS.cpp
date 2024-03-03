#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
using namespace std;

#define ServerCSIPAddress "127.0.0.1"
#define serverCS_Port 22259

int UDP_socket;
struct sockaddr_in serverCS_addr, serverM_addr;

using namespace std;

string courseSection[2000];
string credit[2000];
string professor[2000];
string date[2000];
string courseName[2000];

void createUDPSocket(){
    UDP_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(UDP_socket == -1){
        printf("fail to create UDP socket \n");
        exit(1);
    }
}

void initialServerM(){
    memset(&serverCS_addr, 0, sizeof(serverCS_addr));
    serverCS_addr.sin_family = AF_INET;
    serverCS_addr.sin_addr.s_addr = inet_addr(ServerCSIPAddress);
    serverCS_addr.sin_port = htons(serverCS_Port);
}

void bindProcess(){
    if(::bind(UDP_socket, (struct sockaddr *) &serverCS_addr, sizeof(serverCS_addr)) == -1){
        printf("bind process is failed \n");
        exit(1);
    }
}
void inputFile(void){
    ifstream infile;
    infile.open("cs.txt");   // connect object with file
    string s;
    int i = 0;
    while(getline(infile, s)){
        std::stringstream substring(s);
        std::string substr;
        while(getline(substring,substr,',')){
            if(i % 5 == 0){
                courseSection[i / 5] = substr;
                i++;
            }else if(i % 5 == 1){
                credit[i / 5] = substr;
                i++;
            }else if(i % 5 == 2){
                professor[i / 5] = substr;
                i++;
            }else if(i % 5 ==3){
                date[i / 5] = substr;
                i++;
            }else{
                courseName[i / 5] = substr;
                i++;
            }
        }
        
    }
    infile.close();
}

int main(void){
    inputFile();
    createUDPSocket();
    initialServerM();
    bindProcess();
    cout << "The ServerCS is up and using UDP on port " << serverCS_Port << endl;
    while(1){
        char receiveBuffer[1000];
        memset(receiveBuffer, '\0', 1000);
        while(1){
            int n;
            socklen_t serverM_size = sizeof(serverM_addr);
            // receive hostname and password (encrypted) from main server(ServerM)
            n = recvfrom(UDP_socket, receiveBuffer, sizeof(receiveBuffer), 0, (struct sockaddr *) & serverM_addr, &serverM_size);
            if(n == -1){
                printf("receiving process has been failed \n");
                exit(1);
            }
            if(n == 0){
                continue;
            }else{
                break;
            }
        }
        
        
        
        //split the given string(courseCode + ' ' + request)
        string courseCode;
        string request;
        string temp = receiveBuffer;
        stringstream ss;
        ss << temp;
        int count = 0;
        string str;
        while(getline(ss, str, ' ') && count < 2){
            if(count == 0){
                courseCode = str;
            }else{
                request = str;
            }
            count++;
        }

        cout << "The ServerCS received a request from the Main Server about the " << request << " of " << courseCode << endl;
        // check if hostname and password are vaild
        string result;
            int i = 0;
            for(i = 0; i < 2000; i++){
                if(courseSection[i].compare(courseCode) == 0){
                    if(request == "Credit"){
                        result = credit[i];
                        break;
                    }else if(request == "Professor"){
                        result = professor[i];
                        break;
                    }else if(request == "Days"){
                        result = date[i];
                        break;
                    }else if(request == "CourseName"){
                        result = courseName[i];
                        break;
                    }else{
                        result = "Invaild request / category";
                    }
                }else{
                    continue;
                }
            }
            if(i == 5 && result.empty()){
                cout << "Didn't find the course: " << courseCode << endl;
                result = "No find";
            }else{
                cout << "The course information has been found: The " << request << " of " << courseCode << " is " << result << endl;
            }

        // send the result back to the main server
        char sendBuffer[1024];
        strcpy(sendBuffer, result.data());
        if(::sendto(UDP_socket, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *) &serverM_addr, sizeof(serverM_addr)) == -1){
            printf("fail to send the result to serverM \n");
            exit(1);
        }else{
            printf("The serverCS finished sending the response to the Main Server. \n");
        }
        // cout << sendBuffer << endl;

    }
    return 0;
}