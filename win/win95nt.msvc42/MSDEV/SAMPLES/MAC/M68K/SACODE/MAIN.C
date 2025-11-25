// this pragma guarantees that main appears first in the sacode
// the base$text naming convention works as follows:
//         anything after the $ will be sorted, and then put into the
//         base section.

#pragma code_seg("sacode$1")

extern int _declspec(allocate("_CODE")) cBeeps;
extern void DoBeeps(void);

void main(void)
	{
	cBeeps = 5;
	DoBeeps();
	}

