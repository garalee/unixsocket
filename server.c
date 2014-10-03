#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

#include<time.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<signal.h>
#include<pthread.h>

#define ECHO_PORT 9998
#define TIME_PORT 9997

#define MAX_LISTEN 10
#define MAX_SIZE 1024

void sigpipe_handler(int sig){
    printf("Client Has Been Closed Connection\n");
}

/* Socket Manager Variables */
int sockets[MAX_LISTEN];	/* a list of socket descriptors, -1 as value if it's empty */
pthread_t working_thread_t[MAX_LISTEN]; /* Working Thread id */
struct sockaddr_in client_addrs[MAX_LISTEN]; /* a list of address struct for each connection */
socklen_t client_addr_size;	/* Size of (struct client_addr_size) */
unsigned int socket_counter = 0; /* The number of sockets established connection */
/* ******************************** */

pthread_mutex_t lock;
/* Socket Manager helper functions */
/* 
 * These helper functions manage socket descriptors
 * and keep track of threads corresponding to each socket descriptor.
 * variables above must be accessed by functions who have SKMANAGER prefix.
 * */
int SKMANAGER_get_available();	/* get available socket descriptor from "sockets" list */
void SKMANAGER_set_socket(int id,int sockfd); /* set socket descriptor on the list */
void SKMANAGER_reset_socket(int id);	    /* set -1 on sockets[i] */

void* echo_process(void* arg);
void* time_process(void* arg);

int main(void){
    int fd;
    int timefd,echofd;
    struct sockaddr_in time_addr,echo_addr;
    int available_id;
    int i;

    fd_set accept_request;
    int result;
    
    struct sigaction sa;

    memset(&sa,0,sizeof(sa));
    sa.sa_handler = sigpipe_handler;
    sigaction(SIGPIPE,&sa,NULL);

    if(pthread_mutex_init(&lock,NULL) != 0){
	printf("[SERVER] mutex init failed : %s\n",strerror(errno));
	exit(0);
    }

    /* INIT Socket Manager */
    for(i=0;i<MAX_LISTEN;++i)sockets[i] = -1;
    client_addr_size = sizeof(struct sockaddr_in);
    /* ******************* */

    /* Prepare server for request */
    timefd = socket(AF_INET,SOCK_STREAM,0);
    echofd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&time_addr,sizeof(struct sockaddr_in));
    bzero(&echo_addr,sizeof(struct sockaddr_in));

    time_addr.sin_family = AF_INET;
    echo_addr.sin_family = AF_INET;

    time_addr.sin_port = htons(TIME_PORT);
    echo_addr.sin_port = htons(ECHO_PORT);
    
    time_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    echo_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(timefd,(struct sockaddr*)&time_addr,sizeof(struct sockaddr_in)) <0){
	printf("[TIME] Bind Error : %s\n",strerror(errno));
	exit(-1);
    }

    if (bind(echofd,(struct sockaddr*)&echo_addr,sizeof(struct sockaddr_in)) <0){
	printf("[ECHO] Bind Error : %s\n",strerror(errno));
	exit(-1);
    }

    listen(echofd,MAX_LISTEN);
    listen(timefd,MAX_LISTEN);

    printf("[SERVER] Server is Listening....\n");
    
    /* Service Begins */
    while(1){
	/* Multiplexing using Select*/
	FD_ZERO(&accept_request);
	FD_SET(echofd,&accept_request);
	FD_SET(timefd,&accept_request);
	FD_SET(1,&accept_request);
	result = select((echofd>timefd?echofd:timefd) + 1,&accept_request,0,0,NULL);

	/* Error Handler */
	if(result == -1){
	    printf("[Server] function \"select\" error : %s\n",strerror(errno));
	    continue;
	}
	
	/* Handling Echo Request */
	if(FD_ISSET(echofd,&accept_request)){
	    printf("[SERVER] Echo Service Requested\n");
	    available_id = SKMANAGER_get_available();
	    if(available_id == -1){
		printf("[SERVER] Couldn't Afford to accept new reques\n",strerror(errno));
		/* Send Connection Refuse */
		exit(0);
	    }
	    

	    /* Accept Funcion Error Handling */
	    fd = accept(echofd,(struct sockaddr*)&client_addrs[available_id],&client_addr_size);
	    if( fd < 0){
		printf("[SERVER] Accept Error : %s",strerror(errno));
		continue;
	    }
	    
	    pthread_mutex_lock(&lock);
	    SKMANAGER_set_socket(available_id,fd);
	    pthread_mutex_unlock(&lock);
	    if(pthread_create(&working_thread_t[available_id],NULL,echo_process,(void*)&available_id) < 0){
		printf("[SERVER] pthread Error : %s\n",strerror(errno));
		exit(0);
	    }
	}

	/* Handler Time Request */
	if(FD_ISSET(timefd,&accept_request)){
	    printf("[SERVER] Time Service Requested\n");
	    available_id = SKMANAGER_get_available();
	    if(available_id == -1){
		printf("[SERVER] Couldn't Afford to accept new request : %s\n",strerror(errno));
		/* Send Connection Refuse */
	    }

	    fd = accept(timefd,(struct sockaddr*)&client_addrs[available_id],&client_addr_size);
	    if( fd < 0){
		printf("[SERVER] Accept Error : %s",strerror(errno));
		continue;
	    }
	    
	    pthread_mutex_lock(&lock);	
	    SKMANAGER_set_socket(available_id,fd);
	    pthread_mutex_unlock(&lock);

	    if(pthread_create(&working_thread_t[available_id],NULL,time_process,(void*)&available_id) < 0){
		printf("[SERVER] pthread Error : %s\n",strerror(errno));
		exit(0);
	    }
	}
    }

    close(echofd);
    close(timefd);

    return 0;
}	


