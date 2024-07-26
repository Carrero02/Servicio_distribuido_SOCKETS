#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>    /* For signal handling */
#include <time.h>      /* For time() */

#include "claves/claves.h"


void end(int sig){
    // Signal handler for the SIGINT signal (Ctrl+C)

    printf("Exiting the client...\n");

    // Exit the program    
    exit(0);
}

int main(int argc, char *argv[])
{
    signal (SIGINT, end);
    srand(time(NULL));  // Seed for the random number generator based on the current time

    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];

    char value1_get[256];
    int N_value2_get;
    double V_value2_get[32];

    int error;

    // Infinite loop
    while (1)
    {
        // If the file "tuplas.txt" does not exist, run init()
        FILE *file = fopen("tuplas.txt", "r");
        if (file == NULL)
        {
            init();
        }
        else
        {
        fclose(file);
        }

        // Try to add a key
        // If it already exists, delete it and try again
        key = rand() % 1000;    // Random key between 0 and 999
        sprintf(value1, "value1_%d", key);
        N_value2 = rand() % 32 + 1; // Random number between 1 and 32
        for (int i = 0; i < N_value2; i++)
        {
            // Random integer part and random decimal part
            V_value2[i] = (double)rand() + (double)rand() / RAND_MAX;
        }

        error = set_value(key, value1, N_value2, V_value2);
        if (error == -2)    // Communication error
        {
            return -1;
        }
        while (error == -1) // Key already exists
        {
            if (delete_key(key) == -2)  // Communication error
            {
                return -1;
            }
            printf("Key %d deleted\n", key);
            error = set_value(key, value1, N_value2, V_value2);
            if (error == -2)    // Communication error
            {
                return -1;
            }
        }
        printf("Key %d added\n", key);
        // Sleep one second
        // sleep(1);

        // Try to get the recently added key
        // If it does not exist, try again
        error = get_value(key, value1_get, &N_value2_get, V_value2_get);
        if (error == -2)
        {
            return -1;
        }
        while (error == -1)
        {
            if (set_value(key, value1, N_value2, V_value2) == -2)
            {
                return -1;
            }
            printf("Key %d added\n", key);

            error = get_value(key, value1_get, &N_value2_get, V_value2_get);
            if (error == -2)
            {
                return -1;
            }
        }
        printf("Key %d obtained\n", key);
        // Sleep one second
        // sleep(1);

        // Try to modify the recently added key
        // If it does not exist, use set_value()
        sprintf(value1, "value1_%d", key);
        N_value2 = rand() % 32 + 1;
        for (int i = 0; i < N_value2; i++)
        {
            V_value2[i] = (double)rand() + (double)rand() / RAND_MAX;
        }

        error = modify_value(key, value1, N_value2, V_value2);
        if (error == -2)
        {
            return -1;
        }
        while (error == -1)
        {
            if (set_value(key, value1, N_value2, V_value2) == -2)
            {
                return -1;
            }
            printf("Key %d added\n", key);

            error = modify_value(key, value1, N_value2, V_value2);
            if (error == -2)
            {
                return -1;
            }
        }
        printf("Key %d modified\n", key);
        // Sleep one second
        // sleep(1);

        // Try to get the recently modified key
        // If it does not exist, try again
        error = get_value(key, value1_get, &N_value2_get, V_value2_get);
        if (error == -2)
        {
            return -1;
        }
        while (error == -1)
        {
            if (set_value(key, value1, N_value2, V_value2) == -2)
            {
                return -1;
            }
            printf("Key %d added\n", key);

            error = get_value(key, value1_get, &N_value2_get, V_value2_get);
            if (error == -2)
            {
                return -1;
            }
        }
        printf("Key %d obtained\n", key);
        // Sleep one second
        // sleep(1);
    }

    return 0;
}