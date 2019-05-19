#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
#include <math.h>
#include <time.h>
#include "common.h"

int main(int argc, char **argv)
{
    mqd_t mq;
    struct mq_attr attr;//same as client
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
	printf("%s\n", buffer);
	fprintf(FP,buffer);
	fprintf(FP,"\n");
	attr.mq_curmsgs = attr.mq_curmsgs-1;//now over here each time we read the message we decrease the queue by 1 so the other process knows
   
	}
     
    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));
    CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));
	fclose(FP);
    return 0;
}
