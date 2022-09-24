# Chapter 13: File I/O Buffering
## Exercise 13-5

The command tail [ -n num ] file prints the last num lines (ten by default) of the named file. Implement this command using I/O system calls (lseek() , read(), write(), and so on). Keep in mind the buffering issues described in this chapter, in order to make the implementation efficient.
