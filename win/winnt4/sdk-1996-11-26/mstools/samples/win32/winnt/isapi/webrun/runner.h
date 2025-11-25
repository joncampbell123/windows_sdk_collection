/****************************************************************************
*
*
*   File: WebRunner.h
*
*   PURPOSE: Supplies all defines and prototypes
*
****************************************************************************/


// Defines

#define ENDPAGE "</pre></body></html>"
#define LOCATION "Software\\WebRunner\\Commands"
#define PARAMETRS "Software\\WebRunner\\Parameters"
#define POSITION "HTMLListBoxLines"
#define MODE "Mode"
#define MEMERROR "\nRUNAPP ERROR: Error allocating more memory for I/O buffer\n"
#define INACTIVETIMEOUT 300000
#define IN
#define OUT

// Prototypes
 void EscapeToAscii (CHAR * IN OUT); 
CHAR HexToAscii  (CHAR * IN );
CHAR * GetParamValue (CHAR * IN , CHAR * IN);
DWORD RunApp (LPSTR IN, LPSTR * OUT);
BOOL FreeOutBuffer (LPSTR IN);
CHAR * ReadRegistry ( BOOL * IN);

