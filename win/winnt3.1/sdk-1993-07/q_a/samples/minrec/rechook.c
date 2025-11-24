// ************************************************************************
//
//                      Microsoft Developer Support
//               Copyright (c) 1993 Microsoft Corporation
//
// ************************************************************************
// MODULE    : RecHook.C
// PURPOSE   : A Small Win32 Recorder-like Sample Application Hook DLL
// FUNCTIONS :
//   DllMain()            - Dll entry point (via _DLLMainCRTStartup)
//   StartJournalRecord   - starts the journal record hook
//   StopJournalRecord    - stops the journal record hook
//   StartJournalPlayback - starts the journal playback hook
//   StopJournalPlayback  - stops the journal playback hook
//   StopAllJournalling   - stops any current journal hook
// COMMENTS  : The Journal Record File contains an EVENTHEADER structure
//             followed by a variable number of EVENTMSG structures.
// ************************************************************************
#define   STRICT               // strict type checking enabled
#define   UNICODE              // make the application unicode complient
#include <Windows.H>           // required for all Windows applications

#include "RecHook.H"
#include "MinRec.H"            // we need IDM_MACRO_STOP

// Internal Defines
// -----------------------------------------------------------------------
#define MINREC_FILENAME  TEXT("MinRec.Rec") // File for record/play events
#define MINREC_SIGNATURE "MinRec\0"         // Journal Record File signature

//-- header for the MinRec record/playback file
typedef struct EVENTHEADER_STRUCT {
  CHAR   Signature[7];
  UINT   RecordedMsgCount;
  DWORD  BaseMsgTime;
} EVENTHEADER, *PEVENTHEADER;

//-- various "file global" data
typedef struct GLOBAL_STRUCT {
  BOOL      fRecording;
  BOOL      fPlaying;
  BOOL      fStopRecording;
  BOOL      fStopPlaying;
  UINT      PlayedMsgCount;
  UINT      RecordedMsgCount;
  DWORD     BaseMsgTime;
  DWORD     LastMsgTime;
  HANDLE    hFile;
  EVENTMSG  EventMsg;
  HHOOK     hHookRecord;
  HHOOK     hHookPlayback;
  HHOOK     hHookGetMsg;
  HINSTANCE hInstance;
} GLOBAL, *PGLOBAL;

// Global Data
// -----------------------------------------------------------------------
GLOBAL  Global;                // various global data

// Internal Function Prototypes
// -----------------------------------------------------------------------
LRESULT CALLBACK JournalRecordProc  ( int, WPARAM, LPARAM );
LRESULT CALLBACK JournalPlaybackProc( int, WPARAM, LPARAM );
LRESULT CALLBACK GetMsgProc         ( int, WPARAM, LPARAM );


// ************************************************************************
// FUNCTION : DllMain( HINSTANCE, DWORD, LPVOID )
// PURPOSE  : DLLMain is called by the C run-time library from the
//            _DLLMainCRTStartup entry point.  The DLL entry point gets
//            called (entered) on the following events: "Process Attach",
//            "Thread Attach", "Thread Detach" or "Process Detach".
// ************************************************************************
BOOL WINAPI
DllMain( HINSTANCE hInstanceDll, DWORD dwReason, LPVOID lpvReserved )
{
  switch( dwReason ) {

    case DLL_PROCESS_ATTACH:
      Global.fRecording    = FALSE;
      Global.fPlaying      = FALSE;
      Global.hInstance     = hInstanceDll;
      break;

  }

  return( TRUE );
  UNREFERENCED_PARAMETER( lpvReserved );
}


