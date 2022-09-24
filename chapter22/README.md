# Chapter 22: Signals: Advanced features

## Exercise 22-3

Section 22.10 stated that accepting signals using sigwaitinfo() is faster than the use of a signal handler plus sigsuspend(). The program signals/sig_speed_sigsuspend.c, supplied in the source code distribution for this book, uses sigsuspend() to alternately send signals back and forward between a parent and a child process. Time the operation of this program to exchange one million signals between the two processes. (The number of signals to exchange is provided as a command-line argument to the program.) Create a modified version of the program that instead uses sigwaitinfo(), and time that version. What is the speed difference between the two programs?


## Exercise 22-4

Implement the System V functions sigset(), sighold(), sigrelse(), sigignore(), and
sigpause() using the POSIX signal API.
