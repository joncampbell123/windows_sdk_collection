#ifdef _MPPC_
/////////////////////////////////////////////////////////////////////////
// Code Fragment resources

#include    "CodeFrag.r"
#endif

/////////////////////////////////////////////////////////////////////////
// MultSkel resources

#include "types.r"

/*
 * here is the quintessential MultiFinder friendliness 
 * device, the SIZE resource
 *
 * See types.r 'SIZE' resource for more details
 */

resource 'SIZE' (-1) {
	dontSaveScreen,
	acceptSuspendResumeEvents,
	enableOptionSwitch,
	canBackground,		
	multiFinderAware,	
	backgroundAndForeground,
	dontGetFrontClicks,	
	ignoreChildDiedEvents,	
	not32BitCompatible,	
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
#ifdef _MPPC_
	2500 * 1024,
	2500 * 1024
#else	// 68K Mac
#ifdef _DEBUG
	3000 * 1024,
	3000 * 1024
#else
	2000 * 1024,
	2000 * 1024
#endif
#endif
};

data 'ALRT' (1000, "About") {
	$"0032 0048 00B4 0154 03E8 4444"                      /* .2.H.¥.T..DD */
};

data 'DITL' (1000, "About", purgeable) {
	$"0002 0000 0000 0060 00B5 0074 00F1 0402"            /* .......`.µ.t.... */
	$"4F4B 0000 0000 0011 0043 0050 00FA 884A"            /* OK.......C.P..àJ */
	$"4D75 6C74 6953 6B65 6C20 7665 7273 696F"            /* MultiSkel versio */
	$"6E20 322E 300D 0D54 7261 6E73 536B 656C"            /* n 2.0¬¬TransSkel */
	$"204D 756C 7469 706C 652D 5769 6E64 6F77"            /*  Multiple-Window */
	$"2041 7070 6C69 6361 7469 6F6E 2044 656D"            /*  Application Dem */
	$"6F6E 7374 7261 7469 6F6E 0000 0000 0018"            /* onstration...... */
	$"0012 0038 0032 A002 0080"                           /* ...8.2†..Ä */
};

data 'ICN#' (128) {
	$"0000 0000 000F F000 0078 1C00 01C0 0600"            /* .........x...¿.. */
	$"0300 0300 0200 0180 0400 0080 0400 00C0"            /* .......Ä...Ä...¿ */
	$"0800 0070 0800 0390 103E 0668 2063 0818"            /* ...p...ê.>.h c.. */
	$"2040 97C8 20BE 07C8 20BE 0790 203E 67A0"            /*  @ó» æ.» æ.ê >g† */
	$"101C 7320 1000 F010 0800 F810 0401 FC10"            /* ..s ............ */
	$"0201 F810 0100 0020 0080 0040 0042 4A40"            /* ....... .Ä.@.BJ@ */
	$"0042 5540 0047 5680 0022 AA80 0020 A880"            /* .BU@.GVÄ."™Ä. ®Ä */
	$"0010 0080 000C 0700 0003 FC00 0000 0000"            /* ...Ä............ */
	$"000F F800 007F FC00 03FF FE00 07FF FF00"            /* ................ */
	$"07FF FF80 07FF FFC0 0FFF FFE0 1FFF FFF0"            /* ...Ä...¿........ */
	$"1FFF FFF8 3FFF FFFC 3FFF FFFC 7FFF FFFC"            /* ....?...?....... */
	$"7FFF FFFC 7FFF FFFC 7FFF FFF8 7FFF FFF0"            /* ................ */
	$"3FFF FFF8 3FFF FFF8 3FFF FFF8 1FFF FFF8"            /* ?...?...?....... */
	$"0FFF FFF8 07FF FFF0 03FF FFE0 01FF FFE0"            /* ................ */
	$"00FF FFE0 00FF FFC0 007F FFC0 007F FFC0"            /* .......¿...¿...¿ */
	$"007F FFC0 003F FFC0 000F FF80 0007 FE00"            /* ...¿.?.¿...Ä.... */
};

data 'ICON' (128) {
	$"0000 0000 001F E000 00F0 3800 0380 0C00"            /* ..........8..Ä.. */
	$"0600 0600 0400 0300 0800 0100 0800 0180"            /* ...............Ä */
	$"1000 00E0 1000 0720 207C 0CD0 60C6 1030"            /* .......  |.–`∆.0 */
	$"4081 2F90 417C 0F90 417C 0F20 407C CF40"            /* @Å/êA|.êA|. @|œ@ */
	$"2038 E640 2001 E020 1001 F020 0803 F820"            /*  8.@ .. ... ...  */
	$"0403 F020 0200 0040 0100 0080 0084 9480"            /* ... ...@...Ä.ÑîÄ */
	$"0084 AA80 008E AD00 0045 5500 0041 5100"            /* .Ñ™Ä.é≠..EU..AQ. */
	$"0060 0100 0018 0E00 0007 F800 0000 0000"            /* .`.............. */
};

