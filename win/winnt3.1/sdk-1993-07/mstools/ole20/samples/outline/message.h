/*************************************************************************
**
**	  OLE 2.0 Server Sample Code
**
**	  message.h                                               
**                                                                         
**    This file is a user customizable list of status messages associated 
**	  with menu items.  
**                                                                         
**    (c) Copyright Microsoft Corp. 1992 - 1993 All Rights Reserved        
**
*************************************************************************/

// Status bar messages and associated data.

// Message type for status bar messages.
typedef struct {
	UINT wIDItem;
	char *string;
} STATMESG;

/*
 * CUSTOMIZATION NOTE:  Be sure to change NUM_POPUP if you
 *                      change the number of popup messages.
 */

// REVIEW: these messages should be loaded from a string resource

// List of all menu item messages.
STATMESG MesgList[] =
{
	{ IDM_F_NEW,		"Creates a new outline"	},	
	{ IDM_F_OPEN,		"Opens an existing outline file"	},
	{ IDM_F_SAVE,		"Saves the outline" },
	{ IDM_F_SAVEAS,		"Saves the outline with a new name" },
	{ IDM_F_PRINT,		"Prints the outline" },
	{ IDM_F_PRINTERSETUP, "Changes the printer and the printing options" },
	{ IDM_F_EXIT,		"Quits the application, prompting to save changes" },

	{ IDM_E_UNDO,		"Undo not yet implemented" },
	{ IDM_E_CUT,		"Cuts the selection and puts it on the Clipboard" },
	{ IDM_E_COPY,		"Copies the selection and puts it on the Clipboard" },
	{ IDM_E_PASTE,		"Inserts the Clipboard contents after current line" },
	{ IDM_E_PASTESPECIAL,"Allows pasting Clipboard data using a special format" },
	{ IDM_E_CLEAR,		"Clears the selection" },
	{ IDM_E_SELECTALL,	"Selects the entire outline" },
#if defined( OLE_CNTR )
	{ IDM_E_INSERTOBJECT, "Inserts new object after current line" },
	{ IDM_E_EDITLINKS, "Edit and view links contained in the document" },
   { IDM_E_CONVERTVERB, "Converts or activates an object as another type" },
	{ IDM_E_OBJECTVERBMIN, "Opens, edits or interacts with an object" },
	{ IDM_E_OBJECTVERBMIN+1, "Opens, edits or interacts with an object1" },
	{ IDM_E_OBJECTVERBMIN+2, "Opens, edits or interacts with an object2" },
	{ IDM_E_OBJECTVERBMIN+3, "Opens, edits or interacts with an object3" },
	{ IDM_E_OBJECTVERBMIN+4, "Opens, edits or interacts with an object4" },
	{ IDM_E_OBJECTVERBMIN+5, "Opens, edits or interacts with an object5" },
#endif

	{ IDM_L_ADDLINE,	"Adds a new line after current line" },
	{ IDM_L_EDITLINE,	"Edits the current line" },
	{ IDM_L_INDENTLINE,	"Indents the selection" },
	{ IDM_L_UNINDENTLINE, "Unindents the selection" },

	{ IDM_N_DEFINENAME,	"Assigns a name to the selection" },
	{ IDM_N_GOTONAME,	"Jumps to a specified place in the outline" },

	{ IDM_H_ABOUT,		"Displays program info, version no., and copyright" }
};

#define NUM_STATS	sizeof(MesgList)/sizeof(MesgList[0])
#define NUM_POPUP	10	// Maximum number of popup messages.
#define MAX_MESSAGE	100	// Maximum characters in a popup message.

