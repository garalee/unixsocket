#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/wait.h>

#include<errno.h>

#define MAX_SIZE 1024
#define ECHO_PORT 9998

void read_childproc(int sig);

int main(int argc, char* argv[])
{
    int sockfd, connfd;
    char message[MAX_SIZE];
    int str_len;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
  
    pid_t pid;
    struct sigaction act;
    int state;

    /* SIGCHD Handler**************************** */
    /* act.sa_handler=read_childproc; */
    /* sigemptyset(&act.sa_mask); */
    /* act.sa_flags=0; */
    /* state=sigaction(SIGCHLD, &act, 0); */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
	printf("[Echo Server] Socket Error : %s",strerror(errno));
	exit(0);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(ECHO_PORT);
  
    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
	printf("[Echo Server] Bind Error : %s\n",strerror(errno));
	exit(0);
    }

    if(listen(sockfd, 10) == -1)
    {
	printf("[Echo Server] Listen Error : %s\n",strerror(errno));
	exit(0);
    }

    
    client_addr_size = sizeof(client_addr);
    connfd = accept(sockfd, (struct sockaddr*)&clint_addr, &client_addr_size);
    if(connfd == -1){
	printf("[Echo Server] %s\n",strerror(errno));
	exit(0);
    }
	
    while((str_len = read(connfd, message, MAX_SIZE))!=0)
    {
	if(str_len == 0){
	    printf("Received 0 byte\n");
	    break;
	}
	write(connfd, message, str_len);
    }
    close(connfd);
    return 0;
}


/* void read_childproc(int sig) */
/* { */
/*     pid_t pid; */
/*     int status; */
/*     pid = waitpid(-1, &status, WNOHANG); */
/*     printf("process %d finished\n", pid); */
/* } */
