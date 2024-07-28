// TYPE 1 commands for use with CharString().  The high word contains the
// number of operands on the stack, and the loword contains the encoding
// of the command.  For two byte commands the LSB of the low word contains
// 12 decimal and the MSB of the low word contains the second byte of the
// code.  See Chapter 6 of the Black Book for further details.

#define STARTCHAR   0xffffffffL // dummy command to signal start of character
                                // definition.

#define PUSHNUMBER  0xfffffffeL // dummy command to place number on stack

#define HSTEM       0x00020001L
#define VSTEM       0x00020003L
#define VMOVETO     0x00010004L
#define RLINETO     0x00020005L
#define HLINETO     0x00010006L
#define VLINETO     0x00010007L
#define RRCURVETO   0x00060008L
#define CLOSEPATH   0x00000009L
#define SBW         0x0004070cL
#define HSBW        0x0002000dL
#define ENDCHAR     0x0000000eL 
#define RMOVETO     0x00020015L
#define HMOVETO     0x00010016L
#define VHCURVETO   0x0004001EL
#define HVCURVETO   0x0004001FL

// external types used
typedef BYTE NEAR *PBYTE;
typedef BYTE FAR *LPBYTE;
typedef DWORD NEAR *PDWORD;
typedef DWORD FAR *LPDWORD;
typedef LONG NEAR *PLONG;
typedef LONG FAR *LPLONG;

// initial r parameter to Encrypt() for the different encryption layers.
// See Chapter 7 of the Black Book for details.

#define RCS         4330u       // CharString encryption
#define REEXEC      55665u      // eexec encryption

/* prototypes */
void FAR PASCAL eexec(LPDV lpdv, LPBYTE lpBuf, WORD len, BOOL  reset);
DWORD FAR CharString(DWORD dwCmd, ...);
void FAR efprintf(LPDV lpdv, LPSTR lpFmt, ...);
void FAR PASCAL StartEExec(LPDV lpdv);
void FAR PASCAL EndEExec(LPDV lpdv);
void FAR DbPrintf(LPSTR lpFmt, ...);

/* debug print routine */
#ifdef DEBUG
#define DPRINT(s) DbPrintf s
void FAR PASCAL OutputDebugString(LPSTR);
#else
#define DPRINT(s)
#endif

