/************************************************************

	PROGRAM:	CUSTDICT.C 	(for CUSTDICT.DLL)

	PURPOSE:
	
		The CUSTDICT dynamic link library is used by the expense
		report (sample dictionary application) to perform dictionary
		requests such as DIRQ_QUERY and DIRQ_STRING.

	COMMENTS:


************************************************************/

#define WIN31
#define NOCOMM

#include <windows.h>
#include <penwin.h>
#include <string.h>
#include "custdict.h"


int FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg
                       , WORD wHeapSize, LPSTR lpszCmdLine)
/***************************************************************

FUNCTION:	LibMain(HANDLE hInstance, WORD wDataSeg, 
									WORD wHeapSize, LPSTR lpszCmdLine)

PURPOSE:
	
	DLL entry point. 

***************************************************************/
	{
	if (wHeapSize > 0)
			UnlockData(0);

	return TRUE;
	}



VOID FAR PASCAL WEP(int nParameter)
/***************************************************************

FUNCTION:	WEP(int nParameter)

PURPOSE:
	
	DLL termination point.

***************************************************************/
	{
	}


int FAR PASCAL DictionaryProc(	int irq, LPVOID lpIn, LPVOID lpOut, int cbMax
									, LONG lContext, LONG lD)
/***************************************************************

FUNCTION: int FAR PASCAL DictionaryProc(int irq, LPVOID lpIn, LPVOID lpOut, 
										int cbMax, LONG lContext, LONG lD)

PURPOSE:
	
	Process all DIRQ MESSAGES

***************************************************************/
	{
	WORD wRet;
	char rgSrc[cbMaxLen+1];
	int i;

	wRet = FALSE;

	switch(irq)
		{
		case DIRQ_QUERY: /* query function */
			{
			int qIrq = *(int FAR *)lpIn;

			wRet = (	(qIrq == DIRQ_SUGGEST)		||
						(qIrq == DIRQ_DESCRIPTION)	||
						(qIrq == DIRQ_QUERY)			||
						(qIrq == DIRQ_STRING)		||
						(qIrq == DIRQ_INIT)			||
						(qIrq == DIRQ_CLEANUP)
					);
			}
			break;
		case DIRQ_DESCRIPTION:
			wRet = GetDllDescription(lpOut,cbMax);
			break;
		case DIRQ_INIT:
		case DIRQ_CLEANUP:
			wRet = 1;
			break;
		case DIRQ_STRING: /* search string */
			if (lpIn)
				{
				SymbolToCharacter((LPSYV)lpIn, cbMaxLen, rgSrc, NULL);
				if (FLookup((LPSTR)rgSrc))
					{
					for(i=0; i<cbMax; ++i)
						if(!(*((LPSYV)lpOut)++ = *((LPSYV)lpIn)++))
							break;	 /* also copies the NULL */ 
					wRet = i;
					}
				}
			break;
		case DIRQ_SUGGEST:
			if (lpIn)
				{
				SymbolToCharacter((LPSYV)lpIn, cbMaxLen, rgSrc, NULL);
				wRet = FBestGuess((LPSTR)rgSrc, (LPSYV)lpOut);
				}
			break;
		default:
			break;
		}

	return(wRet);
	}


BOOL NEAR PASCAL FBestGuess(LPSTR lpstr, LPSYV lpsyv)
/***************************************************************

	 
FUNCTION: FBestGuess (LPSTR lpstr, LPSYV lpsyv)

PURPOSE:
	
	Return TRUE if the string pointed to by lpstr is one of the
	strings in the Custom Dictionary rgszCustWords. 
	Returns FALSE if the word is not found.

***************************************************************/
	{
	int i;
	WORD wRet = FALSE;

	/* Perform case-insensitive prefix matching with lpstr and
		those words in the rgszCustWords */

	for (i=0; i<CUST_DICT_SIZE && !wRet; i++)
		{
		if (!_fstrnicmp(rgszCustWords[i],lpstr, lstrlen(lpstr)))
			wRet = CharacterToSymbol(rgszCustWords[i], lstrlen(rgszCustWords[i]), lpsyv);
		}

	return (wRet);
	}



BOOL NEAR PASCAL FLookup(LPSTR lpstr)
/***************************************************************

	 
FUNCTION: FLookup(LPSTR lpstr)

PURPOSE:
	
	Return TRUE if the string pointed to by lpstr is one of the
	strings in the Custom Dictionary rgszCustWords. 
	Returns FALSE if the word is not found.

***************************************************************/
	{
	int i;
	WORD wRet = FALSE;

	for (i=0; i<CUST_DICT_SIZE && !wRet; i++)
		if (!lstrcmp((LPSTR)rgszCustWords[i], lpstr))
			wRet = TRUE;

	return (wRet);
	}


int NEAR PASCAL GetDllDescription(LPSTR lpOut, int cbMax)
/***************************************************************

	 
FUNCTION: GetDllDescription(LPSTR lpOut, int cbMax)

PURPOSE:
	
	Return the description string for this DLL

***************************************************************/
	{
	int sizeT;

	sizeT = _fstrlen(szDescription);
	sizeT = min(sizeT, cbMax-1);
	_fstrncpy((LPSTR)lpOut,(LPSTR)szDescription, sizeT);
	*((LPSTR)lpOut+sizeT) = '\0';
	return(TRUE);
	}




