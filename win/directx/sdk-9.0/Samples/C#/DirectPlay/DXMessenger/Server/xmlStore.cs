//----------------------------------------------------------------------------
// File: xmlStore.cs
//
// Copyright (c) Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Xml;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;
using DXMessenger;


namespace DXMessengerServer
{
// The schema has the following form:
//	<DirectXMessengerServiceSchema>
//	<ClientInfo>
//	<Row>
//	<ClientName> UserName </ClientName>
//	<ClientPassword> EncryptedPassword </ClientPassword>
//	<LoginState> BooleanDefiningLoginState </LoginState>
//	<UserId>CurrentDPlayID or 0 if not logged in </UserId>
//	</Row>
//	</ClientInfo>
//	<FriendList>
//	<Row>
//	<ClientName> UserName </ClientName>
//	<FriendName> Name of Friend </FriendName>
//	<FriendOrBlocked> Boolean, True if Friend, False if Blocked </FriendOrBlocked>
//	</Row>
//	</FriendList>
//	</DirectXMessengerServiceSchema>
	public class XmlDataStore
	{
		// This sample uses an Xml schema for it's data store.

		// While fine for the small scale application such as this sample, in a large 
		// scale environment where you could have thousands of clients connected
		// you would want a more robust data store, such as Sql Server.
		#region XML Schema constants
		private const string gSchemaRootName = "DirectXMessengerServiceSchema";
		private const string gClientRootName = "ClientInfo";
		private const string gFriendRootName = "FriendList";

		private const string gRowName = "Row";
		private const string gClientName = "ClientName";
		private const string gClientPassword = "ClientPassword";
		private const string gLoginState = "LoginState";
		private const string gUserId = "UserId";

		private const string gFriendName = "FriendName";
		private const string gFriend = "FriendOrBlocked";
		#endregion

		private struct FriendType
		{
			public string friendName;
			public bool friend;
			public int friendId;
		}
		private XmlDocument xmlDom = null;
		private string xmlDocumentLocation = null;

