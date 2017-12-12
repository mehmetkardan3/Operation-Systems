#include <stdio.h>
#include <stdlib.h>				// 	Program is tested on Linux 16.4
#include <pthread.h>				// 	To compile:  gcc -pthread server.c -o server
#include <sys/types.h>			//								Mehmet KARDAN
#include <sys/ipc.h>				
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#define MYKEY 150130229
#define SIZE 180
static int flag = 1;
static int totalUser = 0;	//	static var. that stores number of users....

struct messageBuffer{
	char idType[10];		// gets the id name....
	char messageType[15];	//gets the FIFO name...
	char message[SIZE];	//gets the message....
};

void *messageGetter(void *t){
	int counter;		// counter for the receive an array element....
	struct messageBuffer newmsg;
	int queue_id = msgget(MYKEY,IPC_CREAT|0777);
	FILE *file_open;
	
	if(queue_id > 0)	printf("Message Queue is generated \n");
	else	printf("Queue is not created. \n");

	char receiveList[100][SIZE];	// string array to build up receive list...

	while(flag == 1){
		msgrcv(queue_id, &newmsg, SIZE, 0, 0);
	
		if(strcmp(newmsg.message , "")==0){
			printf("'%s' is connected\n" , newmsg.idType);
			strcpy(receiveList[totalUser] , newmsg.messageType);
			totalUser++;		
		}
	
		else{
			strtok(newmsg.message,"\n");

			if(strcmp(newmsg.message , "q") == 0){	
				printf("'%s' is disconnected. Removing '%s'\n",newmsg.idType,newmsg.messageType);
				for(counter=0; counter < totalUser; counter++){
					if(strcmp(newmsg.messageType , receiveList[counter]) == 0){
						strcpy(receiveList[counter] , "");
					}
				} 
			}

			else{	
				for(counter=0; counter < totalUser; counter++){
					if(strcmp(newmsg.messageType , receiveList[counter]) == 0){
						file_open = fopen(newmsg.messageType , "w");
						fputs("Message sent." , file_open);
						fclose(file_open);	
					}

					else if(strcmp(newmsg.messageType , receiveList[counter]) != 0){
						file_open = fopen(receiveList[counter] , "w");
						if(file_open != NULL){
							fputs(newmsg.idType, file_open);
							fputs(":",file_open);
							fputs(newmsg.message, file_open);	
							fclose(file_open);
						}
					}
				}
			}
		}
	}

	pthread_exit(NULL);
}


void main(){
	char choice[10];
	pthread_t threadim;
	pthread_create(&threadim , NULL , messageGetter , NULL);
	int queue_id = msgget(MYKEY,IPC_CREAT|0777);
	while(flag == 1){
		fgets(choice,10,stdin);
		strtok(choice,"\n");		// controls if "q" is entered...
		if(strcmp(choice,"q")==0)	flag=0;
	}
	
	printf("Message queue is ");
	if(msgctl(queue_id, IPC_RMID, 0) ==0)	printf("removed\n");
	else	printf("could not be removed");	
	printf("Server connection closed!\n");

}