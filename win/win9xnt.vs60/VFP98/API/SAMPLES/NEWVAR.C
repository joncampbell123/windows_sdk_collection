/*
**
*
* NEWVAR.C - Sample API routine.
*
* Copyright (c) 1989-1993 Microsoft Corporation as an unpublished
* licensed proprietary work.  All rights reserved.
*
* Description:
*	This is an example of an API rooutine that creates a FoxPro memory
*	variable (success) via the use _NewVar.
*	The variable that is created is numeric with the value of zero.
*
*		The variable is created in the routine SetUp using _NewVar
*		then it is given a type and a value using _Store and a Value
*		structure that represents an integer 0.
*
*               If the variable can not be created, the error is reported by
*              a call to _Error.
*
*               The variable is released by _Release in CleanUp when unloading
*              the API routine.
*
*
*/

#include  <pro_ext.h>
Locator loc;
Value   val;

void FAR SetUp()
{
        int created;

	val.ev_type='I';
	val.ev_width=5;
	val.ev_long=0;

	loc.l_subs=0;

        created= _NewVar("success",&loc,NV_PUBLIC);

        if(created>0)
            created = _Store(&loc, &val);       /* Initialize the varible. */
        else
            _Error(created);            /* An error occured when creating it. */




}

void FAR CleanUp()
{
	_Release(loc.l_NTI);
}
FoxInfo myFoxInfo[] ={
		{"SETUP", (FPFI) SetUp, CALLONLOAD, ""},
	{"CLEANUP", (FPFI) CleanUp,CALLONUNLOAD ,""},
};


FoxTable _FoxTable = {
    (FoxTable FAR *)0, sizeof(myFoxInfo) / sizeof(FoxInfo), myFoxInfo
};
