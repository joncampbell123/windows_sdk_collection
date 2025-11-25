// OvComm.h

// DEFINES
#define STATUS_CHECK_TIMEOUT    500
#define READ_TIMEOUT            1000
#define MAX_READ_BUFFER         80
#define MAX_WRITE_BUFFER        1024
#define PURGE_FLAGS             PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR
//#define EVENT_FLAGS             EV_BREAK|EV_CTS|EV_DSR|EV_ERR|EV_RING|EV_RLSD|EV_RXCHAR|EV_TXEMPTY|EV_RXFLAG
#define EVENT_FLAGS					EV_RING | EV_ERR

#define EVT_CHAR                (char) 0x0A


// GLOBALS
HANDLE 	comHandle, hSetupEvent;
BOOL	bDone;
char    lpszPort[100];
char    lpszSerialSpec[100];
COMMTIMEOUTS g_timeouts;

// PROTOTYPES
void BreakDownCommPort();

