/*****************************************************************************\
*                                                                             *
*    Application.c                                                            *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "except.h"

#include "types.h"
#include "errors.h"
#include "SegLoad.h"
#include <Resources.h>
#include "memory.h"

FailInfo		*gTopHandler;
short			gLastError;
long			gLastMessage;
char			gAskFailure = false;
char			gDefaultPropagation = true;

extern FailInfo	*gTopHandler;

void NoHandler(void)
{
#ifdef DEBUG
	DebugStr((StringPtr)"\pfailure stack is empty!");
#endif

	ExitToShell();
}

void PushTryHandler(FailInfo *fi)
{
	fi->fPropagate = gDefaultPropagation;
	fi->next = gTopHandler;
	gTopHandler = fi;
}


void FailMemError(void)
{
	OSErr	err = MemError();

	if (err != noErr)
		Failure(err, 0);
}

void FailNIL(void* p)
{	
	if (!p)
		Failure(memFullErr, 0);
}

void FailNILRes(void* p)
{
	if (!p)
	{
		OSErr err = ResError();
		Failure((OSErr)(err ? err : resNotFound), 0);
	}
}

void FailOSErr(OSErr err)
{
	if (err != noErr)
		Failure(err, 0);
}

void FailResError(void)
{
	OSErr	err = ResError();
	
	if (err != noErr)
		Failure(err, 0);
}

void Failure(OSErr err, long message)
{	
	FailInfo	*handler;
	
	if (gTopHandler)
	{
		gLastError = err;
		gLastMessage = message;
		
		handler = gTopHandler;
		gTopHandler = handler->next;
		
		ThrowHandler(handler);
	}
	else
	{
		DebugStr("\pFailure: No handler");
		ExitToShell();
	}
}

void Success(void)
{
	if (gTopHandler)
		gTopHandler = gTopHandler->next;
	else
		NoHandler();
}

void RetryException(FailInfo *fi)
{
	gLastError = noErr;
	gLastMessage = 0;
	
	fi->next = gTopHandler;
	gTopHandler = fi;
	
	ThrowHandler(fi);
}

void ThrowHandler(FailInfo *fi)
{
	longjmp(fi->regs, gLastError);
}
