#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>

#define BUF_SIZE 1024
#define DEFAULT_SEG_NAME "chat"
#define EXIT_KEY 27

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define clean(shm_name) do { shm_unlink(shm_name); } while (0)
#define errExitWithCleaning(shm_name, msg) do { clean(shm_name); errExit(msg); } while (0)

enum operating_mode {
    SERVER,
    CLIENT
};

char exit_msg[1] = {EXIT_KEY};

struct shm_buf {
    sem_t  sem_reader;      /* POSIX unnamed semaphore */
    sem_t  sem_writer;      /* POSIX unnamed semaphore */
    size_t cnt;             /* Number of bytes used in 'buf' */
    char   buf[BUF_SIZE];   /* Data being transferred */
};
