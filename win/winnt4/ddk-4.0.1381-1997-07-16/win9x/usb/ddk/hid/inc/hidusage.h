/*++

Copyright (c) 1996      Microsoft Corporation

Module Name:

        HIDPI.H

Abstract:

   Public Definitions of HID USAGES.

Environment:

    Kernel & user mode

Revision History:

    Aug-1996 : created Kenneth D. Ray

--*/

#ifndef   __HIDUSAGE_H__
#define   __HIDUSAGE_H__

//
// Usage pages
//

#define HID_USAGE_PAGE_GENERIC    0x01
#define HID_USAGE_PAGE_VEHICLE    0x02
#define HID_USAGE_PAGE_VR         0x03
#define HID_USAGE_PAGE_SPORT      0x04
#define HID_USAGE_PAGE_GAME       0x05
#define HID_USAGE_PAGE_CONSUMER   0x06
#define HID_USAGE_PAGE_KEYBOARD   0x07
#define HID_USAGE_PAGE_LED        0x08
#define HID_USAGE_PAGE_BUTTON     0x09
#define HID_USAGE_PAGE_ORDINAL    0x0A
#define HID_USAGE_PAGE_TELEPHONY  0x0B

//
// Usages for Generic Desktop Controls
//

#define HID_USAGE_GENERIC_POINTER      0x01
#define HID_USAGE_GENERIC_MOUSE        0x02
#define HID_USAGE_GENERIC_PEN          0x03
#define HID_USAGE_GENERIC_JOYSTICK     0x04
#define HID_USAGE_GENERIC_GAMEPAD      0x05
#define HID_USAGE_GENERIC_KEYBOARD     0x06
#define HID_USAGE_GENERIC_KEYPAD       0x07
#define HID_USAGE_GENERIC_STYLUS2      0x08
#define HID_USAGE_GENERIC_PUCK         0x09

#define HID_USAGE_GENERIC_X                        0x30
#define HID_USAGE_GENERIC_Y                        0x31
#define HID_USAGE_GENERIC_Z                        0x32
#define HID_USAGE_GENERIC_RX                       0x33
#define HID_USAGE_GENERIC_RY                       0x34
#define HID_USAGE_GENERIC_RZ                       0x35
#define HID_USAGE_GENERIC_SLIDER                   0x36
#define HID_USAGE_GENERIC_DIAL                     0x37
#define HID_USAGE_GENERIC_WHEEL                    0x38
#define HID_USAGE_GENERIC_HATSWITCH                0x39
#define HID_USAGE_GENERIC_STYLUS                   0x3A
#define HID_USAGE_GENERIC_STYLUS_TIP_PRESS         0x3B
#define HID_USAGE_GENERIC_STYLUS_TIP_PRESS_TAN     0x3C
#define HID_USAGE_GENERIC_STYLUS_TIP_SWITCH        0x3D
#define HID_USAGE_GENERIC_STYLUS_TIP_THRESHOLD     0x3E
#define HID_USAGE_GENERIC_STYLUS_BARREL            0x3F
#define HID_USAGE_GENERIC_STYLUS_ERASER            0x40
#define HID_USAGE_GENERIC_TABLET_PICK              0x41
#define HID_USAGE_GENERIC_TABLET_BUTTON            0x42
#define HID_USAGE_GENERIC_TABLET_FUNCTION          0x43
#define HID_USAGE_GENERIC_TABLET_PROGRAM           0x44

//
// Vehicle Simulation Page (page 02)
//


//
// Virtual Reality Page (page 03)
//


//
// Sport Page (page 04)
//


//
// Game Control Page (page 05)
//

//
// Cosumer Appliance Page (page 06)
//

#define HID_USAGE_CONSUMER_POWER_AMP   0x01
#define HID_USAGE_CONSUMER_VIDEO_DISK  0x02

//
// Key Code Page (page 07)
//

	// Error "keys"
#define HID_USAGE_KEYBOARD_NOEVENT	   0x00
#define HID_USAGE_KEYBOARD_ROLLOVER    0x01
#define HID_USAGE_KEYBOARD_POSTFAIL    0x02
#define HID_USAGE_KEYBOARD_UNDEFINED   0x03

	// Letters
#define HID_USAGE_KEYBOARD_aA          0x04
#define HID_USAGE_KEYBOARD_zZ          0x1D
	// Numbers
#define HID_USAGE_KEYBOARD_ONE         0x1E
#define HID_USAGE_KEYBOARD_ZERO        0x27
	// Modifier Keys
#define HID_USAGE_KEYBOARD_LCTRL	   0xE0
#define HID_USAGE_KEYBOARD_LSHFT	   0xE1
#define HID_USAGE_KEYBOARD_LALT		   0xE2
#define HID_USAGE_KEYBOARD_LGUI		   0xE3
#define HID_USAGE_KEYBOARD_RCTRL	   0xE4
#define HID_USAGE_KEYBOARD_RSHFT	   0xE5
#define HID_USAGE_KEYBOARD_RALT		   0xE6
#define HID_USAGE_KEYBOARD_RGUI		   0xE7

#define HID_USAGE_KEYBOARD_RETURN      0x28
#define HID_USAGE_KEYBOARD_ESCAPE      0x29
#define HID_USAGE_KEYBOARD_DELETE      0x30

// and hundreds more

