#include<windows.h>
#include<stdio.h>
#include"..\largeint.h"

void main()
{
        LONG a = 0x7FFFFFFE;
        LONG b = 4;
        LARGE_INTEGER c;

        c = EnlargedIntegerMultiply(a,b);

	printf("The Product is %X, %lX\n",c.LowPart, c.HighPart);
}
