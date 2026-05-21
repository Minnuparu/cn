#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define TOTAL_FRAMES 8
#define WINDOW_SIZE 4
#define TIMEOUT 2  // seconds

int main() {
    int sockfd;
    struct sockaddr_in receiver;
    socklen_t len = sizeof(receiver);

    int base = 0, nextseq = 0;
    char sendbuf[50], ackbuf[50];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&receiver, 0, sizeof(receiver));
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(7004);
    receiver.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("\nGBN Sender (Client) – Window Size = %d\n\n", WINDOW_SIZE);

    while (base < TOTAL_FRAMES) {

        // Send frames in window
        while (nextseq < base + WINDOW_SIZE && nextseq < TOTAL_FRAMES) {
            sprintf(sendbuf, "Frame %d", nextseq);
            sendto(sockfd, sendbuf, strlen(sendbuf) + 1, 0,
                   (struct sockaddr *)&receiver, len);
            printf("Sent -> %s\n", sendbuf);
            nextseq++;
        }

        // Wait for ACK with timeout
        fd_set fds;
        struct timeval tv;
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

        int rv = select(sockfd + 1, &fds, NULL, NULL, &tv);

        if (rv > 0) {
            // ACK received
            recvfrom(sockfd, ackbuf, sizeof(ackbuf), 0,
                     (struct sockaddr *)&receiver, &len);

            int ack;
            sscanf(ackbuf, "ACK %d", &ack);
            printf("Received <- %s\n", ackbuf);

            if (ack > base) {
                base = ack;  // Slide window
            }
        } else {
            // Timeout → retransmit all unacknowledged frames
            printf("\nTimeout! Retransmitting from Frame %d (lost frame or lost ACK)\n\n", base);
            nextseq = base;
        }
    }

    close(sockfd);
    return 0;
}