/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define BUFFERSIZE 10

typedef struct {
	int counter;
	int in;
	int out;
	int buffer[BUFFERSIZE];
} shm_struct;


int main(int argc, char**argv)
{
    int shmid;
    key_t key;
    shm_struct *shm;//same as before
    int max_vals;
    int num_vals;
    int *recvd_vals; //we will create an integer pointer for the values that we receive through the shared memory
    srand(time(NULL));
 FILE* FP;

	recvd_vals = malloc (1000*sizeof(int));

    /*
     * We need to get the segment named
     * "5670", created by the server.
     */
    key = 5670;

    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, sizeof(shm_struct), 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (shm_struct *) -1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now read what the server put in the memory.
     */
  FP = fopen("Producer.txt","w+");
    for (int i=0;i<1000;i++)
	{
	while (shm->counter==0)//so each time we read a value from the shared memory we decrease the counter by one. IF the values in the shared memory reaches 0 that would mean that the consumer has read all the values so it goes on wait until new value comes in
		; //wait

	int val_consumed = shm->buffer[shm->out];
	shm->out = (shm->out+1)%BUFFERSIZE;
	
	recvd_vals[i] = val_consumed;//receiving the values
	fprintf(FP,"%d",val_consumed);
	fprintf(FP,"\n");
	usleep (100);//using this before and after the counter to avoid race conditions 
	shm->counter--;//decrease of counter
	usleep (100);

    	}


    // display the values read
    printf("consumer received:\n");
    for (int i=0; i< 1000; i++)
	printf("%d\n", recvd_vals[i]);
    /*
     * Finally, change the first character of the 
     * segment to '*', indicating we have read 
     * the segment.
     */
    //shm->counter = 0;

    /*
     * cleanup: detach  shm
     */
    shmdt (shm);
    fclose(FP);
    exit(0);
}
