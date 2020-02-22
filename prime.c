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

#define SHMKEY 859047
#define BUFF_SZ sizeof(int)

int main(int argc, char* argv[]){
	int shmid = shmget(SHMKEY, BUFF_SZ, 0777);
	if(shmid == -1){
		perror("ERROR IN CHILD FOR SHARED MEMORY");
		exit(0);
	}
	int *cTimeGrab = (int*)(shmat(shmid, 0, 0));
	int iniTime = *cTimeGrab;
	int i;
	int flag = 0;
	int testNum = atoi(argv[1]);
	for(i = 2; i <= testNum / 2; i++){
		int * cCurrent = (int*)(shmat(shmid, 0, 0));
		if(*cCurrent - iniTime >= 100000){
			printf("Coming here\n");
			flag = -1;
			return(-1);
		}
		if(testNum % i  == 0){
			flag = 1;

		}
	}

	if(flag == 0)
		return(0);
	else if(flag == 1)
		return(1);
	
}
