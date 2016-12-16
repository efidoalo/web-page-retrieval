/*=====================================================;
 *
 * File: webrequests.c
 * Content: send and receive http request and response 
 * messages from web servers using http protocol to
 * read webpages and interact with web servers.
 * Date: 5/12/2016
 *
 ******************************************************/
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFF_SIZE 80000

int main()
{ 
  /* obtain pointer to memory block of BUFF_SIZE bytes, this character 
  is used for storing the TCP payload of messages */
  char buf[BUFF_SIZE];
  int msglen =0;        // used to indicate number of bytes in message

  int clsock_fd = 0;  // client socket file descriptor
  if ((clsock_fd=socket(AF_INET, SOCK_STREAM, 0))==-1) {
    printf("\nerror creating socket. %s", strerror(errno));
    return 0;
  }

  // allow reconnection using same ports and ipv4 addr
  int sockoptsval=1;
  setsockopt(clsock_fd, SOL_SOCKET, SO_REUSEADDR, &sockoptsval, sizeof(int));
  setsockopt(clsock_fd, SOL_SOCKET, SO_REUSEPORT, &sockoptsval, sizeof(int)); 
  
  // fill sockaddr_in struct with client machine address data
  struct sockaddr_in client_addr;
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = 2417;
  if (inet_aton("192.168.1.109", &client_addr.sin_addr)==-1) {
    printf("\nerror converting local ip to network byte order");
    return 0;
  }
  // bind clsock_fd socket to client machine address data.
  if (bind(clsock_fd, (struct sockaddr *)&client_addr, 
                  sizeof(struct sockaddr))==-1) {
    printf("\nerror binding local client endpoint address to socket. %s",
    strerror(errno));
  }
  
  /* website domain name, addrinfo struct hints is used in the call
  to getaddrinfo to restrict any associated server machine endpoint
  returned by the call to be IPv4/TCP capable. */
  const char *host = "www.lysator.liu.se"; 
  struct addrinfo hints = {0, AF_INET, SOCK_STREAM, IPPROTO_TCP,
                           0,0,0,0}, *res;

  /* getaddrinfo returns a unidirectional linked list of server machine
  endpoints that host the website specified by the domain name given
  by the null terminated string host. */ 
  int retval=0;
  if ((retval = getaddrinfo(host, "http", &hints, &res))!=0) {
    printf("\nerror obtaining address information for %s:http. %s",
       host, gai_strerror(retval));
    return 0;
  }
  
  // Connect to server machine endpoint.
  while (res) { 
    if (connect(clsock_fd, res->ai_addr,sizeof(struct sockaddr))==0) 
      break;
    res=res->ai_next;
  }
  if (!res) {
    printf("\ncould not connect to any server endpoint.");
    return 0;
  }
 
  
  const char *serverOPTS = "GET /c/pikestyle.html HTTP/1.1\r\n"
   "Host:www.lysator.liu.se\r\n\r\n";
  if ((msglen = send(clsock_fd, memcpy(buf, serverOPTS, strlen(serverOPTS)), 
            strlen(serverOPTS), 0))==-1) {
    printf("\nerror sending OPTIONS request to server. %s", strerror(errno));
    return 0;
  }  
  
  printf("\n");
  // receive response message from server, outputting the mesage to the terminal.
  while ( (msglen = recv(clsock_fd, buf, BUFF_SIZE,0))>0 ) {
    buf[msglen]=0;
    printf("%s",buf);
    if (!recv(clsock_fd, buf, BUFF_SIZE, MSG_PEEK))
      break;
  }
}

