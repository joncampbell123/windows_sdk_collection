VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0   'False
  DataBindingBehavior = 1  'vbSimpleBound
  DataSourceBehavior  = 0  'vbNone
END
Attribute VB_Name = "MyConsumer"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit
Private mDirectory As String
Public Property Get DirName() As String
    DirName = mDirectory
End Property
Public Property Let DirName(mNewDir As String)
    mDirectory = mNewDir
    ' Display the new value in the Immediate window
    Debug.Print mDirectory
End Property
