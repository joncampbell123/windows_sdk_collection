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

#include <stdio.h>
unsigned long dwTime ;

void main (void)
{
_asm {
	push	si
	push	di
	xor	di, di
	mov	es, di
	mov	ax, 1684h
	mov	bx, 7FE1h
	int	2fh
	mov	ax, es
	or	ax, di
	jz	error_message

	mov	ax, offset ret_place
	push	cs
	push	ax
	push	es
	push	di
	retf			; far return to VxD entry point

ret_place:
	mov	word ptr [dwTime+2], dx
	mov	word ptr [dwTime], ax
	pop	di
	pop	si
	}
	printf ("This VM was born at %lu milliseconds.", dwTime) ;
	return ;

error_message:
	_asm {
	pop	di
	pop	si
	}
	puts ("VwatchD API not found.") ;
	return ;
}
