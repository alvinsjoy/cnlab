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
    int k; /* Error Flag */
    socklen_t len;
    int sock_desc, temp_sock_desc;
    struct sockaddr_in server, client;

    /* Creating Socket */
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1)
    {
        perror("Error In Socket Creation");
        return 1;
    }
    printf("Socket Created Successfully!\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    /* Binding Process */
    k = bind(sock_desc, (struct sockaddr *)&server, sizeof(server));
    if (k == -1)
    {
        perror("Error In Binding");
        return 1;
    }
    printf("Socket Bound Successfully!\n");

    /* Listening Process */
    k = listen(sock_desc, 5);
    if (k == -1)
    {
        perror("Error In Listening");
        return 1;
    }
    printf("Waiting for a connection on port %d...\n", PORT);

    len = sizeof(client);
    /* Accepting Process */
    temp_sock_desc = accept(sock_desc, (struct sockaddr *)&client, &len);
    if (temp_sock_desc == -1)
    {
        perror("Error In Accepting Connection");
        return 1;
    }
    printf("Connection Accepted!\n");

    // Fork into two processes: one for receiving, one for sending
    if (fork() == 0)
    {
        // Child process: receive messages
        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            k = recv(temp_sock_desc, buffer, BUFFER_SIZE, 0);
            if (k <= 0)
            {
                printf("Connection closed or error in receiving!\n");
                close(temp_sock_desc);
                exit(0);
            }
            printf("Client: %s", buffer);
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
            k = send(temp_sock_desc, buffer, strlen(buffer), 0);
            if (k == -1)
            {
                perror("Error In Sending Data");
                close(temp_sock_desc);
                exit(1);
            }
        }
    }

    // Cleanup (unreachable in this example)
    close(sock_desc);
    return 0;
}
