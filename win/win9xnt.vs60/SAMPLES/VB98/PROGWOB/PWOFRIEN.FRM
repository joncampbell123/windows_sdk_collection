VERSION 5.00
Begin VB.Form frmFriends 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Friends Passing User-Defined Types"
   ClientHeight    =   3210
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5355
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3210
   ScaleWidth      =   5355
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txtString 
      Height          =   285
      Left            =   1080
      TabIndex        =   5
      Top             =   2640
      Width           =   4095
   End
   Begin VB.TextBox txtLong 
      Height          =   285
      Left            =   1080
      MaxLength       =   9
      TabIndex        =   3
      Top             =   2160
      Width           =   1815
   End
   Begin VB.TextBox txtInteger 
      Height          =   285
      Left            =   1080
      MaxLength       =   4
      TabIndex        =   1
      Top             =   1680
      Width           =   1215
   End
   Begin VB.CommandButton cmdMethod 
      Caption         =   "Friend &Method"
      Height          =   375
      Left            =   3120
      TabIndex        =   7
      Top             =   2040
      Width           =   2055
   End
   Begin VB.CommandButton cmdProperty 
      Caption         =   "Friend &Property"
      Height          =   375
      Left            =   3120
      TabIndex        =   6
      Top             =   1560
      Width           =   2055
   End
   Begin VB.Label Label4 
      Alignment       =   1  'Right Justify
      Caption         =   "&String:"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   2640
      Width           =   855
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Caption         =   "&Long:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   2160
      Width           =   855
   End
   Begin VB.Label Label2 
      Alignment       =   1  'Right Justify
      Caption         =   "&Integer:"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   1680
      Width           =   855
   End
   Begin VB.Label Label1 
      Caption         =   $"PWOFrien.frx":0000
      Height          =   1455
      Left            =   120
      TabIndex        =   8
      Top             =   120
      Width           =   5175
   End
End
Attribute VB_Name = "frmFriends"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
' Demonstrates Friend properties and
'   methods passing UDTs between objects.

Private mtc1 As TestClass
Private mtc2 As TestClass
    
' Use properties to assign/access a UDT.
Private Sub cmdProperty_Click()
    ' The SetDemoParts helper method assigns
    '   the contents of the text boxes to
    '   the user-defined type in the first
    '   TestClass object, so there will be
    '   something to pass to the second
    '   TestClass object.
    Call mtc1.SetDemoParts(CInt("0" & txtInteger), _
        CLng("0" & txtLong), txtString)
    '
    ' Show the first TestClass object's
    '   UDT elements before passing.
    Call mtc1.ShowDemo("Passing a UDT using a Property", "To be passed from:")
    '
    ' Directly assign the UDT from the
    '   first TestClass object to the
    '   UDT in the second TestClass object,
    '   using the Demo property.
    mtc2.Demo = mtc1.Demo
    '
    ' Show the second TestClass object's
    '   UDT elements.
    Call mtc2.ShowDemo("Passing a UDT using a Property", "Passed to:")
    '
    ' When the procedure ends, tc1 and tc2
    '   go out of scope and the TestClass
    '   objects terminate.
End Sub

' Use methods to assign/access a UDT.
Private Sub cmdMethod_Click()
    ' The SetDemoParts helper method assigns
    '   the contents of the text boxes to
    '   the user-defined type in the first
    '   TestClass object, so there will be
    '   something to pass to the second
    '   TestClass object.
    Call mtc1.SetDemoParts(CInt("0" & txtInteger), _
        CLng("0" & txtLong), txtString)
    '
    ' Show the first TestClass object's
    '   UDT elements before passing.
    Call mtc1.ShowDemo("Passing a UDT using a Method", "To be passed from:")
    '
    ' The GetDemo method of the first
    '   TestClass object returns the UDT,
    '   which is passed to the SetDemo
    '   method of the second TestClass
    '   object.
    Call mtc2.SetDemo(mtc1.GetDemo)
    '
    ' Show the second TestClass object's
    '   UDT elements.
    Call mtc2.ShowDemo("Passing a UDT using a Method", "Passed to:")
    '
    ' When the procedure ends, tc1 and tc2
    '   go out of scope and the TestClass
    '   objects terminate.
End Sub

Private Sub Form_Load()
    ' Create TestClass objects.
    Set mtc1 = New TestClass
    Set mtc2 = New TestClass
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' Free form's resources.
    Set frmFriends = Nothing
End Sub

Private Sub txtInteger_KeyPress(KeyAscii As Integer)
    Select Case KeyAscii
        Case 48 To 57     ' Allow digits.
        Case 8      ' Allow backspace.
        Case Else   ' Suppress everything else.
            Beep
            KeyAscii = 0
    End Select
End Sub

Private Sub txtLong_KeyPress(KeyAscii As Integer)
    Select Case KeyAscii
        Case 48 To 57     ' Allow digits.
        Case 8      ' Allow backspace.
        Case Else   ' Suppress everything else.
            Beep
            KeyAscii = 0
    End Select
End Sub

