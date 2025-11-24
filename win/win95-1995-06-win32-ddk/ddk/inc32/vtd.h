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

/*****************************************************************************
 *
 *   Title:	VTD.H -
 *
 *   Version:	1.00
 *
 *****************************************************************************/

/*XLATOFF*/
#define	VTD_Service	Declare_Service
/*XLATON*/

/* API Function Codes */

#define VTD_API_GET_VERSION 0x0000
#define VTD_API_GET_TIME    0x0100

/* Sub-Codes for Get Time */

#define VTD_API_GET_TIME_IN_CLOCK_TICKS 0
#define VTD_API_GET_TIME_IN_MS		1
#define VTD_API_GET_VM_EXEC_TIME	2


#ifndef Not_VxD
/*MACROS*/
Begin_Service_Table(VTD, VxD)

VTD_Service	(VTD_Get_Version, LOCAL)
VTD_Service	(VTD_Update_System_Clock, LOCAL)
VTD_Service	(VTD_Get_Interrupt_Period, LOCAL)
VTD_Service	(VTD_Begin_Min_Int_Period, LOCAL)
VTD_Service	(VTD_End_Min_Int_Period, LOCAL)
VTD_Service	(VTD_Disable_Trapping, LOCAL)
VTD_Service	(VTD_Enable_Trapping, LOCAL)
VTD_Service	(VTD_Get_Real_Time, LOCAL)
VTD_Service	(VTD_Get_Date_And_Time, LOCAL)
VTD_Service	(VTD_Adjust_VM_Count, LOCAL)
VTD_Service	(VTD_Delay, LOCAL)

End_Service_Table(VTD, VxD)
/*ENDMACROS*/
#endif /*NotVxD*/

/*XLATOFF*/
#pragma warning (disable:4035)		// turn off no return code warning

/*
 *  This function is a macro for efficiency.  The parameters passed are
 *  the variables the version (USHORT), cmsFastest and cmsSlowest (ULONG).
 */

#define	VTD_Get_Version(ver, cmsFastest, cmsSlowest) \
{ \
    VxDCall(VTD_Get_Version) \
    __asm xchg [ver],ax \
    __asm xchg [cmsFastest],ebx \
    __asm xchg [cmsSlowest],ecx \
}

void VXDINLINE
VTD_Update_System_Clock(void)
{
    VxDCall(VTD_Update_System_Clock)
}

CMS VXDINLINE
VTD_Get_Interrupt_Period()
{
    Touch_Register(eax)
    VxDCall(VTD_Get_Interrupt_Period)
}

int VXDINLINE
VTD_Begin_Min_Int_Period(CMS cms)
{
    _asm mov eax, cms
    VxDCall(VTD_Begin_Min_Int_Period)
    _asm cmc
    _asm sbb eax,eax
}

int VXDINLINE
VTD_End_Min_Int_Period(CMS cms)
{
    _asm mov eax, cms
    VxDCall(VTD_End_Min_Int_Period)
    _asm cmc
    _asm sbb eax,eax
}

void VXDINLINE
VTD_Enable_Trapping(HVM hvm)
{
    _asm mov ebx, hvm
    VxDCall(VTD_Enable_Trapping)
}

void VXDINLINE
VTD_Disable_Trapping(HVM hvm)
{
    _asm mov ebx, hvm
    VxDCall(VTD_Disable_Trapping)
}

void VXDINLINE
VTD_Get_Real_Time(void)
{
    Touch_Register(eax)
    Touch_Register(edx)
    VxDCall(VTD_Get_Real_Time)
}

ULONG VXDINLINE
VTD_Delay(ULONG cus)
{
    _asm mov ecx, cus
    VxDCall(VTD_Delay)
    _asm mov eax, ecx
}


#pragma warning (default:4035)		// turn on no return code warning
/*XLATON*/
