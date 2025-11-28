* _framewk.h

***********************************************************
* localization strings, constants, 
* and tunable expressions for _framewk.vcx,
* framework-enabling header file for global template classes

***********************************************************

* _application class

#DEFINE APP_MEDIATOR_SUPERCLASS      "_formmediator"
* note: don't use TRANSFORM() or trim below, need the padding spaces:
#DEFINE APP_META_FAVE_ID              STR(RECNO())
#DEFINE APP_META_UNAVAILABLE_LOC     "Metatable in use or unavailable:"
#DEFINE APP_META_WRONGFORMAT_LOC     "Metatable has incorrect structure:"
#DEFINE APP_META_MISSINGINDEX_LOC    "One or more of required indexes"+CHR(13)+ ;
                                     "is missing from your metatable:"

#DEFINE PJX_META_DOC_FORM_TYPE       "F"
#DEFINE PJX_META_DOC_REPORT_TYPE     "R"
#DEFINE PJX_META_DOC_HEADER_TYPE     "A" 

#DEFINE APP_FEATURE_NOT_AVAILABLE_LOC "Feature not available."
#DEFINE APP_FILE_NOT_FOUND_LOC        "File not found"
#DEFINE APP_READY_TO_SHUTDOWN_LOC     "Are you sure you want to quit?"
#DEFINE APP_LOADING_LOC               "Loading..."

#DEFINE APP_PM_WIN_TITLE_LOC         "Project Manager - "
#DEFINE APP_ALREADY_EXISTS_LOC       "already exists"
#DEFINE APP_GO_PAD_LOC               "\<Go"
#DEFINE APP_GO_PAD_HOTKEY_LOC        ALT+G            
#DEFINE APP_GO_MESSAGE_LOC           "Navigates the currently selected table, cursor, or view"  

#DEFINE APP_USER_FAVES_CLEAR_LOC     "Do you want to clear your Favorites list now?"
#DEFINE APP_USER_FAVES_CORRUPT_LOC   "Your Favorites entries appear corrupt, and will be cleared."
#DEFINE APP_META_TABLE_CHANGED_LOC   "Document table for "+THIS.cCaption+ " has been changed. "+ CHR(13)+;
                                     "Some of your Favorites may not run properly. "+ ;
                                     CHR(13)+CHR(13)+ ;
                                     APP_USER_FAVES_CLEAR_LOC

#DEFINE APP_MACRO_RESTORE_LOC        "Restore"                                     
#DEFINE APP_OUTPUT_ONE_REC_LOC       "Current Record"

* _dialog fonts
#DEFINE DIALOG_SMALLFONT_NAME        "MS Sans Serif"
#DEFINE DIALOG_LARGEFONT_NAME        "Arial"

* _aboutbox class
#DEFINE ABOUT_LOC                    "About"

* _options class
#DEFINE OPTIONS_LOC                      "User Options"
#DEFINE OPTIONS_DEFAULTS_SAVED_LOC       "User option defaults have been saved."
#DEFINE OPTIONS_APPLIED_LOC              "User options have been applied."
#DEFINE OPTIONS_DEFAULTS_SHOWN_LOC       "User option saved defaults are shown."
#DEFINE OPTIONS_NOT_STORED_LOC           "User option defaults not yet saved!"
#DEFINE CHANGEPASSWORD_LOC               "Change Password"
#DEFINE OPTIONS_CONFIRM_PASSWORD_LOC     "Please retype your new password for confirmation."
#DEFINE OPTIONS_PASSWORD_UNCONFIRMED_LOC "Password not confirmed."+CHR(13)+"Please try again."
#DEFINE OPTIONS_PASSWORD_CONFIRMED_LOC   "Your new password has been accepted. " 

* _UserLogInClass
#DEFINE LOGIN_CAPTION_LOC            "User Login"
#DEFINE LOGIN_ADD_USER_LOC           "Would you like to add a new user?"
#DEFINE LOGIN_NEW_USER_INFO_LOC      "Store a new password for this user now."+ CHR(13)+CHR(13)+;
                                     "This new user will receive basic-level access."
#DEFINE LOGIN_USER_NOT_FOUND_LOC     "User not listed."
#DEFINE LOGIN_WRONG_PASSWORD_LOC     "Incorrect password for this user."
#DEFINE LOGIN_EMPTY_PASSWORD_LOC     "Empty password not allowed."
#DEFINE LOGIN_TRIES_EXCEEDED_LOC     "Attempts to log in exceed limit!"

* user permissions and levels are provided for, although
* not evaluated, in the basic framework
#DEFINE USER_PERMISSION_DENIED_LOC  "Your user-level does not allow access to this function."

* _documentpicker and descendent classes
#DEFINE DOCUMENTPICKER_NO_DOCUMENTS_LOC   "No registered documents available."
#DEFINE DOCUMENTPICKER_NO_APP_LOC         "Must pass a reference to an object descended from _APPLICATION"

