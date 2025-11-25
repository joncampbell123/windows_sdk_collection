#include "Types.r"
#include "SysTypes.r"

#ifdef _PPCMAC
#define __powerc
#endif

#ifdef DLL

#define _REZ
#define _MAC
#define USE_OLE2_VERS


#include "ole2ver.h"
#include "CodeFrag.r"

resource 'cfrg' (0) {
   {
     kPowerPC,
     kFullLib,
	  kNoVersionNum,kNoVersionNum,
	  0, 0,
	  kIsLib,kOnDiskFlat,kZeroOffset,kWholeFork,
	  "OLEUILIB"
   }
};

#endif

type 'CSTR' {
	cstring;
};

resource 'DITL' (10000, "Insert Object") {
	{	/* array DITLarray: 24 elements */
		/* [1] */
		{31, 407, 49, 473},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{60, 407, 78, 473},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		Button {
			enabled,
			"Hidden"
		},
		/* [4] */
		{14, 281, 29, 383},
		UserItem {
			enabled
		},
		/* [5] */
		{40, 290, 58, 370},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{65, 290, 83, 370},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{35, 98, 133, 275},
		UserItem {
			enabled
		},
		/* [8] */
		{12, 98, 31, 276},
		UserItem {
			enabled
		},
		/* [9] */
		{35, 387, 103, 388},
		Picture {
			disabled,
			10001
		},
		/* [10] */
		{1031, 1407, 1049, 1473},
		Button {
			enabled,
			"OK"
		},
		/* [11] */
		{85, 407, 103, 473},
		Button {
			enabled,
			"Help"
		},
		/* [12] */
		{157, 349, 175, 467},
		CheckBox {
			enabled,
			"Display As Icon"
		},
		/* [13] */
		{40, 10, 58, 90},
		RadioButton {
			enabled,
			"New"
		},
		/* [14] */
		{64, 10, 82, 90},
		RadioButton {
			enabled,
			"From File"
		},
		/* [15] */
		{159, 10, 235, 316},
		UserItem {
			disabled
		},
		/* [16] */
		{1029, 1098, 1143, 1389},
		UserItem {
			enabled
		},
		/* [17] */
		{179, 392, 211, 424},
		UserItem {
			enabled
		},
		/* [18] */
		{218, 353, 230, 463},
		UserItem {
			enabled
		},
		/* [19] */
		{1027, 1403, 1053, 1477},
		UserItem {
			disabled
		},
		/* [20] */
		{102, 288, 120, 336},
		CheckBox {
			enabled,
			"Link"
		},
		/* [21] */
		{1010, 1097, 1026, 1181},
		StaticText {
			disabled,
			"Object Type:"
		},
		/* [22] */
		{152, 15, 168, 60},
		StaticText {
			disabled,
			"Result"
		},
		/* [23] */
		{22, 10, 38, 60},
		StaticText {
			disabled,
			"Create:"
		},
		/* [24] */
		{1216, 1351, 1232, 1465},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DITL' (10001, "Paste Special") {
	{	/* array DITLarray: 15 elements */
		/* [1] */
		{20, 380, 38, 450},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{48, 380, 66, 450},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{76, 380, 94, 450},
		Button {
			enabled,
			"Help"
		},
		/* [4] */
		{174, 335, 192, 453},
		CheckBox {
			enabled,
			"Display As Icon"
		},
		/* [5] */
		{75, 12, 93, 100},
		RadioButton {
			enabled,
			"Paste"
		},
		/* [6] */
		{106, 12, 124, 100},
		RadioButton {
			enabled,
			"Paste Link"
		},
		/* [7] */
		{172, 12, 248, 318},
		UserItem {
			disabled
		},
		/* [8] */
		{58, 110, 154, 360},
		UserItem {
			enabled
		},
		/* [9] */
		{16, 70, 32, 360},
		UserItem {
			disabled
		},
		/* [10] */
		{197, 378, 229, 410},
		UserItem {
			enabled
		},
		/* [11] */
		{236, 339, 248, 449},
		UserItem {
			enabled
		},
		/* [12] */
		{16, 376, 42, 454},
		UserItem {
			disabled
		},
		/* [13] */
		{16, 14, 32, 66},
		StaticText {
			disabled,
			"Source:"
		},
		/* [14] */
		{39, 109, 55, 360},
		StaticText {
			disabled,
			"As:"
		},
		/* [15] */
		{165, 17, 180, 63},
		StaticText {
			disabled,
			"Result"
		},
		/* [16] */
		{234, 337, 250, 451},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DITL' (10002, "Edit Links") {
	{	/* array DITLarray: 18 elements */
		/* [1] */
		{12, 359, 30, 477},
		Button {
			enabled,
			"Cancel"
		},
		/* [2] */
		{48, 359, 66, 477},
		Button {
			enabled,
			"Update Now"
		},
		/* [3] */
		{74, 359, 92, 477},
		Button {
			enabled,
			"Open Source"
		},
		/* [4] */
		{100, 359, 118, 477},
		Button {
			enabled,
			"Change Source."
		},
		/* [5] */
		{143, 359, 161, 477},
		Button {
			enabled,
			"Break Link"
		},
		/* [6] */
		{200, 359, 218, 477},
		Button {
			enabled,
			"Help"
		},
		/* [7] */
		{205, 80, 223, 166},
		RadioButton {
			enabled,
			"Automatic"
		},
		/* [8] */
		{205, 193, 223, 261},
		RadioButton {
			enabled,
			"Manual"
		},
		/* [9] */
		{29, 12, 145, 346},
		UserItem {
			disabled
		},
		/* [10] */
		{156, 83, 172, 346},
		UserItem {
			disabled
		},
		/* [11] */
		{183, 81, 199, 346},
		UserItem {
			disabled
		},
		/* [12] */
		{8, 355, 34, 481},
		UserItem {
			disabled
		},
		/* [13] */
		{10, 15, 26, 51},
		StaticText {
			disabled,
			"Links"
		},
		/* [14] */
		{10, 163, 26, 195},
		StaticText {
			disabled,
			"Type"
		},
		/* [15] */
		{10, 260, 26, 309},
		StaticText {
			disabled,
			"Update"
		},
		/* [16] */
		{156, 13, 172, 64},
		StaticText {
			disabled,
			"Source:"
		},
		/* [17] */
		{183, 14, 199, 50},
		StaticText {
			disabled,
			"Type:"
		},
		/* [18] */
		{206, 14, 222, 66},
		StaticText {
			disabled,
			"Update:"
		}
	}
};

resource 'DITL' (10004, "Convert") {
	{	/* array DITLarray: 15 elements */
		/* [1] */
		{20, 380, 38, 450},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{48, 380, 66, 450},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{76, 380, 94, 450},
		Button {
			enabled,
			"Help"
		},
		/* [4] */
		{174, 335, 192, 453},
		CheckBox {
			enabled,
			"Display As Icon"
		},
		/* [5] */
		{64, 12, 82, 110},
		RadioButton {
			enabled,
			"Convert to:"
		},
		/* [6] */
		{93, 12, 111, 110},
		RadioButton {
			enabled,
			"Activate as:"
		},
		/* [7] */
		{172, 12, 248, 318},
		UserItem {
			disabled
		},
		/* [8] */
		{58, 125, 154, 360},
		UserItem {
			enabled
		},
		/* [9] */
		{16, 110, 32, 360},
		UserItem {
			disabled
		},
		/* [10] */
		{197, 378, 229, 410},
		UserItem {
			enabled
		},
		/* [11] */
		{236, 339, 248, 449},
		UserItem {
			enabled
		},
		/* [12] */
		{16, 376, 42, 454},
		UserItem {
			disabled
		},
		/* [13] */
		{16, 14, 32, 106},
		StaticText {
			disabled,
			"Current Type:"
		},
		/* [14] */
		{39, 124, 55, 360},
		StaticText {
			disabled,
			"Object Type:"
		},
		/* [15] */
		{165, 17, 180, 63},
		StaticText {
			disabled,
			"Result"
		},
		/* [16] */
		{234, 337, 250, 451},
		EditText {
			enabled,
			""
		}
	}
};

resource 'DITL' (10005, "Busy") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{135, 34, 160, 105},
		Button {
			enabled,
			"Switch To"
		},
		/* [2] */
		{135, 145, 160, 216},
		Button {
			enabled,
			"Retry"
		},
		/* [3] */
		{135, 255, 160, 326},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{15, 30, 119, 326},
		UserItem {
			disabled
		},
		/* [5] */
		{131, 30, 164, 109},
		UserItem {
			disabled
		}
	}
};

