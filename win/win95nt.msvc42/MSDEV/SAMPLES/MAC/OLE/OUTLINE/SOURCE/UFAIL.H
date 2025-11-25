/*------------------------------------------------------------------------------
#
#	Apple Macintosh Developer Technical Support
#
#	Exception handling for MPW Pascal, MacApp and MPW C
#
#	UFailure (aka Signals) - "Exceptional code, with a few exceptions."
#
#	UFailure.h	-	C header
#
#	Copyright c 1985-1988 Apple Computer, Inc.
#	All rights reserved.
#
#	Versions:	1.00				11/88
#				1.01				06/92
#
#	Components:	CTestSignal.c		November 1, 1988
#				CTestSignal.make	November 1, 1988
#				PTestSignal.p		November 1, 1988
#				PTestSignal.make	November 1, 1988
#				UFailure.p			November 1, 1988
#				UFailure.h			November 1, 1988
#				UFailure.incl.p		November 1, 1988
#				UFailure.a			November 1, 1988
#
#	UFailure (or Signals) is a set of exception handling routines suitable for
#	use with MacApp, MPW C, and MPW Pascal. It is a jazzed-up version of the MacApp
#	UFailure unit. There is a set of C interfaces to it as well.
#
------------------------------------------------------------------------------*/


#ifndef __UFAILURE_H__
#define __UFAILURE_H__

#include "debug.h"

/*
Theory:
	See the "theory of operation" comments below, and the commentary in Technical
	Note #88.
	
	*** New ***
	The warnings in technote 88 about not using CatchSignal in an expression, etc. no
	longer apply. The exact state of the routine is now restored (all non-scratch
	registers are preserved). Also, a fixed-size nonrelocatable block is now used, so
	there is a limit on the depth of nested CatchSignals. This may be adjusted by
	changing the constant SigBlockSize in UFailure.a and rebuilding the unit. For most
	applications, though, the default depth of eight will be more than sufficient.
	
	Note:
	*** There is a special c version of CatchFailures called CatchCFailures. If you use it
	you will be required to call FreeSignal or Success explicitly (as with CatchFailures in
	Pascal). There is no c version of CatchFailures because c doesn't support the concept
	of nested procedures. CatchCFailures provides the same functionality. ***
	
	*** You can use either the technote mechanism or the MacApp one in your program. It
	is OK to call Signal if CatchFailures was used, and OK to call Failure if
	CatchSignal was used, i.e. the two schemes may be freely intermixed. ***
	
		
	What this version adds to the MacApp mechanism is two things:
		1.
			Exception records (FailInfo) are taken from a special heap block when
			you use CatchSignal, so you don't have to pass a FailInfo record as a
			parameter.
		2.
			You can take advantage of stack frames and use of Signals as described
			in Technical Note #88, without being forced to make explicit calls to
			FreeSignal. This unit is fully upwards compatible with the 1986 version
			of the technote.
			
	This includes all of the MacApp calls from the original UFailure, of course.

					T H E O R Y   O F   O P E R A T I O N

	This unit implements the MacApp and Signal failure mechanisms.

	The failure mechanism is built around exception handlers.  An exception
	handler is a piece of code, generally local to some other routine, that is
	called when a failure occurs and takes action to handle the failure.
	An exception handler is of the form

		PROCEDURE ExceptionHandler (error: OSErr; message: LONGINT);
									or
		the handler may consist of execution returning to the point of a
		CatchSignal

	where error is the error that caused the failure, and message identifies
	the error message that may be displayed.  Consider a routine that opens
	a file, reads its contents, and closes the file.  If a failure occured
	while reading the file, an exception handler would be needed to close the
	file, as the rest of the routine would not be executed.  (See TestCignal
	and TestSignal for examples of how to use these calls.)

	References to exception handlers are kept in the FailInfo record. The
	exception handlers form a linked-list via the nextInfo field of FailInfo.
	The linked list is a stack since new exception handlers are added to the
	front of the list.

	New exception handlers are added to the stack with the CatchSignal
	function or CatchFailures procedure. They are removed from the stack
	automatically (CatchSignal) or via the Success procedure (if CatchFailures
	was used). In general you call CatchFailures/CatchSignal to post an exception
	handler when an error the application should handle might occur. You
	can then manually pop the last handler off the stack with FreeSignal or
	Success, if necessary. You may want to pop the handler (even if you used
	CatchSignal) after the possibility of a specific type of error no longer
	exists. Subsequent exceptions would then be passed to a previous (more
	general) handler.
	
	Any failure detected within the limits of the CatchFailures/CatchSignal call
	results in the execution of the exception handler.  (Failure does
	not have to occur in the same routine as your call to CatchFailures.
	The failure may occur in any routine called after the catch but before the
	implicit/explicit pop of the handler.)

	When MacApp (or your code) determines that a failure has occured, it
	calls Failure or Signal.  As a convenience, several procedures are provided
	to check for standard kinds of failures and call Failure if needed.
	These procedures are:

		FailNIL			Calls Failure if its parameter is NIL.
		FailOSErr		Calls Failure if its parameter is not noErr.
		FailMemError	Calls Failure if MemError returns other than noErr.
		FailResError	Calls Failure if ResError returns other than noErr.

	When the exception is raised, execution of the routine that signalled is
	terminated and the exception handler at the top of the stack is popped.
	For each routine that was called after the handler was posted
	to the stack, execution is terminated as though from an EXIT statement,
	and the exception handler is called.  It generally cleans up for the
	routine in which it is nested.  Upon completion the next exception handler
	is popped from the stack, repeating the process.

	The error causing the failure, and a message code is passed to the handler.
	
	MacApp Specifics
	For MacApp, the last exception handler on the stack is the one in
	TApplication.PollEvent.  It calls TApplication.ShowError, which calls
	ErrorAlert, which decodes the message and displays an alert.  Your exception
	handlers may set the message code to one more specific to your application
	by calling FailNewMessage at the end of your exception handler.
	FailNewMessage changes the message only if the current one is non-zero.
	This has the effect of allowing those exception handlers closest to the
	source of the error to set the message.

	One last note about exception handlers:  It is possible for an exception
	handler to terminate exception processing by using a non-local GOTO to
	jump back into the routine in which the exception handler is nested.  This
	is how MacApp keeps the application running when a failure occurs.  The
	last exception handler on the stack, in TApplication.PollEvent, uses a
	GOTO to continue event processing.
*/

