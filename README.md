# Socket Programming: Student Authentication and Course Inquiry System

## Project Overview

This project has successfully built an entire **C++** "Web registration" system allowing for smooth operation. It comprises the development of a client, main server, credential server, and dedicated servers for EE and CS courses information handling (serverEE and serverCS).

## System Functionality

- TCP and UDP Protocols: Implements TCP for client-main server communication and UDP for server-server interactions, ensuring efficient data transfer. [Each Terminal Video Demo](https://youtu.be/JszrB2YVyvM)

- Authentication Mechanism: Supports "username - password" authentication for secure access. `username:password` pairs are shown in `cred_unencrypted.txt`, but the input will be encrypted and `serverC` can **only** get access to `cred_encrypted.txt`. [Login Video Demo](https://youtu.be/yeSmR_of988)

- Offline Mechanism: Incorporates an offline mechanism to handle excessive authentication attempts, enhancing security. [Offline Mechanism Video Demo](https://youtu.be/f8NFjCZjGo0)

- Dynamic TCP Port: Features dynamic TCP port number allocation at the client terminal for flexibility and network conflict avoidance. [Dynamic TCP Port Check Video Demo](https://youtu.be/pE_iwc__8DA)

- Course Inquiry: Allows "section number - request" based course inquiries post-authentication. [Search Result Video Demo](https://youtu.be/MbLwtngLTLY)

## System Components

### Client (`client.cpp`)

- **Functionality:**
  - Establishes TCP communication with the main server.
  - Handles user input for usernames and passwords, and sends them to the main server.
  - Receives authentication result from the main server.
  - Upon successful login, inputs course code and category are taken and sent to the main server, which then returns the search results.

### Main Server (`serverM.cpp`)

- **Functionality:**
  - Utilizes TCP for client communication and UDP for communication with serverEE and serverCS.
  - Manages encryption of usernames and passwords, and communicates with the credential server for authentication.
  - Directs course code and category requests to the appropriate server (serverEE/serverCS) and relays the search results back to the client.

### Credential Server (`serverC.cpp`)

- **Functionality:**
  - Operates via UDP to interact with the main server.
  - Stores encrypted usernames and passwords, compares them against received credentials, and returns the result.

### EE and CS Course Information Servers (`serverEE.cpp` and `serverCS.cpp`)

- **Functionality:**
  - Both servers use UDP for communication with the main server.
  - They store detailed course information and handle requests for EE and CS courses respectively, returning the relevant information to the main server.

## Message Exchange Format

All messages exchanged within the system utilize a character array format, ensuring consistency and reliability in communication across different components.

## System Idiosyncrasies

The system maintaining a standard protocol for all processes. Notably, the TCP Bind Process in the main server may occasionally require up to a minute to respond, which is considered within normal operational parameters.

Several important considerations need to be noted for optimal system operation:

1. **Data Limitation in serverEE and serverCS:** Both `serverEE` and `serverCS` are currently configured to detect up to 2000 lines of data. Any data beyond 2000 lines may not be accurately detected.
2. **Course Search Formatting:** When searching for courses, the input must be in the format of **"EE“+ digits or "CS” + digits** (e.g., EE123 or CS456), and it is case-sensitive.

These idiosyncrasies highlight specific limitations and requirements within the system that users should be aware of to avoid potential issues.

## Code Reuse

- **Beej's Guide:** The project employs Beej's guide for creating TCP and UDP sockets, binding processes, and handling send/receive operations across all components, ensuring a robust and efficient communication framework.

## Usage Instructions

To deploy and run the web registration system:

1. **Compile:** Each `.cpp` file must be compiled using appropriate commands and a compatible compiler.
2. **Server Initialization:** Start the credential server, EE, and CS servers before initiating the main server to ensure proper communication pathways.
3. **Client Launch:** Run the client application, input authentication credentials, and proceed with course queries as prompted.