resource 'DITL' (10006, "Update Links") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{75, 285, 95, 343},
		Button {
			enabled,
			"Stop"
		},
		/* [2] */
		{75, 10, 96, 265},
		UserItem {
			disabled
		},
		/* [3] */
		{45, 110, 61, 162},
		StaticText {
			disabled,
			"0%"
		},
		/* [4] */
		{15, 15, 30, 119},
		StaticText {
			disabled,
			"Update links..."
		}
	}
};

resource 'DITL' (10010, "Change Source") {
	{	/* array DITLarray: 14 elements */
		/* [1] */
		{1064, 1318, 1082, 1384},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{93, 318, 111, 384},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{0, 0, 0, 0},
		Button {
			enabled,
			"Hidden"
		},
		/* [4] */
		{67, 199, 82, 301},
		UserItem {
			enabled
		},
		/* [5] */
		{93, 210, 111, 290},
		Button {
			enabled,
			"Eject"
		},
		/* [6] */
		{118, 210, 136, 290},
		Button {
			enabled,
			"Desktop"
		},
		/* [7] */
		{63, 12, 161, 189},
		UserItem {
			enabled
		},
		/* [8] */
		{41, 12, 60, 190},
		UserItem {
			enabled
		},
		/* [9] */
		{60, 307, 140, 308},
		Picture {
			disabled,
			10001
		},
		/* [10] */
		{64, 318, 82, 384},
		Button {
			enabled,
			"OK"
		},
		/* [11] */
		{118, 318, 136, 384},
		Button {
			enabled,
			"Help"
		},
		/* [12] */
		{10, 70, 26, 353},
		EditText {
			enabled,
			""
		},
		/* [13] */
		{60, 314, 86, 388},
		UserItem {
			disabled
		},
		/* [14] */
		{10, 13, 26, 63},
		StaticText {
			disabled,
			"Source:"
		}
	}
};

