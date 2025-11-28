VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "ComDlg32.OCX"
Begin VB.Form frmPublishers 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Publishers"
   ClientHeight    =   3090
   ClientLeft      =   3600
   ClientTop       =   4530
   ClientWidth     =   7350
   Icon            =   "frmpub.frx":0000
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3090
   ScaleWidth      =   7350
   ShowInTaskbar   =   0   'False
   Begin VB.CommandButton cmdAbout 
      Caption         =   "&About"
      Height          =   375
      Left            =   5880
      TabIndex        =   9
      Top             =   630
      Width           =   945
   End
   Begin VB.CommandButton cmdDone 
      Cancel          =   -1  'True
      Caption         =   "&Done"
      Height          =   375
      Left            =   5880
      TabIndex        =   8
      Top             =   240
      Width           =   945
   End
   Begin VB.OptionButton optListBox 
      Caption         =   "Use Standard List Box"
      Height          =   255
      Left            =   3360
      TabIndex        =   7
      Top             =   1860
      Width           =   2295
   End
   Begin VB.OptionButton optComboBox 
      Caption         =   "Use Standard Combo Box"
      Height          =   255
      Left            =   3360
      TabIndex        =   6
      Top             =   1560
      Value           =   -1  'True
      Width           =   2295
   End
   Begin VB.TextBox txtCity 
      DataSource      =   "datPublishers"
      Height          =   285
      Left            =   1740
      Locked          =   -1  'True
      TabIndex        =   3
      Top             =   1140
      Width           =   4000
   End
   Begin VB.TextBox txtAddress 
      DataSource      =   "datPublishers"
      Height          =   285
      Left            =   1740
      Locked          =   -1  'True
      TabIndex        =   2
      Top             =   840
      Width           =   4000
   End
   Begin VB.TextBox txtCompanyName 
      DataSource      =   "datPublishers"
      Height          =   285
      Left            =   1740
      Locked          =   -1  'True
      TabIndex        =   1
      Top             =   540
      Width           =   4000
   End
   Begin VB.TextBox txtName 
      DataSource      =   "datPublishers"
      Height          =   285
      Left            =   1740
      Locked          =   -1  'True
      TabIndex        =   0
      Top             =   240
      Width           =   4000
   End
   Begin VB.Data datPublishers 
      Align           =   2  'Align Bottom
      Connect         =   "Access"
      DatabaseName    =   ""
      DefaultCursorType=   0  'DefaultCursor
      DefaultType     =   2  'UseODBC
      Exclusive       =   0   'False
      Height          =   345
      Left            =   0
      Options         =   0
      ReadOnly        =   0   'False
      RecordsetType   =   2  'Snapshot
      RecordSource    =   ""
      Top             =   2745
      Width           =   7344
   End
   Begin VB.ComboBox cboState 
      DataSource      =   "datPublishers"
      Height          =   315
      Left            =   1740
      Sorted          =   -1  'True
      TabIndex        =   4
      Text            =   "cmbState"
      Top             =   1455
      Width           =   1215
   End
   Begin VB.ListBox lstState 
      DataSource      =   "datPublishers"
      Height          =   675
      Left            =   1740
      Sorted          =   -1  'True
      TabIndex        =   5
      Top             =   1455
      Visible         =   0   'False
      Width           =   1215
   End
   Begin MSComDlg.CommonDialog dlgDialog 
      Left            =   120
      Top             =   2160
      _ExtentX        =   847
      _ExtentY        =   847
      FontSize        =   1.73857e-39
   End
   Begin VB.Label Label5 
      Alignment       =   1  'Right Justify
      Caption         =   "State:"
      Height          =   195
      Left            =   360
      TabIndex        =   14
      Top             =   1500
      Width           =   1305
   End
   Begin VB.Label Label4 
      Alignment       =   1  'Right Justify
      Caption         =   "City:"
      Height          =   195
      Left            =   360
      TabIndex        =   13
      Top             =   1200
      Width           =   1305
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Caption         =   "Address:"
      Height          =   195
      Left            =   360
      TabIndex        =   12
      Top             =   900
      Width           =   1305
   End
   Begin VB.Label Label2 
      Alignment       =   1  'Right Justify
      Caption         =   "Company Name:"
      Height          =   195
      Left            =   360
      TabIndex        =   11
      Top             =   600
      Width           =   1305
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "Name:"
      Height          =   195
      Left            =   360
      TabIndex        =   10
      Top             =   300
      Width           =   1305
   End
End
Attribute VB_Name = "frmPublishers"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdAbout_Click()
   Dim frmX As New frmAbout
   '
   ' Display the About dialog box
   '
   frmX.Show vbModal
   Set frmX = Nothing
   
End Sub

Private Sub cmdDone_Click()
   Unload Me
End Sub

Private Sub Form_Load()
   Dim vntTemp As Variant
   Dim vnDummy As Variant
   
   On Error GoTo Form_LoadError
   
   ' Retrieve all the states in the table
   '
   datPublishers.DatabaseName = "biblio.mdb"
   datPublishers.RecordSource = "SELECT DISTINCT state FROM Publishers"
   datPublishers.Refresh
   '
   ' Load the distinct state abbreviations into the List and Combo Controls
   ' If the database name file is not valid an trappable error will occur here
   Do While Not datPublishers.Recordset.EOF
      vntTemp = datPublishers.Recordset!State
      If IsNull(vntTemp) Then vntTemp = ""
      cboState.AddItem CStr(vntTemp)
      lstState.AddItem CStr(vntTemp)
      datPublishers.Recordset.MoveNext
   Loop
   '
   ' Retrieve all records from Publishers table
   '
   datPublishers.RecordSource = "Publishers"
   datPublishers.RecordsetType = vbRSTypeTable
   datPublishers.Refresh
   '
   ' Link fields with data control
   '
   txtName.DataField = "Name"
   txtAddress.DataField = "Address"
   txtCompanyName.DataField = "Company Name"
   txtCity.DataField = "City"
   
   cboState.DataField = "State"
   lstState.DataField = "State"
    
Form_LoadExit:
Exit Sub
    
Form_LoadError:
    ' If the Biblio database can't be found, open the common dialog control
    ' to let the user find it.  The biblio.mdb file is located in the same
    ' directory in which vb5 has been installed.
    If Err = 3024 Then
        With dlgDialog
            .DialogTitle = "Can't Find Biblio.mdb"
            .Filter = "(*.MDB)|*.mdb"
            .ShowOpen
        End With
        
        If dlgDialog.filename <> "" Then
        'make sure that the database file returned is indeed biblio.mdb
            If Right(UCase(dlgDialog.filename), Len("\biblio.mdb")) = "\BIBLIO.MDB" Then
                datPublishers.DatabaseName = dlgDialog.filename
                datPublishers.RecordSource = "SELECT DISTINCT state FROM Publishers"
                datPublishers.Refresh
            End If
            Resume Next
        Else
            Unload Me
        End If
    ElseIf Err <> 0 Then ' another error
        MsgBox "Unexpected Error: " & Err.Description
        End
    End If
    Resume Form_LoadExit
   
End Sub


Private Sub optComboBox_Click()
   '
   ' toggle the visibility of the controls
   '
   cboState.Visible = True
   lstState.Visible = False
   
End Sub

Private Sub optListBox_Click()
   '
   ' toggle the visibility of the controls
   '
   cboState.Visible = False
   lstState.Visible = True

End Sub