#DEFINE NEWOPEN_CAPTION_NEW_LOC           "Choose a document to create"
#DEFINE NEWOPEN_CAPTION_OPEN_LOC          "Choose a document to open"

#DEFINE REPORTPICKER_CAPTION_RUN_LOC      "Choose a report to run"
* report editing may be offered at some point:
#DEFINE REPORTPICKER_CAPTION_MODIFY_LOC   "Choose a report to modify"

#DEFINE FAVORITEPICKER_CAPTION_ADD_LOC    "Documents available for Favorites"
#DEFINE FAVORITEPICKER_ADDBUTTON_LOC      "\<Add"
#DEFINE FAVORITEPICKER_CLOSEBUTTON_LOC    "\<Close"
#DEFINE FAVORITEPICKER_CAPTION_START_LOC  "Quick Start"
#DEFINE FAVORITEPICKER_DOC_ALREADY_LOC    "has already been picked for Favorites."

#DEFINE FAVORITEPICKER_PICKED_LOC         "* "
#DEFINE FAVORITEPICKER_PICKED_LEN         LEN(FAVORITEPICKER_PICKED_LOC)

* _errorlogviewer class
#DEFINE ERRORVIEWER_CAPTION_LOC        "Error Log Viewer"
#DEFINE ERRORVIEWER_EMPTY_LOC          "The error log has no records."
#DEFINE ERRORVIEWER_UNAVAILABLE_LOC    "The error log is not available."
#DEFINE ERRORVIEWER_IN_USE_LOC         "The error log is in use, please close before continuing."

***********************************************************
* Messagebox subset from FOXPRO.H
***********************************************************

#DEFINE MB_OK                   0       && OK button only
#DEFINE MB_OKCANCEL             1       && OK and Cancel buttons
#DEFINE MB_ABORTRETRYIGNORE     2       && Abort, Retry, and Ignore buttons
#DEFINE MB_YESNOCANCEL          3       && Yes, No, and Cancel buttons
#DEFINE MB_YESNO                4       && Yes and No buttons
#DEFINE MB_RETRYCANCEL          5       && Retry and Cancel buttons

#DEFINE MB_ICONSTOP             16      && Critical message
#DEFINE MB_ICONQUESTION         32      && Warning query
#DEFINE MB_ICONEXCLAMATION      48      && Warning message
#DEFINE MB_ICONINFORMATION      64      && Information message

#DEFINE MB_APPLMODAL            0       && Application modal message box
#DEFINE MB_DEFBUTTON1           0       && First button is default
#DEFINE MB_DEFBUTTON2           256     && Second button is default
#DEFINE MB_DEFBUTTON3           512     && Third button is default
#DEFINE MB_SYSTEMMODAL          4096    && System Modal

*-- MsgBox return values
#DEFINE IDOK            1       && OK button pressed
#DEFINE IDCANCEL        2       && Cancel button pressed
#DEFINE IDABORT         3       && Abort button pressed
#DEFINE IDRETRY         4       && Retry button pressed
#DEFINE IDIGNORE        5       && Ignore button pressed
#DEFINE IDYES           6       && Yes button pressed
#DEFINE IDNO            7       && No button pressed

***********************************************************
* Data-handling subset from FOXPRO.H
***********************************************************
*-- Cursor buffering modes
#DEFINE DB_BUFOFF               1
#DEFINE DB_BUFLOCKRECORD        2
#DEFINE DB_BUFOPTRECORD         3        
#DEFINE DB_BUFLOCKTABLE         4
#DEFINE DB_BUFOPTTABLE          5

*-- Update types for views/cursors
#DEFINE DB_UPDATE               1
#DEFINE DB_DELETEINSERT         2

*-- WHERE clause types for views/cursors
#DEFINE DB_KEY                  1
#DEFINE DB_KEYANDUPDATABLE      2
#DEFINE DB_KEYANDMODIFIED       3
#DEFINE DB_KEYANDTIMESTAMP      4

*-- Remote connection login prompt options
#DEFINE DB_PROMPTCOMPLETE       1
#DEFINE DB_PROMPTALWAYS         2
#DEFINE DB_PROMPTNEVER          3

*-- Remote transaction modes
#DEFINE DB_TRANSAUTO            1
#DEFINE DB_TRANSMANUAL          2

*-- Source Types for CursorGetProp()
#DEFINE DB_SRCLOCALVIEW         1
#DEFINE DB_SRCREMOTEVIEW        2
#DEFINE DB_SRCTABLE             3


#DEFINE WINDOWTYPE_MODELESS   0
#DEFINE WINDOWTYPE_MODAL      1
#DEFINE WINDOWTYPE_READ       2
#DEFINE WINDOWTYPE_READMODAL  3

