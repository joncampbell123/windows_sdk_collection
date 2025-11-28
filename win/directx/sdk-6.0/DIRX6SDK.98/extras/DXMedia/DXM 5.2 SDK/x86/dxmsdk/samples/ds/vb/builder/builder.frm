VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "COMDLG32.OCX"
Begin VB.Form frmMain 
   Caption         =   "Filter and Pin Viewer"
   ClientHeight    =   6060
   ClientLeft      =   1245
   ClientTop       =   1935
   ClientWidth     =   7230
   LinkTopic       =   "frmMain"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   6060
   ScaleWidth      =   7230
   Begin VB.Frame frameFilter 
      Caption         =   "Filter Graph"
      Height          =   2652
      Left            =   120
      TabIndex        =   12
      Top             =   120
      Width           =   6972
      Begin VB.CommandButton cmdSource 
         Caption         =   "Add &Source Filter..."
         Height          =   372
         Left            =   4560
         TabIndex        =   14
         Top             =   2160
         Width           =   1572
      End
      Begin VB.ListBox listFilters 
         Height          =   1425
         Left            =   3960
         TabIndex        =   4
         Top             =   480
         Width           =   2775
      End
      Begin VB.CommandButton cmdAddRegFilter 
         Caption         =   "&Add ->"
         Height          =   372
         Left            =   3120
         TabIndex        =   13
         Top             =   600
         Width           =   732
      End
      Begin VB.ListBox listRegFilters 
         Height          =   1425
         Left            =   240
         TabIndex        =   2
         Top             =   480
         Width           =   2775
      End
      Begin VB.Label lblFilters 
         Caption         =   "Filters in current filter &graph"
         Height          =   252
         Left            =   3960
         TabIndex        =   3
         Top             =   240
         Width           =   2052
      End
      Begin VB.Label lblRegFilters 
         Caption         =   "&Registered filters"
         Height          =   252
         Left            =   240
         TabIndex        =   1
         Top             =   240
         Width           =   2052
      End
   End
   Begin VB.Frame framePinInfo 
      Caption         =   "Filter"
      Height          =   3132
      Left            =   120
      TabIndex        =   0
      Top             =   2760
      Width           =   6972
      Begin VB.TextBox txtPinInfo 
         Height          =   1812
         Left            =   3720
         MultiLine       =   -1  'True
         ScrollBars      =   2  'Vertical
         TabIndex        =   17
         Text            =   "builder.frx":0000
         Top             =   1200
         Width           =   3012
      End
      Begin VB.CommandButton cmdConnect 
         Caption         =   "Co&nnect One Pin..."
         Height          =   372
         Left            =   1920
         TabIndex        =   8
         Top             =   2640
         Width           =   1692
      End
      Begin VB.CommandButton cmdRender 
         Caption         =   "&Connect Downstream"
         Height          =   372
         Left            =   120
         TabIndex        =   7
         Top             =   2640
         Width           =   1692
      End
      Begin VB.ListBox listPins 
         Height          =   1230
         Left            =   240
         TabIndex        =   6
         Top             =   1200
         Width           =   3255
      End
      Begin VB.Label lblFilterName 
         Height          =   255
         Left            =   1440
         TabIndex        =   16
         Top             =   240
         Width           =   4815
      End
      Begin VB.Label lblFilter 
         Caption         =   "Filter name:"
         Height          =   255
         Left            =   240
         TabIndex        =   15
         Top             =   240
         Width           =   975
      End
      Begin VB.Label lblVendor 
         Caption         =   "Vendor: "
         Height          =   255
         Left            =   240
         TabIndex        =   11
         Top             =   480
         Width           =   735
      End
      Begin VB.Label lblVendorInfo 
         Height          =   255
         Left            =   1440
         TabIndex        =   10
         Top             =   480
         Width           =   4935
      End
      Begin VB.Label lblPinListbox 
         Caption         =   "&Pins in selected filter"
         Height          =   255
         Left            =   240
         TabIndex        =   5
         Top             =   840
         Width           =   2055
      End
      Begin VB.Label lblPinInfo 
         Caption         =   "Information for selected pin"
         Height          =   252
         Left            =   3720
         TabIndex        =   9
         Top             =   840
         Width           =   2172
      End
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   8760
      Top             =   -120
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327680
      Flags           =   4096
   End
   Begin VB.Menu mnuFilterGraph 
      Caption         =   "&FilterGraph"
      Begin VB.Menu mnu_FilterGraphNew 
         Caption         =   "&New (empty)"
      End
      Begin VB.Menu mnu_FilterGraphGenerate 
         Caption         =   "&Generate from input file..."
      End
      Begin VB.Menu mnu_Separator1 
         Caption         =   "-"
      End
      Begin VB.Menu mnu_FilterGraphRun 
         Caption         =   "&Run"
      End
      Begin VB.Menu mnu_FilterGraphPause 
         Caption         =   "&Pause"
      End
      Begin VB.Menu mnu_FilterGraphStop 
         Caption         =   "&Stop"
      End
      Begin VB.Menu mnu_Separator2 
         Caption         =   "-"
      End
      Begin VB.Menu mnu_FilterGraphExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnu_Options 
      Caption         =   "&Options"
      Begin VB.Menu mnu_BuildCustomGraph 
         Caption         =   "&Build custom graph"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' global variables
