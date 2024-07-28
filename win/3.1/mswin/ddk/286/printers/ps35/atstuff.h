/*
 * atstuff.h	defs for Apple Talk code that is liked in with the PS driver
 *
 */

extern BOOL gfAT;

BOOL	FAR	PASCAL ATQuery(LPSTR);
void	FAR	PASCAL ATChangeState(BOOL);
BOOL	FAR	PASCAL ATBypass(LPSTR);
BOOL	FAR	PASCAL LoadAT(void);
void	FAR	PASCAL UnloadAT(void);
short	FAR	PASCAL TryAT(void);
void	FAR	PASCAL KillAT(void);

#if 0
BOOL	FAR	PASCAL ATState(void);	/* use a macro for this */
#endif

#define ATState()	(BOOL)gfAT
