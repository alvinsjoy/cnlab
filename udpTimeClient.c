#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFSIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFSIZE];
    char *request = "TIME"; // Request string (can be any token)

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change to server IP if remote

    // Send time request to the server
    if (sendto(sockfd, request, strlen(request), 0,
               (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Time request sent to server.\n");

    // Wait for the server's response
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *)&servaddr, &len);
    if (n < 0)
    {
        perror("recvfrom failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Server Time: %s\n", buffer);

    close(sockfd);
    return 0;
}