resource 'DITL' (10020, "Invalid Source") {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{18, 70, 34, 361},
		StaticText {
			disabled,
			"Invalid Source: Do you want to correct i"
			"t?"
		},
		/* [2] */
		{56, 113, 76, 171},
		Button {
			enabled,
			"Yes"
		},
		/* [3] */
		{56, 199, 76, 257},
		Button {
			enabled,
			"No"
		},
		/* [4] */
		{10, 20, 42, 52},
		Icon {
			disabled,
			2
		}
	}
};

resource 'DITL' (10021, "Link Source Unavailable") {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{10, 70, 58, 361},
		StaticText {
			disabled,
			"This action cannot be completed because "
			"the selected link's source is presently "
			"unavailable."
		},
		/* [2] */
		{74, 156, 94, 214},
		Button {
			enabled,
			"OK"
		},
		/* [3] */
		{10, 20, 42, 52},
		Icon {
			disabled,
			0
		}
	}
};

resource 'DLOG' (10000, "Insert Object") {
	{48, 11, 295, 501},
	noGrowDocProc,
	invisible,
	noGoAway,
	0x0,
	10000,
	"Insert Object"
};

resource 'DLOG' (10001, "Paste Special") {
	{48, 21, 308, 491},
	noGrowDocProc,
	invisible,
	noGoAway,
	0x0,
	10001,
	"Paste Special"
};

resource 'DLOG' (10002, "Edit Links") {
	{48, 11, 278, 500},
	documentProc,
	invisible,
	noGoAway,
	0x0,
	10002,
	"Links"
};

resource 'DLOG' (10004, "Convert") {
	{48, 21, 308, 491},
	noGrowDocProc,
	invisible,
	noGoAway,
	0x0,
	10004,
	"Convert"
};

resource 'DLOG' (10005, "Busy") {
	{48, 76, 224, 436},
	noGrowDocProc,
	invisible,
	noGoAway,
	0x0,
	10005,
	"Server Busy"
};

resource 'DLOG' (10006, "Update Links") {
	{48, 78, 158, 434},
	noGrowDocProc,
	invisible,
	noGoAway,
	0x0,
	10006,
	"Update Links"
};

resource 'DLOG' (10010, "Change Source") {
	{48, 58, 221, 454},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	10010,
	"Change Source"
};

resource 'DLOG' (10020, "Invalid Source") {
	{48, 71, 140, 441},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	10020,
	"Invalid Source"
};

resource 'DLOG' (10021, "Link Source Unavailable") {
	{48, 71, 158, 441},
	dBoxProc,
	invisible,
	noGoAway,
	0x0,
	10021,
	"Invalid Source"
};

