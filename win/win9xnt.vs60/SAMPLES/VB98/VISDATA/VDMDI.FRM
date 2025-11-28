VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "ComDlg32.ocx"
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.MDIForm frmMDI 
   BackColor       =   &H8000000C&
   Caption         =   "VisData"
   ClientHeight    =   6780
   ClientLeft      =   4110
   ClientTop       =   2625
   ClientWidth     =   10005
   HelpContextID   =   2016116
   Icon            =   "VDMDI.frx":0000
   LinkTopic       =   "MDIForm1"
   LockControls    =   -1  'True
   Begin MSComCtlLib.Toolbar tlbToolBar 
      Align           =   1  'Align Top
      Height          =   360
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   10005
      _ExtentX        =   17648
      _ExtentY        =   635
      ButtonWidth     =   609
      ButtonHeight    =   582
      AllowCustomize  =   0   'False
      Wrappable       =   0   'False
      Appearance      =   1
      HelpContextID   =   65278
      Style           =   1
      ImageList       =   "imlToolbarPics"
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   12
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Table"
            Object.ToolTipText     =   "Table type Recordset"
            ImageIndex      =   1
            Style           =   2
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Dynaset"
            Object.ToolTipText     =   "Dynaset type Recordset"
            ImageIndex      =   2
            Style           =   2
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Snapshot"
            Object.ToolTipText     =   "Snapshot type Recordset"
            ImageIndex      =   3
            Style           =   2
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Object.Visible         =   0   'False
            Key             =   "PassThrough"
            Object.ToolTipText     =   "Passthrough type Recordset"
            ImageIndex      =   4
            Style           =   2
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "DataControl"
            Object.ToolTipText     =   "Use Data Control on New Form"
            ImageIndex      =   5
            Style           =   2
         EndProperty
         BeginProperty Button7 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "NoDataControl"
            Object.ToolTipText     =   "Don't Use Data Control on New Form"
            ImageIndex      =   6
            Style           =   2
         EndProperty
         BeginProperty Button8 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "DBGrid"
            Object.ToolTipText     =   "Use DBGrid Control on New Form"
            ImageIndex      =   7
            Style           =   2
         EndProperty
         BeginProperty Button9 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
            MixedState      =   -1  'True
         EndProperty
         BeginProperty Button10 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Key             =   "BeginTrans"
            Object.ToolTipText     =   "Begin a Transaction"
            ImageIndex      =   8
         EndProperty
         BeginProperty Button11 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Key             =   "Rollback"
            Object.ToolTipText     =   "Rollback current Transaction"
            ImageIndex      =   9
         EndProperty
         BeginProperty Button12 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Key             =   "Commit"
            Object.ToolTipText     =   "Commit current Transaction"
            ImageIndex      =   10
         EndProperty
      EndProperty
   End
   Begin VB.PictureBox Picture1 
      Align           =   1  'Align Top
      BorderStyle     =   0  'None
      Height          =   15
      Left            =   0
      ScaleHeight     =   15
      ScaleWidth      =   10005
      TabIndex        =   2
      Top             =   360
      Width           =   10005
   End
   Begin MSComDlg.CommonDialog dlgCMD1 
      Left            =   -15
      Top             =   690
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
      FilterIndex     =   1144
      FontSize        =   1.74012e-39
   End
   Begin MSComCtlLib.StatusBar stsStatusBar 
      Align           =   2  'Align Bottom
      Height          =   300
      Left            =   0
      TabIndex        =   0
      Top             =   6480
      Width           =   10005
      _ExtentX        =   17648
      _ExtentY        =   529
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   2
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   14658
            Text            =   "Ready"
            TextSave        =   "Ready"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   2
         EndProperty
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Tahoma"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin MSComCtlLib.ImageList imlToolbarPics 
      Left            =   495
      Top             =   705
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483634
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   10
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":014A
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":025C
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":036E
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":0480
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":0592
            Key             =   ""
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":06A4
            Key             =   ""
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":07B6
            Key             =   ""
         EndProperty
         BeginProperty ListImage8 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":08C8
            Key             =   ""
         EndProperty
         BeginProperty ListImage9 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":09DA
            Key             =   ""
         EndProperty
         BeginProperty ListImage10 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VDMDI.frx":0AEC
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuDatabase 
      Caption         =   "&File"
      HelpContextID   =   2096095
      Begin VB.Menu mnuDBOpen 
         Caption         =   "&Open DataBase..."
         HelpContextID   =   2016062
         Begin VB.Menu mnuDBOMDB 
            Caption         =   "&Microsoft Access..."
         End
         Begin VB.Menu mnuDBOdBASE 
            Caption         =   "&Dbase"
            Begin VB.Menu mnuDBOdBASE5 
               Caption         =   "&5.0..."
            End
            Begin VB.Menu mnuDBOdBASE4 
               Caption         =   "I&V..."
            End
            Begin VB.Menu mnuDBOdBASE3 
               Caption         =   "&III..."
            End
         End
         Begin VB.Menu mnuDBOFoxPro 
            Caption         =   "&FoxPro"
            Begin VB.Menu mnuDBOFox30 
               Caption         =   "&3.0..."
            End
            Begin VB.Menu mnuDBOFox26 
               Caption         =   "2.&6..."
            End
            Begin VB.Menu mnuDBOFox25 
               Caption         =   "2.&5..."
            End
            Begin VB.Menu mnuDBOFox20 
               Caption         =   "2.&0..."
            End
         End
         Begin VB.Menu mnuDBOParadox 
            Caption         =   "&Paradox"
            Begin VB.Menu mnuDBOParadox5 
               Caption         =   "&5.0..."
            End
            Begin VB.Menu mnuDBOParadox4 
               Caption         =   "&4.X..."
            End
            Begin VB.Menu mnuDBOParadox3 
               Caption         =   "&3.X..."
            End
         End
         Begin VB.Menu mnuDBOBtrieve 
            Caption         =   "&Btrieve..."
         End
         Begin VB.Menu mnuDBOExcel 
            Caption         =   "&Excel..."
         End
         Begin VB.Menu mnuDBOText 
            Caption         =   "&Text Files..."
         End
         Begin VB.Menu mnuDBOODBC 
            Caption         =   "&ODBC..."
            HelpContextID   =   2016138
         End
      End
      Begin VB.Menu mnuDBNew 
         Caption         =   "&New..."
         HelpContextID   =   2016083
         Begin VB.Menu mnuDBNMDB 
            Caption         =   "&Microsoft Access"
            Begin VB.Menu mnuDBNMDB2x 
               Caption         =   "Version &2.0 MDB..."
            End
            Begin VB.Menu mnuDBNMDB70 
               Caption         =   "Version &7.0 MDB..."
            End
         End
         Begin VB.Menu mnuDBNdBASE 
            Caption         =   "&Dbase"
            Begin VB.Menu mnuDBNdBASE5 
               Caption         =   "&5.0..."
            End
            Begin VB.Menu mnuDBNdBASE4 
               Caption         =   "I&V..."
            End
            Begin VB.Menu mnuDBNdBASE3 
               Caption         =   "&III..."
            End
         End
         Begin VB.Menu mnuDBNFoxPro 
            Caption         =   "&FoxPro"
            Begin VB.Menu mnuDBNFox30 
               Caption         =   "&3.0..."
            End
            Begin VB.Menu mnuDBNFox26 
               Caption         =   "2.&6..."
            End
            Begin VB.Menu mnuDBNFox25 
               Caption         =   "2.&5..."
            End
            Begin VB.Menu mnuDBNFox20 
               Caption         =   "2.&0..."
            End
         End
         Begin VB.Menu mnuDBNParadox 
            Caption         =   "&Paradox"
            Begin VB.Menu mnuDBNParadox5 
               Caption         =   "&5.0..."
            End
            Begin VB.Menu mnuDBNParadox4 
               Caption         =   "&4.X..."
            End
            Begin VB.Menu mnuDBNParadox3 
               Caption         =   "&3.X..."
            End
         End
         Begin VB.Menu mnuDBNBtrieve 
            Caption         =   "&Btrieve..."
         End
         Begin VB.Menu mnuDBNODBC 
            Caption         =   "&ODBC..."
         End
         Begin VB.Menu mnuDBNText 
            Caption         =   "&Text Files..."
         End
      End
      Begin VB.Menu mnuDBClose 
         Caption         =   "&Close"
         Enabled         =   0   'False
         HelpContextID   =   2016079
      End
      Begin VB.Menu mnuBar0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDBImpExp 
         Caption         =   "&Import/Export..."
         Enabled         =   0   'False
         HelpContextID   =   2016092
      End
      Begin VB.Menu mnuDBWorkspace 
         Caption         =   "&Workspace..."
         HelpContextID   =   2016080
      End
      Begin VB.Menu mnuDBErrors 
         Caption         =   "&Errors..."
         HelpContextID   =   2016081
      End
      Begin VB.Menu mnuBar1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDBCompact 
         Caption         =   "Co&mpact MDB..."
         HelpContextID   =   2016084
         Begin VB.Menu mnuDBC70MDB 
            Caption         =   "&7.0 MDB..."
            HelpContextID   =   2016084
         End
         Begin VB.Menu mnuDBC20MDB 
            Caption         =   "&2.0 MDB..."
            HelpContextID   =   2016084
         End
      End
      Begin VB.Menu mnuDBRepair 
         Caption         =   "&Repair MDB..."
         HelpContextID   =   2016085
      End
      Begin VB.Menu mnuBar2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&1"
         HelpContextID   =   2016095
         Index           =   1
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&2"
         HelpContextID   =   2016095
         Index           =   2
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&3"
         HelpContextID   =   2016095
         Index           =   3
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&4"
         HelpContextID   =   2016095
         Index           =   4
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&5"
         HelpContextID   =   2016095
         Index           =   5
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&6"
         HelpContextID   =   2016095
         Index           =   6
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&7"
         HelpContextID   =   2016095
         Index           =   7
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBMRU 
         Caption         =   "&8"
         HelpContextID   =   2016095
         Index           =   8
         Visible         =   0   'False
      End
      Begin VB.Menu mnuBarMRU 
         Caption         =   "-"
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBExit 
         Caption         =   "E&xit"
         HelpContextID   =   2016095
      End
   End
   Begin VB.Menu mnuUtil 
      Caption         =   "&Utility"
      Enabled         =   0   'False
      HelpContextID   =   2096097
      Begin VB.Menu mnuUQuery 
         Caption         =   "&Query Builder..."
         HelpContextID   =   2016115
      End
      Begin VB.Menu mnuUDataFormDesigner 
         Caption         =   "Data &Form Designer..."
         HelpContextID   =   2098108
         Visible         =   0   'False
      End
      Begin VB.Menu mnuUReplace 
         Caption         =   "&Global Replace..."
         HelpContextID   =   2016091
      End
      Begin VB.Menu mnuUBar1 
         Caption         =   "-"
         Visible         =   0   'False
      End
      Begin VB.Menu mnuUAttachments 
         Caption         =   "&Attachments.."
         HelpContextID   =   2016086
         Visible         =   0   'False
      End
      Begin VB.Menu mnuUGroupsUsers 
         Caption         =   "&Groups/Users..."
         HelpContextID   =   2016088
         Visible         =   0   'False
      End
      Begin VB.Menu mnuUSystemDB 
         Caption         =   "&SYSTEM.MD?..."
         HelpContextID   =   2016090
         Visible         =   0   'False
      End
      Begin VB.Menu mnuUBar2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuPref 
         Caption         =   "&Preferences"
         HelpContextID   =   2093354
         Begin VB.Menu mnuPOpenOnStartup 
            Caption         =   "&Open Last DataBase on Startup"
         End
         Begin VB.Menu mnuPAllowSys 
            Caption         =   "&Include System Tables"
         End
         Begin VB.Menu mnuBar4 
            Caption         =   "-"
         End
         Begin VB.Menu mnuPQueryTimeout 
            Caption         =   "&Query Timeout Value..."
         End
         Begin VB.Menu mnuPLoginTimeout 
            Caption         =   "&Login Timeout Value..."
         End
      End
   End
   Begin VB.Menu mnuWindow 
      Caption         =   "&Window"
      HelpContextID   =   2016100
      WindowList      =   -1  'True
      Begin VB.Menu mnuWTile 
         Caption         =   "&Tile"
      End
      Begin VB.Menu mnuWCascade 
         Caption         =   "&Cascade"
      End
      Begin VB.Menu mnuWArrange 
         Caption         =   "&Arrange Icons"
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      HelpContextID   =   2093307
      Begin VB.Menu mnuHTopics 
         Caption         =   "&Topics..."
      End
      Begin VB.Menu mnuBar7 
         Caption         =   "-"
      End
      Begin VB.Menu mnuHAbout 
         Caption         =   "&About..."
      End
   End
   Begin VB.Menu mnuDBPopUp 
      Caption         =   ""
      Visible         =   0   'False
      Begin VB.Menu mnuDBPUOpen 
         Caption         =   "&Open"
      End
      Begin VB.Menu mnuDBPUDesign 
         Caption         =   "&Design..."
      End
      Begin VB.Menu mnuDBPUEdit 
         Caption         =   "&Edit"
      End
      Begin VB.Menu mnuDBPURename 
         Caption         =   "&Rename"
      End
      Begin VB.Menu mnuDBPUDelete 
         Caption         =   "De&lete"
      End
      Begin VB.Menu mnuDBPUBar1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDBPUCopyStruct 
         Caption         =   "Copy Structure..."
      End
      Begin VB.Menu mnuDBPURemoveAll 
         Caption         =   "Remove All Records"
         Visible         =   0   'False
      End
      Begin VB.Menu mnuDBPURefresh 
         Caption         =   "Refresh List"
      End
      Begin VB.Menu mnuDBPUBar2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuDBPUNewTable 
         Caption         =   "New &Table"
      End
      Begin VB.Menu mnuDBPUNewQuery 
         Caption         =   "New &Query"
      End
   End
