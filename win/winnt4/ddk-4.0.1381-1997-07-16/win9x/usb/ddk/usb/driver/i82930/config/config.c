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


#include "devioctl.h"

#include "usbdi.h"

#include "..\..\ioctl.h"

#define NOISY(_x_) printf _x_ ;

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

	printf("completeDeviceName = (%s)\n", completeDeviceName);
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

void 
print_cd(PUSB_CONFIGURATION_DESCRIPTOR cd)
{
    printf("Config\n");
    printf("---------\n");   
    printf(
    "bLength = 0x%x\n", cd->bLength
    );
    printf(    
    "bDescriptorType = 0x%x\n", cd->bDescriptorType
    );
    printf(        
    "wTotalLength = 0x%x\n", cd->wTotalLength
    );
    printf(    
    "bNumInterfaces = 0x%x\n", cd->bNumInterfaces
    );
    printf(    
    "bConfigurationValue = 0x%x\n", cd->bConfigurationValue
    );
    printf(    
    "iConfiguration = 0x%x\n", cd->iConfiguration
    );
    printf(    
    "bmAttributes = 0x%x\n", cd->bmAttributes
    );
    printf(    
    "MaxPower = 0x%x\n", cd->MaxPower    
    );
}

void 
print_id(PUSB_INTERFACE_DESCRIPTOR id)
{
    printf("Interface\n");
    printf("---------\n");    
    printf(
    "bLength = 0x%x\n", id->bLength
    );
    printf(    
    "bDescriptorType = 0x%x\n", id->bDescriptorType
    );
    printf(        
    "bInterfaceNumber = 0x%x\n", id->bInterfaceNumber
    );
    printf(    
    "bAlternateSetting = 0x%x\n", id->bAlternateSetting
    );
    printf(    
    "bNumEndpoints = 0x%x\n", id->bNumEndpoints
    );
    printf(    
    "bInterfaceClass = 0x%x\n", id->bInterfaceClass
    );
    printf(    
    "bInterfaceSubClass = 0x%x\n", id->bInterfaceSubClass
    );
    printf(    
    "bInterfaceProtocol = 0x%x\n", id->bInterfaceProtocol    
    );
    printf(    
    "bInterface = 0x%x\n", id->iInterface    
    );
}

void 
print_ed(PUSB_ENDPOINT_DESCRIPTOR ed)
{
    printf("Endpoint\n");
    printf("---------\n");    
    printf(
    "bLength = 0x%x\n", ed->bLength
    );
    printf(    
    "bDescriptorType = 0x%x\n", ed->bDescriptorType
    );
    printf(        
    "bEndpointAddress= 0x%x\n", ed->bEndpointAddress
    );
    printf(    
    "bmAttributes= 0x%x\n", ed->bmAttributes
    );
    printf(    
    "wMaxPacketSize= 0x%x\n", ed->wMaxPacketSize
    );
    printf(    
    "bInterval = 0x%x\n", ed->bInterval
    );
}

void
rw_dev()
{
	BOOLEAN success;
	int siz, nBytes, i;
	char buf[256], chBuff[80];
    PUSB_CONFIGURATION_DESCRIPTOR cd;
    PUSB_INTERFACE_DESCRIPTOR id;
    PUSB_ENDPOINT_DESCRIPTOR ed;

	siz = sizeof(buf);

	if (hDEV == INVALID_HANDLE_VALUE) {
		NOISY(("DEV not open"));
		return;
	}
	
	success = DeviceIoControl(hDEV,
			IOCTL_I82930_GET_CONFIG_DESCRIPTOR,
			buf,
			siz,
			buf,
			siz,
			&nBytes,
			NULL);

	NOISY(("request complete, success = %d nBytes = %d\n", success, nBytes));
	
	if (success) {
        int i, j;
        char *pch;
        
        pch = buf;

        cd = (PUSB_CONFIGURATION_DESCRIPTOR) pch;
        print_cd((PUSB_CONFIGURATION_DESCRIPTOR) cd);
        pch += cd->bLength;

        do {
            
            id = (PUSB_INTERFACE_DESCRIPTOR) pch;
            print_id(id);
            pch += id->bLength;
            for (j=0; j<id->bNumEndpoints; j++) {
                ed = (PUSB_ENDPOINT_DESCRIPTOR) pch; 
                print_ed(ed);
                pch += ed->bLength;
            }
            i = pch - buf;
        } while (i<cd->wTotalLength);       
        
	}
	
	return;

}


int _cdecl main(
    int argc,
	char *argv[])
{

	if (open_dev("I82930-0"))
		rw_dev();

	return 0;
}
