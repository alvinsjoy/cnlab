#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_SIZE 4       // Sliding window size
#define TOTAL_FRAMES 10     // Total number of frames to send
#define LOSS_PROBABILITY 20 // Percentage probability of frame loss (0-100)

// Simulate frame transmission
int send_frame(int frame_number)
{
    printf("Sending frame %d...\n", frame_number);
    sleep(1);                      // Simulate delay
    int rand_value = rand() % 100; // Generate random number between 0 and 99
    if (rand_value < LOSS_PROBABILITY)
    {
        printf("Frame %d lost during transmission!\n", frame_number);
        return 0;
    }
    printf("Frame %d sent successfully.\n", frame_number);
    return 1;
}

// Simulate receiving acknowledgment
int receive_ack(int frame_number)
{
    printf("Receiving acknowledgment for frame %d...\n", frame_number);
    sleep(1); // Simulate delay
    int rand_value = rand() % 100;
    if (rand_value < LOSS_PROBABILITY)
    {
        printf("Acknowledgment for frame %d lost!\n", frame_number);
        return 0;
    }
    printf("Acknowledgment for frame %d received.\n", frame_number);
    return 1;
}

// Selective Repeat ARQ Protocol
void selective_repeat_arq()
{
    int sent_frames[TOTAL_FRAMES] = {0};  // Track sent frames
    int ack_received[TOTAL_FRAMES] = {0}; // Track acknowledgments received
    int base = 0;                         // Start of sliding window

    while (base < TOTAL_FRAMES)
    {
        // Send frames within the window
        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_FRAMES; i++)
        {
            if (!sent_frames[i])
            {
                sent_frames[i] = send_frame(i); // Send frame if not sent
            }
        }

        // Check for acknowledgments
        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_FRAMES; i++)
        {
            if (sent_frames[i] && !ack_received[i])
            {
                ack_received[i] = receive_ack(i); // Mark acknowledgment if received
            }
        }

        // Slide the window if the base frame is acknowledged
        while (base < TOTAL_FRAMES && ack_received[base])
        {
            printf("Sliding window forward. Frame %d fully acknowledged.\n", base);
            base++;
        }
    }
    printf("All frames sent and acknowledged successfully.\n");
}

// Main function
int main()
{
    srand(time(0)); // Seed random number generator
    selective_repeat_arq();
    return 0;
}
