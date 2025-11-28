//----------------------------------------------------------------------------
// File: LobbyLaunch.cs
//
// Desc: This tutorial adds lobby registration and remote launch capability
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.IO;
using System.Windows.Forms;
using System.Text;
using System.Collections;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;
using Lobby = Microsoft.DirectX.DirectPlay.Lobby;

namespace Tut07_LobbyLaunch
{
    // Possible connection states
    public enum ConnectionType { Disconnected, Hosting, Connected };

    /// <summary>
    /// Application class.
    /// </summary>
    public class LobbyLaunchApp : IDisposable
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
        private static readonly Guid   m_AppGuid = new Guid("C301FCED-E884-41a9-94D7-DE66943EC7DB"); 
        public static readonly int     DefaultPort = 2607; // Default port number

        // DirectPlay
        private Peer                m_Peer = null;                     // DirectPlay Peer object
        private Address             m_LocalAddress = new Address();    // Local address
        
        // DirectPlay Lobby
        private Lobby.Application   m_LobbyApp = null;                 // DirectPlay Lobby Application
        private int                 m_LobbyHandle = 0;                 // Lobby client handle
        
        // Application 
        private ApplicationForm     m_Form = null;                     // Main application WinForm
        private string              m_SessionName = "New Host";        // Hosted session name 
        private ArrayList           m_FoundSessions = new ArrayList(); // Enumerated sessions
        private ConnectionType      m_Connection = ConnectionType.Disconnected; // Current connection state     
        
        //---------------------------------------------------------------------
        #endregion // Fields

        #region Properties
        //---------------------------------------------------------------------

        public ConnectionType Connection { get{ return m_Connection; } }
        public ArrayList      FoundSessions { get{ return m_FoundSessions; } }

        //---------------------------------------------------------------------
        #endregion // Properties

        #region Constructors
        //---------------------------------------------------------------------

        /// <summary>
        /// Constructor
        /// </summary>
        public LobbyLaunchApp()
        {
            // Initialize the application's UI
            m_Form = new ApplicationForm(this);

            // Update the UI
            UpdateUI();

            // Initialize DirectPlay and event handlers
            InitDirectPlay();

            // Initialize DirectPlay lobby and event handlers
            InitDirectPlayLobby();

            // Set the service provider for our local address to TCP/IP
            m_LocalAddress.ServiceProvider = Address.ServiceProviderTcpIp;

            // Verify the computer supports our chosen service provider
            if (!IsServiceProviderValid(m_LocalAddress.ServiceProvider))
            {
                MessageBox.Show(m_Form, "You must have TCP/IP installed on your " + 
                    "computer to run this tutorial.",
                    "DirectPlay Tutorial", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);

                m_Form.Dispose();
            }

            // If there's an associated lobby handle then we were launched
            // from a lobby client, and should create or join a session
            // based on settings received from the lobby
            if (m_LobbyHandle != 0)
                LobbyLaunch();
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
            if (m_Peer != null)
                m_Peer.Dispose();
            
            // Create a new DirectPlay Peer object
            m_Peer = new Peer();

            // Add handlers for DirectPlay events
            m_Peer.FindHostResponse += new FindHostResponseEventHandler(FindHostResponseHandler);
            m_Peer.Receive += new ReceiveEventHandler(ReceiveHandler);
            m_Peer.HostMigrated += new HostMigratedEventHandler(HostMigratedHandler);
            m_Peer.SessionTerminated += new SessionTerminatedEventHandler(SessionTerminatedHandler);
        
            m_Connection = ConnectionType.Disconnected;
        }

        /// <summary>
        /// Initializes the DirectPlay Lobby.Application object
        /// </summary>
        private void InitDirectPlayLobby()
        {
            // Initialize the Lobby application object
            m_LobbyApp = new Lobby.Application(out m_LobbyHandle);

            // Add handlers for DirectPlay Lobby events
            m_LobbyApp.Connect += new Lobby.ConnectEventHandler(LobbyConnectHandler);   
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
            if (!FoundSessions.Contains(Node))
                FoundSessions.Add(Node);
        }

        /// <summary>
        /// Handler for incoming DirectPlay Receive events
        /// </summary>
        public void ReceiveHandler(object sender, ReceiveEventArgs args)
        {
            if (m_Form.ReceivedMessagesListBox.Enabled)
            {
                // Read the incoming chat message. Since network data is always
                // broken down into raw bytes for transfer, the sent object must
                // be rebuilt. The NetworkPacket class contains methods to 
                // easily send and receive unicode strings, but this sample
                // manually decodes the string for compatibility with the C++
                // tutorials. The data is first read into a byte array, then Unicode 
                // decoded, and finally added to our list of received messages.
                NetworkPacket packet = args.Message.ReceiveData;
                byte[] data = (byte[]) packet.Read(typeof(byte), packet.Length);
                m_Form.ReceivedMessagesListBox.Items.Add(Encoding.Unicode.GetString(data));
            }
        }

