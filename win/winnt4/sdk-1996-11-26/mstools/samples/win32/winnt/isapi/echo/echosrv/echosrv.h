#ifndef _ECHOSERVER
#define	_ECHOSERVER

#define SERVER_PORT 22222

typedef enum _LAST_CLIENT_IO {
    ClientIoRead,
    ClientIoWrite
} LAST_CLIENT_IO, *PLAST_CLIENT_IO;


typedef struct _CLIENT_CONTEXT {
    SOCKET			Socket;
    LAST_CLIENT_IO	LastClientIo;
    OVERLAPPED		Overlapped;
    CHAR			Buffer[8192];
} CLIENT_CONTEXT, *PCLIENT_CONTEXT;


// Function prototypes
void	WINAPI EchoServerMain(DWORD argc, LPTSTR argv[]);
BOOL	UpdateServiceStatus(DWORD   Status);
HANDLE	InitializeThreads ();
BOOL	AcceptClients (HANDLE hCompletionPort);
VOID	CloseClient (PCLIENT_CONTEXT lpClientContext,BOOL bGraceful );
DWORD	WINAPI ServerWorkerThread ( LPVOID WorkContext );

#endif // _ECHOSERVER