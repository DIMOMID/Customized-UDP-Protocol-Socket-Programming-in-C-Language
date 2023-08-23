# Customized UDP Protocol for client & server (Socket Programming) in C Language
/**************************************************\

|*   Computer Network Assignment 1 (Client)       *|

|*   Omid Almasi - 8/14/2023                      *|

|*   SCU Student ID: xxxxxxxxx                    *|

|*   Professor Moataghedi                         *|

|*   Refer to README.txt for how to run the code  *|

\**************************************************/


//********************** Importing necessary standard headers **********************//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>


//Data packet structure:
struct DataPackage_HEADER
{
    unsigned short int startID;
    unsigned char clientID;
    unsigned short int data;
    unsigned char segmentNo;
    unsigned char length;
} header = { 0xFFFF, 0xFF, 0xFFF1, 0x0, 0xFF };
struct DataPackage_ENDER
{
    unsigned short int endID;
} ender = { 0xFFFF };

//Acknowledgement packet structure:
struct ACK_PACKAGE
{
    unsigned short int startID;
    unsigned char clientID;
    unsigned short int ack;
    unsigned char segmentNo;
    unsigned short int endID;
} ack = { 0xFFFF, 0xFF, 0xFFF2, 0x0, 0xFFFF };

//Reject packet structure:
struct Reject_PACKAGE
{
    unsigned short int startID;
    unsigned char clientID;
    unsigned short int reject;
    unsigned short int rejectCode;
    unsigned char segmentNo;
    unsigned short int endID;
} reject = { 0xFFFF, 0xFF, 0xFFF3, 0xFFF4, 0x0, 0xFFFF };


// Print Packet Information for each transmission
void displayPacket()
{
    printf("\n=*=*=* Packet %d=*=*=*n", header.segmentNo);
    printf("ID: \"%#X\"\n", header.startID);
    printf("Client:  \"%#X\"\n", header.clientID);
    printf("Data: \"%#X\"\n", header.data);
    printf("Sequence Number: \"%d\"\n", header.segmentNo);
    printf("Payload Length: \"%#X\"\n", header.length);
    printf("End ID: \"%#X\"\n", ender.endID);
    printf("=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n\n");
}

// Print Acknowledge Information for each tranmitted packet
void printAck()
{
    printf("\n=*=*=* Ack %d =*=*=*\n", ack.segmentNo);
    printf("ID \"%#X\"\n", ack.startID);
    printf("Client \"%#X\"\n", ack.clientID);
    printf("Acknowledge: \"%#X\"\n", ack.ack);
    printf("Sequence Number \"%d\"\n", ack.segmentNo);
    printf("End ID \"%#X\"\n", ack.endID);
    printf("=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n\n");
}

// Print Reject Information for each tranmitted packet
void printReject()
{
    printf("\n=*=*=*  Reject Package %d =*=*=* \n",
        reject.segmentNo);
    printf("ID \"%#X\"\n", reject.startID);
    printf("Client \"%#X\"\n", reject.clientID);
    printf("REJECT \"%#X\"\n", reject.reject);
    printf("Reject Code: \"%#X\"\n", reject.rejectCode);
    printf("Sequence Number \"%d\"\n", reject.segmentNo);
    printf("End ID \"%#X\"\n", reject.endID);
    printf("=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n\n");
}


// Packet information per question prompt
void reject_builder(unsigned short int code)
{
    reject.startID = 0xFFFF;
    reject.clientID = header.clientID;
    reject.reject = 0xFFF3;
    reject.rejectCode = code;
    reject.segmentNo = header.segmentNo;
    reject.endID = 0xFFFF;
}
void error(char* msg)
{
    perror(msg);
    exit(1);
}

char buffer[255];
char package[264];
char reply[16];
int sockfd;
int n;
int packageNo;
int counter;
struct sockaddr_in server;
struct sockaddr_in from;
struct hostent* hp;
socklen_t length;
char payload[256];
struct timeval timeout;

