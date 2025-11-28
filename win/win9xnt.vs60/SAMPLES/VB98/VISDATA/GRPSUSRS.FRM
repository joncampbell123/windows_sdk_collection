VERSION 5.00
Begin VB.Form frmGroupsUsers 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Groups/Users"
   ClientHeight    =   5010
   ClientLeft      =   4380
   ClientTop       =   1905
   ClientWidth     =   5760
   HelpContextID   =   2016088
   Icon            =   "GRPSUSRS.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MDIChild        =   -1  'True
   MinButton       =   0   'False
   ScaleHeight     =   5010
   ScaleWidth      =   5760
   ShowInTaskbar   =   0   'False
   Begin VB.ComboBox cboOwners 
      Height          =   315
      Left            =   3360
      Style           =   2  'Dropdown List
      TabIndex        =   28
      Top             =   2360
      Width           =   2295
   End
   Begin VB.Frame fraPermissions 
      Caption         =   "Permissions"
      Height          =   1695
      Left            =   2520
      TabIndex        =   16
      Top             =   2760
      Width           =   3135
      Begin VB.CommandButton cmdAssign 
         Caption         =   "&Assign"
         Height          =   300
         Left            =   120
         MaskColor       =   &H00000000&
         TabIndex        =   25
         Top             =   1280
         Width           =   1400
      End
      Begin VB.CheckBox chkDeleteData 
         Caption         =   "DeleteData"
         Height          =   255
         Left            =   1680
         MaskColor       =   &H00000000&
         TabIndex        =   24
         Top             =   1320
         Width           =   1335
      End
      Begin VB.CheckBox chkInsertData 
         Caption         =   "InsertData"
         Height          =   255
         Left            =   1680
         MaskColor       =   &H00000000&
         TabIndex        =   23
         Top             =   1000
         Width           =   1335
      End
      Begin VB.CheckBox chkUpdateData 
         Caption         =   "UpdateData"
         Height          =   255
         Left            =   1680
         MaskColor       =   &H00000000&
         TabIndex        =   22
         Top             =   680
         Width           =   1335
      End
      Begin VB.CheckBox chkReadData 
         Caption         =   "ReadData"
         Height          =   255
         Left            =   1680
         MaskColor       =   &H00000000&
         TabIndex        =   21
         Top             =   360
         Width           =   1335
      End
      Begin VB.CheckBox chkAdminister 
         Caption         =   "Administer"
         Height          =   255
         Left            =   120
         MaskColor       =   &H00000000&
         TabIndex        =   20
         Top             =   1000
         Width           =   1455
      End
      Begin VB.CheckBox chkModifyDesign 
         Caption         =   "ModifyDesign"
         Height          =   255
         Left            =   120
         MaskColor       =   &H00000000&
         TabIndex        =   19
         Top             =   680
         Width           =   1575
      End
      Begin VB.CheckBox chkReadDesign 
         Caption         =   "ReadDesign"
         Height          =   255
         Left            =   120
         MaskColor       =   &H00000000&
         TabIndex        =   18
         Top             =   360
         Width           =   1455
      End
   End
   Begin VB.OptionButton optGroups 
      Caption         =   "Groups"
      Height          =   255
      Left            =   1200
      MaskColor       =   &H00000000&
      TabIndex        =   2
      Top             =   120
      Width           =   975
   End
   Begin VB.OptionButton optUsers 
      Caption         =   "Users"
      Height          =   255
      Left            =   120
      MaskColor       =   &H00000000&
      TabIndex        =   1
      Top             =   120
      Value           =   -1  'True
      Width           =   855
   End
   Begin VB.CommandButton cmdClose 
      Cancel          =   -1  'True
      Caption         =   "&Close"
      Height          =   375
      Left            =   2160
      MaskColor       =   &H00000000&
      TabIndex        =   0
      Top             =   4560
      Width           =   1335
   End
   Begin VB.ListBox lstTablesQuerys 
      Height          =   1620
      ItemData        =   "GRPSUSRS.frx":030A
      Left            =   2520
      List            =   "GRPSUSRS.frx":030C
      MultiSelect     =   2  'Extended
      TabIndex        =   15
      Top             =   480
      Width           =   3135
   End
   Begin VB.PictureBox picUsers 
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   3975
      Left            =   120
      ScaleHeight     =   3975
      ScaleWidth      =   2205
      TabIndex        =   3
      Top             =   480
      Width           =   2205
      Begin VB.CommandButton cmdPassword 
         Caption         =   "&Set/Clear Password"
         Height          =   300
         Left            =   15
         MaskColor       =   &H00000000&
         TabIndex        =   26
         Top             =   3615
         Width           =   2160
      End
      Begin VB.ListBox lstUsersGroups 
         Height          =   1035
         ItemData        =   "GRPSUSRS.frx":030E
         Left            =   0
         List            =   "GRPSUSRS.frx":0310
         MultiSelect     =   1  'Simple
         TabIndex        =   12
         Top             =   2400
         Width           =   2175
      End
      Begin VB.CommandButton cmdDeleteUser 
         Caption         =   "&Delete"
         Height          =   300
         Left            =   1080
         MaskColor       =   &H00000000&
         TabIndex        =   10
         Top             =   1800
         Width           =   1080
      End
      Begin VB.CommandButton cmdNewUser 
         Caption         =   "&New"
         Height          =   300
         Left            =   0
         MaskColor       =   &H00000000&
         TabIndex        =   9
         Top             =   1800
         Width           =   1080
      End
      Begin VB.ListBox lstUsers 
         Height          =   1620
         ItemData        =   "GRPSUSRS.frx":0312
         Left            =   0
         List            =   "GRPSUSRS.frx":0314
         TabIndex        =   4
         Top             =   0
         Width           =   2175
      End
      Begin VB.Label lblLabels 
         Caption         =   "Groups Belonged to:"
         Height          =   255
         Index           =   1
         Left            =   0
         TabIndex        =   11
         Top             =   2160
         Width           =   2055
      End
   End
   Begin VB.PictureBox picGroups 
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   3615
      Left            =   120
      ScaleHeight     =   3615
      ScaleWidth      =   2205
      TabIndex        =   5
      Top             =   480
      Visible         =   0   'False
      Width           =   2205
      Begin VB.ListBox lstGroupsUsers 
         Height          =   1035
         ItemData        =   "GRPSUSRS.frx":0316
         Left            =   0
         List            =   "GRPSUSRS.frx":0318
         MultiSelect     =   1  'Simple
         TabIndex        =   13
         Top             =   2400
         Width           =   2175
      End
      Begin VB.CommandButton cmdDeleteGroup 
         Caption         =   "&Delete"
         Height          =   300
         Left            =   1200
         MaskColor       =   &H00000000&
         TabIndex        =   8
         Top             =   1800
         Width           =   975
      End
      Begin VB.CommandButton cmdNewGroup 
         Caption         =   "&New"
         Height          =   300
         Left            =   0
         MaskColor       =   &H00000000&
         TabIndex        =   7
         Top             =   1800
         Width           =   975
      End
      Begin VB.ListBox lstGroups 
         Height          =   1620
         ItemData        =   "GRPSUSRS.frx":031A
         Left            =   0
         List            =   "GRPSUSRS.frx":031C
         TabIndex        =   6
         Top             =   0
         Width           =   2175
      End
      Begin VB.Label lblLabels 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Members:"
         Height          =   255
         Index           =   2
         Left            =   120
         TabIndex        =   14
         Top             =   2160
         Width           =   2055
      End
   End
   Begin VB.Label lblLabels 
      Caption         =   "Owner:"
      Height          =   255
      Index           =   3
      Left            =   2520
      TabIndex        =   27
      Top             =   2400
      Width           =   735
   End
   Begin VB.Label lblLabels 
      Caption         =   "Tables/Querys:"
      Height          =   255
      Index           =   0
      Left            =   2520
      TabIndex        =   17
      Top             =   165
      Width           =   2055
   End
   Begin VB.Line Line1 
      BorderWidth     =   3
      X1              =   2400
      X2              =   2400
      Y1              =   120
      Y2              =   4440
   End
