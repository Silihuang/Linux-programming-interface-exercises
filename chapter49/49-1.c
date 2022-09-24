#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main( int argc, char *argv[] ) {
    int fd1, fd2 ;
    struct stat statbuf ; 
    caddr_t addr1, addr2 ; // 檔案記憶體起始位置 
    if ( argc != 3 || strcmp( argv[1], "--help" )== 0 ) {
        printf("Command Type: ./49-1, Filename.txt, Newfile.txt\n") ;
        exit(1) ;
    } // if 

    // 開啟目標檔案
    fd1 = open( argv[1], O_RDWR ) ;
    if ( fd1 == -1 ) {
        perror("open\n") ;
        exit(1) ;
    } // if 

    // fstat讀取目標文件符()的文件狀態
    // stat讀取目標檔案的文件狀態
    if ( fstat( fd1, &statbuf ) == -1 ) {
        perror("stat\n") ;
        exit(1) ;
    } // if 

    // 找尋檔案memory起始位置 
    addr1 = mmap( NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd1, 0 ) ;
    if ( addr1 == MAP_FAILED ) {
        perror("mmap\n") ;
        exit(1) ;
    } // if 
    else  close(fd1) ;

    // 複製新檔案
    fd2 = open( argv[2], O_RDWR | O_CREAT | O_TRUNC, 0777 ) ;
    if ( fd2 == -1 ) {
        perror("open2\n") ;
        exit(1) ;
    } // if 

    // 將新檔案大小變成跟目標一樣
    if( ftruncate( fd2, statbuf.st_size )== -1 ) {
        perror("ftruncate\n") ;
        exit(1) ;
    } // if 
 
    addr2 = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0 ) ;
    if ( addr2 == MAP_FAILED ) {
        perror("mmap2\n") ;
        exit(1) ;
    } // if 

    memcpy( addr2, addr1, statbuf.st_size);
    close( fd2 ) ;
    return 0 ;
} // main