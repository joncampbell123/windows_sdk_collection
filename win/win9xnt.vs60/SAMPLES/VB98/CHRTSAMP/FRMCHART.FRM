VERSION 5.00
Object = "{65E121D4-0C60-11d2-A9FC-0000F8754DA1}#1.1#0"; "MSChart.OCX"
Object = "{831FDD16-0C5C-11d2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "ComDlg32.OCX"
Begin VB.Form frmChart 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Microsoft Chart Sample"
   ClientHeight    =   7050
   ClientLeft      =   1095
   ClientTop       =   1995
   ClientWidth     =   10560
   Icon            =   "frmChart.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   7050
   ScaleWidth      =   10560
   ShowInTaskbar   =   0   'False
   Begin MSComDlg.CommonDialog dlgChart 
      Left            =   10080
      Top             =   360
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin MSComCtlLib.ProgressBar prgArrays 
      Height          =   255
      Left            =   120
      TabIndex        =   14
      Top             =   0
      Visible         =   0   'False
      Width           =   10455
      _ExtentX        =   18441
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin VB.ComboBox cmbRows 
      Height          =   315
      Left            =   1320
      TabIndex        =   8
      Top             =   840
      Width           =   975
   End
   Begin VB.CommandButton cmdPPGandTotalGallons 
      Caption         =   "Gallons per Tank, Price per Gallon, && total price per tank"
      Height          =   855
      Left            =   360
      TabIndex        =   1
      Top             =   2640
      Width           =   1695
   End
   Begin VB.CommandButton cmdGalAndMPG 
      Caption         =   "Gallons && MPG"
      Height          =   255
      Left            =   360
      TabIndex        =   0
      Top             =   2280
      Width           =   1695
   End
   Begin VB.CommandButton cmdPricePerTank 
      Caption         =   "Price Per Tank"
      Height          =   255
      Left            =   360
      TabIndex        =   6
      Top             =   6120
      Width           =   1695
   End
   Begin VB.ComboBox cmbType 
      Height          =   315
      Left            =   1320
      TabIndex        =   7
      Top             =   480
      Width           =   975
   End
   Begin VB.CommandButton cmdPrices 
      Caption         =   "Price Per Gallon"
      Height          =   255
      Left            =   360
      TabIndex        =   5
      Top             =   5760
      Width           =   1695
   End
   Begin VB.CommandButton cmdGallons 
      Caption         =   "Gallons per Tank"
      Height          =   255
      Left            =   360
      TabIndex        =   4
      Top             =   5400
      Width           =   1695
   End
   Begin VB.CommandButton cmdMiles 
      Caption         =   "Miles per Tank"
      Height          =   255
      Left            =   360
      TabIndex        =   3
      Top             =   5040
      Width           =   1695
   End
   Begin VB.CommandButton cmdMPG 
      Caption         =   "Miles Per Gallon"
      Height          =   255
      Left            =   360
      TabIndex        =   2
      Top             =   4680
      Width           =   1695
   End
   Begin MSChart20Lib.MSChart chtSample 
      Height          =   6015
      Left            =   2400
      OleObjectBlob   =   "frmChart.frx":0442
      TabIndex        =   9
      Top             =   720
      Width           =   7815
   End
   Begin VB.Label lblDataPoint 
      Caption         =   "Select a data point to see its value. Double click to change it."
      Height          =   255
      Left            =   3120
      TabIndex        =   15
      Top             =   360
      Width           =   5775
   End
   Begin VB.Label Label4 
      Caption         =   "Combination Charts"
      Height          =   255
      Left            =   480
      TabIndex        =   13
      Top             =   1800
      Width           =   1575
   End
   Begin VB.Label Label3 
      Caption         =   "# of rows to show"
      Height          =   375
      Left            =   120
      TabIndex        =   12
      Top             =   840
      Width           =   1095
   End
   Begin VB.Shape Shape2 
      Height          =   1935
      Left            =   120
      Top             =   1680
      Width           =   2175
   End
   Begin VB.Label Label2 
      Caption         =   "Single Data Series Charts"
      Height          =   495
      Left            =   360
      TabIndex        =   11
      Top             =   4200
      Width           =   1695
   End
   Begin VB.Shape Shape1 
      Height          =   2535
      Left            =   120
      Top             =   4080
      Width           =   2175
   End
   Begin VB.Label Label1 
      Caption         =   "Chart Type"
      Height          =   255
      Left            =   120
      TabIndex        =   10
      Top             =   480
      Width           =   975
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
   End