// ************************************************************************
// FUNCTION : StartJournalRecord()
// PURPOSE  : creates the record file and installs the record hook.
// ************************************************************************
BOOL
StartJournalRecord()
{
  // Do not allow recording while playing back
  if( Global.fPlaying ) {
    MessageBox( GetFocus(),
      TEXT( "Sorry, recording while\n" )
      TEXT( "playing is not allowed."  ),
      TEXT( "MinRec - Notice!"         ),
      MB_ICONINFORMATION | MB_OK );
    return( FALSE );
  }

  // Create and open the journal record file and set the file pointer to
  //  just past the journal record file header
  Global.hFile = CreateFile( (LPCTSTR) MINREC_FILENAME, GENERIC_WRITE,
                       0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
  SetFilePointer( Global.hFile, (LONG) sizeof(EVENTHEADER), NULL,
    FILE_BEGIN );

  // Initialize the recorded message count to zero and set recording flag
  Global.RecordedMsgCount = 0;
  Global.fRecording       = TRUE;

  // Set the GetMsg hook to trap Ctrl+Esc, Alt+Esc and Ctrl+Alt+Del.
  Global.hHookGetMsg = SetWindowsHookEx( WH_GETMESSAGE, GetMsgProc,
                         Global.hInstance, 0 );

  // Set the Journal Record hook.
  Global.hHookRecord = SetWindowsHookEx( WH_JOURNALRECORD, JournalRecordProc,
                         Global.hInstance, 0 );

  return( TRUE );
}


// ************************************************************************
// FUNCTION : JournalRecordProc()
// PURPOSE  : processes the journal record events by writing them to  the
//            record file one event at a time.
// ************************************************************************
LRESULT CALLBACK
JournalRecordProc( int nCode, WPARAM wParam, LPARAM lParam )
{
  if( nCode < 0 || Global.fStopRecording )
    return( CallNextHookEx( Global.hHookRecord, nCode, wParam, lParam ) );

  switch( nCode ) {

    case HC_ACTION: {
      LPEVENTMSG lpEventMsg;
      DWORD      dwBytesWritten;

      lpEventMsg = (LPEVENTMSG) lParam;

      // user pressed Ctrl+Break to cancel journal recording
      if( (lpEventMsg->message == WM_KEYDOWN)
           && (LOBYTE(lpEventMsg->paramL) == VK_CANCEL) ) {
        StopAllJournalling();
      }
      else {
        // write the event message to the Recorded Journal file
        WriteFile( Global.hFile, (LPCSTR) lParam, sizeof(EVENTMSG),
          &dwBytesWritten, NULL );
        lpEventMsg = (LPEVENTMSG) lParam;
        Global.RecordedMsgCount++;
        if( Global.RecordedMsgCount == 1)
          Global.BaseMsgTime = lpEventMsg->time;
      }
      break;
    }

    case HC_SYSMODALON:
      // halt recording
      Global.fStopRecording = TRUE;
      break;

    case HC_SYSMODALOFF:
      // resume recording
      Global.fStopRecording = FALSE;
      break;

  }

  return( CallNextHookEx( Global.hHookRecord, nCode, wParam, lParam ) );
  UNREFERENCED_PARAMETER( wParam );
}


// ************************************************************************
// FUNCTION : StopJournalRecord()
// PURPOSE  : unhooks the record hook, records the record file header and
//            closes the file.
// ************************************************************************
BOOL
StopJournalRecord()
{
  EVENTHEADER EventHeader;

  // Remove the Journal Record hook if recording
  if( Global.fRecording ) {
    DWORD dwBytesWritten;

    // remove the record hook
    UnhookWindowsHookEx( Global.hHookRecord );

    // remove the get message hook
    UnhookWindowsHookEx( Global.hHookGetMsg );

    // Copy Recorded Journal header data to temporary buffer
    lstrcpyA( EventHeader.Signature, MINREC_SIGNATURE );
    EventHeader.RecordedMsgCount = Global.RecordedMsgCount;
    EventHeader.BaseMsgTime      = Global.BaseMsgTime;

    // Open Recorded Journal file and update the file header and close
    //  the file
    SetFilePointer( Global.hFile, 0, NULL, FILE_BEGIN );
    WriteFile( Global.hFile, (LPCSTR) &EventHeader, sizeof(EVENTHEADER),
      &dwBytesWritten, NULL );
    CloseHandle( Global.hFile );

    // Clear recording flag
    Global.fRecording = FALSE;
  }

  return( TRUE );
}


// ************************************************************************
// FUNCTION : StartJournalPlayback()
// PURPOSE  : opens the playback file and reads the header and installs the
//            playback hook.
// ************************************************************************
BOOL
StartJournalPlayback()
{
  EVENTHEADER EventHeader;
  DWORD       dwBytesRead;

  // Allow infinite playback loop
  if( Global.fRecording ) {
    StopJournalRecord();
    MessageBox( GetFocus(),
      TEXT( "Infinite playback\n" )
      TEXT( "loop recorded!"      ),
      TEXT( "MinRec - Notice!"    ),
      MB_ICONINFORMATION | MB_OK );
    return( FALSE );
  }
  if( Global.fPlaying ) {
    StopJournalPlayback();
  }

  // Open Recorded Journal file
  Global.hFile = CreateFile( (LPCTSTR) MINREC_FILENAME, GENERIC_READ,
                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
  if( Global.hFile == INVALID_HANDLE_VALUE ) {
    MessageBox( GetFocus(),
      TEXT( "The default Journal Record File\n" )
      TEXT( "MINREC.REC could not be found.\n"  ),
      TEXT( "MinRec - Error!"                   ),
      MB_ICONEXCLAMATION | MB_OK );
    return( FALSE );
  }

  // Quick check, file size should be at least the length of the header
  if( GetFileSize( Global.hFile, NULL ) < sizeof(EVENTHEADER) ) {
    MessageBox( GetFocus(),
      TEXT( "The file MINREC.REC is not a \n"     )
      TEXT( "valid MinRec Journal Record file.\n" ),
      TEXT( "MinRec - Error!"                     ),
      MB_ICONEXCLAMATION | MB_OK );
    return( FALSE );
  }

  // Read Recorded Journal file header
  SetFilePointer( Global.hFile, 0L, NULL, FILE_BEGIN );
  ReadFile( Global.hFile, (LPVOID) &EventHeader, sizeof(EVENTHEADER),
        &dwBytesRead, NULL );

  // Quick check, the signature of the file must be "MinRec"
  if( lstrcmpA( EventHeader.Signature, MINREC_SIGNATURE ) != 0 ) {
    MessageBox( GetFocus(),
      TEXT( "The file MINREC.REC does not\n"    )
      TEXT( "contain a valid MinRec signature." ),
      TEXT( "MinRec - Error!"                   ),
      MB_ICONEXCLAMATION | MB_OK );
     return( FALSE );
  }

  // store the header values
  Global.RecordedMsgCount = EventHeader.RecordedMsgCount;
  Global.BaseMsgTime      = EventHeader.BaseMsgTime;
  Global.LastMsgTime      = EventHeader.BaseMsgTime;

  // If no messages were recorded, none to play back.
  if( Global.RecordedMsgCount == 0 ) {
    MessageBox( GetFocus(),
      TEXT( "Sorry, nothing was recorded, so\n")
      TEXT( "there is nothing to playback."    ),
      TEXT( "MinRec - Notice!"                 ),
      MB_ICONINFORMATION | MB_OK);
     return( FALSE );
  }

  // Initialize the played message count to zero and set the play flag
  Global.PlayedMsgCount = 0;
  Global.fPlaying       = TRUE;

  // Get the first recorded message from the file and store it
  ReadFile( Global.hFile, (LPVOID) &(Global.EventMsg), sizeof(EVENTMSG),
        &dwBytesRead, NULL );

  // Set the GetMsg hook to trap Ctrl+Break, Ctrl+Esc and Ctrl+Alt+Del.
  Global.hHookGetMsg = SetWindowsHookEx( WH_GETMESSAGE, GetMsgProc,
                         Global.hInstance, 0 );

  // Set the Journal Playback hook
  Global.hHookPlayback = SetWindowsHookEx( WH_JOURNALPLAYBACK,
                           JournalPlaybackProc, Global.hInstance, 0 );

 return( TRUE );
}


// ************************************************************************
// FUNCTION : JournalPlaybackProc()
// PURPOSE  : processes the journal playback events by reading in the
//            playback file one event at a time.
// ************************************************************************
LRESULT CALLBACK
JournalPlaybackProc( int nCode, WPARAM wParam, LPARAM lParam )
{
  if( nCode < 0  || Global.fStopPlaying )
    return( CallNextHookEx( Global.hHookPlayback, nCode, wParam, lParam ) );

  switch( nCode ) {

   case HC_GETNEXT: {
      LRESULT SleepTime = Global.EventMsg.time - Global.LastMsgTime;

      // copy the current ready event message to lParam
      CopyMemory( (PVOID) lParam, (CONST VOID*) &(Global.EventMsg),
        sizeof(EVENTMSG) );
      Global.LastMsgTime = Global.EventMsg.time;
      return( SleepTime );
    }

    case HC_SKIP: {
      if( ++Global.PlayedMsgCount > Global.RecordedMsgCount ) {
        StopJournalPlayback();
        return( (LRESULT) 0L );
      }
      else {
        DWORD dwBytesRead;

        // Get the next recorded message from the file and store it
        //  in the static buffer for the next HC_GETNEXT message
        ReadFile( Global.hFile, (LPVOID) &(Global.EventMsg), sizeof(EVENTMSG),
          &dwBytesRead, NULL );
        break;
      }
    }

    case HC_SYSMODALON:
      // halt playback
      Global.fStopPlaying = TRUE;
      break;

    case HC_SYSMODALOFF:
      // resume playback
      Global.fStopPlaying = FALSE;
      break;

  }

  return( CallNextHookEx( Global.hHookPlayback, nCode, wParam, lParam ) );
  UNREFERENCED_PARAMETER( wParam );
}


// ************************************************************************
// FUNCTION : StopJournalPlayback()
// PURPOSE  : unhooks the playback hook and closes the playback file.
// ************************************************************************
BOOL
StopJournalPlayback()
{
  // Remove the Playback Journal hook if playing
  if( Global.fPlaying ) {

    // remove the playback hook
    UnhookWindowsHookEx( Global.hHookPlayback );

    // remove the get message hook
    UnhookWindowsHookEx( Global.hHookGetMsg );

    // close the journal record file
    CloseHandle( Global.hFile );

    // Clear playback flag
    Global.fPlaying = FALSE;
  }

 return( TRUE );
}


// ************************************************************************
// FUNCTION : StopAllJournalling()
// PURPOSE  : stops (unhooks) any current journal hook started (hooked) by
//            this application.
// ************************************************************************
BOOL
StopAllJournalling( )
{
 if( Global.fRecording )
   StopJournalRecord();
 if( Global.fPlaying )
   StopJournalPlayback();

 return( TRUE );
}


// ************************************************************************
// FUNCTION : GetMsgProc()
// PURPOSE  : installs a get message hook to monitor messages sent to the
//            queue so we can trap the WM_CANCELJOURNAL message send by the
//            system when the user presses Ctrl+Esc, Alt+Esc or
//            Ctrl+Alt+Del which cancels the journal hook.
// COMMENTS : WM_CANCELJOURNAL gets sent to the thread which installs the
//            hook but the hWnd of this message is NULL and thus does not
//            get dispatched.   Thus this hook is used to retrieve this
//            message.
// ************************************************************************
LRESULT CALLBACK
GetMsgProc( int nCode, WPARAM wParam, LPARAM lParam )
{
  static LPMSG lpMsg;

  if( nCode < 0  )
    return( CallNextHookEx( Global.hHookGetMsg, nCode, wParam, lParam ) );

  lpMsg = (LPMSG) lParam;
  switch( lpMsg->message ) {

    // user pressed Ctrl+Esc, Alt+Esc or Ctrl+Alt+Del, the system will
    // automatically stop all journalling.
    case WM_CANCELJOURNAL:
      StopAllJournalling();
      MessageBox( GetFocus(),
        TEXT( "The user has pressed either\n"            )
        TEXT( "Ctrl+Esc, Alt+Esc or Ctrl+Alt+Del\n"      )
        TEXT( "and thus the system has automatically\n"  )
        TEXT( "canceled all journalling.\n"              ),
        TEXT( "MinRec - Notice!"                         ),
        MB_ICONINFORMATION | MB_OK );
      break;

  }

  return( CallNextHookEx( Global.hHookGetMsg, nCode, wParam, lParam ) );
  UNREFERENCED_PARAMETER( wParam );
}
