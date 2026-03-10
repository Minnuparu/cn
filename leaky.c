#include <stdio.h>
#include <stdlib.h>

int main()
{
    int bucket_size, output_rate, n, i;
    int packet_size;
    int bucket = 0;

    printf("Enter bucket size: ");
    scanf("%d", &bucket_size);

    printf("Enter output rate: ");
    scanf("%d", &output_rate);

    printf("Enter number of packets: ");
    scanf("%d", &n);

    for(i = 1; i <= n; i++)
    {
        printf("\nEnter size of packet %d: ", i);
        scanf("%d", &packet_size);

        if(packet_size + bucket > bucket_size)
        {
            printf("Packet %d dropped (Overflow)\n", i);
        }
        else
        {
            bucket += packet_size;
            printf("Packet %d added, bucket = %d\n", i, bucket);
        }

        bucket -= output_rate;

        if(bucket < 0)
            bucket = 0;

        printf("After leaking, bucket = %d\n", bucket);
    }

    return 0;
}