End
Attribute VB_Name = "frmMDI"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Option Compare Binary
'>>>>>>>>>>>>>>>>>>>>>>>>
'menus
Const MNU_Database = "&File"
Const MNU_DBOpen = "&Open DataBase..."
Const MNU_DBOMDB = "&Microsoft Access..."
Const MNU_DBOText = "&Text Files..."
Const MNU_DBOODBC = "&ODBC..."
Const MNU_DBNew = "&New..."
Const MNU_DBNMDB = "&Microsoft Access"
Const MNU_DBNMDB2x = "Version &2.0 MDB..."
Const MNU_DBNMDB70 = "Version &7.0 MDB..."
Const MNU_DBNODBC = "&ODBC..."
Const MNU_DBNText = "&Text Files..."
Const MNU_DBClose = "&Close"
Const MNU_DBImpExp = "&Import/Export..."
Const MNU_DBWorkspace = "&Workspace..."
Const MNU_DBErrors = "&Errors..."
Const MNU_DBCompact = "Co&mpact MDB..."
Const MNU_DBRepair = "&Repair MDB..."
Const MNU_DBExit = "E&xit"
Const MNU_Util = "&Utility"
Const MNU_UQuery = "&Query Builder..."
Const MNU_UDataFormDesigner = "Data &Form Designer..."
Const MNU_UReplace = "&Global Replace..."
Const MNU_UAttachments = "&Attachments.."
Const MNU_UGroupsUsers = "&Groups/Users..."
Const MNU_USystemDB = "&SYSTEM.MD?..."
Const MNU_Pref = "&Preferences"
Const MNU_POpenOnStartup = "&Open Last DataBase on Startup"
Const MNU_PAllowSys = "&Include System Tables"
Const MNU_PQueryTimeout = "&Query Timeout Value..."
Const MNU_PLoginTimeout = "&Login Timeout Value..."
Const MNU_Window = "&Window"
Const MNU_WTile = "&Tile"
Const MNU_WCascade = "&Cascade"
Const MNU_WArrange = "&Arrange Icons"
Const MNU_Help = "&Help"
Const MNU_HTopics = "&Topics..."
Const MNU_HAbout = "&About..."
Const MNU_DBPUOpen = "&Open"
Const MNU_DBPUDesign = "&Design..."
Const MNU_DBPUEdit = "&Edit"
Const MNU_DBPURename = "&Rename"
Const MNU_DBPUDelete = "De&lete"
Const MNU_DBPUCopyStruct = "Copy Structure..."
Const MNU_DBPURemoveAll = "Remove All Records"
Const MNU_DBPURefresh = "Refresh List"
Const MNU_DBPUNewTable = "New &Table"
Const MNU_DBPUNewQuery = "New &Query"

