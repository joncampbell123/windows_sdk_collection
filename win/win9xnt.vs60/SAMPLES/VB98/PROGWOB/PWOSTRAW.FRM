VERSION 5.00
Begin VB.Form frmStraw 
   Caption         =   "Employees Collection - House of Straw"
   ClientHeight    =   3525
   ClientLeft      =   1140
   ClientTop       =   1515
   ClientWidth     =   4995
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   3525
   ScaleWidth      =   4995
   WhatsThisHelp   =   -1  'True
   Begin VB.CommandButton cmdTrouble 
      Caption         =   "&Trouble"
      Height          =   465
      Left            =   3150
      TabIndex        =   8
      Top             =   2250
      Width           =   1545
   End
   Begin VB.CommandButton cmdClose 
      Caption         =   "&Close"
      Height          =   285
      Left            =   3150
      TabIndex        =   9
      Top             =   2880
      Width           =   1545
   End
   Begin VB.CommandButton cmdListEmployees 
      Caption         =   "&Refresh List"
      Height          =   285
      Left            =   3150
      TabIndex        =   7
      Top             =   1800
      Width           =   1545
   End
   Begin VB.CommandButton cmdDeleteEmployee 
      Caption         =   "&Delete"
      Height          =   285
      Left            =   3150
      TabIndex        =   6
      Top             =   1440
      Width           =   1545
   End
   Begin VB.CommandButton cmdAddEmployee 
      Caption         =   "&Add"
      Default         =   -1  'True
      Enabled         =   0   'False
      Height          =   285
      Left            =   3150
      TabIndex        =   5
      Top             =   1080
      Width           =   1545
   End
   Begin VB.ListBox lstEmployees 
      Height          =   1845
      Left            =   180
      Sorted          =   -1  'True
      TabIndex        =   4
      Top             =   1080
      Width           =   2715
   End
   Begin VB.TextBox txtSalary 
      Height          =   285
      Left            =   2700
      TabIndex        =   3
      Top             =   450
      Width           =   1995
   End
   Begin VB.TextBox txtName 
      Height          =   285
      Left            =   180
      TabIndex        =   1
      Top             =   450
      Width           =   2265
   End
   Begin VB.Label Label2 
      Caption         =   "&Salary"
      Height          =   195
      Left            =   2700
      TabIndex        =   2
      Top             =   180
      Width           =   2025
   End
   Begin VB.Label Label1 
      Caption         =   "&Name"
      Height          =   195
      Left            =   180
      TabIndex        =   0
      Top             =   180
      Width           =   2265
   End
End
Attribute VB_Name = "frmStraw"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public sbMain As New SmallBusiness1

Private Sub cmdAddEmployee_Click()
    Dim empNew As New Employee
    If Not IsNumeric(txtSalary) Then
        MsgBox "Salary is not a valid amount."
        ' Set focus on salary field, and
        '   select all text.
        With txtSalary
            .SetFocus
            .SelStart = 0
            .SelLength = Len(.Text)
        End With
        Exit Sub
    End If
    With empNew
        .ID = sbMain.NewEmployeeID
        .Name = txtName.Text
        .Salary = CDbl(txtSalary.Text)
        sbMain.Employees.Add empNew, .ID
        lstEmployees.AddItem .ID & ", " & .Name & ", " & .Salary
        With lstEmployees
            ' Select the newly added item.
            .ListIndex = .NewIndex
        End With
    End With
    txtName.Text = ""
    txtSalary.Text = ""
    txtName.SetFocus
End Sub

Private Sub cmdClose_Click()
    Unload Me
End Sub

Private Sub cmdDeleteEmployee_Click()
    Dim lngDeletedItem As Long
    With lstEmployees
        lngDeletedItem = .ListIndex
        ' Check to make sure there is an employee selected.
        If .ListIndex > -1 Then
            ' The employee ID is the first six characters on the line.
            sbMain.Employees.Remove Left(lstEmployees.Text, 6)
            ' Remove the selected item.
            .RemoveItem .ListIndex
            If .ListCount = 0 Then
                ' If the list is now empty,
                '   don't attempt to set a new
                '   selection.
                Exit Sub
            End If
            ' Was the deleted item at the very bottom of
            '   the list box?  If so, its index wil be
            '   greater than or equal to the list count...
            If .ListCount <= lngDeletedItem Then
                '   ...so set the current selection to
                '   the new bottom item...
                .ListIndex = lngDeletedItem - 1
            Else
                '   ...otherwise, keep the selection in
                '   the same physical position in the
                '   list.
                .ListIndex = lngDeletedItem
            End If
        Else
            MsgBox "No employee selected."
        End If
    End With
End Sub

Private Sub cmdListEmployees_Click()
    Dim emp As Employee
    With lstEmployees
        .Clear
        For Each emp In sbMain.Employees
            .AddItem emp.ID & ", " & emp.Name & ", " & emp.Salary
            ' After you press the Trouble button, clicking
            '   Refresh causes a type mismatch error (either
            '   in the For Each statement, if the invalid
            '   item is the first one in the list, or at the
            '   Next statement) when Visual Basic attempts
            '   to put the reference to frmStraw into the
            '   iteration variable emp.  To continue exe-
            '   cution, drag the yellow execution arrow to
            '   End Sub (or click on End Sub and then press
            '   Ctrl+F9), then press F5.
        Next
        ' When you break here, see note above.
        '
        If .ListCount <> 0 Then
            ' If there are any items in the list,
            '   select the first.
            .ListIndex = 0
        End If
    End With
End Sub

Private Sub cmdTrouble_Click()
    ' Because the Collection object accepts
    '   any object, a coding error can put
    '   an invalid object in the collection.
    sbMain.Employees.Add Me
    MsgBox "A reference to the data entry form has just been added to the collection.  Press Refresh List to see the error this causes."
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' Set all references to this form to
    '   Nothing, to release its resources.
    '   This means doing two things:
    '   (1) Set the hidden global variable
    '       the form to Nothing:
    Set frmStraw = Nothing
    '   (2) Clear the collection object,
    '       because the Trouble button
    '       put a reference to the form
    '       into the collection -- creating
    '       a circular reference (sbMain
    '       has a reference to Employees,
    '       which has a reference to the
    '       form, which has a reference
    '       to sbMain) that keeps all the
    '       objects alive.
    Set sbMain.Employees = Nothing
    '
    ' Of course, it's a bug that we can
    '   destroy the SmallBusiness object's
    '   Employees collection like this;
    '   but House of Straw is the way
    '   NOT to do things, after all.
End Sub

Private Sub txtName_Change()
    Call EnableAddButton
End Sub

Private Sub txtSalary_Change()
    Call EnableAddButton
End Sub

Private Sub txtSalary_KeyPress(KeyAscii As Integer)
    Select Case KeyAscii
        Case 48 To 57   ' Allow digits
        Case 8      ' Allow backspace
        Case 46     ' Allow period
        Case Else
            KeyAscii = 0
            Beep
    End Select
End Sub

Private Sub EnableAddButton()
    If (Len(txtName) > 0) And (Len(txtSalary) > 0) Then
        cmdAddEmployee.Enabled = True
    Else
        cmdAddEmployee.Enabled = False
    End If
End Sub
