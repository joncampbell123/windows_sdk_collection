	TITLE	VDD - Virtual Display Device for EGA/VGA  vers 3.0a  2/89
;******************************************************************************
;
;VDDMSG.ASM	Messages for VDD
;
;   Author: MDW
;
;   (C) Copyright MICROSOFT Corp. 1986, 1987, 1988, 1989
;
;   February, 1989
;
;DESCRIPTION:
;
;******************************************************************************

        .386p

	INCLUDE VMM.INC

VxD_IDATA_SEG

BeginMsg
;***************
; Load time error messages
;
; The following is a message that results from incorrect installation of
;   device drivers. Another device driver is conflicting with the VDD.
PUBLIC VDD_Str_CheckVidPgs
VDD_Str_CheckVidPgs DB	"Cannot run Windows because of video device "
		    DB	"conflict. You need to run the Setup program "
		    DB	"again.",0

; The following is a message that results from incorrect initialization of
;   of the VDD, probably due to insufficient memory, incorrect display
;   adapter or bad files. User should verify that enough system memory
;   is available and that SETUP was completed properly.
PUBLIC VDD_Str_BadDevice
VDD_Str_BadDevice   DB	"Video initialization failed. You need to run "
		    DB	"the Setup program again.",0

;******************************************************************************
;
; INI entry for window update time.
;
PUBLIC VDD_Time_Ini
VDD_Time_Ini db "WindowUpdateTime", 0

;******************************************************************************
;
; INI entry for Initial number of text rows
;
PUBLIC VDD_Text_Rows_Ini,VDD_Text_Rows_Sect
VDD_Text_Rows_Ini db "ScreenLines", 0
VDD_Text_Rows_Sect db "NonWindowsApp", 0	; System.ini section name

; INI entry to leave 2nd display linear address area (0B000h-0B7000h) alone
;	If this is set to FALSE, the VDD will not reserve the
;	monochrome address area, permitting other devices to use the
;	linear address space.  If this switch is TRUE or not present
;	and a secondary VDD does not exist but memory does exist in the
;	monochrome address area, the VDD will reserve the space but will
;	leave it mapped so that any VM accessing it will access the
;	physical device.  If a seconday display VDD is installed, then
;	this switch has no affect.
PUBLIC VDD_2nd_Ini
VDD_2nd_Ini  db "DualDisplay", 0
EndMsg

VxD_IDATA_ENDS


VxD_DATA_SEG

BeginMsg
;***************
; Video Message Box (VMB) definitions: Message text followed by Msg Box type
;	The caption of the message box is always the VM name. Note that
;	the messages that indicate that the application cannot run in the
;	background may be transitory. Many applications will change their
;	screen mode in the course of program execution.
;
;
; The following is a message that appears when the virtual display device
;   attempts to save the state of the video adapter and runs out of memory
;   The display for the app is probably corrupted. The user should free up
;   some memory by closing another application or getting another application
;   to free up some of its data and then get the application to redraw its
;   display. In some cases, the user will need to exit the application and
;   start it up again in order to redraw the display.
PUBLIC VMB_Str_NoMainMem
VMB_Str_NoMainMem   DB	"There is not enough available memory for this "
		    DB	"application to correctly display information. "
		    DB	"Make sure the application's PIF settings are "
		    DB	"correct.",0

; The following is a message that appears when the virtual display device
;   runs out of memory that it uses to update the display in a window. If the
;   user wants to continue to run the application in a window, they should
;   free up some memory by closing an application or get another application
;   to release some of its working data.
PUBLIC VMB_Str_NoCopyMem
VMB_Str_NoCopyMem   DB	"There is not enough available memory for this "
		    DB	"application to correctly display information. "
		    DB	"Free some memory by quitting applications you "
		    DB	"aren't using, and make sure the application's "
		    DB	"PIF settings are correct.",0

; The following is a message that appears when the virtual display device
;   detects an application that is using the display in a way so that it
;   cannot run simultaneously with Windows or other high resolution graphics
;   applications. Text mode applications can run in the background while
;   this application runs in the forground.
PUBLIC VMB_Str_Exclusive
VMB_Str_Exclusive   DB	"You cannot run this application in a window or in "
		    DB	"the background. You can display it in a window, "
		    DB	"but it will be suspended until you run it in a "
		    DB	"full screen. Check the PIF settings to ensure "
		    DB	"they are correct.",0

; The following is a message that appears when the virtual display device
;   detects an application that using the display in a way so that it
;   cannot run in the background or be displayed in a window. Text mode
;   applications can run in the background while this application runs in
;   the forground.
PUBLIC VMB_Str_NotWindow
VMB_Str_NotWindow   DB	"You cannot display this application in a window "
		    DB	"or run it in the background. It will be suspended "
		    DB	"until you display it in a full screen. Check the "
		    DB	"PIF settings to ensure they are correct.",0

; The following is a message that appears when the virtual display device
;   is not able to correctly handle a mode change or video memory access
;   by a VM.  This will usually occur for a high resolution VM running in
;   the background or not exclusive.  The user should run this application
;   in full screen, exclusive mode and not in the background.  The display
;   for the app is probably corrupted. In some cases, the user will need
;   to exit the application and start it up again in order to redraw the
;   display.
PUBLIC VMB_Str_NoSupMode
VMB_Str_NoSupMode   DB	"This application will be suspended until you run it "
		    DB	"in a full screen. Check the PIF settings to "
		    DB	"ensure they are correct. If you need to restore the "
		    DB	"screen display, restart the application.",0

; The following is a message that appears when the virtual display device
;   is not able to run a background VM because the forground VM is running
;   in a mode that does not support high resolution graphics in the
;   background and this VM runs in high resolution graphics.  When the
;   screen focus changes, this VM may start to run again.
PUBLIC VMB_Str_NoFGrnd
VMB_Str_NoFGrnd     DB	"You cannot run this application while another "
		    DB	"high-resolution application is running in a full "
		    DB	"screen. The application will be suspended until "
		    DB	"a low-resolution or text application is running "
		    DB	"in a full screen. Check the PIF settings to ensure "
		    DB	"they are correct.",0

; The following is a message that appears when the virtual display device
;   is not able to grab the current mode - 13.
PUBLIC VMB_Str_CannotGrab
VMB_Str_CannotGrab  DB	"You cannot copy the screen to the Clipboard because "
		    DB	"of the application video mode.",0

EndMsg

VxD_DATA_ENDS

END
