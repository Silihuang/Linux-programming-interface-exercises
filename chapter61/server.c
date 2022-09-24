#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define SERVER_PORT 8700

int main( int argc, char** argv ) {

    if( argc != 1 ) {
        printf( "Command Type: ./server\n" ) ;
        exit(1) ;
    } // if 

	// 1.
	int listenSock, normal_accept ;
	struct sockaddr_in server ;
	struct sockaddr_in client ;
	// 2. 
	int pri_connect ; 
	struct sockaddr_in pri_server ;

	// ----------------Receive Portnum----------------
	// 			   1. Client---->Server	
	// Create socket to listen incoming connections.
    listenSock = socket( AF_INET, SOCK_STREAM, 0) ;
    // Set local address info.
    memset( &server, 0, sizeof( server ) ); 
    server.sin_family = AF_INET ; 
    server.sin_addr.s_addr = htonl( INADDR_ANY ) ; 
    server.sin_port = htons(SERVER_PORT) ; // server Port.
	// bind to the host
	if ( bind( listenSock, (struct sockaddr*) &server, sizeof(server) )  < 0 ) {    
		perror( "server bind" ) ;
		exit(1) ;
    } // if 
	// Listen
	listen( listenSock, 5 ) ;
	// Accept
	normal_accept = accept( listenSock, NULL, NULL ) ;
	// recv Data
	char client_pri_port[10] ;
	recv( normal_accept, client_pri_port, sizeof(client_pri_port), 0 ) ;

	// ----------------Priority Socket----------------
	// 			   2. Server---->Client	

    // Create a reliable, stream socket using TCP.
    pri_connect = socket( AF_INET, SOCK_STREAM, 0 ) ;
    // Set client address info.
    memset( &pri_server, 0, sizeof(pri_server) ) ; // Init.
    pri_server.sin_family = AF_INET; // Internet address family.
    pri_server.sin_addr.s_addr = inet_addr( "127.0.0.1" ) ; 
    pri_server.sin_port = atoi(client_pri_port) ; // Priority Port.
	// Connect to client
    if( connect( pri_connect, (struct sockaddr*) &pri_server, 
		sizeof(pri_server) ) < 0 ) {
			perror("connect") ;
			exit(1) ;
	} // if ;

	// Send data
	char cookie[10] ; // 將client的port作為cookie再傳回去
	strcpy( cookie, client_pri_port ) ;
    send( pri_connect, cookie, sizeof(cookie), 0 ) ;
	// ----------------Experiment: select()----------------
    // Normal socket's fd: normal_accept
    // Priority socket's fd: pri_connect

  	fd_set readfds ;
	FD_ZERO( &readfds ) ;
	FD_SET( normal_accept, &readfds ) ;
	FD_SET( pri_connect, &readfds ) ;
	// 看哪個fd最大
	int maxfd = normal_accept > pri_connect? normal_accept: pri_connect ; 
    struct timeval tv = {1,0} ; // timeout時間
  	while(1) {
		char buffer[100] ;
		switch( select( maxfd+1, &readfds, &readfds, NULL, &tv ) ) {
			case -1:
				perror("select") ; 
				exit(-1) ;
				break ; 
            case 0:
				break ; //繼續select
            default:
                if( FD_ISSET( normal_accept, &readfds ) ) {
					// 判斷normal socket有沒有request
					recv( normal_accept, buffer, sizeof(buffer), 0 ) ;
					printf("%s\n", buffer ) ;
                } // if 
				else if ( FD_ISSET( pri_connect, &readfds ) ) {
					// 判斷priority socket有沒有request
					recv( pri_connect, buffer, sizeof(buffer), 0 ) ;
					printf("%s\n", buffer ) ;
				} // else if 

				// Close.
				close( normal_accept ) ;
				close( pri_connect ) ;  
				return 0 ;
        } // switch
     } // while

} // main

