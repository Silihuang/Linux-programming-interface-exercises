#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#define Buffer_Size 2048 

int main(int argc, char *argv[]) {

  char buffer[ Buffer_Size + 1 ] ; // 存放文件內容
  int numofinput = 0 ; // input的字數
  int file = 0 ; // 文件描述符

  // --------覆寫檔案or增加內容--------
  if ( argc == 2 || argc == 3 ) {
    // argc == 2 : 覆寫，Command格式: ./hw1-1 xxx.txt
    // argc == 3 : Append，Command格式: ./hw1-1 -a xxx.txt

    // --------開啟檔案--------
    int ch = 0 ;
    // getopt(): 解析命令行參數 
    if ( ( ch = getopt( argc, argv, "a:" ) ) != -1 && ch == 'a' ) { // Append
      // optarg: 參數(檔案名稱) / "a:"單個冒號代表a後面需接參數
      file = open( optarg, O_CREAT | O_WRONLY | O_APPEND, 0666 ) ; 
      if ( file == -1 ) printf("Error") ; 
    } // if 
    else {  // Rewrite
      file = open( argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666 ) ; 
      if ( file == -1 ) printf("Error") ; 
    } //else
  
  } // if

  // --------讀input--------
  while( ( numofinput = read(STDIN_FILENO, buffer, Buffer_Size) ) != 0 ) {

    if ( write( STDOUT_FILENO, buffer, numofinput) < numofinput ) printf("Error") ; // 輸出
    if (  argc == 2 || argc == 3 ) {
      if ( write( file, buffer, numofinput ) < numofinput ) printf("Error") ;   // 寫檔
    } // if 
  } // while

  close(file) ; 
  return 0 ;
} // main
