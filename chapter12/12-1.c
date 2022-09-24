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

uid_t UserIdFromName( const char* name ) ;
void ListFiles( const char *dirpath, const int userid ) ;
static int IsPidFolder( const struct dirent *entry ) ;
int GetPidStatus_UidandName( const char *foldername, char *Name ) ; // only "Name" isn't const

// --------check out Folder that named entry is PID-------- who uses? : ListFiles()
int IsPidFolder( const struct dirent *entry ) { 
  const char *p ;
  for ( p = entry->d_name ; *p ; p++ ) {
    if ( !isdigit(*p) ) return 0 ; // False
  } // for

  return 1 ; // True
} // IsPidFoler

// --------Read all pid's status and find the Uid number, Name is call by reference -------  who uses? : ListFiles()
int GetPidStatus_UidandName( const char *foldername, char *Name ) {

	char line[1000] = "" ;
  // 產生新的陣列空間  
  int length = strlen("/proc/") + strlen("/status") + strlen(foldername) + 1;      
  char *path = (char*)malloc(sizeof(char) * length); 
  memset(path, 0, sizeof(*path)) ; // initialize path

  // -----/proc/pidnum/status-------
  strcat( path, "/proc/" ) ;
  strcat( path, foldername ) ;
  strcat( path, "/status") ;

  // -----Open file-------
	FILE *fp = fopen( path , "r" ); 
	if ( fp == NULL ) {
		printf("failed to open txt\n");
		return 1 ; 
	} // if 

  // -----Read file-------
	while( !feof( fp ) ) { 
		memset(line, 0, sizeof(line)); // initialize line
		fgets( line, sizeof(line) - 1, fp ); // 包含了換行符
    if ( line[0] == 'N' && line[1] == 'a' && line[2] == 'm' && line[3] == 'e' ) {
      // Name: xxx 
      strtok( line, " \t" ); // Name
      strcpy( Name, strtok( NULL, " \t" ) ); // xxx    
    } // if 
    if ( line[0] == 'U' && line[1] == 'i' && line[2] == 'd' ) {
      // Uid: xxx xxx xxx xxx
      char* token = strtok( line, " \t" ); // Uid
      token = strtok( NULL, " \t" ); // xxx
      int int_uid = atoi( token ) ; // string to int
      fclose( fp ); 
      return int_uid ;
    } // if 
	} // while

	fclose(fp);
	return 0 ;
} // GetStatusUid()

// --------return user ID--------               who uses? : main()
uid_t UserIdFromName( const char* name ) { 
  struct passwd *pwd ;
  uid_t u ;
  char *endptr ;

  if ( name == NULL || *name == '\0' ) return -1 ; // On NULL or empty string, return an error

  u = strtol( name, &endptr, 10 ) ; // transform string to long int type on base 10 ; 
  // !! something weird happened here... about strtol !!

  if (*endptr == '\0' ) return u ;
  pwd = getpwnam( name ) ;
  if ( pwd == NULL ) return -1 ; 

  return pwd->pw_uid ;
  
} // userIdFromName

// --------List user's all files in directory 'dirPath'--------      who uses? : main()
void ListFiles( const char *dirpath, int userid ) { 

  DIR *dirp ; // directory streamm, it indicates the directory's location
  struct dirent *dp ; // dirent pointer, including directory's detail

  // -----Open the directory-------
  dirp = opendir(dirpath) ; 
  if ( dirp == NULL ) {
    printf("opendir failed on '%s'", dirpath) ;
    return ;
  } // if

  // -----Detect all PidFolder in /Proc-------
  for( ;; ) {  
    dp = readdir( dirp ) ;
    if( dp == NULL ) break ;

    // Don't print out if dp->d_name is "." / ".." / "not digits".
    if ( strcmp( dp->d_name, "." ) == 0 || strcmp( dp->d_name, ".." ) == 0 || !IsPidFolder( dp ) ) continue ;

    // find user's process, check out status's uid  
    char *processname = (char*)malloc(sizeof(char) * strlen(dp->d_name) );  ; // argument for GetPidStatus_UidandName
    if ( GetPidStatus_UidandName( dp->d_name, processname ) == userid ) { 
      printf( "Pid: %s, Name: %s\n", dp->d_name, processname ) ;
    } // if 
  
  } // for 

  if ( closedir(dirp) == -1 ) printf("closedir") ; // close the directory stream 

} // ListFiles


int main(int argc, char *argv[]) {

  if ( argc != 2 ) printf("Command Type: ./hw3-1, UserName\n") ;
  int userid = UserIdFromName( argv[1] ) ; 
  ListFiles("/proc", userid ) ; // Lists all User's Processes

  return 0 ;
} // main
