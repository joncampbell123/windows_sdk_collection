
/*
 *  Mac #includes.c - source to OleHeaders
 *
 *  To add a header, change its "#if 0" to "#if 1".
 *
 *  To remove a header, change its "#if 1" to "#if 0".
 *
 *  There are some conflicts and order dependencies among the various
 *  headers:
 *
 *	Note that none of the foregoing applies to Symantec C++, nor to
 *	Symantec C/C++ for MPW. This file is #included by the source for
 *	OleHeaders++, the precompiled header for Symantec C++. If a compiler
 *	other than THINK C precompiles this file, asm.h and LoMem.h are
 *	#if'd out. If an MPW-hosted compiler precompiles this file, BDC.h
 *	and pascal.h are #if'd out.
 *
 */

#ifndef _H_OleHeaders
#define _H_OleHeaders

#if defined(USEDUMP)
	#pragma load DUMPFILE
#else

#if !(defined(__SC__) || defined(applec))

// set this to 0 (zero) to retain full prototypes
// set this to 1 (one) for "simplified" prototypes
#define SIMPLIFY_PROTOTYPES		0


#ifndef _MSC_VER
// prototype checking level
#if SIMPLIFY_PROTOTYPES
	#if !__option(check_ptrs)
		#undef SIMPLIFY_PROTOTYPES
	#endif
	#pragma options(!check_ptrs)
#else
	#if __option(check_ptrs)
		#undef SIMPLIFY_PROTOTYPES
	#endif
	#pragma options(check_ptrs)
#endif
#endif

#endif

// ActionAtomIntf
	#if 0
		#include <ActionAtomIntf.h>
	#endif

// ADSP
	#if 0
		#include <ADSP.h>
	#endif

// AEObjects
	#if 0
		#include <AEObjects.h>
	#endif
	
// AEPackObject
	#if 0
		#include <AEPackObject.h>
	#endif

// AERegistry
	#if 0
		#include <AERegistry.h>
	#endif
	
// AIFF
	#if 0
		#include <AIFF.h>
	#endif

// Aliases
	#if 0
		#ifdef __APPLETALK__
			#include <Aliases.h>
		#else
			#define __APPLETALK__			// suppress unnecessary #include <AppleTalk.h>
			#include <Aliases.h>
			#undef __APPLETALK__
		#endif
	#endif

// AppleEvents
	#if 1
		#ifndef _MSC_VER
			#include <AppleEvents.h>
		#else
			#include <AppleEve.h>
		#endif
	#endif

// AppleTalk
	#if 1
		#ifndef _MSC_VER
			#include <AppleTalk.h>
		#else
			#include <AppleTal.h>
		#endif
	#endif

// Balloons
	#if 0
		#ifdef __TRAPS__
			#include <Balloons.h>
		#else
			#define __TRAPS__				// suppress unnecessary #include <Traps.h>
			#define _Pack14 0xA830			// (well, mostly unnecessary...)
			#include <Balloons.h>
			#undef _Pack14
			#undef __TRAPS__
		#endif
	#endif

// BDC
#ifdef THINK_C
	#if 1
		#include <BDC.h>
	#endif
#endif

// CommResources
	#if 0
		#include <CommResources.h>
	#endif

// Components
	#if 0
		#include <Components.h>
	#endif

// Connections
	#if 0
		#include <Connections.h>
	#endif

// ConnectionTools
	#if 0
		#include <ConnectionTools.h>
	#endif

// Controls
	#if 1
		#include <Controls.h>
	#endif

// CRMSerialDevices
	#if 0
		#include <CRMSerialDevices.h>
	#endif

// CTBUtilities
	#if 0
		#include <CTBUtilities.h>
	#endif

// DatabaseAccess
	#if 0
		#include <DatabaseAccess.h>
	#endif

// Desk
	#if 1
		#include <Desk.h>
	#endif

// DeskBus
	#if 0
		#include <DeskBus.h>
	#endif

// Devices
	#if 1
		#include <Devices.h>
	#endif

// Dialogs
	#if 1
		#include <Dialogs.h>
	#endif

// DiskInit
	#if 1
		#include <DiskInit.h>
	#endif

// Disks
	#if 0
		#include <Disks.h>
	#endif

// Editions
	#if 0
		#include <Editions.h>
	#endif

// ENET
	#if 0
		#include <ENET.h>
	#endif

// EPPC
	#if 1
		#include <EPPC.h>
	#endif

// Errors
	#if 1
		#include <Errors.h>
	#endif

// Events
	#if 1
		#include <Events.h>
	#endif

// Files
	#if 1
		#include <Files.h>
	#endif

// FileTransfers
	#if 0
		#include <FileTransfers.h>
	#endif

// FileTransferTools
	#if 0
		#include <FileTransferTools.h>
	#endif

// Finder
	#if 0
		#include <Finder.h>
	#endif

// FixMath
	#if 0
		#include <FixMath.h>
	#endif

// Folders
	#if 0
		#include <Folders.h>
	#endif

// Fonts
	#if 1
		#include <Fonts.h>
	#endif

// GestaltEqu
	#if 0
		#include <GestaltEqu.h>
	#endif

// Graf3D
	#if 0
		#include <Graf3D.h>
	#endif

// HyperXCmd
	#if 0
		#include <HyperXCmd.h>
	#endif

// Icons
	#if 0
		#include <Icons.h>
	#endif

// ImageCodec
	#if 0
		#include <ImageCodec.h>
	#endif

// ImageCompression
	#if 0
		#include <ImageCompression.h>
	#endif
	
// Language
	#if 0
		#include <Language.h>
	#endif

