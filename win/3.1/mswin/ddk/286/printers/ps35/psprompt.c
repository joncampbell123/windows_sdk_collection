/**[f******************************************************************
 * psprompt.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

#include "pscript.h"
#include "psprompt.h"
#include "psdata.h"


BOOL FAR PASCAL PSPrompt(HWND hWnd, short errorNum)
{
	char caption[24];
	char message[64];
	int ids;

	if (!hWnd)
		return FALSE;

	switch(errorNum){

	case PS_PROMPT_HEADER:
		ids = IDS_PROMPT_MESSAGE_HEADER;
		break;

	case PS_PROMPT_HANDSHAKE:
		ids = IDS_PROMPT_MESSAGE_HANDSHAKE;
		break;

	case PS_PROMPT_EHANDLER:
		ids = IDS_PROMPT_MESSAGE_EHANDLER;
		break;

	default:
		ids = IDS_PROMPT_MESSAGE_GENERAL;
		break;
	}

	LoadString(ghInst, IDS_PROMPT_CAPTION, caption,	sizeof(caption));
	LoadString(ghInst, ids, message, sizeof(message));

	return (MessageBox(hWnd, message, caption, MB_OKCANCEL | MB_ICONQUESTION) == IDOK);
}
