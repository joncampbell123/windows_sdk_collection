VERSION 5.00
Begin VB.Form frmSticks 
   Caption         =   "Employees Collection - House of Sticks"
   ClientHeight    =   3525
   ClientLeft      =   1140
   ClientTop       =   1515
   ClientWidth     =   4995
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   3525
   ScaleWidth      =   4995
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
Attribute VB_Name = "frmSticks"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public sbMain As New SmallBusiness2

Private Sub cmdAddEmployee_Click()
    Dim empNew As Employee
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
    Set empNew = sbMain.AddEmployee(txtName.Text, txtSalary.Text)
    With empNew
        lstEmployees.AddItem .ID & ", " & .Name & ", " & .Salary
    End With
    With lstEmployees
        ' Select the newly added item.
        .ListIndex = .NewIndex
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
            sbMain.DeleteEmployee Left(lstEmployees.Text, 6)
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
    Dim lngCt As Long
    Dim emp As Employee
    With lstEmployees
        .Clear
        For lngCt = 1 To sbMain.EmployeeCount
            Set emp = sbMain.Employees(lngCt)
            .AddItem emp.ID & ", " & emp.Name & ", " & emp.Salary
        Next lngCt
        If .ListCount <> 0 Then
            ' If there are any items in the list,
            '   select the first.
            .ListIndex = 0
        End If
    End With
End Sub

Private Sub cmdTrouble_Click()
    ' While the private Collection object
    '   protects against random objects
    '   being added to the collection, an
    '   internal coding error in the
    '   SmallBusiness2 object can still
    '   cause problems.
    sbMain.Trouble
    MsgBox "An internal coding error in SmallBusiness2 has added an uninitialized Employee to the collection.  To see the problem this causes, press Refresh List."
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' Set the hidden global variable for
    '   this form to Nothing, to release
    '   its resources.
    Set frmSticks = Nothing
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

