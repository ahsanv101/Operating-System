#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>


void *sendtoserve(void *sock)
{
	int client_sock = *((int *)sock); //socket for sending messages.
	char msg[500]; //character array of messages to be sent to server

	printf("Enter your command:\n");

	while(fgets(msg,500,stdin) > 0) //will keep on working as input is greater than 0

	{

		if (strncmp(msg,"/list",strlen(msg)-1) == 0)//this will look after our list command and see if we wrote /list or not through the 
				{                           //function string compare and if it is that then it wll right /list to server to be handled later
				int len = write(client_sock,msg,strlen(msg));
				if(len < 0) 
					{
					perror("Message not sent\n");
					exit(1);
					}
				}

		else if (strncmp(msg,"/msg",strlen("/msg")) == 0)//similar for /msg. If we wrote messages then it will write it to server and then server will handle it accordingly
				{
				int len = write(client_sock,msg,strlen(msg));
				if(len < 0) 
					{
					perror("Message not sent\n");
					exit(1);
					}
				}

		else if (strncmp(msg,"/quit",(strlen(msg)-1)) == 0)//if we write quit then it wont write anything to server.
				{
					printf("exiting\n");
					exit(1);
				}

		else
				{
					printf("Error 404\n");//error given if wring command is inputed
				}

		memset(msg,'\0',sizeof(msg)); //ending the msg character array (whatever it is) with a NUL character.
	}
}








void *recvmg(void *sock) //this function is used for receiving messages from server.
{
	int server_socket = *((int *)sock); //for the socket to receive
	char msg[500];  //character array of each message
	while(read(server_socket,msg,sizeof(msg)) > 0){  //until we are receiving messages, we will keep on printing them.
		printf("%s\n",msg);              
		memset(msg,'\0',sizeof(msg));//and then ending the array of char with NUL(\0) to display as a string.
	}
}



int main(int argc, char *argv[])
{

//In the first part we will make all the necessary variables. and store the port number and username from the argument.
	struct sockaddr_in serv_addr;
	int my_sock;
	//int their_sock;
	//int their_addr_size;
	int portno;
	pthread_t sendt,recvt;
	char msg[500];
	char username[100];
	//char res[600];
	char ip[INET_ADDRSTRLEN];
	//int len;

	//in_addr_t t = inet_addr(argv[1]);


	strcpy(username,argv[3]);
	portno = atoi(argv[2]);


//In the second part we will start to establish socket connection.

	my_sock = socket(AF_INET,SOCK_STREAM,0);
	if (my_sock < 0) {
		perror("Error establishing socket\n");
		exit(1);
	}
	//memset(serv_addr.sin_zero,'\0',sizeof(serv_addr.sin_zero));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno); //converts port number in byte order
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);//copies the ip address argument given by client.
	memset(serv_addr.sin_zero,'\0',sizeof(serv_addr.sin_zero));
    	//bcopy((char *) &t,(char *)&serv_addr.sin_addr.s_addr,sizeof(in_addr_t));







//in the third part we will start to connect socket

	if(connect(my_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
	{
		perror("connection not esatablished");
		exit(1);
	}
	inet_ntop(AF_INET, (struct sockaddr *)&serv_addr, ip, INET_ADDRSTRLEN);
	//printf("connected to %s, start chatting\n",ip);
	//pthread_create(&recvt,NULL,recvmg,&my_sock);
	
	
	write(my_sock,username,strlen(username)); //as soon as the connection is established we will write the clients username to server so that server can save this in its list of clients to be refered to in the future.

	memset(msg,'\0',sizeof(msg));//ofcourse like always, we have to end it with a null character to make it a string
	//memset(username,'\0',sizeof(username));
	if (strncmp(msg,"false",strlen("false")) == 0)//here we will make sure that username shouldnt repeat
	{		
		printf("username taken\n");
		exit(1);
	}
	
	memset(msg,'\0',strlen(msg));
	printf("%s connected to server\n","You are");

//In the fourth part we will create threads for sending and receiving messages and then joining them

	pthread_create(&recvt,NULL,recvmg,&my_sock); //using receiving function
	pthread_create(&sendt,NULL,sendtoserve,&my_sock); //using send function
	pthread_join(recvt,NULL);
	pthread_join(sendt,NULL);
	close(my_sock);

}
