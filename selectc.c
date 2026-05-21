#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define TOTAL_FRAMES 8
#define WINDOW_SIZE 4

int main() {
    int sockfd;
    struct sockaddr_in receiver;
    socklen_t len = sizeof(receiver);

    int base = 0, nextseq = 0;
    int acked[TOTAL_FRAMES] = {0};

    char sendbuf[50], recvbuf[50];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(7004);
    receiver.sin_addr.s_addr = inet_addr("127.0.0.1");

    struct timeval tv = {2, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    printf("\nSelective Repeat Sender \n\n");

    while (base < TOTAL_FRAMES) {

        while (nextseq < base + WINDOW_SIZE && nextseq < TOTAL_FRAMES) {
            int checksum = nextseq % 2;

            if (nextseq == 5) checksum = 9;

            sprintf(sendbuf, "Frame %d %d", nextseq, checksum);
            sendto(sockfd, sendbuf, strlen(sendbuf)+1, 0,
                   (struct sockaddr *)&receiver, len);
            printf("Sent -> Frame %d\n", nextseq);
            nextseq++;
        }

        if (recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                     (struct sockaddr *)&receiver, &len) > 0) {

            int num;
            if (sscanf(recvbuf, "ACK %d", &num)) {
                printf("Received <- ACK %d\n", num);
                acked[num] = 1;

                while (acked[base])
                    base++;
            }
            else if (sscanf(recvbuf, "NACK %d", &num)) {
                printf("Received <- NACK %d (Retransmitting)\n", num);
                int checksum = num % 2;
                sprintf(sendbuf, "Frame %d %d", num, checksum);
                sendto(sockfd, sendbuf, strlen(sendbuf)+1, 0,
                       (struct sockaddr *)&receiver, len);
            }
        }
        else {
            printf("Timeout → Retransmitting frames\n");
            for (int i = base; i < nextseq; i++) {
                if (!acked[i]) {
                    sprintf(sendbuf, "Frame %d %d", i, i % 2);
                    sendto(sockfd, sendbuf, strlen(sendbuf)+1, 0,
                           (struct sockaddr *)&receiver, len);
                }
            }
        }
        printf("---------------------------------\n");
    }

    close(sockfd);
    return 0;
}

