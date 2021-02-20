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

 if (argc != 2) {
	  fprintf(stderr,"wrong input: <ip>:<port>\n");
	  exit(1);
	}
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  if(Desthost == NULL || Destport == NULL){
	  fprintf(stderr, "You must enter a IP adress and a Port");
	  exit(1);
  }
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
    int port=atoi(Destport);
	if(port < 1 || port > 65535){
		fprintf(stderr, "Not a valid port! \n");
		exit(1);
	}

  int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	char buf[MAXDATASIZE];

	

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/*if(inet_pton(hints.ai_family, Desthost, buf) < 1)
	{
		fprintf(stderr, "Not a valid IP address!");
		exit(1);
	}*/

	if ((rv = getaddrinfo(Desthost, Destport, &hints, &servinfo)) != 0 ){
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
		fprintf(stderr, "Failed to create socket\n");
		exit(1);
		freeaddrinfo(servinfo);
	}
	
	if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
		close(sockfd);
	  	perror("Failed to connect .\n");
	  	exit(1);
	}

	printf("client: connecting to %s :%d\n", Desthost,port);


	freeaddrinfo(servinfo);

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	  perror("recv");
	  exit(1);
	}
	buf[numbytes -1] = '\0';

	printf("client: received %s\n",buf);
	char command[10];
	char command2[10];
	double f1,f2,fresult;
    int i1,i2,iresult;
	sscanf(buf, "%s %s", command, command2);
		if(strcmp(command, "TEXT ") == 0 && strcmp(command2, "TCP")){
	  		strcpy(buf, "OK\n");

  	  		printf("Sent: %s \n", buf);
	  if ((numbytes = send(sockfd, buf, strlen(buf), 0)) == -1) {
	    perror("sendto:");
	    exit(1);
	  }
		}
		else{
			fprintf(stderr, "NOT OK\n");
			close(sockfd);
			exit(1);
		}
	  buf[numbytes-1] = '\0';
	  if ((numbytes = recv(sockfd, buf, MAXDATASIZE -1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	  }
	  if(numbytes==0){
	    printf("got zero.\n");
		close(sockfd);
		exit(1);
	  }

	 
	  buf[numbytes-1] = '\0';

		printf("client: received %s \n",buf);

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
			printf("Sent %s \n",buf);
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
			  close(sockfd);
			  exit(1);
    		}
			sprintf(buf, "%d\n", iresult);
			send(sockfd, buf, strlen(buf), 0);
			printf("Sent: %s \n",buf);
  			}
			buf[numbytes-1] = '\0';

			if ((numbytes = recv(sockfd, buf, MAXDATASIZE -1, 0)) == -1) {
	    		perror("recv");
	    		exit(1);
	  		}
	  		if(numbytes==0){
	    		printf("got zero.\n");
				close(sockfd);
				exit(1);
	  		}
			buf[numbytes-1] = '\0';
	  		printf("client: received %s\n",buf);
	
	close(sockfd);
	return 0;
  
}
