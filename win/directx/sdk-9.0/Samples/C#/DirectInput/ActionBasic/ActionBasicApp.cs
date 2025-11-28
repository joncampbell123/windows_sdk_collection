//-----------------------------------------------------------------------------
// File: ActionBasicApp.cs
//
// Desc: The ActionBasic sample is intended to be an introduction to action mapping, 
//       and illustrates a step by step approach to creating an action mapped 
//       application.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
using System;
using System.Threading;
using System.Collections;
using System.Windows.Forms;
using DirectInput = Microsoft.DirectX.DirectInput;
using Microsoft.DirectX.DirectInput;
using Microsoft.DirectX;

namespace ActionBasic
{
    #region Step 1: Define the game actions.
    // ****************************************************************************
    // Step 1: Define the game actions. 
    //         
    // One of the big advantages of using action mapping to assist with game input
    // is that it allows you to handle input in terms of game actions instead of
    // device objects. For instance, this sample defines a small set of actions
    // appropriate for a simple hand to hand combat game. When polling for user
    // input, the data can be handled in terms of kicks and punches instead of
    // button presses, axis movements, and keystrokes. This also allows to user
    // to customize the controls without any further effort on the part of the
    // developer.
    //
    // Each game action will be represented within your program as a 32 bit value.
    // For this sample, the game actions correspond to indices within an array.
    // ****************************************************************************
    #endregion

    
    
    
    /// <summary>
    /// Game action constants
    /// </summary>
    enum GameActions 
    {
        Walk,              // Separate inputs are needed in this case for
        WalkLeft,          //   Walk/Left/Right because the joystick uses an
        WalkRight,         //   axis to report both left and right, but the
        Block,             //   keyboard will use separate arrow keys.
        Kick, 
        Punch, 
        TheDeAppetizer,    // "The De-Appetizer" represents a special move
        Apologize,         //   defined by this game.
        Quit,
        NumOfActions       // Not an action; conveniently tracks number of
    };                     //   actions.  Keep this as the last item.




    /// <summary>
    /// Convenience wrapper for device state
    /// </summary>
    public class DeviceState
    {
        public DirectInput.Device Device; //Reference to the device

        public string Name;           //Friendly name of the device      
        public bool   IsAxisAbsolute; //Relative x-axis data flag
        public int[]  InputState;     //Array of the current input values
        public int[]  PaintState;     //Array of the current paint values
        public bool[] IsMapped;       //Flags whether action was successfully mapped  
                                          
        /// <summary>
        /// Constructor
        /// </summary>
        public DeviceState()
        {
            InputState = new int[(int)GameActions.NumOfActions];
            PaintState = new int[(int)GameActions.NumOfActions];
            IsMapped   = new bool[(int)GameActions.NumOfActions];
        }
    };




	/// <summary>
	/// The application class is responsible for initializing DirectInput, 
	/// instantiating the user interface, and launching the game loop.
	/// </summary>
    public class ActionBasicApp
    {
        // Constants
        private readonly Guid       AppGuid   = new Guid("ED48D6E8-91D1-4cf6-9ED3-BB327F76F17D");  
        
        // Friendly names for action constants are used by directInput for the
        // built-in configuration UI
        private readonly string[] ActionNames = 
        {
            "Walk left/right",
            "Walk left",
            "Walk right",
            "Block",
            "Kick",
            "Punch",
            "\"The De-Appetizer\"",
            "Apologize",
            "Quit"
        };
 
        // member variables
        private ActionFormat      DIActionFormat = new ActionFormat();
        private ArrayList         DeviceStates   = new ArrayList();    
        private Thread            InputThread;
        
        /// <summary>
        /// The user interface object
        /// </summary>
        public ActionBasicUI      UserInterface;
        
