//////////////////////////////////////////////////////////////////////////
//	DEBUG.C								//
//									//
//	Debug definitions.						//
//									//
//	For the AuraVision video capture driver AVCAPT.DRV.		//
//									//
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#ifdef DEBUG
	#define D1(sz) if (wDebugLevel >= 1) (OutputDebugString("\r\nAVVideo: "),OutputDebugString(sz))
	#define D2(sz) if (wDebugLevel >= 2) (OutputDebugString(" "),OutputDebugString(sz))
	#define D3(sz) if (wDebugLevel >= 3) (OutputDebugString(" "),OutputDebugString(sz))
	#define D4(sz) if (wDebugLevel >= 4) (OutputDebugString(" "),OutputDebugString(sz))
#else
	#define D1(sz) 0
	#define D2(sz) 0
	#define D3(sz) 0
	#define D4(sz) 0
#endif
