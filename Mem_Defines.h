/*******************************************************************
 * @authors David Whitters and Jonah Bukowsky
 * @date 2/8/2018
 *
 * CIS 452
 * Dr. Dulimarta
 *
 * @description This file contains the common defines used for access
 *              and use of the shared memory.
 ******************************************************************/

/** The size in bytes of the shared memory section to be created. */
#define SHM_SIZE 4096
/** The offset of the string in the shared memory section. */
#define STR_OFFSET 2
/** The index of the data read flag. */
#define DATA_FLAG_IDX 0
/** The index in shared memory that contains the number of readers. */
#define NUM_READER_IDX 1
/** Flag value indicating the shared memory has been read by all readers. */
#define DATA_READ 0
/** The project key used for accessing the shared memory. */
#define PROJECT_KEY 'a'
