#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
/* You will to add includes here */
#include <calcLib.h>
// Enable if you want debugging to be printed, see examble below.
// Alternative, pass 
#define DEBUG

#define MAXDATASIZE 1400


// Included to get the support library
#include <calcLib.h>

int main(int argc, char *argv[]){

  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  int port=atoi(Destport);
  int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	char buf[MAXDATASIZE];

	if (argc != 2) {
	  fprintf(stderr,"usage: %s hostname (%d)\n",argv[0],argc);
	  exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(Desthost, Destport, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	if (connect(sockfd,p->ai_addr, p->ai_addrlen) < 0 ) {
	  perror("talker2: connect .\n");
	  exit(1);
	}

	printf("client: connecting to %s :%d\n", Desthost,port);


	freeaddrinfo(servinfo);

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	  perror("recv");
	  exit(1);
	}
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);
	char command[10];
	double f1,f2,fresult;
    int i1,i2,iresult;
	  strcpy(buf, "OK\n");

	  #ifdef DEBUG 
  		printf("Sent: %s \n", buf);
	  #endif
	  if ((numbytes = send(sockfd, buf, strlen(buf), 0)) == -1) {
	    perror("sendto:");
	    exit(1);
	  }
	  buf[numbytes] = '\0';
	  if ((numbytes = recv(sockfd, buf, MAXDATASIZE -1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	  }
	  if(numbytes==0){
	    printf("got zero.\n");
	  }

	 
	  buf[numbytes] = '\0';

		printf("client: received '%s/%d'\n",buf,numbytes);

	    rv=sscanf(buf,"%s",command);
  
  		if(command[0]=='f'){
    		rv=sscanf(buf,"%s %lg %lg",command,&f1,&f2);
    		if(strcmp(command,"fadd")==0){
      		fresult=f1+f2;
    		} else if (strcmp(command, "fsub")==0){
      		fresult=f1-f2;
    		} else if (strcmp(command, "fmul")==0){
       		fresult=f1*f2;
    		} else if (strcmp(command, "fdiv")==0){
      		fresult=f1/f2;
			
    	}
			sprintf(buf, "%8.8g\n", fresult);
			send(sockfd, buf, strlen(buf), 0);
  			} else {
    		rv=sscanf(buf,"%s %d %d",command,&i1,&i2);
    		if(strcmp(command,"add")==0){
      		iresult=i1+i2;
    		} else if (strcmp(command, "sub")==0){
     		 iresult=i1-i2;
     	 	printf("[%s %d %d = %d ]\n",command,i1,i2,iresult);
    		} else if (strcmp(command, "mul")==0){
      		iresult=i1*i2;
    		} else if (strcmp(command, "div")==0){
      		iresult=i1/i2;
		
			} else {
     	 	printf("No match\n");
    		}
			sprintf(buf, "%d\n", iresult);
			#ifdef DEBUG
			printf("Sent %d \n", iresult);
			#endif
			send(sockfd, buf, strlen(buf), 0);
  			}
			buf[numbytes] = '\0';

			if ((numbytes = recv(sockfd, buf, MAXDATASIZE -1, 0)) == -1) {
	    		perror("recv");
	    		exit(1);
	  		}
	  		if(numbytes==0){
	    		printf("got zero.\n");
				exit(1);
	  		}
			buf[numbytes] = '\0';
	  		printf("client: received '%s/%d'\n",buf,numbytes);
	
	close(sockfd);
	return 0;
  
}
