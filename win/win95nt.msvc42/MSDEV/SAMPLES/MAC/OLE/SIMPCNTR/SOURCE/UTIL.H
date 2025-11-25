#ifndef _UTILITIES_H_

#define _UTILITIES_H_



#define MAX(a,b)	((a)>(b) ? a : b)

#define MIN(a,b)	((a)<(b) ? a : b)





Boolean 	TrapExists			(short theTrap);

TrapType 	GetTrapType			(short theTrap);

short 		NumToolboxTraps		(void);

long		GetGestaltResult	(OSType gestaltSelector);

Boolean 	IsSystem7			(void);



void		GetGlobalMouse(Point *pt);

void		EnDisItem(MenuHandle hMenu, short theItem, Boolean enable);



#endif // _UTILITIES_H_