//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.
//


//
// Dumps the USB Configuration Descriptor
//

#include <windows.h>

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "devioctl.h"

#include "usbdi.h"

#include "..\..\ioctl.h"

#define NOISY(_x_) printf _x_ ;

char inPipe[32] = "PIPE00";
char outPipe[32] = "PIPE01";

HANDLE
open_dev(char *devname, char * pipename)
{
	CHAR   completeDeviceName[64] = "";
   HANDLE hDEV;

	strcat (completeDeviceName,
           "\\\\.\\"
           );

   strcat (completeDeviceName,
	        devname
           );

   strcat (completeDeviceName,
           "\\"
           );

   strcat (completeDeviceName,
           pipename
           );

	printf("completeDeviceName = (%s)\n", completeDeviceName);
	hDEV = CreateFile(completeDeviceName,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hDEV == INVALID_HANDLE_VALUE) {
		NOISY(("Failed to open (%s) = %d", completeDeviceName, GetLastError()));
	} else {
		NOISY(("Opened successfully.\n"));
    }		

	return hDEV;
}

HANDLE
open_file(char *devname, char *filename)
{

	char completeDeviceName[64] = "";
	int success = 1;
	HANDLE h;


	strcat (completeDeviceName,
            "\\\\.\\"
            );

    strcat (completeDeviceName,
			devname
			);

    strcat (completeDeviceName,
			"\\"
			);			

     strcat (completeDeviceName,
			filename
			);					

	printf("completeDeviceName = (%s)\n", completeDeviceName);
	h = CreateFile(completeDeviceName,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (h == INVALID_HANDLE_VALUE) {
		NOISY(("Failed to open (%s) = %d", completeDeviceName, GetLastError()));
		success = 0;
	} else {
		NOISY(("Opened successfully.\n"));
    }		

	return h;
}

void
usage()
{
    static int i=1;

    if (i) {
        printf("usage:\n");
        printf("-r [n] where n is number of bytes to read\n");
        printf("-w [n] where n is number of bytes to write\n");
        printf("-c [n] where n is number of iterations (default = 1)\n");
        printf("-i [s] where s is the input pipe\n");
        printf("-o [s] where s is the output pipe\n");
        printf("-s check input data for standard iso pattern\n");
        i = 0;
    }
}

int Count = 1;
int WriteLen = 0;
int ReadLen = 0;
int Iso = 0;

void
parse(
    int argc,
    char *argv[],
    char *devname)
{
    int i;

    for (i=0; i<argc; i++) {
        if (argv[i][0] == '-' ||
            argv[i][0] == '/') {
            switch(argv[i][1]) {
            case 'r':
                ReadLen = atoi(&argv[i+1][0]);
                i++;
                break;
            case 'w':
                WriteLen = atoi(&argv[i+1][0]);
                i++;
                break;
            case 'c':
                Count = atoi(&argv[i+1][0]);
                i++;
                break;
            case 's':
                Iso = 1;
                break;
            case 'i':
                strcpy(inPipe, &argv[i+1][0]);
                i++;
                break;                     
             case 'o':
                strcpy(outPipe, &argv[i+1][0]);
                i++;
                break;                    
            default:
                usage();
            }
        }
    }
}

int 
compare_buffs(char *buff1, char *buff2, int length)
{
    int i, ok = 1;
    
	if (memcmp(buff1, buff2, length )) {
		DebugBreak();

		// Edi, and Esi point to the mismatching char and ecx indicates the 
		// remaining length.  
		ok = 0;
	}

#if 0
    for (i=0; i<length; i++) {
        if (buff1[i] != buff2[i]) {

            _asm {
                push esi
                push edi
                push eax

                mov esi, buff1
                mov edi, buff2
                mov eax, i

                int 3

                pop eax
                pop edi
                pop esi
            }
            
            ok = 0;
        }
    }
#endif
    return ok;
}

void
dump(
   UCHAR *b,
   int len)
{
    int i;

    for (i=0; i<len; i++) {
        printf("%02.2x ", *(b+i));
    }
    printf("\n");
}

int _cdecl main(
    int argc,
	char *argv[])
{
    char *pinBuf = NULL, *poutBuf = NULL;
    int nBytesRead, nBytesWrite, i, j;
    int x =0, ok;
    BOOLEAN success;
    HANDLE hRead, hWrite;
  

    parse(argc, argv, "I82930-0");

	if ((ReadLen) || (WriteLen)) {

	    if (ReadLen) {
            //
            // open the output file
            //
            hRead = open_file("I82930-0", inPipe);
	    
	        pinBuf = malloc(ReadLen);
	    }

	    if (WriteLen) {

	        hWrite = open_file("I82930-0", outPipe);
	        poutBuf = malloc(WriteLen);
	    }

        for (i=0; i<Count; i++) {

    	    if (WriteLen && poutBuf && hWrite != INVALID_HANDLE_VALUE) {

                //
                // put some data in the output buffer
                //

    	        for (j=0; j<WriteLen; j++) {
    	            *(poutBuf+j) = x + Count;
    	            x++;
    	        }

                //
                // send the write
                //
	            
	            WriteFile(hWrite,
	                      poutBuf,
	                      WriteLen,
	                      &nBytesWrite,
	                      NULL);

                printf("<%s> W (%04.4d) : request %06.6d bytes -- %06.6d bytes written\n", 
                    outPipe, i, WriteLen, nBytesWrite); 	                      
                assert(nBytesWrite == WriteLen);
	        }

	        if (ReadLen && pinBuf) {
	
	            success = ReadFile(hRead,
	                          pinBuf,
                              ReadLen,
	                          &nBytesRead,
	                          NULL);

                printf("<%s> R (%04.4d) : request %06.6d bytes -- %06.6d bytes read\n", 
                    inPipe, i, ReadLen, nBytesRead); 
//                dump(pinBuf, nBytesRead); 
                
                if (WriteLen) {

                    //
                    // validate the input buffer against what
                    // we sent to the 82930 (loopback test)
                    //

                    ok = compare_buffs(pinBuf, poutBuf,  nBytesRead);

                    assert(ok == 1);

                    assert(ReadLen == WriteLen);
                    assert(nBytesRead == ReadLen);
                    assert(nBytesWrite == WriteLen);
                } else if (Iso){

                    //
                    // validate iso data
                    //
                }
	        }
	
        }

        if (pinBuf) {
            free(pinBuf);
        }

        if (poutBuf) {
            free(poutBuf);
        }

    }		

	return 0;
}