		public XmlDataStore()
		{
			// First try to find the default XML Schema
			string mediaFolder = DXUtil.GetDXSDKMediaPath();
			xmlDocumentLocation = mediaFolder + "dxmsgserver.xml";

			xmlDom = new XmlDocument();
			if (!System.IO.File.Exists(xmlDocumentLocation))
			{
				// We need to create our default schema
				MessageBox.Show("The default XML data structure could not be found.  We will create a new one.", "No default schema.", MessageBoxButtons.OK, MessageBoxIcon.Information);
				this.CreateDefaultSchema();
			}
			try
			{
				// Well the file exists, try to load it, and throw an exception if you 
				// don't like it
				xmlDom.Load(xmlDocumentLocation);
				if (xmlDom.ChildNodes.Count < 1)
					throw new Exception();
			}
			catch
			{
				MessageBox.Show("There was an error trying to read the XML data.  We will create a new schema.", "No default schema.", MessageBoxButtons.OK, MessageBoxIcon.Information);
				this.CreateDefaultSchema();
			}
			// No one can be logged on now, make sure everyone is marked offline
			MarkEveryoneOffline();
		}
		public LogonTypes LogonUser(string username, string password)
		{
			XmlNode userNode = GetUserNode(username);
			if (userNode == null)
				return LogonTypes.AccountDoesNotExist;

			// Now we know this user exists. Decrypt the password sent from the client
			string decrypt = MessengerShared.EncodePassword(password, MessengerShared.ClientSideEncryptionKey);
			//Now check the password vs what's stored in our xml node
			if (userNode.NextSibling.FirstChild.Value == MessengerShared.EncodePassword(decrypt, MessengerShared.ServerSideEncryptionKey))
				// Great, this is the user, and the password matches
				return LogonTypes.LogonSuccess;
			else
				// This guy doesn't know his own password, let'em know about it
				return LogonTypes.InvalidPassword;

		}
		public void UpdateSchemaToReflectLogon(string username, int playerId)
		{
			XmlNode userNode = GetUserNode(username);
			if (userNode == null)
				return; // We shouldn't hit this since the user has to be created to get here

			foreach (XmlNode n in userNode.ParentNode.ChildNodes)
			{
				if (n.Name == gLoginState)
					n.FirstChild.Value = true.ToString();

				if (n.Name == gUserId)
					n.FirstChild.Value = playerId.ToString();
			}
		}
		public void UpdateSchemaToReflectLogoff(int playerId)
		{
			XmlNode userNode = GetUserNodeFromId(playerId);
			if (userNode == null)
				return; // We shouldn't hit this since the user has to be created to get here

			foreach (XmlNode n in userNode.ParentNode.ChildNodes)
			{
				if (n.Name == gLoginState)
					n.FirstChild.Value = false.ToString();

				if (n.Name == gUserId)
					n.FirstChild.Value = "0";
			}
		}
		private void MarkEveryoneOffline()
		{
			// Go through every node and mark everyone as logged off
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gClientRootName + "/" + gRowName + "/" + gClientName))
			{
				n.NextSibling.NextSibling.FirstChild.Value = false.ToString();
				n.NextSibling.NextSibling.NextSibling.FirstChild.Value = "0";
			}
		}
		private XmlNode GetUserNode(string username)
		{
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gClientRootName + "/" + gRowName + "/" + gClientName))
			{
				// Is this the user?  First child node will hold the ClientName text
				if (n.FirstChild.Value.ToLower() == username.ToLower())
					return n;
			}
			return null;
		}
		private XmlNode GetUserNodeFromId(int userId)
		{
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gClientRootName + "/" + gRowName + "/" + gUserId))
			{
				// Is this the user?  First child node will hold the ClientName text
				if (n.FirstChild.Value == userId.ToString())
					return n.ParentNode.FirstChild;
			}
			return null;
		}
		public void CloseSchema()
		{
			//First mark everyone offline
			this.MarkEveryoneOffline();
			// Make sure we save our structure before leaving
			this.SaveXmlStructure();
			xmlDom = null;
		}
		private void CreateDefaultSchema()
		{
			XmlNode root, client, friend;
			// Create our root node and apped to our document
			root = xmlDom.CreateNode(XmlNodeType.Element, gSchemaRootName, null);
			xmlDom.AppendChild(root);
			// Create our client and friend nodes and apped those to our roots.
			client = xmlDom.CreateNode(XmlNodeType.Element, gClientRootName, null);
			friend = xmlDom.CreateNode(XmlNodeType.Element, gFriendRootName, null);
			root.AppendChild(client);
			root.AppendChild(friend);

			// Now save the schema
			this.SaveXmlStructure();
		}
		public void SaveXmlStructure()
		{
			xmlDom.Save(xmlDocumentLocation);
		}
		public bool DoesUserExist(string name)
		{
			// If a valid usernode is returned, this user does exist
			return (GetUserNode(name) != null);
		}
		public void AddUser(string username, string password, int currentDplayId)
		{
			// First decrypt the password
			string decrypt = MessengerShared.EncodePassword(password, MessengerShared.ClientSideEncryptionKey);
			// Now we can add the user to the XML Schema
			this.AddUserXml(username, MessengerShared.EncodePassword(decrypt, MessengerShared.ServerSideEncryptionKey), true, currentDplayId);
		}
		private void AddUserXml(string username, string password, bool loggedIn, int currentId)
		{
			// We know the first item is the main node
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gClientRootName))
			{
				XmlNode row = xmlDom.CreateNode(XmlNodeType.Element, gRowName, null);
				// Now add the client

				// First the name
				XmlNode client = xmlDom.CreateNode(XmlNodeType.Element, gClientName, null);
				XmlNode clientText = xmlDom.CreateNode(XmlNodeType.Text, null, null);
				clientText.Value = username;
				// Append to our row
				client.AppendChild(clientText);
				row.AppendChild(client);

				// Next the password
				XmlNode pwd = xmlDom.CreateNode(XmlNodeType.Element, gClientPassword, null);
				XmlNode pwdText = xmlDom.CreateNode(XmlNodeType.Text, null, null);
				pwdText.Value = password;
				//Append to our row
				pwd.AppendChild(pwdText);
				row.AppendChild(pwd);

				// Next the logged in state
				XmlNode lis = xmlDom.CreateNode(XmlNodeType.Element, gLoginState, null);
				XmlNode lisText = xmlDom.CreateNode(XmlNodeType.Text, null, null);
				lisText.Value = loggedIn.ToString();
				//Append to our row
				lis.AppendChild(lisText);
				row.AppendChild(lis);

				// Next the dplay Id
				XmlNode dpid = xmlDom.CreateNode(XmlNodeType.Element, gUserId, null);
				XmlNode dpidText = xmlDom.CreateNode(XmlNodeType.Text, null, null);
				dpidText.Value = currentId.ToString();
				//Append to our row
				dpid.AppendChild(dpidText);
				row.AppendChild(dpid);

				// Finally append this row to the client info
				n.AppendChild(row);
			}
		}
		public void NotifyFriends(string username, MessageType msg, Server serverObject)
		{
			int offset = 0;
			byte[] buffer = null;

			// We need to check to see if the user logging in is anyones friend.
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gFriendRootName + "/" + gRowName + "/" + gFriendName))
			{
				if (n.FirstChild.Value.ToLower() == username.ToLower()) // Yup, I got some friends!
				{
					// Are they logged on?
					XmlNode friend = GetUserNode(n.PreviousSibling.FirstChild.Value);
					int playerId = int.Parse(friend.NextSibling.NextSibling.NextSibling.FirstChild.Value);
					if (playerId != 0) // Yup, they got a dplay ID
					{
						DXHelp.AddDataToBuffer(ref buffer, msg, ref offset);
						DXHelp.AddDataToBuffer(ref buffer, username, ref offset);
						serverObject.SendTo(playerId, buffer, 0, 0);
					}
					break;
				}
			}
		}
		public bool AddFriend(int playerId, string friendName, bool friend)
		{
			// If friend is true, this user will be a friend, otherwise, we will block 
			// the user
			XmlNode friendNode = GetUserNode(friendName);
			// Check to see if this user is logged in
			int friendPlayerId = 0;
			if (friendNode.NextSibling.NextSibling.NextSibling.FirstChild.Value != null)
				friendPlayerId = int.Parse(friendNode.NextSibling.NextSibling.NextSibling.FirstChild.Value);

			XmlNode userNode = GetUserNodeFromId(playerId);

			bool foundFriend = false;
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gFriendRootName + "/" + gRowName + "/" + gClientName))
			{
				if (n.FirstChild.Value.ToLower() == userNode.FirstChild.Value.ToLower())
				{
					if (n.NextSibling.FirstChild.Value.ToLower() == friendName.ToLower())
					{
						n.NextSibling.NextSibling.FirstChild.Value = friend.ToString();
						foundFriend = true;
						if (friendPlayerId != 0)
							return true;
						break;
					}
				}
			}
			if (!foundFriend)
				return AddFriendXml(userNode.FirstChild.Value, friendName, friend);

			return false;
		}
		private bool AddFriendXml(string username, string friendName, bool friend)
		{
			// Check to see if this user is logged in
			XmlNode friendNode = GetUserNode(friendName);
			int friendPlayerId = 0;
			if (friendNode.NextSibling.NextSibling.NextSibling.FirstChild.Value != null)
				friendPlayerId = int.Parse(friendNode.NextSibling.NextSibling.NextSibling.FirstChild.Value);

			// We know the first item is the main node
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gFriendRootName))
			{
				XmlNode row = xmlDom.CreateNode(XmlNodeType.Element, gRowName, null);
				// Now add the client

				// First the name
				XmlNode client = xmlDom.CreateNode(XmlNodeType.Element, gClientName, null);
				XmlNode clientText = xmlDom.CreateNode(XmlNodeType.Text, null, null);
				clientText.Value = username;
				// Append to our row
				client.AppendChild(clientText);
				row.AppendChild(client);

				// Next the friendname
				XmlNode pwd = xmlDom.CreateNode(XmlNodeType.Element, gFriendName, null);
				XmlNode pwdText = xmlDom.CreateNode(XmlNodeType.Text, null, null);
				pwdText.Value = friendName;
				//Append to our row
				pwd.AppendChild(pwdText);
				row.AppendChild(pwd);

				// Next the friend or blocked state
				XmlNode lis = xmlDom.CreateNode(XmlNodeType.Element, gFriend, null);
				XmlNode lisText = xmlDom.CreateNode(XmlNodeType.Text, null, null);
				lisText.Value = friend.ToString();
				//Append to our row
				lis.AppendChild(lisText);
				row.AppendChild(lis);

				// Finally append this row to the client info
				n.AppendChild(row);

				return (friendPlayerId != 0);
			}
			return false;
		}
		public void DeleteFriend(int playerId, string friendName)
		{
			XmlNode userNode = GetUserNodeFromId(playerId);

			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gFriendRootName + "/" + gRowName + "/" + gClientName))
			{
				if (n.FirstChild.Value.ToLower() == userNode.FirstChild.Value.ToLower())
				{
					if (n.NextSibling.FirstChild.Value.ToLower() == friendName.ToLower())
					{
						// We found this friend, remove it.
						n.ParentNode.ParentNode.RemoveChild(n.ParentNode);
						break;
					}
				}
			}
		}
		public bool AmIBlocked(string username, string friendName)
		{
			XmlNode userNode = GetUserNode(username);

			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gFriendRootName + "/" + gRowName + "/" + gClientName))
			{
				if ((n.NextSibling.FirstChild.Value.ToLower() == friendName.ToLower()) && (n.FirstChild.Value.ToLower() == userNode.FirstChild.Value.ToLower()))
				{
					if (!bool.Parse(n.NextSibling.NextSibling.FirstChild.Value))
						return true;

					break;
				}
			}
			return false;
		}
		public int GetCurrentPlayerId(string username)
		{
			XmlNode userNode = GetUserNode(username);
			if (userNode == null)
				return 0;

			return int.Parse(userNode.NextSibling.NextSibling.NextSibling.FirstChild.Value);
		}
		public void FindMyFriends(string username, Server serverObject)
		{
			System.Collections.ArrayList friendList = new System.Collections.ArrayList();
			foreach (XmlNode n in xmlDom.SelectNodes(gSchemaRootName + "/" + gFriendRootName + "/" + gRowName + "/" + gClientName))
			{
				if (n.FirstChild.Value.ToLower() == username.ToLower())
				{
					FriendType f = new FriendType();
					f.friendName = n.NextSibling.FirstChild.Value;
					XmlNode friendNode = GetUserNode(f.friendName);
					f.friend = bool.Parse(n.NextSibling.NextSibling.FirstChild.Value);
					f.friendId = int.Parse(friendNode.NextSibling.NextSibling.NextSibling.FirstChild.Value);
					friendList.Add(f);
				}
			}
			if (friendList.Count > 0)
			{
				byte[] buffer = null;
				int offset = 0;
				// Well, we obviously have some friends, tell me all about them
				int sendId = GetCurrentPlayerId(username);
				DXHelp.AddDataToBuffer(ref buffer, MessageType.SendClientFriends, ref offset);
				// How many friends are we sending?
				DXHelp.AddDataToBuffer(ref buffer, friendList.Count, ref offset);
				foreach (FriendType f in friendList)
				{
					// Add whether they are a friend or blocked as well as the name
					DXHelp.AddDataToBuffer(ref buffer, f.friend, ref offset);
					DXHelp.AddDataToBuffer(ref buffer, f.friendName, ref offset);
				}
				serverObject.SendTo(sendId, buffer, 0, 0);

				// Now that's done, for every friend that's online, notify me again
				foreach (FriendType f in friendList)
				{
					if (f.friendId != 0)
					{
						buffer = null;
						offset = 0;
						DXHelp.AddDataToBuffer(ref buffer, MessageType.FriendLogon, ref offset);
						DXHelp.AddDataToBuffer(ref buffer, f.friendName, ref offset);
						serverObject.SendTo(sendId, buffer, 0, 0);
					}
				}
			}
		}
	}
}
