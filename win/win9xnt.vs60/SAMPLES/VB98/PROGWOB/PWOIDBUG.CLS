VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "IDebug"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

' IDebug interface is implemented by all
' ======    objects in the project.  It
'   contains debugging methods and
'   properties (in ProgWOb, there's only
'   one of these:  the DebugID property).
'
' Why have a separate interface?  In this
'   project, the Polygon class is used
'   two ways:  Polygon objects are
'   created, and the Polygon interface
'   is implemented by two other classes.
'   Thus, if we put a DebugID property on
'   Polygon's default interface, we would
'   have to put dummy DebugID properties
'   on those implementations, as well --
'   as was done with the TimeTest method.
'
' Having a separate interface requires
'   only one more line of code (the
'   Implements IDebug statement) than
'   putting DebugID on each class's
'   default interface.
'
'
' DebugID is implemented by providing a
' -------       Long to store the debug
'   ID, and calling DebugInit to set
'   the ID (in the Initialize event).
'
Public Property Get DebugID() As Long
End Property
