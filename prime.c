#include <stdio.h>
#include <stdlib.h>
#include "ossPrime.h"
#include <string.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

// Fictional clock
struct Clock{
	int sec;
	int nanosec;
};

int main(int argc, char* argv[]){
	// Generate same key that parent did for shared memory for time
	key_t key1 = ftok("./oss.c", 0);
	if(key1 == -1){
		perror("ERROR in child for getting key1");
		exit(0);
	}
	// Get and assign such a key
	int timerid = shmget(key1, sizeof(struct Clock), 0600 | IPC_CREAT);
	if(timerid == -1){
		perror("ERROR in child for getting shared memory for key1");
		exit(0);
	}
	// Attach key
	struct Clock *cTime = (struct Clock*)shmat(timerid, (void*)0, 0);
	if(cTime == (void*)-1){
		perror("Failed to attach memory for cTime.");
		exit(0);
	}
	// Set the grab value
	long iniTimeSec = cTime->sec, iniTimeNanoSec = cTime->nanosec;
	//printf("Time in child: %d.%d\n", iniTimeSec, iniTimeNanoSec);
	//printf("Time of cTime now: %d.%d\n", cTime->sec, cTime->nanosec);

	// Shared memory for array
	key_t key2 = ftok("./oss.c", 1);
	if(key2 == -1){
		perror("ERROR in child for getting key2");
		exit(0);
	}
	int arrayid = shmget(key2, sizeof(int), 0666 | IPC_CREAT);
	if(arrayid == -1){
		perror("ERROR in child for getting shared memory for key2");
		exit(0);
	}
	int *cArray = (int*)shmat(arrayid, (void*)0,0);
	if(cArray == (void*)-1){
		perror("ERROR failed to attach memor for cArray");
		exit(0);
	}
	
	// Get child index value (not the same as PID)
	int childPos = atoi(argv[2]);
	//printf("Given number = %d\n", cArray[childPos]);
	// Variables for testing for primilty
	int i;
	int flag = 0;
	int testNum = atoi(argv[1]);
	for(i = 2; i <= testNum / 2; i++){
		//printf("In for loop, current time is %d\n", (((cTime->sec * 1000000000) + cTime->nanosec)) -((iniTimeSec * 1000000000) + iniTimeNanoSec));
		// Check if longer then 1 microsecond	
		if(((cTime->sec * 1000000000) + cTime->nanosec) - ((iniTimeSec * 1000000000) + iniTimeNanoSec ) > 1000000){
			flag = -1;
			cArray[childPos] = -1;
			return(-1);
		}
		if(testNum % i  == 0){
			cArray[childPos] = testNum;
			return(1);
		}
	}
	if(flag == 0){
		cArray[childPos] = testNum * -1;
		return(0);
	}
}