resource 'PICT' (10000, purgeable) {
	4104,
	{0, 0, 280, 60},
	$"0011 02FF 0C00 FFFE 0000 0048 0000 0048"
	$"0000 0000 0000 0118 003C 0000 0000 0001"
	$"000A 0000 0000 0118 003C 0098 8020 0000"
	$"0000 0118 003C 0000 0000 0000 0000 0048"
	$"0000 0048 0000 0000 0004 0001 0004 0000"
	$"0000 0000 0000 0000 0000 0047 EBDD 0000"
	$"0009 0000 FFFF FFFF FFFF 0001 FFFF 0000"
	$"FFFF 0002 C0C0 C0C0 C0C0 0003 BFBF 0000"
	$"BFBF 0004 8080 8080 8080 0005 0000 FFFF"
	$"0000 0006 0000 BFBF 0000 0007 0000 0000"
	$"FFFF 0008 0000 0000 BFBF 0009 0000 0000"
	$"0000 0000 0000 0118 003C 0000 0000 0118"
	$"003C 0000 02E1 0002 E100 02E1 0002 E100"
	$"02E1 0002 E100 06F5 00FD 99F1 0009 F500"
	$"0390 0000 09F1 000D F800 FE99 0390 9999"
	$"09FE 99F4 0010 F900 0309 0000 49FD 0003"
	$"9400 0290 F500 10F9 0003 0902 2290 FD00"
	$"0309 2224 90F5 000E F900 0209 0222 FB99"
	$"0222 2490 F500 0CF9 0001 0902 F922 0124"
	$"90F5 000C F900 0109 02F9 2201 2490 F500"
	$"0CF9 0001 0902 F922 0124 90F5 000C F900"
	$"0109 02F9 2201 2490 F500 0DF9 0001 0902"
	$"FD22 FC99 0090 F500 0FF9 0001 0902 FD22"
	$"0090 FD00 0099 F500 0FF9 0001 0902 FD22"
	$"0090 FD00 FF90 F600 10F9 0001 0902 FD22"
	$"0090 FD00 0190 09F6 0011 F900 0109 02FD"
	$"2200 90FD 00FF 9900 90F7 0012 F900 0109"
	$"02FD 2207 9004 4440 0002 2294 F700 0FF9"
	$"0001 0902 FD22 0090 FB00 0094 F700 11F9"
	$"0001 0902 FD22 0190 04FD 4401 0094 F700"
	$"0FF9 0001 0902 FD22 0090 FB00 0094 F700"
	$"11F9 0001 0902 FD22 0190 04FD 4401 0094"
	$"F700 0FF9 0001 0902 FD22 0090 FB00 0094"
	$"F700 11F9 0001 0902 FD22 0190 04FD 4401"
	$"0094 F700 0FF9 0001 0902 FD22 0090 FB00"
	$"0094 F700 11F9 0001 0924 FD44 0190 04FD"
	$"4401 0094 F700 0CF8 00FC 9900 90FB 0000"
	$"94F7 000A F300 0090 FB00 0094 F700 0AF3"
	$"0000 90FB 0000 94F7 0008 F300 FA99 0094"
	$"F700 08F3 0000 04FA 44F7 0002 E100 02E1"
	$"0002 E100 02E1 0002 E100 02E1 0002 E100"
	$"02E1 0002 E100 0AF5 0000 09F9 9900 90F7"
	$"000A F500 0009 F900 0099 F700 0AF5 0000"
	$"09F9 00FF 90F8 000B F500 0009 F900 0190"
	$"09F8 000C F500 0009 F900 0290 0090 F900"
	$"0CF5 0000 09F9 0002 9000 09F9 000C F500"
	$"0009 F900 FE99 0090 FA00 12FD 0000 09F8"
	$"9901 9009 FC00 0302 2222 94FA 000F FD00"
	$"0109 22F9 8800 92F8 0000 94FA 0011 FD00"
	$"0009 F888 0392 0000 90FB 0000 94FA 000E"
	$"FD00 0009 F800 0092 F800 0094 FA00 12FD"
	$"0000 09F8 0000 92FE 0000 09FC 0000 94FA"
	$"0010 FD00 0009 F800 0292 0080 FA00 0094"
	$"FA00 15FD 0002 0900 08FA 0005 9200 8200"
	$"0777 FD00 0094 FA00 1AFD 0006 0900 0820"
	$"0077 70FE 0006 9200 8200 0777 20FE 0000"
	$"94FA 001A FD00 0609 0008 2000 7772 FE00"
	$"0A92 0082 0007 7766 6000 0094 FA00 1AFD"
	$"0014 0900 0820 0077 7666 0000 9200 8200"
	$"0777 6662 0000 94FA 001A FD00 1409 0008"
	$"2000 7776 6620 0092 0082 3337 7766 6200"
	$"0094 FA00 1AFD 0014 0900 0823 3377 7666"
	$"2000 9200 8233 3777 6662 0000 94FA 001A"
	$"FD00 1409 0008 2333 7776 6620 0092 0082"
	$"3337 7766 6200 0094 FA00 1AFD 0014 0900"
	$"0823 3377 7666 2000 9200 8233 3777 6662"
	$"0000 94FA 0017 FD00 0B09 0008 2333 7776"
	$"6620 0092 00FB 8802 8000 94FA 0016 FD00"
	$"0209 0008 FB88 0500 9200 0222 29FD 2201"
	$"0094 FA00 11FD 0002 0900 00FB 2201 2092"
	$"F800 0094 FA00 11FD 0000 09F8 0003 9200"
	$"0090 FB00 0094 FA00 0EFD 0000 09F8 0000"
	$"92F8 0000 94FA 000F FD00 0009 F899 0192"
	$"09F9 0000 94FA 000C F500 0209 2222 F800"
	$"0094 FA00 0AF5 0000 09F6 0000 94FA 000A"
	$"F500 0009 F600 0094 FA00 0AF5 0000 09F6"
	$"9900 94FA 0006 F400 F544 FA00 02E1 0002"
	$"E100 02E1 0002 E100 02E1 0002 E100 02E1"
	$"0002 E100 0AF5 0000 09F9 9900 90F7 000A"
	$"F500 0009 F900 0099 F700 0AF5 0000 09F9"
	$"00FF 90F8 000B F500 0009 F900 0190 09F8"
	$"000C F500 0009 F900 0290 0090 F900 0CF5"
	$"0000 09F9 0002 9000 09F9 000C F500 0009"
	$"F900 FE99 0090 FA00 12FD 0000 09F8 9901"
	$"9009 FC00 0302 2222 94FA 000F FD00 0109"
	$"22F9 8800 92F8 0000 94FA 0011 FD00 0009"
	$"F888 0392 0000 90FB 0000 94FA 000E FD00"
	$"0009 F800 0092 F800 0094 FA00 12FD 0000"
	$"09F8 0000 92FE 0000 09FC 0000 94FA 000E"
	$"FD00 0009 F800 0092 F800 0094 FA00 10FD"
	$"0002 0900 08FA 0000 92F8 0000 94FA 001A"
	$"FD00 0609 0008 2000 7770 FE00 0692 0000"
	$"0333 3330 FE00 0094 FA00 1AFD 0006 0900"
	$"0820 0077 72FE 0006 9200 0003 3333 32FE"
	$"0000 94FA 001A FD00 1009 0008 2000 7776"
	$"6600 0092 0000 0777 7772 FE00 0094 FA00"
	$"1AFD 0010 0900 0820 0077 7666 2000 9200"
	$"0007 7777 72FE 0000 94FA 001A FD00 1009"
	$"0008 2333 7776 6620 0092 0000 0666 6662"
	$"FE00 0094 FA00 1AFD 0010 0900 0823 3377"
	$"7666 2000 9200 0006 6666 62FE 0000 94FA"
	$"0018 FD00 0A09 0008 2333 7776 6620 0092"
	$"FE00 FE22 FE00 0094 FA00 14FD 000A 0900"
	$"0823 3377 7666 2000 92F8 0000 94FA 0015"
	$"FD00 0209 0008 FB88 0100 92FE 0000 09FC"
	$"0000 94FA 0011 FD00 0209 0000 FB22 0120"
	$"92F8 0000 94FA 0011 FD00 0009 F800 0392"
	$"0000 90FB 0000 94FA 000E FD00 0009 F800"
	$"0092 F800 0094 FA00 0FFD 0000 09F8 9901"
	$"9209 F900 0094 FA00 0CF5 0002 0922 22F8"
	$"0000 94FA 000A F500 0009 F600 0094 FA00"
	$"0AF5 0000 09F6 0000 94FA 000A F500 0009"
	$"F699 0094 FA00 06F4 00F5 44FA 0002 E100"
	$"02E1 0002 E100 02E1 0002 E100 02E1 0002"
	$"E100 02E1 0002 E100 02E1 0002 E100 02E1"
	$"0008 FE00 FB99 0090 EB00 10FE 0000 90FC"
	$"0000 99F9 00FB 9900 90FA 0012 FE00 0090"
	$"FC00 FF90 FA00 0090 FC00 0099 FA00 13FE"
	$"0000 90FC 0001 9009 FA00 0090 FC00 FF90"
	$"FB00 15FE 0000 90FC 0002 9000 90FB 0000"
	$"90FC 0001 9009 FB00 14FE 0000 90FC 00FE"
	$"99FB 0000 90FC 0002 9000 90FC 0015 FE00"
	$"0090 FC00 0302 2229 40FC 0000 90FC 00FE"
	$"99FC 0017 FE00 0190 07FB 0001 0940 FC00"
	$"0090 FC00 0302 2229 40FD 0018 FE00 0990"
	$"0720 0077 7000 0009 40FC 0000 90FA 0001"
	$"0940 FD00 1BFE 0009 9007 2000 7772 0000"
	$"0940 FE00 0390 0090 04FB 0001 0940 FD00"
	$"1EFE 0009 9007 2000 7775 5500 0940 FE00"
	$"0B99 0090 0400 0044 4000 0009 40FD 001D"
	$"FE00 0990 0720 0077 7555 2009 40FD 99FF"
	$"9008 0400 0044 4000 0009 40FD 001E FE00"
	$"0990 0721 1177 7555 2009 40FE 0005 9900"
	$"9004 0000 FE44 0200 0940 FD00 1EFE 0009"
	$"9007 2111 7775 5520 0940 FE00 0590 0090"
	$"0400 00FE 4402 0009 40FD 001B FE00 0990"
	$"0721 1177 7555 2009 40FC 0002 9004 04FD"
	$"4402 0009 40FD 001B FE00 0990 0721 1177"
	$"7555 2009 40FC 0002 9004 04FD 4402 0009"
	$"40FD 0018 FE00 0190 07FB 7701 0940 FC00"
	$"0290 0404 FD44 0200 0940 FD00 18FE 0001"
	$"9000 FB22 0129 40FC 0002 9004 04FD 4402"
	$"0009 40FD 0015 FE00 0090 FA00 0109 40FC"
	$"0001 9004 FB44 0109 40FD 0014 FE00 0090"
	$"FA00 0109 40FC 0000 90FA 0001 0940 FD00"
	$"11FE 00F8 9900 40FC 0000 90FA 0001 0940"
	$"FD00 0EFD 00F9 4400 40FC 00F8 9900 40FD"
	$"0008 EE00 F944 0040 FD00 02E1 0002 E100"
	$"02E1 0002 E100 02E1 0002 E100 02E1 0002"
	$"E100 02E1 0002 E100 02E1 0002 E100 02E1"
	$"0002 E100 02E1 0002 E100 02E1 0008 FE00"
	$"FB99 0090 EB00 10FE 0000 90FC 0000 99F9"
	$"00FB 9900 90FA 0012 FE00 0090 FC00 FF90"
	$"FA00 0090 FC00 0099 FA00 13FE 0000 90FC"
	$"0001 9009 FA00 0090 FC00 FF90 FB00 15FE"
	$"0000 90FC 0002 9000 90FB 0000 90FC 0001"
	$"9009 FB00 14FE 0000 90FC 00FE 99FB 0000"
	$"90FC 0002 9000 90FC 0015 FE00 0090 FC00"
	$"0302 2229 40FC 0000 90FC 00FE 99FC 0017"
	$"FE00 0190 07FB 0001 0940 FC00 0090 FC00"
	$"0302 2229 40FD 0018 FE00 0990 0720 0077"
	$"7000 0009 40FC 0000 90FA 0001 0940 FD00"
	$"1AFE 0009 9007 2000 7772 0000 0940 FE00"
	$"0290 0090 FA00 0109 40FD 001E FE00 0990"
	$"0720 0077 7555 0009 40FE 000B 9900 9000"
	$"0444 4440 0000 0940 FD00 1DFE 0009 9007"
	$"2000 7775 5520 0940 FD99 FF90 0800 0444"
	$"4442 0000 0940 FD00 1EFE 0009 9007 2111"
	$"7775 5520 0940 FE00 0B99 0090 0004 4444"
	$"4200 0009 40FD 001E FE00 0990 0721 1177"
	$"7555 2009 40FE 000B 9000 9000 0444 4442"
	$"0000 0940 FD00 1CFE 0009 9007 2111 7775"
	$"5520 0940 FC00 0990 0004 4444 4200 0009"
	$"40FD 001C FE00 0990 0721 1177 7555 2009"
	$"40FC 0009 9000 0444 4442 0000 0940 FD00"
	$"19FE 0001 9007 FB77 0109 40FC 0002 9000"
	$"00FE 22FF 0001 0940 FD00 15FE 0001 9000"
	$"FB22 0129 40FC 0000 90FA 0001 0940 FD00"
	$"14FE 0000 90FA 0001 0940 FC00 0090 FA00"
	$"0109 40FD 0014 FE00 0090 FA00 0109 40FC"
	$"0000 90FA 0001 0940 FD00 11FE 00F8 9900"
	$"40FC 0000 90FA 0001 0940 FD00 0EFD 00F9"
	$"4400 40FC 00F8 9900 40FD 0008 EE00 F944"
	$"0040 FD00 02E1 0002 E100 02E1 0002 E100"
	$"02E1 0002 E100 02E1 0002 E100 02E1 0002"
	$"E100 02E1 0002 E100 02E1 0002 E100 02E1"
	$"0002 E100 02E1 0008 FE00 FB99 0090 EB00"
	$"10FE 0000 90FC 0000 99F9 00FB 9900 90FA"
	$"0012 FE00 0090 FC00 FF90 FA00 0090 FC00"
	$"0099 FA00 13FE 0000 90FC 0001 9009 FA00"
	$"0090 FC00 FF90 FB00 15FE 0000 90FC 0002"
	$"9000 90FB 0000 90FC 0001 9009 FB00 14FE"
	$"0000 90FC 00FE 99FB 0000 90FC 0002 9000"
	$"90FC 0015 FE00 0090 FC00 0302 2229 40FC"
	$"0000 90FC 00FE 99FC 0016 FE00 0090 FA00"
	$"0109 40FC 0000 90FC 0003 0222 2940 FD00"
	$"15FE 0001 9004 FB00 0109 40FC 0000 90FA"
	$"0001 0940 FD00 1BFE 0009 9004 0000 4440"
	$"0000 0940 FE00 0390 0090 04FB 0001 0940"
	$"FD00 1EFE 0009 9004 0000 4440 0000 0940"
	$"FE00 0B99 0090 0400 0044 4000 0009 40FD"
	$"001D FE00 0390 0400 00FE 4402 0009 40FD"
	$"99FF 9008 0400 0044 4000 0009 40FD 001E"
	$"FE00 0390 0400 00FE 4402 0009 40FE 0005"
	$"9900 9004 0000 FE44 0200 0940 FD00 1DFE"
	$"0002 9004 04FD 4402 0009 40FE 0005 9000"
	$"9004 0000 FE44 0200 0940 FD00 1AFE 0002"
	$"9004 04FD 4402 0009 40FC 0002 9004 04FD"
	$"4402 0009 40FD 001A FE00 0290 0404 FD44"
	$"0200 0940 FC00 0290 0404 FD44 0200 0940"
	$"FD00 1AFE 0002 9004 04FD 4402 0009 40FC"
	$"0002 9004 04FD 4402 0009 40FD 0018 FE00"
	$"0190 04FB 4401 0940 FC00 0290 0404 FD44"
	$"0200 0940 FD00 15FE 0000 90FA 0001 0940"
	$"FC00 0190 04FB 4401 0940 FD00 14FE 0000"
	$"90FA 0001 0940 FC00 0090 FA00 0109 40FD"
	$"0011 FE00 F899 0040 FC00 0090 FA00 0109"
	$"40FD 000E FD00 F944 0040 FC00 F899 0040"
	$"FD00 08EE 00F9 4400 40FD 0002 E100 02E1"
	$"0002 E100 02E1 0002 E100 02E1 0002 E100"
	$"02E1 0002 E100 02E1 0002 E100 02E1 0002"
	$"E100 02E1 0002 E100 02E1 0002 E100 08FE"
	$"00FB 9900 90EB 0010 FE00 0090 FC00 0099"
	$"F900 FB99 0090 FA00 12FE 0000 90FC 00FF"
	$"90FA 0000 90FC 0000 99FA 0013 FE00 0090"
	$"FC00 0190 09FA 0000 90FC 00FF 90FB 0015"
	$"FE00 0090 FC00 0290 0090 FB00 0090 FC00"
	$"0190 09FB 0014 FE00 0090 FC00 FE99 FB00"
	$"0090 FC00 0290 0090 FC00 15FE 0000 90FC"
	$"0003 0222 2940 FC00 0090 FC00 FE99 FC00"
	$"16FE 0000 90FA 0001 0940 FC00 0090 FC00"
	$"0302 2229 40FD 0015 FE00 0190 04FB 0001"
	$"0940 FC00 0090 FA00 0109 40FD 001A FE00"
	$"0990 0400 0044 4000 0009 40FE 0002 9000"
	$"90FA 0001 0940 FD00 1EFE 0009 9004 0000"
	$"4440 0000 0940 FE00 0B99 0090 0004 4444"
	$"4000 0009 40FD 001D FE00 0390 0400 00FE"
	$"4402 0009 40FD 99FF 9008 0004 4444 4200"
	$"0009 40FD 001E FE00 0390 0400 00FE 4402"
	$"0009 40FE 000B 9900 9000 0444 4442 0000"
	$"0940 FD00 1DFE 0002 9004 04FD 4402 0009"
	$"40FE 000B 9000 9000 0444 4442 0000 0940"
	$"FD00 1BFE 0002 9004 04FD 4402 0009 40FC"
	$"0009 9000 0444 4442 0000 0940 FD00 1BFE"
	$"0002 9004 04FD 4402 0009 40FC 0009 9000"
	$"0444 4442 0000 0940 FD00 1BFE 0002 9004"
	$"04FD 4402 0009 40FC 0002 9000 00FE 22FF"
	$"0001 0940 FD00 15FE 0001 9004 FB44 0109"
	$"40FC 0000 90FA 0001 0940 FD00 14FE 0000"
	$"90FA 0001 0940 FC00 0090 FA00 0109 40FD"
	$"0014 FE00 0090 FA00 0109 40FC 0000 90FA"
	$"0001 0940 FD00 11FE 00F8 9900 40FC 0000"
	$"90FA 0001 0940 FD00 0EFD 00F9 4400 40FC"
	$"00F8 9900 40FD 0008 EE00 F944 0040 FD00"
	$"02E1 0002 E100 02E1 0002 E100 02E1 0002"
	$"E100 02E1 0002 E100 02E1 0000 00FF"
};

