#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket)
{
    char buffer[BUFFER_SIZE];
    char username[50], password[50];

    send(client_socket, "220 FTP Server Ready\n", 21, 0);

    // USER
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    sscanf(buffer, "USER %s", username);

    send(client_socket, "331 Username OK, need password\n", 32, 0);

    // PASS
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    sscanf(buffer, "PASS %s", password);

    if (strcmp(username, "admin") == 0 &&
        strcmp(password, "1234") == 0)
    {
        send(client_socket, "230 Login successful\n", 21, 0);
        printf("Client logged in\n");
    }
    else
    {
        send(client_socket, "530 Login incorrect\n", 20, 0);
        close(client_socket);
        return;
    }

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        int bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (bytes <= 0)
            break;

        // LIST
        if (strncmp(buffer, "LIST", 4) == 0)
        {
            FILE *fp = popen("ls", "r");

            send(client_socket, "150 Opening directory\n", 22, 0);

            while (fgets(buffer, BUFFER_SIZE, fp))
                send(client_socket, buffer, strlen(buffer), 0);

            pclose(fp);

            send(client_socket, "226 Directory transfer complete\n", 32, 0);

            printf("Directory listed\n");
        }

        // RETR
        else if (strncmp(buffer, "RETR", 4) == 0)
        {
            char filename[100];
            sscanf(buffer, "RETR %s", filename);

            FILE *fp = fopen(filename, "r");

            if (!fp)
            {
                send(client_socket, "550 File not found\n", 19, 0);
                printf("Download failed\n");
            }
            else
            {
                send(client_socket, "150 Opening file\n", 17, 0);

                while (fgets(buffer, BUFFER_SIZE, fp))
                    send(client_socket, buffer, strlen(buffer), 0);

                fclose(fp);

                send(client_socket, "226 Download complete\n", 22, 0);

                printf("File downloaded: %s\n", filename);
            }
        }

        // STOR
        else if (strncmp(buffer, "STOR", 4) == 0)
        {
            char filename[100];
            sscanf(buffer, "STOR %s", filename);

            FILE *fp = fopen(filename, "w");

            if (!fp)
            {
                send(client_socket, "550 Cannot create file\n", 23, 0);
            }
            else
            {
                send(client_socket, "150 Ready to receive\n", 21, 0);

                memset(buffer, 0, BUFFER_SIZE);
                recv(client_socket, buffer, BUFFER_SIZE, 0);

                fprintf(fp, "%s", buffer);

                fclose(fp);

                send(client_socket, "226 Upload complete\n", 21, 0);

                printf("File uploaded: %s\n", filename);
            }
        }

        // DELE
        else if (strncmp(buffer, "DELE", 4) == 0)
        {
            char filename[100];
            sscanf(buffer, "DELE %s", filename);

            if (remove(filename) == 0)
            {
                send(client_socket, "250 File deleted\n", 17, 0);
                printf("File deleted: %s\n", filename);
            }
            else
            {
                send(client_socket, "550 File not found\n", 19, 0);
            }
        }

        // QUIT
        else if (strncmp(buffer, "QUIT", 4) == 0)
        {
            send(client_socket, "221 Goodbye\n", 12, 0);
            printf("Client disconnected\n");
            break;
        }

        else
        {
            send(client_socket, "500 Unknown command\n", 20, 0);
        }
    }

    close(client_socket);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(server_socket, 5);

    printf("FTP Server running on port %d\n", PORT);

    while (1)
    {
        client_socket = accept(server_socket,
                               (struct sockaddr *)&client_addr,
                               &addr_len);

        printf("Client connected\n");

        handle_client(client_socket);
    }

    close(server_socket);

    return 0;
}