/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/* 
	Purpose	:  Data for the new resources in the keyboard driver.
*/

#define US_KEYBOARD_TABLE 			1000
#define RT_RCDATA MAKEINTRESOURCE(10)


/*
	These names are indexed by scan code for ease of loading !

*/

#define	IDS_ESCAPE 		0x01 
#define	IDS_OEM_MINUS	0x0c 
#define	IDS_OEM_PLUS 	0x0d 
#define  IDS_BACKSPACE  0x0e
#define  IDS_TAB        0x0f
#define	IDS_OEM_4    	0x1a
#define	IDS_OEM_6    	0x1b
#define	IDS_RETURN 		0x1c
#define	IDS_CONTROL		0x1d
#define	IDS_OEM_1    	0x27
#define	IDS_OEM_7    	0x28
#define	IDS_OEM_3    	0x29
#define	IDS_LSHIFT 		0x2a
#define	IDS_OEM_5    	0x2b
#define	IDS_OEM_COMMA	0x33
#define	IDS_OEM_PERIOD	0x34
#define	IDS_OEM_2    	0x35
#define	IDS_RSHIFT 		0x36
#define	IDS_MULTIPLY	0x37
#define	IDS_MENU   		0x38
#define	IDS_SPACE  		0x39
#define	IDS_CAPITAL		0x3a
#define	IDS_F1     		0x3b
#define	IDS_F2     		0x3c
#define	IDS_F3     		0x3d
#define	IDS_F4     		0x3e
#define	IDS_F5     		0x3f
#define	IDS_F6     		0x40
#define	IDS_F7     		0x41
#define	IDS_F8     		0x42
#define	IDS_F9     		0x43
#define	IDS_F10    		0x44
#define	IDS_NUMLOCK		0x45
#define	IDS_OEM_SCROLL	0x46
#define	IDS_HOME   		0x47
#define	IDS_UP     		0x48
#define	IDS_PRIOR  		0x49
#define	IDS_SUBTRACT	0x4a
#define	IDS_LEFT   		0x4b
#define	IDS_CLEAR  		0x4c
#define	IDS_RIGHT  		0x4d
#define	IDS_ADD    		0x4e
#define	IDS_END    		0x4f
#define	IDS_DOWN   		0x50
#define	IDS_NEXT   		0x51
#define	IDS_INSERT 		0x52
#define	IDS_DELETE 		0x53
#define	IDS_OEM_102		0x56
#define	IDS_F11    		0x57
#define	IDS_F12			0x58

#define	IDS_VK_LWIN		0x15b
#define	IDS_VK_RWIN		0x15c
#define	IDS_VK_APPS		0x15d

/* 
	Now we have extended key names ie. some keys have two names
	so we add 0x100 to differentiate.
*/
								
#define  IDS_PrtScreen 	0x101								
#define  IDS_NumLock    0x145								
#define  IDS_NumEnter   0x11c
#define  IDS_RCtrl 		0x11d
#define  IDS_NumDiv  	0x135
#define  IDS_AltGr 		0x138
#define  IDS_Home 		0x147
#define  IDS_Up 			0x148
#define  IDS_PgUp 		0x149
#define  IDS_Left 		0x14b
#define  IDS_Right 		0x14d
#define  IDS_End 			0x14f
#define  IDS_Down 		0x150
#define  IDS_PgDown  	0x151
#define  IDS_Insert  	0x152
#define  IDS_Delete  	0x153

/*
	These are dead key names. The index in this case is an ANSI Value
	Nevertheless they are unique !

*/

#define 	IDS_ACUTE0		0x3b4
#define 	IDS_ACUTE1		0x392
#define 	IDS_grave		0x360
#define 	IDS_Circumflex	0x35e
#define 	IDS_umlaut0		0x322
#define 	IDS_umlaut1		0x3a8
#define 	IDS_umlaut2		0x3b7
#define 	IDS_Tilde 		0x398
#define 	IDS_Cedilla		0x3b8
#define 	IDS_Ring	 	0x3b0
#define		IDS_TildeH	   	0x37e
#define		IDS_Circumflx2 	0x388
#define		IDS_Hachek     	0x3a1
#define		IDS_Breve      	0x3a2
#define		IDS_Dacute     	0x3bd
#define		IDS_Ogonek     	0x3b2

