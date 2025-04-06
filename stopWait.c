#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // for sleep()

#define TIMEOUT 3       // Timeout duration in seconds
#define TOTAL_PACKETS 5 // Number of packets to send

int simulate_acknowledgment()
{
    // Simulate a 70% chance of successful acknowledgment
    return rand() % 10 < 7;
}

int main()
{
    srand(time(0)); // Seed for random number generation

    int packet = 1;
    int ack_received;

    while (packet <= TOTAL_PACKETS)
    {
        printf("Sender: Sending packet %d...\n", packet);

        // Simulate waiting for acknowledgment
        sleep(1); // Simulate transmission delay
        ack_received = simulate_acknowledgment();

        if (ack_received)
        {
            printf("Receiver: ACK for packet %d received.\n\n", packet);
            packet++; // Move to the next packet
        }
        else
        {
            printf("Receiver: ACK for packet %d lost! Retransmitting...\n\n", packet);
            sleep(TIMEOUT); // Simulate timeout before retransmission
        }
    }

    printf("All packets sent successfully!\n");
    return 0;
}