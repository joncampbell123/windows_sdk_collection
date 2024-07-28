/**[f******************************************************************
 * psprompt.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

#define PS_PROMPT_GENERAL	0
#define PS_PROMPT_HEADER	1
#define PS_PROMPT_HANDSHAKE	2
#define PS_PROMPT_EHANDLER	3

#define IDS_PROMPT_CAPTION 	2000

#define IDS_PROMPT_MESSAGE_GENERAL 	2010
#define IDS_PROMPT_MESSAGE_HEADER  	2011
#define IDS_PROMPT_MESSAGE_HANDSHAKE	2012
#define IDS_PROMPT_MESSAGE_EHANDLER	2013

BOOL FAR PASCAL PSPrompt(HWND, short);

