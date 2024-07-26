#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funciones_servidor.h"


int init()
{
    // Try opening the FILE_NAME file
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        // If the file does not exist, create it
        file = fopen(FILE_NAME, "w");
        if (file == NULL)
        {
            perror("Error creating the file\n");
            return -1;
        }
    }
    else
    {
        // If the file exists, delete it and create a new one
        if (remove(FILE_NAME) != 0)
        {
            perror("Error deleting the file\n");
            return -1;
        }
        file = fopen(FILE_NAME, "w");
        if (file == NULL)
        {
            perror("Error creating the file\n");
            return -1;
        }
        // Close the file
        fclose(file);
    }
    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2)
{

    // Check that N_value2 is between 1 and 32
    if (N_value2 < 1 || N_value2 > 32)
    {
        perror("N_value2 must be between 1 and 32\n");
        return -1;
    }

    // Check if the key already exists with the exist function (exist returns 0)
    // We also check if the file exists with the exist function (exist returns -1)
    int exist_value = exist(key);
    if (exist_value == -1)
    {
        return -1;
    }
    else if (exist_value == 1)
    {
        perror("The key already exists\n");
        return -1;
    }

    // If the key does not exist, add it at the end of the file
    FILE *file = fopen(FILE_NAME, "a");
    if (file == NULL)
    {
        perror("Error opening the file\n");
        return -1;
    }

    // Write the key, value1, N_value2 to the file
    if (fprintf(file, "%d %s %d", key, value1, N_value2) < 0)
    {
        perror("Error writing to the file\n");
        fclose(file);
        return -1;
    }
    // Write the values of the vector V_value2 to the file
    for (int i = 0; i < N_value2; i++)
    {   
        if (fprintf(file, " %lf", V_value2[i]) < 0)
        {
            perror("Error writing to the file\n");
            fclose(file);
            return -1;
        }
    }

    // Write a new line to the file
    if (fprintf(file, "\n") < 0)
    {
        perror("Error writing to the file\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2)
{   
    // Try opening the FILE_NAME file
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        perror("Error opening the file\n");
        return -1;
    }

    int key_file;
    // The doubles vector in characters (the line, except the key, value1 and N_value2)
    // 32 * 24 characters for the 32 doubles of V_value2 (each double can be represented with a max of 24 characters) +
    // 31 spaces (one for each double) + one new line
    // In total, 32 * 24 + 31 = 799 characters
    char doubles_vector[800];
    char value1_file[256];
    int N_value2_file;
    double V_value2_file[32];

    // Read the file line by line
    while (fscanf(file, "%d %s %d %[^\n]\n", &key_file, value1_file, &N_value2_file, doubles_vector) != EOF)
    {
        // If the key is found, copy the values to the output variables
        if (key_file == key)
        {
            strcpy(value1, value1_file);
            *N_value2 = N_value2_file;
            // add an extra space at the end of the doubles_vector
            strcat(doubles_vector, " ");
            char *ptr = doubles_vector;
            for (int i = 0; i < N_value2_file; i++)
            { 
                sscanf(ptr, "%lf ", &V_value2_file[i]);
                // Advance the pointer to the next double
                ptr += strcspn(ptr, " ") + 1;
            }
            // Copy the values to the output variables
            for (int i = 0; i < N_value2_file; i++)
            {
                V_value2[i] = V_value2_file[i];
            }
            fclose(file);
            return 0;
        }
    }

    // If the key is not found, return -1
    fclose(file);
    return -1;
}
            

int modify_value(int key, char *value1, int N_value2, double *V_value2)
{
    // Check that N_value2 is between 1 and 32
    if (N_value2 < 1 || N_value2 > 32)
    {
        perror("N_value2 must be between 1 and 32\n");
        return -1;
    }

    if (delete_key(key) < 0)
    {
        return -1;
    }

    if (set_value(key, value1, N_value2, V_value2) < 0)
    {
        return -1;
    }

    return 0;
}


int delete_key(int key)
{
    // Check if the key exists with the exist 
    if (exist(key) <= 0)
    {
        return -1;
    }

    // Try opening the FILE_NAME file
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        perror("Error opening the file\n");
        return -1;
    }

    int key_file;
    char doubles_vector[799];
    char value1_file[256];
    int N_value2_file;
    double V_value2_file[32];

    // Create a temporary file
    FILE *temp_file = fopen("temp_file.txt", "w");
    if (temp_file == NULL)
    {
        perror("Error creating the temporary file\n");
        return -1;
    }

    // Read the file line by line
    while (fscanf(file, "%d %s %d %[^\n]\n", &key_file, value1_file, &N_value2_file, doubles_vector) != EOF)
    {
        // If the key is found, do not write it to the temporary file
        // (We copy all the lines except the one with the key to delete to the temporary file)
        if (key_file != key)
        {
            if (fprintf(temp_file, "%d %s %d %s\n", key_file, value1_file, N_value2_file, doubles_vector) < 0)
            {
                perror("Error writing to the temporary file\n");
                fclose(file);
                fclose(temp_file);
                return -1;
            }
        }
    }

    // Close the files
    fclose(file);
    fclose(temp_file);

    // Delete the original file
    if (remove(FILE_NAME) != 0)
    {
        perror("Error deleting the file\n");
        return -1;
    }

    // Rename the temporary file to the original file
    if (rename("temp_file.txt", FILE_NAME) != 0)
    {
        perror("Error renaming the file\n");
        return -1;
    }

    return 0;
}


int exist(int key)
{
    // Try opening the FILE_NAME file
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        perror("Error opening the file\n");
        return -1;
    }

    int key_file;
    char doubles_vector[799];
    char value1_file[256];
    int N_value2_file;
    double V_value2_file[32];

    // Read the file line by line
    while (fscanf(file, "%d %s %d %[^\n]\n", &key_file, value1_file, &N_value2_file, doubles_vector) != EOF)
    {
        // If the key is found, return 1
        if (key_file == key)
        {
            fclose(file);
            return 1;
        }
    }

    // If the key is not found, return 0
    fclose(file);
    return 0;
}