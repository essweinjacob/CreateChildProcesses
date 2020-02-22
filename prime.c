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
		return(1);
	
	else
		return(0);
}
