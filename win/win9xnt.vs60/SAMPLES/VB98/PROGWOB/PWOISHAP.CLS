VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "IShape"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

' The IShape class module is an "abstract
'   class" that defines the IShape
'   interface.  (Properly speaking, this is
'   not an abstract class:  A true abstract
'   class cannot be used to create objects.
'   This class, like any class defined in
'   Visual Basic, can be used to create
'   objects -- but those objects would not
'   be particularly useful because, like
'   a true abstract class, there are
'   members with no implementation.)

' TimeTest method takes no arguments, and
' --------      its implementation is
'   always void -- that is, it executes
'   no code.  Its only purpose is to
'   illustrate early vs. late binding.
Public Sub TimeTest()
End Sub

Public Sub DrawToPictureBox(ByVal pb As PictureBox)
End Sub