End
Attribute VB_Name = "frmGroupsUsers"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'>>>>>>>>>>>>>>>>>>>>>>>>
Const FORMCAPTION = "Groups/Users/Permissions"
Const Label1 = "Tables/Querys:"
Const Label2 = "Groups Belonged to:"
Const LABEL3 = "Members:"
Const LABEL4 = "Owner:"
Const BUTTON1 = "&New"
Const BUTTON2 = "&Delete"
Const BUTTON3 = "&Set/Clear Password"
Const BUTTON4 = "&Assign"
Const BUTTON5 = "&Close"
Const OPTION1 = "Users"
Const OPTION2 = "Groups"
Const FRAME1 = "Permissions"
Const MSG1 = "You do not have permission to change the Owner!"
Const MSG2 = "No Group Selected!"
Const MSG3 = "Delete Group?"
Const MSG4 = "No User Selected!"
Const MSG5 = "Delete User?"
Const MSG6 = "New Group"
Const MSG7 = "New User"
Const MSG8 = "Clear the Password?"
Const MSG9 = "No Object Selected!"
'>>>>>>>>>>>>>>>>>>>>>>>>


Dim mbSettingUser As Integer
Dim mbSettingOwner As Integer
Dim mbSettingPerm As Integer
Dim mbLoading As Integer
Dim mobjCurrObject As Object    'currently selected table or query

