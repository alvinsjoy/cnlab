#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 3003
#define BUFFER_SIZE 100

int main()
{
    char buffer[BUFFER_SIZE];
    int k; // Error Flag
    int sock_desc;
    struct sockaddr_in client;

    /* Creating Socket */
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1)
    {
        perror("Error In Socket Creation");
        return 1;
    }
    printf("Socket Created Successfully!\n");

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY; // For local testing. Replace with server IP as needed.
    client.sin_port = htons(PORT);

    /* Connecting Process */
    k = connect(sock_desc, (struct sockaddr *)&client, sizeof(client));
    if (k == -1)
    {
        perror("Error In Connecting To The Server");
        return 1;
    }
    printf("Connection Successful!\n");

    // Fork into two processes: one for receiving, one for sending
    if (fork() == 0)
    {
        // Child process: receive messages
        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            k = recv(sock_desc, buffer, BUFFER_SIZE, 0);
            if (k <= 0)
            {
                printf("Connection closed or error in receiving!\n");
                close(sock_desc);
                exit(0);
            }
            printf("Server: %s", buffer);
        }
    }
    else
    {
        // Parent process: send messages
        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            printf("Enter message: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            k = send(sock_desc, buffer, strlen(buffer), 0);
            if (k == -1)
            {
                perror("Error In Sending Data");
                close(sock_desc);
                exit(1);
            }
        }
    }

    // Cleanup (unreachable in this example)
    close(sock_desc);
    return 0;
}
