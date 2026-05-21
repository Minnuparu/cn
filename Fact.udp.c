#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int sockfd, n, fact;
    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter number: ");
    scanf("%d", &n);

    sendto(sockfd, &n, sizeof(n), 0,
           (struct sockaddr*)&server, sizeof(server));

    recvfrom(sockfd, &fact, sizeof(fact), 0,
             NULL, NULL);

    printf("Factorial = %d\n", fact);

    close(sockfd);

    return 0;
}
