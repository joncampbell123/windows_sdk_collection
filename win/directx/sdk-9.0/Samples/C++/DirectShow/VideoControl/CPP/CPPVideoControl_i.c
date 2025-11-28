/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Jul 23 18:31:14 2001
 */
/* Compiler settings for D:\DXSDK\samples\Multimedia\DirectShowXP\VideoControl\CPPVideoControl\CPPVideoControl.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
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

const IID IID_ICompositeControl = {0x3E119535,0xD5AB,0x4520,{0xB0,0xE1,0x49,0x5B,0x32,0x2E,0x2A,0x1A}};


const IID LIBID_CPPVIDEOCONTROLLib = {0xC03567A2,0x8044,0x40F0,{0x8A,0xBB,0x30,0x1A,0x00,0x5F,0x9F,0xF1}};


const CLSID CLSID_CompositeControl = {0xCDDFD429,0xEDFD,0x4C72,{0xAE,0x9C,0xB7,0x0F,0xE6,0x95,0x50,0x51}};


#ifdef __cplusplus
}
#endif

