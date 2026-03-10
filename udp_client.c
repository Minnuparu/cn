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
    int sock_desc;
    socklen_t len;
    struct sockaddr_in server;

    // Create UDP socket
    sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_desc == -1) {
        printf("Error in socket creation\n");
        return 1;
    }

    // Server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(3000);

    len = sizeof(server);

    printf("UDP Client ready. Start chatting (type exit to quit)\n");

    while (1) {
        // Send message
        printf("Client: ");
        fgets(buf, sizeof(buf), stdin);

        sendto(sock_desc, buf, strlen(buf), 0,
               (struct sockaddr *)&server, len);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        // Receive reply
        int k = recvfrom(sock_desc, buf, sizeof(buf)-1, 0,
                         NULL, NULL);

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
