#include "semun.h" /* Definition of semun union */
#include "svshm_xfr.h"
#include <sys/mman.h>
#include <fcntl.h>
int main(int argc, char *argv[]) {

    if ( getuid() != 0 ) {
        printf("Execution in root mode\n") ;
        exit(1) ;
    } // if 

    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;
    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS); // 建立訊息號集合, 數量:2, semid = 信號指標符  
    if (semid == -1) perror("semget") ;
    if (initSemAvailable(semid, WRITE_SEM) == -1) perror("initSemAvailable") ; // semid設定為第一個訊號
    if (initSemInUse(semid, READ_SEM) == -1) perror("initSemInUse");

    shmid = shm_open( SHM_KEY, O_CREAT | O_RDWR, 0777 ) ; // 建立shared memory 
    if (shmid == -1) {
        perror("shm_open") ;
        exit(1) ;
    } // if 

    // 避免bus error
    write( shmid, " ", sizeof( struct shmseg ) ) ;

    // 第一個參數是sharing memory的起始位址, 填NULL代表讓OS去分配位址
    shmp = mmap( NULL, sizeof(struct shmseg) , PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0 ) ; // 輸出sharing memory的位址
    if ( shmp == MAP_FAILED ) {
        perror("mmap") ;
        exit(1) ;
    } // if 

 /* Transfer blocks of data from stdin to shared memory */
    for ( xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt ) {

        /* Wait for our turn */
        if (reserveSem(semid, WRITE_SEM) == -1) perror("reserveSem");

        shmp->cnt = read( STDIN_FILENO, shmp->buf, BUF_SIZE );
        if (shmp->cnt == -1) {
            perror("read");
            exit(1) ;
        } // if 
        /* Give reader a turn */
        if (releaseSem(semid, READ_SEM) == -1) perror("releaseSem");

        /* Have we reached EOF? We test this after giving the reader
        a turn so that it can see the 0 value in shmp->cnt. */
        if (shmp->cnt == 0) break ;
    
    } // for

    /* Wait until reader has let us have one more turn. We then know
    reader has finished, and so we can delete the IPC objects. */
    if ( reserveSem(semid, WRITE_SEM) == -1 ) {
        perror( "reserveSem" ) ;
        exit(1) ;
    } // if 
    if ( semctl(semid, 0, IPC_RMID, dummy) == -1) {
        perror( "semctl" ) ;
        exit(1) ;
    } // if 

    if ( munmap( shmp, sizeof(struct shmseg) ) == -1) {
        perror("shmdt") ; // 刪除shared memory
        exit(1) ;
    } // if 
    if ( shm_unlink(SHM_KEY) == -1) {
        perror("shmctl"); // 刪除訊號
        exit(1) ;
    } // if
    
    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
} //main