data 'MENU' (1002, "Edit") {
	$"03EA 0000 0000 0000 0000 FFFF FFFB 0445"            /* ...............E */
	$"6469 7404 556E 646F 005A 0000 012D 0000"            /* dit.Undo.Z...-.. */
	$"0000 0343 7574 0058 0000 0443 6F70 7900"            /* ...Cut.X...Copy. */
	$"4300 0005 5061 7374 6500 5600 0005 436C"            /* C...Paste.V...Cl */
	$"6561 7200 0000 0000"                                /* ear..... */
};

data 'MENU' (1001, "File") {
	$"03E9 0000 0000 0000 0000 FFFF FFF7 0446"            /* ...............F */
	$"696C 6505 4F70 656E C900 4F00 0005 436C"            /* ile.Open….O...Cl */
	$"6F73 6500 0000 0001 2D00 0000 0004 5175"            /* ose.....-.....Qu */
	$"6974 0051 0000 00"                                  /* it.Q... */
};

data 'MENU' (1000, "Apple") {
	$"03E8 0000 0000 0000 0000 FFFF FFFB 0114"            /* ................ */
	$"1041 626F 7574 204D 756C 7469 536B 656C"            /* .About MultiSkel */
	$"C900 0000 0001 2D00 0000 0000"                      /* ….....-..... */
};

data 'WIND' (1000, "Help") {
	$"0078 000E 014E 01A8 0008 0100 0100 0000"            /* .x...N.®........ */
	$"0000 0B48 656C 7020 5769 6E64 6F77"                 /* ...Help Window */
};

data 'WIND' (1003, "Rgn") {
	$"002D 005A 0104 01F4 0008 0100 0100 0000"            /* .-.Z............ */
	$"0000 0D52 6567 696F 6E20 5769 6E64 6F77"            /* ..¬Region Window */
};

data 'WIND' (1001, "Edit") {
	$"005F 0028 0136 01C2 0008 0100 0100 0000"            /* ._.(.6.¬........ */
	$"0000 0B45 6469 7420 5769 6E64 6F77"                 /* ...Edit Window */
};

data 'WIND' (1002, "Zoom") {
	$"0046 0041 011D 01DB 0008 0100 0100 0000"            /* .F.A............ */
	$"0000 0B5A 6F6F 6D20 5769 6E64 6F77"                 /* ...Zoom Window */
};

