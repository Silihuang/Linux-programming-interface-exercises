#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main( int argc, char *argv[] ) {

    char buf[200] ;
    int fd ;


    mkfifo( "open", 0666 ) ;
    fd = open("open", O_RDONLY ) ; // O_NONBLOCK

    printf("Open Success\n");
    close(fd) ;
    unlink("open") ;
    return 0 ;

} // main