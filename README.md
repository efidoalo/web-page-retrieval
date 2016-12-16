# web-page-retrieval
A short C program that uses linux sockets to connect to a web server using IPv4/TCP on port 80 to request a particular web page.
The IPv4 address and port number of the local machine endpoint are specified by the user, the web server address information
is obtained via DNS lookup implicitly within the getaddrinfo() call (see linux man page getaddrino(3)). The http get request 
sent to the web server must be specified in full by the program user.
