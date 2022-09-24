#define _GNU_SOURCE /* Get strsignal() declaration from <string.h> */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "tlpi_hdr.h"

typedef void (*sighandler_t)(int) ; // sighandler_t類型的函式，返回void類別，參數為int

void handler( int signo ) { // 新的disposition
    printf( "\nChanged disposition of SIGSET.\n" );
} // handler

// The sighold() function adds sig to the calling process's signal mask.
int sighold( int signo ) {
    errno = 0 ;
    sigset_t block;
    sigemptyset( &block ) ;
    sigaddset( &block, signo );
    if ( sigprocmask( SIG_BLOCK, &block, NULL ) == -1 ) {
        printf("Error(sighold)： %s\n",strerror( errno ) );
        return -1 ;
    } // if 
    return 0 ;
} // sighold

// The sigrelse() function removes sig from the calling process's signal mask.
int sigrelse( int signo ) {
    errno = 0 ;
    sigset_t block;
    sigemptyset( &block ) ;
    sigaddset( &block, signo );
    if ( sigprocmask( SIG_UNBLOCK, &block, NULL ) == -1 ) {
        printf("Error(sigrelse)： %s\n",strerror( errno ) );
        return -1 ;
    } // if 
    return 0;
} // sigrelse

// The sigignore() function sets the disposition of sig to SIG_IGN.
int sigignore( int signo ) { 
    // 這邊設定SIGINT的disposition, SIGINT無法正常執行
    errno = 0 ;
    struct sigaction sa;
    sa.sa_handler = SIG_IGN ;
    if( sigaction( signo, &sa, NULL ) == -1 ) {
        printf("Error(sigignore)： %s\n",strerror( errno ) );
        return -1 ;
    } // if 

    return 0;
} // sigignore

// 從mask移除signal, 且暫停process, 直到有訊號進來
int sigpause( int sig ) {
  errno = 0 ;
  sigset_t block ;
  if ( sigprocmask( SIG_SETMASK, NULL, &block ) == -1 ) {
    printf("Error(sigpause)： %s\n",strerror( errno ) ) ;
    return -1 ;
  } // if 

  sigdelset( &block, sig ) ;
  sigsuspend( &block ) ; 
  return 0 ;

} // sigpause

/*
The sigset() function modifies the disposition of the signal sig.
       The disp argument can be the address of a signal handler
       function, or one of the following constants:
       SIG_DFL
              Reset the disposition of sig to the default.

       SIG_IGN
              Ignore sig.

       SIG_HOLD
              Add sig to the process's signal mask, but leave the
              disposition of sig unchanged.
*/
sighandler_t sigset( int sig, sighandler_t disp ) {

    errno = 0 ; // 紀錄錯誤訊息
    sigset_t nowMask ;
    sigemptyset(&nowMask); // 將nowMask清為零
    sigprocmask( SIG_BLOCK, NULL, &nowMask ) ; // 獲取目前的Mask

    // 判斷有沒有在Mask中
    Boolean blocked = sigismember( &nowMask, sig ) ;

    if ( disp == SIG_HOLD ) { // 加入sig signal， 但保留sig的配置(sigaction)
        /*Add sig to the process's signal mask, but leave the
          disposition of sig unchanged. */
        sigset_t blockMask ;
        sigemptyset( &blockMask ) ; // 將blockMask清為零
        sigaddset( &blockMask, sig ) ; 
        if ( sigprocmask( SIG_BLOCK, &blockMask, NULL ) == -1 ) {
            errno = -1 ;
            printf("Error： %s\n",strerror(errno));
            exit(1) ;
        } // if 

    } // if 
    else {     // 移除sig signal並設定disposition 
        /* If disp was specified as a value other than SIG_HOLD, then sig is
        removed from the process's signal mask. */
        if ( ( sig != SIGKILL ) && ( sig != SIGSTOP) && blocked ) { 
            // 有在Mask裡才可unblock
            sigset_t blockMask ;
            sigemptyset( &blockMask ) ; // 將blockMask清為零
            sigaddset( &blockMask, sig ) ;  
            if ( sigprocmask( SIG_UNBLOCK, &blockMask, NULL ) == -1 ) {
                errno = -1 ;
                printf("Error： %s\n",strerror(errno));
                exit(1) ;
            } // if 
        } // if 

        // -------disposition，設定handler； SIG_IGN, SIG_DFL, other functions---------
        struct sigaction sa ; 
        sa.sa_flags = 0;
        sa.sa_handler = disp ;
        if (sigaction(sig, &sa, NULL) == -1) {
            errno = -1 ;
            printf("Error： %s\n",strerror(errno));
            exit(1) ;
        } // if 
  
    } // else 

    /* sigset() returns SIG_HOLD if sig was blocked before
       the call, or the signal's previous disposition if it was not
       blocked before the call*/
    if ( blocked ) return SIG_HOLD ;
    else return disp ;

} // sigset

