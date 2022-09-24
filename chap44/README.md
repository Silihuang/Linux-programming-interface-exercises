# Chapter 44:  Pipes and FIFOs

## Exercise 44-1

Write a program that uses two pipes to enable bidirectional communication between a parent and child process. The parent process should loop reading a block of text from standard input and use one of the pipes to send the text to the child, which converts it to uppercase and sends it back to the parent via the other pipe. The parent reads the data coming back from the child and echoes it on standard output before continuing around the loop once more

## Exercise 44-2

Implement popen() and pclose(). Although these functions are simplified by not requiring the signal handling employed in the implementation of system() (Section 27.7), you will need to be careful to correctly bind the pipe ends to file streams in each process, and to ensure that all unused descriptors referring to the pipe ends are closed. Since children created by multiple calls to popen() may be running at one time, you will need to maintain a data structure that associates the file stream pointers allocated by popen() with the corresponding child process IDs. (If using an array for this purpose, the value returned by the fileno() function, which obtains the file descriptor corresponding to a file stream, can be used to index the array.) Obtaining the correct process ID from this structure will allow pclose() to select the child upon which to wait. This structure will also assist with the SUSv3 requirement that any still-open file streams created by earlier calls to popen() must be closed in the new child process.
