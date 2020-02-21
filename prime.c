#include <stdio.h>
#include <stdlib.h>
#include "ossPrime.h"

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

	return 0;
}
