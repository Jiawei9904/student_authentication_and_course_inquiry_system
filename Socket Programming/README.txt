Developer: Jiawei Guo


StudentID: 6673891259


Work that has completed: Build the whole system of "Web registration" and let the system run normally.
Develop client, main server, credential server, serverEE(storing information of EE courses and get the result of searching), serverCS(storing
CS courses' information and get the result of searching).

For client end:
Client first needs to get connection with the main server and get input user names and passwords. After getting the user names and passwords,
will send the information to the main server(serverM) and get the response "PASS / NO PASS"(for example) given by credential server(serverC).
After logging in successfully, client will get the input of course code and category from the keyboard and will send the information to the main
serever. After getting the results of searching, the results of will be presented in the terminal of client.

For main server:
Main server needs to use TCP to communicate with the client and needs to use UDP to communication with serverEE and serverCS. First, the main
server will develop the parent socket do the bind process. After accept process, main server will get the username and pasword given by client.
The main server will encrypted the username and password by offsetting 4. After the encryption, main server will send the encrypted information 
to the credential server(serverC) and send the log-in result to the client. After getting log-in successful information, main server will receive 
the course code and category from the client and send the result given by the corresponding server to the client.

For credential server(serverC):
Credential server needs to use UDP to communicate with main server. First, the credential server needs to create the UDP socket and stores the 
encrypted usernames and passwords. After receiving the username and password passed by the main server, the credential server will compare the 
received username and password with storing usernames and passwords and send the result to the main server. 
(main server should then passes the result to the client.)

For serverEE and serverCS:
serverCS and serverEE need to use UDP to communicate with main server. First, serverEE and serverCS develop the UDP sockets and store the detailed
informsation about EE / CS courses. When the main server sends the EE courses and CS courses request to the corresponding server(serverEE / serverCS),
the corresponding server will find if the coursecode is vaild and get the information that client end requests. After getting the result, serverEE / 
serverCS will send the result to the main server and the main server pass the result to the client.


Code files and their functions:
    1. client.cpp
    function:
        Create TCP socket for communicating with the main server.
        Get connections with the main server.
        Get input username and password.
        Send the input username and password to the main server.
        Receiving the authenticatimon result passed by the main server.
        If aunthentication is successful:
            Get the input course code and category / request.
            Send the course code and category / request to the main server.
            Receive the searching result passed by the main server.
        If aunthentication is failed:
            If attempts < 3:
            Get input username and password again.
            Resend the input username and password. 
            Re-receive the authentication result passed by the main server.
            if attempts > 3:
            exit the program.

    2. serverM.cpp (main server):
    function:
        Create TCP parent socket for communicating with the client.
        Bind the socket with main server's address information and TCP properties.
        Create the UDP socket for communicating with other servers.
        Accept the socket from the client and develop the child socket for further process.
        Receive the username and password from the client.
        Encrypted username and password.
        Send the encrypted username and password to serverC (credential server).
        Receive the authentication result from the serverC.
        Send the authentication result to the client.
        Receive the course code and category / request from the client.
        Determine which server (serverEE / serverCS) to send.
        Send the course code and category / reuqest to the corresponding server. (serverEE / serverCS)
        Receive the result from the corresponding server.
        Send the searching result from to the client.

    3. serverC.cpp (credential server):
    fucntion:
        Create UDP socket socket for communicating with the main server.
        Create "hostname" and "password" string arrays to store the input file's(cred.txt) hostname and password.
        Receive the username and password passed by the main server. 
        Compare the recieved username and password with the file's content.
        Send the compare result to the main server.

    4. serverEE.cpp (serverEE):
    function:
        Create UDP socket socket for communicating with the main server.
        Create "CourseName", "Credit", "Date", "Professor", "courseSection" string arrays to store the content given by the input file. (EE.txt)
        Receive the course code and category / request passed by the main server.
        Comapre the course code with the contents in the "courseSection" array.
        Get the results of the corresponding category.
        Send the result to the main server.

    5. serverCS.cpp (ServerCS):
    function:
        Create UDP socket socket for communicating with the main server.
        Create "CourseName", "Credit", "Date", "Professor", "courseSection" string arrays to store the content given by the input file. (CS.txt)
        Receive the course code and category / request passed by the main server.
        Comapre the course code with the contents in the "courseSection" array.
        Get the results of the corresponding category.
        Send the result to the main server.


The format of all messages exchanged:
    1. client
        strings for getting input password and username: hostname, password.
        char array for gathering password and username and send to the main server together: clientSent
        char array for getting result of authentication: sereverReply
        strings for getting input course code and category / request: CourseCode, category
        char array for gathering course code and category / request: sendToServerM
        char array for getting result of searching result: searchResult

    2. serverM (main server):
        char array for receiving the input password and username: buffer
        strings for storing the encrypted username and paswords: encrypted_hostname, encrypted_password
        char array for sending the encrypted username and password to the credential server: UDP_send
        char array for receiving the aunthentication result from credential server: loginResult
        char array for sending the aunthentication result to the client: sendToClient
        char array for receiving course code and category / request from the client: category_coursecode
        char array for sending course code and category / request to the corresponding server (serverEE / serverCS): category_coursecode
        char array for receiving the searching result from the corresponding server (serverEE / serverCS): receiveEEResult, receiveCSResult
        char array for sending the searching result to the client: receiveEEResult, receiveCSResult

    3. severC (credential server):
        char array for receiving the encrypted username and password: receiveBuffer
        char array for sending the authentication result to the main server: sendBuffer

    4. serverEE:
        char array for receiving the encrypted username and password: receiveBuffer
        char array for sending the authentication result to the main server: sendBuffer
    
    5. serverCS:
        char array for receiving the encrypted username and password: receiveBuffer
        char array for sending the authentication result to the main server: sendBuffer

    Conclusion: all the exchanged messages' format is the character array.

Idiosyncrasy:
    There is no Idiosyncrasy in the system.
    Occasionally, 1 minute need to wait the reaction of TCP Bind Process in main server 

Reuse Code:
    1. Beej's Code
        Create TCP, UDP sockets in each cpp file
        Bind process in each cpp file
        Send and receive process of TCP and UDP in each file