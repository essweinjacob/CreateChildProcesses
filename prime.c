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
	int i;
	int flag = 0;
	int testNum = atoi(argv[1]);
	for(i = 2; i <= testNum / 2; i++){
		if(testNum % i  == 0){
			flag = 1;

		}
	}
	if(flag == 0)
		printf("%d is a prime number\n", testNum);
	else
		printf("%d is not a prime number\n", testNum);

	sleep(5);
	int shmid = shmget(SHMKEY, BUFF_SZ, 0777);
	if(shmid == -1){
		perror("ERROR in child for shmget\n");
		exit(1);
	}
	int* cint = (int*)(shmat(shmid, 0, 0));
	printf("Child mem storage = %d\n", *cint);

	return 0;
}
