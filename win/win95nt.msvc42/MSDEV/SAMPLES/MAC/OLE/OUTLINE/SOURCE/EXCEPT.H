#ifndef _MSC_VER
#pragma once
#endif

#include <setjmp.h>

#define errRetry				1	// value returned from setjmp when retrying
#define errSilent				2	// pass to Failure to fail without any user alert

// this hack gets around the fact that the Metrowerks compiler
// doesn't currently handle the volatile keywork correctly.
#ifdef __MWERKS__
#define volatile static
#endif

typedef struct FailInfo
{
	char			fPropagate;
	struct FailInfo	*next;
	jmp_buf			regs;
} FailInfo;

extern short	gLastError;			// last error code that caused a failure
extern long		gLastMessage;		// last message associated with a failure
extern char		gAskFailure;		// When TRUE, break into debugger on functions

void NoHandler(void);
void PushTryHandler(FailInfo *fi);

void Failure(short error, long message);
void FailMemError(void);
void FailResError(void);
void FailNIL(void *p);
void FailOSErr(short error);
void FailNILRes(void* p);
void Success(void);
void RetryException(FailInfo *fi);
void ThrowHandler(FailInfo *fi);


#define TRY											\
		{	FailInfo	__fi;						\
			int			__result;					\
			PushTryHandler(&__fi);					\
			__result = setjmp(__fi.regs);			\
			if (__result == 0 || __result == errRetry) {
		
#define CATCH										\
			Success();								\
		} else {
		
#define ENDTRY										\
			if (__fi.fPropagate)					\
				Failure(gLastError, gLastMessage);	\
		} }
			
#define RETRY										\
			RetryException(&__fi);
		
#define NO_PROPAGATE								\
			__fi.fPropagate = false
