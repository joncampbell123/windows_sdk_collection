Imports System
Imports System.Windows.Forms
Imports System.Runtime.Serialization.Formatters.Binary
Imports System.Collections
Imports Microsoft.DirectX
Imports Microsoft.DirectX.DirectPlay

Namespace DXMessengerServer
    '/ <summary>
    '/ The data for each client of our application
    '/ </summary>
    <Serializable()> Public Class ClientData
        Public name As String
        Public password As String
        Public loggedin As Boolean
        Public currentId As Integer
        Public friends As New ArrayList()


        ' This is here so the serialization can work
        Public Sub New()
        End Sub 'New

        Public Sub New(ByVal n As String, ByVal pwd As String, ByVal log As Boolean, ByVal id As Integer)
            name = n
            password = pwd
            loggedin = log
            currentId = id
        End Sub 'New
    End Class 'ClientData

    '/ <summary>
    '/ A friend's data.  Simply the name and whether or not the user is a friend
    '/ </summary>
    <Serializable()> Public Class FriendData
        Public friendName As String
        Public [friend] As Boolean


        ' This is here so the serialization can work
        Public Sub New()
        End Sub 'New

        Public Sub New(ByVal n As String, ByVal f As Boolean, ByVal id As Integer)
            friendName = n
            [friend] = f
        End Sub 'New
    End Class 'FriendData
    _
    '/ <summary>
    '/ The main data store for the sample.  We will let the Binary Serializer format our data
    '/ for us.  While this sample simply keeps all of the data in memory at any given time
    '/ for a large scale application, where thousands of clients could be connected at once, 
    '/ this type of data store would not be very useful.  In those cases, it would be better
    '/ to use a data store designed for large amounts of data, for example SQL Server.
    '/ </summary>
    Public Class DataStore
        ' This sample uses a binary file for it's data store.
        Private dataStoreFile As String = String.Empty
        Private ourData As ArrayList = Nothing


        '/ <summary>
        '/ Create a new data store
        '/ </summary>
        Public Sub New()
            ' First try to find the default data store
            Dim mediaFolder As String = DXUtil.SdkMediaPath
            dataStoreFile = mediaFolder + "dxvbmsgserver.dat"

            If Not System.IO.File.Exists(dataStoreFile) Then
                ' We need to create our default schema
                MessageBox.Show("The default data store could not be found.  We will create a new one.", "No default store.", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Me.CreateDefaultStore()
            End If
            Dim file As System.IO.FileStream = Nothing
            Try
                ' Well the file exists, try to load it, and throw an exception if you 
                ' don't like it
                file = System.IO.File.OpenRead(dataStoreFile)
                Dim serial As New BinaryFormatter()
                ourData = CType(serial.Deserialize(file), ArrayList)
            Catch ee As Exception
                Console.WriteLine(ee.ToString())
                MessageBox.Show("There was an error trying to read the data store.  We will create a new store.", "No default store.", MessageBoxButtons.OK, MessageBoxIcon.Information)
                Me.CreateDefaultStore()
            Finally
                If Not (file Is Nothing) Then
                    file.Close()
                End If
            End Try ' No one can be logged on now, make sure everyone is marked offline
            MarkEveryoneOffline()
        End Sub 'New

        '/ <summary>
        '/ Mark this user as logged in, if they exist
        '/ </summary>
        '/ <param name="username">The user we are logging on</param>
        '/ <param name="password">A hash of this users password</param>
        '/ <returns>
        '/ LogonTypes.AccountDoesNotExist if the account does not exist
        '/ LogonTypes.InvalidPassword if the password hash doesn't match
        '/ LogonTypes.LogonSuccess if the login was successfull
        '/ </returns>
        Public Function LogonUser(ByVal username As String, ByVal password As String) As LogonTypes
            If Not DoesUserExist(username) Then
                Return LogonTypes.AccountDoesNotExist
            End If
            Dim user As ClientData = GetUser(username)

            'Now check the password vs what's stored in our data
            If user.password = password Then
                ' Great, this is the user, and the password matches
                Return LogonTypes.LogonSuccess
                ' This guy doesn't know his own password, let'em know about it
            Else
                Return LogonTypes.InvalidPassword
            End If
        End Function 'LogonUser


        '/ <summary>
        '/ Check to see if a user exists
        '/ </summary>
        '/ <param name="username">Username we are checking</param>
        '/ <returns>true if they exist, false otherwise</returns>
        Public Overloads Function DoesUserExist(ByVal username As String) As Boolean
            Return Not (GetUser(username) Is Nothing)
        End Function 'DoesUserExist

        Private Overloads Function DoesUserExist(ByVal id As Integer) As Boolean
            Return Not (GetUser(id) Is Nothing)
        End Function 'DoesUserExist


        '/ <summary>
        '/ Mark this user as now logged in
        '/ </summary>
        '/ <param name="username">Username we are updating</param>
        '/ <param name="playerId">Current DirectPlay Player ID</param>
        Public Sub UpdateDataToReflectLogon(ByVal username As String, ByVal playerId As Integer)
            If Not DoesUserExist(username) Then
                Throw New System.ApplicationException() ' We shouldn't hit this since the user has to be created to get here
            End If
            Dim user As ClientData = GetUser(username)
            user.currentId = playerId
            user.loggedin = True
        End Sub 'UpdateDataToReflectLogon

        '/ <summary>
        '/ Mark this user as now logged out
        '/ </summary>
        '/ <param name="username">Username we are updating</param>
        Public Sub UpdateDataToReflectLogoff(ByVal playerId As Integer)
            If Not DoesUserExist(playerId) Then
                Return ' We should only hit this on the case when they log on somewhere else, in which case we don't care
            End If
            Dim user As ClientData = GetUser(playerId)
            user.currentId = 0
            user.loggedin = False
        End Sub 'UpdateDataToReflectLogoff

        '/ <summary>
        '/ Mark everyone in the data store as logged out
        '/ </summary>
        Private Sub MarkEveryoneOffline()
            ' Go through and mark everyone as logged off
            Dim u As ClientData
            For Each u In ourData
                u.currentId = 0
                u.loggedin = False
            Next u
        End Sub 'MarkEveryoneOffline

        '/ <summary>
        '/ Find a user based on the name (case insensitive)
        '/ </summary>
        '/ <param name="username">Username to search for</param>
        '/ <returns>The users data requested, or null</returns>
        Private Overloads Function GetUser(ByVal username As String) As ClientData
            Dim u As ClientData
            For Each u In ourData
                If u.name.ToLower() = username.ToLower() Then
                    Return u
                End If
            Next u
            Return Nothing
        End Function 'GetUser

        '/ <summary>
        '/ Find a user based on the player id
        '/ </summary>
        '/ <param name="userId">PlayerId to search for</param>
        '/ <returns>The users data requested, or null</returns>
        Private Overloads Function GetUser(ByVal userId As Integer) As ClientData
            Dim u As ClientData
            For Each u In ourData
                If u.currentId = userId Then
                    Return u
                End If
            Next u
            Return Nothing
        End Function 'GetUser


        '/ <summary>
        '/ Shut down the datastore and save any data
        '/ </summary>
        Public Sub Close()
            'First mark everyone offline
            Me.MarkEveryoneOffline()
            ' Make sure we save our structure before leaving
            Me.SaveDataStore()
        End Sub 'Close

        '/ <summary>
        '/ Create and save a default (empty) data store
        '/ </summary>
        Private Sub CreateDefaultStore()
            ' Just create a blank arraylist
            ourData = New ArrayList()
            ' Now save the schema
            Me.SaveDataStore()
        End Sub 'CreateDefaultStore

        '/ <summary>
        '/ Save the Datastore to a file, using the BinaryFormatter
        '/ </summary>
        Public Sub SaveDataStore()
            Dim file As System.IO.FileStream = System.IO.File.OpenWrite(dataStoreFile)
            Try
                Dim serial As New BinaryFormatter()
                serial.Serialize(file, ourData)
            Catch e As Exception
                MessageBox.Show("An unhandled exception has occured saving the data." + ControlChars.Cr + ControlChars.Lf + e.ToString(), "Unhandled exception")
            Finally
                file.Close()
            End Try
        End Sub 'SaveDataStore

        '/ <summary>
        '/ Add a user to our data store
        '/ </summary>
        Public Overloads Sub AddUser(ByVal username As String, ByVal password As String, ByVal currentDplayId As Integer)
            ' Now we can add the user to the data store
            Me.AddUser(username, password, True, currentDplayId)
        End Sub 'AddUser

        '/ <summary>
        '/ Add a user to our data store
        '/ </summary>
        Private Overloads Sub AddUser(ByVal username As String, ByVal password As String, ByVal loggedIn As Boolean, ByVal currentId As Integer)
            ourData.Add(New ClientData(username, password, loggedIn, currentId))
        End Sub 'AddUser

        '/ <summary>
        '/ Notify friends when we are online
        '/ </summary>
        Public Sub NotifyFriends(ByVal username As String, ByVal msg As MessageType, ByVal serverObject As Server)
            Dim stm As New NetworkPacket()

            ' We need to check to see if the user logging in is anyones friend.
            Dim u As ClientData
            For Each u In ourData
                ' We can skip ourself
                If u.name.ToLower() <> username.ToLower() Then
                    ' This isn't me, scan through all this users friends
                    Dim f As FriendData
                    For Each f In u.friends
                        ' Is this friend me?
                        If f.friendName.ToLower() = username.ToLower() Then
                            ' Yup, I got some friends!
                            ' Are they logged on?
                            If GetUser(u.name).currentId <> 0 Then
                                stm.Write(msg)
                                stm.Write(username)
                                serverObject.SendTo(GetUser(u.name).currentId, stm, 0, 0)
                                stm = New NetworkPacket()
                            End If
                        End If
                    Next f
                End If
            Next u
        End Sub 'NotifyFriends

        Public Overloads Function AddFriend(ByVal playerId As Integer, ByVal friendName As String, ByVal isFriend As Boolean) As Boolean
            ' If friend is true, this user will be a friend, otherwise, we will block 
            ' the user
            Dim user As ClientData = GetUser(playerId)
            Dim [friend] As ClientData = GetUser(friendName)

            ' Check to see if this user is logged in
            Dim friendPlayerId As Integer = [friend].currentId

            Dim foundFriend As Boolean = False
            Dim f As FriendData
            For Each f In user.friends
                If f.friendName = friendName Then
                    ' we found our friend
                    f.friend = isFriend
                    foundFriend = True
                End If
            Next f
            If Not foundFriend Then
                Return AddFriend(user.name, friendName, isFriend)
            End If
            Return False
        End Function 'AddFriend

        Private Overloads Function AddFriend(ByVal username As String, ByVal friendName As String, ByVal isFriend As Boolean) As Boolean
            ' Check to see if this user is logged in
            Dim user As ClientData = GetUser(username)
            Dim [friend] As ClientData = GetUser(friendName)

            Dim friendPlayerId As Integer = [friend].currentId
            user.friends.Add(New FriendData(friendName, isFriend, friendPlayerId))
            Return friendPlayerId <> 0
        End Function 'AddFriend

        Public Sub DeleteFriend(ByVal playerId As Integer, ByVal friendName As String)
            Dim user As ClientData = GetUser(playerId)

            Dim f As FriendData
            For Each f In user.friends
                If f.friendName = friendName Then
                    user.friends.Remove(f)
                    Exit For
                End If
            Next f
        End Sub 'DeleteFriend

        Public Function AmIBlocked(ByVal username As String, ByVal friendName As String) As Boolean
            Dim user As ClientData = GetUser(username)

            Dim f As FriendData
            For Each f In user.friends
                If f.friendName = friendName Then
                    Return f.friend = False ' Will be false if we are blocked
                End If
            Next f
            Return False
        End Function 'AmIBlocked


        Public Function GetCurrentPlayerId(ByVal username As String) As Integer
            Dim user As ClientData = GetUser(username)
            If user Is Nothing Then
                Return 0
            End If
            Return user.currentId
        End Function 'GetCurrentPlayerId

        Public Sub FindMyFriends(ByVal username As String, ByVal serverObject As Server)
            Dim user As ClientData = GetUser(username)

            If user.friends.Count > 0 Then
                Dim stm As New NetworkPacket()

                ' Well, we obviously have some friends, tell me all about them
                Dim sendId As Integer = GetCurrentPlayerId(username)
                stm.Write(MessageType.SendClientFriends)
                ' How many friends are we sending?
                stm.Write(user.friends.Count)
                Dim f As FriendData
                For Each f In user.friends
                    ' Add whether they are a friend or blocked as well as the name
                    stm.Write(f.friend)
                    stm.Write(f.friendName)
                Next f
                serverObject.SendTo(sendId, stm, 0, 0)

                ' Now that's done, for every friend that's online, notify me again
                For Each f In user.friends
                    If GetUser(f.friendName).currentId <> 0 Then
                        stm = New NetworkPacket()

                        stm.Write(MessageType.FriendLogon)
                        stm.Write(f.friendName)
                        serverObject.SendTo(sendId, stm, 0, 0)
                    End If
                Next f
            End If
        End Sub 'FindMyFriends
    End Class 'DataStore
End Namespace 'DXMessengerServer