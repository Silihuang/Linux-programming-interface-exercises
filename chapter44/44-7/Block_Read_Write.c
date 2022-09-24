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

    mkfifo( "R_W", 0666 ) ;
    fd = open("R_W", O_RDWR ) ; // O_NONBLOCK

    // write( fd, "R_W Success", strlen("R_W Success") + 1 ) ;
    read( fd, buf, 200 ) ;

    printf("Success\n" ) ;
    close(fd) ;
    unlink("R_W") ;
    return 0 ;

} // main