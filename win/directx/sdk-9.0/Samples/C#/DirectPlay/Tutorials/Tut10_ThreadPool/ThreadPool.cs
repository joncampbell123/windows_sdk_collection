//----------------------------------------------------------------------------
// File: ThreadPool.cs
//
// Desc: This tutorial uses the IDirectPlay8ThreadPool interface to control
//       when and for how long the DirectPlay worker threads are allowed to 
//       run.
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using System.Text;
using System.Collections;
using System.Threading;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;
using Lobby = Microsoft.DirectX.DirectPlay.Lobby;
using DirectPlay = Microsoft.DirectX.DirectPlay;

namespace Tut10_ThreadPool
{
    // Possible connection states
    public enum ConnectionType { Disconnected, Hosting, Connected };

    // Describes a single paint point on the canvas
    public struct PaintPoint
    {
        public int X;
        public int Y;
        public int Argb;
    }

    // Message format for outgoing paint messages
    public struct GameMsgPaint
    {
        public byte Type;
        public int NumPoints;
        public PaintPoint[] Points;
    }




    /// <summary>
    /// Application class.
    /// </summary>
    public class ThreadPoolApp : IDisposable
    {
        #region Fields
        //---------------------------------------------------------------------
        
        // Application identifier
        #region About Application Guids
        // This guid allows DirectPlay to find other instances of the same game on
        // the network, so it must be unique for every game, and the same for 
        // every instance of that game. You can use the guidgen.exe program to
        // generate a new guid.
        #endregion
        private static readonly Guid appGuid = new Guid("C301FCED-E884-41a9-94D7-DE66943EC7DB"); 
        public static readonly int   DefaultPort = 2610; // Default port number

        // Network messages
        public static readonly int SendBufferSize = 100; // Size of send queue
        public const byte GameMsgidPaint = 1; // Paint message ID
        public const byte GameMsgidClear = 2; // Clear message ID
       
        // DirectPlay
        private Peer myPeer = null; // DirectPlay Peer object
        private Address localAddress = new Address(); // Local address
        
        // DirectPlay ThreadPool
        private DirectPlay.ThreadPool myThreadPool = null; // DirectPlay ThreadPool
        
        // DirectPlay Lobby
        private Lobby.Application myLobbyApp = null; // DirectPlay Lobby Application
        private int myLobbyHandle = 0; // Lobby client handle
        
        // Application 
        private ApplicationForm myForm = null; // Main application WinForm
        private string sessionName = "New Host"; // Hosted session name 
        private ArrayList myFoundSessions = new ArrayList(); // Enumerated sessions
        private ConnectionType myConnection = ConnectionType.Disconnected; // Current connection state     
        private GameMsgPaint sendBuffer = new GameMsgPaint(); // Send buffer
        private Thread gameThread; // Game loop thread
        private bool isGameRunning = true; // Active game flag
        
        //---------------------------------------------------------------------
        #endregion // Fields

        #region Properties
        //---------------------------------------------------------------------

        public ConnectionType Connection { get{ return myConnection; } }
        public ArrayList FoundSessions { get{ return myFoundSessions; } }

        //---------------------------------------------------------------------
        #endregion // Properties

        #region Constructors
        //---------------------------------------------------------------------

        /// <summary>
        /// Constructor
        /// </summary>
        public ThreadPoolApp()
        {
            // Initialize the application's UI
            myForm = new ApplicationForm(this);

            // Initialize the send buffer
            sendBuffer.Points = new PaintPoint[ThreadPoolApp.SendBufferSize];

            // Update the UI
            UpdateUI();

            // Initialize DirectPlay and event handlers
            InitDirectPlay();

            // Initialize DirectPlay lobby and event handlers
            InitDirectPlayLobby();

            // Set the service provider for our local address to TCP/IP
            localAddress.ServiceProvider = Address.ServiceProviderTcpIp;

            // Verify the computer supports our chosen service provider
            if (!IsServiceProviderValid(localAddress.ServiceProvider))
            {
                MessageBox.Show(myForm, "You must have TCP/IP installed on your " + 
                                        "computer to run this tutorial.",
                                        "DirectPlay Tutorial", 
                                        MessageBoxButtons.OK, MessageBoxIcon.Error);

                myForm.Dispose();
            }

            // If there's an associated lobby handle then we were launched
            // from a lobby client, and should create or join a session
            // based on settings received from the lobby
            if (myLobbyHandle != 0)
                LobbyLaunch();

            // Start the game loop
            gameThread = new Thread(new ThreadStart(RunGameLoop));
            gameThread.Start();
        }

