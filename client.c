/*-------------------------------------+
| POSIX/UNIX header files              |
+-------------------------------------*/
#include <fcntl.h>

#include <sys/ipc.h>

#include <sys/shm.h>

/*-------------------------------------+
| ISO/ANSI header files                |
+-------------------------------------*/
#include <stdlib.h>

#include <stdio.h>

#include <string.h>

/*-------------------------------------+
| Constants                            |
+-------------------------------------*/
/* memory segment character value     */
#define MEM_CHK_CHAR        '*'

/* shared memory key                  */
#define SHM_KEY      (key_t)1097

#define SHM_SIZE     (size_t)256

/* size of memory segment (bytes)     */
/* give everyone read/write           */
/* permission to shared memory        */
#define SHM_PERM  (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/*------------------------------------+
| Name:      main                     |
| Returns:   exit(EXIT_SUCCESS) or    |
|            exit(EXIT_FAILURE)       |
+------------------------------------*/
int main()
{
/* loop counter                       */
    int i;

/* shared memory segment id           */
    int shMemSegID;

/* shared memory flags                */
    int shmFlags;

/* ptr to shared memory segment       */
    char * shMemSeg;

/* generic char pointer               */
    char * cptr;

/*-------------------------------------*/
/* Get the shared memory segment for   */
/* SHM_KEY, which was set by           */
/* the shared memory server.           */
/*-------------------------------------*/
    shmFlags = SHM_PERM;

    if ( (shMemSegID = shmget(SHM_KEY, SHM_SIZE, shmFlags)) < 0 )
    {
        perror("CLIENT: shmget");
        exit(EXIT_FAILURE);
    }

/*-----------------------------------------*/
/* Attach the segment to the process's     */
/* data space at an address                */
/* selected by the system.                 */
/*-----------------------------------------*/
    shmFlags = 0;
    if ( (shMemSeg =
                  shmat(shMemSegID, NULL, shmFlags)) ==
         (void *) -1 )
    {
        perror("SERVER: shmat");
        exit(EXIT_FAILURE);
    }

/*-------------------------------------------*/
/* Read the memory segment and verify that   */
/* it contains the values                    */
/* MEM_CHK_CHAR and print them to the screen */
/*-------------------------------------------*/
    for (i=0, cptr = shMemSeg; i < SHM_SIZE;
         i++, cptr++)
    {
        if ( *cptr != MEM_CHK_CHAR )
        {
            fprintf(stderr, "CLIENT: Memory Segment corrupted!\n");
            exit(EXIT_FAILURE);
        }

        putchar( *cptr );
/* print 40 columns across            */

        if ( ((i+1) % 40) == 0 )
        {
            putchar('\n');
        }
    }
    putchar('\n');

/*--------------------------------------------*/
/* Clear shared memory segment.               */
/*--------------------------------------------*/
    memset(shMemSeg, '\0', SHM_SIZE);

/*------------------------------------------*/
/* Call shmdt() to detach shared            */
/* memory segment.                          */
/*------------------------------------------*/
    if ( shmdt(shMemSeg) < 0 )
    {
        perror("SERVER: shmdt");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}  /* end of main() */