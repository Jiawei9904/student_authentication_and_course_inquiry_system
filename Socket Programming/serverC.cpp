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

#define CredentialServerIPAddress "127.0.0.1"
#define CredentialServerPORT 21259


int UDP_socket;
struct sockaddr_in serverC_addr, serverM_addr;

string userName[2000];
string password[2000];


void createUDPSocket(){
    UDP_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(UDP_socket == -1){
        printf("fail to create UDP socket \n");
        exit(1);
    }
}

void initialServerM(){
    memset(&serverC_addr, 0, sizeof(serverC_addr));
    serverC_addr.sin_family = AF_INET;
    serverC_addr.sin_addr.s_addr = inet_addr(CredentialServerIPAddress);
    serverC_addr.sin_port = htons(CredentialServerPORT);
}

void bindProcess(){
    if(::bind(UDP_socket, (struct sockaddr *) &serverC_addr, sizeof(serverC_addr)) == -1){
        printf("bind process is failed \n");
        exit(1);
    }
}

// bool myCompare(string s1, string s2){
//     int i = 0;
//     int j = 0;
//     int l1 =s1.size();
//     int l2 = s2.size();
//     while(i < l1 && s1[i] == ' '){
//         i++;
//     }
//     while(j < l2 && s2[j] == ' '){
//         j++;
//     }
//     while(i < l1 && j < l2 && s1[1] != ' ' && s2[j] == ' '){
//         if(s1[i] != s2[j]){
//             return false;
//         }else{
//             i++;
//             j++;
//         }
//     }
//     if(l1 == l2){
//         return true;
//     }
//     if((i < l1 && s1[i] == ' ') || (i < l2 && s2[i]) == ' '){
//         return true;
//     }
//     return false;
// }
void inputFile(void){
    ifstream infile;
    infile.open("cred.txt");   // connect object with file
    string s;
    int i = 0;
    while(getline(infile, s)){
        std::stringstream substring(s);
        std::string substr;
        while(getline(substring,substr,',')){
            if(i % 2 == 0){
                userName[i / 2] = substr;
                i++;
            }else{
               
                    password[i / 2] = substr;
                    i++;
                
            }
        }
        
    }
    // for(int i = 0; i < 5; i++){
    //     for(int j = 0; j < password[i].size(); j++){
    //         string t = password[i];
    //     // cout << userName[i] << '\t';
    //     cout << (int)t[j] << endl;
    //     // cout << password[i].size() <<endl;
    //     }
    // }
    
}

string cleanEmpty(string s){
    string sub;
    for(int i = 0; i < s.size(); i++){
        if((int)s[i] != 13){
            sub += s[i];
        }
    }
    return sub;
} 

int main(void){
    inputFile();
    createUDPSocket();
    initialServerM();
    bindProcess();
    cout << "The ServerC is up and running using UDP port " << CredentialServerPORT << endl;
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
        
        printf("The ServerC received an authentication request from the Main Server. \n");
        
        //split the given string(hostname + ' ' + password)
        string hostnameSent;
        string passwordSent;
        string temp = receiveBuffer;
        stringstream ss;
        ss << temp;
        int count = 0;
        string str;
        while(getline(ss, str, ' ') && count < 2){
            if(count == 0){
                hostnameSent = str;
            }else{
                passwordSent = str;
            }
            count++;
        }
        // cout << hostnameSent << endl;
        // cout << passwordSent << endl;
        // check if hostname and password are vaild
        string result;
        if(hostnameSent.empty()){
            result = "No user";
            
        }else if(passwordSent.empty()){
            result = "No Password";
           
        }else{
            int i = 0;
            // cout << passwordSent << endl;
            for(i = 0; i < 2000; i++){
                if(userName[i].compare(hostnameSent) == 0){
                    if(password[i][password[i].size()-1] == ' '){
                        password[i] = password[i].substr(0, password[i].size() - 1);
                    }
                    string s1 = cleanEmpty(password[i]);
                    string s2 = cleanEmpty(passwordSent);
                    if(s1.compare(s2) == 0){
                        result = "Match";
                        break;
                    }else{
                        // cout << password[i] << endl;
                        // cout << passwordSent << endl;
                        result = "Password does not match";
                        break;
                    }
                }else{
                    continue;
                }
            }
            if(i == 2000 && result.empty()){
                result = "Username does not exist";
            }
            
        }

        // send the result back to the main server
        char sendBuffer[1024];
        strcpy(sendBuffer, result.data());
        if(::sendto(UDP_socket, sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *) &serverM_addr, sizeof(serverM_addr)) == -1){
            printf("fail to send the result to serverM \n");
            exit(1);
        }else{
            printf("The ServerC finished sending the response to the Main Server. \n");
        }

        cout << sendBuffer << endl;

    }
    return 0;
}