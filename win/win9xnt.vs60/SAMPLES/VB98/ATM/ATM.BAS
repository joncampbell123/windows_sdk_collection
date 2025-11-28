Attribute VB_Name = "modATM"
Option Explicit

' Offset variable
Public I As Long

' Currency conversion table from USD
Public ConversionTable As Variant

' High level sound support API
Declare Function sndPlaySound Lib "WINMM.DLL" Alias "sndPlaySoundA" _
        (lpszSoundName As Any, ByVal uFlags As Long) As Long

Global Const SND_ASYNC = &H1     ' Play asynchronously
Global Const SND_NODEFAULT = &H2 ' Don't use default sound
Global Const SND_MEMORY = &H4    ' lpszSoundName points to a memory file

Global SoundBuffer() As Byte

' Mousepointer over command buttons.
Dim curSelect As StdPicture

Sub ConversionTable_Initialize()
    ' Upadated on 08/14/1996.
    ConversionTable = Array(1@, 5.0776@, 1.4861@, 1519.28@, 126.1@, 107.95@)
End Sub

Sub BeginPlaySound(ByVal ResourceId As Integer)
    SoundBuffer = LoadResData(ResourceId, "ATM_SOUND")
    sndPlaySound SoundBuffer(0), SND_ASYNC Or SND_NODEFAULT Or SND_MEMORY
End Sub

Sub EndPlaySound()
    sndPlaySound ByVal vbNullString, 0&
End Sub

Sub Cursor_Initialize()
    Set curSelect = LoadResPicture(1, vbResCursor)
End Sub

Sub SetCursor(Button As CommandButton)
    With Button
        .MousePointer = 99
        .MouseIcon = curSelect
    End With
End Sub
