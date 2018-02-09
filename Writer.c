/*******************************************************************
 * @authors David Whitters and Jonah Bukowsky
 * @date 2/8/2018
 *
 * CIS 452
 * Dr. Dulimarta
 *
 * @description The main interface to the shared memory. Takes user
 *              input and routes it into shared memory to be accessed
 *              by reader programs.
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "Mem_Defines.h"

int main (void)
{
    int shm_id;
    struct shmid_ds buff;
    key_t shm_key;
    char *shm_ptr;
    bool quit_not_entered = true;

    /* Generate a key for shared memory access from the pathname and project id. */
    if((shm_key = ftok(".", PROJECT_KEY)) == -1)
    {
        perror("Key generation failure.");
        exit(EXIT_FAILURE);
    }

    /* Create a new shared memory segment for the writer and readers. */
    if ((shm_id =
         shmget (shm_key, SHM_SIZE,
                 IPC_CREAT | 0666)) < 0) {
        perror("i can't get no..\n");
        exit(EXIT_FAILURE);
    }
    /* Attach this process to the shared memory. */
    if ((shm_ptr = shmat(shm_id, 0, 0)) == (void *) -1) {
        perror("can't attach\n");
        exit(EXIT_FAILURE);
    }

    while(quit_not_entered)
    {
        /* Prompt and receive user input. */
        printf("> ");
        if(fgets(shm_ptr + STR_OFFSET, SHM_SIZE - STR_OFFSET, stdin) == NULL)
        {
            perror("fgets failure");
            exit(EXIT_FAILURE);
        }

        /* Exit if quit is entered. */
        if(strncmp("quit", shm_ptr + STR_OFFSET, 4) == 0)
        {
            quit_not_entered = false;
            continue;
        }

        /* Set the value of the data flag to the number of readers. */
        shm_ptr[DATA_FLAG_IDX] = shm_ptr[NUM_READER_IDX];

        /* Wait until all readers have read the data. */
        while(shm_ptr[DATA_FLAG_IDX] != DATA_READ);
    }

    /* Send negative number to signal the readers must detach. */
    shm_ptr[DATA_FLAG_IDX] = (0 - shm_ptr[NUM_READER_IDX]);

    /* Wait until all readers have detached. */
    bool waiting = true;
    while(waiting)
    {
        if(shmctl(shm_id, IPC_STAT, &buff) != 0) /* Get the shared memory meta-data. */
        {
            perror("Shared memory control.");
            exit(EXIT_FAILURE);
        }

        waiting = (buff.shm_nattch != 1); /* Wait until this process is the only one attached. */
    }

    /* Detach from the shared memory. */
    if(shmdt(shm_ptr) < 0)
    {
        perror("just can't let go\n");
        exit(EXIT_FAILURE);
    }
    /* Deallocate the shared memory. */
    if(shmctl(shm_id, IPC_RMID, 0) < 0)
    {
        perror("can't deallocate\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
