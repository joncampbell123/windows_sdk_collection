'-----------------------------------------------------------------------------
' File: ActionBasicApp.cs
'
' Desc: ActionBasic Sample
'
' Copyright (c) 1997-2001 Microsoft Corporation. All rights reserved
'-----------------------------------------------------------------------------
Imports System
Imports System.Threading
Imports System.Collections
Imports System.Windows.Forms
Imports DirectInput = Microsoft.DirectX.DirectInput
Imports Microsoft.DirectX.DirectInput
Imports Microsoft.DirectX

Namespace ActionBasic
    ' ****************************************************************************
    '
    ' Step 1: Define the game actions. 
    '         
    ' One of the big advantages of using action mapping to assist with game input
    ' is that it allows you to handle input in terms of game actions instead of
    ' device objects. For instance, this sample defines a small set of actions
    ' appropriate for a simple hand to hand combat game. When polling for user
    ' input, the data can be handled in terms of kicks and punches instead of
    ' button presses, axis movements, and keystrokes. This also allows to user
    ' to customize the controls without any further effort on the part of the
    ' developer.
    '
    ' Each game action will be represented within your program as a 32 bit value.
    ' For this sample, the game actions correspond to indices within an array.
    ' ****************************************************************************
    '
    '/ <summary>
    '/ Game action constants
    '/ </summary>
    Enum GameActions
        Walk ' Separate inputs are needed in this case for
        WalkLeft ' Walk/Left/Right because the joystick uses an
        WalkRight ' axis to report both left and right, but the
        Block ' keyboard will use separate arrow keys.
        Kick
        Punch
        TheDeAppetizer ' "The De-Appetizer" represents a special move
        Apologize ' defined by this game.
        Quit
        NumOfActions ' Not an action; conveniently tracks number of GameActions
        ' actions.  Keep this as the last item.
    End Enum

    '/ <summary>
    '/ Convenience wrapper for device state
    '/ </summary>
    Public Class DeviceState
        Public Device As DirectInput.Device 'Reference to the device
        Public Name As String 'Friendly name of the device      
        Public IsAxisAbsolute As Boolean 'Relative x-axis data flag
        Public InputState() As Integer 'Array of the current input values
        Public PaintState() As Integer 'Array of the current paint values
        Public IsMapped() As Boolean 'Flags whether action was successfully mapped  
        '/ <summary>
        '/ Constructor
        '/ </summary>
        Public Sub New()
            InputState = New Integer(GameActions.NumOfActions - 1) {}
            PaintState = New Integer(GameActions.NumOfActions - 1) {}
            IsMapped = New Boolean(GameActions.NumOfActions - 1) {}
        End Sub 'New
    End Class 'DeviceState

    '/ <summary>
    '/ The application class is responsible for initializing DirectInput, 
    '/ instantiating the user interface, and launching the game loop.
    '/ </summary>
    Public Class ActionBasicApp
        ' Constants
        Private AppGuid As New Guid("ED48D6E8-91D1-4cf6-9ED3-BB327F76F17D")

        ' Friendly names for action constants are used by directInput for the
        ' built-in configuration UI
        Private ActionNames As String() = {"Walk left/right", "Walk left", "Walk right", "Block", "Kick", "Punch", """The De-Appetizer""", "Apologize", "Quit"}

        ' member variables
        Private DIActionFormat(0) As ActionFormat
        Private DeviceStates As New ArrayList()
        Private InputThread As Thread

        '/ <summary>
        '/ The user interface object
        '/ </summary>
        Public UserInterface As ActionBasicUI

        '/ <summary>
        '/ Constructor
        '/ </summary>
        Public Sub New()
            ' Instantiate the user interface object. The UI is given the
            ' list of game actions and a reference to the DeviceStates array,
            ' which the chart will use to display user input.
            UserInterface = New ActionBasicUI(Me, ActionNames, DeviceStates)
            ' ************************************************************************
            ' Step 3: Enumerate Devices.
            ' 
            ' Enumerate through devices according to the desired action map.
            ' Devices are enumerated in a prioritized order, such that devices which
            ' can best be mapped to the provided action map are returned first.
            ' ************************************************************************
            ' Setup the action format for actual gameplay
            DIActionFormat(0) = New ActionFormat()
            DIActionFormat(0).ActionMapGuid = AppGuid
            DIActionFormat(0).Genre = CInt(FightingHandToHand.FightingHandToHand)
            DIActionFormat(0).AxisMin = -99
            DIActionFormat(0).AxisMax = 99
            DIActionFormat(0).BufferSize = 16
            CreateActionMap(DIActionFormat(0).Actions)

            Try
                ' Enumerate devices according to the action format
                Dim DevList As DeviceList = Manager.GetDevices(DIActionFormat(0), EnumDevicesBySemanticsFlags.AttachedOnly)
                Dim instance As SemanticsInstance
                For Each instance In DevList
                    SetupDevice(instance.Device)
                Next instance

            Catch ex As InputException
                UserInterface.ShowException(ex, "EnumDevicesBySemantics")
            End Try

            ' Start the input loop
            InputThread = New Thread(New ThreadStart(AddressOf RunInputLoop))
            InputThread.Start()
        End Sub 'New

        '/ <summary>
        '/ The main entry point for the application.
        '/ </summary>
        <STAThread()> _
        Public Shared Sub Main()
            Dim app As New ActionBasicApp()

            Try
                Application.Run(app.UserInterface)
            Catch e As ObjectDisposedException
            End Try

        End Sub 'Main

        '/ <summary>
        '/ Handles device setup
        '/ </summary>
        '/ <param name="device">DirectInput Device</param>
        Private Sub SetupDevice(ByVal device As Device)
            ' Create a temporary DeviceState object and store device information.
            Dim state As New DeviceState()
            device = device

            ' ********************************************************************
            '
            ' Step 4: Build the action map against the device, inspect the
            '         results, and set the action map.
            '
            ' It's a good idea to inspect the results after building the action
            ' map against the current device. The contents of the action map
            ' structure indicate how and to what object the action was mapped. 
            ' This sample simply verifies the action was mapped to an object on
            ' the current device, and stores the result. Note that not all actions
            ' will necessarily be mapped to an object on all devices. For instance,
            ' this sample did not request that QUIT be mapped to any device other
            ' than the keyboard.
            ' ********************************************************************
            Try
                ' Build the action map against the device
                device.BuildActionMap(DIActionFormat(0), ActionMapControl.Default)
            Catch ex As DirectXException
                UserInterface.ShowException(ex, "BuildActionMap")
                Return
            End Try

            ' Inspect the results
            Dim action As Action
            For Each action In DIActionFormat(0).Actions
                If action.How <> ActionMechanism.Error And action.How <> ActionMechanism.Unmapped Then
                    state.IsMapped(action.ApplicationData) = True
                End If
            Next action

            ' Set the action map
            Try
                device.SetActionMap(DIActionFormat(0), ApplyActionMap.Default)
            Catch ex As DirectXException
                UserInterface.ShowException(ex, "SetActionMap")
                Return
            End Try

            ' Store the device.
            state.Device = device

            ' Store the device's friendly name for display on the chart
            state.Name = device.DeviceInformation.InstanceName

            ' Store axis absolute/relative flag
            state.IsAxisAbsolute = device.Properties.AxisModeAbsolute

            DeviceStates.Add(state)
            UserInterface.UpdateChart()
            Return
        End Sub 'SetupDevice

        '/ <summary>
        '/ Launches the game loop
        '/ </summary>
        Private Sub RunInputLoop()
            While Not UserInterface.IsDisposed
                CheckInput()
                UserInterface.UpdateChart()
            End While
        End Sub 'RunInputLoop

        '/ <summary>
        '/ Handle user input
        '/ </summary>
        Private Sub CheckInput()
            ' For each device gathered during enumeration, gather input. Although when
            ' using action maps the input is received according to actions, each device 
            ' must still be polled individually. Although for most actions your
            ' program will follow the same logic regardless of which device generated
            ' the action, there are special cases which require checking from which
            ' device an action originated.
            Dim state As DeviceState
            For Each state In DeviceStates
                Dim dataCollection As BufferedDataCollection = Nothing
                Dim curState As Integer = 0

                Try
                    state.Device.Acquire()
                    state.Device.Poll()
                    dataCollection = state.Device.GetBufferedData()
                Catch
                    ' GetDeviceData can fail for several reasons, some of which are
                    ' expected during a program's execution. A device's acquisition is not
                    ' permanent, and your program might need to reacquire a device several
                    ' times. Since this sample is polling frequently, an attempt to
                    ' acquire a lost device will occur during the next call to CheckInput.
                    GoTo ContinueForEach
                End Try

                If Nothing Is dataCollection Then
                    GoTo ContinueForEach
                End If
                ' For each buffered data item, extract the game action and perform
                ' necessary game state changes. A more complex program would certainly
                ' handle each action separately, but this sample simply stores raw
                ' axis data for a WALK action, and button up or button down states for
                ' all other game actions. 
                ' Relative axis data is never reported to be zero since relative data
                ' is given in relation to the last position, and only when movement 
                ' occurs. Manually set relative data to zero before checking input.
                If Not state.IsAxisAbsolute Then
                    state.InputState(GameActions.Walk) = 0
                End If

                Dim d As BufferedData
                For Each d In dataCollection

                    ' The value stored in dwAction equals the 32 bit value stored in 
                    ' the ApplicationData member of the BufferedData structure. For this sample
                    ' we selected these action constants to be indices into an array,
                    ' but we could have chosen these values to represent anything
                    ' from variable addresses to function pointers.

                    Select Case d.ApplicationData
                        Case GameActions.Walk
                            ' Axis data. Absolute axis data is already scaled to the
                            ' boundaries selected in the BufferedData structure, but
                            ' relative data is reported as relative motion change 
                            ' since the last report. This sample scales relative data
                            ' and clips it to axis data boundaries.
                            curState = d.Data

                            If state.IsAxisAbsolute Then
                                ' scale relative data
                                curState *= 5

                                ' clip to boundaries
                                If curState < 0 Then
                                    curState = Math.Max(curState, DIActionFormat(0).AxisMin)
                                Else
                                    curState = Math.Min(curState, DIActionFormat(0).AxisMax)
                                End If
                            End If

                        Case Else
                            ' 0x80 is the directInput mask to determine whether
                            ' a button is pressed
                            curState = IIf((d.Data And &H80) <> 0, 1, 0)
                    End Select

                    state.InputState(d.ApplicationData) = curState

                Next
