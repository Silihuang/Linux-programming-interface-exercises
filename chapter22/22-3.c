#define _GNU_SOURCE /* Get strsignal() declaration from <string.h> */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "tlpi_hdr.h"
#include <ctype.h>
void handler( int signo ) {
    return ;
} // handler

int Suspend( int argc, char *argv[] ) {

    int Runtimes = 0 ; // 計算要執行幾次
    Runtimes = atoi( argv[1] ) ; // 要交換的次數

    // ---------handler!!!--------
    // Suspend一定要設定handler, 不然會出現ERROR: USER Defined Siganl 
    struct sigaction sa ; 
    sa.sa_flags = 0;
    sa.sa_handler = handler ;
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
        perror("sigaction");
    // ---------handler!!!-------- 

    sigset_t blockMask, orgiMask ;
    sigemptyset(&orgiMask); // 將orgiMask清為零
    sigemptyset(&blockMask); // 將blockMask清為零
    sigaddset(&blockMask, SIGUSR1); 
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) // 新增SIGUSR1 Signal到MASK中, 第3個參數將返回舊的Mask
        perror("sigprocmask - SIG_BLOCK") ; 

    pid_t pid ; 
    switch( pid = fork() ) {

		case -1:	
 			perror("fork");
        	exit(EXIT_FAILURE);
        case 0 : // 子 -> 父 -> 子
            for( int i = 0 ; i < Runtimes ; i++ ) {
                if ( kill( getppid(), SIGUSR1 ) == -1 ) // 傳遞Signal
                    perror( "Fail to send signal" ) ;
                if (sigsuspend(&orgiMask) == -1 && errno != EINTR) // 等待訊號
                    perror("sigsuspend");  
                    // 猜測：block後suspend會去pending buffer找取訊號
                    // 等待除了MASK的訊號發生, 與sigwantinfo相反
                    // 需要sigaction, 因為要處理不需要的訊號 

            } // for
            break ;
        default :

            for( int i = 0 ; i < Runtimes ; i++ ) {
                if (sigsuspend(&orgiMask) == -1 && errno != EINTR) // 等待訊號
                    perror("sigsuspend");  
                if ( kill( pid, SIGUSR1 ) == -1 ) // 傳遞Signal
                    perror( "Fail to send signal" ) ;
            } // for
            return 0 ;
    } // fork

    if (sigprocmask(SIG_UNBLOCK, &blockMask, NULL) == -1) // 移除SIGUSR1 Signal
        perror("sigprocmask - SIG_BLOCK") ; 

    exit(EXIT_SUCCESS);
} // Suspend

int Wantinfo( int argc, char *argv[] ) {

    int Runtimes = 0 ; // 計算要執行幾次
    Runtimes = atoi( argv[1] ) ; // 要交換的次數

    sigset_t blockMask  ;
    sigemptyset(&blockMask); // 將blockMask清為零
    sigaddset(&blockMask, SIGUSR1); 
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) // 新增SIGUSR1 Signal到MASK中, 第3個參數將返回舊的Mask
        perror("sigprocmask - SIG_BLOCK") ; 

    pid_t pid ; 
    switch( pid = fork() ) {

		case -1:	
 			perror("fork");
        	exit(EXIT_FAILURE);
        case 0 : // 子 -> 父 -> 子
            for( int i = 0 ; i < Runtimes ; i++ ) {
                if ( kill( getppid(), SIGUSR1 ) == -1 ) // 傳遞Signal
                    perror( "Fail to send signal" ) ;
                if ( sigwaitinfo(&blockMask, NULL) == -1 ) // 等待訊號
                    perror("sigsuspend");  
                    // sigwaitinfo跟sigsuspend相反, 等待Mask的訊息
                    // sigwaitinfo不需sigaction, 因為只收需要的signal不必處理其他訊號
            } // for
            break ;
        default :
            for( int i = 0 ; i < Runtimes ; i++ ) {
                if ( sigwaitinfo(&blockMask, NULL) == -1 ) // 等待訊號
                    perror("sigsuspend");  
                if ( kill( pid, SIGUSR1 ) == -1 ) // 傳遞Signal
                    perror( "Fail to send signal" ) ;
            } // for
            return 0 ;
    } // fork

    if (sigprocmask(SIG_UNBLOCK, &blockMask, NULL) == -1) // 移除SIGUSR1 Signal
        perror("sigprocmask - SIG_BLOCK") ; 
        
    exit(EXIT_SUCCESS);
} // Wantinfo

int main(int argc, char *argv[]) {

    // Command Type
    if( argc != 2 ) {
        printf( "Command Type: ./22-3, 執行次數\n" );
        return 1 ;
    } // if 

    clock_t start, end, start2, end2 ;
    // ----------Sigsuspend----------
    start = clock();
    Suspend( argc, argv) ; 
    end = clock();
    double diff = end - start; // ms
    printf("Sigsuspend(): %.2fms\n" , diff);
    // ----------Sigwantinfo----------
    start2 = clock();
    Wantinfo( argc, argv) ; 
    end2 = clock();
    diff = end2 - start2; // ms
    printf("Sigwantinfo(): %.2fms\n" , diff);

    return 0 ; 
} // main()