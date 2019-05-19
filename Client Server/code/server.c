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

typedef struct {
	int sockno;
	char ip[INET_ADDRSTRLEN];
	char username[100];
} clinfo; //we will create a typedef structure so that we dont have to write 'struct' everytime we make a new object (as we have to make arrays of it) and to save information about the clients connected. We will have their socket, ip address and username



int total=20; //we will now have a total capacity of 50 clients
int n = 0;  //counter
clinfo client[20];//we will create an array of 50 client information objects
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//for synchronization


char *FirstWord(char *line)
{
char *word = strtok(line," ");
return word;
}
/*
void sendtoall(char *msg,int curr)
{
	int i;
	pthread_mutex_lock(&mutex);
	for(i = 0; i < n; i++) {
		if(client[i] != curr) {
			if(send(client[i],msg,strlen(msg),0) < 0) {
				perror("sending failure");
				continue;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
}


*/
void *recvmg(void *sock)
{
	//struct client_info cl = *((struct client_info *)sock);
	
	char msg[500];
	int len;
	int i;
	int j;
	char * pch;
	int index = *((int *)sock); //creating index. Pointer dereferencing with casting (basically teling the pointer to treat this as int. 
	//recv(cl.sockno,msg,500,0);
	//msg[len] = '\0';
	//printf("%s\n",msg);
	//printf ("%s\n",recv(cl.sockno,msg,500,0));
	while(read(client[index].sockno,msg,500) > 0) {//we will keep on reading from socket and then according to what command we receive as the first word we will accordingly perform that using conditions below


		 if (strncmp(msg,"/msg",4) == 0)//if the fist word is '/msg' then we will get ready to send that message to the client refered to
			      {
				char* user = strtok(msg," "); //first we wil break the string by spaces and save the client, the message has refered to
				user = strtok(NULL, " ");
				char buffer[500];

				for (int a = 0; a < n; a++)
						{///we will now search through all array of clients saved and see if the username that we're already connected with is the same the client refered to and if it is then through the buffer, we will send them the message
						if (strcmp(client[a].username,user)==0)
								{
								user = strtok(NULL,"");
								strcpy(buffer,client[index].username);//this is the client that sent the message
								strcat(buffer, ": ");
								strcat(buffer,user);
								pthread_mutex_lock(&mutex);
								if (write(client[a].sockno,buffer,strlen(buffer)) < 0)//sending message to the refered client
										{
											perror("error\n");
										}
								pthread_mutex_unlock(&mutex);
								}
						}

//NOTE: counter 'a' keeps track of the refered client and counter 'index' keeps track of the client that sent the message.
				}

		else if (strncmp(msg,"/list",strlen(msg)-1) == 0)//Now we will look at this function when the first word is list. We have to send the client which write this message all the list of clients connected to server. 
				{
					memset(msg,'\0',sizeof(msg));
					strcat(msg,client[0].username);
					for (int b = 1; b < n; b++)
						{
							strcat(msg, ", ");
							strcat(msg,client[b].username);//copying all the usernames of clients from the array to our msg char array
						}
					if (write(client[index].sockno,msg,strlen(msg)) < 0)//finally writing all the list
						{
							perror("error sending\n");
						}
				}		
		
		memset(msg,'\0',sizeof(msg));
	}
	pthread_mutex_lock(&mutex);
	printf("%s disconnected\n",client[index].username);//once any client is disconnected
	for(i = 0; i < n; i++) 
		{
			if(client[i].sockno == client[index].sockno) //searching through our array of clients
				{
					j = i;
					while(j < n-1) 
						{
							client[j] = client[j+1];
							j++;
						}
				}
		}
	n--;//decresing our number of clients connected
	pthread_mutex_unlock(&mutex);
}



int main(int argc,char *argv[])
{

//In the first part all the variables are being set and arguments being saved.
	struct sockaddr_in serv_addr, cli_addr;
	int my_sock;
	int their_sock;
	socklen_t their_addr_size;
	int portno;
	pthread_t recvt;
	char msg[500];
	//int len;
	//struct client_info cl;
	char ip[INET_ADDRSTRLEN];;
	;
	if(argc > 2) {
		printf("so many arguments");
		exit(1);
	}
	portno = atoi(argv[1]);


//In the second part socket connection will tried to be established.(same as client)
	my_sock = socket(AF_INET,SOCK_STREAM,0);
	if (my_sock < 0) {
		perror("Error establishing socket\n");
		exit(1);
	}

	//bzero((char *) &serv_addr, sizeof(serv_addr));
	//memset(serv_addr.sin_zero,'\0',sizeof(serv_addr.sin_zero));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	memset(serv_addr.sin_zero,'\0',sizeof(serv_addr.sin_zero));
	their_addr_size = sizeof(cli_addr);



//In the third part we will bind the socket and start listening to 50 clients all at once.
	if(bind(my_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))< 0) {
		perror("binding unsuccessful");
		exit(1);
	}

	if(listen(my_sock,total) != 0) {
		perror("listening unsuccessful");
		exit(1);
	}

	while(1) {
		if((their_sock = accept(my_sock,(struct sockaddr *)&cli_addr,&their_addr_size)) < 0) {
			perror("accept unsuccessful");
			exit(1);
		}
//This is the fourth part in which major work will be done. We will first create a seperate thread for this to receive messages. At the start we will check whether the clients username is already taken in server or not and send message accordingly
		pthread_mutex_lock(&mutex);
		bool ok=true;
		int index = 0;
		memset(msg,'\0',sizeof(msg));
		if (read(their_sock,msg,500) == 0){
			perror("Username error\n");
			exit(1);
		}

		int a = 0;
		while (a<n)
			{
				if (strncmp(client[a].username, msg, strlen(msg)) == 0) //checking all the client's username to see that the currrent client's userename is unique or not. If it is unique then we will write to socket and client can read it and not connect.
						{
						write(their_sock,"false",strlen("false"));
						ok=false;
						break;
						}
				a++;
			}

		if (ok)//if there was no error with username we are good to go
			{
				//copying username, socket number and ip-address
				int b = n;
				strncpy(client[n].username,msg,strlen(msg));//copying username
				printf("%s connected\n",client[n].username);
				inet_ntop(AF_INET, (struct sockaddr *)&cli_addr, ip, INET_ADDRSTRLEN);
				client[n].sockno = their_sock;
				strcpy(client[n].ip,ip);
				pthread_create(&recvt,NULL,recvmg,&b);//starting thread and calling the receiving function above
				n++;
			}

		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