' use the prefix "g_obj" to indicate a global object
Dim g_objMC As IMediaControl ' object  'IMediaControl
Dim g_objSelFilter As Object 'IFilterInfo
Dim g_objRegFilters As Object ' IFilterInfo collection
Dim g_objLocalRegFilter As Object ' IFilterInfo
Dim g_objSelPin As Object  ' IPinInfo; pin selected from listbox
Dim g_fRunning As Boolean  ' indicates whether the video is running or not

Private Sub RefreshRegFilters()
' update the listbox of registered filters
' using the global variable g_objRegFilters
    Dim filter As IRegFilterInfo
    listRegFilters.Clear
    If Not g_objRegFilters Is Nothing Then
        For Each filter In g_objRegFilters
            listRegFilters.AddItem filter.Name
        Next filter
    End If
    If listRegFilters.ListCount > 0 Then
        listRegFilters.ListIndex = 0  ' select first in list
    End If
End Sub

Private Sub AddFilter(FName As String, f As IFilterInfo)
' call IRegFilterInfo::filter
    Dim LocalRegFilters As Object
    
    Set LocalRegFilters = g_objMC.RegFilterCollection
    Dim filter As IRegFilterInfo
    For i = 0 To (LocalRegFilters.Count - 1) Step 1
        LocalRegFilters.Item i, filter
        If filter.Name = FName Then
            filter.filter f
            Exit For
        End If
    Next i
    RefreshFilters

End Sub

Private Sub cmdAddRegFilter_Click()
' IMediaControl property RegFilterCollection is an
' an IAMCollection interface
    Dim filter As IRegFilterInfo
    For Each filter In g_objRegFilters ' listRegFilters
        If filter.Name = listRegFilters.Text Then
            Dim f As IFilterInfo
            filter.filter f
            If f.IsFileSource Then
               CommonDialog1.ShowOpen
               ' handle user cancel
               f.filename = CommonDialog1.filename
            End If
            Exit For
        End If
    Next filter

    ' Set frmRegFilters = g_objMC.RegFilterCollection
    RefreshFilters
End Sub



Private Sub cmdConnect_Click()
' connects the current selected pin, indicated by
' g_objSelPin , g_objMC, and g_objSelFilter
    On Error Resume Next ' if already connected, connect fails
    frmSelectPin.OtherDir = g_objSelPin.Direction
    Set frmSelectPin.g_objMC = g_objMC
    Set frmSelectPin.g_objFI = g_objSelFilter
    frmSelectPin.RefreshFilters
    frmSelectPin.Show 1
    If frmSelectPin.bOK Then
        Dim objPI As IPinInfo
        Set objPI = frmSelectPin.g_objPI
        g_objSelPin.Connect objPI
        RefreshFilters  ' sub in this VB app that updates display
    End If
End Sub







Private Sub Form_Load()
' initialize the display
    listFilters.Clear
    listPins.Clear
    txtPinInfo.Text = ""
' create the global object used throughout the app
    Set g_objMC = New FilgraphManager
' update the display for registered filters
    Set g_objRegFilters = g_objMC.RegFilterCollection
    RefreshRegFilters
End Sub




Private Sub cmdRender_Click()
' call IPinInfo::Render
' complete the graph downstream from this pin
    On Error Resume Next  ' if already connected, this fails
    g_objSelPin.Render
    RefreshFilters
End Sub




