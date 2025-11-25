/* this file contains the actual definitions of 
 the IIDs and CLSIDs 

 link this file in with the server and any clients 


 File created by MIDL compiler version 3.00.15 
 at Fri May 17 15:00:22 1996

 Compiler settings for settime.idl:
    Os, W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none


THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.

Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.
*/

//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef IID_DEFINED
#define IID_DEFINED

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // IID_DEFINED

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IMySettime = {0x043DACA1,0xA8ED,0x11CF,{0x91,0xE3,0x00,0xA0,0xC9,0x03,0x97,0x6F}};


const IID LIBID_SETTIMELib = {0x043DACA0,0xA8ED,0x11CF,{0x91,0xE3,0x00,0xA0,0xC9,0x03,0x97,0x6F}};


const CLSID CLSID_MySettime = {0x043DACA5,0xA8ED,0x11CF,{0x91,0xE3,0x00,0xA0,0xC9,0x03,0x97,0x6F}};


#ifdef __cplusplus
}
#endif

