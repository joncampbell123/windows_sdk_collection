VERSION 5.00
Begin VB.Form frmMCITest 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "MCI Control Application"
   ClientHeight    =   870
   ClientLeft      =   2040
   ClientTop       =   2040
   ClientWidth     =   3765
   Icon            =   "mcitest.frx":0000
   LinkMode        =   1  'Source
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   ScaleHeight     =   870
   ScaleWidth      =   3765
   Begin VB.Menu AL_DEVICES 
      Caption         =   "&Devices"
      Begin VB.Menu AI_CDAUDIO 
         Caption         =   "&CDAudio..."
      End
      Begin VB.Menu AI_SEQUENCER 
         Caption         =   "&Sequencer..."
      End
      Begin VB.Menu AI_WAVEAUDIO 
         Caption         =   "&WaveAudio..."
      End
   End
   Begin VB.Menu AL_HELP 
      Caption         =   "&Help"
      Begin VB.Menu AI_ABOUT 
         Caption         =   "&About..."
      End
   End
End
Attribute VB_Name = "frmMCITest"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub AI_ABOUT_Click()
    ' Display the About Box.  Allow Windows messages
    ' to be processed before setting the focus to the
    ' OK button.
    frmAboutBox.Show
    WaitForEventsToFinish 50
    On Error Resume Next
    frmAboutBox.BTN_OK.SetFocus
End Sub

Private Sub AI_CDAUDIO_Click()
    frmCD.Show
    frmMCITest.Hide
    DisplayWarning
    
End Sub

Private Sub AI_SEQUENCER_Click()
    DialogCaption = "MIDI Sequencer - "
    frmWave.Caption = "MIDI Sequencer"
    
    ' Set the Open File dialog box to display *.mid files.
    frmOpenDlg.dlgOpenFile.Filter = "MIDI File (*.mid)|*.mid"
    frmWave.mciWave.DeviceType = "Sequencer"
    frmWave.Show
    frmMCITest.Hide
End Sub

Private Sub AI_WAVEAUDIO_Click()
    DialogCaption = "WaveAudio - "
    frmWave.Caption = "WaveAudio"
    
    ' Set the Open File dialog box to display *.wav files.
    frmOpenDlg.dlgOpenFile.Filter = "WaveForm File (*.wav)|*.wav"
    frmWave.mciWave.DeviceType = "WaveAudio"
    frmWave.Show
    frmMCITest.Hide
End Sub

Private Sub Form_Unload(Cancel As Integer)
    ' Remember to unload the OpenDlg form since it is
    ' only "hidden" when it is removed.
    Unload frmOpenDlg
End Sub

