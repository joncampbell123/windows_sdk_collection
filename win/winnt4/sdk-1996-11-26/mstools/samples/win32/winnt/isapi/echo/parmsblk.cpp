// ParamsBlk.cpp
//
// Copyright (c)1996 Microsoft Corporation, All Right Reserved
//
// Module Description:
//
// This module includes functions which parse GET and POST FORM parameters 
// received from the HTML form. In addition, these functions manage
// parameter block creation, searching and deallocation. 
//
// Written by: ZorG			1/12/96
//


#include <windows.h>
#include "Echo.h"


static	CHAR	szEmptyString[] = "";



LPPARAMSBLOCK	CreateParametersBlock(EXTENSION_CONTROL_BLOCK *pECB)
/*++

Routine Description:

    This routine creates new parameter block lis

Arguments:

 pECB				- pointer to extension control block
 
Return Value:

  lpClientStart		- returns a pointer to the beginning of linked list	
   
--*/
{
LPSTR			lpTemp,  lpCurrent;
LPPARAMSBLOCK	lpParamsStart = NULL, lpParamsCurrentBlock;
DWORD			dwLen;

	if (0 == stricmp(pECB->lpszMethod, "GET")) {
		// Process GET parameters
		lpCurrent = pECB->lpszQueryString;
	} else if (0 == stricmp(pECB->lpszMethod, "POST")) {
		//Process POST parameters
		lpCurrent = (LPSTR)pECB->lpbData;
	} else
		return NULL;

	while(TRUE) {

		// Get parameter name
		lpTemp = lpCurrent;
		lpCurrent = strchr(lpTemp, '=');
		if (lpCurrent) {

			// Found new parameter
			lpParamsCurrentBlock = (LPPARAMSBLOCK)LocalAlloc( 0, sizeof(*lpParamsCurrentBlock) );
			if (NULL == lpParamsCurrentBlock)
				goto Error;

			AddParameterBlockToList(&lpParamsStart, lpParamsCurrentBlock);
				

			// Allocate memory for the parameter name
			dwLen = lpCurrent - lpTemp + 1;
			lpParamsCurrentBlock->Name = (LPSTR)LocalAlloc( 0, dwLen );
			if (NULL == lpParamsCurrentBlock->Name)
				goto Error;
			strncpy(lpParamsCurrentBlock->Name , lpTemp, dwLen - 1);
			lpParamsCurrentBlock->Name[dwLen - 1] = '\0';
			lpCurrent++; // skip = sign
			if ( '\0' == *lpCurrent) {
				// end of parameter list
				return lpParamsStart;
			}

			if ('&' == *lpCurrent)  {
				// parameter has no value, i.e is empty
				lpCurrent++; // skip &
				continue;
			}

			// Get parameter value
			lpTemp = lpCurrent;
			lpCurrent = strchr(lpTemp, '&');
			if (NULL == lpCurrent)  {
				// '&'was not found, must be the last 
				// parameter on the list
				lpCurrent = lpTemp;
				while(*lpCurrent)
					lpCurrent++;
			}
			if (lpCurrent) {
				// Allocate memory for the parameter value
				dwLen = lpCurrent - lpTemp + 1;
				lpParamsCurrentBlock->Value = (LPSTR)LocalAlloc( 0, dwLen );
				if (NULL == lpParamsCurrentBlock->Value)
					goto Error;

				SubstituteParameterValue(lpTemp, dwLen, lpParamsCurrentBlock->Value); 				

				if ('\0' == *lpCurrent) {
					// end of parameter list
					return lpParamsStart;
				}

				lpCurrent++; // goto next char
			}
			else
				goto Error;
		}
		else
			// End of parameter list
			return lpParamsStart;
	}

Error:

	ReleaseParametersBlock(lpParamsStart);

	return NULL;
}

LPSTR	GetParameterValue(LPPARAMSBLOCK lpParamsStart, LPSTR lpParamName)
/*++

Routine Description:

    This routine searches list for the name and returns corresponding value

Arguments:

 lpParamsStart	- pointer to the beginning of parameter list

Return Value:

  LPSTR			- parameter value or NULL if not found
   
--*/
{
	while(lpParamsStart) {
		if (!strcmp(lpParamsStart->Name, lpParamName)) {
			if (lpParamsStart->Value)
				return lpParamsStart->Value;
			else
				return szEmptyString;
		}
		lpParamsStart = lpParamsStart->Next;
	}	

	return NULL;
}

void ReleaseParametersBlock(LPPARAMSBLOCK lpParamsStart)
/*++

Routine Description:

    This routine releases paramer list and deallocates memory

Arguments:

 lpParamsStart	- pointer to the beginning of parameter list

Return Value:

  void
  
--*/
{
LPPARAMSBLOCK	lpTempPB = lpParamsStart;

	while(lpTempPB) { 
		if (lpParamsStart->Name)
			LocalFree(lpParamsStart->Name);
		if (lpParamsStart->Value)
			LocalFree(lpParamsStart->Value);
		lpTempPB = lpParamsStart->Next;
		LocalFree(lpParamsStart);
		lpParamsStart = lpTempPB;
	}
}


void  AddParameterBlockToList(LPPARAMSBLOCK *lpParamsStart, LPPARAMSBLOCK lpParamsCurrentBlock)
/*++

Routine Description:

    This routine adds new paramenter block to the list

Arguments:

 lpParamsStart			- double pointer to the beginning of the list
 lpParamsCurrentBlock	- pointer to the params. block to be added

Return Value:

  void 
   
--*/
{
	// Init. Name and Value
	lpParamsCurrentBlock->Name = NULL;
	lpParamsCurrentBlock->Value = NULL;
	if (NULL == *lpParamsStart) {
		// First element on the list
		*lpParamsStart = lpParamsCurrentBlock;
		(*lpParamsStart)->Next = NULL;
	}
	else {
		// Put new element on a top of the list
		lpParamsCurrentBlock->Next = *lpParamsStart;
		*lpParamsStart = lpParamsCurrentBlock;
	}	
}

/*++

Routine Description:

    This routine substitutes special characters
	in the parameter value. I actually process 
	two cases
	  1. Substitute '+' to ' '
	  2. Substitute special characters that begin
	     with % foolowed by hex representation 
		 of character
Arguments:

 lpSrc				- pointer to source WWW server string
 dwLen				- length of source string
 lpDest				- destination buffer
 
Return Value:

	void   
--*/

void	SubstituteParameterValue(LPSTR lpSrc, DWORD dwLen, LPSTR lpDest)
{

	int i = 0;
	while (i < (int)dwLen - 1) {
		if (*lpSrc == '+') {
			// Substitute '+' with ' ' 
			*lpDest = ' ';
			lpSrc++;
			lpDest++;
			i++;
		}
		else if (*lpSrc == '%') {
			// Special character, process
			// next two bytes as hex digits

			// Get the first byte
			lpSrc++;
			if (*lpSrc >= '0' && *lpSrc <= '9') {
				(*lpDest) = *lpSrc - '0';
			}
			else {
				(*lpDest) = *lpSrc + 0xA - 'A';
			}
			(*lpDest) <<= 4;

			// Get the second byte
			lpSrc++;
			if (*lpSrc >= '0' && *lpSrc <= '9') {
				(*lpDest) |= *lpSrc - '0';
			}
			else {
				(*lpDest) |= *lpSrc + 0xA - 'A';
			}

			lpSrc++;
			i += 3; // skip 3 characters %xx

			lpDest++;
		}
		else {
			*lpDest = *lpSrc;
			lpSrc++;
			i++;
			lpDest++;
		}
	}
	*lpDest = '\0';
}
