#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct packet {
    char data[1024];
} Packet;

typedef struct frame {
    int frame_kind;  
    int sq_no;
    int ack;
    Packet packet;
} Frame;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }
    
    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    socklen_t addr_size;
    int frame_id = 0;
    Frame frame_send;
    Frame frame_recv;
    struct timeval tv;
    
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    printf("Sender ready. Type 'quit' to exit.\n");
    
    while (1) {
       
        printf("\nEnter Data: ");
        if (scanf("%s", buffer) != 1) {
            break;
        }
        
        
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
        
       
        frame_send.sq_no = frame_id;
        frame_send.frame_kind = 1;
        frame_send.ack = 0;
        strcpy(frame_send.packet.data, buffer);
        
        int ack_received = 0;
        int retries = 0;
        int max_retries = 5;
        
       
        while (!ack_received && retries < max_retries) {
            
            sendto(sockfd, &frame_send, sizeof(Frame), 0, 
                  (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            printf("[+] Frame %d Sent (attempt %d)\n", frame_id, retries + 1);
            
            
            addr_size = sizeof(serverAddr);
            int f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(frame_recv), 0, 
                                      (struct sockaddr*)&serverAddr, &addr_size);
            
            if (f_recv_size > 0 && frame_recv.frame_kind == 0 && 
                frame_recv.ack == frame_id + 1) {
                printf("[+] ACK %d Received\n", frame_recv.ack);
                ack_received = 1;
                frame_id++; 
            } else if (f_recv_size > 0) {
                printf("[-] Wrong ACK received (expected %d, got %d)\n", 
                       frame_id + 1, frame_recv.ack);
                retries++;
            } else {
                printf("[-] Timeout! Retransmitting...\n");
                retries++;
            }
        }
        
        if (!ack_received) {
            printf("[-] Max retries reached. Frame %d not acknowledged.\n", frame_id);
            break;
        }
    }
    
    close(sockfd);
    printf("Sender terminated.\n");
    return 0;
}