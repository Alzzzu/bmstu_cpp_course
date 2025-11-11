#include "str2int.h"
#include <assert.h>
#include <stdio.h> 
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

int str2int(const char* str)
{

	int res = 0;
	int sign = 1;
	int hasDigs = 0;

	for (int i=0; str[i] != '\0'; i++) {
		if(isdigit(str[i])){
			hasDigs = 1;
			int curr = (str[i]-'0');
			if((res==INT_MAX/10 && (sign<0 && (curr>-(INT_MIN%10)) || sign>0 && (curr>(INT_MAX%10))))||res>INT_MAX/10){
				 assert(0==1);		 
			}
			res = res*10 + curr;
		}
		if(str[i]=='-'){
			sign*=-1;
		}
	}

	assert(hasDigs==1);

	return res*sign;
}