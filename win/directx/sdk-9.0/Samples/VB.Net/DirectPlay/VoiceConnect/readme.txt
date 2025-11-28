//-----------------------------------------------------------------------------
// 
// Sample Name: VoiceConnect Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  VoiceConnect shows how to network other players using DirectPlay to
  start a DirectPlay Voice chat session.  After joining or creating a 
  session, the players may use computer microphone to talk to one other.  
  Other players may join the game in progress at any time.  

Path
====
  Source:     DXSDK\Samples\VB.NET\DirectPlay\VoiceConnect 
  Executable: DXSDK\Samples\VB.NET\DirectPlay\Bin

User's Guide
============
  Refer to User's Guide section of the SimplePeer sample for information
  on how to connect.  After connecting, the host will be presented with a 
  dialog to select the voice codec to use.  Typical voice applications will
  what automatically select one, or present this to the user in some other 
  fashion.  Once the chat session has begin, any client may alter the playback
  or capture settings by clicking on "Setup".

Programming Notes
=================
  The VoiceConnect sample is very similar in form to the SimplePeer sample.  For 
  detailed programming notes on the basics this sample, refer to Programming 
  Notes section of the SimplePeer sample.

  The VoiceConnect differs by letting clients use DirectPlay Voice to talk
  to each other using a computer microphone.  Here's how this is done:
  
  * Initialize DirectPlayVoice.  
       Before using voice you need a to establish a DirectPlay Voice session.
       
         - Tests the audio setup with DirectPlay Voice. 
         - If hosting, then we create the voice session:  
          - If either hosting or joining, then connect to the session. 
  * Handle voice client messages.  