Private Sub cmdSource_Click()
' calls IMediaControl::AddSourceFilter
' adds to the graph the source filter that can read the given filename
' Returns an IFilterInfo object.

    Dim objFilter As Object  ' tmp object for valid syntax; not used here
    
    On Error GoTo cmdsourceclick_cancelerror
    CommonDialog1.CancelError = True
    CommonDialog1.filter = "ActiveMovie files (*.mpg;*.avi;*.mov)|*.mpg;*.avi;*.mov|"
    CommonDialog1.ShowOpen  ' open the source filter
    g_objMC.AddSourceFilter CommonDialog1.filename, objFilter
    
    ' we don't actually use the returned IFilterInfo object here...
    
    RefreshFilters  ' update all info displayed by this VB app
    ' the app displays filters, pins, and detailed pin info
    Exit Sub

cmdsourceclick_cancelerror:
    ' user canceled out of the Open File dialog; just exit
    Exit Sub
End Sub




Public Sub RefreshFilters()
' update the contents of the "Filters" combo box
' using the current IMediaControl.FilterCollection
    Dim objFI As IFilterInfo
    
    listFilters.Clear
    For Each objFI In g_objMC.FilterCollection
        listFilters.AddItem objFI.Name
    Next objFI
    
    If listFilters.ListCount > 0 Then
        listFilters.ListIndex = 0  ' select first in list
    End If
End Sub


Private Sub listFilters_Click()
' user clicked on a filter in the filters combo box
' or entry 0 was selected by default when filling the
' "listFilters" listbox
' update the pins listbox to show all of its pins
    Dim objFI As IFilterInfo
    For Each objFI In g_objMC.FilterCollection
        If objFI.Name = listFilters.Text Then
            Set g_objSelFilter = objFI
            lblFilterName.Caption = objFI.Name
            lblVendorInfo.Caption = objFI.VendorInfo
            ' add all of this filter's pins to the combo box
            listPins.Clear
            Dim objPI As IPinInfo
            For Each objPI In objFI.Pins
                listPins.AddItem objPI.Name
            Next objPI
        End If
    Next objFI   ' next filter info
    listPins.ListIndex = 0  ' select the first in the combobox

End Sub

Private Sub listPins_Click()
' Add detailed pin information to the listbox on the right
' when the user clicks on a pin in the listbox on the left
  Dim strTemp As String
  On Error Resume Next
  Dim objPin As IPinInfo
  For Each objPin In g_objSelFilter.Pins
    If objPin.Name = listPins.Text Then ' selected in listbox?
      Set g_objSelPin = objPin  'yes, get all info
      strTemp = ""
      Dim objPinOther As IPinInfo
      Set objPinOther = objPin.ConnectedTo
      If Err.Number = 0 Then
        strTemp = "Connected to pin: " + objPinOther.Name + " "
        Dim objPeer As IFilterInfo
        Set objPeer = objPinOther.FilterInfo
        strTemp = strTemp + " on filter: " + objPeer.Name + " "
        Dim objMTI As IMediaTypeInfo
        Set objMTI = objPin.ConnectionMediaType
        strTemp = strTemp + vbCrLf + "Media Type: " + objMTI.Type
      End If
      If objPin.Direction = 0 Then
        strTemp = strTemp + " " + vbCrLf + "Direction: Input"
      Else
        strTemp = strTemp + " " + vbCrLf + "Direction: Output"
      End If
    txtPinInfo.Text = strTemp
    End If
  Next objPin
  Exit Sub
 
End Sub


Private Sub listRegFilters_DblClick()
    cmdAddRegFilter_Click
End Sub


Private Sub mnu_BuildCustomGraph_Click()

' This routine demonstrates a likely common use
' of these methods in Visual Basic applications:
' Directly creating the filter graph needed
' for a specific multimedia file.

' The graph has the following filters: AVI Source, AVI Decompressor,
' Video Renderer, AVI Splitter, and Audio Renderer.
' Note that these filters can be connected by reusing just
' two pin object variables, but for clarity of the example,
' all are defined using names that reflect their position
' in the filter graph.

' The filters are declared with their pins, as follows:

Dim pSourceFilter As IFilterInfo  ' AVI source filter; has two pins
Dim SourceOutputPin As IPinInfo  'Source Filter output pin

Dim pAVISplitter As IFilterInfo ' AVI splitter
Dim SplitterInPin As IPinInfo   ' AVI splitter pin "Input"
Dim SplitterOut00Pin As IPinInfo  ' AVI splitter pin "Stream 00"
Dim SplitterOut01Pin As IPinInfo  ' AVI splitter pin "Stream 01"

