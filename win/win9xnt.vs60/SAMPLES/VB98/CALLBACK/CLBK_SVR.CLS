VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "CbServerClass"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

'This sample demonstrates the use of an ole object ptr being passed to
'an external (and optionally remote) ole server.  The external ole server
'periodically calls a method on the passed object ptr. This has the effect
'of a server initiated callback to the client... which can be a much better
'app model that the polling a client app might have to do otherwise to
'find the status of a server.  Although this demo simply returns the time
'to the client, it could just as easily return data, news, or other information
'it has been told the client wants to know.  The benefit here is that the
'server does all the work looking for data the client might need... and the
'client does other work... only being interrupted when the server actually
'has something that interests it.

'Note1: In this scenario, the client creates an object that is internal to it'
'It then creates an instance of the remote server and passes an object
'ptr to it of its internal object.  The server hangs onto the client's object,
'"feeding it" from time to time.  At somepoint the client decides to
'disconnect (DropObjectReference) and the server does some clean up
'work in preparation for the client setting its reference to the server =
'Nothing.  Since this server has a visible form (servers usually will not
'have any visible displays except for debugging information), it unloads
'it at this point so that the instance will be closed by ole when the client
'sets the reference = Nothing.

'Note2: Since the client created both the instance to its internal object
'and the instance to the server... it is important that the client be the one
'to close these instances by setting them = Nothing.  OLE errors will
'occur if the server tries to set the reference to the client = Nothing.

'Note3: This app was developed very quickly... and should not be taken
'as gospel on how to build good servers.  For example, a good server
'should not have ANY msgbox calls outside of a debug mode.  (If a
'production server tried to display a msg for the user... it could wait a
'long time for someone to come by the server machine to click "OK".)
'This app also has very little error handling...it will be refined and cleaned
'up before final release.

'Note4: The following class properties are critical for the behavior of
'the application:
'
'  "Instancing = Creatable SingleUse" This causes OLE to create a
'           new physical instance of the class for each client.  This
'           provides a simple programming model for creating parallel
'           execution paths on the NT server, but requires a fair amount
'           of memory to be allocated for each client.  Literally, SingleUse
'           means the server is only used by a a single client at a time.
'           A more efficient runtime model can be achieved if the setting
'           of "Instancing = MultiUse" is set.  This causes OLE to only
'           create an instance of the server for the first client... all other
'           clients will get a ptr to the initial instance when they do their
'           creates.  This model is more efficient from a memory standpoint,
'           but OLE will only allow one client to use the class instance at
'           a time... which could cause client blocking if the work the class
'           does takes longer than the average client request interval.  A
'           good compromise between these two implementation options is
'           to use a pool of SingleUse servers.  In this case, 100 clients
'           might have access to a pool of 10 class instances.  See the
'           poolmngr sample for more details on this scenario.
'
'  "Public = True" This allows external apps to access this class through
'           OLE.
'  "Name = CbServerClass" This name is used as the class name in the second part
'           of the progid.  The client references this progid to tell OLE what
'           object it wants to start.

'Note5:
'In the Tools.Options.Project dialog, it is very important that the project name
'be set.  This is used by the client to define the project name in the progid.  After
'creating an ole server exe, the exe must be run once so that it can register its
'OLE typeinfo data in the system registry.  After running it once, close the server
'manually and everything should be set for your client app to call the server through
'OLE.

'Note6: Every time you build a new exe of your server, VB will generate a new
'unique ID for each of its classes.  Since this ID must be the same ID that is also
'registered in the client machine's registry, this constant changing of the ID can
'make keeping the client and the server working together a real chore.  To get
'around this, go to the Tools.Options.Project dialog and set the "Compatible OLE
'Server" field to a previous instance of the server exe.  Then, each time VB rebuilds
'your server exe, it will steal the classid(s) out of the previous version.  It will also
'do other checking to try to make sure you have maintained compatibility with earlier
'versions.

Private Sub Class_Terminate()
'  Debug.Print "CbServerProj.CbServerClass has terminated"
End Sub
Private Sub Class_Initialize()
'  Debug.Print "CbServerProj.CbServerClass has intialized"
End Sub

Public Function AddObjectReference(Caller As Object) As Boolean
  On Error GoTo AddObjectReferenceError
  
  Set gObjRef = Caller
  frmMain.Timer1.Enabled = True
  AddObjectReference = True
  gbConnected = True
  Exit Function

AddObjectReferenceError:
  #If DEBUG_ON Then
  MsgBox Error$, vbOKOnly + vbExclamation, "AddObjectReference - Error" & Str$(Err)
  #End If
  AddObjectReference = False
  Exit Function
End Function

Public Function DropObjectReference(Caller As Object) As Boolean
Dim iCounter As Integer
On Error GoTo DropObjReferenceError

  If gObjRef Is Caller Then
    gbConnected = False
    frmMain.Timer1.Enabled = False
    Unload frmMain
    DropObjectReference = True
  Else
'    Debug.Print "Caller not the same as ObjRef.  Unable to quit."
    DropObjectReference = False
  End If
Exit Function

DropObjReferenceError:
  #If DEBUG_ON Then
  MsgBox Error$, vbOKOnly + vbExclamation, "DropObjectReference - Error" & Str$(Err)
  #End If
  DropObjectReference = False
  Exit Function

End Function

Public Function SetInterval(iInterval As Integer) As Boolean
    
    On Error GoTo SetIntervalError
    frmMain.Timer1.Interval = (iInterval * 1000)
    frmMain.lblInterval.Caption = Format$(iInterval)
    SetInterval = True
    Exit Function
    
SetIntervalError:
    SetInterval = False
    Exit Function
    
End Function
