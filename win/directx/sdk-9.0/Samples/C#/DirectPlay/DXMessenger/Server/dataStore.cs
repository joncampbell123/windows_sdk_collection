//----------------------------------------------------------------------------
// File: dataStore.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Windows.Forms;
using System.Runtime.Serialization.Formatters.Binary;
using System.Collections;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;
using DXMessenger;


namespace DXMessengerServer
{
	/// <summary>
	/// The data for each client of our application
	/// </summary>
	[Serializable]
	public class ClientData
	{
		public string name;
		public string password;
		public bool loggedin;
		public int currentId;
		public ArrayList friends = new ArrayList();

		// This is here so the serialization can work
		public ClientData() { }
		public ClientData(string n, string pwd, bool log, int id)
		{
			name = n;
			password = pwd;
			loggedin = log;
			currentId = id;
		}
	}

	/// <summary>
	/// A friend's data.  Simply the name and whether or not the user is a friend
	/// </summary>
	[Serializable]
	public class FriendData
	{
		public string friendName;
		public bool friend;

		// This is here so the serialization can work
		public FriendData() { }
		public FriendData(string n, bool f, int id)
		{
			friendName = n;
			friend = f;
		}
	}
	/// <summary>
	/// The main data store for the sample.  We will let the Binary Serializer format our data
	/// for us.  While this sample simply keeps all of the data in memory at any given time
	/// for a large scale application, where thousands of clients could be connected at once, 
	/// this type of data store would not be very useful.  In those cases, it would be better
	/// to use a data store designed for large amounts of data, for example SQL Server.
	/// </summary>
	public class DataStore
	{
		// This sample uses a binary file for it's data store.
		private string dataStoreFile = string.Empty;
		private ArrayList ourData = null;

		/// <summary>
		/// Create a new data store
		/// </summary>
		public DataStore()
		{
			// First try to find the default data store
			string mediaFolder = DXUtil.SdkMediaPath;
			dataStoreFile = mediaFolder + "dxmsgserver.dat";

			if (!System.IO.File.Exists(dataStoreFile))
			{
				// We need to create our default schema since it doesn't already exist
				this.CreateDefaultStore();
			}
			System.IO.FileStream file = null;
			try
			{
				// Well the file exists, try to load it, and throw an exception if you 
				// don't like it
				file =	System.IO.File.OpenRead(dataStoreFile);
				BinaryFormatter serial = new BinaryFormatter();
				ourData = (ArrayList)serial.Deserialize(file);
			}
			catch (Exception ee)
			{
				Console.WriteLine(ee.ToString());
				MessageBox.Show("There was an error trying to read the data store.  We will create a new store.", "No default store.", MessageBoxButtons.OK, MessageBoxIcon.Information);
				this.CreateDefaultStore();
			}
			finally
			{
				if (file != null)
					file.Close();
			}
			// No one can be logged on now, make sure everyone is marked offline
			MarkEveryoneOffline();
		}
		/// <summary>
		/// Mark this user as logged in, if they exist
		/// </summary>
		/// <param name="username">The user we are logging on</param>
		/// <param name="password">A hash of this users password</param>
		/// <returns>
		/// LogonTypes.AccountDoesNotExist if the account does not exist
		/// LogonTypes.InvalidPassword if the password hash doesn't match
		/// LogonTypes.LogonSuccess if the login was successfull
		/// </returns>
		public LogonTypes LogonUser(string username, string password)
		{
			if (!DoesUserExist(username))
				return LogonTypes.AccountDoesNotExist;

			ClientData user = GetUser(username);

			//Now check the password vs what's stored in our data
			if (user.password == password)
				// Great, this is the user, and the password matches
				return LogonTypes.LogonSuccess;
			else
				// This guy doesn't know his own password, let'em know about it
				return LogonTypes.InvalidPassword;

		}

		/// <summary>
		/// Check to see if a user exists
		/// </summary>
		/// <param name="username">Username we are checking</param>
		/// <returns>true if they exist, false otherwise</returns>
		public bool DoesUserExist(string username)
		{
			return GetUser(username) != null;
		}
		private bool DoesUserExist(int id)
		{
			return GetUser(id) != null;
		}

		/// <summary>
		/// Mark this user as now logged in
		/// </summary>
		/// <param name="username">Username we are updating</param>
		/// <param name="playerId">Current DirectPlay Player ID</param>
		public void UpdateDataToReflectLogon(string username, int playerId)
		{
			if (!DoesUserExist(username))
				throw new System.ApplicationException(); // We shouldn't hit this since the user has to be created to get here

			ClientData user = GetUser(username);
			user.currentId = playerId;
			user.loggedin = true;
		}
		/// <summary>
		/// Mark this user as now logged out
		/// </summary>
		/// <param name="username">Username we are updating</param>
		public void UpdateDataToReflectLogoff(int playerId)
		{
			if (!DoesUserExist(playerId))
				return; // We should only hit this on the case when they log on somewhere else, in which case we don't care

			ClientData user = GetUser(playerId);
			user.currentId = 0;
			user.loggedin = false;
		}
		/// <summary>
		/// Mark everyone in the data store as logged out
		/// </summary>
		private void MarkEveryoneOffline()
		{
			// Go through and mark everyone as logged off
			foreach (ClientData u in ourData)
			{
				u.currentId = 0;
				u.loggedin = false;
			}
		}
		/// <summary>
		/// Find a user based on the name (case insensitive)
		/// </summary>
		/// <param name="username">Username to search for</param>
		/// <returns>The users data requested, or null</returns>
		private ClientData GetUser(string username)
		{
			foreach (ClientData u in ourData)
			{
				if (u.name.ToLower() == username.ToLower())
					return u;
			}
			return null;
		}
		/// <summary>
		/// Find a user based on the player id
		/// </summary>
		/// <param name="userId">PlayerId to search for</param>
		/// <returns>The users data requested, or null</returns>
		private ClientData GetUser(int userId)
		{
			foreach (ClientData u in ourData)
			{
				if (u.currentId == userId)
					return u;
			}
			return null;
		}

