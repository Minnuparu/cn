#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char buf[100];
    int k;
    int sock_desc;
    struct sockaddr_in server;

    // Create socket (TCP)
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1) {
        printf("Error in socket creation!\n");
        return 1;
    }

    // Server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(3000);

    // Connect to server
    k = connect(sock_desc, (struct sockaddr *)&server, sizeof(server));
    if (k == -1) {
        printf("Error in connecting to server!\n");
        return 1;
    }

    printf("Connected to server. Start chatting (type exit to quit)\n");

    while (1) {
        // Send message
        printf("Client: ");
        fgets(buf, sizeof(buf), stdin);
        send(sock_desc, buf, strlen(buf), 0);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        // Receive reply
        k = recv(sock_desc, buf, sizeof(buf)-1, 0);
        if (k <= 0)
            break;

        buf[k] = '\0';
        printf("Server: %s", buf);

        if (strncmp(buf, "exit", 4) == 0)
            break;
    }

    close(sock_desc);
    return 0;
}