//----------------------------------------------------------------------------
// File: GetServiceProviders.cs
//
// Desc: This simple program inits DirectPlay and enumerates the available
//       DirectPlay Service Providers.
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.IO;
using System.Windows.Forms;
using System.Collections;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;

namespace Tut01_GetServiceProviders
{
    /// <summary>
    /// Application class.
    /// </summary>
    public class GetServiceProvidersApp : IDisposable
    {
        #region Fields
        //---------------------------------------------------------------------
        
        // DirectPlay
        private Peer                m_Peer = null;                     // DirectPlay Peer object
        
        // Application 
        private ApplicationForm     m_Form = null;                     // Main application WinForm
        
        //---------------------------------------------------------------------
        #endregion // Fields

        #region Constructors
        //---------------------------------------------------------------------

        /// <summary>
        /// Constructor
        /// </summary>
        public GetServiceProvidersApp()
        {
            // Initialize the application's UI
            m_Form = new ApplicationForm();

            // Initialize DirectPlay
            InitDirectPlay();

            // Enumerate and list the installed service providers
            ListServiceProviders();
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
            // Create a new DirectPlay Peer object
            m_Peer = new Peer();
        }

        //---------------------------------------------------------------------
        #endregion // DirectPlay Object Initializations

        /// <summary>
        /// Query DirectPlay for the list of installed service providers on
        /// this computer.
        /// </summary>
        public void ListServiceProviders()
        {
            ServiceProviderInformation[] SPInfoArray = null;
            
            try
            {
                // Ask DirectPlay for the service provider list
                SPInfoArray = m_Peer.GetServiceProviders(true);
            }
            catch(Exception ex)
            {
                m_Form.ShowException(ex, "GetServiceProviders", true);
                m_Form.Dispose();
                return;
            }

            // For each service provider in the returned list...
            foreach (ServiceProviderInformation info in SPInfoArray)
            {
                // Add the service provider's name to the UI listbox
                m_Form.SPListBox.Items.Add(info.Name);
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
            GetServiceProvidersApp App = new GetServiceProvidersApp();
            
            // Start the form's message loop
            if (!App.m_Form.IsDisposed)
                Application.Run(App.m_Form);

            // Release resources
            App.Dispose();
        }
	}
}
