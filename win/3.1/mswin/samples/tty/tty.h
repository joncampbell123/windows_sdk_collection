//---------------------------------------------------------------------------
//
//  Module: tty.h
//
//  Purpose:
//     This is the header file for the TTY sample.
//
//  Development Team:
//     Bryan A. Woodruff
//
//  History:   Date       Author      Comment
//              5/ 9/91   BryanW      Wrote it.
//
//---------------------------------------------------------------------------
//
//  Written by Microsoft Product Support Services, Windows Developer Support.
//  Copyright (c) 1991 Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#define WIN31        // this is a Windows 3.1 application
#define USECOMM      // yes, we need the COMM API
#define STRICT       // be bold!

#include <windows.h>
#include <commdlg.h>
#include <string.h>

#include "version.h"
#include "resource.h"

// constant definitions

#define GWW_NPTTYINFO       0
#define ABOUTDLG_USEBITMAP  1

#define ATOM_TTYINFO       0x100

// terminal size

#define MAXROWS         25
#define MAXCOLS         80

#define MAXBLOCK        80

#define MAXLEN_TEMPSTR  81

#define RXQUEUE         4096
#define TXQUEUE         4096

// cursor states

#define CS_HIDE         0x00
#define CS_SHOW         0x01

// Flow control flags

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

// ascii definitions

#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

// data structures

typedef struct tagTTYINFO
{
   int     idComDev ;
   BYTE    bPort, abScreen[ MAXROWS * MAXCOLS ] ;
   BOOL    fConnected, fXonXoff, fLocalEcho, fNewLine, fAutoWrap,
           fUseCNReceive, fDisplayErrors;
   BYTE    bByteSize, bFlowCtrl, bParity, bStopBits ;
   WORD    wBaudRate, wCursorState ;
   HFONT   hTTYFont ;
   LOGFONT lfTTYFont ;
   DWORD   rgbFGColor ;
   int     xSize, ySize, xScroll, yScroll, xOffset, yOffset,
           nColumn, nRow, xChar, yChar ;

} TTYINFO, NEAR *NPTTYINFO ;

// macros ( for easier readability )

#define GETHINST( hWnd )  ((HINSTANCE) GetWindowWord( hWnd, GWW_HINSTANCE ))

#define COMDEV( x ) (x -> idComDev)
#define PORT( x )   (x -> bPort)
#define SCREEN( x ) (x -> abScreen)
#define CONNECTED( x ) (x -> fConnected)
#define XONXOFF( x ) (x -> fXonXoff)
#define LOCALECHO( x ) (x -> fLocalEcho)
#define NEWLINE( x ) (x -> fNewLine)
#define AUTOWRAP( x ) (x -> fAutoWrap)
#define BYTESIZE( x ) (x -> bByteSize)
#define FLOWCTRL( x ) (x -> bFlowCtrl)
#define PARITY( x ) (x -> bParity)
#define STOPBITS( x ) (x -> bStopBits)
#define BAUDRATE( x ) (x -> wBaudRate)
#define CURSORSTATE( x ) (x -> wCursorState)
#define HTTYFONT( x ) (x -> hTTYFont)
#define LFTTYFONT( x ) (x -> lfTTYFont)
#define FGCOLOR( x ) (x -> rgbFGColor)
#define XSIZE( x ) (x -> xSize)
#define YSIZE( x ) (x -> ySize)
#define XSCROLL( x ) (x -> xScroll)
#define YSCROLL( x ) (x -> yScroll)
#define XOFFSET( x ) (x -> xOffset)
#define YOFFSET( x ) (x -> yOffset)
#define COLUMN( x ) (x -> nColumn)
#define ROW( x ) (x -> nRow)
#define XCHAR( x ) (x -> xChar)
#define YCHAR( x ) (x -> yChar )
#define USECNRECEIVE( x ) (x -> fUseCNReceive)
#define DISPLAYERRORS( x ) (x -> fDisplayErrors)

#define SET_PROP( x, y, z )  SetProp( x, MAKEINTATOM( y ), z )
#define GET_PROP( x, y )     GetProp( x, MAKEINTATOM( y ) )
#define REMOVE_PROP( x, y )  RemoveProp( x, MAKEINTATOM( y ) )

// global stuff

char     gszTTYClass[] = "TTYWndClass" ;
char     gszAppName[] = "TTY" ;
HANDLE   ghAccel ;
WORD     gawBaudTable[] = { CBR_110, 
                            CBR_300,
                            CBR_600,
                            CBR_1200,
                            CBR_2400,
                            CBR_4800,
                            CBR_9600,
                            CBR_14400,
                            CBR_19200,
                            CBR_38400,
                            CBR_56000,
                            CBR_128000,
                            CBR_256000   } ;

WORD     gawParityTable[] = { NOPARITY,
                              EVENPARITY,
                              ODDPARITY,
                              MARKPARITY,
                              SPACEPARITY } ;

WORD     gawStopBitsTable[] = { ONESTOPBIT,
                                ONE5STOPBITS,
                                TWOSTOPBITS } ;

// function prototypes (private)

BOOL NEAR InitApplication( HANDLE ) ;
HWND NEAR InitInstance( HANDLE, int ) ;
LRESULT NEAR CreateTTYInfo( HWND ) ;
BOOL NEAR DestroyTTYInfo( HWND ) ;
BOOL NEAR ResetTTYScreen( HWND, NPTTYINFO ) ;
BOOL NEAR KillTTYFocus( HWND ) ;
BOOL NEAR PaintTTY( HWND ) ;
BOOL NEAR SetTTYFocus( HWND ) ;
BOOL NEAR ScrollTTYHorz( HWND, WORD, WORD ) ;
BOOL NEAR ScrollTTYVert( HWND, WORD, WORD ) ;
BOOL NEAR SizeTTY( HWND, WORD, WORD ) ;
BOOL NEAR ProcessTTYCharacter( HWND, BYTE ) ;
BOOL NEAR WriteTTYBlock( HWND, LPSTR, int ) ;
int NEAR ReadCommBlock( HWND, LPSTR, int ) ;
BOOL NEAR WriteCommByte( HWND, BYTE ) ;
BOOL NEAR MoveTTYCursor( HWND ) ;
BOOL NEAR OpenConnection( HWND ) ;
BOOL NEAR SetupConnection( HWND ) ;
BOOL NEAR CloseConnection( HWND ) ;
BOOL NEAR ProcessCOMMNotification( HWND, WORD, LONG ) ;
VOID NEAR GoModalDialogBoxParam( HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM ) ;
VOID NEAR FillComboBox( HINSTANCE, HWND, int, WORD NEAR *, WORD, WORD ) ;
BOOL NEAR SelectTTYFont( HWND ) ;
BOOL NEAR SettingsDlgInit( HWND ) ;
BOOL NEAR SettingsDlgTerm( HWND ) ;

// function prototypes (public)

LRESULT FAR PASCAL TTYWndProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL AboutDlgProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL FAR PASCAL SettingsDlgProc( HWND, UINT, WPARAM, LPARAM ) ;

//---------------------------------------------------------------------------
//  End of File: tty.h
//---------------------------------------------------------------------------


