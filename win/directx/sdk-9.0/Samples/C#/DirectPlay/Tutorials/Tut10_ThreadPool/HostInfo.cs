//----------------------------------------------------------------------------
// File: HostInfo.cs
//
// Desc: Contains the HostInfo collection item class. Objects of this class
//       are used to store information about detected hosts.
//
// Copyright (c) Microsoft Corp. All rights reserved.
//----------------------------------------------------------------------------- 
using System;
using Microsoft.DirectX.DirectPlay;

namespace Tut10_ThreadPool
{
    /// <summary>
    /// This class contains display and address information for a detected host.
    /// </summary>
    public class HostInfo
    {
        public Guid GuidInstance; // Instance Guid for the session
        public Address HostAddress; // DirectPlay address of the session host  
        public string SessionName; // Display name for the session

        /// <summary>
        /// Used by the system collection class
        /// </summary>
        public override bool Equals( object obj ) 
        { 
            HostInfo node = (HostInfo) obj;
            return GuidInstance.Equals( node.GuidInstance );
        }

        /// <summary>
        /// Used by the system collection class
        /// </summary>
        public override int GetHashCode()
        {
            return GuidInstance.GetHashCode();
        }

        /// <summary>
        /// Used by the system collection class
        /// </summary>
        public override string ToString()
        {
            string displayString = (SessionName != null) ? SessionName : "<unnamed>";
            displayString += " (" + HostAddress.GetComponentString("hostname");
            displayString += ":" + HostAddress.GetComponentInteger("port").ToString() + ")";

            return displayString;
        }
    }
}