//
// LED Page (page 08)
//

#define HID_USAGE_LED_NUM_LOCK               0x01
#define HID_USAGE_LED_CAPS_LOCK              0x02
#define HID_USAGE_LED_SCROLL_LOCK            0x03
#define HID_USAGE_LED_COMPOSE                0x04
#define HID_USAGE_LED_KANA                   0x05
#define HID_USAGE_LED_POWER                  0x06
#define HID_USAGE_LED_SHIFT                  0x07
#define HID_USAGE_LED_DO_NOT_DISTURB         0x08
#define HID_USAGE_LED_MUTE                   0x09
#define HID_USAGE_LED_TONE_ENABLE            0x0A
#define HID_USAGE_LED_HIGH_CUT_FILTER        0x0B
#define HID_USAGE_LED_LOW_CUT_FILTER         0x0C
#define HID_USAGE_LED_EQUALIZER_ENABLE       0x0D
#define HID_USAGE_LED_SOUND_FIELD_ON         0x0E
#define HID_USAGE_LED_SURROUND_FIELD_ON      0x0F
#define HID_USAGE_LED_REPEAT                 0x10
#define HID_USAGE_LED_STEREO                 0x11
#define HID_USAGE_LED_SAMPLING_RATE_DETECT   0x12
#define HID_USAGE_LED_SPINNING               0x13
#define HID_USAGE_LED_CAV                    0x14
#define HID_USAGE_LED_CLV                    0x15
#define HID_USAGE_LED_RECORDING_FORMAT_DET   0x16
#define HID_USAGE_LED_OFF_HOOK               0x17
#define HID_USAGE_LED_RING                   0x18
#define HID_USAGE_LED_MESSAGE_WAITING        0x19
#define HID_USAGE_LED_DATA_MODE              0x1A
#define HID_USAGE_LED_BATTERY_OPERATION      0x1B
#define HID_USAGE_LED_BATTERY_OK             0x1C
#define HID_USAGE_LED_BATTERY_LOW            0x1D
#define HID_USAGE_LED_SPEAKER                0x1E
#define HID_USAGE_LED_HEAD_SET               0x1F
#define HID_USAGE_LED_HOLD                   0x20
#define HID_USAGE_LED_MICROPHONE             0x21
#define HID_USAGE_LED_COVERAGE               0x22
#define HID_USAGE_LED_NIGHT_MODE             0x23
#define HID_USAGE_LED_SEND_CALLS             0x24
#define HID_USAGE_LED_CALL_PICKUP            0x25
#define HID_USAGE_LED_CONFERENCE             0x26
#define HID_USAGE_LED_STAND_BY               0x27
#define HID_USAGE_LED_CAMERA_ON              0x28
#define HID_USAGE_LED_CAMERA_OFF             0x29
#define HID_USAGE_LED_ON_LINE                0x2A
#define HID_USAGE_LED_OFF_LINE               0x2B
#define HID_USAGE_LED_BUSY                   0x2C
#define HID_USAGE_LED_READY                  0x2D
#define HID_USAGE_LED_PAPER_OUT              0x2E
#define HID_USAGE_LED_PAPER_JAM              0x2F
#define HID_USAGE_LED_REMOTE                 0x30
#define HID_USAGE_LED_FORWARD                0x31
#define HID_USAGE_LED_REVERSE                0x32
#define HID_USAGE_LED_STOP                   0x33
#define HID_USAGE_LED_REWIND                 0x34
#define HID_USAGE_LED_FAST_FORWARD           0x35
#define HID_USAGE_LED_PLAY                   0x36
#define HID_USAGE_LED_PAUSE                  0x37
#define HID_USAGE_LED_RECORD                 0x38
#define HID_USAGE_LED_ERROR                  0x39


//
//  Button Page (page 09)
//
//  There is no need to label these buttons
//



//
//  Ordinal Page (page 0A)
//
//  There is no need to label these usages
//


//
//  Telephone Device Page (page 0B)
//

#define HID_USAGE_TELEPHONY_PHONE            0x01
#define HID_USAGE_TELEPHONY_MESSAGE          0x02

#define HID_USAGE_TELEPHONY_HOOK_SWITCH      0x08
#define HID_USAGE_TELEPHONY_DISPLAY          0x09

#define HID_USAGE_TELEPHONY_KEYPAD           0x0F
#define HID_USAGE_TELEPHONY_DIAL_ONE         0x10
#define HID_USAGE_TELEPHONY_DIAL_ZERO        0x1A
#define HID_USAGE_TELEPHONY_DIAL_STAR        0x1B
#define HID_USAGE_TELEPHONY_DIAL_POUND       0x1C
#define HID_USAGE_TELEPHONY_FLASH            0x20
#define HID_USAGE_TELEPHONY_HOLD             0x21
#define HID_USAGE_TELEPHONY_REDIAL           0x22
#define HID_USAGE_TELEPHONY_CALLERID         0x23
#define HID_USAGE_TELEPHONY_TRANSFER         0x24
#define HID_USAGE_TELEPHONY_PARK             0x25
#define HID_USAGE_TELEPHONY_SETBUSY          0x26
#define HID_USAGE_TELEPHONY_PAGE             0x27
#define HID_USAGE_TELEPHONY_DATA_MODE        0x28

//
// And others
//


#endif


