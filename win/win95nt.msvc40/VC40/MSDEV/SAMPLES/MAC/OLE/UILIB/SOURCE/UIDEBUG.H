#ifndef _DEBUG_H
#define _DEBUG_H


#ifndef ASSERTCOND
#ifdef _DEBUG
extern void __UIASSERTCONDSZ(char *, char *, char *, int);
#define ASSERTCOND(f) ( (f) ? (void) 0 : (__UIASSERTCONDSZ(#f, NULL, __FILE__, __LINE__)))
#else
#define ASSERTCOND(f) ((void)0)
#endif // _DEBUG
#endif // ASSERTCOND


#endif //!_DEBUG_H