        //---------------------------------------------------------------------
        #endregion // Constructors

        #region DirectPlay Object Initializations
        //---------------------------------------------------------------------

        /// <summary>
        /// Initializes the DirectPlay Peer object
        /// </summary>
        private void InitDirectPlay()
        {
            // Release any exising resources
            if (myPeer != null)
            {
                myPeer.CancelAsyncOperation(CancelFlags.AllOperations);
                myPeer.Dispose();
            }
            
            if(myThreadPool != null)
                myThreadPool.Dispose();

            // Create the ThreadPool
            myThreadPool = new DirectPlay.ThreadPool();

            // Create a new DirectPlay Peer object
            myPeer = new Peer();

            // Add handlers for DirectPlay events
            myPeer.FindHostResponse += new FindHostResponseEventHandler(FindHostResponseHandler);
            myPeer.Receive += new ReceiveEventHandler(ReceiveHandler);
            myPeer.HostMigrated += new HostMigratedEventHandler(HostMigratedHandler);
            myPeer.SessionTerminated += new SessionTerminatedEventHandler(SessionTerminatedHandler);
        
            // Turn off worker DirectPlay worker threads since we'll be using the DoWork
            // method to synchronously handle network messages. 
            myThreadPool.SetThreadCount(-1, 0);

            myConnection = ConnectionType.Disconnected;
        }




        /// <summary>
        /// Initializes the DirectPlay Lobby.Application object
        /// </summary>
        private void InitDirectPlayLobby()
        {
            // Initialize the Lobby application object
            myLobbyApp = new Lobby.Application(out myLobbyHandle);

            // Add handlers for DirectPlay Lobby events
            myLobbyApp.Connect += new Lobby.ConnectEventHandler(LobbyConnectHandler);   
        }

        //---------------------------------------------------------------------
        #endregion // DirectPlay Object Initializations

        #region DirectPlay Event Handlers
        //---------------------------------------------------------------------

        /// <summary>
        /// Event handler for the DirectPlay FindHostResponse message
        /// </summary>
        public void FindHostResponseHandler(object sender, FindHostResponseEventArgs args)
        {
            HostInfo Node = new HostInfo();
            Node.GuidInstance  = args.Message.ApplicationDescription.GuidInstance;
            Node.HostAddress   = (Address) args.Message.AddressSender.Clone();
            Node.SessionName   = args.Message.ApplicationDescription.SessionName;

            // If he haven't already seen this host, add the detected session 
            // to the stored list
            if (!myFoundSessions.Contains(Node))
                myFoundSessions.Add(Node);
        }




        /// <summary>
        /// Handler for incoming DirectPlay Receive events
        /// </summary>
        public void ReceiveHandler(object sender, ReceiveEventArgs args)
        {
            NetworkPacket packet = args.Message.ReceiveData;
            
            // Extract the data
            byte type = (byte)packet.Read(typeof(byte));

            switch(type)
            {
                case GameMsgidPaint:
                {
                    int numPoints = (int)packet.Read(typeof(int));
                    for (int i=0; i < numPoints; i++)
                    {
                        PaintPoint point = (PaintPoint)packet.Read(typeof(PaintPoint));

                        unchecked
                        {
                            // Messages are sent in XRGB format since transparency is ignored, so set
                            // this pixel to be opaque before calling the paint routine
                            myForm.DrawPoint(point.X, point.Y, Color.FromArgb((int)0xff000000 | point.Argb));
                        }
                    }
                    break;
                }
            }

        }




        /// <summary>
        /// Handler for incoming DirectPlay HostMigrated events
        /// </summary>
        public void HostMigratedHandler(object sender, HostMigratedEventArgs args)
        {
            PlayerInformation info = myPeer.GetPeerInformation(args.Message.NewHostID);
            
            // See if we are the new host
            if (info.Local)
            {
                myConnection = ConnectionType.Hosting;
                UpdateUI();
            }
        }




        /// <summary>
        /// Handler for incoming DirectPlay SessionTerminated events
        /// </summary>
        public void SessionTerminatedHandler(object sender, SessionTerminatedEventArgs args)
        {
            MessageBox.Show(myForm, "Connect lost or host terminated session", "DirectPlay Tutorial");
            myConnection = ConnectionType.Disconnected;
            UpdateUI();
        }