Private Sub cboOwners_Click()
  On Error GoTo COErr
  
  'if we are setting thru code, just exit
  If mbSettingOwner Then Exit Sub
  
  If (mobjCurrObject.Permissions And dbSecWriteOwner) = dbSecWriteOwner Then
    'try to set it
    mobjCurrObject.Owner = cboOwners.Text
  Else
    MsgBox MSG1, 48
    Exit Sub
  End If
  
  Exit Sub
  
COErr:
  ShowError
End Sub

Private Sub chkAdminister_Click()
  If mbSettingPerm Then Exit Sub
  
  If chkAdminister.Value = vbChecked Then
    'set all of them
    chkReadDesign.Value = vbChecked
    chkModifyDesign.Value = vbChecked
    chkReadData.Value = vbChecked
    chkUpdateData.Value = vbChecked
    chkInsertData.Value = vbChecked
    chkDeleteData.Value = vbChecked
  End If
End Sub

Private Sub chkDeleteData_Click()
  If mbSettingPerm Then Exit Sub
  
  If chkDeleteData.Value = vbUnchecked Then
    'unset others that need it
    chkAdminister.Value = vbUnchecked
    chkModifyDesign.Value = vbUnchecked
  Else
    chkReadDesign.Value = vbChecked
    chkReadData.Value = vbChecked
  End If
End Sub

Private Sub chkInsertData_Click()
  If mbSettingPerm Then Exit Sub
  
  If chkInsertData.Value = vbUnchecked Then
    'unset others that need it
    chkAdminister.Value = vbUnchecked
  Else
    chkReadDesign.Value = vbChecked
    chkReadData.Value = vbChecked
  End If
End Sub

Private Sub chkModifyDesign_Click()
  If mbSettingPerm Then Exit Sub
  
  If chkModifyDesign.Value = vbUnchecked Then
    'unset administer of them
    chkAdminister.Value = vbUnchecked
  Else
    chkReadDesign.Value = vbChecked
    chkReadData.Value = vbChecked
    chkInsertData.Value = vbChecked
    chkDeleteData.Value = vbChecked
  End If
End Sub

