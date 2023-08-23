# Customized UDP Protocol for client & server (Socket Programming) in C Language
/**************************************************\

|*   Computer Network Assignment 1 (Client)       *|

|*   Omid Almasi - 8/14/2023                      *|

|*   SCU Student ID: xxxxxxxxx                    *|

|*   Professor Moataghedi                         *|

|*   Refer to README.txt for how to run the code  *|

\**************************************************/
## Breif Introduction
This protocol is a custom design protocl built on top of the standard UDP protocol, enhancing the communication between a client and a server with a set of predefined primitives and error handling mechanisms. Unlike the traditional UDP, which offers no guarantees on packet delivery, our custom UDP ensures more reliable communication by introducing acknowledgments (ACKs) and rejection sub-codes.

##Features:

Packet Identifiers: We employ unique identifiers for the start and end of packets (0XFFFF) ensuring packet integrity.
Client Identification: A unique ID for each client, allowing support for up to 255 clients.
Defined Packet Types:
DATA (0XFFF1) for data transmission.
ACK (0XFFF2) for positive acknowledgments.
REJECT (0XFFF3) with specific sub-codes to provide precise feedback on encountered errors.
Error Handling:
Out of sequence packets.
Length mismatches.
Missing end of packet identifier.
Duplicate packet detection.
##Workflow:

The client sends a series of packets to the server.
For each correctly received packet, the server acknowledges with an ACK.
If there are issues with the received packets, the server responds with the appropriate REJECT code.
The client utilizes an acknowledgment timer (ack_timer) to ensure timely responses. If an ACK isn't received in the set time, the client retransmits the packet.
The primary objective of this protocol is to add a layer of reliability to UDP. Through our error detection and feedback mechanisms, we aim to minimize data corruption and loss during transmission. Dive into the codebase to explore more, and feel free to contribute!
## How to run the code

The following are instruction for runnning both client.c & server.c for question #1:

1. Launch Terminal:
	* Start your Ubuntu terminal.
2. Open the Code:
	* Run the command code . within the terminal to open Visual Studio Code in the current directory.
3. Navigate to Source Directory:
	* In Visual Studio Code, navigate to the folder where both client.c and server.c files are located.
	* Right-click the folder and choose "Open in Integrated Terminal".
4. Setup Terminal:
	* Split the integrated terminal by clicking the split terminal button (usually represented by a square and arrow) to simulate both client and server concurrently.
5. Compile the Server Code:
	* Enter the following command to build and compile the server:
	"gcc -o server server.c"
6. Compile the Client Code:
	* Enter the following command to build and compile the client:
	* "gcc -o client client.c"
7. Run the Server:
	* First, execute the server by entering the following command:
	* "./server <port number>"
	* For example: "./server 5050"
8. Run the Client:
	* With the server running, switch to the other terminal pane. Execute the client with:
	* ./client <host name> <port number>
	* For example: "./client 127.0.0.1 5050"
9. Ensure that the port number matches the server port number. For localhost, you can use the IP address "127.0.0.1" or its symbolic name.
