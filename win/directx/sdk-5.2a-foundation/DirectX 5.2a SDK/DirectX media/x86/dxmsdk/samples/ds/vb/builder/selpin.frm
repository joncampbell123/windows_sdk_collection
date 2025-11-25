VERSION 5.00
Begin VB.Form frmSelectPin 
   Caption         =   "Connect to Pin"
   ClientHeight    =   3210
   ClientLeft      =   4890
   ClientTop       =   4920
   ClientWidth     =   6270
   LinkTopic       =   "Form2"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   3210
   ScaleWidth      =   6270
   Begin VB.ListBox listPins 
      Height          =   1425
      Left            =   3360
      TabIndex        =   7
      Top             =   360
      Width           =   2655
   End
   Begin VB.ListBox listFilters 
      Height          =   1425
      Left            =   240
      TabIndex        =   5
      Top             =   360
      Width           =   2655
   End
   Begin VB.CommandButton Cancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3360
      TabIndex        =   4
      Top             =   2640
      Width           =   975
   End
   Begin VB.CommandButton OK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   1920
      TabIndex        =   3
      Top             =   2640
      Width           =   975
   End
   Begin VB.Label Label2 
      Caption         =   "Pins"
      Height          =   252
      Left            =   3240
      TabIndex        =   6
      Top             =   120
      Width           =   492
   End
   Begin VB.Label VendorInfoLabel 
      Caption         =   "Vendor Info:"
      Height          =   252
      Left            =   120
      TabIndex        =   2
      Top             =   2160
      Width           =   972
   End
   Begin VB.Label VendorInfo 
      Height          =   252
      Left            =   1320
      TabIndex        =   1
      Top             =   2160
      Visible         =   0   'False
      Width           =   2772
   End
   Begin VB.Label Label1 
      Caption         =   "Filters"
      Height          =   255
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   495
   End
End
Attribute VB_Name = "frmSelectPin"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' create filter components to connect filter pins
Public OtherDir As Long
Public g_objFI As IFilterInfo
Public g_objPI As IPinInfo
Public g_objMC As IMediaControl
Public bOK As Boolean

Private Sub Cancel_Click()
' cancel was clicked, so no pin connection is made
    bOK = False
    Hide
End Sub

Private Sub listFilters_Click()
' when the user clicks on a specific filter in the filter graph, this creates the
' list of pins for that filter in the pin listbox
    Dim pfilter As IFilterInfo
    For Each pfilter In g_objMC.FilterCollection
        If pfilter.Name = listFilters.Text Then
            ' display the information and pins for the selected filter
            Set g_objFI = pfilter  ' global FilterInfo object
            VendorInfo.Caption = pfilter.VendorInfo
            listPins.Clear
            Dim pin As IPinInfo
            For Each pin In pfilter.Pins
                Dim pinOther As IPinInfo
                On Error Resume Next
                Set pinOther = pin.ConnectedTo
                If Err.Number <> 0 Then
                    If pin.Direction <> OtherDir Then
                        listPins.AddItem pin.Name
                    End If
                End If
            Next pin
        End If
    Next pfilter
    listPins.ListIndex = 0
End Sub


Private Sub Form_Load()
' fill the filters listbox with all filters in the current filter graph
    RefreshFilters
End Sub



Public Sub RefreshFilters()
' fill the filters listbox with all filters in the current filter graph
    listFilters.Clear
    Dim filter As IFilterInfo
    For Each filter In g_objMC.FilterCollection
        Dim pin As IPinInfo
        Dim pinOther As IPinInfo
        For Each pin In filter.Pins
            On Error Resume Next
            Set pinOther = pin.ConnectedTo
            If Err.Number <> 0 Then
                If pin.Direction <> OtherDir Then
                    listFilters.AddItem filter.Name
                    Exit For
                End If
            End If
        Next pin
    Next filter
    listFilters.ListIndex = 0
End Sub

Private Sub OK_Click()
' connect the selected pins, if possible. if no connection is possible, the pin
' selection box closes and the program continues normally.
    Dim pin As IPinInfo
    For Each pin In g_objFI.Pins
        If pin.Name = listPins.Text Then
            Set g_objPI = pin
            bOK = True
            Exit For
        End If
    Next pin
    Hide
End Sub

Private Sub listPins_Click()
' when a pin is selected, store it in the global pin object
  Dim pPin As IPinInfo
  For Each pPin In g_objFI.Pins
    If pPin.Name = listPins.Text Then
      Set SelPin = pPin
    End If
  Next pPin
End Sub