Private Sub chkReadData_Click()
  If mbSettingPerm Then Exit Sub
  
  If chkReadData.Value = vbUnchecked Then
    'unset others that need it
    chkAdminister.Value = vbUnchecked
    chkModifyDesign.Value = vbUnchecked
  Else
    chkReadDesign.Value = vbChecked
  End If
End Sub

Private Sub chkReadDesign_Click()
  If mbSettingPerm Then Exit Sub
  
  If chkReadDesign.Value = vbUnchecked Then
    'unset all of them
    chkAdminister.Value = vbUnchecked
    chkModifyDesign.Value = vbUnchecked
    chkReadData.Value = vbUnchecked
    chkUpdateData.Value = vbUnchecked
    chkInsertData.Value = vbUnchecked
    chkDeleteData.Value = vbUnchecked
  End If
End Sub

Private Sub chkUpdateData_Click()
  If mbSettingPerm Then Exit Sub
  
  If chkUpdateData.Value = vbUnchecked Then
    'unset others that need it
    chkAdminister.Value = vbUnchecked
    chkModifyDesign.Value = vbUnchecked
  Else
    chkReadDesign.Value = vbChecked
    chkReadData.Value = vbChecked
  End If
End Sub

Private Sub cmdAssign_Click()
  SetPermissions True    'this will assign them
End Sub

Private Sub cmdClose_Click()
  Unload Me
End Sub

Private Sub cmdDeleteGroup_Click()
  On Error GoTo DGErr
  
  Dim i As Integer
    
  If lstGroups.ListIndex < 0 Then
    Beep
    MsgBox MSG2
    Exit Sub
  End If
  If MsgBox(MSG3, vbYesNo + vbQuestion) <> vbYes Then Exit Sub
  
  gwsMainWS.Groups.Delete lstGroups.Text
  i = lstGroups.ListIndex
  lstGroups.RemoveItem i
  lstUsersGroups.RemoveItem i
  If lstGroups.ListCount > 0 Then
    lstGroups.ListIndex = 0
  Else
    'need to unselect all users
    For i = 0 To lstGroupsUsers.ListCount - 1
      lstGroupsUsers.Selected(i) = False
    Next
  End If
  
  Exit Sub
  
DGErr:
  ShowError
End Sub

Private Sub cmdDeleteUser_Click()
  On Error GoTo DUErr
  
  Dim i As Integer
  
  If lstUsers.ListIndex < 0 Then
    Beep
    MsgBox MSG4
    Exit Sub
  End If
  If MsgBox(MSG5, vbYesNo + vbQuestion) <> vbYes Then Exit Sub
  
  gwsMainWS.Users.Delete lstUsers.Text
  lstUsers.RemoveItem lstUsers.ListIndex
  If lstUsers.ListCount > 0 Then
    lstUsers.ListIndex = 0
  Else
    'need to unselect all groups
    For i = 0 To lstUsersGroups.ListCount - 1
      lstUsersGroups.Selected(i) = False
    Next
  End If
  Exit Sub
  
DUErr:
  ShowError
End Sub

Private Sub cmdNewGroup_Click()
  frmNewUserGroup.UserOrGroup = 1
  frmNewUserGroup.Caption = MSG6
  frmNewUserGroup.Show vbModal
End Sub

Private Sub cmdNewUser_Click()
  frmNewUserGroup.UserOrGroup = 0
  frmNewUserGroup.Caption = MSG7
  frmNewUserGroup.Show vbModal
End Sub

Private Sub cmdPassword_Click()
  On Error GoTo CPErr
  
  If lstUsers.Text = gwsMainWS.UserName Then
    frmNewPassword.Show vbModal
  Else
    If MsgBox(MSG8, vbYesNo + vbQuestion) = vbYes Then
      gwsMainWS.Users(lstUsers.Text).NewPassword vbNullString, vbNullString
    End If
  End If
  
  Exit Sub
  
CPErr:
  ShowError
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
  If KeyCode = vbKeyF1 And Shift = 0 Then
    DisplayTopic 2016088
  End If
