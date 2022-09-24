#include "hw8.h"


static int clientId;

static void removeQueue(void) {
    if (msgctl(clientId, IPC_RMID, NULL) == -1)
    perror("msgctl");
} // removeQueue


int main(int argc, char *argv[]) {

    int serverId, numMsgs;
    struct requestMsg req;
    struct responseMsg resp;
    ssize_t msgLen, totBytes;

    if ( argc != 2 || strcmp(argv[1], "--help") == 0 ) printf("./ Q_clinet, seqlen\n" ) ;

    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) perror("msgget - server message queue");
    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientId == -1) perror("msgget - client message queue");
    // 設定結束程式時會呼叫removeQueue() 
    if ( atexit(removeQueue) != 0) perror("atexit");

    /* Send message asking for file named in argv[1] */
    req.mtype = 1 ; /* Any type will do */
    req.clientId = clientId ;
    req.seqlen = atoi(argv[1]) ;

    /* Ensure string is terminated */
    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1) perror("msgsnd");

    /* File was opened successfully by server; process messages
    (including the one already received) containing file data */
    msgLen = msgrcv(serverId, &resp, RESP_MSG_SIZE, 0, 0);
    if (msgLen == -1) perror("msgrcv") ;
    printf("%d\n", resp.seqNum ) ;
    return 0 ;
} // main