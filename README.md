# Socket Programming: Student Authentication and Course Inquiry System

## Project Overview

This project has successfully built an entire **C++** "Web registration" system allowing for smooth operation. It comprises the development of a client, main server, credential server, and dedicated servers for EE and CS courses information handling (serverEE and serverCS).

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

The system operates without any peculiarities, maintaining a standard protocol for all processes. Notably, the TCP Bind Process in the main server may occasionally require up to a minute to respond, which is considered within normal operational parameters.

## Code Reuse

- **Beej's Guide:** The project employs Beej's guide for creating TCP and UDP sockets, binding processes, and handling send/receive operations across all components, ensuring a robust and efficient communication framework.

## Usage Instructions

To deploy and run the web registration system:

1. **Compile:** Each `.cpp` file must be compiled using appropriate commands and a compatible compiler.
2. **Server Initialization:** Start the credential server, EE, and CS servers before initiating the main server to ensure proper communication pathways.
3. **Client Launch:** Run the client application, input authentication credentials, and proceed with course queries as prompted.
