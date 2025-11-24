/*********************************************************************
 * GETDATA.C
 *
 * 20Aug87	sjp		Creation: Extracted from RESET.C.
 *********************************************************************
 */

#include "pscript.h"
#include "printers.h"


/********************************************************************/

LPSTR FAR PASCAL GetResourceData(hInstance,lphData,lpName,lpType)
	HANDLE hInstance;
	LPHANDLE lphData;
	LPSTR lpName;
	LPSTR lpType;
{
	HANDLE hInfo;
	LPSTR lpData=(LPSTR)(long)NULL;

	if( (hInfo=FindResource(hInstance,lpName,lpType)) &&
		(*lphData=LoadResource(hInstance,hInfo))
	){
		if( !(lpData=LockResource(*lphData)) ){
			FreeResource(*lphData);
		}
	}
	return(lpData);
}


/********************************************************************/

BOOL FAR PASCAL UnGetResourceData(hData)
	HANDLE hData;
{
	GlobalUnlock(hData);

	/* remember backwards logic on FreeResource() */
	return(!FreeResource(hData));
}


/********************************************************************/

BOOL FAR PASCAL GetPrinterCaps(hInstance,iPrinter,lpPrinter)
	HANDLE hInstance;
	short iPrinter;
	LPPRINTER lpPrinter;
{
	LPPRINTER lpTempPrinter;
	HANDLE hData;

	/* get the selected printer's capabilities structure */
	if(iPrinter>=0 && iPrinter<CPRINTERS){
		if(!(lpTempPrinter=(LPPRINTER)GetResourceData(hInstance,
			(LPHANDLE)&hData,(LPSTR)(long)iPrinter+1,
			(LPSTR)(long)PR_CAPS))
		){
			goto ERROR;
		}
		*lpPrinter=*lpTempPrinter;
	}else goto ERROR;

	/* if the data can't be freed then ERROR... */
	if(!UnGetResourceData(hData)) goto ERROR;

	return(TRUE);

ERROR:
	return(FALSE);
}