// Lists
	#if 1
		#include <Lists.h>
	#endif

// MediaHandlers
	#if 0
		#include <MediaHandlers.h>
	#endif

// Memory
	#if 1
		#include <Memory.h>
	#endif

// Menus
	#if 1
		#include <Menus.h>
	#endif

// MIDI
	#if 0
		#include <MIDI.h>
	#endif

// Movies
	#if 0
		#include <Movies.h>
	#endif

// MoviesFormat
	#if 0
		#include <MoviesFormat.h>
	#endif
		
// Notification
	#if 1
		#ifndef _MSC_VER
			#include <Notification.h>
		#else
			#include <Notifica.h>
		#endif
	#endif

// OSEvents
	#if 1
		#include <OSEvents.h>
	#endif

// OSUtils
	#if 1
		#include <OSUtils.h>
	#endif

// Packages
	#if 1
		#include <Packages.h>
	#endif

// Palette
	#if 0
		#include <Palette.h>
	#endif

// Palettes
	#if 0
		#include <Palettes.h>
	#endif

// Picker
	#if 0
		#include <Picker.h>
	#endif

// PictUtil
	#if 0
		#include <PictUtil.h>
	#endif

// Power
	#if 0
		#include <Power.h>
	#endif

// PPCToolBox
	#if 1
		#ifndef _MSC_VER
			#include <PPCToolBox.h>
		#else
			#include <PPCToolB.h>
		#endif
	#endif

// Printing
	#if 0
		#include <Printing.h>
	#endif

// PrintTraps
	#if 1
		#ifndef _MSC_VER
			#include <PrintTraps.h>
		#else
			#include <PrintTra.h>
		#endif
	#endif

// Processes
	#if 1
		#ifndef _MSC_VER
			#include <Processes.h>
		#else
			#include <Processe.h>
		#endif
	#endif

// QDOffscreen
	#if 0
		#include <QDOffscreen.h>
	#endif

// Quickdraw
	#if 1
		#ifndef _MSC_VER
			#include <Quickdraw.h>
		#else
			#include <Quickdra.h>
		#endif
	#endif

// QuickTimeComponents
	#if 0
		#include <QuickTimeComponents.h>
	#endif
	
// Resources
	#if 1
		#ifndef _MSC_VER
			#include <Resources.h>
		#else
			#include <Resource.h>
		#endif
	#endif

// Retrace
	#if 0
		#include <Retrace.h>
	#endif

// ROMDefs
	#if 0
		#include <ROMDefs.h>
	#endif

// SANE
	#if 0
		#include <SANE.h>
	#endif

// Scrap
	#if 1
		#include <Scrap.h>
	#endif

// Script
	#if 1
		#include <Script.h>
	#endif

// SCSI
	#if 0
		#include <SCSI.h>
	#endif

// SegLoad
	#if 1
		#include <SegLoad.h>
	#endif

// Serial
	#if 0
		#include <Serial.h>
	#endif

// ShutDown
	#if 0
		#include <ShutDown.h>
	#endif

// Slots
	#if 0
		#include <Slots.h>
	#endif

// Sound
	#if 0
		#include <Sound.h>
	#endif

// SoundInput
	#if 0
		#include <SoundInput.h>
	#endif

// StandardFile
	#if 1
		#ifndef _MSC_VER
			#include <StandardFile.h>
		#else
			#include <Standard.h>
		#endif
	#endif

// Start
	#if 0
		#include <Start.h>
	#endif

// SysEqu
	#if 1
		#include <SysEqu.h>
	#endif

// Terminals
	#if 0
		#include <Terminals.h>
	#endif

// TerminalTools
	#if 0
		#include <TerminalTools.h>
	#endif

// TextEdit
	#if 1
		#include <TextEdit.h>
	#endif

// Timer
	#if 1
		#include <Timer.h>
	#endif

// ToolUtils
	#if 1
		#ifndef _MSC_VER
			#include <ToolUtils.h>
		#else
			#include <ToolUtil.h>
		#endif
	#endif

// Types
	#if 1
		#include <Types.h>
	#endif

// Values
	#if 0
		#include <Values.h>
	#endif

// Video
	#if 0
		#include <Video.h>
	#endif

// Windows
	#if 1
		#include <macos\Windows.h>
	#endif

// pascal.h
#ifdef THINK_C
	#if 1
		#include <pascal.h>
	#endif
#endif

// asm.h
#ifdef THINK_C
	#if 1
		#include <asm.h>
	#endif
#else
	#if 0
		#include <asm.h>
	#endif
#endif

// LoMem
#ifdef THINK_C
	#if 0 && !defined(__cplusplus)
		#include <LoMem.h>
	#endif
#endif

// THINK
#ifdef THINK_C
	#if 1
		#include <THINK.h>
	#endif
#endif

// Traps
	#if 0
		#include <Traps.h>
	#endif


#if !(defined(__SC__) || defined(applec))

// restore "Check Pointer Types" to previous setting
#ifndef _MSC_VER

#if SIMPLIFY_PROTOTYPES
	#pragma options(check_ptrs)
#elif defined(SIMPLIFY_PROTOTYPES)
	#pragma options(!check_ptrs)
#endif
#undef SIMPLIFY_PROTOTYPES

#endif
#endif

#define _MAC	1

#define TRUE	1
#define FALSE	0

// string.h
	#if 1
		#include <string.h>
	#endif

// stdio.h
	#if 1
		#include <stdio.h>
	#endif

// Ole2
	#if 1
		#include <Ole2.h>
	#endif

#endif // USEDUMP

#endif
