//-----------------------------------------------------------------------------
// 
// Sample Name: MusicLines Sample
//
// Authored by: Jim Geist and Mark Burton
// 
// Copyright © 1998, Microsoft Corp. All Rights Reserved.
//
// GM/GS® Sound Set Copyright ©1996, Roland Corporation U.S.
// 
//-----------------------------------------------------------------------------


  [This is preliminary documentation and subject to change.] 

Description
===========
  The MusicLines sample demonstrates interactive music elements in a simple 
  game. 

Path
====
  Source: Mssdk\Samples\Multimedia\Dmusic\Src\MusicLines

  Executable: Mssdk\Samples\Multimedia\DMusic\Bin

User's Guide
============
  In the opening dialog box, choose windowed or full-screen mode and a 
  difficulty level, and set the players to human or computer. If two humans are 
  playing, one can use the arrow keys while the other uses the keys AZSW.

  The object of the game is to make it impossible for the other player to 
  continue extending his or her line. 

  Change the direction of your line by pressing the arrow keys or the 
  equivalent letter keys. Observe how the main music changes to reflect the 
  current state of play, and how motifs are used to signal events such as 
  turns. 

  When one player can no longer move, the other player can continue playing or 
  can bring play to a halt by deliberately hitting a line. Play again by 
  pressing the space bar.

Programming Notes
=================
  The music logic is in Mlmusic.cpp and is amply commented.

