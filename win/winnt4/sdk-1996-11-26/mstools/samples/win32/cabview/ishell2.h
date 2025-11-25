//*******************************************************************************************
//
// Filename : IShell2.h
//	
//				Shell_MessageBox and Shell_MergeMenu
//
// Copyright (c) 1994 - 1995 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************



#ifndef _ISHELL2_H_
#define _ISHELL2_H_

#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------        
// **** helper macro                                                              
//----------------------------------------------------------------------------        
                                                                                      
#define _IOffset(class, itf)         ((UINT)&(((class *)0)->itf))                     
#define IToClass(class, itf, pitf)   ((class  *)(((LPSTR)pitf)-_IOffset(class, itf))) 
#define IToClassN(class, itf, pitf)  IToClass(class, itf, pitf)                       
                                                                                      

//===========================================================================
// ITEMIDLIST
//===========================================================================

// unsafe macros from shesimp.h & idlist.c
#define _ILSkip(pidl, cb)       ((LPITEMIDLIST)(((BYTE*)(pidl))+cb))
#define _ILNext(pidl)           _ILSkip(pidl, (pidl)->mkid.cb)
#ifdef DEBUG
// Dugging aids for making sure we dont use free pidls
#define VALIDATE_PIDL(pidl) Assert((pidl)->mkid.cb != 0xC5C5)
#else
#define VALIDATE_PIDL(pidl)
#endif

void        ILFree(LPITEMIDLIST pidl);
LPITEMIDLIST ILClone(LPCITEMIDLIST pidl);
UINT ILGetSize(LPCITEMIDLIST pidl);

typedef void (WINAPI FAR* RUNDLLPROC)(HWND hwndStub,                      
        HINSTANCE hAppInstance,                                           
        LPSTR lpszCmdLine, int nCmdShow);                                 

UINT  Shell_MergeMenus(HMENU hmDst, HMENU hmSrc, UINT uInsert, UINT uIDAdjust, UINT uIDAdjustMax, ULONG uFlags);

//===================================================================
// Shell_MergeMenu parameter
//
#define MM_ADDSEPARATOR		0x00000001L
#define MM_SUBMENUSHAVEIDS	0x00000002L

#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))


//====== ShellMessageBox ================================================
                                                                         
// If lpcTitle is NULL, the title is taken from hWnd                     
// If lpcText is NULL, this is assumed to be an Out Of Memory message    
// If the selector of lpcTitle or lpcText is NULL, the offset should be a
//     string resource ID                                                
// The variable arguments must all be 32-bit values (even if fewer bits  
//     are actually used)                                                
// lpcText (or whatever string resource it causes to be loaded) should   
//     be a formatting string similar to wsprintf except that only the   
//     following formats are available:                                  
//         %%              formats to a single '%'                        
//         %nn%s           the nn-th arg is a string which is inserted    
//         %nn%ld          the nn-th arg is a DWORD, and formatted decimal
//         %nn%lx          the nn-th arg is a DWORD, and formatted hex    
//     note that lengths are allowed on the %s, %ld, and %lx, just        
//                         like wsprintf /* ;Internal */                  
//                                                                        
//int _cdecl ShellMessageBox(HINSTANCE hAppInst, HWND hWnd, LPCSTR      
//        lpcText, LPCSTR lpcTitle, UINT fuStyle, ...);    


#define STRRET_OLESTR   0x0000

#ifdef __cplusplus
}
#endif
                                           

#endif // _ISHELL2_H_


