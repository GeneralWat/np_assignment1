#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
/* You will to add includes here */


// Included to get the support library
#include <calcLib.h>

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass argument during compilation '-DDEBUG'
#define DEBUG

#define BACKLOG 5


using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char *argv[]){
  
  if(argc != 2){
    fprintf(stderr, "Usage; <ip>:<port> \n");
    exit(1);
  }
  
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

    /* Do magic */
  int port=atoi(Destport);

  int sockfd, new_sockfd;//Listening o sockfd, new connection on new_sockfd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_address; //Connectors address info
  socklen_t sin_size = sizeof(their_address);
  struct sigaction sa;
  int yes = 1;
  char s[INET6_ADDRSTRLEN]; //Should the server be able to handle IPV6 address?
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; //Should the server be able to handle IPV6 address?
  hints.ai_socktype = SOCK_STREAM;

  if((rv = getaddrinfo(NULL, Destport, &hints, &servinfo)) != 0){
    fprintf(stderr, "getaddrinfo : %s \n", gai_strerror(rv));
    return 1;//Should the server be able to handle IPV6 address?
  }

  for(p = servinfo; p != NULL; p = p->ai_next){
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      perror("Server: socket");
      continue;
    }
    if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) == -1){
      perror("setsockopt ");
      exit(1);
    }
    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
      close(sockfd);
      perror("Server: bind");
      continue;
    }
    break;
  }
  freeaddrinfo(servinfo);
  #ifdef DEBUG  
  printf("Host %s, and port %d.\n",Desthost,port);
  #endif

  if(p == NULL) {
    fprintf(stderr, "Server failed to bind \n");
    exit(1);
  }

  if(listen(sockfd, BACKLOG) == -1){
    perror("listen");
    exit(1);
  }
  
  char msg[1500];

  int childCount = 0;
  int readsize;
  while(1){
    new_sockfd = accept(sockfd, (struct sockaddr*)&their_address, &sin_size);
    if(new_sockfd == -1){
      perror("accept");
      continue;
    }
    inet_ntop(their_address.ss_family, get_in_addr((struct sockaddr *)&their_address), s, sizeof(s));
    #ifdef DEBUG
      printf("server: Connection %d from %s\n",childCount, s);
    #endif
    struct sockaddr_in *local_sin = (struct sockaddr_in*)&their_address;
    sprintf(msg, "TEXT TCP 1.0\n\n");
    if(send(new_sockfd, &msg, sizeof(msg), 0) == -1){
      perror("send");
      close(new_sockfd);
      continue;
    }
    while(1){


      
    }

  }


  return 0;

}
