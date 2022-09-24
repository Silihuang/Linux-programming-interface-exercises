#define _GNU_SOURCE /* Get strsignal() declaration from <string.h> */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "hw8.h"

int main(int argc, char *argv[]) {
    // FIFO: 沒read, write會被block, 沒write, read會被block

    int serverId ; 
    struct requestMsg req ;
    struct responseMsg resp;
    int seqNum = 0; /* This is our "service" */

    serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | 0666 );
    if (serverId == -1) {  // 可能有存在的Message Queue
        system("ipcrm -Q 0x1234567") ;
        serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | 0666 );
    } // if 

    for ( ;; ) { /* Read requests and send responses */
        ssize_t msgLen = msgrcv( serverId, &req, REQ_MSG_SIZE, 0, 0 ) ; 
        if (msgLen == -1) {
            if (errno == EINTR) /* Interrupted by SIGCHLD handler? */
            continue; /* ... then restart msgrcv() */
            perror("msgrcv"); /* Some other error */
            break; /* ... so terminate loop */
        } // if 

		resp.mtype = req.clientId ;
		resp.seqNum = seqNum ;
        msgsnd( serverId, &resp, RESP_MSG_SIZE, 0 ) ; /* Zero-length mtext */   

        seqNum += req.seqlen; /* Update our sequence number */
    } // for


    exit(EXIT_SUCCESS);
}