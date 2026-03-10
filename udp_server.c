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
    struct sockaddr_in server, client;

    // Create UDP socket
    sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_desc == -1) {
        printf("Error in socket creation\n");
        return 1;
    }

    // Server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(3000);

    // Bind
    if (bind(sock_desc, (struct sockaddr *)&server, sizeof(server)) == -1) {
        printf("Error in binding\n");
        return 1;
    }

    printf("UDP Server ready. Waiting for client...\n");

    len = sizeof(client);

    while (1) {
        // Receive from client
        int k = recvfrom(sock_desc, buf, sizeof(buf)-1, 0,
                         (struct sockaddr *)&client, &len);

        if (k <= 0)
            break;

        buf[k] = '\0';
        printf("Client: %s", buf);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        // Send reply
        printf("Server: ");
        fgets(buf, sizeof(buf), stdin);

        sendto(sock_desc, buf, strlen(buf), 0,
               (struct sockaddr *)&client, len);

        if (strncmp(buf, "exit", 4) == 0)
            break;
    }

    close(sock_desc);
    return 0;
}
