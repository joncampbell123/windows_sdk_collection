VERSION 5.00
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Object = "{6FBA474E-43AC-11CE-9A0E-00AA0062BB4C}#1.0#0"; "SysInfo.Ocx"
Begin VB.Form frmShowReport 
   Caption         =   "Data Report"
   ClientHeight    =   840
   ClientLeft      =   1335
   ClientTop       =   870
   ClientWidth     =   4515
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   ScaleHeight     =   840
   ScaleWidth      =   4515
   Begin SysInfoLib.SysInfo sysInfo 
      Left            =   3960
      Top             =   720
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
   End
   Begin MSComCtlLib.Toolbar tlbReport 
      Align           =   1  'Align Top
      Height          =   630
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   4515
      _ExtentX        =   7964
      _ExtentY        =   1111
      ButtonWidth     =   1905
      ButtonHeight    =   953
      Appearance      =   1
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   4
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Show Report"
            Description     =   "Show the report"
            Object.ToolTipText     =   "Show Report"
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Export Report"
            Description     =   "Exports the report"
            Object.ToolTipText     =   "Export Report"
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Templates"
            Description     =   "Print Templates"
            Object.ToolTipText     =   "Print Templates"
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Exit"
            Description     =   "Exits Program"
            Object.ToolTipText     =   "Exit"
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuShow 
         Caption         =   "&Show Report"
      End
      Begin VB.Menu mnuExport 
         Caption         =   "Export Report"
      End
      Begin VB.Menu mnuPrintReport 
         Caption         =   "&Print Report"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
   End
End
Attribute VB_Name = "frmShowReport"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub mnuExit_Click()

    While rptNwind.AsyncCount > 0
        DoEvents
    Wend
    Unload rptNwind
    Unload frmShowReport
End Sub

Private Sub mnuExport_Click()
    ' Export a report. The code displays the
    ' Export Report dialog box. The ExportFormat
    ' is created in the Data Report's Initialize
    ' event.
    
     rptNwind.ExportReport _
     FormatIndexOrKey:="DailyReport", _
     FileName:="C:\Temp\DailyRpt", _
     overwrite:=True, _
     showdialog:=True, _
     range:=rptRangeFromTo, _
     pagefrom:=1, _
     pageto:=3

End Sub

Private Sub mnuPrintReport_Click()
    rptNwind.PrintReport
End Sub

Private Sub mnuShow_Click()
    rptNwind.Show
End Sub

Private Sub tlbReport_ButtonClick(ByVal Button As MSComCtlLib.Button)
    Select Case Button.Caption
    Case "Show Report"
        rptNwind.Show
    Case "Export Report"
        mnuExport_Click
    Case "Templates"
    
        ' Print all the templates.
        Dim i As Integer
        For i = 1 To rptNwind.ExportFormats.Count
            Debug.Print "TEMPLATE " & i
            Debug.Print rptNwind.ExportFormats(i).Template
            Debug.Print ' Space
        Next i
        MsgBox "Templates printed to Immediate window."
    Case "Exit"
        mnuExit_Click
    End Select
End Sub
