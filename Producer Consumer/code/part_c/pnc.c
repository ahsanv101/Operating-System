// including libraries
#include <stdio.h>
#include <stdlib.h>		
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <errno.h>
#include "common.h"
#include <pthread.h>
# define BufferSize 10
void *Producer(void *);
void *Consumer(void *);
int BufferIndex=0;
char *BUFFER;



int main()
{    
    pthread_t ptid,ctid;//pthread_t is a type similar to int and it's created when you define it although it doesn't hold anything useful until the return from pthread_create.
    pthread_create(&ptid,NULL,Producer,NULL);//this is used to create a new thread of the producer
    pthread_create(&ctid,NULL,Consumer,NULL);//similar here
    pthread_join(ctid,NULL); //suspends execution of the calling thread until the target thread terminates, unless the target thread has already terminated
    pthread_join(ptid,NULL);
    return 0;
}

void *Producer(void *a)//same code as our previous producer from message queueu having a limit of size 10 of the buffer
{    
    struct mq_attr attr;
    mqd_t mq;
    char buffer[MAX_SIZE+1]={'\0'};
    srand(time(NULL));
    int random[1000];
    FILE* FP;
   /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
   /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);
    FP = fopen("Producer.txt","w+");
	for(int count = 0; count < 1000; count ++)
	{
	mq_getattr(mq,&attr);
	if(  attr.mq_curmsgs<attr.mq_maxmsg)
		{
		int di = rand();
		random[count]=di;
		sprintf(buffer,"%d",di);
		printf("%s %d\n",buffer,count);
		CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));
		attr.mq_curmsgs++;
		fprintf(FP,buffer);
		fprintf(FP,"\n");
		}
	else
		{
			count=count-1;
		}
	}
    CHECK((mqd_t)-1 != mq_close(mq));
	fclose(FP);
	pthread_exit(NULL);

}

void *Consumer(void *b)//similarly same code for consumer as well
{

    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE];
    srand(time(NULL));  
	int count;
    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
  /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);
    mq_getattr(mq,&attr);
	FILE* FP;
	FP = fopen("Consumer.txt","w+");
 
	for(count = 0; count < 1000; count ++)
	{      
        mq_receive(mq, buffer, MAX_SIZE, NULL);
	printf("%s %d\n", buffer,count);
	fprintf(FP,buffer);
	fprintf(FP,"\n");
	attr.mq_curmsgs = attr.mq_curmsgs-1;
   
	}
     
    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));
	fclose(FP);
 pthread_exit(NULL);//exiting thread cleanly   
}