'tooltips
Const TOOLTIP1 = "Table type Recordset"
Const TOOLTIP2 = "Dynaset type Recordset"
Const TOOLTIP3 = "Snapshot type Recordset"
Const TOOLTIP4 = "Passthrough type Recordset"
Const TOOLTIP5 = "Use Data Control on New Form"
Const TOOLTIP6 = "Don't Use Data Control on New Form"
Const TOOLTIP7 = "Use DBGrid Control on New Form"
Const TOOLTIP8 = "Begin a Transaction"
Const TOOLTIP9 = "Rollback current Transaction"
Const TOOLTIP10 = "Commit current Transaction"

'misc strings
Const MSG3 = "Press any key to Close About Box"
Const MSG4 = "Enter New Database Parameters"
Const MSG5 = "Enter Driver Name:"
Const MSG6 = "Driver Name"
Const MSG7 = "You must Close First!"
Const MSG8 = "NOTE: Use of Attached Tables is the Recommended Method"
Const MSG9 = "Microsoft Access MDBs (*.mdb)|*.mdb|All Files (*.*)|*.*"
Const MSG10 = "Open Microsoft Access Database to Repair"
Const MSG11 = "Repairing "
Const MSG12 = "Open Repaired Database?"
Const MSG13 = "System Databases|SYSTEM.MD?"
Const MSG14 = "Select SYSTEM.MD? (Microsoft Access Security File)"
Const MSG15 = " User: "
Const MSG16 = "Current Database must be closed due to the error!"
Const MSG17 = "No Users found, try 'Utility/System MD?'!"
Const MSG18 = "Login Timeout (in seconds):"
Const MSG19 = "No Database Open"
Const MSG20 = "Query Timeout (in seconds):"
Const MSG21 = "Delete Table?"
Const MSG22 = "Delete QueryDef?"
Const MSG23 = "Delete Field?"
Const MSG24 = "Delete Index?"
Const MSG25 = "Delete All Records in Table?"
Const MSG26 = "Rows deleted: "
Const MSG27 = "SYSTEM.MD? Not found, Add one to VB Settings?"
Const MSG28 = "Transactions not supported by this Driver!"
Const MSG29 = "All changes will be gone, Rollback anyway?"
Const MSG30 = " Property is Read Only!"
Const MSG31 = "This function requires an active project!"
'>>>>>>>>>>>>>>>>>>>>>>>>

