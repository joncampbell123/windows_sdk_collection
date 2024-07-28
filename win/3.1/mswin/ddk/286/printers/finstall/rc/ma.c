// ma.c
// turns binary .tbl files into .asm source
// so when some one gives us binary .tbl files we can generate source.
//
// Copyright (c) 1989-1990, Microsoft Corp.
//

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

main()
{
int ich;
int state;
int lastchar;
int chno;

setmode(fileno(stdin), O_BINARY);

printf("\nDATA SEGMENT\n");

state = 0;				// 1st or 2nd char for translation.
chno = 0x80;				// character value

while (EOF != (ich = getchar()))
    {
    switch (state)
	{
	case 0:
	    lastchar = ich;		// save first byte
	    break;

	case 1:
	default:
		printf("\n\tDB\t");
		putbyte(lastchar);
		printf(",\t");
		putbyte(ich);
		printf("\t; 0%xh", chno++);
		break;
	}

    state = 1 - state;
    }

printf("\nDATA ENDS\n    END\n");

} // main

putbyte(ich)
int ich;
{
if ((ich >= 0x21) && (ich <= 0x7e) && (ich != '\''))
    printf("'%c'", ich);
else
    printf("0%xh", ich);

} // ich
