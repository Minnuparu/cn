#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sockfd, n, fact = 1, i;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server, sizeof(server));

    printf("UDP Server waiting...\n");

    recvfrom(sockfd, &n, sizeof(n), 0,
             (struct sockaddr*)&client, &len);

    for(i = 1; i <= n; i++)
        fact *= i;

    sendto(sockfd, &fact, sizeof(fact), 0,
           (struct sockaddr*)&client, len);

    close(sockfd);

    return 0;
}
