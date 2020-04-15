#include "main.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

enum operating_mode op;

void* reader_thread(void* arg) {
    struct shm_buf* shm_ptr = (struct shm_buf*)arg;
    // Initialize buf for messages
    char* msg = NULL;

    // Start data transfer
    while (1) {
        // Check reader semaphore
        if (op == SERVER) {
            sem_wait(&shm_ptr->sem_reader);
        } else {
            sem_wait(&shm_ptr->sem_writer);
        }
        // Read data from seg
        msg = malloc(shm_ptr->cnt);
        memmove(msg, shm_ptr->buf, shm_ptr->cnt);
        if (strcmp(msg, exit_msg) == 0) {
            printf("Chat closed");
            return 0;
        }
        printf("New msg %.*s\n", (int)(shm_ptr->cnt), msg);
        // Clean resources
        free(msg);
        memset(shm_ptr->buf, 0, shm_ptr->cnt);
        shm_ptr->cnt = 0;
    }
}

void* writer_thread(void* arg)
{
    struct shm_buf* shm_ptr = (struct shm_buf*)arg;
    char msg_buf [BUF_SIZE + 1];
    uint is_done = 0;

    // Start data transfer
    while (!is_done) {
        char tmp_char;
        int cnt = 0;
        do {
            tmp_char = (char)getchar();
            msg_buf[cnt++] = tmp_char;
        } while (tmp_char != '\n' && tmp_char != EOF);
        cnt--;
        // Handle stdin data
        switch (cnt) {
            case 0: { // User input nothing
                break;
            }
            case 1: { // User input 1 symbol or EXIT_KEY
                if (msg_buf[0] == EXIT_KEY) {
                    memcpy(shm_ptr->buf, exit_msg, cnt);
                    shm_ptr->cnt = 1;
                    is_done = 1;
                }
            }
            case BUF_SIZE + 1: {
                if (cnt > BUF_SIZE) {
                    printf("Invalid msg size");
                    break;
                }
            }
            default: { // User input something
                memcpy(shm_ptr->buf, msg_buf, cnt);
                shm_ptr->cnt = cnt;
            }
        }
        memset(msg_buf, 0, cnt + 1);
        if (op == SERVER) {
            sem_post(&shm_ptr->sem_writer);
        } else {
            sem_post(&shm_ptr->sem_reader);
        }
        // Iteration timeout
        sleep(1);
    }
}

int main(int argc, char * argv[]) {
    // Parse arguments
    char* shm_name;

    if (argc != 3) {
        printf("Invalid arguments count");
        exit(1);
    }

    if (strcmp(argv[1], "server") == 0) {
        op = SERVER;
        shm_name = DEFAULT_SEG_NAME;
    } else if (strcmp(argv[1], "client") == 0) {
        op = CLIENT;
    } else {
        printf("Invalid arguments");
        exit(1);
    }

    shm_name = argv[2];

    // Init memory segment
    int fd;
    uint struct_size = sizeof(struct shm_buf);
    if (op == SERVER) {
        // Create memory segment
        fd = shm_open(shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            errExit("shm_open");
        }

        // Truncate segment to struct size
        if (ftruncate(fd, struct_size) == -1) {
            errExit("ftruncate");
        }
    } else {
        // Open memory segment
        fd = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            errExit("shm_open");
        }
    }
    // Map segment
    struct shm_buf* shm_ptr = mmap(
            NULL,
            struct_size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            fd,
            0
    );
    if (shm_ptr == MAP_FAILED) {
        errExit("mmap");
    }

    // Initialize semaphores
    if (op == SERVER) {
        if (sem_init(&shm_ptr->sem_reader, 1, 0) == -1) {
            errExitWithCleaning(shm_name, "sem_init-sem_reader");
        }

        if (sem_init(&shm_ptr->sem_writer, 1, 0) == -1) {
            errExitWithCleaning(shm_name, "sem_init-sem_writer");
        }
    }

    // Create worker threads
    pthread_t reader_t, writer_t;
    pthread_create(&reader_t, NULL, reader_thread, shm_ptr);
    pthread_create(&writer_t, NULL, writer_thread, shm_ptr);

    // Wait EXIT_KEY
    pthread_join(writer_t, NULL);

    pthread_cancel(reader_t);
    pthread_join(reader_t, NULL);

    // Clean resources
    clean(shm_name);

    return 0;
}