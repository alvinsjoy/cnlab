#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define PORT 8080
#define BUFSIZE 1024

// Structure to package client request details for the thread
typedef struct
{
    int sockfd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    char message[BUFSIZE];
} thread_arg;

// Thread function to process a client request
void *handle_request(void *arg)
{
    thread_arg *t = (thread_arg *)arg;
    printf("Handling request from %s:%d\n",
           inet_ntoa(t->cliaddr.sin_addr),
           ntohs(t->cliaddr.sin_port));

    // Retrieve the current system time
    time_t now = time(NULL);
    char *time_str = ctime(&now); // ctime returns a string with newline at the end

    // Send the time string back to the client
    if (sendto(t->sockfd, time_str, strlen(time_str), 0,
               (struct sockaddr *)&(t->cliaddr), t->len) < 0)
    {
        perror("sendto failed");
    }

    free(t);
    pthread_exit(NULL);
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFSIZE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Time Server is running on port %d\n", PORT);

    while (1)
    {
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0)
        {
            perror("recvfrom error");
            continue;
        }
        buffer[n] = '\0';
        printf("Received request: \"%s\" from %s:%d\n",
               buffer, inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

        // Allocate and set up the thread argument structure
        thread_arg *targ = malloc(sizeof(thread_arg));
        if (!targ)
        {
            perror("Memory allocation failed");
            continue;
        }
        targ->sockfd = sockfd;
        targ->cliaddr = cliaddr;
        targ->len = len;
        strncpy(targ->message, buffer, BUFSIZE);

        // Create a new thread to handle the request concurrently
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_request, targ) != 0)
        {
            perror("pthread_create failed");
            free(targ);
        }
        else
        {
            // Detach the thread so its resources are released on completion
            pthread_detach(thread_id);
        }
    }

    close(sockfd);
    return 0;
}