		/// <summary>
		/// Shut down the datastore and save any data
		/// </summary>
		public void Close()
		{
			//First mark everyone offline
			this.MarkEveryoneOffline();
			// Make sure we save our structure before leaving
			this.SaveDataStore();
		}
		/// <summary>
		/// Create and save a default (empty) data store
		/// </summary>
		private void CreateDefaultStore()
		{
			// Just create a blank arraylist
			ourData = new ArrayList();
			// Now save the schema
			this.SaveDataStore();
		}
		/// <summary>
		/// Save the Datastore to a file, using the BinaryFormatter
		/// </summary>
		public void SaveDataStore()
		{
			System.IO.FileStream file = System.IO.File.OpenWrite(dataStoreFile);
			try
			{
				BinaryFormatter serial = new BinaryFormatter();
				serial.Serialize(file, ourData);
			}
			catch (Exception e)
			{
				MessageBox.Show("An unhandled exception has occured saving the data.\r\n"+ e.ToString(), "Unhandled exception");
			}
			finally
			{
				file.Close();
			}
		}
		/// <summary>
		/// Add a user to our data store
		/// </summary>
		public void AddUser(string username, string password, int currentDplayId)
		{
			// Now we can add the user to the data store
			this.AddUser(username, password, true, currentDplayId);
		}
		/// <summary>
		/// Add a user to our data store
		/// </summary>
		private void AddUser(string username, string password, bool loggedIn, int currentId)
		{
			ourData.Add(new ClientData(username, password, loggedIn, currentId));
		}
		/// <summary>
		/// Notify friends when we are online
		/// </summary>
		public void NotifyFriends(string username, MessageType msg, Server serverObject)
		{
			NetworkPacket stm = new NetworkPacket();

			// We need to check to see if the user logging in is anyones friend.
			foreach (ClientData u in ourData)
			{
				// We can skip ourself
				if (u.name.ToLower() != username.ToLower())
				{
					// This isn't me, scan through all this users friends
					foreach (FriendData f in u.friends)
					{
						// Is this friend me?
						if (f.friendName.ToLower() == username.ToLower())
						{
							// Yup, I got some friends!
							// Are they logged on?
							if (GetUser(u.name).currentId != 0)
							{
								stm.Write(msg);
								stm.Write(username);
								serverObject.SendTo(GetUser(u.name).currentId, stm, 0, 0);
								stm = new NetworkPacket();
							}
						}
					}
				}
			}
		}
		public bool AddFriend(int playerId, string friendName, bool isFriend)
		{
			// If friend is true, this user will be a friend, otherwise, we will block 
			// the user

			ClientData user = GetUser(playerId);
			ClientData friend = GetUser(friendName);

			// Check to see if this user is logged in
			int friendPlayerId = friend.currentId;

			bool foundFriend = false;
			foreach (FriendData f in user.friends)
			{
				if (f.friendName == friendName)
				{
					// we found our friend
					f.friend = isFriend;
					foundFriend = true;
				}
			}
			if (!foundFriend)
				return AddFriend(user.name, friendName, isFriend);

			return false;
		}
		private bool AddFriend(string username, string friendName, bool isFriend)
		{
			// Check to see if this user is logged in
			ClientData user = GetUser(username);
			ClientData friend = GetUser(friendName);

			int friendPlayerId = friend.currentId;
			user.friends.Add(new FriendData(friendName, isFriend, friendPlayerId));
			return (friendPlayerId != 0);
		}
		public void DeleteFriend(int playerId, string friendName)
		{
			ClientData user = GetUser(playerId);

			foreach (FriendData f in user.friends)
			{
				if (f.friendName == friendName)
				{
					user.friends.Remove(f);
					break;
				}
			}
		}
		public bool AmIBlocked(string username, string friendName)
		{
			ClientData user = GetUser(username);

			foreach (FriendData f in user.friends)
			{
				if (f.friendName == friendName)
				{
					return (f.friend == false); // Will be false if we are blocked
				}
			}
			return false;
		}

		public int GetCurrentPlayerId(string username)
		{
			ClientData user = GetUser(username);
			if (user == null)
				return 0;

			return user.currentId;
		}
		public void FindMyFriends(string username, Server serverObject)
		{
			ClientData user = GetUser(username);

			if (user.friends.Count > 0)
			{
				NetworkPacket stm = new NetworkPacket();

				// Well, we obviously have some friends, tell me all about them
				int sendId = GetCurrentPlayerId(username);
				stm.Write(MessageType.SendClientFriends);
				// How many friends are we sending?
				stm.Write(user.friends.Count);
				foreach (FriendData f in user.friends)
				{
					// Add whether they are a friend or blocked as well as the name
					stm.Write(f.friend);
					stm.Write(f.friendName);
				}
				serverObject.SendTo(sendId, stm, 0, 0);

				// Now that's done, for every friend that's online, notify me again
				foreach (FriendData f in user.friends)
				{
					if (GetUser(f.friendName).currentId != 0)
					{
						stm = new NetworkPacket();

						stm.Write(MessageType.FriendLogon);
						stm.Write(f.friendName);
						serverObject.SendTo(sendId, stm, 0, 0);
					}
				}
			}
		}
	}
}
