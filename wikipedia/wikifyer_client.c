#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFFERMAXLENGTH	(1024*1024)

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFERMAXLENGTH];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");    
    while(!feof(stdin))
    {
	bzero(buffer,BUFFERMAXLENGTH);
    	if(fgets(buffer,BUFFERMAXLENGTH-1,stdin)==NULL)
		break;
    	n = write(sockfd,buffer,strlen(buffer));
    	if (n < 0) 
         	error("ERROR writing to socket");
    	bzero(buffer,BUFFERMAXLENGTH);
    	n = read(sockfd,buffer,BUFFERMAXLENGTH-1);
    	if (n < 0) 
        	 error("ERROR reading from socket");
    	printf("%s",buffer);
    }
    close(sockfd);
    return 0;
}
