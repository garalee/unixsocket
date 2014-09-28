#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#include <stdio.h>
#include <sys/types.h> 

#include <sys/time.h> 
#include <time.h>		/* timespec{} for pselect() */
#include <errno.h>

#define TIME_PORT 9997
#define	MAX_SIZE 1024   


int main(int argc, char** argv)
{
    int sockfd;
    char message[MAX_SIZE];
    int str_len;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
	printf("[TIME] Socket Error : %s\n",strerror(errno));
	exit(0);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TIME_PORT);
    
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0){
	printf("[TIME] inet_pton Error : %s\n",strerror(errno));
	exit(-1);
    }
	
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
	printf("[TIME] Connection Error : %s\n",strerror(errno));
	exit(0);
    }
    
    printf("[TIME] Connection Established\n");
    while(1)
    {  
	str_len = read(sockfd,message, MAX_SIZE);
	if(str_len > 0){
	    message[str_len] = 0;
	    printf("[TIME] %s\n",message);
	    /* if(fputs(recvline, stdout) == EOF) */
	    /* 	err_handling("fputs error"); */
	}
	/* if(str_len < 0) */
	/*     err_handling("read error"); */
    }
    
    return 0;
}