End Sub

Private Sub Form_Load()
  On Error GoTo FLErr
  
  Dim grp As Group
  Dim usr As User
  Dim i As Integer

  Me.Caption = FORMCAPTION
  optUsers.Caption = OPTION1
  optGroups.Caption = OPTION2
  fraPermissions.Caption = FRAME1
  cmdNewUser.Caption = BUTTON1
  cmdDeleteUser.Caption = BUTTON2
  cmdNewGroup.Caption = BUTTON1
  cmdDeleteGroup.Caption = BUTTON2
  cmdPassword.Caption = BUTTON3
  cmdAssign.Caption = BUTTON4
  cmdClose.Caption = BUTTON5
  lblLabels(0).Caption = Label1
  lblLabels(1).Caption = Label2
  lblLabels(2).Caption = LABEL3
  lblLabels(3).Caption = LABEL4

  mbLoading = True
  'add the groups and users
  For Each usr In gwsMainWS.Users
    lstUsers.AddItem usr.Name
    lstGroupsUsers.AddItem usr.Name
    cboOwners.AddItem usr.Name
  Next
  For Each grp In gwsMainWS.Groups
    lstGroups.AddItem grp.Name
    lstUsersGroups.AddItem grp.Name
    cboOwners.AddItem grp.Name
  Next
  'set the 1st item if possible
  If lstUsers.ListCount > 0 Then
    lstUsers.ListIndex = 0
  End If
  If lstGroups.ListCount > 0 Then
    lstGroups.ListIndex = 0
  End If
  
  'fill in the objects lists from the tables form
  GetTableList lstTablesQuerys, True, False, False
  mbLoading = False
  lstTablesQuerys.Selected(0) = True
  Screen.MousePointer = vbDefault
  Exit Sub
  
FLErr:
  mbLoading = False
  ShowError
End Sub

Private Sub lstGroups_Click()
  On Error GoTo GSErr

  Dim i As Integer
  
  mbSettingUser = True
  For i = 0 To lstGroupsUsers.ListCount - 1
    If IsMemberOf(lstGroups.Text, lstGroupsUsers.List(i)) Then
      lstGroupsUsers.Selected(i) = True
    Else
      lstGroupsUsers.Selected(i) = False
    End If
  Next
  mbSettingUser = False
  
  Exit Sub
  
GSErr:
  ShowError
  mbSettingUser = False
End Sub

Private Sub lstGroupsUsers_Click()
  On Error GoTo GUErr
  
  Dim usr As User
  
  If mbSettingUser Then Exit Sub
  
  If lstGroups.ListIndex < 0 Then
    Beep
    MsgBox MSG2
    Exit Sub
  End If
  
  If lstGroupsUsers.Selected(lstGroupsUsers.ListIndex) Then
    'add the user to the group
    Set usr = gwsMainWS.CreateUser(lstGroupsUsers.Text)
    gwsMainWS.Groups(lstGroups.Text).Users.Append usr
    gwsMainWS.Users(lstGroupsUsers.Text).Groups.Refresh
  Else
    'remove the user from the group
    gwsMainWS.Groups(lstGroups.Text).Users.Delete lstGroupsUsers.Text
    gwsMainWS.Users(lstGroupsUsers.Text).Groups.Refresh
  End If
  
  Exit Sub
  
GUErr:
  ShowError
End Sub

Private Sub lstTablesQuerys_Click()
  SetPermissions False
End Sub

Private Sub lstUsers_Click()
  On Error GoTo USErr

  Dim i As Integer
  
  mbSettingUser = True
  For i = 0 To lstUsersGroups.ListCount - 1
    If IsMemberOf(lstUsersGroups.List(i), lstUsers.Text) Then
      lstUsersGroups.Selected(i) = True
    Else
      lstUsersGroups.Selected(i) = False
    End If
  Next
  mbSettingUser = False
  
  'show permissions
  SetPermissions False
  
  Exit Sub
  
