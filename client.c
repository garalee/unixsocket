#include<stdio.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<errno.h>
#include<signal.h>
#include<sys/wait.h>

#define MAX_SIZE 1024

#define ECHO_PORT 9998
#define TIME_PORT 9997

void printHost(char* argv,char* ip_address);
void sigchld_handler(int sig);

int main(int argc,char* argv[]){
    
    char input[MAX_SIZE];	/* input data from user */
    int input_len;		/* input length from user*/

    char message[MAX_SIZE];	/* Message from pipe(child processes */
    int msg_len;

    char ip_address[64];	/* server ip address(presentation format) */

    int fds[2];			/* ipc pipe fds[0] for reading fds[1] for writing */
    fd_set ipc;			/* inter process communication pipe */
    pid_t cp;			/* child process */
    int result;			/* result of "select" function */

    struct sigaction sa;	/* signal handler */
    
    char temp[8];		/* temporary char */

    /* Error Check the Number of Arguments */
    if( argc != 2 ){
	printf("Usage: client <hostname or ip address>\n");
	exit(0);
    }

    printHost(argv[1],ip_address);		/* Print Out Peer Information */
    
    /* ******************************************************************* */
    /**************** Client Service ************************************* */
    /* ******************************************************************* */

    /* TODO in Client Service:
     * print data sent from child process (need pipe)
     * what if server is not listening for new incoming request.
     *
     * */
    pipe(fds);			/* Inter Process Communication between child and parent process */
    sprintf(temp,"%d",fds[1]);

    memset(&sa,0,sizeof(sa));	/* Signal Handler */
    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD,&sa,NULL);

    while(1){ 
	FD_ZERO(&ipc);
	FD_SET(0,&ipc);
	FD_SET(1,&ipc);
	FD_SET(fds[0],&ipc);

	result = select(fds[0] +1,&ipc,0,0,NULL);
	
	if(result == -1){
	    continue;
	}
	
	if(FD_ISSET(0,&ipc)){
	    /* TODO ERROR HANDLER
	     * fgets error*/
	    
	    fgets(input,MAX_SIZE,stdin); 
	    /* TODO
	     *
	     * Sending termination signal to all of children processes */
	    if(strcmp("echo\n",input) == 0){
		printf("Echo Service Requested\n");
		cp = fork();
		if(cp == 0){	// Child Process
		    close(fds[0]);
		    execlp("xterm","xterm","-e","./echocli",ip_address,temp,(char*) 0);
		    exit(0);
		}
	    }else if(strcmp("time\n",input) == 0){
		printf("Time Service Requested\n");
		cp = fork();     
		if(cp == 0){
		    close(fds[0]);
		    execlp("xterm","xterm","-e","./timecli",ip_address,temp,(char*)0);
		    exit(0);
		}
	    }else if(strcmp("quit\n",input) == 0){
		/* Error Handler Required
		 * Expected Error:trying to close socket while few other process is still running
		 *                trying to close while server is closed already
		 */
		break;
	    }else{
		char *p = strtok(input,"\n");
		printf("command %s is not valid\n",p);
	    }
	}else if(FD_ISSET(fds[0],&ipc)){
	    if((msg_len=read(fds[0],message,MAX_SIZE))>0){
		message[msg_len] = 0;
		printf("%s",message);
	    }else{
		printf("PIPE LINE ERROR %s\n",strerror(errno));
	    }
	}
    }
    /* ******************************************************Client Service */
    
    close(fds[0]);
    close(fds[1]);
    printf("Program Exited Normally\n");
    return 0;
}


/* TODO ERROR HANDLING
 *
 * If two or more processes are terminated simultaneously
 * one or more SIGCHLD signal might be overlapped.
 * Handler this !!
 * */
void sigchld_handler(int sig){
    pid_t cp;
    int status;

    while((cp=waitpid(-1,&status,WNOHANG)) > 0){
	printf("[CLIENT] process %ld terminated normally\n",cp);
    }	    
}

void printHost(char* host,char* ip_address){
    in_addr_t addr;
    struct hostent *hp;
    struct in_addr in_addr;

    char** pptr;
    
    printf("** Host Information **\n");
    if((int)(addr = inet_addr(host)) == -1){
	hp = gethostbyname(host);
	if( hp != NULL){
	    //in_addr = *(struct in_addr*)(hp->h_addr);
	    printf("Official Server Host Name :  %s\n",hp->h_name);
	    for(pptr = hp->h_addr_list; *pptr != NULL ; ++pptr){
		memcpy(&in_addr.s_addr,*pptr,sizeof(in_addr.s_addr));
		printf("The IP address of Server Host(%s) : %s\n",host,inet_ntoa(in_addr));
	    }
	}else{
	    printf("%s was not resolved\n",host);
	}
    }else{
	inet_pton(AF_INET,host,&in_addr);
	hp = gethostbyaddr((void*)&in_addr,sizeof(in_addr),AF_INET);
	if ( hp != NULL){
	    printf("Official Server Host Name: %s\n",hp->h_name);
	    
	    for(pptr = hp->h_addr_list;*pptr!=NULL;++pptr){
		memcpy(&in_addr.s_addr,*pptr,sizeof(in_addr.s_addr));
		printf("The Host Name of Server Host(%s) : %s\n",host,inet_ntoa(in_addr));
	    }
	}else{
	    printf("Host Information for %s not found\n",host);
	}
    } 
}
