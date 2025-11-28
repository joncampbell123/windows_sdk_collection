VERSION 5.00
Object = "{65E121D4-0C60-11d2-A9FC-0000F8754DA1}#1.1#0"; "MSChart.OCX"
Begin VB.Form frmChart 
   Caption         =   "Revenue Chart"
   ClientHeight    =   5055
   ClientLeft      =   1665
   ClientTop       =   4425
   ClientWidth     =   8160
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5055
   ScaleWidth      =   8160
   WhatsThisHelp   =   -1  'True
   Begin MSChart20Lib.MSChart grfRevenue 
      Height          =   4935
      Left            =   1080
      OleObjectBlob   =   "frmchart.frx":0000
      TabIndex        =   1
      Top             =   120
      Width           =   5415
   End
   Begin VB.CommandButton cmdClose 
      Caption         =   "&Close"
      Default         =   -1  'True
      Height          =   420
      Left            =   6840
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
End
Attribute VB_Name = "frmChart"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdClose_Click()
  Unload Me

End Sub

Private Sub Form_Load()
  
  SetGraphData
  
End Sub
Sub SetGraphData()
  Dim strGraphData As String
  Dim vGraphData As Variant
  Dim strSrchString As String * 1
  Dim lStart As Long
  Dim lEnd As Long
  Dim lstrLen As Long
  Dim i As Integer
  Dim j As Integer

  On Error GoTo SetGraphDataError
  Screen.MousePointer = vbHourglass

  ' Request results from business rule.
  frmRevenue.sbrRev.Panels(1).Text = "Retrieving chart data from Bobsvr..."
  vGraphData = gobjServer.GetRevenue(CInt(Val(frmRevenue.frmSalesModel.Tag)), _
                                                                CCur(Val(frmRevenue.txtRevParm(0).Tag)), _
                                                                CCur(frmRevenue.txtRevParm(1).Text), _
                                                                CInt(frmRevenue.txtRevParm(2).Text), _
                                                                CLng(frmRevenue.txtRevParm(3).Text), _
                                                                CBool(frmRevenue.chkDiscount.Value), _
                                                                frmRevenue.cboBooks.Text)
 
  ' Check if Server returned 0 (error occured)
  If Not IsArray(vGraphData) Then
      MsgBox "An error has occured with the Server. The Client will not work properly. The application will end!", vbExclamation, "Book Sales Client Error"
      End
  End If

  grfRevenue.RowCount = Val(frmRevenue.txtRevParm(2).Text)
  grfRevenue.ColumnCount = 2
  grfRevenue.AutoIncrement = True
  
  goStatusPanel.Text = "Plotting chart data..."
  
  For j = 0 To UBound(vGraphData, 2)
    For i = 0 To UBound(vGraphData, 1)
      grfRevenue.Row = i + 1
      grfRevenue.RowLabel = CStr(i + 1)
      grfRevenue.Column = j + 1
      grfRevenue.Data = vGraphData(i, j)
    Next i
  Next j
 
  Screen.MousePointer = vbDefault
  Exit Sub

SetGraphDataError:
  Screen.MousePointer = vbDefault
  MsgBox Error$ & " - " & Str$(Err), vbExclamation, "SetGraphData Error"
  Exit Sub

End Sub

