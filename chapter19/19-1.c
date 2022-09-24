#define _XOPEN_SOURCE 500 // 預編譯命令、nftw() 跟 S_IFSOCK
#include <stdio.h>
#include <sys/inotify.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tlpi_hdr.h" // inotify
#include "error_functions.h"
#include <stdarg.h>
#include <ftw.h>

// 從監控文件符read的buffer，一次最多讀10次事件, buffer = event + 文件名最大255 + 空字符
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static int seeknum ; // 監控資料夾的數量
static int inotifyFd ; // 監控描述符集合

struct detail{
	int wd ; // 紀錄資料夾的wd
	char realpath[100] ; // 資料夾的絕對路徑
} ; // detail

static struct detail detailset[1000] ; // 存放對應資料的集合

static void usageError( const char *progName, const char *msg) {
	if (msg != NULL)
	fprintf(stderr, "%s\n", msg);
	fprintf(stderr, "Usage: %s [-d] [-m] [-p] [directory-path]\n", progName);
	fprintf(stderr, "\t-d Use FTW_DEPTH flag\n");
	fprintf(stderr, "\t-m Use FTW_MOUNT flag\n");
	fprintf(stderr, "\t-p Use FTW_PHYS flag\n");
	exit(EXIT_FAILURE);
} //usageError

static int add_watch(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb ) {

	int wd ; // inotify：文件描述符(監控集合), wd: 監控描述符

	// 將指定的文件加入監控中, IN_ALL_EVENTS:監控所有事件
	// pathname: 絕對位置, &pathname[ftwb->base]:相對位置
	wd = inotify_add_watch(inotifyFd, pathname, IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO ); 
 	if ( wd == -1 ) {
		printf( "Error: inotify_add_watch()" );
		return 1 ;
	} // if 	

	// 紀錄監控符跟對應資料夾的路徑, 才方便新增新資料夾的監控
	detailset[seeknum].wd = wd ;
	strcpy( detailset[seeknum].realpath, pathname) ;
	seeknum++ ;

	return 0; 
} // direTree

// 將inotify_event的內部事件印出 
static void DisplayInotifyEvent( struct inotify_event *i ) {

	printf("wd =%2d, ", i->wd ) ; // 監控的descriptor
	// 觀察的事件
	// 如果將被監控目錄中的某個文件改名，那麼inotify會發送IN_MOVED_FROM和 IN_MOVED_TO兩個事件，並且這兩個事件的cookie相同

	if (i->mask & IN_CREATE) {
		// !!!依題目要求新資料夾內也要監控!!!
		int j ;
		for ( j = 0 ; j < seeknum ; j++ ) {
			// 找新資料夾的絕對路徑 
			if ( i->wd == detailset[j].wd ) {
				// 把新資料夾加入監控
				char path[100] = "" ;
				strcpy( path, detailset[j].realpath ) ;
				strcat( path, "/" ) ;
				strcat( path, i->name ) ;
				if ( nftw( path, add_watch, 1000, 0) == -1) {
					perror("nftw");
					exit(EXIT_FAILURE);
				} // if 
				//printf("%s\n", path ) ;
			} // if 
		} // for 

		printf("mask = IN_CREATE, ");
	} // if 
	if (i->mask & IN_DELETE) printf("mask = IN_DELETE, ");
	if (i->mask & IN_MOVED_FROM) printf("mask = IN_MOVED_FROM, ");
	if (i->mask & IN_MOVED_TO) printf("mask = IN_MOVED_TO, ");
	if (i->len > 0) printf("name = %s\n", i->name);
} // DisplayInotifyEvent 


int main( int argc, char *argv[] ) {
	
 	if ( argc < 2 || strcmp(argv[1], "--help") == 0) {
		printf("Command Type: %s, pathname...\n", argv[0] );
		return 0 ;
	} // if   
	int flags = 0;
	seeknum = 0 ;
	inotifyFd = inotify_init() ; // 初始化inotify文件描述符
 	if (inotifyFd == -1) printf("inotify_init");

	if ( nftw( argv[1], add_watch, 1000, flags) == -1) {
		perror("nftw");
		exit(EXIT_FAILURE);
	} // if 

 	struct inotify_event *event;
	while( 1 ) {  // 持續監控

 		ssize_t numRead; // Read()結果, 正常會回傳length
 		char buf[BUF_LEN]; // 從監控文件符read的buffer，一次最多讀10次事件 
		numRead = read(inotifyFd, buf, BUF_LEN) ; 
 		if (numRead == 0 ) printf("read() from inotify fd returned 0!"); // 讀到EOF
 		if ( numRead == -1 ) printf("read"); // ERROR

 		for ( char *p = buf ; p < buf + numRead ; ) {
 			event = ( struct inotify_event * ) p ;
			DisplayInotifyEvent(event);
			p += sizeof(struct inotify_event) + event->len;
 		} // for
		
 	} // while
 	
	exit(EXIT_SUCCESS);

} // main


