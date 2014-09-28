#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<signal.h>
#include<sys/wait.h>

#include<errno.h>
#include<pthread.h>

#define MAX_SIZE 1024
#define ECHO_PORT 9998

#define MAX_LISTEN 10


int sockets[MAX_LISTEN];
int listen_flag[MAX_LISTEN];
pthread_t listen_thread_t[MAX_LISTEN];
struct sockaddr_in client_addrs[MAX_LISTEN];
socklen_t client_addr_size;
unsigned int socket_counter = 0;



void* echo_process(void* arg);

int SKMANAGER_get_available(){
    int i;
    for(i=0;i<MAX_LISTEN;++i){
	if(listen_flag[i] == 0)
	    return i;
    }

    return -1;
}

void SKMANAGER_set_socket(int i){
    listen_flag[i] = 1;
    socket_counter++;
}

void SKMANAGER_reset_socket(int i){
    listen_flag[i] = 0;
    socket_counter--;
}
    
    

int main(void)
{
    int sockfd, connfd;
    struct sockaddr_in server_addr;

    int i,available;

    /* SIGCHD Handler**************************** */
    /* act.sa_handler=read_childproc; */
    /* sigemptyset(&act.sa_mask); */
    /* act.sa_flags=0; */
    /* state=sigaction(SIGCHLD, &act, 0); */

    for(i=0;i<MAX_LISTEN;++i){
	listen_flag[i] = 0;
	bzero(&client_addrs[i],sizeof(struct sockaddr_in));
    }

    /* ******************************************************************** */
    /* Socket Initialization ************************************************/
    /* ******************************************************************** */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
	printf("[Echo Server] Socket Error : %s",strerror(errno));
	exit(0);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(ECHO_PORT);
  
    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
	printf("[Echo Server] Bind Error : %s\n",strerror(errno));
	exit(0);
    }

    if(listen(sockfd, 10) == -1){
	printf("[Echo Server] Listen Error : %s\n",strerror(errno));
	exit(0);
    }

    
    client_addr_size = sizeof(client_addrs[0]);


    /* WATCHOUT
     * Requesting more than server can take */
    while(1){
	available = SKMANAGER_get_available();
	sockets[available] = accept(sockfd,(struct sockaddr*)&client_addrs[available],&client_addr_size);
	printf("%d established\n",available);
	if(sockets[available] == -1){
	    printf("[Echo Server] %s\n",strerror(errno));
	    SKMANAGER_reset_socket(available);
	}

	pthread_create(&listen_thread_t[available],NULL,echo_process,(void*)&available);
    }

    close(connfd);
    return 0;
}


void* echo_process(void* arg){
    int thread_id;
    int sockfd;
    int str_len;
    char message[MAX_SIZE];

    thread_id = *((int*)arg);
    sockfd = sockets[thread_id];
    
    pthread_detach(pthread_self());
    /* **************************************************************** */
    /* TODO
     * No Error Handler.
     * Think about Error Scinario
     * Signal Handler to break while loop to terminate server
     * */
    while((str_len = read(sockfd, message, MAX_SIZE))!=0)
	write(sockfd, message, str_len);

    printf("%d connection closed\n",thread_id);
    close(sockfd);
    SKMANAGER_reset_socket(thread_id);

    return NULL;
}
  