typedef struct {
	long			regs[11];			/* D3-D7/A2-A7 */
	short			error;
	long			message;
	long			failA6;
	long			failPC;
	Ptr				nextInfo;
	/* this is needed for compatibility with MacApp debugging */
	long			whoPC;
	/* these are added for USignalFailure unit use */
	short			whatSignals;
	/* this is used to keep the old stack frame return address */
	long			sigFRet;
} FailInfo, *PFailInfo;


#ifndef _MSC_VER
typedef pascal void (*HandlerFuncPtr)(short error, long message);
#else
typedef void (__pascal *HandlerFuncPtr)(short error, long message);
#endif


#ifndef _MSC_VER
#define EXTERN_API_(type)		extern pascal type
#else
#define EXTERN_API_(type)		extern type __pascal
#endif


/* Call the following initialization routine before your other initializations (InitGraf, etc.)-
 in other words as early as you can in the application. */

EXTERN_API_(void) InitUFailure();
	/* Allocates the heap block for CatchSignals and initializes the global
		variables used by the unit. C programs must use this instead of InitSignals. */


EXTERN_API_(short) CatchSignal();
	/* Until the function which encloses this call returns, this will catch
		subsequent Signal calls, returning the code passed to Signal.  When
		CatchSignal is encountered initially, it returns a code of zero.  These
		calls may "nest"; i.e. you may have multiple CatchSignals in one function.
		Each nested CatchSignal call uses 72 bytes of heap space.
		If you signal with Failure and pass in a non-zero message you should use
		CatchCFailures instead so you have a way of getting at the message. */

EXTERN_API_(void) FreeSignal();
	/* This undoes the effect of the last CatchSignal/CatchFailures.  A Signal will then invoke
		the CatchSignal prior to the last one. */
		
