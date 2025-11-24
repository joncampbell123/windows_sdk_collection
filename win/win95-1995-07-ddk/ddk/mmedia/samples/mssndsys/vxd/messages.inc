;******************************************************************************
        title   MESSAGES.INC - localizeable messages
;******************************************************************************
;---------------------------------------------------------------------------;
;
;  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;  PURPOSE.
;
;  Copyright (c) 1992- 1995	Microsoft Corporation.	All Rights Reserved.
;
;---------------------------------------------------------------------------;
;
;   Title:    MSSNDSYS.386 - MICROSOFT Windows Sound System 386 Driver
;
;   Module:   MESSAGES.INC - localizeable messages
;
;   Version:  4.00
;
;   Date:     March 23, 1994
;
;******************************************************************************

VxD_MESSAGE_SEG

ifdef MSSNDSYS
DECLARE_MESSAGE <gszNoAccessMessageCODEC>, <"Unable to play sound - the Windows Sound System is in use by another application.">
DECLARE_MESSAGE <gszNoAccessMessageOPL3>, <"Unable to play MIDI - the Windows Sound System MIDI Synthesizer is use by another application.">
endif

ifdef AZTECH
DECLARE_MESSAGE <gszNoAccessMessageCODEC>, <"Unable to play sound - the Aztech Sound Card is in use by another application.">
DECLARE_MESSAGE <gszNoAccessMessageOPL3>, <"Unable to play MIDI - the Aztech Sound Card MIDI Synthesizer is use by another application.">
endif

VxD_MESSAGE_ENDS
