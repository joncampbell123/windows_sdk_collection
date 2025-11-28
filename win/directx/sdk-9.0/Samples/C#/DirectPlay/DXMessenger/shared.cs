using System;
using System.Security.Cryptography;

namespace DXMessenger
{
	public enum MessageType
	{
		//Login messages
		Login , //Login information
		LoginSuccess , //Logged in successfully
		CreateNewAccount , //A new account needs to be created
		InvalidPassword , //The password for this account is invalid
		InvalidUser , //This user doesn't exist

		UserAlreadyExists , //This user already exists (only can be received after a CreateNewAcct msg)

		//Friend Controls
		AddFriend , //Add a friend to my list
		FriendAdded , //User was added
		FriendDoesNotExist , //Tried to add a friend that doesn't exist
		BlockUserDoesNotExist , //Tried to block a user that doesn't exist
		BlockFriend , //Block someone from contacting me
		FriendBlocked , //User was blocked
		DeleteFriend , //Delete this user from my list of friends
		FriendDeleted, //The user was deleted from your list of friends
		SendClientFriends , //The Server will send the client it's list of friends

		//Messages
		SendMessage , //Send a message to someone
		UserBlocked , //Can't send a message to this person, they've blocked you
		ReceiveMessage , //Received a message
		UserUnavailable , //The user you are trying to send a message to is no longer logged on

		//Friend Logon messages
		FriendLogon , //A friend has just logged on
		FriendLogoff , //A friend has just logged off

		//System Wide Messages
		LogonOtherLocation, // This account has been logged on somewhere else
		ServerKick, // The server has disconnectd you for some reason
	}
	public class MessengerShared
	{
		public const string ApplicationName = "DxMessenger";
		public static readonly Guid applicationGuid = new Guid("0AC3AAC4-5470-4AB0-ABBE-6EF0B614E52A");
		public const int DefaultPort = 9132;

		private MessengerShared(){}

		public static string EncodePassword(string password)
		{
			// First we need to turn our password into a byte array
			byte[] data = System.Text.Encoding.Unicode.GetBytes(password);

			// Now generate a basic hash
			MD5 md5 = new MD5CryptoServiceProvider();

			byte[] result = md5.ComputeHash(data);

			return System.Text.Encoding.ASCII.GetString(result, 0, result.Length);
		}
	}
}
