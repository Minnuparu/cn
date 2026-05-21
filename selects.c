include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TOTAL_FRAMES 8
#define WINDOW_SIZE 4

int is_corrupted(int frame, int checksum) {
    return (frame % 2) != checksum;
}

int main() {
    int sockfd;
    struct sockaddr_in receiver, sender;
    socklen_t len = sizeof(sender);

    int base = 0;
    int received[TOTAL_FRAMES] = {0};

    char recvbuf[50], sendbuf[50];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    int lost_once = 0;

    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(7004);
    receiver.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&receiver, sizeof(receiver));

    printf("\nSelective Repeat Receiver (Loss + Corruption + NACK)\n\n");

    while (base < TOTAL_FRAMES) {

        recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,
                 (struct sockaddr *)&sender, &len);

        int frame, checksum;
        sscanf(recvbuf, "Frame %d %d", &frame, &checksum);

        if (frame == 3 && !lost_once) {
            printf("Frame %d LOST (simulated once)\n", frame);
            lost_once = 1;
            continue;
        }

        if (is_corrupted(frame, checksum)) {
            printf("Frame %d CORRUPTED → Sending NACK\n", frame);
            sprintf(sendbuf, "NACK %d", frame);
            sendto(sockfd, sendbuf, strlen(sendbuf)+1, 0,
                   (struct sockaddr *)&sender, len);
            continue;
        }

        if (frame >= base && frame < base + WINDOW_SIZE) {
            if (!received[frame]) {
                received[frame] = 1;
                printf("Received -> Frame %d (Buffered)\n", frame);
            }

            sprintf(sendbuf, "ACK %d", frame);
            sendto(sockfd, sendbuf, strlen(sendbuf)+1, 0,
                   (struct sockaddr *)&sender, len);
            printf("Sent <- ACK %d\n", frame);

            while (received[base]) {
                printf("Delivered -> Frame %d\n", base);
                base++;
            }
        }

        printf("---------------------------------\n");
    }

    close(sockfd);
    return 0;
}