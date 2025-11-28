//-----------------------------------------------------------------------------
// 
// Sample Name: DxMessenger Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  DXMessenger shows how to use DirectPlay to create a client/server session
  and start an instant message chat session.  After logging on to a server
  session (and possibly creating a new account), the players may add friends
  and instant message them.

Path
====
  Source:     DXSDK\Samples\C#\DirectPlay\DXMessenger
  Executable: DXSDK\Samples\C#\DirectPlay\Bin

User's Guide
============
From the Server component you can simply start or stop the server, and remove 
players that are currently logged in.

From the Client component, you connect to servers, add friends and hold instant
message conversations.

Programming Notes
=================
The server maintains it's data in a store by serializing the data class.  If the data class isn't
found when the application first starts up, it will create a new (blank) one in which to use.  The
data store is saved periodically while running and again at shutdown.