End
Attribute VB_Name = "frmChart"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ------------------------------------------------------------------------
'      Copyright © 1997 Microsoft Corporation.  All rights reserved.
'
' You have a royalty-free right to use, modify, reproduce and distribute
' the Sample Application Files (and/or any modified version) in any way
' you find useful, provided that you agree that Microsoft has no warranty,
' obligations or liability for any Sample Application Files.
' ------------------------------------------------------------------------

Option Explicit
Private cmdLastClicked As CommandButton ' Stores the last command button clicked.
Private Const cdlHelpTopics = &HB 'Help constant that is missing from
                                  'the Common Dialog type library.

Private Sub chtSample_LostFocus()
    lblDataPoint.Caption = "Select a point to see it's value. Double-click to change it."
End Sub

Private Sub chtSample_PointActivated(Series As Integer, DataPoint As Integer, MouseFlags As Integer, Cancel As Integer)
    Dim vtPoint
    With chtSample
        .Column = Series
        .Row = DataPoint
        vtPoint = InputBox("Change the data point:", , .Data)
        If vtPoint <> "" Then
            If IsNumeric(vtPoint) Then
                .Data = vtPoint
            Else
                MsgBox "That's not a valid data point!"
            End If
        End If
    End With
    
End Sub

Private Sub chtSample_PointSelected(Series As Integer, DataPoint As Integer, MouseFlags As Integer, Cancel As Integer)
    ' This allows the user to see the value of any particular data point in a
    '   series by selecting it. The value of the data point is shown in the label
    '   named lblDatapoint.
    chtSample.Column = Series
    chtSample.Row = DataPoint
    lblDataPoint.Caption = "Value of Series " & Series & ", point " & DataPoint & " = " & chtSample.Data
End Sub

Private Sub cmbType_LostFocus()
    lblDataPoint.Caption = "Select a point to see it's value"
End Sub

Private Sub Form_Load()
    Me.Show
    SetupChart          ' Configures the chart.
    
    
    ' Configure combobox with chart types.
    With cmbType
        .AddItem "3dBar"    ' 0
        .AddItem "2dBar"    ' 1
        .AddItem "3dLine"   ' 2
        .AddItem "2dLine"   ' 3
        .AddItem "3dArea"   ' 4
        .AddItem "2dArea"   ' 5
        .AddItem "3dStep"   ' 6
        .AddItem "2dStep"   ' 7
        .AddItem "3dCombination"    ' 8
        .AddItem "2dCombination"    ' 9
        .AddItem "2dPie"    ' 14
        .AddItem "2dXY"     ' 16
        .ListIndex = 3
    End With
    
    ' The combobox cmbRows shows the number of rows in the
    '   chart. By default, it shows all the rows. But if you
    '   wish to see a smaller range of rows, simply click
    '   the combobox, and select the number of rows you wish
    '   to see plotted.
    Dim i As Integer
    For i = 5 To intRows
        cmbRows.AddItem i
    Next i
    cmbRows.ListIndex = intRows - 5
    
    threeColChart    ' Show a combination graph.
    ' The cmdLastClicked variable contains the last command button clicked.
    '   This information is used whenever the number of rows displayed (using
    '   cmbRows) changes. After changing the rows, the last button referenced
    '   in the variable is clicked, so the chart is repopulated.
    Set cmdLastClicked = cmdPPGandTotalGallons
