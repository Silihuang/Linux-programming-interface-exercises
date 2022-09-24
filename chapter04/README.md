# Chapter 04: File I/O: The Universal I/O Model
## Exercise 4-1

The tee command reads its standar input until end-of-file, writing a copy of the input to standard output and to the file named in its command-line argument. (We show an example of the use of this command hwne we discuss FIFOs in Section 44.7.) Implement tee using I/O system calls. By default, tee overwites any existing file with the given name. Impelment the -a command-line option (tee -a file), which casuses te to append text to the end of a file if it already exists. (Refer to Appendix B for desription ofthe getopt() function, which can be used to parse command-line options.)

## Exercise 4-2

Write a program like cp that, when used to copy a regular file that contains holes (sequences of null bytes), also creates correspdonding holes in the target file.
