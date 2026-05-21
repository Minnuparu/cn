#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int sockfd, n, fact;
    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd,
            (struct sockaddr*)&server,
            sizeof(server));

    printf("Enter a number: ");
    scanf("%d", &n);

    send(sockfd, &n, sizeof(n), 0);

    recv(sockfd, &fact, sizeof(fact), 0);

    printf("Factorial = %d\n", fact);

    close(sockfd);

    return 0;
}