End Sub


Private Sub cmbRows_Click()
    ' First of all, if the user didn't mean to change this, there's no need to
    '   go through the rest of the code.
    Static intCount As Integer
    If intCount > 0 And intRows = cmbRows.Text Then Exit Sub
    intCount = intCount + 1
    
    ' When this combobox is clicked, the public variable intRows is set to the
    '   value of the combobox. The code then shows the progressbar, which is used
    '   to give feedback on progress of the array population. The code then calls
    '   the MakeArrays procedure, which repopulates the arrays using the new
    '   intRows value. After repopulating the arrays, the progressbar is hidden
    '   again, then the variable cmdLastClicked, which contains the last button clicked,
    '   is clicked again.
    intRows = cmbRows.Text
    With prgArrays ' Show the ProgressBar while populating arrays, which may take
                   '    will take more than a few seconds.
        .Max = intRows
        .Visible = True
    End With
    
    ' Populate arrays with values from the spreadsheet using user functions.
    '   We'll use these arrays later when the user clicks any of the
    '   buttons on the form.
    PopOneArray arrMiles, "B"
    PopOneArray arrMPG, "E"
    PopOneArray arrGall, "C"
    PopOneArray arrPrices, "D"
    PopOneArray arrPerTank, "H"
    PopTwoArray arrMPGandTank, "C", "E"
    PopThreeArray arrMPGandMiles, "D", "C", "H"

    prgArrays.Visible = False ' Hide the Progressbar, now that we're done.
    
    ' If the cmdLastClicked variable contains a button, then click it. This
    '   redraws the same chart with the new array.
    If Not cmdLastClicked Is Nothing Then cmdLastClicked.Value = True
    
End Sub


Private Sub cmbType_Click()
    Select Case cmbType.ListIndex
    Case 0 To 9
        chtSample.chartType = cmbType.ListIndex
    Case 10
        chtSample.chartType = VtChChartType2dPie
    Case 11
        chtSample.chartType = VtChChartType2dXY
    End Select
    If chtSample.Chart3d = True Then
        lblDataPoint.Caption = "Hold down the Ctrl key and mouse down to rotate the chart."
    End If
End Sub

Private Sub cmdGalAndMPG_Click()
        ' Show a combination graph.
    twoColChart
    Set cmdLastClicked = cmdGalAndMPG
End Sub

Private Sub cmdGallons_Click()
    Chart arrGall, "Gallons per Tank", "Gallons" ' Show gallons.
    Set cmdLastClicked = cmdGallons
End Sub

Private Sub cmdMiles_Click()
    Chart arrMiles, "Miles per Tank", "Miles per tank"
    Set cmdLastClicked = cmdMiles
End Sub

Private Sub cmdMPG_Click()
    ' The public array MPG was populated in the SetupChart procedure.
    ' MPG
    Chart arrMPG, "Miles Per Gallon", "Miles per gallon"
    Set cmdLastClicked = cmdMPG
End Sub

Private Sub cmdPPGandTotalGallons_Click()
    threeColChart
    Set cmdLastClicked = cmdPPGandTotalGallons
End Sub

Private Sub cmdPricePerTank_Click()
    Chart arrPerTank, "Price per Tank", "Price per tankful"
    Set cmdLastClicked = cmdPricePerTank
End Sub

Private Sub cmdPrices_Click()
    Chart arrPrices, "Price Per Gallon", "Price per gallon"
    Set cmdLastClicked = cmdPrices
End Sub


Private Sub Form_Terminate()
    ' This procedure simply sets all object variables to Nothing.
    Cleanup
End Sub

Private Sub mnuContents_Click()
    With dlgChart
        .HelpFile = App.Path & "\MSChart.hlp"
        .HelpCommand = cdlHelpTopics
        .ShowHelp
    End With
End Sub

Private Sub mnuExit_Click()
    Unload Me
End Sub