EXTERN_API_(void) Signal(short code);
	/* Returns control to the point of the last CatchSignal/CatchFailures.  The program will
		then behave as though that CatchSignal had returned with the code parameter
		supplied to Signal. If CatchCFailures is catching, the message parameter will be 0. */
		
EXTERN_API_(void) SignalMessage(short code, long message);
	/* Returns control to the point of the last CatchSignal. If CatchCFailures is catching,
		the message parameter will be returned. */

		
		
/*------------------------------------+
|			MacApp routines			  |
+------------------------------------*/



#ifndef _MSC_VER
pascal long BuildMessage(short lowWord, short highWord)
			= 0x2E9F;	/*  MOVE.L		(A7)+,(A7)	 */
	/* Takes the 2 integers and combines them into a LONGINT.  Note that the
		low-order word is the first parameter. */
#endif

EXTERN_API_(void) CatchCFailures(FailInfo *fi, HandlerFuncPtr handler);
	/* Call this to set up an exception handler. This pushes your handler onto
		a stack of exception handlers. */

EXTERN_API_(void) Failure(short error, long message);
	/* Call this to signal a failure.  Control will branch to the most recent
		exception handler, which will be popped off the handler stack. */

EXTERN_API_(void) PropagateFailure(void);
	/* Call this to propagate a failure to the next signal. */



EXTERN_API_(void) FailNewMessage(short error, long oldMessage, long newMessage);
	/* This does:
		if (oldMessage == 0)
			Failure(error, newMessage);
		else
			Failure(error, oldMessage);
	 */

#ifndef _MSC_VER
EXTERN_API_(void) InitUFailure();
	/* Allocates the heap block for CatchSignals and initializes the global
		variables used by the unit. C programs must use this instead of InitSignals. */

EXTERN_API_(void) FailMemError();
	/* If MemError != noErr then call Failure(MemError, 0);  If you are using
		assembler, then you should just test the return code from the Memory
		Manager in DO by calling FailOSErr.	(See the discussion of MemError in
		Inside Macintosh.) */

EXTERN_API_(void) FailNIL(void *);
	/* Call this with a pointer/handle; this signals Failure(memFullErr, 0) iff
		the pointer is nil. */

EXTERN_API_(void) FailOSErr(short error);
	/* Call this with an OSError; signals Failure(error, 0) iff error != noErr. */

EXTERN_API_(void) FailResError();
	/* If ResError != noErr then call Failure(ResError, 0); (See Inside Macintosh.) */

#else
#define InitUFailure()
#define FailMemError() ASSERTCONDSZ(0, "FailMemError:  Exception Thrown")
#define FailNIL(a) ASSERTCONDSZ(a, "FailNIL: Exception Thrown")
#define FailOSErr(a) ASSERTCONDSZ(a == noErr, "FailOSErr: Exception Thrown")
#define FailResError() ASSERTCONDSZ(0, "FailResError:  Exception Thrown")
#define Failure(error,message) ASSERTCONDSZ(0, "Failure:  Exception Thrown")
#endif



EXTERN_API_(void) Success(FailInfo *fi);
	/* Call this when you want to de-install your exception handler (pop 1
		element off the handler stack). */



/*------------------------------------+
|			  Our Stuff				  |
+------------------------------------*/


#define kSilentErr				1	// pass to Failure to fail without any user alert


extern char			gAskFailure;	/* When TRUE, break into debugger on functions
									 *	that may conditionally fail, e.g. FailNIL().
									 *	Provides opportunity for simulating failures
									 */


#ifndef _MSC_VER
extern short		gLastError;		/* last error code that caused a failure  */
extern long			gLastMessage;	/* last message associated with a failure */
#endif

#ifndef _MSC_VER

#define TRY		{												\
					char fPropagate = TRUE;						\
					if (!CatchSignal())							\
					{

#define CATCH			FreeSignal();							\
					}											\
					else										\
					{

#define NO_PROPAGATE	fPropagate = FALSE

#define ENDTRY			if (fPropagate)							\
							PropagateFailure();					\
					}											\
				}
#else

#define TRY		{												\
					if (1)							\
					{

#define CATCH		}											\
					else										\
					{

#define NO_PROPAGATE	

#define ENDTRY		}											\
				}
#endif // _MSC_VER

#endif  // __UFAILURE_H__
