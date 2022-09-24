#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define Buffer_Size 1024 

int main(int argc, char *argv[]) {

  char buf[Buffer_Size+1] ;
  int linenum = 0 ; // 行數
  int file = 0 ; // 文件描述符

  if ( argc == 2 || argc == 4 ) {
    // argc == 2 : 後十行: ./hw2 xxx.txt
    // argc == 4 : 後n行 : ./hw2 -n num xxx.txt
    // --------開啟檔案--------
    int ch = 0 ;
    // getopt(): 解析命令行參數 
    if ( ( ch = getopt( argc, argv, "n:" ) ) != -1 && ch == 'n' ) { // tail -n num xxx.txt
      // optarg: 參數(檔案名稱) 
      file = open( argv[3], O_RDONLY , 0666 ) ; 
      if ( file == -1 ) {
      	printf("Error") ; 
      	exit(1) ;
      } // if 
      linenum = atoi(argv[2]) ; // 要求的行數
    } // if 
    else {  // tail xxx.txt
      file = open( argv[1], O_RDONLY , 0666 ) ; 
      if ( file == -1 ) {
      	printf("Error") ; 
      	exit(1) ;
      } // if 
      linenum = 10 ; // 預設
    } //else
  
  } // if
  else return 0 ;
  
  
  int size = lseek(file, 0, SEEK_END) ; // 判斷檔案大小

  while(1) {
 
   	char buf[ Buffer_Size + 1 ] ;
   	int charnum = 0 ; // 目前buffer有多少char
  	if ( size >= Buffer_Size ) { // 檔案比較大, 從最後開始讀
    		lseek( file, -Buffer_Size, SEEK_CUR ) ;
    		read( file, buf, Buffer_Size ) ;
    		charnum = Buffer_Size ;
  	} // if 
  	else { // buffer足夠容納file
  		lseek( file, 0, SEEK_SET ) ;
  		read( file, buf, size ) ;
  		charnum = size ;
  	} // else 

 	size = size - Buffer_Size ; // 更新檔案大小
 	
 	// -----------判斷換行-----------
 	int i = 0 ; 
	while( i < charnum ) {
 	  if ( buf[i] == '\n') linenum-- ;
 	  if ( linenum == 0 ) break ;
	  i++ ;
	} // while 

	if ( size <= 0 || linenum == 0 ) {
	  lseek( file, -i-1 , SEEK_CUR ) ; // 將文件符往前移
	  break ; 
	} // if 
 	
	lseek( file, -Buffer_Size, SEEK_CUR ) ; // 將文件符往前移

  } // while

  // -----------印出-----------
  int eof ;
  while( eof = read( file, buf, Buffer_Size ) ) {
    buf[eof] = '\0' ; 
    printf( "%s", buf) ;
    if ( eof != Buffer_Size) break ;
 	
  } // while

  close(file) ; 
  return 0 ;
} // main
