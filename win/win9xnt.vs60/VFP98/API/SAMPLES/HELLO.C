// include the library construction header file
#include <pro_ext.h>

void hello(ParamBlk  *parm) // the function definition
	{
	_PutStr("\nHello, World!\n"); //print the message
	}

// the FoxInfo structure registers the function
FoxInfo myFoxInfo[] = {
	{"HELLO",(FPFI) hello, 0, ""},
};

// the FoxTable structure
FoxTable _FoxTable = {
	(FoxTable  *) 0, sizeof(myFoxInfo)/sizeof(FoxInfo), myFoxInfo
};

