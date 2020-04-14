#include "shared_mutex/shared_mutex.h"

int main(int argc, char * argv[]) {
    shared_mutex_t mutex = shared_mutex_init("mutex");
    if (mutex.ptr == NULL) {
        return 1;
    }
    shared_mutex_destroy(mutex);
    return 0;
}


/*
#include <stdio.h>˚
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <tclDecls.h>

#define async read

enum operating_mode {
    SERVER,
    CLIENT
};

int main(int argc, char * argv[]) {
    enum operating_mode op;
    char *shm_name;
    switch (argc) {
        case 1: {
            op = SERVER;
            shm_name = "default_chat";
            break;
        }
        case 2: {
            op = CLIENT;
            shm_name = argv[1];
            break;
        }
        default: {
            perror("Invalid arguments");
            exit(1);
        }
    }

    int fd = shm_open(shm_name, O_RDWR);

    // TODO Defer
    shm_unlink(shm_name);

    printf("Hello, World %s %d!\n", shm_name, fd);


    int shm_id = shmget(0, 1024, IPC_CREAT);

    int shm = shmctl(shm_id,)

    printf("Hello, World %s %d!\n", shm_id,);

    return 0;
}

async wait() {

}*/
