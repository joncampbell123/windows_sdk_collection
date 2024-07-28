/**[f******************************************************************
* debug.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
#ifdef DEBUG
extern int FAR PASCAL GimmeDBGfile(void);
void FAR PASCAL DBGclose(void);
extern LPSTR FAR PASCAL DoFormat(int, LPSTR, LPSTR far *);
extern LPSTR FAR PASCAL PutDec(int, int, BOOL, LPSTR);
extern LPSTR FAR PASCAL PutHex(int, int, BOOL, LPSTR);
extern LPSTR FAR PASCAL MyPutChar(int, int far *);
extern LPSTR FAR PASCAL PutString(int, BOOL, LPSTR);
extern void FAR PASCAL CharToFile(int, char);
extern LPSTR FAR PASCAL GetDigits(LPSTR, int far *);
  
static void cdecl DBGprint(LPSTR,...);
  
static void cdecl DBGprint(lsz,...)
LPSTR lsz;
{
    int fhDebug;
    char bCh;
    LPSTR lpbParams;
  
    if ((fhDebug = GimmeDBGfile()) < 0)
        return;
  
    lpbParams = ((LPSTR)&lsz) + sizeof(LPSTR);
  
    while (*lsz)
    {
        bCh = *lsz++;
        if (bCh=='%')
        {
            lsz = DoFormat(fhDebug, lsz, (LPSTR far *) &lpbParams);
        }
        else
        {
            if (bCh=='\\')
            {
                switch(bCh = *lsz++)
                {
                    case 'n':
                        bCh = 0x0a;
                        break;
                    case 't':
                        bCh = 0x09;
                        break;
                    case 0:
                        return;
                    default:
                        break;
                }
            }
            CharToFile(fhDebug, bCh);
        }
    }
}
  
#define DBMSG(msg) DBGprint msg
#else
#define DBMSG(msg) /*null*/
#endif