Dim mHwnd As Long

Private Sub mnuDBC70MDB_Click()
  CompactDB dbVersion30
End Sub

Private Sub mnuDBNMDB2x_Click()
  NewMDB dbVersion20
End Sub

Private Sub mnuDBNMDB70_Click()
  NewMDB dbVersion30
End Sub

Private Sub mnuDBOExcel_Click()
  'we can use Excel 5.0 for all Excel files because
  'the ISAM will figure out the version when
  'it opens file
  gsDataType = gsEXCEL50
  OpenLocalDB False
End Sub

Private Sub mnuDBPUDesign_Click()
  On Error Resume Next

  If gnodDBNode2 Is Nothing Then Exit Sub
  If gnodDBNode2.Tag = TABLE_STR Then
    gbAddTableFlag = False
    Screen.MousePointer = vbHourglass
    frmTblStruct.Show vbModal
  ElseIf gnodDBNode2.Tag = QUERY_STR Then
    Set frmDatabase.tvDatabase.SelectedItem = gnodDBNode2
    frmSQL.txtSQLStatement.Text = gdbCurrentDB.QueryDefs(gnodDBNode2.Text).SQL
  End If

End Sub

Sub mnuDBPUEdit_Click()
  On Error GoTo mnuDBPUEdit_ClickErr
  
  Dim prpObj As Property
  Dim vTmp As Variant
  Dim vNew As Variant
  Dim frmProp As New frmProperty
  
  If gnodDBNode2.Parent.Parent Is Nothing Then
    'must be a database property
    Set prpObj = gdbCurrentDB.Properties(VBA.Left(gnodDBNode2.Text, InStr(gnodDBNode2.Text, "=") - 1))
  Else
    Select Case gnodDBNode2.Parent.Parent.Tag
      Case TABLE_STR
        Set prpObj = gdbCurrentDB.TableDefs(gnodDBNode2.Parent.Parent.Text).Properties(VBA.Left(gnodDBNode2.Text, InStr(gnodDBNode2.Text, "=") - 1))
      Case QUERY_STR
        Set prpObj = gdbCurrentDB.QueryDefs(gnodDBNode2.Parent.Parent.Text).Properties(VBA.Left(gnodDBNode2.Text, InStr(gnodDBNode2.Text, "=") - 1))
      Case FIELDS_STR
        Set prpObj = gdbCurrentDB.TableDefs(gnodDBNode2.Parent.Parent.Parent.Text).Fields(gnodDBNode2.Parent.Text).Properties(VBA.Left(gnodDBNode2.Text, InStr(gnodDBNode2.Text, "=") - 1))
      Case INDEXES_STR
        Set prpObj = gdbCurrentDB.TableDefs(gnodDBNode2.Parent.Parent.Parent.Text).Indexes(gnodDBNode2.Parent.Text).Properties(VBA.Left(gnodDBNode2.Text, InStr(gnodDBNode2.Text, "=") - 1))
      Case Else
        Exit Sub
    End Select
  End If
  
  'store the value
  vTmp = prpObj.Value
  
  On Error Resume Next
  'try to set it to it's current value
  'to see if it is readonly
  prpObj.Value = vTmp
  If Err Then
    'readonly so just exit
    Err.Clear
    MsgBox "'" & prpObj.Name & "'" & MSG30, vbExclamation
    Exit Sub
  End If
  On Error GoTo mnuDBPUEdit_ClickErr
  
  With frmProp
    Set .PropObject = prpObj
    .Show vbModal
    If .OK Then
      gnodDBNode2.Text = prpObj.Name & "=" & prpObj.Value
      'see if it was a Name property
      If prpObj.Name = "Name" Then
        gnodDBNode2.Parent.Text = prpObj.Value
      End If
    End If
    Unload frmProp
  End With
  Set frmProp = Nothing
  
  Exit Sub
mnuDBPUEdit_ClickErr:
  ShowError
End Sub

Private Sub mnuDBPUNewQuery_Click()
  'load the query form to help build a query
  Unload frmQuery      'just to clear things out
  frmQuery.Show
End Sub

Private Sub mnuDBPUNewTable_Click()
  gbAddTableFlag = True
  Screen.MousePointer = vbHourglass
  frmTblStruct.Show vbModal
End Sub

Sub mnuDBPUOpen_Click()
  On Error Resume Next
  
  gbFromSQL = False
  Screen.MousePointer = vbHourglass
  
  If gnodDBNode.Tag = TABLE_STR Then
    OpenTable StripConnect(gnodDBNode2.Text)
  ElseIf gnodDBNode.Tag = QUERY_STR Then
    OpenQuery gnodDBNode2.Text, False
  End If
  
  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False

End Sub

Private Sub mnuHAbout_Click()
  MsgBar MSG3, False
  frmAboutBox.Show vbModal
  MsgBar vbNullString, False
End Sub

