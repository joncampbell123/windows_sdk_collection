#ifndef __APPSVR_H__
#define __APPSVR_H__


#ifdef __PPCC__
struct OleApplicationRec;
#endif


// OLDNAME: OleInPlaceServerApplication.h
#if qOleInPlace

struct OleInPlaceServerAppRec {
	Boolean								m_fIgnoreResumeEvent;
	Boolean								m_fIgnoreSuspendEvent;
};

typedef struct OleInPlaceServerAppRec OleInPlaceServerAppRec, *OleInPlaceServerAppPtr;

Boolean OleInPlaceServerAppDoSuspend(struct OleApplicationRec* pOleApp);
Boolean OleInPlaceServerAppDoResume(struct OleApplicationRec* pOleApp);
#endif  // qOleInPlace

// OLDNAME: OleServerApplication.h
struct OleServerAppRec {
#if qOleInPlace
	OleInPlaceServerAppRec		inplace;
#else
	char						bogus;		// this is a bogus member so this struct is
#endif										// still valid when qOleInPlace isn't defined
};

typedef struct OleServerAppRec OleServerAppRec, *OleServerAppPtr;

void OleServerAppInit(struct OleApplicationRec* pOleApp);
void OleServerAppDispose(struct OleApplicationRec* pOleApp);

// OLDNAME: OleOutlineServer.h

void OleOutlineServerInit(struct OleOutlineAppRec* pOleOutlineApp, OSType creator);



#endif // __APPSVR_H__
