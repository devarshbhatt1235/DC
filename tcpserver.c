#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>         // for exit()
#include <unistd.h>         // for fork(), close(), read(), write()
#include <netinet/in.h>
#include <string.h>         // for memset()
#include <arpa/inet.h>      // for inet_pton()
#include <signal.h>         // for signal()

#define MAXLINESIZE 100
#define SERV_PORT 5555

int listensd, clientsd;
char buffer[MAXLINESIZE + 1];
struct sockaddr_in servaddr;
ssize_t noBytesRead;

/* Function prototype */
void processClient(int clientsd);

int main() {
    /* Ignore SIGCHLD to prevent zombie processes */
    signal(SIGCHLD, SIG_IGN);

    /* Create socket */
    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    /* Initialize socket address structure */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    /* Bind to a specific IP address */
    const char *bind_ip = "192.168.1.10";  // <-- change this to your desired IP address
    if (inet_pton(AF_INET, bind_ip, &servaddr.sin_addr) <= 0) {
        perror("Invalid IP address");
        exit(EXIT_FAILURE);
    }

    /* Bind socket address to the socket */
    if (bind(listensd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Error in bind");
        exit(EXIT_FAILURE);
    }

    /* Make the socket listening socket */
    if (listen(listensd, 5) < 0) {
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%d\n", bind_ip, SERV_PORT);

    for (;;) {
        /* Wait for client connection */
        clientsd = accept(listensd, NULL, NULL);
        if (clientsd < 0) {
            perror("Error in accept");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("Error in fork");
            close(clientsd);
            continue;
        }

        if (pid == 0) {
            /* Child process */
            close(listensd);  // Child does not need listening socket

            processClient(clientsd);

            close(clientsd);
            exit(EXIT_SUCCESS);
        }

        /* Parent process */
        close(clientsd);  // Parent does not need connected socket
    }

    /* Not reachable */
    close(listensd);
    return 0;
}

void processClient(int clientsd) {
    /* Read message from client and send back */
    while ((noBytesRead = read(clientsd, buffer, sizeof(buffer))) > 0) {
        if (write(clientsd, buffer, noBytesRead) < 0) {
            perror("Error writing to client");
            break;
        }
    }

    if (noBytesRead < 0) {
        perror("Error reading from client");
    }
}
