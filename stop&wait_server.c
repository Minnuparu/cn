#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct packet {
    char data[1024];
} Packet;

typedef struct frame {
    int frame_kind;  
    int sq_no;
    int ack;
    Packet packet;
} Frame;

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <port> <loss_percentage>\n", argv[0]);
        printf("Example: %s 8080 30 (30%% frame loss)\n", argv[0]);
        exit(0);
    }
    
    int port = atoi(argv[1]);
    int loss_percentage = atoi(argv[2]);  
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    int expected_frame = 0;
    Frame frame_recv;
    Frame frame_send;
    
    
    srand(time(NULL));
    
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
   
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    
    printf("Receiver listening on port %d...\n", port);
    printf("Simulating %d%% frame loss\n", loss_percentage);
    
    addr_size = sizeof(clientAddr);
    
    while (1) {
       
        int f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(Frame), 0, 
                                   (struct sockaddr*)&clientAddr, &addr_size);
        
        if (f_recv_size > 0 && frame_recv.frame_kind == 1) {
            // Simulate frame loss
            int random_val = rand() % 100;
            if (random_val < loss_percentage) {
                printf("[SIMULATION] Frame %d DROPPED (random=%d < %d%%)\n", 
                       frame_recv.sq_no, random_val, loss_percentage);
                continue;  
            }
            
            if (frame_recv.sq_no == expected_frame) {
              
                printf("[+] Frame %d Received: %s\n", frame_recv.sq_no, frame_recv.packet.data);
                
                
                random_val = rand() % 100;
                if (random_val < loss_percentage) {
                    printf("[SIMULATION] ACK %d DROPPED (random=%d < %d%%)\n", 
                           expected_frame + 1, random_val, loss_percentage);
                    expected_frame++;  
                    continue;
                }
                
                
                frame_send.sq_no = 0;
                frame_send.frame_kind = 0;
                frame_send.ack = frame_recv.sq_no + 1;
                
                sendto(sockfd, &frame_send, sizeof(frame_send), 0, 
                      (struct sockaddr*)&clientAddr, addr_size);
                printf("[+] ACK %d Sent\n", frame_send.ack);
                
               
                expected_frame++;
            } else {
                
                printf("[-] Unexpected frame %d (expected %d), sending duplicate ACK\n", 
                       frame_recv.sq_no, expected_frame);
                
               
                frame_send.sq_no = 0;
                frame_send.frame_kind = 0;
                frame_send.ack = expected_frame;
                
                sendto(sockfd, &frame_send, sizeof(frame_send), 0, 
                      (struct sockaddr*)&clientAddr, addr_size);
            }
        }
    }
    
    close(sockfd);
    return 0;
}