resource 'PICT' (10001, purgeable) {
	156,
	{0, 0, 1, 1},
	$"0011 02FF 0C00 FFFE 0000 0048 0000 0048"
	$"0000 0000 0000 0001 0001 0000 0000 0001"
	$"000A 0000 0000 0001 0001 0090 8004 0000"
	$"0000 0001 0001 0000 0000 0000 0000 0048"
	$"0000 0048 0000 0000 0002 0001 0002 0000"
	$"0000 0000 0000 0000 0000 0048 432F 0000"
	$"0002 0000 FFFF FFFF FFFF 0001 7D7D 7D7D"
	$"7D7D 0002 0000 0000 0000 0000 0000 0001"
	$"0001 0000 0000 0001 0001 0000 4000 0000"
	$"00FF"
};

resource 'STR#' (10000, "Insert Object") {
	{	/* array StringArray: 6 elements */
		/* [1] */
		"Inserts a new %s object into your docume"
		"nt.",
		/* [2] */
		"Inserts a new %s object into your docume"
		"nt. It will be displayed as an icon.",
		/* [3] */
		"Inserts the contents of the file as an o"
		"bject into your document so that you can"
		" activate it using the application that "
		"created it.",
		/* [4] */
		"Inserts the contents of the file as an o"
		"bject into your document so that you can"
		" activate it using the application that "
		"created it. It will be displayed as an i"
		"con.",
		/* [5] */
		"Inserts a picture of the file contents i"
		"nto your document. The picture will be l"
		"inked to the file so that changes to the"
		" file will be reflected in your document"
		".",
		/* [6] */
		"Inserts an icon into your document which"
		" represents the file. The icon will be l"
		"inked to the file so that changes to the"
		" file will be reflected in your document"
		"."
	}
};

