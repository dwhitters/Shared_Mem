/*******************************************************************
 * @authors David Whitters and Jonah Bukowsky
 * @date 2/8/2018
 *
 * CIS 452
 * Dr. Dulimarta
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>

#include "Mem_Defines.h"

#define DATA_NOT_READ 1

int main(void)
{
    int shm_id;
    key_t shm_key;
    char * shm_ptr;
    int reader_num; /* Set to the number of readers when it's added. */

    /* Get the key for shared memory access from the pathname and project id. */
    if((shm_key = ftok(".", PROJECT_KEY)) == -1)
    {
        perror("Key generation failure.");
        exit(EXIT_FAILURE);
    }
    /* Get the shared memory segment created by the writer. */
    if ((shm_id = shmget(shm_key, SHM_SIZE,0)) < 0)
    {
        perror("i can't get no..\n");
        exit(EXIT_FAILURE);
    }
    /* Attach this process to the shared memory. */
    if ((shm_ptr = shmat(shm_id, 0, 0)) == (void *) -1)
    {
        perror("can't attach\n");
        exit(EXIT_FAILURE);
    }

    /* Set the reader number and add the reader to the writer process. */
    reader_num = shm_ptr[NUM_READER_IDX] + 1;
    shm_ptr[NUM_READER_IDX] = reader_num;

    bool is_running = true;

    /* Loop until detaching. */
    while(is_running)
    {
        /* Wait for new data. */
        while(shm_ptr[DATA_FLAG_IDX] != DATA_READ)
        {
            /* Only reads the data when the flag matches the reader number. */
            if(abs(shm_ptr[DATA_FLAG_IDX]) == reader_num)
            {
                if(shm_ptr[DATA_FLAG_IDX] < 0)
                {
                    printf("Detaching!\n");
                    is_running = false;
                    shm_ptr[DATA_FLAG_IDX] += 1;
                    break;
                }
                else
                {
                    printf("%s", shm_ptr + STR_OFFSET); /* Print the new data. */
                    shm_ptr[DATA_FLAG_IDX] -= 1; /* Reset the flag. */
                }
            }
        }
    }

    /* Detach from the shared memory. */
    if(shmdt(shm_ptr) < 0)
    {
        perror("just can't let go\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
