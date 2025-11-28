Imports System
Imports System.Drawing
Imports System.Windows.Forms
Imports System.IO
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectInput
Imports System.Collections

Public Class MainForm
    Inherits Form
    Private components As System.ComponentModel.Container = Nothing
    Private lblStatic As Label
    Private lblStatic1 As Label
    Private lblStatic2 As Label

    Private strPath As String = String.Empty
    Private applicationDevice As Device = Nothing
    Private WithEvents btnReadFile As System.Windows.Forms.Button
    Private WithEvents btnPlayEffects As System.Windows.Forms.Button
    Private WithEvents btnExit As System.Windows.Forms.Button
    Private applicationEffects As ArrayList = New ArrayList()

    Public Shared Sub Main()
        Application.Run(New MainForm())
    End Sub

    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If (disposing) Then
            If Not components Is Nothing Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub
    Public Sub New()
        InitializeComponent()
    End Sub

#Region "InitializeComponent code"
    Private Sub InitializeComponent()
        Me.lblStatic = New System.Windows.Forms.Label()
        Me.lblStatic1 = New System.Windows.Forms.Label()
        Me.lblStatic2 = New System.Windows.Forms.Label()
        Me.btnReadFile = New System.Windows.Forms.Button()
        Me.btnPlayEffects = New System.Windows.Forms.Button()
        Me.btnExit = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'lblStatic
        '
        Me.lblStatic.Location = New System.Drawing.Point(10, 9)
        Me.lblStatic.Name = "lblStatic"
        Me.lblStatic.Size = New System.Drawing.Size(342, 13)
        Me.lblStatic.TabIndex = 3
        Me.lblStatic.Text = "This sample reads a DirectInput force feedback effects file.  After"
        '
        'lblStatic1
        '
        Me.lblStatic1.Location = New System.Drawing.Point(10, 24)
        Me.lblStatic1.Name = "lblStatic1"
        Me.lblStatic1.Size = New System.Drawing.Size(326, 13)
        Me.lblStatic1.TabIndex = 4
        Me.lblStatic1.Text = "reading the file the effects can be played back on the force "
        '
        'lblStatic2
        '
        Me.lblStatic2.Location = New System.Drawing.Point(10, 38)
        Me.lblStatic2.Name = "lblStatic2"
        Me.lblStatic2.Size = New System.Drawing.Size(110, 13)
        Me.lblStatic2.TabIndex = 5
        Me.lblStatic2.Text = "feedback device."
        '
        'btnReadFile
        '
        Me.btnReadFile.Location = New System.Drawing.Point(8, 72)
        Me.btnReadFile.Name = "btnReadFile"
        Me.btnReadFile.Size = New System.Drawing.Size(80, 24)
        Me.btnReadFile.TabIndex = 6
        Me.btnReadFile.Text = "&Read File"
        '
        'btnPlayEffects
        '
        Me.btnPlayEffects.Enabled = False
        Me.btnPlayEffects.Location = New System.Drawing.Point(96, 72)
        Me.btnPlayEffects.Name = "btnPlayEffects"
        Me.btnPlayEffects.Size = New System.Drawing.Size(80, 24)
        Me.btnPlayEffects.TabIndex = 7
        Me.btnPlayEffects.Text = "&Play Effects"
        '
        'btnExit
        '
        Me.btnExit.Location = New System.Drawing.Point(256, 72)
        Me.btnExit.Name = "btnExit"
        Me.btnExit.Size = New System.Drawing.Size(80, 24)
        Me.btnExit.TabIndex = 8
        Me.btnExit.Text = "&Exit"
        '
        'MainForm
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(344, 106)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.btnExit, Me.btnPlayEffects, Me.btnReadFile, Me.lblStatic, Me.lblStatic1, Me.lblStatic2})
        Me.Name = "MainForm"
        Me.Text = "DirectInput FFE File Reader"
        Me.ResumeLayout(False)

    End Sub
#End Region

    Private Sub btnReadFile_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnReadFile.Click
        '-----------------------------------------------------------------------------
        ' Name: OnReadFile()
        ' Desc: Reads a file contain a collection of DirectInput force feedback 
        '       effects.  It creates each of effect read in and stores it 
        '       in the linked list, g_EffectsList.
        '-----------------------------------------------------------------------------

        Dim ofd As OpenFileDialog = New OpenFileDialog()
        Dim effects As EffectList = Nothing

        If (strPath = String.Empty) Then strPath = DXUtil.SdkMediaPath

        ' Setup the OpenFileDialog structure.
        ofd.Filter = "FEdit Files|*.ffe|All Files|*.*"
        ofd.InitialDirectory = strPath
        ofd.Title = "Open FEdit File"
        ofd.ShowReadOnly = False

        ' Display the OpenFileName dialog. Then, try to load the specified file.
        If DialogResult.Cancel = ofd.ShowDialog() Then Return

        ' Store the path.
        strPath = Path.GetDirectoryName(ofd.FileName)

        ' Get the effects in the file selected.
        effects = applicationDevice.GetEffects(ofd.FileName, FileEffectsFlags.ModifyIfNeeded)

        EmptyEffectList()

        Dim f As FileEffect
        For Each f In effects

            Dim eo As EffectObject = New EffectObject(f.EffectGuid, f.EffectStruct, applicationDevice)
            applicationEffects.Add(eo)
        Next

        ' If list of effects is empty, then there are no effects created.
        If (0 = applicationEffects.Count) Then
            ' Pop up a box informing the user.
            MessageBox.Show("Unable to create any effects.")
            btnPlayEffects.Enabled = False
        Else
            ' There are effects, so enable the 'play effects' button.
            btnPlayEffects.Enabled = True
        End If
    End Sub

    Private Sub MainForm_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Dim list As DeviceList = Manager.GetDevices(DeviceClass.GameControl, EnumDevicesFlags.AttachedOnly Or EnumDevicesFlags.ForceFeeback)
        If (0 = list.Count) Then
            MessageBox.Show("No force feedback devices attached to the system. Sample will now exit.")
            Close()
        End If

        list.MoveNext()
        applicationDevice = New Device((list.Current).InstanceGuid)
        applicationDevice.SetDataFormat(DeviceDataFormat.Joystick)
        applicationDevice.SetCooperativeLevel(Me, CooperativeLevelFlags.Exclusive Or CooperativeLevelFlags.Background)

        Try
            applicationDevice.Acquire()
        Catch ie As InputException
        End Try

    End Sub

    Private Sub OnPlayEffects()

        '-----------------------------------------------------------------------------
        ' Name: OnPlayEffects()
        ' Desc: Plays all of the effects enumerated in the file 
        '-----------------------------------------------------------------------------

        ' Stop all previous forces.
        applicationDevice.SendForceFeedbackCommand(ForceFeedbackCommand.StopAll)

        Dim eo As EffectObject
        For Each eo In applicationEffects
            ' Play all of the effects enumerated in the file .
            eo.Start(1, EffectStartFlags.NoDownload)
        Next
    End Sub

    Private Sub btnPlayEffects_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnPlayEffects.Click
        OnPlayEffects()
    End Sub

    Private Sub MainForm_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated
        If (Not Nothing Is applicationDevice) Then
            Try
                applicationDevice.Acquire()
            Catch ie As InputException
            End Try
        End If
    End Sub

    Private Sub btnExit_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnExit.Click
        Close()
    End Sub

    Private Sub EmptyEffectList()
        applicationEffects.Clear()
    End Sub
End Class
