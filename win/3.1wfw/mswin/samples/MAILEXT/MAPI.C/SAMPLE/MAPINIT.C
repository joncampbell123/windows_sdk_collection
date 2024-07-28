#include <windows.h>
#include "mapi.h"
#include "mapinit.h"


extern HANDLE hLibrary;

int FAR PASCAL InitMAPI()
{

  if ((hLibrary = LoadLibrary(MAPIDLL)) < 32)
    return(ERR_LOAD_LIB);

  if ((lpfnMAPILogon = GetProcAddress(hLibrary,SZ_MAPILOGON)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPILogoff= GetProcAddress(hLibrary,SZ_MAPILOGOFF)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPISendMail= GetProcAddress(hLibrary,SZ_MAPISENDMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPISendDocuments= GetProcAddress(hLibrary,SZ_MAPISENDDOC)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIFindNext= GetProcAddress(hLibrary,SZ_MAPIFINDNEXT)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIReadMail= GetProcAddress(hLibrary,SZ_MAPIREADMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPISaveMail= GetProcAddress(hLibrary,SZ_MAPISAVEMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIDeleteMail= GetProcAddress(hLibrary,SZ_MAPIDELMAIL)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIFreeBuffer= GetProcAddress(hLibrary,SZ_MAPIFREEBUFFER)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIAddress= GetProcAddress(hLibrary,SZ_MAPIADDRESS)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIDetails= GetProcAddress(hLibrary,SZ_MAPIDETAILS)) == NULL)
    return(ERR_LOAD_FUNC);

  if ((lpfnMAPIResolveName= GetProcAddress(hLibrary,SZ_MAPIRESOLVENAME)) == NULL)
    return(ERR_LOAD_FUNC);

  return(0);
}


int FAR PASCAL DeInitMAPI()
{

  lpfnMAPILogon = NULL;
  lpfnMAPILogoff= NULL;
  lpfnMAPISendMail= NULL;
  lpfnMAPISendDocuments= NULL;
  lpfnMAPIFindNext= NULL;
  lpfnMAPIReadMail= NULL;
  lpfnMAPISaveMail= NULL;
  lpfnMAPIDeleteMail= NULL;
  lpfnMAPIFreeBuffer = NULL;
  lpfnMAPIAddress= NULL;
  lpfnMAPIDetails = NULL;
  lpfnMAPIResolveName;

  FreeLibrary(hLibrary);

  return(0);
}