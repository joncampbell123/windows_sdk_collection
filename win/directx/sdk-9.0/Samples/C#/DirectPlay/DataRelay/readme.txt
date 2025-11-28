//-----------------------------------------------------------------------------
// 
// Sample Name: DataRelay Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
//-----------------------------------------------------------------------------


Description
===========
  The DataRelay is similar to SimplePeer but differs by sending a single 
  target (or everyone) a packet of data with options specified in the 
  dialog's UI. It uses a worker thread to process received data, and 
  uses the ReturnBuffer() API so that no copying of the received buffers
  is done.

Path
====
  Source:     DXSDK\Samples\C#\DirectPlay\DataRelay 
  Executable: DXSDK\Samples\C#\DirectPlay\Bin

User's Guide
============
  Host or connect to a session in the same manner as explained in SimplePeer.
  When the main dialog appears select the target, size, rate, and timeout values.
  Then click "Push to Send". This will send a packet of data to the target as
  the rate specified with the specified size.  Use the "Connection Info" dropdown 
  to specify a target to gather connection info on periodically.

Programming Notes
=================
  The DataRelay sample is very similar in form to the SimplePeer sample.  For 
  detailed programming notes on the basics this sample, refer to Programming 
  Notes section of the SimplePeer sample.

  The DataRelay differs by sending a single target (or everyone) a packet of 
  data with options specified in the dialog's UI. 

  When the "Push to Send" button is clicked, then timer is created 
  that goes off every number of ms according to the UI.
