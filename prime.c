#include <stdio.h>
#include "ossPrime.h"

void prime(int inNum){
	int i;
	int flag = 0;
	for(i = 2; i <= inNum / 2; i++){
		if(inNum % i  == 0){
			flag = 1;

		}
	}
	if(flag == 0)
		printf("%d is a prime number\n", inNum);
	else
		printf("%d is not a prime number\n", inNum);	
}
