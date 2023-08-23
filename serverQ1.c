/**************************************************\
|*   Computer Network Assignment 1 (Client)       *|
|*   Omid Almasi - 8/14/2023                      *|
|*   SCU Student ID: xxx-xxxx                     *|
|*   Professor Moataghedi                         *|
|*   Refer to README.txt for how to run the code  *|
\**************************************************/

//********************** Importing necessary standard headers **********************//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    unsigned short startID;
    unsigned char clientID;
    unsigned short data;
    unsigned char segmentNo;
    unsigned char length;
} DataHeader;

typedef struct {
    unsigned short endID;
} DataFooter;

typedef struct {
    unsigned short startID;
    unsigned char clientID;
    unsigned short ack;
    unsigned char segmentNo;
    unsigned short endID;
} AckPackage;

typedef struct {
    unsigned short startID;
    unsigned char clientID;
    unsigned short reject;
    unsigned short rejectCode;
    unsigned char segmentNo;
    unsigned short endID;
} RejectPackage;

void showError(char* msg) {
    perror(msg);
    exit(1);
}

// Print Packet Information for each transmission
void displayPacket(DataHeader header, DataFooter footer) {
    printf("\n=*=*=* Packet %d =*=*=*\n", header.segmentNo);
    printf("ID: \"%#X\"\n", header.startID);
    printf("Client: \"%#X\"\n", header.clientID);
    printf("Data: \"%#X\"\n", header.data);
    printf("Sequence Number: %d\n", header.segmentNo);
    printf("Payload Length: \"%#X\"\n", header.length);
    printf("End ID: \"%#X\"\n", footer.endID);
    printf("=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");

}

// Print Acknowledge Information for each tranmitted packet
void displayAck(AckPackage ack) {
    printf("\n=*=*=* Acknowledgment %d =*=*=*\n", ack.segmentNo);
    printf("ID: \"%#X\"\n", ack.startID);
    printf("Client: \"%#X\"\n", ack.clientID);
    printf("Acknowledge: \"%#X\"\n", ack.ack);
    printf("Sequence Number: %d\n", ack.segmentNo);
    printf("End ID: \"%#X\"\n", ack.endID);
    printf("=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");
}

// Print Reject Information for each tranmitted packet
void displayReject(RejectPackage reject) {
    printf("\n=*=*=* Rejection %d =*=*=*\n", reject.segmentNo);
    printf("ID: \"%#X\"\n", reject.startID);
    printf("Client: \"%#X\"\n", reject.clientID);
    printf("Reject Code: \"%#X\"\n", reject.rejectCode);
    printf("Sequence Number: %d\n", reject.segmentNo);
    printf("End ID: \"%#X\"\n", reject.endID);
    printf("=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n");

}
// Packet information per question prompt

void resetReject(RejectPackage* reject, DataHeader header, unsigned short code) {
    reject->startID = 0xFFFF;
    reject->clientID = header.clientID;
    reject->reject = 0xFFF3;
    reject->rejectCode = code;
    reject->segmentNo = header.segmentNo;
    reject->endID = 0xFFFF;
}

int main(int argc, char* argv[]) {
    int sockfd, n, counter = 0;
    struct sockaddr_in server, from;
    socklen_t fromlen;
    char buffer[255], package[1024], reply[16];
    DataHeader header;
    DataFooter footer;
    AckPackage ack = { 0xFFFF, 0xFF, 0xFFF2, 0x0, 0xFFFF };
    RejectPackage reject;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Function to create a UDP socket "SOCK_DGRAM for UDP"
    if (sockfd < 0) showError("Error opening socket"); //-1 is the error

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET; //IPV4
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    // Function to bind server to PORT
    if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        showError("Error on binding");
    }
    fromlen = sizeof(struct sockaddr_in);
    printf("Server is up and listening...\n");

    while (1) {
        n = recvfrom(sockfd, package, sizeof(package), 0, (struct sockaddr*)&from, &fromlen);
        if (n < 0) showError("Error on recvfrom");

        memcpy(&header, package, sizeof(header));
        memcpy(&buffer, package + sizeof(header), n - sizeof(header) - sizeof(footer));
        buffer[strlen(buffer)] = '\0';
        memcpy(&footer, package + n - sizeof(footer), sizeof(footer));

        printf("\nReceived Packet %d | Size: %d\nContent: %s", header.segmentNo, n, buffer);
        displayPacket(header, footer);

        // Packet analysis and error handling
        if (header.segmentNo == counter) {
            printf("Rejected: Duplicated Packet\n");
            resetReject(&reject, header, 0xFFF7);
            displayReject(reject);
            memcpy(reply, &reject, sizeof(reject));
        }
        else if (header.segmentNo != counter + 1) {
            printf("Rejected: Out of Sequence\n");
            resetReject(&reject, header, 0xFFF4);
            displayReject(reject);
            memcpy(reply, &reject, sizeof(reject));
        }
        else if (header.length != strlen(buffer)) {
            printf("Rejected: Length Mismatch\n");
            resetReject(&reject, header, 0xFFF5);
            displayReject(reject);
            memcpy(reply, &reject, sizeof(reject));
        }
        else if (footer.endID != 0xFFFF) {
            printf("Rejected: End of Packet Missing\n");
            resetReject(&reject, header, 0xFFF6);
            displayReject(reject);
            memcpy(reply, &reject, sizeof(reject));
        }
        else {
            counter++;
            ack.clientID = header.clientID;
            ack.segmentNo = header.segmentNo;
            displayAck(ack);
            memcpy(reply, &ack, sizeof(ack));

            if (counter == 5) {
                printf("\nResetting Packet Counter...\n");
                counter = 0;
            }
        }

        n = sendto(sockfd, reply, sizeof(ack), 0, (struct sockaddr*)&from, fromlen);
        if (n < 0) showError("Error sending ACK/Reject");
    }

    return 0;
}