resource 'STR#' (10001, "Paste Special") {
	{	/* array StringArray: 10 elements */
		/* [1] */
		"Pastes the contents of the Clipboard int"
		"o your document as %s.",
		/* [2] */
		"Pastes the contents of the Clipboard int"
		"o your document so that you can activate"
		" it using %s.",
		/* [3] */
		"Pastes the contents of the Clipboard int"
		"o your document so that you can activate"
		" it using %s.  It will be displayed as a"
		"n icon.",
		/* [4] */
		"Pastes the contents of the Clipboard int"
		"o your document as %s. Paste Link create"
		"s a link to the source file so that chan"
		"ges to the source file will be reflected"
		" in your document.",
		/* [5] */
		"Pastes a picture of the Clipboard conten"
		"ts into your document. Paste Link create"
		"s a link to the source file so that chan"
		"ges to the source file will be reflected"
		" in your document.",
		/* [6] */
		"Pastes an icon into your document which "
		"represents the Clipboard contents. Paste"
		" Link creates a link to the source file "
		"so that changes to the source file will "
		"be reflected in your document.",
		/* [7] */
		"Pastes the contents of the clipboard int"
		"o your document.",
		/* [8] */
		"Unknown Type",
		/* [9] */
		"Unknown Source",
		/* [10] */
		"the application which created it"
	}
};

