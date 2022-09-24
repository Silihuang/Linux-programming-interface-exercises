#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

#define SERVER_PORT 8700

static int pri_port ;

int main(int argc, char** argv) {

    if( argc != 2 || ( strcmp( argv[1], "1" ) != 0 
        && strcmp( argv[1], "2" ) != 0 ) ) {
        printf( "Normal  連線: ./client, 1\n" ) ;
        printf( "Priority連線: ./client, 2\n" ) ;
        exit(1) ;
    } // if 

	// 1. 
	int normal_connect ;
	struct sockaddr_in server ;
	// 2.
	int priSock, pri_accept ;
	struct sockaddr_in pri_client ;

	// ---------------Priority Socket---------------
	// 			 2-1(Part1). Server---->Client 

    // Create socket to listen incoming connections.

    priSock = socket( AF_INET, SOCK_STREAM, 0) ;
    // Set local address info.
    memset( &pri_client, 0, sizeof( pri_client ) ); 
    pri_client.sin_family = AF_INET ; 
    pri_client.sin_addr.s_addr =  INADDR_ANY  ; 
    pri_client.sin_port = htons(0) ; // ephemeral Port.
    // Bind to the local address.
    if ( bind( priSock, (struct sockaddr*) &pri_client, sizeof(pri_client) ) < 0 ) {    
		perror( "client bind" ) ;
		exit(1) ;
    } // if 
	// getsockname()
    socklen_t cLen = sizeof( pri_client ) ;
    getsockname( priSock, (struct sockaddr*) &pri_client, &cLen ) ; 
	// ----------------Send Portnum----------------
	// 			   1. Client---->Server
    // Set server address info.
	
    memset( &server, 0, sizeof(server)); // Init.
    server.sin_family = AF_INET; // Internet address family.
    server.sin_addr.s_addr = inet_addr( "127.0.0.1" ) ; 
    server.sin_port = htons(SERVER_PORT); // server Port
	normal_connect = socket( AF_INET, SOCK_STREAM, 0 ) ;
	connect( normal_connect, (struct sockaddr*) &server, sizeof(server));
	char portnum[10] ; // 傳遞priority socket的port
	sprintf( portnum, "%d", pri_client.sin_port ) ;
    send( normal_connect, portnum, sizeof(portnum), 0 ) ;

	// ----------------Priority Socket----------------
	// 			   2-2(Part2). Server---->Client	
  	// Listen
    listen( priSock, 1 ) ;
    // Accept
    pri_accept = accept( priSock,  NULL, NULL ) ;
    // Recv data
	char cookie[10] ;
	if ( recv( pri_accept, cookie, sizeof(cookie), 0 ) < 0 ) {
        perror( "recv" ) ;
        exit(1) ;
    } // if 
    
    if ( strcmp( cookie, portnum ) == 0 ) printf("Correct cookies\n") ;
    else printf( "Negative cookies\n" ) ;

    // ----------------Experiment----------------
    // Normal socket's fd: normal_connect
    // Priority socket's fd: pri_accept

    if (  strcmp( argv[1], "1" ) == 0 ) {
        send( normal_connect, "Normal Socket success", sizeof("Normal Socket success"), 0 ) ;
    } // if 
    else {
        send( normal_connect, "Priority Socket success", sizeof("Priority Socket success"), 0 ) ;
    } // else 

    // close
    close( pri_accept ) ; 
	close( normal_connect ) ;

    return 0 ;

} // main