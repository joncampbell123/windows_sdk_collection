
#define TABGRP  WS_TABSTOP | WS_GROUP

dtMODE DIALOG 20, 20,  168,  122
    STYLE   WS_POPUP | WS_DLGFRAME
    CAPTION ": Output Mode"
BEGIN
    GROUPBOX,    "Orientation", -1,          12,  5, 70, 36, WS_GROUP
    RADIOBUTTON  "&Portrait",    PORTRAIT,   17, 16, 58, 10, WS_GROUP
    RADIOBUTTON  "&Landscape",   LANDSCAPE,  17, 28, 58, 10

    GROUPBOX,    "Print Quality", -1,        95,  5, 60, 36, WS_GROUP
    RADIOBUTTON  "&High",     HIGH,           100, 16, 28, 10, WS_GROUP
    RADIOBUTTON  "L&ow",      LOW,            100, 28, 28, 10

    GROUPBOX        "Paper Format" , -1,      12, 48, 70, 67, WS_GROUP
    RADIOBUTTON     "&US Letter",   LETTER,    17, 60, 64, 10, WS_GROUP
    RADIOBUTTON     "&DIN A4",   DINA4,        17, 72, 64, 10
    RADIOBUTTON     "&Euro Fanfold",  FANFOLD, 17, 84, 64, 10
    CHECKBOX	    "&Wide Carriage", PAPERWIDTH,  17, 100, 64, 10
    CONTROL	    "", -1, STATIC, SS_BLACKRECT, 12, 95, 70, 1

    DEFPUSHBUTTON"OK",       IDOK,      108, 65, 32, 14, TABGRP
    PUSHBUTTON   "Cancel",   IDCANCEL,  108, 85, 32, 14, TABGRP
END
