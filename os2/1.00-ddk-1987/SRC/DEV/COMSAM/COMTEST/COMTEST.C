/* 		-----	Comtest utility for OS/2  -----  		

Purpose:

	This program will display a start up message to the COM1 device.
	It will then read any characters waiting at the COM1 device 
	and send echo them back out to the COM1 device.

*/

#define FALSE 0
#define TRUE 1
    
#include <stdio.h>
#include <doscalls.h>
#include <dos.h>

main(argc, argv)
int argc;
char *argv[];
{
unsigned fh, err;
unsigned filesize, byteswriten, bytesread;
int ch;
char tmp;

union REGS inregs, outregs;
struct SREGS segregs;
    
    /* Using the DOS open call open the device for input and output */
    DOSOPEN((char far *) "COM1",
	    (unsigned far *) &fh,
	    (unsigned far *) &err,
	    (unsigned long) filesize,
	    (unsigned) 0x0,
	    (unsigned) 0x01,
	    (unsigned) 0x0042,
	    (unsigned long) 0);
    
    /* Check to see if an error occurred during the open */
        if(err != 1) 
	printf("Error opening the com device, err = %x\n", err);

    /* Write the signon message to the COM1 device */
    err = DOSWRITE((unsigned) fh, 
	    (char far *) "\r\n*** DOS COM1 TESTER ***\r\n",
	    (unsigned) 27,
	    (unsigned far *) &byteswriten);

    /* Check for an error on the write */
    if(err != 0)
	printf("Error on writing com device, err = %x\n", err);

    err = DOSWRITE((unsigned) fh, 
	    (char far *) "\r\nTo quit, Press CTRL-Z.\r\n",
	    (unsigned) 26,
	    (unsigned far *) &byteswriten);

    /* Check for an error on the write */
    if(err != 0)
	printf("Error on writing com device, err = %x\n", err);

    /* Now do a read and write of all characters typed at COM1 device
    until an EOF or a key is typed on the system console */
    
    while(!kbhit() && ch != 0x1a)
	{
	/* Read the character using the OS/2 read call */
	err = DOSREAD((unsigned) fh,
		(char far *) &ch,
		(unsigned) 1,
		(unsigned far *) &bytesread);

	/* Check for an error on the read */
	if(err != 0)
	    printf("Error reading COM1 device, err = %x\n", err);
	else
	    {
	    tmp=ch;
	    if(ch == 0xd)
		printf("Char Read = <CR> (0x%x)\n", ch);
	    else
		if(ch == 0xa)
		    printf("Char Read = <LF> (0x%x)\n", ch);
		else
		    printf("Char Read =  %c  (0x%x)\n", tmp, ch);

	    /* Now write the character out to the COM1 device. */
	    err = DOSWRITE((unsigned) fh, 
		(char far *) &ch,
		(unsigned) 1,
		(unsigned far *) &byteswriten);

	    if(err != 0)
		printf("Error writing COM1 device, err = %x\n", err);

	    /* If a <CR> is read, then send out a line feed. */
	    if( ch == 0x0d)
		{
		err = DOSWRITE((unsigned) fh, 
		    (char far *) "\n",
		    (unsigned) 1,
		    (unsigned far *) &byteswriten);
		if(err != 0)
		    printf("Error writing COM1 device, err = %x\n", err);
		}
	    }
	}
    /* close the COM1 device */
    DOSCLOSE((unsigned) fh );
}
