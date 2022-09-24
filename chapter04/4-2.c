#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define Buffer_Size 1024 

int main(int argc, char *argv[]) {
 
  // Command格式: ./hw1-2 XXX.txt XXX.txt
  if ( argc == 3 ) {
    char buffer[ Buffer_Size + 1 ] ; // 存放文件內容
    int numofread = 0 ; // 讀取的字數節  
    int InputFile = 0 ; // 來源file的文件描述符
    int OutputFile = 0 ; // 目的file的文件描述符

	  if ( ( ( InputFile = open(argv[1], O_RDONLY) ) == -1 ) 
       || ( ( OutputFile = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0666) ) == -1 ) ) {
      // 設定文件描述符
	    printf("Failed to open file") ;
	    return 0 ;
	  } // if 

    while( ( numofread = read(InputFile, buffer, Buffer_Size) )  > 0 ) {
      // 讀取資料放到buffer中
      // Nullbytes也需要Copy
      for ( int i = 0 ; i < numofread ; i++ ) {
        // 藉由for迴圈將讀取到的
        write( OutputFile, &buffer[i], 1 ) ; 
      } // for
  
    } // while 
    if ( numofread < 0 ) printf("Data exception") ; // 讀取錯誤
    close(InputFile) ;
    close(OutputFile) ;

  } // if 
  else {
    // Command格式錯誤
    if ( argc != 3 ) { 
      printf("Syntax error") ;
      return 0 ;
    } // if 
  } // else 

  return 0 ;
} // main