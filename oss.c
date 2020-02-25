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
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

// Imaginary clock
struct Clock{
	int sec;
	int nanosec;
	long total;
};

struct Clock *timer;	// Simulated clock

void god(int signal);
void clockHandler(int signal);
void addTime(int* sec, int* nanosec);

// List of PIDS if we need to clear them
int* listOfPIDS;
int numOfPIDS = 0;

char outFile[255] = "output.txt";       // Output file

int main(int argc, char* argv[]){
	// Setup for real clock timer and interupt
	struct itimerval time;
	time.it_value.tv_sec = 2;
	time.it_value.tv_usec = 0;
	time.it_interval = time.it_value;
	signal(SIGALRM, god);	// Will call god if program takes longer then 2 seconds to run
	setitimer(ITIMER_REAL, &time, NULL);

	// Variables for getOps and more
	int option;		// Placeholder for command line arguments arg
	int maxChild = 4;	// Max total of child processes oss wil create so we dont have infinite loop
	int childExist = 2;	// The number of children processes that can exists at any gvient time
	int startNum = 0;	// The start of numbers to be tested for primality
	int increNum = 0;	// Increment between the numbers we test
	
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

	// Create array for the test numbers
	int i;
	int numArr[maxChild];
	numArr[0] = startNum;
	for(i = 1; i < maxChild; i++){
		numArr[i] = numArr[i-1] + increNum;
	}
	
	// Arrays for prime and non prime numbers
	int primeNum[maxChild];
	int primeNumCount = 0;
	int nonPrimeNum[maxChild];
	int nonPrimeNumCount = 0;
	int timeOut[maxChild];
	int timeOutCount = 0;
	
	// Variables for the fork loop
	int activeChildren = 0;
	int childDone = 0;
	int exitCount = 0;
	pid_t pid;
	int status;
	listOfPIDS = calloc(maxChild, sizeof(int));		// Dynamically allocate memory for pids

	// File Varianles and error handeling
	FILE *fn = fopen(outFile, "w");
	if(!fn){
		perror("ERROR, could not open file\n");
		return(1);
	}
	fprintf(fn, "File has been opened\n");

	// Create shared memory key
	key_t key1 = ftok("./oss.c", 0);
	if(key1 == -1){
		perror("ERROR deriving key1 in parent. Ftok() failed.\n");
		printf("key1 = %d\n", key1);
		return EXIT_FAILURE;
	}
	// Get Shared memory key
	int timerid = shmget(key1, sizeof(struct Clock), 0600 | IPC_CREAT);
	if(timerid == -1){
		perror("ERROR Getting shared memory tiemrid. shmget failed.\n");
		return(0);
	}
	// Attach key
	timer = (struct Clock*)shmat(timerid,(void*)0,0);
	if(timer == (void*)-1){
		perror("Failed to attach memory for timer shmat failed.\n");
		return(0);
	}
	// Set timer initial values
	timer->sec = 0;
	timer->nanosec = 0;
	// Create shared memory key
	key_t key2 = ftok("./oss.c", 1);
	if(key2 == -1){
		perror("ERROR deriving key2 in parent. ftok() failed.");
		return EXIT_FAILURE;
	}
	// Get shared memory key
	int arrayid = shmget(key2, sizeof(int), 0666 | IPC_CREAT);
	if(arrayid == -1){
		perror("ERROR getting shared memory id for arrayid. shmget failed.");
		return EXIT_FAILURE;
	}
	// Attach memory key
	int* pArray = (int*)shmat(arrayid,(void*)0,0);
	if(pArray == (void*)-1){
		perror("Failed to attac memory for shared memory array shmat failed.");
		return EXIT_FAILURE;
	}
	// Fill array with 0s
	for(i = 0; i <= maxChild; i++){
		pArray[i] = 0;
	}


	// Signal for catching ctl-c
	signal(SIGINT, god);
	signal(SIGPROF, god);
	
	// Create children unit we have reached the limit
	while(childDone <= maxChild && exitCount < maxChild){
		// Create more children unitl we reach active child limit, or activechildren reaches 20
		if(childDone < maxChild && activeChildren < childExist && activeChildren <= 20){
			pid = fork();
			// Fork error
			if(pid < 0){
				perror("FORKING ERROR\n");
				fclose(fn);
				return EXIT_FAILURE;
			}
			// Child is created
			else if(pid == 0){
				//printf("[son] pid %d from [parent] pid %d\n", getpid(),getppid());
				char convertNum[15];
				char childNum[15];
				// Convet numbers to strings for command arguements
				sprintf(convertNum, "%d", numArr[childDone]);
				sprintf(childNum, "%d", childDone);
				// Execute ./prime
				char *args[] = {"./prime", convertNum, childNum, NULL};
				execvp(args[0], args);
			}
			listOfPIDS[numOfPIDS] = pid;
			numOfPIDS++;
			fprintf(fn, "Child %d with PID %d and number %d has launched at time %d seconds and %d nanoseconds\n", childDone, pid, numArr[childDone], timer->sec, timer->nanosec);
			childDone++;
			activeChildren++;
		}
		//Check and see if child has ended
		if((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0){
			if(WIFEXITED(status)){
				int exitStatus = WEXITSTATUS(status);
				//printf("Child %d with PID:%d has been terminated after %d seconds and %d nanoseconds\n", exitCount, pid, timer.sec, timer.nanosec);
				//fprintf(fn, "Child %d with PID:%d has been terminated after %d seconds and %d nanoseconds\n", exitCount, pid, timer->sec, timer->nanosec);
				//printf("Exit status of child %d was %d\n", pid, exitStatus);
				if(exitStatus == 0){
					fprintf(fn, "Child %d with PID %d has been terminated with the number %d after %d seconds and %d nanoseconds and determined it was prime. It returned %d\n", exitCount, pid, numArr[exitCount],timer->sec, timer->nanosec, pArray[exitCount]);
					primeNum[primeNumCount] = numArr[exitCount];
					primeNumCount++;
				}else if(exitStatus == 1){
					fprintf(fn, "Child %d with PID %d has been terminated with the number %d after %d seconds and %d nanoseconds and determined it wasn't prime. It returned %d\n", exitCount, pid, numArr[exitCount],timer->sec, timer->nanosec, pArray[exitCount]);
					nonPrimeNum[nonPrimeNumCount] = numArr[exitCount];
					nonPrimeNumCount++;
				}else{
					fprintf(fn, "Child %d with PID %d has been terminated with the number %d after %d seconds and %d nanoseconds. %d caused a timeout\n", exitCount, pid, numArr[exitCount], timer->sec, timer->nanosec, pArray[exitCount]);
					timeOut[timeOutCount] = numArr[exitCount];
					timeOutCount++;
				}
				activeChildren--;
				exitCount++;
			}
		}
		// Increment timer
		timer->nanosec += 10000;
		while(timer->nanosec >= 1000000000){
			timer->sec++;
			timer->nanosec -= 1000000000;
		}
		//printf("Active Children = %d\n", activeChildren);
	}

	// Destroy shared memory
	shmdt(timer);
	shmctl(timerid, IPC_RMID, NULL);
	shmdt(pArray);
	shmctl(arrayid, IPC_RMID, NULL);
	free(listOfPIDS);

	// Write primes and non primes to file
	fprintf(fn, "The prime numbers were: \n");
	for(i = 0; i < primeNumCount; i++){
		fprintf(fn, "%d ", primeNum[i]);
	}
	fprintf(fn, "\nThe non prime numbers were: \n");
	for(i = 0; i < nonPrimeNumCount; i++){
		fprintf(fn, "%d ", nonPrimeNum[i]);
	}
	fprintf(fn, "\nThe numbers the caused the time out were: \n");
	for(i = 0; i < timeOutCount; i++){
		fprintf(fn, "%d ", timeOut[i]);
	}
	fprintf(fn, "\nFile has been closed\n");

	fclose(fn);		// Close file
	return 0;	
}

// Is my signal handler please do not anger it
void god(int signal){
	//printf("GOD HAS BEEN CALLED. THE RAPTURE HAS STARTED\n");
	int i;
	for(i = 0; i < numOfPIDS; i++){
		kill(listOfPIDS[i], SIGTERM);
	}

	// Access file for saying god was called
	FILE* out = fopen(outFile, "a");
	fprintf(out, "God was called, and the rapture has begun.. Soon there will be nothing.\n");
	fprintf(out, "Time of world ending: %d seconds %d nanoseconds\n", timer->sec, timer->nanosec);
	fclose(out);

	free(listOfPIDS);
	kill(getpid(), SIGTERM);	// God must now kill himself
}
