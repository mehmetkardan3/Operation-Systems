#include <stdio.h>
#include <stdlib.h>				// 	Program is tested on Linux 16.4
#include <sys/types.h>			//	To compile: gcc -pthread client.c -o client
#include <sys/ipc.h>				//								Mehmet KARDAN	
#include <sys/msg.h>			
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define SIZE 180
#define MYKEY 150130229
static int flag2 = 0;
static int flag = 1;
static char fifo_name[15] = "FIFO_";

struct messageBuffer{
	char idType[10];
	char messageType[15];
	char message[SIZE];
};

struct inmessageBuffer{
	char idType2[10];
	char messageType2[15];
	char message2[SIZE];
};

void *messageGetter(void *t){
	struct inmessageBuffer abc;
	FILE *file_open;
	while(flag == 1){
		file_open = fopen(fifo_name , "r");
		fgets(abc.message2 , SIZE , file_open);		// reads the FIFO
		fclose(file_open);
		printf("%s\n" , abc.message2);
	}
	flag2 = 1;
	pthread_exit(NULL);
}

int main(){
	pthread_t threadim;
	void *status;
	
	struct messageBuffer msg;
	struct inmessageBuffer abc;	// temp struct for an empty message

	char user_id[10];
	printf("Enter a username:");
	fgets(user_id , 10 , stdin);
	strtok(user_id , "\n");		// getting rid of the "new line"....

	strncpy(msg.idType , user_id , 15);		// login id is copied to structures....
	strncpy(abc.idType2 , user_id , 15);

	strcat(fifo_name , user_id);
	strncpy(msg.messageType , fifo_name , 15);		//	FIFO id is coppied to structures...
	strncpy(abc.messageType2 , fifo_name , 15);

	strncpy(abc.message2 , "" , SIZE);		// first message is the empty string for login request...
		
	mkfifo(fifo_name , 0777);
	printf("%s is created\n" , fifo_name);
	
	int server_qid = msgget(MYKEY , 0);		// connection to the server via the server key...
	FILE *file_open;
	msgsnd(server_qid , &abc , SIZE , 0);
	pthread_create(&threadim , NULL , messageGetter , NULL);
	
	while(1){
		fgets(msg.message , SIZE , stdin);		
		strtok(msg.message ,"\n");
		msgsnd(server_qid , &msg , SIZE , 0);
		if(strcmp(msg.message , "q")==0){
			printf("Log out request sent.\n");
			flag=-1;	// flag to end the thread...
			break;
		}
	}
	while(flag2 = 0);	// flag to make main to wait thread...
	unlink(fifo_name);
}