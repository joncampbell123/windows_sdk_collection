/****************************************************************************

Copyright (c) 1995  Microsoft Corporation

Module Name:

    utbd.c

Abstract:

    Ring3 loopback app output routines

Environment:

    user mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:

    4-4-96 : created

****************************************************************************/

#include "winutb.h"


/*************************************************************************
FUNCTION: 	DisplayOutput

INPUT:		variable argument list

OUTPUT:		void
						
AUTHOR:		BradCa

DATE:		4/5/96

PURPOSE:	Outputs passed in data to the log window.
*************************************************************************/
void DisplayOutput(LPSTR pString,...)
{
	 char   Buffer[512];
	 MSG    msg;
	 int    iIndex;

	 va_list list;

	 va_start(list,pString);

	 vsprintf(Buffer,pString,list);

	 if (ulCount == MAX_LOOP)
	 {
		ulCount = 0;
		SendMessage(hLogWindow,LB_RESETCONTENT,0,0);		
	 }
	 
	 iIndex=SendMessage(hLogWindow,LB_ADDSTRING,0,(LPARAM)Buffer);
	 SendMessage(hLogWindow,LB_SETCARETINDEX,iIndex,(LPARAM)MAKELONG(FALSE,0));

	 while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	 {
	  TranslateMessage(&msg);
	  DispatchMessage(&msg);
	 }

	 UpdateWindow(hLogWindow);
}

/************************************************************
FUNCTION: 	OutputDeviceDesc

INPUT:		pDevDesc - ptr to device descriptor

DATE:		4/5/96

PURPOSE:	Output device decsriptor info to the screen.
************************************************************/
void OutputDeviceDesc(PUSB_DEVICE_DESCRIPTOR pDevDesc)
{
	DisplayOutput("********** DEVICE DESCRIPTOR **********");
	DisplayOutput("bLength =            0x%02X", pDevDesc->bLength);
	DisplayOutput("bDescriptorType =    0x%02X", pDevDesc->bDescriptorType);
	DisplayOutput("bcdUSB =             0x%04X", pDevDesc->bcdUSB);
	DisplayOutput("bDeviceClass =       0x%02X", pDevDesc->bDeviceClass);
	DisplayOutput("bDeviceSubClass =    0x%02X", pDevDesc->bDeviceSubClass);
	DisplayOutput("bDeviceProtocol =    0x%02X", pDevDesc->bDeviceProtocol);
	DisplayOutput("bMaxPacketSize0 =    0x%02X", pDevDesc->bMaxPacketSize0);
	DisplayOutput("idVendor =           0x%04X", pDevDesc->idVendor);
	DisplayOutput("idProduct =          0x%04X", pDevDesc->idProduct);
	DisplayOutput("bcdDevice =          0x%02X", pDevDesc->bcdDevice);
	DisplayOutput("iManufacturer =      0x%02X", pDevDesc->iManufacturer);
	DisplayOutput("iProduct =           0x%02X", pDevDesc->iProduct);
	DisplayOutput("iSerialNumber =      0x%02X", pDevDesc->iSerialNumber);
	DisplayOutput("bNumConfigurations = 0x%02X", pDevDesc->bNumConfigurations);

	if(pDevDesc->bNumConfigurations > 1)
		DisplayOutput("MULTIPLE CONFIG DEVICE!!");

	DisplayOutput("**************************************************");	

}

/************************************************************
FUNCTION: 	OutputConfigDesc

INPUT:		pConfigDesc - ptr to config descriptor

DATE:		4/5/96

PURPOSE:	Output config decsriptor info to the screen.
************************************************************/
void OutputConfigDesc(PUSB_CONFIGURATION_DESCRIPTOR pConfigDesc)
{
	DisplayOutput("********** CONFIG DESCRIPTOR **********");
	DisplayOutput("bLength =             0x%02X", pConfigDesc->bLength);
	DisplayOutput("bDescriptorType =     0x%02X", pConfigDesc->bDescriptorType);	
	DisplayOutput("wTotalLength =        0x%02X", pConfigDesc->wTotalLength);	
	DisplayOutput("bNumInterfaces =      0x%02X", pConfigDesc->bNumInterfaces);	
	DisplayOutput("bConfigurationValue = 0x%02X", pConfigDesc->bConfigurationValue);	
	DisplayOutput("iConfiguration =      0x%02X", pConfigDesc->iConfiguration);	
	DisplayOutput("bmAttributes =        0x%02X", pConfigDesc->bmAttributes);
	DisplayOutput("MaxPower =            0x%02X", pConfigDesc->MaxPower);

	if(pConfigDesc->bNumInterfaces > 1)
		DisplayOutput("MULTIPLE INTERFACE DEVICE!!");

	DisplayOutput("**************************************************");	
}

