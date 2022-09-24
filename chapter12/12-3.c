#include<sys/stat.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pwd.h>
#include<grp.h>
#include<ctype.h>
#include<dirent.h>
#include<errno.h>

#define BUF_SIZE PATH_MAX

// --------check out Folder that named entry is PID-------- who uses? : ListFiles()
int IsPidFolder( const struct dirent *entry ) { 
  const char *p ;
  for ( p = entry->d_name ; *p ; p++ ) {
    if ( !isdigit(*p) ) return 0 ; // False
  } // for

  return 1 ; // True
} // IsPidFoler

// ----Get the link under /proc/pid/fd/------- who uses? : ListFiles()
int GetLink( char *fdpath, const char *linkname, char *fdnum ) {  

  DIR *fdstream ; // directory stream, it indicates the fd's location
  struct dirent *fdfile ; // dirent pointer, including fd's detail 

  fdstream = opendir(fdpath) ;  // path : /proc/pid/fd/

  for( ;; ) {  
    char buf[BUF_SIZE] = "" ;
    char linkpath[1024] = "" ;
    fdfile = readdir( fdstream ) ; // path: /proc/pid/fd/xxx, automatic catch next file
    if( fdfile == NULL ) break ; 

    // if /proc/pid/fd/. || /proc/pid/fd/.. stop doing
    if ( strcmp( fdfile->d_name, "." ) == 0 || strcmp( fdfile->d_name, ".." ) == 0  ) continue ;

    strcpy( linkpath, fdpath ) ;
    strcat( linkpath, fdfile->d_name ) ;
    int rslt = readlink( linkpath, buf, BUF_SIZE-1  );
     if( rslt < 0 || rslt >= BUF_SIZE-1 ) return 1 ;

    // compare linkname 
    if ( strcmp( linkname, buf ) == 0 ) {
      strcpy( fdnum, fdfile->d_name ) ;
      if ( closedir(fdstream) == -1 ) printf("closedir") ; // close the directory stream 
      return 1 ;
    } // if 
  
  } // for
  if ( closedir(fdstream) == -1 ) printf("closedir") ; // close the directory stream 
  return 0 ;

} // GetLink()


// --------List user's all files in directory 'dirPath'--------      who uses? : main()
int ListFiles( const char *dirpath, const char *linkname ) { 

  DIR *dirp ; // directory stream, it indicates the directory's location
  struct dirent *dp ; // dirent pointer, including directory's detail 
  // -----Open the directory-------
  dirp = opendir(dirpath) ; 
  if ( dirp == NULL ) {
    printf("opendir failed on '%s'", dirpath) ;
    return 1 ;
  } // if 

  // -----Detect all PidFolder in /Proc-------
  for( ;; ) {  
    dp = readdir( dirp ) ;
    if( dp == NULL ) break ;
    // Don't print out if dp->d_name is "." / ".." / "not digits".
    if ( strcmp( dp->d_name, "." ) == 0 || strcmp( dp->d_name, ".." ) == 0 || !IsPidFolder( dp ) ) continue ;

    char fdpath[30] = "" ; // link's path 
    strcat( fdpath, "/proc/" ) ; // reset path
    strcat( fdpath, dp->d_name ) ;
    strcat( fdpath, "/fd/" ) ; 
    char fdnum[100] = "" ; 
    if( GetLink( fdpath, linkname, fdnum ) == 1 ){ 
      // Find the corresponding file
      // output the processes that had used linkname 
      printf( "Pid:%s, %s, %s\n\n", dp->d_name, fdnum, linkname ) ;
    } // if 
  } // for 

  if ( closedir(dirp) == -1 ) printf("closedir") ; // close the directory stream 
  return 0 ;

} // ListFiles


int main(int argc, char *argv[]) {

  if ( argc != 2 ) {
    printf("Command Type: ./hw3-3, linkpath\n") ;
    return 0 ;
  } // if 
  ListFiles("/proc", argv[1] ) ; // Lists all User's Processes

  return 0 ;
} // main
