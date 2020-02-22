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

struct Clock{
	int sec;
	int nanosec;
};

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
	
	// Variables for the fork loop
	int activeChildren = 0;
	int childDone = 0;
	int exitCount = 0;
	pid_t pid;
	int status;

	// File Varianles and error handeling
	FILE *fn = fopen(outFile, "w");
	if(!fn){
		perror("ERROR, could not open file\n");
		return(1);
	}
	fprintf(fn, "File has been opened\n");
	
	// Clock Setup
	struct Clock timer;
	timer.sec = 0;
	timer.nanosec = 0;

	// Shared Memory variables and error handeling
	int shmid = shmget(SHMKEY, BUFF_SZ, 0777 | IPC_CREAT);
	if(shmid == -1){
		perror("ERROR in Parent for reguarind shmget\n");
		exit(1);
	}
	
	char* paddr = (char*)(shmat(shmid, 0, 0));
	int* pint = (int*)(paddr);

	
	while(childDone <= maxChild && exitCount < maxChild){
		if(childDone < maxChild && activeChildren < childExist){
			pid = fork();
			if(pid < 0){
				perror("FORKING ERROR\n");
				fclose(fn);
				exit(0);
			}
			else if(pid == 0){
				// Shared memory testing
				//*pint = 10 * childDone;
				//printf("Parent mem int = %d\n", *pint);
				// Good stuff here
				//printf("[son] pid %d from [parent] pid %d\n", getpid(),getppid());
				char convertNum[15];
				char convertPID[15];
				sprintf(convertNum, "%d", arr[childDone]);
				sprintf(convertPID, "%d", getpid());
				char *args[] = {"./prime", convertNum, NULL};
				execvp(args[0], args);
			}
			fprintf(fn, "Child with PID %d and number %d has launched at time %d seconds and %d nanoseconds\n", pid, arr[childDone], timer.sec, timer.nanosec);
			childDone++;
			activeChildren++;
		}
		timer.nanosec += 10000;                                                                                                                                                                                                                                                                                                      if(timer.nanosec > 1000000000){
                        timer.sec++;
			timer.nanosec = 1000000000 - timer.nanosec;
		}
		if((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0){
				waitpid(pid, &status, 0);
				if(WIFEXITED(status)){
					int exitStatus = WEXITSTATUS(status);
					fprintf(fn, "Child with PID:%d has been terminated after %d seconds and %d nanoseconds\n", pid, timer.sec, timer.nanosec);
					//printf("Exit status of child %d was %d\n", pid, exitStatus);
					//if(exitStatus == 1)
						//fprintf(fn, "%d is a prime number and was calculated after %d seconds and %d nanoseconds\n", arr[exitCount], timer.sec, timer.nanosec);
					//else
						//fprintf(fn, "%d is not a prime number and was calculated after %d seconds and %d nanoseconds\n", arr[exitCount], timer.sec, timer.nanosec);
					activeChildren--;
					exitCount++;
				}
		}
		//printf("Active Children = %d\n", activeChildren);
	}
	fprintf(fn, "File has been closed\n");
	fclose(fn);

	return 0;	
}

