//----------------------------------------------------------------------------
// File: Server.cs
//
// Desc: This tutorial modifes the Send tutorial to be a client/server
//       topology instead of peer to peer. This file contains the server
//       code.
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

namespace Tut09_Server
{
    // Possible connection states
    public enum ConnectionType { Disconnected, Hosting };

    /// <summary>
    /// Application class.
    /// </summary>
    public class ServerApp : IDisposable
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
        private static readonly Guid   m_AppGuid = new Guid("1AD4CA3B-AC68-4d9b-9522-BE59CD485276"); 
        public static readonly int     DefaultPort = 2609; // Default port number

        // DirectPlay
        private Server              m_Server = null;                   // DirectPlay Server object
        private Address             m_LocalAddress = new Address();    // Local address
        
        // Application 
        private ApplicationForm     m_Form = null;                     // Main application WinForm
        private string              m_SessionName = "New Host";        // Hosted session name 
        private ConnectionType      m_Connection = ConnectionType.Disconnected; // Current connection state     
        
        //---------------------------------------------------------------------
        #endregion // Fields

        #region Properties
        //---------------------------------------------------------------------

        public ConnectionType Connection { get{ return m_Connection; } }
        
        //---------------------------------------------------------------------
        #endregion // Properties

        #region Constructors
        //---------------------------------------------------------------------

        /// <summary>
        /// Constructor
        /// </summary>
        public ServerApp()
        {
            // Initialize the application's UI
            m_Form = new ApplicationForm(this);

            // Update the UI
            UpdateUI();

            // Initialize DirectPlay and event handlers
            InitDirectPlay();

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
        }

        //---------------------------------------------------------------------
        #endregion // Constructors

        #region DirectPlay Object Initializations
        //---------------------------------------------------------------------

        /// <summary>
        /// Initializes the DirectPlay Server object
        /// </summary>
        private void InitDirectPlay()
        {
            // Release any exising resources
            if (m_Server != null)
                m_Server.Dispose();
            
            // Create a new DirectPlay Server object
            m_Server = new Server();

            // Add handlers for DirectPlay events
            m_Server.Receive += new ReceiveEventHandler(ReceiveHandler);
            
            m_Connection = ConnectionType.Disconnected;
        }

        //---------------------------------------------------------------------
        #endregion // DirectPlay Object Initializations

        #region DirectPlay Event Handlers
        //---------------------------------------------------------------------

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

        //---------------------------------------------------------------------
        #endregion // DirectPlay Event Handlers

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
            AppDesc.Flags = SessionFlags.ClientServer | SessionFlags.NoDpnServer;

            try
            {
                // Host a new session
                m_Server.Host(AppDesc,           // Application description 
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
        /// Sends the current outgoing chat string to all connected clients
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
            // packet, the DirectPlay send method can be called. You must tell
            // DirectPlay which client you wish to receive the data; for this
            // sample, we'll deliver the message to all connected players
            // (except the local player, which we exclude with the NoLoopback
            // flag)
            m_Server.SendTo((int) PlayerID.AllPlayers, // Send to session
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
            // Disconnect by closing the current server and opening
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
            ServiceProviderInformation[] SPInfoArray = m_Server.GetServiceProviders(true);

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
                    break;
                }
            }
        }

        /// <summary>
        /// Handles program cleanup
        /// </summary>
        public void Dispose()
        {
            if (m_Server != null && !m_Server.Disposed)
                m_Server.Dispose();
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main() 
        {
            // Create the main application object
            ServerApp App = new ServerApp();
            
            // Start the form's message loop
            if (!App.m_Form.IsDisposed)
                Application.Run(App.m_Form);

            // Release resources
            App.Dispose();
        }
	}
}
