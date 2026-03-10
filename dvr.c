#include <stdio.h>

#define INF 999

int main() {
    int n, i, j, k;
    int cost[20][20];
    int dist[20][20];
    int via[20][20];
    int updated = 1;

    printf("Enter number of routers: ");
    scanf("%d", &n);

    printf("Enter the cost matrix (use 999 for no direct link):\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            scanf("%d", &cost[i][j]);
            dist[i][j] = cost[i][j];
            via[i][j] = j;
        }
    }

   
    while (updated) {
        updated = 0;

        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                for (k = 0; k < n; k++) {
                    if (dist[i][j] > cost[i][k] + dist[k][j]) {
                        dist[i][j] = cost[i][k] + dist[k][j];
                        via[i][j] = k;
                        updated = 1;
                    }
                }
            }
        }
    }

    
    for (i = 0; i < n; i++) {
        printf("\nRouting table for Router %d\n", i);
        printf("Destination\tNext Hop\tCost\n");

        for (j = 0; j < n; j++) {
            printf("%d\t\t%d\t\t%d\n", j, via[i][j], dist[i][j]);
        }
    }

    return 0;
}