        //---------------------------------------------------------------------
        #endregion // DirectPlay Event Handlers

        #region DirectPlay Lobby Event Handlers

        /// <summary>
        /// Handler for LobbyConnectEvent. By registering with the lobby which triggered
        /// this event, we can continue to receive notifications from this lobby client.
        /// </summary>
        public void LobbyConnectHandler(object sender, Lobby.ConnectEventArgs args)
        {
            try 
            {
                // Register with the DirectPlay lobby so we'll receive automatic notifications
                myLobbyHandle = args.Message.ConnectionHandle;
                myPeer.RegisterLobby(myLobbyHandle, myLobbyApp);
            }
            catch(Exception ex)
            {
                myForm.ShowException(ex, "RegisterLobby", true);
                myForm.Dispose();
                return;
            }
        }

        //---------------------------------------------------------------------
        #endregion // DirectPlay Lobby Event Handlers
        
        /// <summary>
        /// Run the game loop
        /// </summary>
        public void RunGameLoop()
        {
            while (isGameRunning)
            {
                // Send outgoing network data
                if (myConnection != ConnectionType.Disconnected)
                    SendData();
                      
                try
                {
                    // Handle incoming network data
                    // For this tutorial, we'll set the allowed timeslice at 100 
                    // millesconds, which should give plenty of time for DirectPlay
                    // to read the network data without holding up the next rendering
                    myThreadPool.DoWork(100);
                }
                catch
                {
                }

                // Render the scene
                myForm.RenderCanvas();

                // Yield some time to prevent starvation
                Thread.Sleep(10);
            }
        }




        /// <summary>
        /// Host or connect to a session based on the settings provided by the lobby
        /// </summary>
        public void LobbyLaunch()
        {
            // Create an object to retrieve the connection settings from the lobby client
            Lobby.ConnectionSettings settings;

            try 
            {
                // Get settings
                settings = myLobbyApp.GetConnectionSettings(myLobbyHandle);
                sessionName = settings.ApplicationDescriptionChanged.SessionName;

                // If host flag is set, this application should create a new session
                if (0 != (settings.Flags & Lobby.ConnectionSettingsFlags.Host))
                {
                    // Host a new session
                    myPeer.Host(settings.ApplicationDescriptionChanged, // Application description
                                settings.GetDeviceAddresses()); // Local device addresses

                   myConnection = ConnectionType.Hosting;
                }
                else
                {
                    // Connect to an existing session
                    myPeer.Connect(settings.ApplicationDescriptionChanged, // Application description
                                   settings.HostAddress, // Host address
                                   settings.GetDeviceAddresses()[ 0 ], // Local device address
                                   null, // Async handle                                   
                                   ConnectFlags.Sync); // Connect flags

                    myConnection = ConnectionType.Connected;
                }
            }
            catch(Exception ex)
            {
                myForm.ShowException(ex, null, true);
                myForm.Dispose();
                return;
            }

            UpdateUI();
        }



        /// <summary>
        /// Add the given point to the canvas bitmap
        /// </summary>
        public void AddPoint(int x, int y, Color color)
        {
            // Add this point to the send buffer
            if (sendBuffer.NumPoints < SendBufferSize)
            {
                sendBuffer.Points[sendBuffer.NumPoints].X = x;
                sendBuffer.Points[sendBuffer.NumPoints].Y = y;
                sendBuffer.Points[sendBuffer.NumPoints].Argb = color.ToArgb();
                sendBuffer.NumPoints++;
            }
        }




        /// <summary>
        /// Register this program for lobby access. 
        /// </summary>
        public void Register()
        {
            // Fill in a new ProgramDescription for this application. The information
            // in these fields will be entered into the system registry so lobby
            // clients can find and launch instances of this program.
            Lobby.ProgramDescription desc = new Lobby.ProgramDescription();
            desc.ApplicationName = myForm.Text;
            desc.GuidApplication = appGuid;

            int index = Application.ExecutablePath.LastIndexOf('\\');
            desc.ExecutablePath = Application.ExecutablePath.Substring(0, index);
            desc.ExecutableFilename = Application.ExecutablePath.Substring(index+1);

            try
            {
                // Register the program
                myLobbyApp.RegisterProgram(desc);
            }
            catch(Exception ex)
            {
                myForm.ShowException(ex, "RegisterProgram", false);
                return;
            }

            MessageBox.Show(myForm, "Successfully registered lobby support.", "DirectPlay Tutorial");
        }




