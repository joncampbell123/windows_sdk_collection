#ifndef OPTIONS_INCLUDED
#define OPTIONS_INCLUDED

   // Defines for controls in dialog box.  Note the PLACEHOLDER_* defines.
   //  These are so we can have a contigous string table with help for each
   //  toggled state of check boxes.  See references to IDS_HELP_*.

#define GB_DISPLAY            100
#define GB_PRINTER            101
#define IDRB_STRETCHWINDOW    102
#define PLACEHOLDER_NOSTRETCH 103   // Used for IDS_HELP_NORMALWINDOW
#define IDRB_USEDIBS          104
#define IDRB_USEDDBS          105
#define IDRB_USESETDIBITS     106
#define IDRB_BESTFIT          107
#define IDRB_STRETCH          108
#define IDRB_SCALE            109
#define IDEF_XAXIS            110
#define IDEF_YAXIS            111
#define IDRB_USEBANDING       112
#define PLACEHOLDER_NOBANDING 113
#define IDRB_USE31APIS        114
#define PLACEHOLDER_NO31APIS  115
#define IDEF_HELP             116
#define IDC_XLABEL            200
#define IDC_YLABEL            201


   // Defines for help string table.

#define IDS_HELP_STRETCHWINDOW PLACEHOLDER_NOSTRETCH
#define IDS_HELP_NORMALWINDOW  IDRB_STRETCHWINDOW
#define IDS_HELP_USEDIBS       IDRB_USEDIBS
#define IDS_HELP_USEDDBS       IDRB_USEDDBS
#define IDS_HELP_USESETDIBITS  IDRB_USESETDIBITS
#define IDS_HELP_BESTFIT       IDRB_BESTFIT
#define IDS_HELP_STRETCH       IDRB_STRETCH
#define IDS_HELP_SCALE         IDRB_SCALE
#define IDS_HELP_XAXIS         IDEF_XAXIS
#define IDS_HELP_YAXIS         IDEF_YAXIS
#define IDS_HELP_USEBANDING    PLACEHOLDER_NOBANDING
#define IDS_HELP_NOBANDING     IDRB_USEBANDING
#define IDS_HELP_USE31APIS     PLACEHOLDER_NO31APIS
#define IDS_HELP_NO31APIS      IDRB_USE31APIS


   // Other string table defines used by OPTIONS.C.

#define IDS_ERRXYSCALE         700


   // Structure copied to/from dialog box.

typedef struct
   {
   BOOL bStretch;                         // True = stretch to window
   BOOL bPrinterBand;                     // True = want to band DIB to printer.
   BOOL bUse31PrintAPIs;                  // True = Use the 3.1 Printing API
   WORD wDispOption;                      // See defines below
   WORD wPrintOption;                     // See defines below
   WORD wXScale;                          // X Scale Edit control value
   WORD wYScale;                          // Y Scale Edit control value
   } FAR *LPOPTIONSINFO, OPTIONSINFO;



   // Values used for wDispOption in OPTIONSINFO structure.

#define DISP_USE_DIBS         IDRB_USEDIBS
#define DISP_USE_DDBS         IDRB_USEDDBS
#define DISP_USE_SETDIBITS    IDRB_USESETDIBITS


   // Values used for wPrintOption in OPTIONSINFO structure.

#define PRINT_BESTFIT         IDRB_BESTFIT         // Best proportional stretch fit
#define PRINT_STRETCH         IDRB_STRETCH         // Stretch to fill page
#define PRINT_SCALE           IDRB_SCALE           // Independent X/Y scaling







void ShowOptions (HWND hWnd, LPOPTIONSINFO lpInfo);

#endif