data 'TEXT' (1000, "Help Text") {
	$"5769 6E64 6F77 730D 0D54 6869 7320 7769"            /* Windows¬¬This wi */
	$"6E64 6F77 2069 7320 7468 6520 4D75 6C74"            /* ndow is the Mult */
	$"6953 6B65 6C20 4865 6C70 2057 696E 646F"            /* iSkel Help Windo */
	$"772E 2020 4974 2065 7870 6C61 696E 7320"            /* w.  It explains  */
	$"7468 6520 6675 6E63 7469 6F6E 206F 6620"            /* the function of  */
	$"7468 6520 6F74 6865 7220 4D75 6C74 6953"            /* the other MultiS */
	$"6B65 6C20 5769 6E64 6F77 732C 2061 7320"            /* kel Windows, as  */
	$"7765 6C6C 2061 7320 7468 6520 4D65 6E75"            /* well as the Menu */
	$"732E 0D0D 5468 6520 5465 7874 2057 696E"            /* s.¬¬The Text Win */
	$"646F 7720 6973 2061 2073 696D 706C 6520"            /* dow is a simple  */
	$"7465 7874 2065 6469 7469 6E67 2077 696E"            /* text editing win */
	$"646F 772C 2069 6E20 7768 6963 6820 7374"            /* dow, in which st */
	$"616E 6461 7264 204D 6163 696E 746F 7368"            /* andard Macintosh */
	$"2065 6469 7469 6E67 206F 7065 7261 7469"            /*  editing operati */
	$"6F6E 7320 6D61 7920 6265 2070 6572 666F"            /* ons may be perfo */
	$"726D 6564 2E20 2054 6865 2045 6469 7420"            /* rmed.  The Edit  */
	$"6D65 6E75 206D 6179 2062 6520 7573 6564"            /* menu may be used */
	$"2077 6974 6820 7468 6973 2077 696E 646F"            /*  with this windo */
	$"772E 0D0D 5468 6520 5A6F 6F6D 2057 696E"            /* w.¬¬The Zoom Win */
	$"646F 7720 636F 6E74 6169 6E73 2061 2073"            /* dow contains a s */
	$"696D 706C 6520 6772 6170 6869 6373 2064"            /* imple graphics d */
	$"6973 706C 6179 2C20 7768 6963 6820 6164"            /* isplay, which ad */
	$"6A75 7374 7320 6974 7365 6C66 2074 6F20"            /* justs itself to  */
	$"7468 6520 7369 7A65 206F 6620 7468 6520"            /* the size of the  */
	$"7769 6E64 6F77 2E20 2043 6C69 636B 2074"            /* window.  Click t */
	$"6865 206D 6F75 7365 2069 6E20 7468 6973"            /* he mouse in this */
	$"2077 696E 646F 7720 746F 2070 6175 7365"            /*  window to pause */
	$"2074 6865 2064 6973 706C 6179 2E0D 0D53"            /*  the display.¬¬S */
	$"696D 706C 6520 7265 6769 6F6E 206D 616E"            /* imple region man */
	$"6970 756C 6174 696F 6E20 6973 2064 6F6E"            /* ipulation is don */
	$"6520 696E 2074 6865 2052 6567 696F 6E20"            /* e in the Region  */
	$"5769 6E64 6F77 2E20 2043 6C69 636B 2061"            /* Window.  Click a */
	$"6E64 2064 7261 6720 7468 6520 6D6F 7573"            /* nd drag the mous */
	$"6520 696E 2069 7420 746F 2064 7261 7720"            /* e in it to draw  */
	$"7265 6374 616E 676C 6573 2E20 2054 6865"            /* rectangles.  The */
	$"7365 2061 7265 2063 6F6D 6269 6E65 6420"            /* se are combined  */
	$"616E 6420 616E 206F 7574 6C69 6E65 206F"            /* and an outline o */
	$"6620 7468 6520 7265 7375 6C74 696E 6720"            /* f the resulting  */
	$"7265 6769 6F6E 2069 7320 6472 6177 6E2E"            /* region is drawn. */
	$"2020 5265 6374 616E 676C 6573 2064 7261"            /*   Rectangles dra */
	$"776E 2077 6869 6C65 2074 6865 2073 6869"            /* wn while the shi */
	$"6674 206B 6579 2069 7320 6865 6C64 2064"            /* ft key is held d */
	$"6F77 6E20 6172 6520 7375 6274 7261 6374"            /* own are subtract */
	$"6564 2066 726F 6D20 7468 6520 7265 6769"            /* ed from the regi */
	$"6F6E 2E20 2044 6F75 626C 652D 636C 6963"            /* on.  Double-clic */
	$"6B20 7468 6520 6D6F 7573 6520 746F 2063"            /* k the mouse to c */
	$"6C65 6172 2074 6865 2064 6973 706C 6179"            /* lear the display */
	$"2E0D 0D4D 656E 7573 0D0D 5468 6520 4170"            /* .¬¬Menus¬¬The Ap */
	$"706C 6520 4D65 6E75 2073 7570 706F 7274"            /* ple Menu support */
	$"7320 6465 736B 2061 6363 6573 736F 7269"            /* s desk accessori */
	$"6573 2069 6E20 7468 6520 7374 616E 6461"            /* es in the standa */
	$"7264 206D 616E 6E65 722E 0D0D 496E 2074"            /* rd manner.¬¬In t */
	$"6865 2046 696C 6520 4D65 6E75 2C20 4F70"            /* he File Menu, Op */
	$"656E 206D 616B 6573 2061 6C6C 204D 756C"            /* en makes all Mul */
	$"7469 536B 656C 2077 696E 646F 7773 2076"            /* tiSkel windows v */
	$"6973 6962 6C65 2E20 2043 6C6F 7365 2068"            /* isible.  Close h */
	$"6964 6573 2074 6865 2066 726F 6E74 6D6F"            /* ides the frontmo */
	$"7374 2077 696E 646F 772E 2020 4966 2074"            /* st window.  If t */
	$"6865 2077 696E 646F 7720 6265 6C6F 6E67"            /* he window belong */
	$"7320 746F 2061 2064 6573 6B20 6163 6365"            /* s to a desk acce */
	$"7373 6F72 792C 2074 6865 2061 6363 6573"            /* ssory, the acces */
	$"736F 7279 2069 7320 636C 6F73 6564 2E20"            /* sory is closed.  */
	$"2051 7569 7420 6578 6974 7320 4D75 6C74"            /*  Quit exits Mult */
	$"6953 6B65 6C2E 0D0D 5468 6520 4564 6974"            /* iSkel.¬¬The Edit */
	$"204D 656E 7520 6973 2075 7365 6420 7769"            /*  Menu is used wi */
	$"7468 2074 6865 2054 6578 7420 5769 6E64"            /* th the Text Wind */
	$"6F77 2061 6E64 2064 6573 6B20 6163 6365"            /* ow and desk acce */
	$"7373 6F72 6965 7320 7468 6174 2061 6C6C"            /* ssories that all */
	$"6F77 2065 6469 7469 6E67 2E"                        /* ow editing. */
};

#ifdef _MPPC_
resource 'cfrg' (0) {
  {
    kPowerPC,
    kFullLib,
    kNoVersionNum,kNoVersionNum,
    0, 0,
    kIsApp,kOnDiskFlat,kZeroOffset,kWholeFork,
    ""
  }
};
#endif