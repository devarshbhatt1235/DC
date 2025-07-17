#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>         // For exit()
#include <netinet/in.h>
#include <arpa/inet.h>      // For inet_pton()
#include <string.h>
#include <unistd.h>         // For read(), write()

#define MAXLINESIZE 100
#define SERV_PORT 5555

int main(int argc, char **argv) {
    int connectsd;
    char sendBuffer[MAXLINESIZE + 1];
    char recvBuffer[MAXLINESIZE + 1];
    struct sockaddr_in servaddr;
    ssize_t noBytesRead;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s IP-Address\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((connectsd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    // Initialize socket address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    // Assign server address
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "Error in inet_pton\n");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(connectsd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Error in connect");
        exit(EXIT_FAILURE);
    }

    // Read from stdin and communicate with server
    while (fgets(sendBuffer, sizeof(sendBuffer), stdin) != NULL) {
        // Remove trailing newline if present
        size_t len = strlen(sendBuffer);
        if (len > 0 && sendBuffer[len - 1] == '\n') {
            sendBuffer[len - 1] = '\0';
        }

        // Send to server
        if (write(connectsd, sendBuffer, strlen(sendBuffer) + 1) < 0) {
            perror("Error writing to socket");
            break;
        }

        // Read response from server
        noBytesRead = read(connectsd, recvBuffer, sizeof(recvBuffer) - 1);
        if (noBytesRead < 0) {
            perror("Error reading from socket");
            break;
        } else if (noBytesRead == 0) {
            // Server closed connection
            fprintf(stderr, "Server closed connection\n");
            break;
        }

        recvBuffer[noBytesRead] = '\0'; // Null terminate
        fprintf(stdout, "Server: %s\n", recvBuffer);
    }

    close(connectsd);
    return 0;
}
