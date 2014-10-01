#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>

#include<signal.h>

#include<errno.h>

#define ECHO_PORT 9998
#define MAX_SIZE 1024

int sockfd;
int pipefd;

void sigint_handler(int sig){
    char temp[MAX_SIZE];
    sprintf(temp,"[ECHO] Process %ld is terminated normally\n",getpid());
    write(pipefd,temp,strlen(temp));
    close(sockfd);
    exit(0);
}

int main(int argc, char *argv[])
{
    char message[MAX_SIZE];
    char temp[MAX_SIZE];
    int str_len=argc;
    struct sockaddr_in server_addr;
    struct sigaction sa;	/* SIGINT signal handler */
    int server_terminated;

    memset(&sa,0,sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT,&sa,NULL);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    pipefd = atoi(argv[2]);

    if(sockfd == -1){
	sprintf(temp,"[ECHO] Socket Error : %s",strerror(errno));
	write(pipefd,temp,strlen(temp));
	exit(-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ECHO_PORT);

    /* ERROR HANDLING
     * */
    if(inet_pton(AF_INET,argv[1],&server_addr.sin_addr) < 0){
	sprintf(temp,"[ECHO] inet_pton Error : %s",strerror(errno));
	/* THIS STATEMENT MAKES CLIENT FALL IN INFINITE LOOP */
	write(pipefd,temp,strlen(temp));
	exit(-1);
    }
    
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
	sprintf(temp,"[ECHO] Connection Error : %s\n",strerror(errno));
	write(pipefd,temp,strlen(temp));
	exit(0);
    }


    /* ****************************************************** */
    /* ****************Echo Service ************************* */
    /* ****************************************************** */
    /* TODO in Echo Service
     * Close socket normally when EOF requested
     * Handler that handles reading data from server more than it can recieve.
     * Sending Message that recieved from server to its parent process(Need PIPE). 
     * 
     */

    printf("[ECHO] Connection Established\n");
    while(1)
    {
	printf("[ECHO] command(case-sensitive): ");
	fflush(STDIN_FILENO);
	
	if(!fgets(message,MAX_SIZE,stdin)){
	    break;
	}
	server_terminated = write(sockfd, message, strlen(message));
	if(server_terminated < 0){
	    sprintf(temp,"[ECHO] Server Has Crashed. Please Connect few seconds later : %s\n",strerror(errno));
	    write(pipefd,temp,strlen(temp));
	    break;
	}
	str_len = read(sockfd, message, MAX_SIZE);
	if(str_len <= 0){
	    sprintf(temp,"[ECHO] Server Has Crashed. Please Connect few seconds later : %s\n",strerror(errno));
	    write(pipefd,temp,strlen(temp));
	    break;
	}
	message[str_len] = 0;
	printf("Message from server: %s", message);
    }

    
    sprintf(temp,"[ECHO] Service Terminated : Process ID(%ld)\n",getpid());
    write(pipefd,temp,strlen(temp)); /* ERROR HANDLE? */
    close(sockfd);
    
    return 0;
}
