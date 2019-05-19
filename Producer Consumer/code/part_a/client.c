#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <math.h>
#include <time.h>
 
#include "common.h"


int main(int argc, char **argv)
{
    struct mq_attr attr; //So first of all we make a struct so that we can use the attributes to commmunicate
    mqd_t mq;
    char buffer[MAX_SIZE+1]={'\0'}; //ending the array of characters
    srand(time(NULL));
    int random[1000]; //the array we'll store the values on
    FILE* FP; //file pointer
   /* initialize the queue attributes */ //setting their values accordingly
    attr.mq_flags = 0;        
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
   /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);
    FP = fopen("Producer.txt","w+"); //opening file
	for(int count = 0; count < 1000; count ++)
	{
	mq_getattr(mq,&attr); //what we'll do first is get the attribute informatin from the other process so that we may know the condition of our queue
	if(  attr.mq_curmsgs<attr.mq_maxmsg)
		{
		int di = rand();//random value
		random[count]=di;//inserting in array
		sprintf(buffer,"%d",di);
		printf("%s\n",buffer);
		CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));
		attr.mq_curmsgs++;//increasing this to inscrease the queue size
		fprintf(FP,buffer);
		fprintf(FP,"\n");
		}
	else
		{
			count=count-1;//decreasing the count size of the  (to get space)
		}
	}
    CHECK((mqd_t)-1 != mq_close(mq));
	fclose(FP);
    return 0;
}
