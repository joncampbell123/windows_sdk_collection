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

/*
 File:		This file contains C rappers for commonly used VMM calls.  They may
			some day be in VMM.H.
 Purpose:	Making it all easier!
*/
DWORD VXDINLINE
Get_Cur_VM_Handle()
{
	DWORD dwRet;
	Touch_Register(eax);
	Touch_Register(ebx);
	VMMCall(Get_Cur_VM_Handle)
	_asm mov dwRet,ebx
	return (dwRet);
}


DWORD VXDINLINE
_VTD_Get_Real_Time()
{
	DWORD dwRet;
	_asm push ecx
	//Touch_Register(eax);
	//Touch_Register(edx);
	VTD_Get_Real_Time();
// Fix for Winword winexcel bug in time stamp. now do division in assembly
// so we should never return a zero value!  EDX:EAX contains the value from
// VTD_Get_Real_Time().
	_asm mov ecx,1193
	_asm div ecx
	_asm mov dwRet,eax
	_asm pop ecx
	return(dwRet);

//	return(dwRet/(DWORD)1193);
}

// eax == High_Pri_Device_Boost
// ecx == PEF_Wait_For_STI
// edx == VPEND_RET_TIMERTICK
// dwRoutine == cCall_Installable_Driver

void VXDINLINE
_Call_Priority_VM_Event(DWORD PriorityBoost, DWORD Flags, DWORD RefData, DWORD EventCallback)
{
	VMMCall(Get_Sys_VM_Handle);
	_asm mov	eax,PriorityBoost
	_asm mov	ecx,Flags
	_asm mov	edx,RefData
	_asm mov	esi,EventCallback
	VMMCall(Call_Priority_VM_Event);
}


void VXDINLINE
VCD_Set_Port_Global(DWORD dwPort,DWORD dwType)
{
	_asm mov	eax,dwPort
	_asm mov	edx,dwType
//	VxDCall(VCD_Set_Port_Global);
}


//DWORD static
DWORD VXDINLINE
_Get_Profile_Hex_Int(char *pSection,char *pKey,DWORD dwDefault)
{
	__asm {
		mov	eax,dwDefault
		mov	esi,pSection
		mov	edi,pKey
		VMMCall(Get_Profile_Hex_Int);
	}
}

/*
DWORD VXDINLINE
_Get_Profile_Hex_Int(char *pSection,char *pKey,DWORD dwDefault)
{
	DWORD dwRet;
	_asm mov	eax,dwDefault
	_asm mov	esi,[pSection]
	_asm mov	edi,[pKey]
	VMMCall(Get_Profile_Hex_Int);
	_asm mov dwRet,eax
	return(dwRet);
}
*/

DWORD VXDINLINE
_Get_Profile_Decimal_Int(char *pSection,char *pKey,DWORD dwDefault)
{
	DWORD dwRet;
	_asm mov	eax,dwDefault
	_asm mov	esi,pSection
	_asm mov	edi,pKey
	VMMCall(Get_Profile_Decimal_Int);
	_asm mov dwRet,eax
	return (dwRet);
}


DWORD VXDINLINE
_Get_Profile_Boolean(char *pSection,char *pKey,DWORD dwDefault)
{
	DWORD dwRet;
	_asm mov	eax,dwDefault
	_asm mov	esi,pSection
	_asm mov	edi,pKey
	VMMCall(Get_Profile_Boolean);
	_asm mov dwRet,eax
	return (dwRet);
}

DWORD VXDINLINE
_Get_Profile_String(char *pSection,char *pKey,char *pDefault)
{
	DWORD dwRet;
	__asm {
		mov	edx,pDefault
		mov	esi,pSection
		mov	edi,pKey
	}
	VMMCall(Get_Profile_String);
	__asm {
		jnc SHORT GPS_OK
		xor edx,edx
GPS_OK:	mov dwRet,edx
	}
	return dwRet;
}


void VXDINLINE _cdecl
My_Debug_Printf_Service(char *pszfmt, ...)
{
    __asm lea  eax,(pszfmt + 4)
    __asm push eax
	__asm push pszfmt
//	VMMCall(_Debug_Printf_Service)
	__asm add esp,8
}