        /// <summary>
        /// Handler for incoming DirectPlay HostMigrated events
        /// </summary>
        public void HostMigratedHandler(object sender, HostMigratedEventArgs args)
        {
            PlayerInformation info = m_Peer.GetPeerInformation(args.Message.NewHostID);
            
            // See if we are the new host
            if (info.Local)
            {
                m_Connection = ConnectionType.Hosting;
                UpdateUI();
            }
        }

        /// <summary>
        /// Handler for incoming DirectPlay SessionTerminated events
        /// </summary>
        public void SessionTerminatedHandler(object sender, SessionTerminatedEventArgs args)
        {
            MessageBox.Show(m_Form, "Connect lost or host terminated session", "DirectPlay Tutorial");
            m_Connection = ConnectionType.Disconnected;
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
                m_LobbyHandle = args.Message.ConnectionHandle;
                m_Peer.RegisterLobby(m_LobbyHandle, m_LobbyApp);
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, "RegisterLobby", true);
                m_Form.Dispose();
                return;
            }
        }

        //---------------------------------------------------------------------
        #endregion // DirectPlay Lobby Event Handlers
        
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
                settings = m_LobbyApp.GetConnectionSettings(m_LobbyHandle);
                m_SessionName = settings.ApplicationDescriptionChanged.SessionName;

                // If host flag is set, this application should create a new session
                if (0 != (settings.Flags & Lobby.ConnectionSettingsFlags.Host))
                {
                    // Host a new session
                    m_Peer.Host(settings.ApplicationDescriptionChanged, // Application description
                                 settings.GetDeviceAddresses());             // Local device addresses

                   m_Connection = ConnectionType.Hosting;
                }
                else
                {
                    // Connect to an existing session
                    m_Peer.Connect(settings.ApplicationDescriptionChanged, // Application description
                                    settings.HostAddress,                   // Host address
                                    settings.GetDeviceAddresses()[ 0 ],          // Local device address
                                    null,                                   // Async handle                                   
                                    ConnectFlags.Sync);                    // Connect flags

                    m_Connection = ConnectionType.Connected;
                }
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, null, true);
                m_Form.Dispose();
                return;
            }

            UpdateUI();
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
            desc.ApplicationName = m_Form.Text;
            desc.GuidApplication = m_AppGuid;

            int index = Application.ExecutablePath.LastIndexOf('\\');
            desc.ExecutablePath = Application.ExecutablePath.Substring(0, index);
            desc.ExecutableFilename = Application.ExecutablePath.Substring(index+1);

            try
            {
                // Register the program
                m_LobbyApp.RegisterProgram(desc);
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, "RegisterProgram", false);
                return;
            }

            MessageBox.Show(m_Form, "Successfully registered lobby support.", "DirectPlay Tutorial");
        }

        /// <summary>
        /// Remove this application's lobby information from the system registry
        /// </summary>
        public void Unregister()
        {
            try 
            {
                m_LobbyApp.UnregisterProgram(m_AppGuid);
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, "UnregisterProgram", false);
                return;
            }

            MessageBox.Show(m_Form, "Application lobby information removed.", "DirectPlay Tutorial");
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
            m_Form.SessionStatusLabel.Text = "Connecting...";
            m_Form.Update();

            // Store host dialog choices
            m_SessionName = dialog.SessionName;
            
            // Add the port number
            if (dialog.LocalPort > 0)
                m_LocalAddress.AddComponent("port", dialog.LocalPort);
            
            // Create an application description
            ApplicationDescription AppDesc = new ApplicationDescription();
            AppDesc.GuidApplication = m_AppGuid;
            AppDesc.SessionName = m_SessionName;
            AppDesc.Flags = SessionFlags.NoDpnServer;

            // Enable host migration
            if (dialog.IsHostMigrationEnabled)
                AppDesc.Flags |= SessionFlags.MigrateHost;

            try
            {
                // Host a new session
                m_Peer.Host(AppDesc,           // Application description 
                    m_LocalAddress);   // Local device address

                m_Connection = ConnectionType.Hosting;
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, "Host", true);
                m_Form.Dispose();
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
            AppDesc.GuidApplication = m_AppGuid;

            // Find all sessions hosted at the given address. When a session is
            // found, DirectPlay calls our FindHostResponse delegate. Since we're
            // passing in the "Sync" flag, Connect will block until the search
            // timeout has expired.
            try
            {
                m_Peer.FindHosts(AppDesc,               // Application description
                    HostAddress,           // Host address
                    m_LocalAddress,        // Local device address
                    null,                  // Enumeration data
                    0,                     // Enumeration count (using default)
                    0,                     // Retry interval (using default)
                    0,                     // Timeout (using default)
                    FindHostsFlags.Sync); // Flags
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, "FindHosts", true);
                m_Form.Dispose();
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
            m_Form.SessionStatusLabel.Text = "Connecting...";
            m_Form.Update();

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
                m_Peer.Connect(appDesc,                  // Application description
                    SelectedHost.HostAddress, // Host address
                    m_LocalAddress,           // Local device address
                    null,                     // User connection data (none)  
                    ConnectFlags.Sync);      // Flags

                m_SessionName = SelectedHost.SessionName;
                m_Connection = ConnectionType.Connected;
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, "Connect", false);
                return;
            }
        }

        
        /// <summary>
        /// Sends the current outgoing chat string to all connected peers
        /// </summary>
        public void SendData()
        {
            // Create a network packet object to which we can write our chat message.
            // For compatibility with the current C++ tutorials, the unicode text
            // will be encoded as a null-terminated string for network transfer, as 
            // opposed to the .NET convention of string length followed by characters; 
            // however, there are no limits on how the data can be formatted since 
            // all data is received as a raw byte array.
            NetworkPacket packet = new NetworkPacket();
            packet.Write(Encoding.Unicode.GetBytes(m_Form.SendTextBox.Text));
            
            // Now that all the outgoing data has been encoded to a network
            // packet, the DirectPlay send method can be called. Since in a
            // peer-to-peer topology there is no server, you must inform
            // DirectPlay which peer you wish to receive the data. For this
            // sample, we'll deliver the message to all connected players
            // (except the local player, which we exclude with the NoLoopback
            // flag)
            m_Peer.SendTo((int) PlayerID.AllPlayers, // Send to session
                           packet,                 // Outgoing data
                           0,                         // Timeout (default)        
                           SendFlags.Sync |           // Flags
                           SendFlags.NoLoopback);  

            m_Form.SendTextBox.Text = "";
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
            ServiceProviderInformation[] SPInfoArray = m_Peer.GetServiceProviders(true);

            // For each service provider in the returned list...
            foreach (ServiceProviderInformation info in SPInfoArray)
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
            // Flush the incoming/outgoing messages box
            m_Form.SendTextBox.Text = "";
            m_Form.ReceivedMessagesListBox.Items.Clear();

            // Set UI elements based on current connection state
            switch (m_Connection)
            {
                case ConnectionType.Hosting:
                {
                    m_Form.SessionStatusLabel.Text = "Hosting session \"" + m_SessionName + "\"";
                    m_Form.HostButton.Text = "&Disconnect";

                    m_Form.ReceivedMessagesListBox.Enabled = true;
                    m_Form.SendTextBox.Enabled = true;
                    m_Form.SendButton.Enabled =  true;
                    m_Form.ConnectButton.Enabled = false;
                    break;
                }

                case ConnectionType.Connected:
                {
                    m_Form.SessionStatusLabel.Text = "Connected to session \"" + m_SessionName + "\"";
                    m_Form.HostButton.Text = "&Disconnect";

                    m_Form.ReceivedMessagesListBox.Enabled = true;
                    m_Form.SendTextBox.Enabled = true;
                    m_Form.SendButton.Enabled =  true;
                    m_Form.ConnectButton.Enabled = false;
                    break;
                }

                case ConnectionType.Disconnected:
                {
                    m_Form.SessionStatusLabel.Text = "Not connected to a session";
                    m_Form.HostButton.Text = "&Host...";
                    m_Form.ReceivedMessagesListBox.Items.Add("You must first connect to a session.");

                    m_Form.ReceivedMessagesListBox.Enabled = false;
                    m_Form.SendTextBox.Enabled = false;
                    m_Form.SendButton.Enabled =  false;
                    m_Form.ConnectButton.Enabled = true;
                    break;
                }
            }
        }

        /// <summary>
        /// Handles program cleanup
        /// </summary>
        public void Dispose()
        {
            if (m_Peer != null && !m_Peer.Disposed)
                m_Peer.Dispose();
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {
            // Create the main application object
            LobbyLaunchApp App = new LobbyLaunchApp();
            
            // Start the form's message loop
            if (!App.m_Form.IsDisposed)
                Application.Run(App.m_Form);

            // Release resources
            App.Dispose();
        }
	}
}