Private Sub mnuDBC20MDB_Click()
  CompactDB dbVersion20
End Sub


Private Sub mnuDBClose_Click()
  CloseCurrentDB
End Sub

Private Sub mnuDBErrors_Click()
  On Error Resume Next
  Screen.MousePointer = vbHourglass
  RefreshErrors
  Screen.MousePointer = vbDefault
  If Err Then ShowError
End Sub

Private Sub mnuDBExit_Click()
  Unload Me
End Sub

Private Sub mnuDBNBtrieve_Click()
   gsDataType = gsBTRIEVE
   NewLocalISAM
End Sub

Private Sub mnuDBNDbase3_Click()
   gsDataType = gsDBASEIII
   NewLocalISAM
End Sub

Private Sub mnuDBNDbase4_Click()
   gsDataType = gsDBASEIV
   NewLocalISAM
End Sub

Private Sub mnuDBNDbase5_Click()
   gsDataType = gsDBASE5
   NewLocalISAM
End Sub

Private Sub mnuDBNFox20_Click()
   gsDataType = gsFOXPRO20
   NewLocalISAM
End Sub

Private Sub mnuDBNFox25_Click()
   gsDataType = gsFOXPRO25
   NewLocalISAM
End Sub

Private Sub mnuDBNFox26_Click()
   gsDataType = gsFOXPRO26
   NewLocalISAM
End Sub

Private Sub mnuDBNFox30_Click()
   gsDataType = gsFOXPRO30
   NewLocalISAM
End Sub

Private Sub mnuDBNODBC_Click()
  On Error GoTo DBNErr

  Dim sDriverName As String

  MsgBar MSG4, False

  'driver must be an valid entry in ODBCINST.Registry
  sDriverName = InputBox(MSG5, MSG6, gsDEFAULT_DRIVER)

  If Len(sDriverName) = 0 Then Exit Sub 'they cancelled
  
  DBEngine.RegisterDatabase vbNullString, sDriverName, False, vbNullString

  SendKeys "%FOO"   'force open database dialog

  MsgBar vbNullString, False
  Exit Sub

DBNErr:
  ShowError
End Sub

Private Sub mnuDBNParadox3_Click()
  gsDataType = gsPARADOX3X
  NewLocalISAM
End Sub

Private Sub mnuDBNParadox4_Click()
  gsDataType = gsPARADOX4X
  NewLocalISAM
End Sub

Private Sub mnuDBNParadox5_Click()
  gsDataType = gsPARADOX5X
  NewLocalISAM
End Sub

Private Sub mnuDBNText_Click()
  gsDataType = gsTEXTFILES
  NewLocalISAM
End Sub

Private Sub mnuDBOMDB_Click()
  gsDataType = gsMSACCESS
  OpenLocalDB False
End Sub

Private Sub mnuDBOBtrieve_Click()
  gsDataType = gsBTRIEVE
  OpenLocalDB False
End Sub

Private Sub mnuDBODbase3_Click()
   gsDataType = gsDBASEIII
   OpenLocalDB False
End Sub

Private Sub mnuDBODbase4_Click()
   gsDataType = gsDBASEIV
   OpenLocalDB False
End Sub

Private Sub mnuDBODbase5_Click()
   gsDataType = gsDBASE5
   OpenLocalDB False
End Sub

Private Sub mnuDBOFox20_Click()
   gsDataType = gsFOXPRO20
   OpenLocalDB False
End Sub

Private Sub mnuDBOFox25_Click()
   gsDataType = gsFOXPRO25
   OpenLocalDB False
End Sub

Private Sub mnuDBOFox26_Click()
   gsDataType = gsFOXPRO26
   OpenLocalDB False
End Sub

Private Sub mnuDBOFox30_Click()
   gsDataType = gsFOXPRO30
   OpenLocalDB False
End Sub

Private Sub mnuDBOODBC_Click()
  Dim frm As New frmODBCLogon

  frm.Show vbModal
  
  If frm.DBOpened Then
    ShowDBTools
    RefreshTables Nothing
    MsgBar MSG8, False
  End If
  
  Unload frm
  Set frm = Nothing
  
End Sub

Private Sub mnuDBOParadox3_Click()
   gsDataType = gsPARADOX3X
   OpenLocalDB False
End Sub

Private Sub mnuDBOParadox4_Click()
   gsDataType = gsPARADOX4X
   OpenLocalDB False
End Sub

Private Sub mnuDBOParadox5_Click()
   gsDataType = gsPARADOX5X
   OpenLocalDB False
End Sub

Private Sub mnuDBOText_Click()
   gsDataType = gsTEXTFILES
   OpenLocalDB False
End Sub

Private Sub mnuDBRepair_Click()
  On Error GoTo RepairAccErr

  Dim sNewName As String

  'get file name to repair
  With dlgCMD1
    .Filter = MSG9
    .DialogTitle = MSG10
    .FilterIndex = 1
    .Flags = FileOpenConstants.cdlOFNHideReadOnly
    .ShowOpen
  End With
  If Len(dlgCMD1.FileName) > 0 Then
    sNewName = dlgCMD1.FileName
  Else
    Exit Sub
  End If

  Screen.MousePointer = vbHourglass
  MsgBar MSG11 & sNewName, True
  DBEngine.RepairDatabase sNewName
  Screen.MousePointer = vbDefault
  MsgBar vbNullString, False

  If MsgBox(MSG12, vbYesNo + vbQuestion) = vbYes Then
    If gbDBOpenFlag Then
      Call mnuDBClose_Click
    End If
    gsDataType = gsMSACCESS
    gsDBName = sNewName
    OpenLocalDB True
  End If

  If gbDBOpenFlag Then
    ShowDBTools
    RefreshTables Nothing
  End If

  Exit Sub

RepairAccErr:
  If Err <> 32755 Then
    ShowError
  End If
End Sub

Private Sub mnuHTopics_Click()
  DisplayTopic 2016111
End Sub



