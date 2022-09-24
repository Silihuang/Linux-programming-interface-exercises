# Chapter 61: Sockets: Advanced topics

# Exercise 61.13.1

Out-of-band data is a feature of stream sockets that allows a sender to mark transmitted data as high priority; that is, the receiver can obtain notification of the availability of out-of-band data without needing to read all of the intervening data in the
stream. This feature is used in programs such as telnet, rlogin, and ftp to make it possible
to abort previously transmitted commands. Out-of-band data is sent and received
using the MSG_OOB flag in calls to send() and recv(). When a socket receives notification
of the availability of out-of-band data, the kernel generates the SIGURG signal for the
socket owner (normally the process using the socket), as set by the fcntl() F_SETOWN
operation.
When employed with TCP sockets, at most 1 byte of data may be marked as
being out-of-band at any one time. If the sender transmits an additional byte of outof-band data before the receiver has processed the previous byte, then the indication for the earlier out-of-band byte is lost.
TCP’s limitation of out-of-band data to a single byte is an artifact of the mismatch between the generic out-of-band model of the sockets API and its specific implementation using TCP’s urgent mode. We touched on TCP’s urgent
mode when looking at the format of TCP segments in Section 61.6.1. TCP
indicates the presence of urgent (out-of-band) data by setting the URG bit in
the TCP header and setting the urgent pointer field to point to the urgent
data. However, TCP has no way of indicating the length of an urgent data
sequence, so the urgent data is considered to consist of a single byte.
Further information about TCP urgent data can be found in RFC 793.
Under some UNIX implementations, out-of-band data is supported for UNIX
domain stream sockets. Linux doesn’t support this.
The use of out-of-band data is nowadays discouraged, and it may be unreliable in
some circumstances (see [Gont & Yourtchenko, 2009]). An alternative is to maintain
a pair of stream sockets for communication. One of these is used for normal communication, while the other is used for high-priority communication. An application
can monitor both channels using one of the techniques described in Chapter 63.
This approach allows multiple bytes of priority data to be transmitted. Furthermore,
it can be employed with stream sockets in any communication domain (e.g., UNIX
domain sockets).
