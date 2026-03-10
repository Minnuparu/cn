#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char buf[100];
    int k;
    socklen_t len;
    int sock_desc, temp_sock_desc;
    struct sockaddr_in server, client;

    // Create socket (TCP)
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1) {
        printf("Error in socket creation\n");
        return 1;
    }

    // Server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(3000);

    // Bind
    k = bind(sock_desc, (struct sockaddr *)&server, sizeof(server));
    if (k == -1) {
        printf("Error in binding\n");
        return 1;
    }

    // Listen
    listen(sock_desc, 5);
    printf("Server waiting for connection...\n");

    // Accept
    len = sizeof(client);
    temp_sock_desc = accept(sock_desc, (struct sockaddr*)&client, &len);
    if (temp_sock_desc == -1) {
        printf("Error in accept\n");
        return 1;
    }

    printf("Client connected. Start chatting\n");

    while (1) {
        // Receive message
        k = recv(temp_sock_desc, buf, sizeof(buf)-1, 0);
        if (k <= 0)
            break;

        buf[k] = '\0';
        printf("Client: %s", buf);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        // Send reply
        printf("Server: ");
        fgets(buf, sizeof(buf), stdin);
        send(temp_sock_desc, buf, strlen(buf), 0);

        if (strncmp(buf, "exit", 4) == 0)
            break;
    }

    close(temp_sock_desc);
    close(sock_desc);
    return 0;
}