Private Sub mnuUSystemDB_Click()
  On Error Resume Next
  
  Dim sTmp As String
  Dim x As Integer
  
  With dlgCMD1
    .Filter = MSG13
    .DialogTitle = MSG14
    .FilterIndex = 1
    .FileName = "SYSTEM.MDW"
    .CancelError = True
    .Flags = FileOpenConstants.cdlOFNHideReadOnly + FileOpenConstants.cdlOFNFileMustExist
  End With
  On Error Resume Next
  dlgCMD1.ShowOpen
  If Err = 32755 Then         'user cancelled
    Exit Sub
  Else
    sTmp = dlgCMD1.FileName  'must be a good filename
    SaveSetting APP_CATEGORY & "\VisData", "Engines", "SystemDB", sTmp
  End If

End Sub

Private Sub mnuDBWorkspace_Click()
  On Error GoTo WSErr
    
  Dim sDBName As String
  Dim sConnect As String
  Dim sUser As String
    
  If gbDBOpenFlag Then
    'save the old settings
    sDBName = gdbCurrentDB.Name
    sConnect = gdbCurrentDB.Connect
    sUser = gwsMainWS.UserName
  End If
  
  frmLogin.Show vbModal
  stsStatusBar.Panels(2).Text = MSG15 & gwsMainWS.UserName & " "
  
  'reopen the database if the user changed
  If UCase(sUser) <> UCase(gwsMainWS.UserName) And gbDBOpenFlag Then
    'have to close objects that will be invalid after reopening the DB
    CloseAllRecordsets
    Set gdbCurrentDB = gwsMainWS.OpenDatabase(sDBName, False, gnReadOnly, sConnect)
  End If
  
  Exit Sub
  
WSErr:
  ShowError
  If gbDBOpenFlag Then
    MsgBox MSG16, 48
  End If
  Call mnuDBClose_Click
End Sub

Private Sub mnuUAttachments_Click()
  On Error Resume Next
  Screen.MousePointer = vbHourglass
  frmAttachments.Show
  Screen.MousePointer = vbDefault
  If Err Then ShowError
End Sub

Private Sub mnuUGroupsUsers_Click()
  On Error Resume Next
    
  If gwsMainWS.Users.Count = 0 Then
    Beep
    MsgBox MSG17, 48
    Exit Sub
  End If
    
  Screen.MousePointer = vbHourglass
  frmGroupsUsers.Show
  Screen.MousePointer = vbDefault
  If Err Then ShowError
End Sub

Private Sub mnuPAllowSys_Click()
  On Error Resume Next

  mnuPAllowSys.Checked = Not mnuPAllowSys.Checked
  If Not gbDBOpenFlag Then Exit Sub
  
  RefreshTables Nothing
  
End Sub

Private Sub mnuPLoginTimeout_Click()
  On Error GoTo LTErr

  Dim sNewValue As String

  sNewValue = InputBox(MSG18, , CStr(glLoginTimeout))
  If Len(sNewValue) = 0 Then Exit Sub

  'try to set the new value
  If Val(sNewValue) >= 0 Then
    glLoginTimeout = Val(sNewValue)
    DBEngine.LoginTimeout = glLoginTimeout
  End If

  Exit Sub

LTErr:
  ShowError
End Sub

Private Sub mnuPOpenOnStartup_Click()
  mnuPOpenOnStartup.Checked = Not mnuPOpenOnStartup.Checked
End Sub

Private Sub mnuPQueryTimeout_Click()
  On Error GoTo QTErr

  Dim sNewValue As String

  If Not gbDBOpenFlag Then MsgBox MSG19, 48: Exit Sub
  
  sNewValue = InputBox(MSG20, , CStr(gdbCurrentDB.QueryTimeout))
  If Len(sNewValue) = 0 Then Exit Sub

  'try to set the new value
  gdbCurrentDB.QueryTimeout = Val(sNewValue)
  glQueryTimeout = Val(sNewValue)

  Exit Sub

QTErr:
  ShowError
  'reset the form control after the error
  glQueryTimeout = gdbCurrentDB.QueryTimeout
End Sub

Private Sub mnuUDataFormDesigner_Click()
  On Error Resume Next
  
  'make sure a project is loaded
  If gVDClass.VBInstance.ActiveVBProject Is Nothing Then
    MsgBox MSG31, vbInformation
    Exit Sub
  End If
  
  frmDFD.Show vbModal
  If Err Then ShowError
End Sub

Private Sub mnuUQuery_Click()
  frmQuery.Show
  frmQuery.WindowState = 0
End Sub

Private Sub mnuDBPUCopyStruct_Click()
  On Error Resume Next
  frmCopyStruct.Show vbModal
  If Err Then ShowError
End Sub

Private Sub mnuDBPUDelete_Click()
  On Error GoTo TblDelErr

  Dim sName As String
  If gnodDBNode2 Is Nothing Then Exit Sub
  
  Select Case gnodDBNode2.Tag
    Case TABLE_STR
      sName = StripConnect(gnodDBNode2.Text)
      If MsgBox(MSG21, vbYesNo + vbQuestion) = vbYes Then
        gdbCurrentDB.TableDefs.Delete sName
        frmDatabase.tvDatabase.Nodes.Remove gnodDBNode2.Index
      End If
    Case QUERY_STR
      sName = gnodDBNode2.Text
      If MsgBox(MSG22, vbYesNo + vbQuestion) = vbYes Then
        gdbCurrentDB.QueryDefs.Delete sName
        frmDatabase.tvDatabase.Nodes.Remove gnodDBNode2.Index
      End If
    Case FIELD_STR
      sName = gnodDBNode2.Text
      If MsgBox(MSG23, vbYesNo + vbQuestion) = vbYes Then
        gdbCurrentDB.TableDefs(gnodDBNode2.Parent.Parent.Text).Fields.Delete sName
        frmDatabase.tvDatabase.Nodes.Remove gnodDBNode2.Index
      End If
    Case INDEX_STR
      sName = gnodDBNode2.Text
      If MsgBox(MSG24, vbYesNo + vbQuestion) = vbYes Then
        gdbCurrentDB.TableDefs(gnodDBNode2.Parent.Parent.Text).Indexes.Delete sName
        frmDatabase.tvDatabase.Nodes.Remove gnodDBNode2.Index
      End If
  End Select

  Exit Sub