void* time_process(void* arg){
    int id;
    int sockfd;
    time_t t;
    char message[MAX_SIZE];
    int client_terminated=0;


    id=*((int*)arg);
    sockfd = sockets[id];

    pthread_detach(pthread_self());
    /* **************************************************************** */
    /* TODO
     * No Error Handler.
     * Think about Error Scinario
     * Signal Handler to break while loop to terminate server
     * */


    /* TO HANDLE ERROR */
    /* Time Server doesn't read from client so termination request from client
     * should be handled by reading from client.
     * */

    while(1){
	t = time(NULL);
	snprintf(message, sizeof(message), "%.24s", ctime(&t));
	client_terminated = write(sockfd, message, strlen(message));
	/* If client is terminated while server is still writing on, quit */
	if(client_terminated <= 0){
	    printf("[TIME] Client Has Been Terminated : %s Handled\n",strerror(errno));
	    break;
	}

	sleep(5);

    }
    
    printf("id:%d connection closed\n",id);

    pthread_mutex_lock(&lock);
    SKMANAGER_reset_socket(id);
    pthread_mutex_unlock(&lock);

    close(sockfd);
    return NULL;
}
    
void* echo_process(void* arg){
    int id;
    int sockfd;
    int str_len;
    int client_terminated;
    char message[MAX_SIZE];

    id = *((int*)arg);
    sockfd = sockets[id];
    pthread_detach(pthread_self());
    /* ************** */
    /* **************************************************************** */
    /* TODO
     * No Error Handler.
     * Think about Error Scenario
     * Signal Handler to break while loop to terminate server
     * */
    while(1){
	str_len = read(sockfd,message,MAX_SIZE);
	if(str_len == 0){
	    printf("[ECHO] EOF TERMINATION : %s\n",strerror(errno));
	    break;
	}

	if(str_len < 0){
	    printf("[ECHO] Client Has Been Terminated : %s Handled\n",strerror(errno));
	    break;
	}

	message[str_len] = 0;
	printf("[SERVER] ECHO MESSAGE from ID%d : %s",id,message);
	client_terminated = write(sockfd, message, str_len);
	if(client_terminated < 0){
	    printf("[ECHO] Client Has Been Terminated : %s Handled\n",strerror(errno));
	    break;
	}

    }
    
    printf("ID:%d connection closed\n",id);
    close(sockfd);
    SKMANAGER_reset_socket(id);   
    return NULL;
}


int SKMANAGER_get_available(){
    int i;
    for(i=0;i<MAX_LISTEN;++i){
	if(sockets[i] == -1)
	    return i;
    }
    return -1;
}

void SKMANAGER_set_socket(int id,int sockfd){
    sockets[id] = sockfd;
    socket_counter++;
}

void SKMANAGER_reset_socket(int id){
    sockets[id] = -1;
    socket_counter--;
}
