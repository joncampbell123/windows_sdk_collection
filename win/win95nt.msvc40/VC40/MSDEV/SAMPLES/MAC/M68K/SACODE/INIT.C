// this pragma guarantees that init appears 2nd in the sacode section.
// the base$text naming convention works as follows:
//         anything after the $ will be sorted, and then put into the
//         base section.

#pragma code_seg("sacode$2")

#include <macos\osutils.h>


// Note that cBeeps ends up as the first item in sacode$2.  Since
// sacode$1 preceeds sacode$2, you have the following order in sacode:
//		1.  main
//		2.  cBeeps
//		3.  DoBeeps


int _declspec(allocate("_CODE")) cBeeps;

void DoBeeps(void)
	{
	int iBeeps;
	
	for(iBeeps=0; iBeeps<cBeeps; iBeeps++)
		{
		SysBeep(250);
		}
	}