TblDelErr:
  ShowError
End Sub

Private Sub mnuDBPURefresh_Click()
  gdbCurrentDB.TableDefs.Refresh
  RefreshTables Nothing
End Sub

Private Sub mnuDBPURename_Click()
  On Error GoTo mnuDBPURename_ClickErr
  
  If Not gnodDBNode2 Is Nothing Then
    'set it to the new node for editing
    Set frmDatabase.tvDatabase.SelectedItem = gnodDBNode2
    frmDatabase.tvDatabase.StartLabelEdit
  End If

  Exit Sub

mnuDBPURename_ClickErr:
  ShowError
End Sub

Private Sub mnuDBPURemoveAll_Click()
  On Error GoTo RemoveAllErr

  Dim sTBLName As String
  
  sTBLName = StripConnect(gnodDBNode.Text)

  If MsgBox(MSG25 & " '" & sTBLName & "'", vbYesNo + vbQuestion) = vbYes Then
    'delete all rows with a sql statement
    If gsDataType = gsSQLDB Then
      gdbCurrentDB.Execute ("delete from " & sTBLName), dbSQLPassThrough
    Else
      gdbCurrentDB.Execute ("delete from " & sTBLName)
    End If
    If gdbCurrentDB.RecordsAffected > 0 Then
      MsgBox MSG26 & gdbCurrentDB.RecordsAffected, 48
      If gbTransPending Then gbDBChanged = True
    End If

  End If

  Exit Sub

RemoveAllErr:
  If Err = gnEOF_ERR Then Resume Next
  ShowError
End Sub

Private Sub mnuDBImpExp_Click()
  On Error Resume Next
  frmImpExp.Show vbModal
  If Err Then ShowError
End Sub

Private Sub mnuUReplace_Click()
  On Error GoTo ReplaceErr

  frmReplace.Show vbModal

  Exit Sub

ReplaceErr:
  ShowError
End Sub

Private Sub mnuWArrange_Click()
  Me.Arrange 3
End Sub

Private Sub mnuWCascade_Click()
  Me.Arrange 0
End Sub

Private Sub mnuWTile_Click()
  Me.Arrange 2
End Sub

Private Sub MDIForm_Load()
  On Error GoTo MDILErr

  Dim x As Integer

  'load strings from constants
  'menus
  mnuDatabase.Caption = MNU_Database
  mnuDBOpen.Caption = MNU_DBOpen
  mnuDBOMDB.Caption = MNU_DBOMDB
  mnuDBOText.Caption = MNU_DBOText
  mnuDBOODBC.Caption = MNU_DBOODBC
  mnuDBNew.Caption = MNU_DBNew
  mnuDBNMDB.Caption = MNU_DBNMDB
  mnuDBNMDB2x.Caption = MNU_DBNMDB2x
  mnuDBNMDB70.Caption = MNU_DBNMDB70
  mnuDBNODBC.Caption = MNU_DBNODBC
  mnuDBNText.Caption = MNU_DBNText
  mnuDBClose.Caption = MNU_DBClose
  mnuDBImpExp.Caption = MNU_DBImpExp
  mnuDBWorkspace.Caption = MNU_DBWorkspace
  mnuDBErrors.Caption = MNU_DBErrors
  mnuDBCompact.Caption = MNU_DBCompact
  mnuDBRepair.Caption = MNU_DBRepair
  mnuDBExit.Caption = MNU_DBExit
  mnuUtil.Caption = MNU_Util
  mnuUQuery.Caption = MNU_UQuery
  mnuUDataFormDesigner.Caption = MNU_UDataFormDesigner
  mnuUReplace.Caption = MNU_UReplace
  mnuUAttachments.Caption = MNU_UAttachments
  mnuUGroupsUsers.Caption = MNU_UGroupsUsers
  mnuUSystemDB.Caption = MNU_USystemDB
  mnuPref.Caption = MNU_Pref
  mnuPOpenOnStartup.Caption = MNU_POpenOnStartup
  mnuPAllowSys.Caption = MNU_PAllowSys
  mnuPQueryTimeout.Caption = MNU_PQueryTimeout
  mnuPLoginTimeout.Caption = MNU_PLoginTimeout
  mnuWindow.Caption = MNU_Window
  mnuWTile.Caption = MNU_WTile
  mnuWCascade.Caption = MNU_WCascade
  mnuWArrange.Caption = MNU_WArrange
  mnuHelp.Caption = MNU_Help
  mnuHTopics.Caption = MNU_HTopics
  mnuHAbout.Caption = MNU_HAbout
  mnuDBPUOpen.Caption = MNU_DBPUOpen
  mnuDBPUDesign.Caption = MNU_DBPUDesign
  mnuDBPUEdit.Caption = MNU_DBPUEdit
  mnuDBPURename.Caption = MNU_DBPURename
  mnuDBPUDelete.Caption = MNU_DBPUDelete
  mnuDBPUCopyStruct.Caption = MNU_DBPUCopyStruct
  mnuDBPURemoveAll.Caption = MNU_DBPURemoveAll
  mnuDBPURefresh.Caption = MNU_DBPURefresh
  mnuDBPUNewTable.Caption = MNU_DBPUNewTable
  mnuDBPUNewQuery.Caption = MNU_DBPUNewQuery
  
  'tooltips
  tlbToolBar.Buttons(1).ToolTipText = TOOLTIP1
  tlbToolBar.Buttons(2).ToolTipText = TOOLTIP2
  tlbToolBar.Buttons(3).ToolTipText = TOOLTIP3
  tlbToolBar.Buttons(4).ToolTipText = TOOLTIP4
  tlbToolBar.Buttons(6).ToolTipText = TOOLTIP5
  tlbToolBar.Buttons(7).ToolTipText = TOOLTIP6
  tlbToolBar.Buttons(8).ToolTipText = TOOLTIP7
  tlbToolBar.Buttons(10).ToolTipText = TOOLTIP8
  tlbToolBar.Buttons(11).ToolTipText = TOOLTIP9
  tlbToolBar.Buttons(12).ToolTipText = TOOLTIP10

  gnMULocking = True   'pessimistic locking by default
  App.HelpFile = "vb98.chm"
  setHelpFile App.HelpFile            ' set the HTML Help file name for the HTML Help engine
  setHelpLocaleID GetVbIdeLocale      ' get the locale of VBIDE for the HTML Help engine
 
  'need to disable Btrieve menu items under 32 bit
  mnuDBOBtrieve.Visible = False
  mnuDBNBtrieve.Visible = False

  'get form coordinates
  x = Val(GetRegistryString("WindowState", "2"))
  If x = vbMaximized Then
    Show
  ElseIf x <> vbMinimized Then
    frmMDI.WindowState = x
  Else
    frmMDI.WindowState = 0
  End If
  If frmMDI.WindowState = 0 Then
    frmMDI.Left = Val(GetRegistryString("WindowLeft", "0"))
    frmMDI.Top = Val(GetRegistryString("WindowTop", "0"))
    Show
    frmMDI.Width = Val(GetRegistryString("WindowWidth", "9135"))
    frmMDI.Height = Val(GetRegistryString("WindowHeight", "6900"))
  End If
  
  On Error GoTo MDILErr
  
  'setup the DBEngine
  DBEngine.IniPath = "HKEY_CURRENT_USER\Software\VB and VBA Program Settings\" & APP_CATEGORY & "\" & APPNAME
  DBEngine.DefaultUser = "admin"
  DBEngine.DefaultPassword = vbNullString

  'login to Jet
  On Error Resume Next
  Set gwsMainWS = DBEngine.CreateWorkspace("MainWS", "admin", vbNullString)
  If Err = 3029 Then
    frmLogin.Show vbModal
  ElseIf Err = 3044 Then  'invalid path so system.mda is bogus
    If MsgBox(MSG27, vbYesNo + vbQuestion) = vbYes Then
      mnuUSystemDB_Click
    Else
      'store info so we don't keep asking
      SaveSetting APP_CATEGORY & "\VisData", "Engines", "SystemDB", vbNullString
    End If
  ElseIf Err <> 0 Then
    ShowError
    Unload Me
    Exit Sub
  End If
  stsStatusBar.Panels(2).Text = MSG15 & gwsMainWS.UserName & " "
  
  On Error GoTo MDILErr
  
  'add the workspace to the collection to bump the count
  Workspaces.Append gwsMainWS
  Me.Show
  LoadRegistrySettings
  
  'attempt to open the last database if that option
  'has been set on the preferences menu
  If frmMDI.mnuPOpenOnStartup.Checked And Len(gsDBName) > 0 Then
    If gsDataType = gsSQLDB Then
