#define ifnAbout            0
#define ifnTerm             1
#define ifnPhone            2
#define ifnComm             3
#define ifnDisCon           4
#define ifnPhNum            5
#define ifnAnswer           6
#define cfnDlg              7


#define idlgOpen            1
#define idlgSaveAs          2
#define idlgAbout           3
#define idlgTerm            4
#define idlgPhone           5
#define idlgComm            6
#define idlgDisCon          7
#define idlgCapture         8
#define idlgPhNum           9
#define idlgAnswer         10
#define cDlg               10           /* The number of dialogs */

#define IDS_ABOUT           1
#define IDS_UNTITLED        2
#define IDS_TERMINAL        3
#define IDS_CONNECT         4
#define IDS_DISCON          5
#define IDS_FNCAPTURE       6
#define IDS_FNTERM          7
#define IDS_EXTTERM         8
#define IDS_EXTTEXT         9
#define IDS_MB1            10           /* Message box strings */
#define IDS_MB2            11
#define IDS_MB3            12
#define IDS_MB4            13
#define IDS_MB5            14
#define IDS_MB6            15
#define IDS_MB7            16
#define IDS_MB8            17
#define IDS_MB9            18
#define IDS_MB10           19
#define IDS_MB11           20
#define IDS_MB12           21
#define IDS_MB13           22
#define IDS_MB14           23
#define IDS_MB15           24
#define IDS_MB16           25
#define IDS_MB17           26
#define IDS_MB18           27
#define IDS_MB19           28
#define IDS_INIT           29
#define IDS_SETTINGS       30
#define IDS_CAPTURE        31
#define IDS_COPY           32
#define IDS_COMPUTER       33
#define IDS_MODEM          34
#define IDS_CMDPRINT       35
#define IDS_CMDCAPTURE     36
#define IDS_LBRACKET       37
#define IDS_RBRACKET       38
#define IDS_PRINT          39

/* Note: IDS_NONE must be the smallest country string index and IDS_SWISS
/*       must be the largest country string index because these two end
/*       points are used as loop control variables in dialogs.c to
/*       load the list box with the country values.
/**/


#define IDS_NONE        40
#define IDS_BRITISH     41
#define IDS_DANISH      42
#define IDS_FINNISH     43
#define IDS_FRENCH      44
#define IDS_CANADIAN    45
#define IDS_GERMAN      46
#define IDS_ITALIAN     47
#define IDS_SPANISH     48
#define IDS_SWEDISH     49
#define IDS_SWISS       50
#define IDS_MERGESTRING 55

/* The country codes */
#define CC_USA          1
#define CC_UK          44
#define CC_DENMARK     45
#define CC_NORWAY      47
#define CC_FINLAND    358
#define CC_FRANCE      33
#define CC_GERMANY     49
#define CC_ITALY       39
#define CC_SPAIN       34
#define CC_SWEDEN      46
#define CC_SWITZERLAND 41
#define cCountryCodes  11           /* The number of country codes */



#define IDSCOUNTRYMIN    40              /* The beginning country number */
#define IDSCOUNTRYMAX    50              /* The last country number */

/* File Menu Items */
#define miAbout          1001           /* About terminal */
#define miExit           1002

#define  miNew              1           /* New */
#define  miOpen             2           /* Open */
#define  miSave             3           /* Save */
#define  miSaveAs           4           /* Save as */

/* Edit Menu Items */
#define  miCopy             5           /* Copy */
#define  miPaste            6           /* Paste */

/* Control Menu Items */
#define miConnect           7           /* Connect */
#define miPrinter           8           /* Printer */
#define miCapture           9           /* Capture */
#define miPause            10           /* Pause */
#define miBreak            14           /* Break */

/* Modify Menu Items */
#define miTerm             11           /* Terminal type */
#define miComm             12           /* Parameters */
#define miPhone            13           /* Phone setting */


/* Dialog items */
#define diNull             0            /* A dummy dialog item */
#define diOk               1            /* Ok button */
#define diCancel           2            /* Cancel button */
#define diNo               3            /* NO button */
#define diListBox          4            /* A list box */
#define diEdit             5            /* An edit text item */

#define diPhNum            6            /* Phone number */
#define diTone             7            /* DTMF dialing */
#define diPulse            8            /* Pulse dialing */
#define diSlow             9            /* Slow dialing speed */
#define diFast            10            /* Fast dialing speed */
#define diTWait           11            /* Dial tone wait period */
#define diPath            12            /* Static path name */
#define diBaud            13            /* Baud rate */
#define diSB1             14            /* One stop bit */
#define diSB15            15            /* 1.5 stop bits */
#define diSB2             16            /* Two stop bits */

#define diWrap            17            /* The autowrap button */
#define diEcho            18            /* The echo on/off button */
#define diNewLine         19            /* Terminal settings */
#define diAnsi            20            /* ANSII terminal */
#define diVT52            21            /* VT52 terminal */
#define diLarge           22
#define diSmall           23
#define diBuffer          24            /* Buffer size */

#define diParEven         25            /* Even parity */
#define diParOdd          26            /* Odd parity */
#define diParNone         27            /* No parity */
#define diXON             28            /* XOn/XOff handshaking */
#define diHardware        29            /* Hardware handshaking */
#define diNone            30            /* No handshaking */
#define diModem           31            /* Modem connection */
#define diComputer        32            /* Computer connection */
#define diPort1           33            /* Comm port #1 */
#define diPort2           34            /* Comm port #2 */
#define diPort3           35            /* Comm port #3 */
#define diPort4           36            /* Comm port #4 */
#define diWBits4          37            /* 4 bits per char */
#define diWBits5          38            /* 5 bits per char */
#define diWBits6          39            /* 6 bits per char */
#define diWBits7          40            /* 7 bits per char */
#define diWBits8          41            /* 8 bits per char */
#define diAnswer          42            /* Seconds to wait for answer */
#define diCount           43            /* Countdown timer for anwer */

#define TERMINAL 1
