#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char username[50], password[50];
    char command[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(client_socket,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr));

    recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    // USER
    printf("Username: ");
    scanf("%s", username);

    sprintf(buffer, "USER %s", username);
    send(client_socket, buffer, strlen(buffer), 0);

    recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    // PASS
    printf("Password: ");
    scanf("%s", password);

    sprintf(buffer, "PASS %s", password);
    send(client_socket, buffer, strlen(buffer), 0);

    recv(client_socket, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);

    getchar();

    while (1)
    {
        printf("ftp> ");
        fgets(command, BUFFER_SIZE, stdin);

        send(client_socket, command, strlen(command), 0);

        if (strncmp(command, "STOR", 4) == 0)
        {
            printf("Enter file content: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(client_socket, buffer, strlen(buffer), 0);
        }

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);

            int bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);

            if (bytes <= 0)
                break;

            printf("%s", buffer);

            if (strstr(buffer, "226") ||
                strstr(buffer, "221") ||
                strstr(buffer, "550") ||
                strstr(buffer, "250") ||
                strstr(buffer, "500"))
                break;
        }

        if (strncmp(command, "QUIT", 4) == 0)
            break;
    }

    close(client_socket);

    return 0;
}