'      'for an ODBC database, we need to
'      'sendkeys to open the ODBC dialog
'      SendKeys "%FOO{Enter}"
      mnuDBOODBC_Click
    Else
      OpenLocalDB True
    End If
  Else
    HideDBTools
  End If

  Exit Sub

MDILErr:
  ShowError
End Sub

Private Sub MDIForm_QueryUnload(Cancel As Integer, UnloadMode As Integer)
  On Error Resume Next
  ShutDownVisData
End Sub

Private Sub mnuDBMRU_Click(Index As Integer)
  On Error GoTo MRUErr

  gsDBName = Mid(mnuDBMRU(Index).Caption, 4, Len(mnuDBMRU(Index).Caption))
  gsDataType = mnuDBMRU(Index).Tag
  If UCase(Left(gsDataType, 5)) <> gsSQLDB Then
    OpenLocalDB True
  Else
    'must be an ODBC database so we need to load frmOpenDB
    'this will get the connect parts
    GetODBCConnectParts gsDataType
    'call the routine that will load the form
    mnuDBOODBC_Click
  End If

  Exit Sub

MRUErr:
  ShowError
End Sub

Private Sub tlbToolBar_ButtonClick(ByVal BUTTON As BUTTON)
  On Error GoTo tlbToolBar_ButtonClickErr
  
  Select Case BUTTON.Key
    Case "DataControl"
      gnFormType = gnFORM_DATACTL
    Case "NoDataControl"
      gnFormType = gnFORM_NODATACTL
    Case "DBGrid"
      gnFormType = gnFORM_DATAGRID
    
    Case "Table"
      gnRSType = gnRS_TABLE
    Case "Dynaset"
      gnRSType = gnRS_DYNASET
    Case "Snapshot"
      gnRSType = gnRS_SNAPSHOT
    Case "PassThrough"
      gnRSType = gnRS_PASSTHRU
    
    Case "BeginTrans"
      If gdbCurrentDB.Transactions = False Then
        Beep
        MsgBox MSG28
        Exit Sub
      End If
      gwsMainWS.BeginTrans
      gbDBChanged = False
      gbTransPending = True
      tlbToolBar.Buttons("BeginTrans").Enabled = False
      tlbToolBar.Buttons("Commit").Enabled = True
      tlbToolBar.Buttons("Rollback").Enabled = True
    Case "Rollback"
      If MsgBox(MSG29, vbYesNo + vbQuestion) = vbYes Then
        gwsMainWS.Rollback
        gbDBChanged = False
        gbTransPending = False
        tlbToolBar.Buttons("BeginTrans").Enabled = True
        tlbToolBar.Buttons("Commit").Enabled = False
        tlbToolBar.Buttons("Rollback").Enabled = False
      End If
    Case "Commit"
      gwsMainWS.CommitTrans
      gbDBChanged = False
      gbTransPending = False
      tlbToolBar.Buttons("BeginTrans").Enabled = True
      tlbToolBar.Buttons("Commit").Enabled = False
      tlbToolBar.Buttons("Rollback").Enabled = False
  End Select
  
  Exit Sub
tlbToolBar_ButtonClickErr:
  ShowError
End Sub
