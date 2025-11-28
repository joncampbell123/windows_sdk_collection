Imports System
Imports System.Security.Cryptography


Public Enum MessageType
    'Login messages
    Login 'Login information
    LoginSuccess 'Logged in successfully
    CreateNewAccount 'A new account needs to be created
    InvalidPassword 'The password for this account is invalid
    InvalidUser 'This user doesn't exist
    UserAlreadyExists 'This user already exists (only can be received after a CreateNewAcct msg)
    'Friend Controls
    AddFriend 'Add a friend to my list
    FriendAdded 'User was added
    FriendDoesNotExist 'Tried to add a friend that doesn't exist
    BlockUserDoesNotExist 'Tried to block a user that doesn't exist
    BlockFriend 'Block someone from contacting me
    FriendBlocked 'User was blocked
    DeleteFriend 'Delete this user from my list of friends
    FriendDeleted 'The user was deleted from your list of friends
    SendClientFriends 'The Server will send the client it's list of friends
    'Messages
    SendMessage 'Send a message to someone
    UserBlocked 'Can't send a message to this person, they've blocked you
    ReceiveMessage 'Received a message
    UserUnavailable 'The user you are trying to send a message to is no longer logged on
    'Friend Logon messages
    FriendLogon 'A friend has just logged on
    FriendLogoff 'A friend has just logged off
    'System Wide Messages
    LogonOtherLocation ' This account has been logged on somewhere else
    ServerKick ' The server has disconnectd you for some reason
End Enum 'MessageType
    _
Public Class MessengerShared
    Public Shared ReadOnly ApplicationName As String = "DxMessenger"
    Public Shared applicationGuid As New Guid("0AC3AAC4-5470-4AB0-ABBE-6EF0B614E52A")
    Public Const DefaultPort As Integer = 9132


    Private Sub New()
    End Sub 'New

    Public Shared Function EncodePassword(ByVal password As String) As String
        ' First we need to turn our password into a byte array
        Dim data As Byte() = System.Text.Encoding.Unicode.GetBytes(password)

        ' Now generate a basic hash
        Dim md5 = New MD5CryptoServiceProvider()

        Dim result As Byte() = md5.ComputeHash(data)

        Return System.Text.Encoding.ASCII.GetString(result, 0, result.Length)
    End Function 'EncodePassword
End Class 'MessengerShared