ContinueForEach:
            Next state
        End Sub 'CheckInput

        '/ <summary>
        '/ Creates the action map
        '/ </summary>
        '/ <returns>
        '/ An array of Action objects describing the game action 
        '/ to device object map for this application
        '/ </returns>
        Private Sub CreateActionMap(ByVal map As ActionCollection)
            ' ****************************************************************************
            ' Step 2: Define the action map. 
            '         
            ' The action map instructs DirectInput on how to map game actions to device
            ' objects. By selecting a predefined game genre that closely matches our game,
            ' you can largely avoid dealing directly with device details. For this sample
            ' we've selected the DIVIRTUAL_FIGHTING_HAND2HAND, and this constant will need
            ' to be selected into the DIACTIONFORMAT structure later to inform DirectInput
            ' of our choice. Every device has a mapping from genre actions to device
            ' objects, so mapping your game actions to genre actions almost guarantees
            ' an appropriate device configuration for your game actions.
            '
            ' If DirectInput has already been given an action map for this GUID, it
            ' will have created a user map for this application 
            ' (C:\Program Files\Common Files\DirectX\DirectInput\User Maps\*.ini). If a
            ' map exists, DirectInput will use the action map defined in the stored user 
            ' map instead of the map defined in your program. This allows the user to
            ' customize controls without losing changes when the game restarts. If you 
            ' wish to make changes to the default action map without changing the 
            ' GUID, you will need to delete the stored user map from your hard drive
            ' for the system to detect your changes and recreate a stored user map.
            ' ****************************************************************************
            ' Device input (joystick, etc.) that is pre-defined by dinput according
            ' to genre type. The genre for this app is Action->Hand to Hand Fighting.
            map.Add(CreateAction(GameActions.Walk, FightingHandToHand.AxisLateral, 0, "Walk left/right"))
            map.Add(CreateAction(GameActions.Block, FightingHandToHand.ButtonBlock, 0, "Block"))
            map.Add(CreateAction(GameActions.Kick, FightingHandToHand.ButtonKick, 0, "Kick"))
            map.Add(CreateAction(GameActions.Punch, FightingHandToHand.ButtonPunch, 0, "Punch"))
            map.Add(CreateAction(GameActions.TheDeAppetizer, FightingHandToHand.ButtonSpecial1, 0, """The De-Appetizer"""))

            ' Map the apologize button to any button on the device. directInput
            ' defines several "Any-Control Constants" for mapping game actions to
            ' any device object of a particular type.
            map.Add(CreateAction(GameActions.Apologize, DInputHelper.ButtonAny(1), 0, "Apologize"))

            ' Keyboard input mappings
            map.Add(CreateAction(GameActions.WalkLeft, Keyboard.Left, 0, "Walk left"))
            map.Add(CreateAction(GameActions.WalkRight, Keyboard.Right, 0, "Walk right"))
            map.Add(CreateAction(GameActions.Block, Keyboard.B, 0, "Block"))
            map.Add(CreateAction(GameActions.Kick, Keyboard.K, 0, "Kick"))
            map.Add(CreateAction(GameActions.Punch, Keyboard.P, 0, "Punch"))
            map.Add(CreateAction(GameActions.TheDeAppetizer, Keyboard.D, 0, """The De-Appetizer"""))
            map.Add(CreateAction(GameActions.Apologize, Keyboard.A, 0, "Apologize"))

            ' The AppFixed constant can be used to instruct directInput that the
            ' current mapping can not be changed by the user.
            map.Add(CreateAction(GameActions.Quit, Keyboard.Q, ActionAttributeFlags.AppFixed, "Quit"))

            ' Mouse input mappings
            map.Add(CreateAction(GameActions.Walk, Mouse.XAxis, 0, "Walk"))
            map.Add(CreateAction(GameActions.Punch, Mouse.Button0, 0, "Punch"))
            map.Add(CreateAction(GameActions.Kick, Mouse.Button1, 0, "Kick"))
        End Sub 'CreateActionMap


        '/ <summary>
        '/ Helper method to fill the action map during initialization
        '/ </summary>
        Private Function CreateAction(ByVal AppData As GameActions, ByVal Semantic As Object, ByVal Flags As Object, ByVal ActionName As String) As Action
            Dim action As New Action()

            action.ApplicationData = AppData
            action.Semantic = Semantic
            action.Flags = Flags
            action.ActionName = ActionName

            Return action
        End Function 'CreateAction


        '/ <summary>
        '/ Show the native device configuration UI
        '/ </summary>
        Public Sub ConfigureDevices()
            Dim formats As ActionFormat() = DIActionFormat

            Dim cdParams As New ConfigureDevicesParameters()
            cdParams.SetActionFormats(formats)
            Manager.ConfigureDevices(cdParams, ConfigureDevicesFlags.Default)
        End Sub 'ConfigureDevices
    End Class 'ActionBasicApp
End Namespace 'ActionBasic
