#include "semun.h" /* Definition of semun union */
#include "svshm_xfr.h"
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    if ( getuid() != 0 ) {
        printf("Execution in root mode\n") ;
        exit(1) ;
    } // if 
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;
    /* Get IDs for semaphore set and shared memory created by writer */
    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) perror("semget");

    shmid = shm_open(SHM_KEY, O_RDWR , 0777 ) ; // 開啟writer建立的shared memory

    if (shmid == -1) perror("shmget");
    shmp = mmap( NULL, sizeof(struct shmseg), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0 ) ; // 輸出sharing memory的位址
    if (shmp == (void *) -1) perror("shmat");

    /* Transfer blocks of data from shared memory to stdout */
    for (xfrs = 0, bytes = 0; ; xfrs++) {
    
        if (reserveSem(semid, READ_SEM) == -1) /* Wait for our turn */
            perror("reserveSem");

        if (shmp->cnt == 0) /* Writer encountered EOF */
            break;
        
        bytes += shmp->cnt;
        if ( write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt ) perror("partial/failed write");
        if (releaseSem(semid, WRITE_SEM) == -1) /* Give writer a turn */
            perror("releaseSem");
    } // for

    if ( munmap( shmp, sizeof(struct shmseg) ) == -1) perror("shmdt");
 /* Give writer one more turn, so it can clean up */
    if (releaseSem(semid, WRITE_SEM) == -1)
        perror("releaseSem");
    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
} // main