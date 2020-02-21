/*
 * Author: Jacob Esswein
 * Class: CMPSCI 4760
 * Start Date: Feb 14, 2020
 * Last Worked on Date: Feb 14, 2020
 *
 * Goal of this hw is to become familiar with shared memory and creating multiple processes.
 * In this project we will be using multiple processes to deteremine a set of numbers
 * are prime or not using getopt, perror and fork.
 *
 */

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

#define SHMKEY 	859047
#define BUFF_SZ sizeof(int)

void createChildren();
void handler(int signal, int numpids, int *listPids);
void child();

int main(int argc, char* argv[]){
	int option;		// Placeholder for command line arguments arg
	int maxChild = 4;	// Max total of child processes oss wil create so we dont have infinite loop
	int childExist = 2;	// The number of children processes that can exists at any gvient time
	int startNum = 0;	// The start of numbers to be tested for primality
	int increNum = 0;	// Increment between the numbers we test
	char outFile[255] = "output.txt";	// Output file
	
	// Read throguht command line arguments
	while((option = getopt(argc, argv, "hn:s:b:i:o:")) != -1){
		switch(option){
			case 'h':
				printf("This program will detect if numbers within a set are prime using child processes\n");
				printf("-h Will display a help message of what each command line object does and then will exist the program\n");
				printf("-s x Indicate the number of children allowed to exist in the system at the same time. (Default 2)\n");
				printf("-b B Start of the sequence of numbers to be tested for primality\n");
				printf("-i I Increment between numbers that we test\n");
				printf("-o filename Output file\n");
				return EXIT_SUCCESS;
			case 'n':
				maxChild = atoi(optarg);
				break;
			case 's':
				childExist = atoi(optarg);
				break;
			case 'b':
				startNum = atoi(optarg);
				break;
			case 'i':
				increNum = atoi(optarg);
				break;
			case 'o':
				strncpy(outFile, optarg, 255);
				break;
			default:
				return EXIT_FAILURE;
		}
	}

	// Create increments for prime numbers
	int i;
	int numArray[maxChild];
	numArray[0] = startNum;
	for(i = 1; i < maxChild; i++){
		numArray[i] = numArray[i-1] + increNum;
	}
	
	int arr[] = {10,11,12,14};
	int activeChildren = 0;
	int childDone = 0;
	pid_t pid;
	
	while(childDone <= maxChild){
		if(childDone < maxChild){
			pid = fork();
		
		if(pid < 0){
			perror("FORKING ERROR\n");
			exit(0);
		}
		if(pid == 0){
			char convert[15];
			sprintf(convert, "%d", arr[childDone]);
			char *args[] = {"./prime", convert, NULL};
			execvp(args[0], args);
			exit(0);
		}
	}
		if(pid > 0){
			if(childDone != 0)
				activeChildren++;
			childDone++;
			if(activeChildren >= childExist){
				printf("Currently waitng on children to die\n");
				wait(NULL);
				activeChildren--;
			}
			
		}
	}
	wait(NULL);
	printf("Out of forking\n");

	return 0;	
}

