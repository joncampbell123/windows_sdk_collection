/**[f******************************************************************
 * mgesc.h
 *
 * micrographics escapes module
 *
 * this module implements the micrographics escapes.  this stuff
 * was really designed for postscript but it does not fit in
 * well with GDI. 
 *
 **f]*****************************************************************/

#include "pscript.h"
#include "psdata.h"
#include "utils.h"
#include "debug.h"
#include "channel.h"
#include "escapes.h"
#include "mgesc.h"
#include "graph.h"
#include "realize.h"
#include "resource.h"

char MoveCmd = 'M';	/* these really should be in the PDEVICE */
int XformLevel = 0;	/* but since they are only used by designer... */
int ArcDir = COUNTERCLOCKWISE;	/* GDI default */


void FAR PASCAL DeviceToDefaultPS( LPDV lpdv, LPPOINT  lpPoint);

int FAR PASCAL MGControl(lpdv, ifn, lpbIn, lpbOut)
LPDV	lpdv;	    /* ptr to the device descriptor */
int	ifn;	    /* the escape function to execute */
LPSTR	lpbIn;	    /* ptr to input data for the escape function */
LPSTR	lpbOut;	    /* ptr to the output buffer for the esc function*/
{
	int i;

	switch (ifn) {

	case SET_SCREEN_ANGLE:
		{
		char buf[100];
		int angle;

		angle = lpdv->angle;

		LoadString(ghInst, IDS_SETSCREENANGLE, buf, sizeof(buf));
		PrintChannel(lpdv, buf, lpdv->angle = *(int far *)lpbIn);

		return angle;
		}

	case SET_BOUNDS:
        {
            POINT  pointA, pointB;
            int    swap;

    		// lpbIn  is a rectangle in device coords.
            // EpsRect will be in default Postscript coords.
            pointA.x = ((LPRECT)lpbIn)->left;
            pointA.y = ((LPRECT)lpbIn)->bottom;
            pointB.x = ((LPRECT)lpbIn)->right;
            pointB.y = ((LPRECT)lpbIn)->top;

            DeviceToDefaultPS(lpdv, (LPPOINT)&pointA);
            DeviceToDefaultPS(lpdv, (LPPOINT)&pointB);

            if(pointA.y > pointB.y)  // need to swap
            {
                swap = pointA.y;
                pointA.y = pointB.y;
                pointB.y = swap;
            }
            if(pointA.x > pointB.x)  // need to swap
            {
                swap = pointA.x;
                pointA.x = pointB.x;
                pointB.x = swap;
            }
		    lpdv->EpsRect.top    = pointB.y;
		    lpdv->EpsRect.bottom = pointA.y;
		    lpdv->EpsRect.right  = pointB.x;
		    lpdv->EpsRect.left   = pointA.x;
        }
		break;

	case SET_ARC_DIRECTION:
		DBMSG(("SET_ARC_DIRECTION\n"));
		i = ArcDir;	/* save old */
		ArcDir = *(short FAR *)lpbIn;

		if (ArcDir != i) {
			PrintChannel(lpdv, "/ARC /arc%s ld\n",
				(ArcDir == CLOCKWISE) ?
				szNull : (LPSTR)"n");
		}
		return i;
		break;

	case RESTORE_CTM:

		DBMSG(("RESTORE_CTM\n"));

		/* restore prev xform from stack */
		PrintChannel(lpdv, "sm\n");

		XformLevel--;

		return XformLevel;
		break;

	case SAVE_CTM:
		DBMSG(("SAVE_CTM\n"));

		/* leave curr xform on stack */
		PrintChannel(lpdv, "sM cm\n");

		XformLevel++;

		return XformLevel;
		break;

	case TRANSFORM_CTM:

		DBMSG(("TRANSFORM_CTM\n"));

		PrintChannel(lpdv, "[");
		for (i = 0; i < 9; i += 3) {
			PrintChannel(lpdv, " %ld TR %ld TR",
				((DWORD FAR *)lpbIn)[i],
				((DWORD FAR *)lpbIn)[i+1]);
		}
		PrintChannel(lpdv, "] concat\n");

		return TRUE;
		break;

	case SET_CLIP_BOX:

		DBMSG(("SET_CLIP_BOX "));

		if (!XformLevel)
			return TRUE;
#ifdef DEBUG_ON
		if (lpbIn) {
			DBMSG(("(set)\n"));
		} else {
			DBMSG(("(restore)\n"));
		}

		PrintChannel(lpdv, "%% SET_CLIP_BOX\n");
#endif

		ClipBox(lpdv, (LPRECT)lpbIn);

		return TRUE;

		break;


	case SET_POLY_MODE:

		DBMSG((" SET_POLY_MODE %d\n", *(LPSHORT)lpbIn));

		if (*(LPSHORT)lpbIn == PM_POLYLINE ||
		    *(LPSHORT)lpbIn == PM_BEZIER ||
		    *(LPSHORT)lpbIn == POLYLINESEGMENT) {

		    	i = lpdv->PolyMode;
			lpdv->PolyMode = *(LPSHORT)lpbIn;

#ifdef DEBUG_ON
			PrintChannel(lpdv, "%% SET_POLY_MODE %d\n", lpdv->PolyMode);
#endif
			return i;	/* previous value */

		} else
			return FALSE;

		break;


	case CLIP_TO_PATH:
		DBMSG(("CLIP_TO_PATH mode:%d\n", 
			LOWORD(*(DWORD FAR *)lpbIn)));

#ifdef DEBUG_ON
		PrintChannel(lpdv, "%% CLIP_TO_PATH clip %d fill_mode %d\n",
			LOWORD(*(DWORD FAR *)lpbIn),
			HIWORD(*(DWORD FAR *)lpbIn));
#endif

		switch (LOWORD(*(DWORD FAR *)lpbIn)) {
		case CLIP_SAVE:	/* gsave */
			PrintChannel(lpdv, "gs\n");
			break;

		case CLIP_RESTORE:	/* grestore */
			PrintChannel(lpdv, "gr\n");
			break;

		case CLIP_INCLUSIVE:	/* clip to inside */
					/* clip (safe) newpath */
			PrintChannel(lpdv, "%d CP n\n", 
				HIWORD(*(DWORD FAR *)lpbIn) == ALTERNATE);
			break;
		default:
			return 0;	/* not supported */
		}
		return 1;	/* success */


	case BEGIN_PATH:

		DBMSG(("BEGIN_PATH\n"));

		lpdv->PathLevel++;
#ifdef DEBUG_ON
		PrintChannel(lpdv, "%% BEGIN_PATH level %d\n", lpdv->PathLevel);
#endif
//		if ((lpdv->PathLevel == 1) && MoveCmd != 'M') {
			MoveCmd = 'M';
//		}

		return lpdv->PathLevel;


	case END_PATH:
		{
		DRAWMODE dm;
		BR br;
		PEN pen;

		DBMSG(("END_PATH rendermode:%d fillmode:%d\n", 
			((LPPATHINFO)lpbIn)->RenderMode,
			((LPPATHINFO)lpbIn)->FillMode));

		switch (((LPPATHINFO)lpbIn)->RenderMode) {

		case RM_NO_DISPLAY:	/* will clip */
			break;

		case RM_CLOSED:

			PrintChannel(lpdv, "cp\n");

			/* fall through... */

		case RM_OPEN:	/* no filling */

			if (lpdv->PathLevel != 1)
				break;

			if (((LPPATHINFO)lpbIn)->RenderMode == RM_CLOSED) {

				RealizeBrush(lpdv, &((LPPATHINFO)lpbIn)->Brush, &br);

				/* this sucks as well.  we aren't
				 * passed a draw mode structure so
				 * we have to assume TRANSPARENT
				 * because we don't know what bkColor
				 * to use when filling */

				dm.bkMode = ((LPPATHINFO)lpbIn)->BkMode;	/* DRAWMODE for SelectBrush() */
				dm.bkColor = ((LPPATHINFO)lpbIn)->BkColor;
				    	
				SelectBrush(lpdv, &br, &dm);

				/* fill */
			}

			RealizePen(lpdv, &((LPPATHINFO)lpbIn)->Pen, &pen);

			SelectPen(lpdv, &pen);

			FillDraw(lpdv, ((LPPATHINFO)lpbIn)->RenderMode == RM_CLOSED, TRUE,
				((LPPATHINFO)lpbIn)->FillMode == ALTERNATE);
			
			/* fill and draw */
			break;

		default:
			return -1;	/* failure */
		}

#ifdef DEBUG_ON
		PrintChannel(lpdv, "%% END_PATH level %d  render mode %d\n",
			lpdv->PathLevel,
			((LPPATHINFO)lpbIn)->RenderMode);
#endif
		if (lpdv->PathLevel == 1)	/* leaving path mode? */
			MoveCmd = 'M';		/* restore to regular move */

		lpdv->PathLevel--;

		return lpdv->PathLevel;
		}

	case EXT_DEVICE_CAPS:

		DBMSG(("EXT_DEVICE_CAPS %d\n", *(WORD FAR *)lpbIn));
		switch(*(WORD FAR *)lpbIn) {
		case PATH_CAPS:
			*(DWORD FAR *)lpbOut = PATH_ALTERNATE | PATH_WINDING |
					      PATH_INCLUSIVE;
			break;
		case POLYGON_CAPS:
			*(DWORD FAR *)lpbOut = 1500L;
			break;

		default:
			DBMSG((" not supported\n"));
			return 0;	/* not supported */
		}

		DBMSG((" supported\n"));
		return 1;	/* everything supported */

	default:
		return FALSE;
	}

	return TRUE;
}
