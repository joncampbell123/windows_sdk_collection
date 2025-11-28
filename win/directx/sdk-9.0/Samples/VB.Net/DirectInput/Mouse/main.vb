Imports System.Threading

Module main
    Public Class cMain
        Implements IDisposable
        ' These two fields hold common data that
        ' different threads will have to access
        Public Shared m_dims As MouseState2
        Public Shared g_bRunning As Boolean = True

        Private m_di As InputObject = Nothing
        Private m_dev As Device = Nothing
        Private m_threadInput As Thread = Nothing
        Private m_UI As frmUI = Nothing

        Public Sub New()
            ' Create an instance of the UI form.
            m_UI = New frmUI()
            ' Create a DirectInputObject object.
            m_di = New InputObject(DXHelp.AppInstance)
            ' Create the device.
            m_dev = New Device(SystemGuid.Mouse, m_di)
            ' Set the cooperative level for the device.
            m_dev.SetCooperativeLevel(m_UI.Handle, CooperativeLevelFlags.Exclusive Or CooperativeLevelFlags.Foreground)
            ' Set the data format to the mouse2 pre-defined format.
            m_dev.SetDataFormat(DeviceDataFormat.Mouse2)
            ' Create the thread that the app will use
            ' to get state information from the device.
            m_threadInput = New Thread(AddressOf Me.DIThread)
            ' Name it something so we can see in the output
            ' window when it has exited.
            m_threadInput.Name = "DIThread"
        End Sub
        Public Function Start()
            ' Start the thread.
            m_threadInput.Start()
            ' Show the UI form.
            m_UI.ShowDialog()
            ' Wait until the input thread exits.
            m_threadInput.Join()
            ' Call the function that destroys all the objects.
            Me.Dispose()
        End Function
        Public Sub DIThread()
            ' The actual input loop runs in this thread.

            ' Bool flag that is set when it's ok
            ' to get device state information.
            Dim bOk As Boolean = False

            ' Make sure there is a valid device.
            If (Not m_dev Is Nothing) Then
                ' Keep looping.
                While (g_bRunning)
                    Try
                        ' Don't really need to poll a mouse, but
                        ' this is a good way to check if the app
                        ' can get the device state.
                        m_dev.Poll()
                    Catch ex As DirectXException
                        ' Check to see if either the app
                        ' needs to acquire the device, or
                        ' if the app lost the mouse to another
                        ' process.
                        If ((ex.ErrorCode = ErrorCode.NotAcquired) Or (ex.ErrorCode = ErrorCode.InputLost)) Then
                            Try
                                ' Acquire the device.
                                m_dev.Acquire()
                                ' Set the flag for now.
                                bOk = True
                            Catch ex2 As DirectXException
                                If (ex2.ErrorCode <> ErrorCode.OtherAppHasPrio) Then
                                    ' Something very odd happened.
                                    Throw New Exception("An unknown error has occcurred. This app won't be able to process device info.")
                                End If
                                ' Failed to aquire the device.
                                ' This could be because the app
                                ' doesn't have focus.
                                bOk = False
                            End Try
                        End If
                    End Try
                    If (bOk = True) Then
                        ' Lock the UI class so it can't overwrite
                        ' the m_dims structure during a race condition.
                        SyncLock (m_UI)
                            ' Get the state of the device
                            Try
                                m_dims = m_dev.CurrentMouseState2
                                ' Catch any exceptions. None will be handled here, 
                                ' any device re-aquisition will be handled above.	
                            Catch ex As DirectXException
                            End Try
                            ' Call the function in the other thread that updates the UI.
                            m_UI.UpdateUI()
                        End SyncLock
                    End If
                End While
            End If
        End Sub

        Public Sub Dispose() Implements System.IDisposable.Dispose
            ' Unacquire and destroy all Dinput objects.
            m_dev.Unacquire()
            m_dev.Dispose()
            m_di.Dispose()
        End Sub
    End Class
End Module

Module Startup
    <MTAThread()> Public Sub Main()
        Dim mainClass As cMain = New cMain()
        mainClass.Start()
    End Sub
End Module