        /// <summary>
        /// Constructor
        /// </summary>
        public ActionBasicApp()
        {
            // Instantiate the user interface object. The UI is given the
            // list of game actions and a reference to the DeviceStates array,
            // which the chart will use to display user input.
            UserInterface = new ActionBasicUI(this, ActionNames, DeviceStates);

            #region Step 3: Enumerate Devices.
            // ************************************************************************
            // Step 3: Enumerate Devices.
            // 
            // Enumerate through devices according to the desired action map.
            // Devices are enumerated in a prioritized order, such that devices which
            // can best be mapped to the provided action map are returned first.
            // ************************************************************************
            #endregion

            // Setup the action format for actual gameplay
            DIActionFormat.ActionMapGuid = AppGuid;
            DIActionFormat.Genre = (int)FightingHandToHand.FightingHandToHand;
            DIActionFormat.AxisMin = -99;
            DIActionFormat.AxisMax = 99;
            DIActionFormat.BufferSize = 16;
            CreateActionMap(DIActionFormat.Actions);
            
            try
            {
                // Enumerate devices according to the action format
                DeviceList DevList = Manager.GetDevices(DIActionFormat, EnumDevicesBySemanticsFlags.AttachedOnly);
                foreach (SemanticsInstance instance in DevList)
                {
                    SetupDevice(instance.Device);
                }

            }
            catch (DirectXException ex)
            {
                UserInterface.ShowException(ex, "EnumDevicesBySemantics");
            }

            // Start the input loop
            InputThread = new Thread(new ThreadStart(RunInputLoop));
            InputThread.Start();
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main() 
        {   
            ActionBasicApp app = new ActionBasicApp();
            Application.Run(app.UserInterface); 
        }


        
        
        /// <summary>
        /// Handles device setup
        /// </summary>
        /// <param name="device">DirectInput Device</param>
        private void SetupDevice(Device device)
        {
            // Create a temporary DeviceState object and store device information.
            DeviceState state = new DeviceState();
            device = device;   

            #region Step 4: Build the action map against the device
            // ********************************************************************
            // Step 4: Build the action map against the device, inspect the
            //         results, and set the action map.
            //
            // It's a good idea to inspect the results after building the action
            // map against the current device. The contents of the action map
            // structure indicate how and to what object the action was mapped. 
            // This sample simply verifies the action was mapped to an object on
            // the current device, and stores the result. Note that not all actions
            // will necessarily be mapped to an object on all devices. For instance,
            // this sample did not request that QUIT be mapped to any device other
            // than the keyboard.
            // ********************************************************************
            #endregion
            try 
            {
                // Build the action map against the device
                device.BuildActionMap(DIActionFormat, ActionMapControl.Default);
            }
            catch(DirectXException ex)
            {
                UserInterface.ShowException(ex, "BuildActionMap");
                return;
            }

            // Inspect the results
            foreach (Action action in DIActionFormat.Actions)
            {
                if ((int)action.How != (int)ActionMechanism.Error &&
                    (int)action.How != (int)ActionMechanism.Unmapped)
                {
                    state.IsMapped[(int)action.ApplicationData] = true;
                }
            }

            // Set the action map
            try
            {
                device.SetActionMap(DIActionFormat, ApplyActionMap.Default);
            }
            catch(DirectXException ex)
            {
                UserInterface.ShowException(ex, "SetActionMap");
                return;
            }
            
            state.Device = device;

            // Store the device's friendly name for display on the chart
            state.Name = device.DeviceInformation.InstanceName;

            // Store axis absolute/relative flag
            state.IsAxisAbsolute = device.Properties.AxisModeAbsolute;
 
            DeviceStates.Add(state);
            UserInterface.UpdateChart();
            return;
        }


        
        
        /// <summary>
        /// Launches the game loop
        /// </summary>
        private void RunInputLoop()
        {
            while(!UserInterface.IsDisposed)
            {
                CheckInput();
                UserInterface.UpdateChart();                
                Application.DoEvents();
                Thread.Sleep(10);
            }
        }

        
        
        
        /// <summary>
        /// Handle user input
        /// </summary>
        private void CheckInput()
        {
            // For each device gathered during enumeration, gather input. Although when
            // using action maps the input is received according to actions, each device 
            // must still be polled individually. Although for most actions your
            // program will follow the same logic regardless of which device generated
            // the action, there are special cases which require checking from which
            // device an action originated.

            foreach (DeviceState state in DeviceStates)
            {
                BufferedDataCollection dataCollection; 
                int curState = 0;

                try 
                {
                    state.Device.Acquire();
                    state.Device.Poll();
                    dataCollection = state.Device.GetBufferedData(); 
                }
                catch(DirectXException)
                {
                    // GetDeviceData can fail for several reasons, some of which are
                    // expected during a program's execution. A device's acquisition is not
                    // permanent, and your program might need to reacquire a device several
                    // times. Since this sample is polling frequently, an attempt to
                    // acquire a lost device will occur during the next call to CheckInput.
                    continue;
                }

                if (null == dataCollection)
                    continue;
                // For each buffered data item, extract the game action and perform
                // necessary game state changes. A more complex program would certainly
                // handle each action separately, but this sample simply stores raw
                // axis data for a WALK action, and button up or button down states for
                // all other game actions. 

                // Relative axis data is never reported to be zero since relative data
                // is given in relation to the last position, and only when movement 
                // occurs. Manually set relative data to zero before checking input.
                if (!state.IsAxisAbsolute)
                    state.InputState[(int)GameActions.Walk] = 0;

                foreach (BufferedData data in dataCollection)
                {
                    // The value stored in Action.ApplicationData equals the value stored in 
                    // the ApplicationData property of the BufferedData class. For this sample
                    // we selected these action constants to be indices into an array,
                    // but we could have chosen these values to represent anything
                    // from class objects to delegates.

                    curState = 0;

                    switch ((int)data.ApplicationData)
                    {
                        case (int)GameActions.Walk:  
                        {
                            // Axis data. Absolute axis data is already scaled to the
                            // boundaries selected in the diACTIONFORMAT structure, but
                            // relative data is reported as relative motion change 
                            // since the last report. This sample scales relative data
                            // and clips it to axis data boundaries.
                            curState = data.Data;

                            if (!state.IsAxisAbsolute)
                            {
                                // scale relative data
                                curState *= 5;

                                // clip to boundaries
                                if (curState < 0)
                                    curState = Math.Max(curState, DIActionFormat.AxisMin);
                                else
                                    curState = Math.Min(curState, DIActionFormat.AxisMax);
                            }

                            break;
                        }

                        default:
                        {
                            // 0x80 is the DirectInput mask to determine whether
                            // a button is pressed
                            curState = ((data.Data & 0x80) != 0) ? 1 : 0;
                            break;
                        }
                    }

                    state.InputState[ (int)data.ApplicationData ] = curState;
                }
            }
        }

        
        
        
        /// <summary>
        /// Creates the action map
        /// </summary>
        /// <returns>
        /// An array of Action objects describing the game action 
        /// to device object map for this application
        /// </returns>
        private void CreateActionMap(ActionCollection map)
        {
            #region Step 2: Define the action map.
            // ****************************************************************************
            // Step 2: Define the action map. 
            //         
            // The action map instructs DirectInput on how to map game actions to device
            // objects. By selecting a predefined game genre that closely matches our game,
            // you can largely avoid dealing directly with device details. For this sample
            // we've selected the DIVIRTUAL_FIGHTING_HAND2HAND, and this constant will need
            // to be selected into the DIACTIONFORMAT structure later to inform DirectInput
            // of our choice. Every device has a mapping from genre actions to device
            // objects, so mapping your game actions to genre actions almost guarantees
            // an appropriate device configuration for your game actions.
            //
            // If DirectInput has already been given an action map for this GUID, it
            // will have created a user map for this application 
            // (C:\Program Files\Common Files\DirectX\DirectInput\User Maps\*.ini). If a
            // map exists, DirectInput will use the action map defined in the stored user 
            // map instead of the map defined in your program. This allows the user to
            // customize controls without losing changes when the game restarts. If you 
            // wish to make changes to the default action map without changing the 
            // GUID, you will need to delete the stored user map from your hard drive
            // for the system to detect your changes and recreate a stored user map.
            // ****************************************************************************
            #endregion
            
            // Device input (joystick, etc.) that is pre-defined by dinput according
            // to genre type. The genre for this app is Action->Hand to Hand Fighting.
            map.Add(CreateAction(GameActions.Walk,           FightingHandToHand.AxisLateral,    0, "Walk left/right"));
            map.Add(CreateAction(GameActions.Block,          FightingHandToHand.ButtonBlock,    0, "Block"));
            map.Add(CreateAction(GameActions.Kick,           FightingHandToHand.ButtonKick,     0, "Kick"));
            map.Add(CreateAction(GameActions.Punch,          FightingHandToHand.ButtonPunch,    0, "Punch"));
            map.Add(CreateAction(GameActions.TheDeAppetizer, FightingHandToHand.ButtonSpecial1, 0, "\"The De-Appetizer\""));
            
            // Map the apologize button to any button on the device. directInput
            // defines several "Any-Control Constants" for mapping game actions to
            // any device object of a particular type.
            map.Add(CreateAction(GameActions.Apologize,      DInputHelper.ButtonAny(1),         0, "Apologize"));
            
            // Keyboard input mappings
            map.Add(CreateAction(GameActions.WalkLeft,       Keyboard.Left,                  0, "Walk left"));
            map.Add(CreateAction(GameActions.WalkRight,      Keyboard.Right,                 0, "Walk right"));
            map.Add(CreateAction(GameActions.Block,          Keyboard.B,                     0, "Block"));
            map.Add(CreateAction(GameActions.Kick,           Keyboard.K,                     0, "Kick"));
            map.Add(CreateAction(GameActions.Punch,          Keyboard.P,                     0, "Punch"));
            map.Add(CreateAction(GameActions.TheDeAppetizer, Keyboard.D,                     0, "\"The De-Appetizer\""));
            map.Add(CreateAction(GameActions.Apologize,      Keyboard.A,                     0, "Apologize"));
            
            // The AppFixed constant can be used to instruct directInput that the
            // current mapping can not be changed by the user.
            map.Add(CreateAction(GameActions.Quit,           Keyboard.Q, ActionAttributeFlags.AppFixed, "Quit"));
            
            // Mouse input mappings
            map.Add(CreateAction(GameActions.Walk,           Mouse.XAxis,                       0, "Walk"));
            map.Add(CreateAction(GameActions.Punch,          Mouse.Button0,                     0, "Punch"));
            map.Add(CreateAction(GameActions.Kick,           Mouse.Button1,                     0, "Kick"));
        }

        
        
        
        /// <summary>
        /// Helper method to fill the action map during initialization
        /// </summary>
        private Action CreateAction(GameActions AppData, object Semantic, object Flags, string ActionName)
        {
            Action action = new Action();

            action.ApplicationData = (int)AppData;
            action.Semantic   = (int)Semantic;
            action.Flags      = (ActionAttributeFlags)Flags;
            action.ActionName = ActionName;

            return action;
        }

        
        
        
        /// <summary>
        /// Show the native device configuration UI
        /// </summary>
        public void ConfigureDevices()
        {
            ActionFormat[] formats = { DIActionFormat };

            ConfigureDevicesParameters cdParams = new ConfigureDevicesParameters();
            cdParams.SetActionFormats(formats);
            Manager.ConfigureDevices(cdParams, ConfigureDevicesFlags.Default);
        }
    }
}
