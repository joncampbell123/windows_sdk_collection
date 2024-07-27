/***************************************************************************\
*
* Module Name: MEMDEF.H
*
* Copyright (c) 1987  Microsoft Corporation
*
\***************************************************************************/
 
#define FAR     far
#define NEAR    near
#define VOID    void
 
#define CHAR    char
#define SHORT   short
#define LONG    long
#define INT     int
 
typedef unsigned short USHORT;
typedef unsigned long ULONG; 
typedef unsigned int  UINT; 
 
typedef unsigned char BYTE;
 
#define NULL    0
 
typedef char far  *PCH;
typedef char near *NPCH;
 
typedef int   (pascal far  *PFN)();
typedef int   (pascal near *NPFN)();
 
typedef BYTE   FAR  *PBYTE;
typedef BYTE   near *NPBYTE;
 
typedef CHAR   FAR *PCHAR;
typedef SHORT  FAR *PSHORT;
typedef LONG   FAR *PLONG;
typedef INT    FAR *PINT;
 
typedef unsigned char UCHAR;

typedef UCHAR  FAR *PUCHAR;
typedef USHORT FAR *PUSHORT;
typedef ULONG  FAR *PULONG;
typedef UINT   FAR *PUINT;
 
typedef VOID   FAR *PVOID;
 
typedef unsigned short BOOL;
typedef BOOL FAR *PBOOL;
 
#define FALSE   0
#define TRUE    1
 
