/* DECIDE.C - Decide which display adapter the user has after 
 * running the diagnostic tests.
 */
#include "setup.h"
#include "id.h"


/* IDs for machines and displays defined in id.h */

extern BOOL Is386;

extern int VideoAdapter(), CGAtype(), Herc(), Detect8514();
/*extern int VideoAdapter(), CGAtype(), GAD(), Herc(), bMouse(), bMouseOnPS2();
 */

Get20Display(MachineId)
int MachineId;
{

	if (MachineId == 0)
		return 0;


	switch (VideoAdapter())	{
		case 1:
			/* It is a CGA. Determine the type of CGA. */
			switch (CGAtype())	{
				case 1:			/* It is an IDC, Compaq Plasma Display. */
					return COMPAQ_PLASMA;
				case 2:			/* ATT VDC 400 Monochrome */
					/* Since ATT Monochrome Indigenous Display Board uses the attdc.drv -
					 * the driver for ATT VDC 400 Monochrome, assume VDC 400 is a synonym
					 * of ATT_MONO_IDB */
					return ATT_MONO_IDB;
					break;
				case 3:
					if (MachineId == TANDY_1000)
						return 0;
					return IBMCGA;
				/* (HP c-ralphp 6/9/88) begin modification  */
				case 4:
					return HPMULTIMODE;
				default:
					break;
			}
			break;
		case 2:
			return MCGA;
		case 3:		/* EGA driving a monochrome display */
			return EGAHIRES_MONO;
		case 4:		/* EGA driving a color display */
			/* determine the amount of EGA memory. */
			switch (EGAmemory())	{
				case 0:
					/* when to return EGAHIRES_BW ? When to return EGALORES_COLOR ? 
					 * the user can have either one with 64K memory*/
					return EGAHIRES_BW;
				case 1:
				case 2:
				case 3:
					return EGA_GT_64K_COLOR;
				default:
					break;
			}
			break;
		case 5:
			if (detect8514())
				return GAD_8514;
			else
				return VGA;
		default:
			break;
	}
	
/*	if (GAD())
		return GAD_8514;
*/

	if (Herc())
		return HERC_HIRES_MONO;

	return 0;
}
																			
Get386Display(MachineId)
int MachineId;
{

	if (MachineId == 0)
		return 0;

	switch	(VideoAdapter())	{
		case 1:		/* It is a CGA */
			/* determine the CGA type. */
			switch	(CGAtype())	{
				case 1:		/* IDC - i.e. Compaq plasma Display. */
					return COMPAQ_PLASMA;
					break;
				case 2:		/* ATT VDC 400 Monochrome Adapter. */
					return ATT_VDC400_MONO;
					break;
				case 3:
					if (MachineId == COMPAQ_386) 
						return COMPAQ_VDCB;
					else
						return IBMCGA;
					break;
				default:
					break;
			}
		break;
	case 2:		/* MCGA, not supported by Win386 */
		return 0;
	case 3:		/* EGA monochrome display, not supported by Win386 */
		return 0;
	case 4:		/* EGA Color Display */
		if (EGAmemory() == 3)	{		/* 256K EGA memory */
			if (MachineId == ATT_6386)
				return ATT_VDC750_COLOR;
			else if (MachineId == COMPAQ_386)
				return COMPAQ_ECGB;
			else
				return EGA_256K_COLOR;
		}
		return 0;		/* EGA memory < 256K not supported by WIn386 */
	case 5:		/* VGA, default 640 X 450 */
		if (CTVGA())
			return CTVGA_640X450;
		else
			return VGA_640X450;
	default:
		break;
	}

	if (Herc())
		return HERC_HIRES_MONO;

	return 0;
}

/* returns TRUE is no mouse on the machine. */
/*
NoMouse(MachineId)
int MachineId;
{
*/
	/* IF clause:
	 * It is a PS2 machine and we have detected a mouse, hence return FALSE. */
/*	if ( (MachineId == IBMPS2_25_30 || MachineId == IBMPS2_50_60_80 ||
		 	MachineId == IBMPS2_80)	
			&&  (bMouseOnPS2()))
		return FALSE;
*/		
		/* ELSE clause:
		 * Serial Mouse on a PS2 not detected by bMouseOnPS2(). Use bMouse() to
		 * determine presence of either the serial mouse on PS2 or some other mouse.*/				
/*	else if (bMouse())
		return FALSE;

	return TRUE;
}
*/		
						
					
			
							
											
