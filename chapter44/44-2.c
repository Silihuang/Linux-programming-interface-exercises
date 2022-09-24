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

// popen 產生pipe, 再fork子程序去執行execvp()
// "w"： 寫入pipe / "r": 讀取pipe
// pclose 1.關閉檔案符 2.等待execvp結束

static int pidfdarray[1000] ; // 用來查詢文件符的對應子程序

FILE *Popen( char *cmd[], char *R_W  ) {

	FILE *fp ;
	int pipefd[2] ; 
	pipe(pipefd) ; // 跟pipe做配合，再透過fileno回傳文件符
	
	pid_t pid ; // switch value
	int status = 0 ;
	switch( pid = fork() ) {

		case -1 :
			perror("popen fork");
			return NULL ;

		case 0 : // Child 

			if ( strcmp( R_W, "r" ) == 0 || strcmp( R_W, "R") == 0 ) { // 讀取data

				close( pipefd[0] ) ; 
				dup2( pipefd[1], STDOUT_FILENO ) ; // 讓execvp寫入pipe， 再從fd中讀取
				close( pipefd[1] ) ;

			} // if 
			else if ( strcmp( R_W, "w" ) == 0 || strcmp( R_W, "W") == 0 ){ // 寫入data
				close( pipefd[1] ) ; 
				dup2( pipefd[0], STDIN_FILENO ) ; // execvp從pipe讀， 以生成檔案	
				close( pipefd[0] ) ;
			} // else 

			// 子程序執行指令

			exit(0) ;

		default : // Father

			if ( strcmp( R_W, "r" ) == 0 || strcmp( R_W, "R") == 0 ) { // 讀取data
				close( pipefd[1] ) ;
				pidfdarray[pipefd[0]] = pid ;
				fp = fdopen( pipefd[0], "r") ; // 將pipe的文件描述符轉成文件pointer， 再來讓fp指向
			} // if 
			else if ( strcmp( R_W, "w" ) == 0 || strcmp( R_W, "W") == 0 ){ // 寫入data
				close( pipefd[0] ) ;
				pidfdarray[pipefd[1]] = pid ;
				fp = fdopen( pipefd[1], "w") ; // 將pipe的文件描述符轉成文件pointer， 再來讓fp指向	
			} // else 

			break ;

	} // switch

	if ( execvp( cmd[0], cmd ) == -1 )  { 
		return NULL ;
	} // if 
	return fp ;

} // popen 

void Pclose( FILE *fp ) {

	int fpnum = fileno(fp) ;
	int status ;
	int pid = pidfdarray[fpnum] ;
	waitpid( pid, &status, 0 ) ; // 等待運行中的子程序
	close(fpnum) ; // 關閉文件符

} // Pclose()

int main( int argc, char *argv[] ) {

	if ( argc < 2 || ( strcmp( argv[argc-1], "r" ) != 0  && strcmp(argv[argc-1], "w") != 0 
		&& strcmp( argv[argc-1], "R" ) != 0 && strcmp(argv[argc-1], "W") != 0 ) ) {
		printf( "Command Type: ./44-2, char *command, char *mode\n" ) ;
		return 0 ;
	} // if 

	char *cmd[argc] ;
	int i ;
	for( i = 1 ; i < argc - 1  ; i++ ) {
		// 把後面的Ｒ,Ｗ刪除 execvp才好扔環境變數
		cmd[i-1] = (char*)malloc( sizeof(char) * strlen( argv[i] ) ) ;
		strcpy( cmd[i-1], argv[i] ) ;
	} // for 
	cmd[i] = NULL ;
	// ------------popen & pclose---------------
	FILE *fp ;
	if( ( fp = Popen( cmd, argv[argc-1] ) ) == NULL ) { 
		perror("Failed to popen()") ;
		exit(1);
	} // if

	if ( strcmp( argv[argc-1], "r" ) == 0 || strcmp(argv[argc-1], "R") == 0 ) {
		// -----------Popen_read測試-----------
		char buf[10] = {0};
		while( fgets( buf, 10, fp ) != NULL ) {
			printf("%s", buf);
		} // while
	} // if 
	else if ( strcmp( argv[argc-1], "w" ) == 0  || strcmp(argv[argc-1], "W") == 0 ) {
		// -----------Popen_write測試： 請直接在螢幕上輸入文字來寫檔-----------
	} // else if 

	Pclose(fp);
 
   	return 0 ;
} // main