/*		-----	Comtest utility for MS OS/2 -----

 Copyright (c) Microsoft Corporation,  1988

Purpose:

	This program will display a start up message to the COM1 device.
	It will then read any characters waiting at the COM1 device 
	and send echo them back out to the COM1 device.

*/

#include <os2.h>

/* C include files */
#include <stdio.h>
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
    DosOpen((PSZ)"COM1",
	         (PHFILE)  &fh,
	         (PUSHORT) &err,
	         (ULONG)   filesize,
	         (USHORT)  0,
	         (USHORT)  1,
	         (USHORT)  0x0042,
	         (ULONG)   0);
    
    /* Check to see if an error occurred during the open */
    if(err != 1) 
      	printf("Error opening the com device, err = %x\n", err);

    /* Write the signon message to the COM1 device */
    err = DosWrite((HFILE) fh, 
                   (PVOID) "\r\n*** 286 COM1 TESTER ***\r\n",
           	       (USHORT) 27,
	                (PUSHORT) &byteswriten);

      /* Check for an error on the write */
    if(err)
      	printf("Error on writing com device, err = %x\n", err);

    err = DosWrite((HFILE) fh, 
	                (PVOID) "\r\nTo quit, Press CTRL-Z.\r\n",
	                (USHORT) 26,
	                (PUSHORT) &byteswriten);

    /* Check for an error on the write */
    if(err)
	  printf("Error on writing com device, err = %x\n", err);

    /* Now do a read and write of all characters typed at COM1 device
    until an EOF or a key is typed on the system console */
    
    while(!kbhit() && ch != 0x1a) {
   	/* Read the character using the MS 0S/2 read call */
   	err = DosRead((HFILE)   fh,
		              (PVOID)   &ch,
		              (USHORT)  1,
		              (PUSHORT) &bytesread);

   	/* Check for an error on the read */
	   if(err != 0)
	    printf("Error reading COM1 device, err = %x\n", err);
	   else {
	    tmp=ch;
	    if(ch == 0xd)
		    printf("Char Read = <CR> (0x%x)\n", ch);
	    else	if(ch == 0xa)
		    printf("Char Read = <LF> (0x%x)\n", ch);
		 else
		    printf("Char Read =  %c  (0x%x)\n", tmp, ch);

	     /* Now write the character out to the COM1 device. */
	     err = DosWrite((HFILE)   fh, 
		                (PVOID)   &ch,
		                (USHORT)  1,
		                (PUSHORT) &byteswriten);

	     if (err)
		    printf("Error writing COM1 device, err = %x\n", err);

	     /* If a <CR> is read, then send out a line feed. */
	     if( ch == 0x0d) {
		    err = DosWrite((HFILE)   fh, 
		                   (PVOID)   "\n",
		                   (USHORT)  1,
		                   (PUSHORT) &byteswriten);
		    if(err)
		     printf("Error writing COM1 device, err = %x\n", err);
	     }
	    }	/* else */
	} /* while */
	
   /* close the COM1 device */
   DosClose((HFILE) fh );
}