        /// <summary>
        /// Remove this application's lobby information from the system registry
        /// </summary>
        public void Unregister()
        {
            try 
            {
                myLobbyApp.UnregisterProgram(appGuid);
            }
            catch(Exception ex)
            {
                myForm.ShowException(ex, "UnregisterProgram", false);
                return;
            }

            MessageBox.Show(myForm, "Application lobby information removed.", "DirectPlay Tutorial");
        }




        /// <summary>
        /// Host a new DirectPlay session.
        /// </summary>
        public void HostSession()
        {
            // Get desired hosting options
            HostDialog dialog = new HostDialog();
            dialog.LocalPort = DefaultPort;

            if (DialogResult.Cancel == dialog.ShowDialog())
                return;

            // Update the UI
            myForm.SessionStatusLabel.Text = "Connecting...";
            myForm.Update();

            // Store host dialog choices
            sessionName = dialog.SessionName;
            
            // Add the port number
            if (dialog.LocalPort > 0)
                localAddress.AddComponent("port", dialog.LocalPort);
            
            // Create an application description
            ApplicationDescription AppDesc = new ApplicationDescription();
            AppDesc.GuidApplication = appGuid;
            AppDesc.SessionName = sessionName;
            AppDesc.Flags = SessionFlags.NoDpnServer;

            // Enable host migration
            if (dialog.IsHostMigrationEnabled)
                AppDesc.Flags |= SessionFlags.MigrateHost;

            try
            {
                // Host a new session
                myPeer.Host(AppDesc,           // Application description 
                    localAddress);   // Local device address

                myConnection = ConnectionType.Hosting;
            }
            catch(Exception ex)
            {
                myForm.ShowException(ex, "Host", true);
                myForm.Dispose();
                return;
            }
        }

        /// <summary>
        /// Find all sessions at the given host address
        /// </summary>
        /// <param name="hostname">IP address or hostname to search</param>
        /// <param name="port">Remote port to search</param>
        public void EnumerateSessions(string hostname, int port)
        {
            // Set the desired search options
            Address HostAddress = new Address();
            HostAddress.ServiceProvider = Address.ServiceProviderTcpIp;
            
            if (hostname.Length > 0)
                HostAddress.AddComponent("hostname", hostname);
            
            if (port > 0)
                HostAddress.AddComponent("port", port);

            ApplicationDescription AppDesc = new ApplicationDescription();
            AppDesc.GuidApplication = appGuid;

            // Find all sessions hosted at the given address. When a session is
            // found, DirectPlay calls our FindHostResponse delegate. Since we're
            // passing in the "Sync" flag, Connect will block until the search
            // timeout has expired.
            try
            {
                myPeer.FindHosts(AppDesc,               // Application description
                    HostAddress,           // Host address
                    localAddress,        // Local device address
                    null,                  // Enumeration data
                    0,                     // Enumeration count (using default)
                    0,                     // Retry interval (using default)
                    0,                     // Timeout (using default)
                    FindHostsFlags.Sync); // Flags
            }
            catch(Exception ex)
            {
                myForm.ShowException(ex, "FindHosts", true);
                myForm.Dispose();
                return;
            }
        }

        
        /// <summary>
        /// Connect to the currently selected session
        /// </summary>
        public void ConnectToSession()
        {
            // Display connection dialog
            ConnectDialog dialog = new ConnectDialog(this);
            dialog.RemotePort = DefaultPort;

            if (DialogResult.Cancel == dialog.ShowDialog())
                return;

            // Update the UI
            myForm.SessionStatusLabel.Text = "Connecting...";
            myForm.Update();

            // Store connection dialog choices
            HostInfo SelectedHost = dialog.SelectedHost;
            if (SelectedHost == null)
                return;
            
            // Create an application description object to hold the desired
            // host's instance guid.
            ApplicationDescription appDesc = new ApplicationDescription();
            appDesc.GuidInstance = SelectedHost.GuidInstance;

            // Attempt to connect to the selected DirectPlay session. Once we
            // are connected to the session, we will receive DirectPlay messages
            // about session events (like players joining/exiting and received game 
            // data). Since we're passing in the "Sync" flag, the Connect call will
            // block until the session is connected or the timeout expires.
            try 
            {
                myPeer.Connect(appDesc,       // Application description
                    SelectedHost.HostAddress, // Host address
                    localAddress,           // Local device address
                    null,                     // User connection data (none)  
                    ConnectFlags.Sync);       // Flags

                sessionName = SelectedHost.SessionName;
                myConnection = ConnectionType.Connected;
            }
            catch(Exception ex)
            {
                myForm.ShowException(ex, "Connect", false);
                return;
            }
        }

        


