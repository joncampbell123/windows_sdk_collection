/**[f******************************************************************
* sflb.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
/*  Listbox status
*/
#define SFLB_PERM   0x0001      /* Item marked for permanent download */
#define SFLB_SEL    0x0002      /* Item is selected */
#define SFLB_NEWID  0x0004      /* Item ID number has changed */
#define SFLB_CART   0x0008      /* Item is a cartridge */
#define SFLB_FAIS   0x0010      /* Item is a cartridge */
  
#define MAX_SFLB_FONTS  500     /* Max fonts allowed in sflb struct */
  
  
/*  Soft Font listbox array
*/
typedef struct {
    WORD native;            /* Initial status of entry */
    WORD state;             /* Current status of entry */
    int id;                 /* ID number */
    int indSFfile;          /* Index to file description */
    WORD fileSize;          /* Font download size for DeskJet */
} SFLBENTRY;
  
typedef struct {
    WORD len;               /* Number of entries */
    WORD free;              /* Pointer to first free entry */
    int prevsel;            /* Previous item shown in status line */
    SFLBENTRY sflb[1];      /* Listbox entries */
} SFLB;
  
typedef SFLBENTRY FAR * LPSFLBENTRY;
typedef SFLB FAR * LPSFLB;
  
int FAR PASCAL dupSFlistbox(HANDLE, int, LPSTR, int);
HANDLE FAR PASCAL
addSFlistbox(HWND, HANDLE, WORD, int, int, WORD, LPSTR, int, WORD FAR *);
HANDLE FAR PASCAL
replaceSFlistbox(HWND, HANDLE, WORD, int, int FAR *, int, WORD, LPSTR, int);
  
