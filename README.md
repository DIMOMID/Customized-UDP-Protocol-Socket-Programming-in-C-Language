# Customized UDP Protocol for client & server (Socket Programming) in C Language
/**************************************************\

|*   Computer Network Assignment 1 (Client)       *|

|*   Omid Almasi - 8/14/2023                      *|

|*   SCU Student ID: xxxxxxxxx                    *|

|*   Professor Moataghedi                         *|

|*   Refer to README.txt for how to run the code  *|

\**************************************************/


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
