#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sockfd, newsockfd, n, fact = 1, i;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    listen(sockfd, 5);

    printf("Waiting for client...\n");

    newsockfd = accept(sockfd,
                       (struct sockaddr*)&client, &len);

    recv(newsockfd, &n, sizeof(n), 0);

    for(i = 1; i <= n; i++)
        fact *= i;

    send(newsockfd, &fact, sizeof(fact), 0);

    close(newsockfd);
    close(sockfd);

    return 0;
}
