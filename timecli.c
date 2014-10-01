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

#include<signal.h>

#define TIME_PORT 9997
#define	MAX_SIZE 1024   


int pipefd;
int sockfd;

void sigint_handler(int sig){
    char temp[MAX_SIZE];
    sprintf(temp,"[TIME] Process %ld is terminated normally\n",getpid());
    write(pipefd,temp,strlen(temp));
    close(sockfd);
    exit(0);
}
    

int main(int argc, char** argv)
{
    char message[MAX_SIZE];
    char temp[MAX_SIZE];
    int str_len;
    struct sockaddr_in server_addr;

    struct sigaction sa;	/* SIGINIT signal handler */

    /* Setting Signal Handler */
    memset(&sa,0,sizeof(sa));
    sa.sa_handler=sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    pipefd = atoi(argv[2]);

    if(sockfd == -1){
	sprintf(temp,"[TIME] Socket Error : %s\n",strerror(errno));
	write(pipefd,temp,strlen(temp));
	exit(-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TIME_PORT);
    
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) < 0){
	sprintf(temp,"[TIME] inet_pton Error : %s\n",strerror(errno));
	/* THIS STATEMENT MAKES CLIENT.c FALL IN INFINITE LOOP */
	write(pipefd,temp,strlen(temp));
	exit(-1);
    }
	
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
	sprintf(temp,"[TIME] Connection Error : %s\n",strerror(errno));
	write(pipefd,temp,strlen(temp));
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

	/* TODO ERROR HANDLING */
	if(str_len <= 0){
	    sprintf(temp,"[TIME] Server Has Crashed. Please Connect few seconds later : %s\n",strerror(errno));
	    write(pipefd,temp,strlen(temp));
	    break;
	}
    }

    sprintf(temp,"[TIME] Service Terminated : Process ID(%ld)\n",getpid());
    write(pipefd,temp,strlen(temp));
    close(sockfd);

    return 0;
}
