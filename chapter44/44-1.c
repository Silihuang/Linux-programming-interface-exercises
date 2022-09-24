
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>

int main( int argc, char *argv[] ) {

	int pipedata[2];
	int pipeupper[2] ;
	pid_t pid ;
	char pipedata_buff[1000] ; // read出來的資料
	char pipeupper_buff[1000]  ; // write資料回去
	printf("----------Lower to Upper----------\n") ;
	printf("Input: ") ;
	while( fgets( pipedata_buff, 1000, stdin ) ) { 

		if ( pipe(pipedata) ) printf("pipe error") ; // 從父傳給子
		if ( pipe(pipeupper) ) printf("pipe error") ; // 子把資料變大寫傳回給父
		switch ( pid = fork() ) {
			case -1:	
 				perror("fork");
            	exit(EXIT_FAILURE);

			case 0:	// Child
				// 父---->子。P.S.沒有資料寫進pipe的情況下read會等write
				close( pipedata[1] ) ;
				read( pipedata[0], pipeupper_buff, 1000 ) ;

				// 把字母轉成大寫。a-->A, b--->B
				for( int i = 0 ; pipeupper_buff[i] != '\n' ; i++ ) {
					if ( isalpha( pipeupper_buff[i] ) ) {
						// 是字母
						pipeupper_buff[i] = toupper(pipedata_buff[i]) ;
					} // if 
					else { 
						// 字母或是特殊符號
						pipeupper_buff[i] = pipedata_buff[i] ;
					} // else 
				} // for

				// 子---->父。
				close(pipeupper[0]) ;
				write( pipeupper[1], pipeupper_buff, 1000 ) ;
				break;

			default: // Parent
				// 讀取input，將資料寫入, 父---->子
				close( pipedata[0]) ;
				write( pipedata[1], pipedata_buff, 1000 ) ;
				// 讀取從child修正完的資料, 子---->父
				close( pipeupper[1] ) ;
				read( pipeupper[0], pipeupper_buff, 1000 ) ;
				printf("Output: %s", pipeupper_buff) ;
				printf("Input: ") ;
		} // switch

		memset( pipedata_buff, 0, 1000) ; // initial
		memset( pipeupper_buff, 0, 1000) ;

	} // while
  	return 0 ;
} // main