void create_packet(int seg, char* payload) //pac
{
    header.segmentNo = seg;
    header.length = strlen(payload);
    memset(package, 0, 264);
    memcpy(package, &header, sizeof(header));
    memcpy(package + sizeof(header), payload, strlen(payload));
    memcpy(package + sizeof(header) + strlen(payload), &ender, 2);
    printf("Packet created:");
    displayPacket();
}
void send_package()
{
    //try 3 times or quit per question prompt
    counter = 3;
    while (counter >= 0)
    {
        //sending packet to server
        if (counter == 3)
        {
            printf("\nSending message: %s\n", payload);
        }
        else
        {
            printf("\nError sending message. Trying to resend the message");
            printf("\nSending message: %s\n", payload);
        }
        n = sendto(sockfd, package, sizeof(header) + strlen(payload) + 2, 0, (struct sockaddr*)&server, length);
        if (n < 0) {
            error("Client failed to send the packet.\n");
        }
        //waiting for ack
        bzero(reply, 16);
        n = recvfrom(sockfd, reply, 16, 0, (struct sockaddr*)&from, &length);


        if (n >= 0) {
            memcpy(&ack, reply, sizeof(ack));
            if (ack.ack == 0xFFF2) {
                printf("Acknowledgement received.\n ");
                printAck();
                break;
            }
            else {
                memcpy(&reject, reply, sizeof(reject));
                printf("Received a reject packet.\n ");
                printf("Error Message: ");
                if (reject.rejectCode == 0xFFF4) {
                    printf("Rejected: Out of Sequence\n");
                }
                else if (reject.rejectCode == 0xFFF5) {
                    printf("Rejected: Length Mismatch\n");
                }
                else if (reject.rejectCode == 0xFFF6) {
                    printf("Rejected: End of Packet Missing\n");
                }
                else if (reject.rejectCode == 0xFFF7) {
                    printf("Rejected: Duplicated Packet\n");
                }
                else {
                    printf("Undefined error.\n");
                }
                printReject();
                counter++;
                break;
            }
        }
        counter--;
    }
    if (counter < 0) {
        error("Error: Server does not respond\n");
    }
}
int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Enter the folllowing command in terminal to run the code.");
        printf("./client server_name port_number(<localhost/ 127.0.0.1> 5050)\n");
        exit(1);
    }
    // socket creation. SOCK_DGRAM for UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        error("Client error opening socket.\n");
    }
    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp == 0) {
        error("Unknown host for Client.\n");
        printf("./client server_name port_number(<localhost/ 127.0.0.1> 5050)\n");

    }
    bcopy((char*)hp->h_addr_list[0], (char*)&server.sin_addr, hp->h_length);

    server.sin_port = htons(atoi(argv[2]));
    length = sizeof(struct sockaddr_in);
    //Set ack_timer= 3 per question
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    //Simulating First 5 error-free packets per question
    printf("\n\nTest Case# 1: Sending First 5 error-free packets in sequence\n\n");
    packageNo = 1;
    while (packageNo <= 5) {
        //creating packages
        bzero(payload, 256);
        memcpy(payload, "Packet Payload:.\n", 18); //number of byte from source to destination buffer (to avoid buffer overflow)
        create_packet(packageNo, payload);
        send_package();
        packageNo++;
    }
    printf("\nResetting Packet Counter for next test case...\n");

    //Simulating second 5 packets for testing error handling (4 cases) per question
    printf("\nTest Case#2: Sending 5 packets to test errors handleing (1 Correct & 4 Errors)\n");

    //Packet# 1 : correct packet sent
    printf("Packet# 1: Correct Packet.\n");
    bzero(payload, 256);
    memcpy(payload, "Packet Payload.\n", 18);
    create_packet(1, payload);
    send_package();

    //Packet# 2: duplication error
    printf("Packet# 2: Simulating duplicate packet error\n");
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 18);
    create_packet(1, payload);
    send_package();

    //Packet# 3: out of sequence error
    printf("Packet# 3: Simulating out of sequence error\n");
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 18);
    create_packet(3, payload);
    send_package();

    //Packet# 4: length mismatch error
    printf("Packet# 4: Simulating length mismatch error\n");
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 18);
    header.segmentNo = 2;
    header.length = 0x44;
    memset(package, 0, 264);
    memcpy(package, &header, sizeof(header));
    memcpy(package + sizeof(header), payload, strlen(payload));
    memcpy(package + sizeof(header) + strlen(payload), &ender, 2);
    printf("Packet created:");
    displayPacket();
    send_package();

    //Packet# 5: end of packet missing error
    printf("Packet# 5: Simulating end of packet missing error\n");
    bzero(payload, 256);
    memcpy(payload, "Payload of a package.\n", 18);
    create_packet(2, payload);
    ender.endID = 0xFFF0;
    memcpy(package + sizeof(header) + strlen(payload), &ender, 2);
    printf("End of packet ID changed");
    displayPacket();
    send_package();
}
