#ifndef _ERRORS_H_
#define _ERRORS_H_

#define kScodeAlert		200
#define kFatalAlert 	201
#define kGenericAlert	202
#define kWarningAlert	203
#define	kOSErrAlert		204
#define kUnknownAEAlert	205

void FailAlert		(StringPtr str);            // Alert, OleUninitialize->ExitToShell
void GenericAlert	(StringPtr str);
void WarningAlert	(StringPtr str);
void ScodeAlert		(StringPtr str, SCODE sc);
void OSErrAlert		(StringPtr str, OSErr err);

void FailScode(StringPtr str, SCODE sc);  // tests sc==S_OK, calls ScodeAlert on failure
void FailOSErr(StringPtr, OSErr err);     // tests err==noErr, calls OSErrAlert on failure

void 	Scode2PStr		(SCODE sc, StringPtr	pstr);  // 0x80000002 -> "\pE_OUTOFMEMORY"
void 	Scode2Literal	(SCODE sc, StringPtr pstr);		// 0x80000002 -> "\p0x80000002"
short 	ReturnLen		(char * theString);

#endif // _ERRORS_H_