resource 'STR#' (10002, "Edit Links") {
	{	/* array StringArray: 11 elements */
		/* [1] */
		"Automatic",
		/* [2] */
		"Manual",
		/* [3] */
		"Unavail",
		/* [4] */
		"Links",
		/* [5] */
		"Operation failed!",
		/* [6] */
		"Change Source",
		/* [7] */
		"Invalid Source: Do you want to correct i"
		"t?",
		/* [8] */
		"The selected link has been changed. This"
		" document contains additional links to %"
		"s. Change additional links?",
		/* [9] */
		"Fail to get source of the link!",
		/* [10] */
		"Fail to get update option of the link!",
		/* [11] */
		"Fail to add item to ListBox!",
        /* [12] */
        "Ok",
        /* [13] */
        "Close",
        /* [14] */
        "Cancel",
        /* [15] */
        "Open"
	}
};

resource 'STR#' (10004, "Convert") {
	{	/* array StringArray: 6 elements */
		/* [1] */
		"A linked object must be converted at the"
		" source.",
		/* [2] */
		"Permanently changes the selected %s to a"
		" %s.",
		/* [3] */
		"The selected %s will not be converted as"
		" it is the same as the current type.",
		/* [4] */
		" It will be displayed as an icon.",
		/* [5] */
		"Every %s will be activated as %s",
		/* [6] */
		", but it will not be converted."
	}
};

