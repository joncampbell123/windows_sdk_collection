//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.
//


#include <windows.h>

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>


#include "devioctl.h"

#include "..\..\ioctl.h"

//#define NOISY(_x_) printf _x_ ;
#define NOISY(_x_) 

HANDLE hDEV;

int
open_dev(char *devname)
{

	char completeDeviceName[64] = "";
	int success = 1;


	strcat (completeDeviceName,
            "\\\\.\\"
            );

    strcat (completeDeviceName,
			devname
			);

	NOISY(("completeDeviceName = (%s)\n", completeDeviceName));
	hDEV = CreateFile(completeDeviceName,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hDEV == INVALID_HANDLE_VALUE) {
		NOISY(("Failed to open (%s) = %d", completeDeviceName, GetLastError()));
		success = 0;
	} else {
		NOISY(("Opened successfully.\n"));
    }		

	return success;
}

char
*get_type(PI82930_PIPE_INFO pi)
{
    
	switch(pi->PipeType) {
	case INTERRUPT:
		return "Interrupt";
	case BULK:
		return "Bulk     ";	
	case ISO:
		return "Iso    ";			
	case CONTROL:
		return "Control  ";				
	default:
		return "???      ";	
	}
}

char
*get_direction(PI82930_PIPE_INFO pi)
{
	if (pi->In) 
	    return "in ";
	else
		return "out";
}


void
rw_dev(char *name)
{
	BOOLEAN success;
	int siz, nBytes, i;
	char buf[256], chBuff[80];
    PI82930_INTERFACE_INFO interfaceInfo;
    PI82930_PIPE_INFO pipeInfo;

	siz = sizeof(buf);

	if (hDEV == INVALID_HANDLE_VALUE) {
		NOISY(("DEV not open"));
		return;
	}
	
	success = DeviceIoControl(hDEV,
			IOCTL_I82930_GET_PIPE_INFO,
			buf,
			siz,
			buf,
			siz,
			&nBytes,
			NULL);

	NOISY(("request complete, success = %d nBytes = %d\n", success, nBytes));
	
	if (success) {

        interfaceInfo = (PI82930_INTERFACE_INFO) buf;
        NOISY(("info = %x\n", interfaceInfo));

        sprintf(chBuff, "(%s)*** Number Of Pipes %02.2d", name,
    				interfaceInfo->PipeCount);
  		puts(chBuff);
        for (i=0; i< interfaceInfo->PipeCount; i++) {
        
		    pipeInfo = &interfaceInfo->Pipes[i];
			sprintf(chBuff, 
		    "(%s) :: EP address (0x%02.2x)-(%s %s) Max Packet = %02.2d bytes [%d ms]", 
				pipeInfo->Name, 
				pipeInfo->EndpointAddress,
				get_type(pipeInfo),
				// address includes direction
				get_direction(pipeInfo),
				pipeInfo->MaximumPacketSize,
				pipeInfo->PipeType == INTERRUPT ? pipeInfo->Interval : 0);
			puts(chBuff);
			sprintf(chBuff, 
			 "           MaximumTransferSize = 0x%x", 
				pipeInfo->MaximumTransferSize);
			puts(chBuff);
   		}
	}
	
	return;

}


int _cdecl main(
    int argc,
	char *argv[])
{

	if (open_dev("I82930-0"))
		rw_dev("I82930-0");

	return 0;
}
