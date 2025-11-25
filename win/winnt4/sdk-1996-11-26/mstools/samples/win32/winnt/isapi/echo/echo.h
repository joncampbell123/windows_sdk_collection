#ifndef _Echo
#define _Echo


#include "httpext.h"

#define COOKIE_LENGTH		256
#define	COOKIE_NAME			"Echo"
#define	BUFLEN				4096
#define	SERVER_PORT			22222
#define	CLEANUP_TIMEOUT		60000  // check for inactive connections 
								   // every 10 minutes
#define	INACTIVE_CONNECTION 120000 // no client activity on a connection for 20 
								   // minutes automatically makes it inactive

typedef struct _CONNCONTEXT {
	EXTENSION_CONTROL_BLOCK	*pEcb;
    SOCKET					Socket;
	DWORD					dwTimeAccessed;
	_CONNCONTEXT			*Next;
	_CONNCONTEXT			*Prev;
} CONNCONTEXT, * LPCONNCONTEXT;

typedef struct _PARAMSBLOCK
{
	char			*Name;
	char			*Value;
	_PARAMSBLOCK	*Next;
} PARAMSBLOCK, * LPPARAMSBLOCK;

// Function prototypes
LPCONNCONTEXT	GetClientCookie(EXTENSION_CONTROL_BLOCK *pEcb);
BOOL			ConnectClient ( LPCTSTR lpHostName, SOCKET *s);
BOOL			CallEchoServer(LPCONNCONTEXT lpConnContext, LPPARAMSBLOCK lpParamsBlock);
void			DisconnectEchoServer(LPCONNCONTEXT lpConnContext);
void			ErrorResponse(EXTENSION_CONTROL_BLOCK *pECB, LPSTR lpError);
DWORD WINAPI	EchoTimerThread(LPVOID lpVoid);

LPCONNCONTEXT	AddConnectionContext(EXTENSION_CONTROL_BLOCK *pEcb, SOCKET s);
void			RemoveConnectionContext(LPCONNCONTEXT lpConnContext);
BOOL			FindConnectionContext(LPCONNCONTEXT lpConnContext, EXTENSION_CONTROL_BLOCK *pEcb);
void			RemoveInactiveConnections(void);

LPSTR			GetParameterValue(LPPARAMSBLOCK lpParamsList, LPSTR lpParamName);
LPPARAMSBLOCK	CreateParametersBlock(EXTENSION_CONTROL_BLOCK *pECB);
void			ReleaseParametersBlock(LPPARAMSBLOCK lpParamsList);
void			AddParameterBlockToList(LPPARAMSBLOCK *lpParamsList, LPPARAMSBLOCK lpParamsCurrentBlock);
void			SubstituteParameterValue(LPSTR lpSrc, DWORD dwLen, LPSTR lpDest);


#endif // _ISAPIPECHO