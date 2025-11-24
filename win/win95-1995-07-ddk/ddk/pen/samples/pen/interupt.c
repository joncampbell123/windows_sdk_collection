/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/* INTERUPT.c: 

	This file contains all interrupt time processing code.

	Routines contained in module:
		Deal_With_Pen_Packets
		Pen_DoMouseEvent
		Pen_SendMouseEvent
		Pen_ActLikeMouse
*/

#include <Windows.h>
#define NOPENAPPS
#include <penwin.h>

#include "PenDrv.h"

UINT viNumberOfHookRoutines=0;

int viActingLikeMouse;
HOOKROUTINE vrglpfnProcessTabletEvents[MAXHOOKS+1]={0};

// file global variables
DRV_PENPACKET rgTempPenPackets[MAX_BUFFER_SIZE];


DWORD vdwDataType;
DWORD vdwNumberOfPenPackets;
DWORD vdwOffsetIntoBuffer;
DWORD vdwIndexIntoBuffer;
DRV_PENPACKET PenPacketBuffer[MAX_BUFFER_SIZE]={0xcccc};
DRV_PENINFO vpiPenInfo;
char szOEMBuffer[1];

WORD wOldPDK;
BYTE rgbButtons[]={0,SF_B1_DOWN,SF_B1_UP,0};

int viLastX;
int viLastY;
int viLastNormalX;
int viLastNormalY;

WORD vwOldPDK;

extern void (FAR PASCAL MOUSE_EVENT)(void);

// local prototypes
DWORD NEAR PASCAL Pen_DoMouseEvent(LPDRV_PENPACKET lpppHead);
void NEAR PASCAL Pen_SendMouseEvent(WORD wNormX, WORD wNormY, WORD wState);
void NEAR PASCAL Pen_ActLikeMouse(LPDRV_PENPACKET lpppHead);



//*****************************************************************************
// The idea is to call all clients who have registered to receive
// interrupt-time information.	This includes penwin.dll and any other
// DLL that has registered a hook.
//
// By the time this routine is called, VpenD will have set:
//
// vdwDataType
// vdwNumberOfPenPackets
// vdwOffsetIntoBuffer
// vdwIndexIntoBuffer
//
// So, in order to handle this information properly, need to check the
// DataType to see if the Virtual pen driver is sending through pen packets
// and if so, pass them on.
//
//*****************************************************************************
void _cdecl _interrupt _loadds _export Deal_With_Pen_Packets(void)
{
	int i;
	DWORD dwHRValue=HR_CONTINUE;

	LPDRV_PENPACKET lppp=(LPDRV_PENPACKET)&PenPacketBuffer[(int)vdwIndexIntoBuffer];
	LPFNHOOKROUTINE lpFunction=&vrglpfnProcessTabletEvents[0];
	_asm sti

    // Check to see if there is more then one pen packet and if it wraps
    // around in the buffer.  If so, copy it into a buffer and then
    // call the hooked routines.  Keep in mind that the index into the buffer
    // is zero based!!!  Thus MAX_BUFFER_SIZE-1 is the zero-based size of
    // the buffer.

    if(vdwIndexIntoBuffer+vdwNumberOfPenPackets>MAX_BUFFER_SIZE)
    {
		int k=0;
		for(i=(int)vdwIndexIntoBuffer;i<MAX_BUFFER_SIZE;i++)
		{
			rgTempPenPackets[k++]=*lppp;
			lppp++;
		}

		lppp=(LPDRV_PENPACKET)&PenPacketBuffer[0];
		for(i=0;i< ((int)(vdwIndexIntoBuffer+vdwNumberOfPenPackets)-MAX_BUFFER_SIZE);i++)
		{
			rgTempPenPackets[k++]=*lppp;
			lppp++;
		}

		lppp=(LPDRV_PENPACKET)&rgTempPenPackets[0];
    }
//
// !!Note!! Make sure that the pointers are valid before calling
// the hooked routine.	If the caller failed, or just forgot to 
// deinstall itself, this code must not fail.
//
	for(i=0;i<MAXHOOKS;i++)
	{
		switch((WORD)vdwDataType)
		{
			case DT_OORPENPACKET:
			case DT_PENPACKET:
				{
				if(*lpFunction)
					if( (dwHRValue=(*lpFunction)((WORD)i+1,
								(WORD)DT_PENPACKET, //(WORD)vdwDataType,
								(WPARAM)(WORD)(vdwNumberOfPenPackets),
								(LPARAM)(LPBYTE)lppp)) == HR_STOP )
					{
						// Caller doesn't want the pen driver to continue to send info
						// into the system.
						dwHRValue=HR_CONTINUE;
						return;
					}
				}
				break;
			default:
				break;
	    }
	    lpFunction++;
	}


// Has someone registered to handle the mouse moves?	If not, this code will do
// it.	-1 means that this code will handle it.

	if( (viActingLikeMouse==-1) &&
		((WORD)vdwDataType & (DT_PENPACKET | DT_OORPENPACKET)) )
	{
		Pen_ActLikeMouse(lppp);
	}
}