Dim pDECFilter As IFilterInfo  ' AVI Decompressor; has two pins
Dim DECInPin As IPinInfo   'AVI Decompressor pin "XForm In"
Dim DECOutPin As IPinInfo   ' AVI Decompressor pin "XForm Out"
    
Dim pVidRenderer As IFilterInfo ' Video renderer, has one pin
Dim VidRendInPin As IPinInfo  ' Video Renderer pin "Input"

Dim pAudioRenderer As IFilterInfo 'Audio renderer, has one pin
Dim AudioRendInPin As IPinInfo ' Audio Renderer pin "Input"

Dim pPin As IPinInfo
    Set SVideoPin = Nothing
    Set SAudioPin = Nothing
    Set DECInPin = Nothing
    Set DECOutPin = Nothing
    Set VidRendInPin = Nothing
    Set AudioRendInPin = Nothing
' reinitialize all global variables
    Set g_objRegFilters = Nothing
    Set g_objSelFilter = Nothing
    Set g_objSelPin = Nothing
    Set g_objMC = Nothing
' create a new IMediaControl object
    Set g_objMC = New FilgraphManager
    ' reset the listRegFilters again
    Set g_objRegFilters = g_objMC.RegFilterCollection
    RefreshRegFilters
' reinitialize the display
    listFilters.Clear
    lblFilterName.Caption = ""
    lblVendorInfo.Caption = ""
    listPins.Clear
    txtPinInfo.Text = ""
    g_fRunning = False
        
'Add source filter for an AVI file
    On Error GoTo err_CustomGraph_Cancel
    CommonDialog1.CancelError = True
    CommonDialog1.filter = "AVI files (*.avi)|*.avi"
    CommonDialog1.ShowOpen  ' get the name of the source or filter graph file
    g_objMC.AddSourceFilter CommonDialog1.filename, pSourceFilter
    
    On Error GoTo err_CustomGraph  ' handle other errors
    ' Get the pins we need to connect
    For Each pPin In pSourceFilter.Pins
      Debug.Print pPin.Name
      If pPin.Name = "Output" Then
          Set SourceOutputPin = pPin
      End If
    Next pPin
    
    'Add DEC filter
    AddFilter "AVI Decompressor", pDECFilter
    'Print out list of pins on decompressor filter
    For Each pPin In pDECFilter.Pins
      Debug.Print pPin.Name
      ' save specific pins to connect them
      If pPin.Name = "XForm In" Then
          Set DECInPin = pPin
      End If
      If pPin.Name = "XForm Out" Then
          Set DECOutPin = pPin
      End If
    Next pPin
 
    'Add AVI Splitter
    AddFilter "AVI Splitter", pAVISplitter
    'Print out list of pins on decompressor filter
    For Each pPin In pAVISplitter.Pins
      Debug.Print pPin.Name
      ' save specific pins to connect them
      ' pin 0, pin 1
      If pPin.Name = "input pin" Then
          Set SplitterInPin = pPin
      ElseIf pPin.Name = "Stream 00" Then
          Set SplitterOut00Pin = pPin
      ElseIf pPin.Name = "Stream 01" Then
          Set SplitterOut01Pin = pPin
      End If
    Next pPin
    
    'Connect Source video output pin to AVI splitter input pin
    If Not SourceOutputPin Is Nothing And Not SplitterInPin Is Nothing Then
        SourceOutputPin.Connect SplitterInPin
    End If
 
    ' Splitter now knows how many output pins it needs
    For Each pPin In pAVISplitter.Pins
      Debug.Print pPin.Name
      ' save specific pins to connect them
      ' pin 0, pin 1
      If pPin.Name = "Stream 00" Then
          Set SplitterOut00Pin = pPin
      ElseIf pPin.Name = "Stream 01" Then
          Set SplitterOut01Pin = pPin
      End If
    Next pPin
 
    'Add Video Renderer filter and set its pin variables
    AddFilter "Video Renderer", pVidRenderer
    'Print out list of pins on video renderer filter
    For Each pPin In pVidRenderer.Pins
      Debug.Print pPin.Name
      If pPin.Name = "Input" Then
          Set VidRendInPin = pPin
      End If
    Next pPin
    
    'Add Audio Renderer filter and set its pin variables
    AddFilter "Audio Renderer", pAudioRenderer
    'Print out list of pins on audioo renderer filter
    For Each pPin In pAudioRenderer.Pins
      Debug.Print pPin.Name
      If InStr(pPin.Name, "Input") Then
          Set AudioRendInPin = pPin
      End If
    Next pPin
    
    
    ' Connect AVI splitter stream 01 to AVI decompressor
    If Not DECInPin Is Nothing And Not SplitterOut00Pin Is Nothing Then
        SplitterOut00Pin.Connect DECInPin
    End If
    
    'Connect DEC filter output pin to Video Renderer input pin
    If Not DECOutPin Is Nothing And Not VidRendInPin Is Nothing Then
        DECOutPin.Connect VidRendInPin
    End If
    
    ' Connect AVI splitter stream 01 to audio renderer
    ' continue if there is no audio connection for the source AVI file
    On Error Resume Next
    If Not AudioRendInPin Is Nothing And Not SplitterOut01Pin Is Nothing Then
        SplitterOut01Pin.Connect AudioRendInPin
    End If
    
    RefreshFilters
    Exit Sub