USErr:
  ShowError
  mbSettingUser = False
End Sub

Private Function IsMemberOf(rsGrp As String, rsUsr As String) As Integer
  Dim usr As User
  Dim grp As Group
  Dim i As Integer
  
  Set usr = gwsMainWS.Users(rsUsr)
  
  For Each grp In usr.Groups
    If grp.Name = rsGrp Then
      IsMemberOf = True
      Exit Function
    End If
  Next
  IsMemberOf = False
End Function

Private Sub lstUsersGroups_Click()
  On Error GoTo UGErr
  
  Dim grp As Group
  
  If mbSettingUser Then Exit Sub
  
  If lstUsers.ListIndex < 0 Then
    Beep
    MsgBox MSG4
    Exit Sub
  End If
  
  If lstUsersGroups.Selected(lstUsersGroups.ListIndex) Then
    'add the group to the user
    Set grp = gwsMainWS.CreateGroup(lstUsersGroups.Text)
    gwsMainWS.Users(lstUsers.Text).Groups.Append grp
    gwsMainWS.Groups(lstUsersGroups.Text).Users.Refresh
  Else
    'remove the group from the user
    gwsMainWS.Users(lstUsers.Text).Groups.Delete lstUsersGroups.Text
    gwsMainWS.Groups(lstUsersGroups.Text).Users.Refresh
  End If
  
  Exit Sub
  
UGErr:
  ShowError
End Sub

Private Sub optGroups_Click()
  picUsers.Visible = False
  picGroups.Visible = True
End Sub

Private Sub optUsers_Click()
  picGroups.Visible = False
  picUsers.Visible = True
End Sub

