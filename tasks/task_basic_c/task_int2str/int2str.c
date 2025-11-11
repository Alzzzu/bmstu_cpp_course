#include "int2str.h"
#include <assert.h>
#include "stdio.h"
#include <limits.h>

int countDigits(int n) {
    if (n == 0) {
        return 0; 
    }
    return 1 + countDigits(n / 10);
}

char* int2str(int number)
{
	if(number == INT_MIN){
		return "-2147483648";
	}
    int size = countDigits(number);
	int counter = 10;
	char digs[size + (number<0?1:0) + 1];

	if(number<0&&number!=2147483648){
		digs[0] = '-';
		number = -number;
		++size;
	}
	digs[size] = '\0';
	
    do{
		size--;
        digs[size] = number%10+'0';
		number/=10;
    } while (number/10!=0||number%10!=0);
	
	char* str = &digs[0];
	return str;
}