#if !defined(__powerc)

data 'LDEF' (10001, "Edit Links LDEF") {
	$"600E 0000 4C44 4546 2711 0000 0000 0000"
	$"41FA FFEE 4E71 4E71 6000 0002 4E56 FFF8"
	$"48E7 0F30 266E 0014 1E2E 0018 246E 0008"
	$"302E 001A 6700 01AA 6B00 01A6 5740 6700"
	$"01A0 6A00 019C 4A6E 000C 6F00 0194 486E"
	$"FFFC A874 2052 2F28 0008 A873 2052 2068"
	$"0008 3C28 0044 3F3C 0003 A887 3D7C 0004"
	$"000C 486E FFF8 486E 000C 2F2E 0010 2F0A"
	$"3F3C 0038 A9E7 4A07 6704 7021 6002 701E"
	$"2F00 A862 2F0B A8A2 4878 0021 A862 4A07"
	$"670C 4878 001E A862 4878 0021 A863 3A2B"
	$"0006 206E FFF8 3028 00F6 D16B 0002 206E"
	$"FFF8 302B 0002 D068 00F8 9068 00F6 3740"
	$"0006 206E FFF8 4AA8 006C 6732 7800 6002"
	$"5204 206E FFF8 1004 4880 2068 006C 4A30"
	$"0000 66EC 206E FFF8 2F28 006C 1004 4880"
	$"3240 2F09 2F0B 3F3C FFFE A9CE 6004 2F0B"
	$"A8A3 206E FFF8 3028 00F8 9068 00F6 D16B"
	$"0002 206E FFF8 3028 00FA 9068 00F8 D16B"
	$"0006 206E FFF8 4AA8 00D4 6732 7800 6002"
	$"5204 206E FFF8 1004 4880 2068 00D4 4A30"
	$"0000 66EC 206E FFF8 2F28 00D4 1004 4880"
	$"3240 2F09 2F0B 3F3C FFFE A9CE 6004 2F0B"
	$"A8A3 206E FFF8 3028 00FA 9068 00F8 D16B"
	$"0002 3745 0006 206E FFF8 4AA8 00E8 6732"
	$"7800 6002 5204 206E FFF8 1004 4880 2068"
	$"00E8 4A30 0000 66EC 206E FFF8 2F28 00E8"
	$"1004 4880 3240 2F09 2F0B 3F3C FFFE A9CE"
	$"6004 2F0B A8A3 4A07 670C 4878 0021 A862"
	$"4878 001E A863 3F06 A887 2F2E FFFC A873"
	$"4CDF 0CF0 4E5E 205F 4FEF 0014 4ED0 846D"
	$"6169 6E00 0000"
};

#endif

#ifdef __powerc

data 'PROC' (10042, "LSPatch", purgeable) {
	$"600E 0000 5052 4F43 273A 0000 0000 0000"
	$"41FA FFEE 4E71 4E71 6000 000C 6104 0000"
	$"0000 225F 4E75 4EBA FFF4 2288 2F0C 4EBA"
	$"FFEC 2851 303C A9E7 A746 2948 0098 41FA"
	$"000C 303C A9E7 A647 285F 4E75 4E56 FFF8"
	$"41FA FFD4 2D48 FFFC 7026 91AE FFFC 0C6E"
	$"005C 0008 6618 2D4C FFF8 286E FFFC 303C"
	$"A9E7 206C 0098 A647 286E FFF8 6014 2D4C"
	$"FFF8 286E FFFC 206C 0098 286E FFF8 4E5E"
	$"4ED0 4E5E 4E74 000C 0000 0000"
};

#endif

// string resource for ole2ui.c
resource 'CSTR' (11000, locked, preload) {
	"Unknown"
};

resource 'CSTR' (11001, locked, preload) {
	"Link"
};

resource 'CSTR' (11002, locked, preload) {
	"Object"
};

resource 'CSTR' (11003, locked, preload) {
	"Edit"
};

resource 'CSTR' (11004, locked, preload) {
	"Convert..."
};

resource 'CSTR' (11005, locked, preload) {
	"0%s Linked %s &Object"
};

resource 'CSTR' (11006, locked, preload) {
	"0%s %s Object"
};

resource 'CSTR' (11007, locked, preload) {
	"Linked %s Object"
};

resource 'CSTR' (11008, locked, preload) {
	"%s Object"
};

resource 'CSTR' (11009, locked, preload) {
	"Object"
};

// string resource for Busy Dialog
resource 'CSTR' (11010, locked, preload) {
	"This action cannot be completed because the process "
    "(%s) is not responding. Choose \"Switch To\" to activate '%s' and "
    "correct the problem."
};

resource 'CSTR' (11011, locked, preload) {
	"This action cannot be completed because the process "
    "(%s) is busy. Choose \"Switch To\" to activate '%s' and "
    "correct the problem."
};

// string resource for Message Filter
resource 'CSTR' (11012, locked, preload) {
	"The Application (%s) is blocked by an OLE call. If %s is still blocked when "
	"it comes to the foreground, a busy dialog will appear."
};