Private Sub SetPermissions(rbAssign As Integer)
  On Error GoTo SPErr
  
  Dim lPermissions As Long
  Dim lPermissions2 As Long
  Dim bUncommon As Integer    'multiselected flag for common permissions
  Dim nCnt As Integer         'count of selected objects
  Dim sUserGroup As String
  Dim sObject As String
  Dim i As Integer
  
  mbSettingPerm = True
  
  If rbAssign Then
    'determine what permissions are set and Or them together
    If chkReadDesign.Value = vbUnchecked Then
      lPermissions = 0
    Else
      If chkAdminister.Value = vbChecked Then
        'set them all
        lPermissions = dbSecFullAccess Or _
                       dbSecReadDef Or _
                       dbSecWriteDef Or _
                       dbSecRetrieveData Or _
                       dbSecReplaceData Or _
                       dbSecInsertData Or _
                       dbSecDeleteData
      Else
        'set them one at a time
        lPermissions = dbSecReadDef
        If chkModifyDesign.Value = vbChecked Then
          lPermissions = lPermissions Or dbSecWriteDef
        End If
        If chkReadData.Value = vbChecked Then
          lPermissions = lPermissions Or dbSecRetrieveData
        End If
        If chkUpdateData.Value = vbChecked Then
          lPermissions = lPermissions Or dbSecReplaceData
        End If
        If chkInsertData.Value = vbChecked Then
          lPermissions = lPermissions Or dbSecInsertData
        End If
        If chkDeleteData.Value = vbChecked Then
          lPermissions = lPermissions Or dbSecDeleteData
        End If
      End If
    End If
  End If
  
  'determine if it's a user or a group
  If optUsers.Value Then
    'users
    sUserGroup = lstUsers.Text
  Else
    'groups
    sUserGroup = lstGroups.Text
  End If
  
  'set or get the permissions
  If lstTablesQuerys.ListIndex = -1 Then
    If mbLoading = False Then   'don't issue error on form load
      Beep
      MsgBox MSG9
    End If
    Exit Sub
  End If
  
  'walk the object list and process the selected objects
  For i = 0 To lstTablesQuerys.ListCount - 1
    If lstTablesQuerys.Selected(i) Then
      nCnt = nCnt + 1
      If lstTablesQuerys.ListIndex = 0 Then
        'must be <New Object>
        gdbCurrentDB.Containers("Tables").UserName = sUserGroup
        If rbAssign = False Then
          lPermissions = gdbCurrentDB.Containers("Tables").Permissions
        Else
          gdbCurrentDB.Containers("Tables").Permissions = lPermissions
        End If
        ShowOwner gdbCurrentDB.Containers("Tables")
        Set mobjCurrObject = gdbCurrentDB.Containers("Tables")
      Else
        sObject = StripConnect(lstTablesQuerys.List(i))
        'a table ot query was selected
        gdbCurrentDB.Containers("Tables").Documents(sObject).UserName = sUserGroup
        If rbAssign = False Then
          lPermissions = gdbCurrentDB.Containers("Tables").Documents(sObject).Permissions
        Else
          gdbCurrentDB.Containers("Tables").Documents(sObject).Permissions = lPermissions
        End If
        ShowOwner gdbCurrentDB.Containers("Tables").Documents(sObject)
        Set mobjCurrObject = gdbCurrentDB.Containers("Tables").Documents(sObject)
      End If
      If nCnt > 1 Then
        'if there is more than 1, they need to match or we set the flag
        If lPermissions <> lPermissions2 Then
          bUncommon = True
        End If
      End If
      'store it for the next time through
      lPermissions2 = lPermissions
    End If
  Next
  
  If rbAssign = False Then
    If bUncommon Then
      'there was some mismatch so they need to be greyed
      chkReadDesign.Value = 2
      chkModifyDesign.Value = 2
      chkAdminister.Value = 2
      chkReadData.Value = 2
      chkUpdateData.Value = 2
      chkInsertData.Value = 2
      chkDeleteData.Value = 2
    Else
      'there was either only one or they were all the same
      'so we need to set them appropriately
      If (lPermissions And dbSecReadDef) = dbSecReadDef Then
        chkReadDesign.Value = vbChecked
      Else
        chkReadDesign.Value = vbUnchecked
      End If
      If (lPermissions And dbSecWriteDef) = dbSecWriteDef Then
        chkModifyDesign.Value = vbChecked
      Else
        chkModifyDesign.Value = vbUnchecked
      End If
      If (lPermissions And dbSecFullAccess) = dbSecFullAccess Then
        chkAdminister.Value = vbChecked
      Else
        chkAdminister.Value = vbUnchecked
      End If
      If (lPermissions And dbSecRetrieveData) = dbSecRetrieveData Then
        chkReadData.Value = vbChecked
      Else
        chkReadData.Value = vbUnchecked
      End If
      If (lPermissions And dbSecReplaceData) = dbSecReplaceData Then
        chkUpdateData.Value = vbChecked
      Else
        chkUpdateData.Value = vbUnchecked
      End If
      If (lPermissions And dbSecInsertData) = dbSecInsertData Then
        chkInsertData.Value = vbChecked
      Else
        chkInsertData.Value = vbUnchecked
      End If
      If (lPermissions And dbSecDeleteData) = dbSecDeleteData Then
        chkDeleteData.Value = vbChecked
      Else
        chkDeleteData.Value = vbUnchecked
      End If
    End If
  End If
  
  mbSettingPerm = False
  Exit Sub
  
SPErr:
  mbSettingPerm = False
  ShowError
End Sub

Private Sub ShowOwner(vObj As Object)
  On Error GoTo SOErr
  
  Dim i As Integer
  
  For i = 0 To cboOwners.ListCount - 1
    If cboOwners.List(i) = vObj.Owner Then
      mbSettingOwner = True
      cboOwners.ListIndex = i
      mbSettingOwner = False
      Exit For
    End If
  Next

  Exit Sub
  
SOErr:
  mbSettingOwner = True
  cboOwners.ListIndex = -1
  mbSettingOwner = False
  ShowError
End Sub