/*
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// This routine walks the buffer of pen packets and sends only the
// important pen packets.  If the buffer only has one pen packet in it,
// that pen packet will be sent.  If the buffer has more then one packet,
// the buffer will be walked, and if the wPDK value of the next pen packet is
// the same as the wPDK of the current pen packet, the current pen packet will
// be skipped unless it's the last pen packet in the buffer.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void NEAR PASCAL Pen_ActLikeMouse(LPDRV_PENPACKET lpppHead)
	{
   BOOL bLastOneSent;
	int i =(int)vdwNumberOfPenPackets;
	int iUnitsUntilMoveX = (WORD)vpiPenInfo.cxRawWidth
		/ GetSystemMetrics(SM_CXSCREEN);
	int iUnitsUntilMoveY = (WORD)vpiPenInfo.cyRawHeight
		/ GetSystemMetrics(SM_CYSCREEN);

	while(i)
		{
		bLastOneSent=FALSE;
		if((WORD)lpppHead->wPDK != vwOldPDK) //viLastState)
			{
			// The state of the pen has changed. Should always update!!
			Pen_DoMouseEvent(lpppHead);

			// Sent one, pretend it was the last one.  If it is not, this
			// variable will get changed the next time through the loop.
			bLastOneSent=TRUE;
			}
		i--;
		if(i!=0)  // advance for next one
			lpppHead++;
		}

	// Always send the last packet if the difference in its position information
        // is great enough (unless it was sent above).
	if(!bLastOneSent)
		{
		if (((int)lpppHead->wTabletX > viLastX+iUnitsUntilMoveX) || // greater
			 ((int)lpppHead->wTabletX < viLastX-iUnitsUntilMoveX) || // less
			 ((int)lpppHead->wTabletY > viLastY+iUnitsUntilMoveY) ||
			 ((int)lpppHead->wTabletY < viLastY-iUnitsUntilMoveY))
			Pen_DoMouseEvent(lpppHead);
		}
	}

/*
-------------------------------------------------------------------------------
There are nine possible states that need to be mapped to five that are
reported to the system.  The possible nine states are listed below.
(OutOfRange messages are special case messages, generally disregard.)

					      new input
			 0000		0003		  0007		 4000
                      -|----------------------------------------------------------
  Old input	0000   | SF_MOVEMENT   	SF_B1_DOWN	SF_B2_DOWN	 no-op
		0003   | SF_B1_UP	SF_MOVEMENT	eaten		 SF_B1_UP
		0007   | SF_B2_UP	eaten		SF_MOVEMENT  	 SF_B2_UP
		4000   | SF_MOVEMENT   	SF_B1_DOWN	SF_B2_DOWN	 no-op

The five reportable states are listed in the above matrix. They are: SF_MOVEMENT, 
SF_B1_UP, SF_B2_UP, SF_B1_DOWN, and SF_B2_DOWN.

To determine the current state the New PDK value will be subtracted from
the Old PDK value. This will result in the following table.

SubValue=(OldState-NewState)+3

			         new input
			0000   0001    0002   0003
                       ----------------------------------------------
  Old input	0000	   3	  2	1	0
		0001	   4	  3	2	1
		0002	   5	  4	3	2
		0003	   6	  5	4	3

AddValue=OldState+NewState

			         new input
			0000   	0001   0002    0003
                        -----------------------------
 Old input	0000	   0	  1	2	3
		0001	   1	  2	3	4
		0002	   2	  3	4	5
		0003	   3	  4	5	6
-------------------------------------------------------------------------------
*/
DWORD NEAR PASCAL Pen_DoMouseEvent(LPDRV_PENPACKET lppp)
{
	WORD wNormX,wNormY,wState;
	WORD wNewPDK,wOldPDK,wSubValue,wAddValue;
	WORD wRawH,wRawW;

	// On the first out-of-range packet that is received, change the state of the
	// pen if it quickly moved out of range from a down state.
	if(lppp->wPDK & PDK_OUTOFRANGE)
	{
		// second OOR packet, do nothing
		if(vwOldPDK & PDK_OUTOFRANGE)
		{
			vwOldPDK=lppp->wPDK;
			//viLastState=lppp->state;
			return 1;
		}
		else // this is the first out-of-range packet
		{
			WORD wState=(vwOldPDK & (PDK_DOWN | PDK_BARREL1));

			// Was the Left button down?
			if( wState == PDK_DOWN)
			{
				Pen_SendMouseEvent(viLastNormalX,viLastNormalY,SF_B1_UP);
			}

			// Was the right button down?
			if( wState == (PDK_DOWN | PDK_BARREL1) )
			{
				Pen_SendMouseEvent(viLastNormalX,viLastNormalY,SF_B2_UP);
			}

			// update last state to OOR
			vwOldPDK=lppp->wPDK;
			return 1;
		}
	}

	// Should have a valid pen packet to report.  Normalize the
	// coordinate, determine the next state transition and notify USER.
	wNormX=lppp->wTabletX;
	wNormY=lppp->wTabletY;

//
// Compute current X and Y in normalized screen coordinates (not screen
// coordinates). For example, X=(tabletXpnt*65535)/TabletWidth and
// Y=(tabletYpnt*65535)/TabletHeight)
//
	wRawW=(WORD)vpiPenInfo.cxRawWidth;
	wRawH=(WORD)vpiPenInfo.cyRawHeight;

#ifdef DEBUG
	// can't divide by zero
	if( (wRawW==0) || (wRawH==0) )
		_asm int 3
#endif
	_asm {
		mov	dx,wNormX
		xor	ax,ax
		mov	cx,wRawW
		div	cx
		mov	wNormX,ax

		mov	dx,wNormY
		xor	ax,ax
		mov	cx,wRawH
		div	cx
		mov	wNormY,ax
	}


	// retreive state, then AND it with 0003h
	wNewPDK=(lppp->wPDK & (PDK_DOWN | PDK_BARREL1));
	wOldPDK=(vwOldPDK & (PDK_DOWN | PDK_BARREL1));
	wSubValue=(wOldPDK-wNewPDK)+3;

	switch(wSubValue)
	{
		case 1:
		case 3:
		case 5:
			wState=SF_MOVEMENT;
			break;
		case 0:
			wState=SF_B2_DOWN;
			//vwOldPDK=lppp->state;
			break;
		case 6:
			wState=SF_B2_UP;
			//vwOldPDK=lppp->state;
			break;
		case 2:
			wAddValue=wNewPDK+wOldPDK;
			if(wAddValue==1)
				wState=SF_B1_DOWN;
			if(wAddValue==3)
				wState=SF_B1_UP;
			if(wAddValue==5)
				wState=SF_B2_DOWN;
			//vwOldPDK=lppp->state;
			break;
		case 4:
			wAddValue=wNewPDK+wOldPDK;
			if(wAddValue==1)
				wState=SF_B1_UP;
			if(wAddValue==3)
				wState=SF_B1_DOWN;
			if(wAddValue==5)
				wState=SF_B2_UP;
			//vwOldPDK=lppp->state;
			break;
		default:
			// Error
			break;
	}

	Pen_SendMouseEvent(wNormX,wNormY,wState);

	// save these coordinates for next time through
	viLastNormalX=(int)wNormX;
	viLastNormalY=(int)wNormY;

	viLastX=(int)lppp->wTabletX;
	viLastY=(int)lppp->wTabletY;
	vwOldPDK=lppp->wPDK;

	//viLastState=(int)lppp->state;

	return 1;
}

void NEAR PASCAL Pen_SendMouseEvent(WORD wNormX, WORD wNormY, WORD wState)
{
	_asm {

		mov		ax,wState	; button State
		or		ax,SF_ABSOLUTE
		mov		bx,wNormX	; normalized X coordinate
		mov		cx,wNormY	; normalized Y coordinate
		mov		dx,2		; number of buttons
		xor		si,si		; clear extra data registers
		xor		di,di		;
		}

	MOUSE_EVENT();
}

// End-Of-File
