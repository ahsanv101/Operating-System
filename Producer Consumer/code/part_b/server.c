#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
    shm_struct * shm; //initializing the shared memory structure
    int max_vals;
    int num_vals;
srand(time(NULL)); 
 FILE* FP;

    /*
     * We'll name our shared memory segment
     * "5670".
     */
    key = 5670;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, sizeof(shm_struct), IPC_CREAT | 0666)) < 0) {
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
     * Now put some things into the memory for the
     * other process to read.
     */
    shm->counter=0;//we will use additional variables through which the other process can know about the shared memory
    shm->in = 0;
    shm->out = 0;
   
FP = fopen("Consumer.txt","w+");
   for(int i=0;i<1000;i++){
	while (shm->counter==BUFFERSIZE); //here if the counter of the shared memory ie the space of the shared memory becomes equal to the buffersize which was 10 then this process will wait
	int di = rand();//random number
	shm->buffer[shm->in] = di;//sending in the buffer of shared memory
	//shm->counter++;	
	// next vlaue prduced
	printf("%d\n", di);
	
	shm->in = (shm->in+1)%BUFFERSIZE;

	fprintf(FP,"%d",di);
	fprintf(FP,"\n");
	shm->counter++;//increase the counter of the shared memory by one at each insertion of 
	
	//int tmp = shm->counter; 
	//usleep (100);
	//tmp++;
	//usleep (100);
	//shm->counter = tmp;
    }

     // cleanup: detach and remove shm
     
    shmdt (shm);
    shmctl (shmid, IPC_RMID, NULL);
    fclose(FP);
    exit(0);
}