        /// <summary>
        /// Sends the current outgoing chat string to all connected peers
        /// </summary>
        public void SendData()
        {
            // Create a network packet object to which we can send our painting
            // data.
            NetworkPacket packet = new NetworkPacket();
            
            // Add the raw data
            packet.Write(GameMsgidPaint);
            packet.Write(sendBuffer.NumPoints);
            for (int i=0; i < sendBuffer.NumPoints; i++)
            {
                packet.Write(sendBuffer.Points[i]);
            }
            
            // Flush the send queue
            sendBuffer.NumPoints = 0;

            try
            {
                // Now that all the outgoing data has been encoded to a network
                // packet, the DirectPlay send method can be called. Since in a
                // peer-to-peer topology there is no server, you must inform
                // DirectPlay which peer you wish to receive the data. For this
                // sample, we'll deliver the message to all connected players
                // (except the local player, which we exclude with the NoLoopback
                // flag)
                myPeer.SendTo((int) PlayerID.AllPlayers, // Send to session
                              packet, // Outgoing data
                              0, // Timeout (default)        
                              SendFlags.Sync | // Flags
                              SendFlags.NoLoopback);  
            }
            catch
            {
            }
        }




        /// <summary>
        /// Terminate or disconnect from the session.
        /// </summary>
        public void Disconnect()
        {
            // Disconnect by closing the current peer and opening
            // a new one.
            InitDirectPlay();
            UpdateUI();
        }




        /// <summary>
        /// Verify the given service provider if found in the enumerated list
        /// of installed providers on this computer.
        /// </summary>
        /// <param name="provider">Guid of the provider to search for</param>
        /// <returns>true if the given provider is found</returns>
        private bool IsServiceProviderValid(Guid provider)
        {
            // Ask DirectPlay for the service provider list
            ServiceProviderInformation[] SPInfoArray = myPeer.GetServiceProviders(true);

            // For each service provider in the returned list...
            foreach(ServiceProviderInformation info in SPInfoArray)
            {
                // Compare the current provider against the passed provider
                if (info.Guid == provider)
                    return true;
            }

            // Not found
            return false;
        }




        /// <summary>
        /// Update visual elements according to current state
        /// </summary>
        public void UpdateUI()
        {
            
            // Set UI elements based on current connection state
            switch(myConnection)
            {
                case ConnectionType.Hosting:
                {
                    myForm.SessionStatusLabel.Text = "Hosting session \"" + sessionName + "\"";
                    myForm.HostButton.Text = "&Disconnect";

                    myForm.ConnectButton.Enabled = false;
                    break;
                }

                case ConnectionType.Connected:
                {
                    myForm.SessionStatusLabel.Text = "Connected to session \"" + sessionName + "\"";
                    myForm.HostButton.Text = "&Disconnect";

                    myForm.ConnectButton.Enabled = false;
                    break;
                }

                case ConnectionType.Disconnected:
                {
                    myForm.SessionStatusLabel.Text = "Not connected to a session";
                    myForm.HostButton.Text = "&Host...";
                   
                    myForm.ConnectButton.Enabled = true;
                    break;
                }
            }
        }




        /// <summary>
        /// Handles program cleanup
        /// </summary>
        public void Dispose()
        {
            // Signal the game end
            isGameRunning = false;
            gameThread.Join();
                
            // Release resources
            if (myPeer != null && !myPeer.Disposed)
                myPeer.Dispose();

            if (myThreadPool != null && !myThreadPool.Disposed)
                myThreadPool.Dispose();
        }




        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {
            // Create the main application object
            ThreadPoolApp App = new ThreadPoolApp();
            
            // Start the form's message loop
            if (!App.myForm.IsDisposed)
                Application.Run(App.myForm);

            // Release resources
            App.Dispose();
        }
	}
}