Boolean TestNum( char *argv ) {
    int i ;
    for( i = 0 ; i < strlen(argv) ; i++ ) {}
    if ( i != 1 || (int)argv[0] < 49 || (int)argv[0] > 55  ) return FALSE ;
    else return TRUE ;
} // TestNum

int main( int argc, char *argv[] ) {

    // Command Type
    if( argc != 2 || !TestNum( argv[1] ) ) {
        printf( "Command Type: ./22-4, TestNum\n" ) ;
        printf( "---------TestNum List---------\n") ;
        printf( "(1) sigset(SIGINT, SIG_IGN)\n" ) ;
        printf( "(2) sigset(SIGINT, SIG_DFL)\n" ) ;
        printf( "(3) sigset(SIGINT, handler)\n" ) ;
        printf( "(4) sighold(SIGINT)\n" ) ;
        printf( "(5) sigrelse(SIGINT)\n" ) ;
        printf( "(6) sigignore(SIGINT)\n" ) ;
        printf( "(7) sigpause(SIGINT)\n" ) ;
        return 1 ;
    } // if 

    struct sigaction sa ; // sigpause測試用
    int testnum = atoi( argv[1] ) ;
    switch ( testnum ) {
        case 1: // (1) sigset(SIGINT, SIG_IGN)
            sigset( SIGINT, SIG_IGN ) ;
            printf( "Sigset(SIGINT, SIG_IGN): Ignore SIGINT, Type Ctrl+\\ to quit\n") ;
            pause() ;
            break ;
        case 2: // (2) sigset(SIGINT, SIG_DFL)
            sigset( SIGINT, SIG_DFL ) ;
            printf( "Sigset(SIGINT, SIG_DFL): Reset the disposition of sig to the default.\n") ;
            pause() ;
            break ;
        case 3: // (3) sigset(SIGINT, handler)
            sigset( SIGINT, handler ) ;
            printf("Sigset(SIGINT, handler): Type Ctrl+C to see new handler.\n");
            pause();
            break ;
        case 4: // (4) sighold(SIGINT)
            sighold( SIGINT ) ;
            printf( "Sighold(SIGINT): Block SIGINT, Type Ctrl+\\ to quit.\n" ) ;
            pause() ;
            break ;
        case 5: // (5) sigrelse(SIGINT)
            sighold( SIGINT ) ;
            printf( "Sighold(SIGINT): Block SIGINT\n" ) ;
            sigrelse( SIGINT ) ;
            printf( "sigrelse(SIGINT): Unblock SIGINT, Type Ctrl+c to terminate.\n" ) ;       
            pause() ;
            break ;
        case 6: // (6) sigignore(SIGINT)
            sigignore(SIGINT);
            printf("sigignore(SIGINT): SIGINT is ignored, Type Ctrl+\\ to quit.\n");
            pause() ;
            break ;  
        case 7: // (7) sigpause(SIGINT)
            // 暫停程式, 直到SIGINT訊號出現
            printf( "Sigpause(SIGINT): Pause, Type Ctrl+c to continue.\n" ) ;
            sigpause(SIGINT);
            pause() ;
            break ;  
    } // switch

    return 0 ; 
} // main()
