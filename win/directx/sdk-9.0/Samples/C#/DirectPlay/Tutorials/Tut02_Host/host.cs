//----------------------------------------------------------------------------
// File: Host.cs
//
// Desc: This simple program builds upon the last tutorial by adding the 
//       creation of an address object and hosting a session.
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.IO;
using System.Windows.Forms;
using System.Collections;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;

namespace Tut02_Host
{
    // Possible connection states
    public enum ConnectionType { Disconnected, Hosting };

    /// <summary>
    /// Application class.
    /// </summary>
    public class HostApp : IDisposable
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
        private static readonly Guid   m_AppGuid = new Guid("5e4ab2ee-6a50-4614-807e-c632807b5eb1"); 
        public static readonly int     DefaultPort = 2602; // Default port number

        // DirectPlay
        private Peer                m_Peer = null;                     // DirectPlay Peer object
        private Address             m_LocalAddress = new Address();    // Local address
        
        // Application 
        private ApplicationForm     m_Form = null;                     // Main application WinForm
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
        public HostApp()
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
        /// Initializes the DirectPlay Peer object
        /// </summary>
        private void InitDirectPlay()
        {
            // Release any exising resources
            if (m_Peer != null)
                m_Peer.Dispose();
            
            // Create a new DirectPlay Peer object
            m_Peer = new Peer();

            m_Connection = ConnectionType.Disconnected;
        }

        //---------------------------------------------------------------------
        #endregion // DirectPlay Object Initializations

        /// <summary>
        /// Host a new DirectPlay session.
        /// </summary>
        public void HostSession()
        {
            // Update the UI
            m_Form.SessionStatusLabel.Text = "Connecting...";
            m_Form.Update();

            // Create an application description
            ApplicationDescription AppDesc = new ApplicationDescription();
            AppDesc.GuidApplication = m_AppGuid;
            AppDesc.Flags = SessionFlags.NoDpnServer;
            
            // Set the port number on which to host
            m_LocalAddress.AddComponent("port", DefaultPort);

            try
            {
                // Host a new session
                m_Peer.Host(AppDesc,           // Application description 
                             m_LocalAddress);  // Local device address

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
            // Set UI elements based on current connection state
            switch (m_Connection)
            {
                case ConnectionType.Hosting:
                {
                    m_Form.SessionStatusLabel.Text = "Hosting a session";
                    m_Form.HostButton.Text = "&Disconnect";

                    break;
                }

                case ConnectionType.Disconnected:
                {
                    m_Form.SessionStatusLabel.Text = "Not connected to a session";
                    m_Form.HostButton.Text = "&Host";

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
            HostApp App = new HostApp();
            
            // Start the form's message loop
            if (!App.m_Form.IsDisposed)
                Application.Run(App.m_Form);

            // Release resources
            App.Dispose();
        }
	}
}