err_CustomGraph:
    MsgBox "Could not create the custom filter graph. Please select an .AVI file that uses the AVI splitter and AVI decompressor filters."
    Exit Sub
err_CustomGraph_Cancel:
    ' user cancelled out of File Open dialog; just exit
    Exit Sub
End Sub

Private Sub mnu_FilterGraphExit_Click()
    Set g_objLocalRegFilter = Nothing
    Set g_objRegFilters = Nothing
    Set g_objSelFilter = Nothing
    Set g_objSelPin = Nothing
    Set g_objMC = Nothing
    End
End Sub

Private Sub mnu_FilterGraphGenerate_Click()
' User is initializing the filter graph based on a source file
' Create a new filter graph and then get all filters, connections

' reset the application's global objects
    Set g_objRegFilters = Nothing
    Set g_objSelFilter = Nothing
    Set g_objSelPin = Nothing
    Set g_objMC = Nothing
' initialize the display
    listFilters.Clear
    listPins.Clear
    txtPinInfo.Text = ""
' create a new IMediaControl object
    Set g_objMC = New FilgraphManager
' refresh the display for registered filters
    Set g_objRegFilters = g_objMC.RegFilterCollection
    RefreshRegFilters
    
    ' use the common dialog to let the user select the input file
    On Error GoTo filtergraphgenerate_cancelerror
    CommonDialog1.CancelError = True
    CommonDialog1.filter = "ActiveMovie files (*.mpg;*.avi;*.mov;*.wav)|*.mpg;*.avi;*.mov;*.wav|"
    CommonDialog1.ShowOpen
    
    ' call IMediaControl::RenderFile to add all filters and connect all pins
    g_objMC.RenderFile CommonDialog1.filename
    ' update and display the list of registered filters
    ' update and display all active filters for this filter graph
    RefreshFilters  ' local subroutine to update the listbox
    Exit Sub
filtergraphgenerate_cancelerror:
    ' user cancelled out of the File Open dialog
    Exit Sub
End Sub

Private Sub mnu_FilterGraphNew_Click()
' user wants to start with a fresh filter graph
' reset the global objects
    Set g_objRegFilters = Nothing
    Set g_objSelFilter = Nothing
    Set g_objSelPin = Nothing
    Set g_objMC = Nothing
' create a new IMediaControl object
    Set g_objMC = New FilgraphManager
    ' reset the listRegFilters again
    Set g_objRegFilters = g_objMC.RegFilterCollection
    RefreshRegFilters
' clear the contents of the listboxes, textboxes, and labels
    listFilters.Clear
    lblFilterName.Caption = ""
    lblVendorInfo.Caption = ""
    listPins.Clear
    txtPinInfo.Text = ""
' set the current playback state to stopped
    g_fRunning = False
End Sub


Private Sub mnu_FilterGraphPause_Click()
' sets the playback state to pause
    g_objMC.Pause
End Sub

Private Sub mnu_FilterGraphRun_Click()
' sets the playback state to run and starts the filter graph if it wasn't running
    If Not g_fRunning Then
      Dim pPosition As IMediaPosition
      Set pPosition = g_objMC
      If Err.Number = 0 Then
        On Error Resume Next
        pPosition.CurrentPosition = 0
      End If
    End If
    g_objMC.Run
    g_fRunning = True
End Sub


Private Sub mnu_FilterGraphStop_Click()
' stops the filter graph playback
    g_objMC.Stop
    g_fRunning = False
End Sub
