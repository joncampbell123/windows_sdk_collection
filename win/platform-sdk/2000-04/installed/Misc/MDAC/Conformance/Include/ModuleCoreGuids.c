/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu May 20 18:03:30 1999
 */
/* Compiler settings for ModuleCore.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IError = {0xA0C3FEC1,0x8FBB,0x11d0,{0x98,0xCE,0x44,0x45,0x53,0x54,0x00,0x00}};


const IID IID_IProviderInfo = {0xA0C3FEC2,0x8FBB,0x11d0,{0x98,0xCE,0x44,0x45,0x53,0x54,0x00,0x00}};


const IID LIBID_ModuleBase = {0xDE32F910,0x2975,0x11d1,{0xA5,0xFA,0x00,0xC0,0x4F,0xC2,0xCA,0xBA}};


const IID IID_ITestCases = {0xA0C3FEC3,0x8FBB,0x11d0,{0x98,0xCE,0x44,0x45,0x53,0x54,0x00,0x00}};


const IID IID_ITestModule = {0xA0C3FEC4,0x8FBB,0x11d0,{0x98,0xCE,0x44,0x45,0x53,0x54,0x00,0x00}};


#ifdef __cplusplus
}
#endif