/************************************************************
FUNCTION: 	OutputInterfaceDesc

INPUT:		pIntfDesc - ptr to interface descriptor

DATE:		4/5/96

PURPOSE:	Output interface decsriptor info to the screen.
************************************************************/
void OutputInterfaceDesc(PUSB_INTERFACE_DESCRIPTOR pIntfDesc)
{
	DisplayOutput("********** INTERFACE DESCRIPTOR **********");
	DisplayOutput("bLength =             0x%02X", pIntfDesc->bLength);
	DisplayOutput("bDescriptorType =     0x%02X", pIntfDesc->bDescriptorType);	
	DisplayOutput("bInterfaceNumber =    0x%02X", pIntfDesc->bInterfaceNumber);	
	DisplayOutput("bAlternateSetting =   0x%02X", pIntfDesc->bAlternateSetting);	
	DisplayOutput("bNumEndPoints =       0x%02X", pIntfDesc->bNumEndpoints);	
	DisplayOutput("bInterfaceClass =     0x%02X", pIntfDesc->bInterfaceClass);	
	DisplayOutput("bInterfaceSubClass =  0x%02X", pIntfDesc->bInterfaceSubClass);	
	DisplayOutput("bInterfaceProtocol =  0x%02X", pIntfDesc->bInterfaceProtocol);	
	DisplayOutput("iInterface =          0x%02X", pIntfDesc->iInterface);	
	DisplayOutput("**************************************************");	
}

/************************************************************
FUNCTION: 	OutputEndPtDesc

INPUT:		pEndPtDesc - ptr to endpoint descriptor
			ulNum - endpoint number

DATE:		4/5/96

PURPOSE:	Output endpoint decsriptor info to the screen.
************************************************************/
void OutputEndPtDesc(PUSB_ENDPOINT_DESCRIPTOR pEndPtDesc, ULONG ulNum)
{
	DisplayOutput("********** ENDPOINT DESCRIPTOR #%ld **********", ulNum);	
	DisplayOutput("bLength =             0x%02X", pEndPtDesc->bLength);	
	DisplayOutput("bDescriptorType =     0x%02X", pEndPtDesc->bDescriptorType);	
	DisplayOutput("bEndpointAddress =    0x%02X", pEndPtDesc->bEndpointAddress);	
	DisplayOutput("bmAttributes =        0x%02X", pEndPtDesc->bmAttributes);	
	DisplayOutput("wMaxPacketSize =      0x%02X", pEndPtDesc->wMaxPacketSize);	
	DisplayOutput("bInterval =           0x%02X", pEndPtDesc->bInterval);	
	DisplayOutput("**************************************************");	
}

//***************************************************************************
//
// ctlParseCmdLine()
//
// Parse command line.
//
// ENTRY:
//  szCmdLine -  Command line.
//
// EXIT:
//  BOOL - Tells whether or not we are automating this test run.
//
// 			NOTE: This code was stolen from the setup src tree.
//	
//			MODIFIED - BradCa - 5-3-95 - removed win95 setup based
//						code and use the parsing guts.
// 
//***************************************************************************

long ctlParseCmdLine( LPCSTR szCmdLine )
{
    int i;
    LPCSTR pLine, pArg;
    char szTmpBuf[ MAX_BUFFER_SIZE ];
	BOOL bInput = FALSE;
	BOOL bOutput = FALSE;
	
    pLine = szCmdLine;

    while ( *pLine != EOL)
    {
	        // Move to first non-white char.
        pArg = pLine;
        while ( isspace( (int) *pArg ) )
            pArg++;

        if ( *pArg == EOL )
            break;

    	    // Move to next white char.
        pLine = pArg;
        while ( (*pLine != EOL) && (!isspace((int) *pLine)) )
            pLine++;

	        // Copy Arg. to buffer.
        i = pLine - pArg + 1;   // +1 for NULL.

        lstrcpyn( szTmpBuf, pArg, i );

        if ( szTmpBuf[0] == '/' || szTmpBuf[0] == '-')
        {

		        // Look for other switches
            switch( toupper( szTmpBuf[1] ) )
            {
                case    'C':	// auto close
                    if ( szTmpBuf[2] != EOL )
                        bAutoClose = TRUE;
                    break;
                case    'I':	// input pipe number
                    if ( szTmpBuf[2] != EOL )
                    	iINpipe = atoi( szTmpBuf+2 );
                    bInput = TRUE;
                    break;

                case    'J':    // input pipe buffer
					if ( szTmpBuf[2] != EOL )
                    	iInputBuffLen = atoi( szTmpBuf+2 );
                    break;
                    
                case    'K':    // instance of driver
                    if ( szTmpBuf[2] != EOL )
                    	iInstance = atoi( szTmpBuf+2 );
                    break;
                    
                case    'L':    // number of iterations
                    if ( szTmpBuf[2] != EOL )
                    	iIterations = atoi( szTmpBuf+2 );
                    break;
                    
                case    'O':    // output pipe number
                    if ( szTmpBuf[2] != EOL )
                    	iOUTpipe = atoi( szTmpBuf+2 );
                    bOutput = TRUE;                   	
                    break;
                    
                case    'P':    // output pipe buffer
                    if ( szTmpBuf[2] != EOL )
                    	iOutputBuffLen = atoi( szTmpBuf+2 );
                    break;

                default:
                	return USAGE_ERROR;
			}
		}
	}		

//	wsprintf(chBuff, "%ld %ld %ld %ld %ld %ld", iINpipe, iInputBuffLen, 
//			iInstance, iIterations, iOUTpipe, iOutputBuffLen);
//	MessageBox(NULL, chBuff, szAppName, MB_OK);

	if(bInput && bOutput)
		return TRUE;

	return FALSE;
}

