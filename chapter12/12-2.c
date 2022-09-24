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

typedef struct process {
  int pid  ;
  char *name ;
  int childnum ;
  struct process *neighbor ; 
  struct process *child ;
  struct process *father ;
} process ;

static process *Tree ; 

void InitializeTree() {
    // Fetch pid_max ;
    long max = 0 ;
    FILE* fp = fopen("/proc/sys/kernel/pid_max", "r");
    if( fp == NULL || ( fscanf( fp, "%ld", &max ) != 1 ) ) printf( "Error: fp" ) ;
    // Initialize
    if( Tree == NULL){
        Tree = malloc(sizeof(process) * max ); 
        memset(Tree, 0, sizeof(process) * max );
        Tree->name = (char*)malloc(sizeof(char)*1024) ;
        Tree->name = "" ;
        Tree->childnum = 0 ;
        Tree->father = NULL ;
        Tree->child = NULL ;
        Tree->neighbor = NULL ;
    } // if 
} // Initialize

// 判斷數字有多少位元
int Digitsofnum( int number ) {
  int count = 0 ;
  while( number != 0 ) {   
    number /= 10;
    count++ ;
  } // while

  return count ;
} // int Digitsofnum

// --------check out Folder that named entry is PID-------- 
int IsPidFolder( const struct dirent *entry ) { 
  const char *p ;
  for ( p = entry->d_name ; *p ; p++ ) {
    if ( !isdigit(*p) ) return 0 ; // False
  } // for

  return 1 ; // True
} // IsPidFoler

// --------Read all pid's PPid and number, Name is call by reference ------- 
int GetPPidandName( const char *pid, char *name ) {
  // ppid, name is not const, they were been return ;
	char line[1000] = "" ;
  // 產生新的陣列空間  
  int length = strlen("/proc/") + strlen("/status") + strlen(pid) + 1;      
  char *path = (char*)malloc(sizeof(char) * length); 
  memset(path, 0, sizeof(*path)) ; // initialize path

  // -----/proc/pidnum/status-------
  strcat( path, "/proc/" ) ;
  strcat( path, pid ) ;
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
      strtok( line, " \t\n" ); // Name
      strcpy( name, strtok( NULL, " \t\n" ) ); // xxx    
    } // if 
    if ( line[0] == 'P' && line[1] == 'P' && line[2] == 'i' && line[3] == 'd' ) {
      // PPid: x
      char* token = strtok( line, " \t\n" ); // PPid
      token = strtok( NULL, " \t\n" ); // xxx
      int ppid = atoi( token ) ; // string to int
      fclose( fp ); 
      return ppid ;
    } // if 
	} // while

	fclose(fp);
  printf( "Error: Get ppid\n" ) ;
	return 1 ;
} // GetPPidandName()

// --------Add the branches on a tree--------     
int BuildTree( const char *dirpath ) { 

  DIR *dirp ; // directory streamm, it indicates the directory's location
  struct dirent *dp ; // dirent pointer, including directory's detail
  // -----Open the directory-------
  dirp = opendir(dirpath) ; 
  if ( dirp == NULL ) {
    printf("opendir failed on '%s'", dirpath) ;
    return 0;
  } // if
  // -----Detect all PidFolder in /Proc-------
  for( ;; ) {  
    dp = readdir( dirp ) ;
    if( dp == NULL ) break ; // break ;

    // excluded Files 
    if ( strcmp( dp->d_name, "." ) == 0 || strcmp( dp->d_name, ".." ) == 0 || !IsPidFolder( dp ) ) continue ;

    // Fetch /proc/pid/status's data  
    char *processname = (char*)malloc( sizeof(char) * strlen( dp->d_name ) )  ; 
    int pid = atoi(dp->d_name) ;
    int ppid = GetPPidandName( dp->d_name, processname ) ;

    // Plant Process on the Tree 
    process *parent = &Tree[ppid]; // Get Father的位址
    process *currentprocess = &Tree[pid]; // Get child的位址

    currentprocess->name = realloc( currentprocess->name, sizeof(char)*strlen(processname) ); 
    strcpy( currentprocess->name, processname ) ;
    currentprocess->pid = pid ;
    currentprocess->neighbor = parent->child ; // 兄弟相連
    parent->child = currentprocess ; 
    currentprocess->father = parent ; 
  
    parent->childnum++ ; // calculated the num of child 

  } // for 

  if ( closedir(dirp) == -1 ) printf("closedir") ; // close the directory stream 
  return 0 ;

} // ListFiles


int PrintTree( process *current ) {
  if ( current == NULL ) {
    return 0 ;
  } // if 
  else {
    while( current != NULL ) {

      // 往上找父親且縮排，直到root
      for( process *p = current->father ; p -> father!= NULL ; p = p -> father ) {
        if( p->child->pid == current->pid ) { // 進到此迴圈，代表是最小的兒子
          printf("---") ;
          break ;
        } // if  
        else {     
          int spacenum = strlen( p->name ) + Digitsofnum( p->pid ) + 5 ;
          for(int i = 0 ; i < spacenum ; i++) printf(" ") ;
        } // else 
      } // for

      printf( "%s(%d)", current->name, current->pid ) ;
      if ( current->childnum == 0 )printf("\n") ;
      PrintTree( current->child ) ;
      current = current->neighbor ; 

    } // while
  } // else 
} // PrintTree() 

int main(int argc, char *argv[]) {

  InitializeTree() ; // 初始化樹，給樹空間大小為Pid_max
  BuildTree("/proc") ; // Insert all User's Processes
  PrintTree( &Tree[1] ) ; // Print the family tree( root is pid: 1)
  return 0 ;

} // main
