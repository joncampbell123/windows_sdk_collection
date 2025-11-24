#include <afxwin.h>
#include <afxcoll.h>

#include "types.h"
#include "list.h"

extern "C" char * UnDName(char *);

ListItem::ListItem(char * szModule, char * szName, int cCalls, TIMETYPE time2, TIMETYPE timeFunc) :
       strMod(szModule), strFunc(szName)
{
    totalTime = timeFunc;
    timeAll = time2;
    calls = cCalls;

    return;
}

int ListArray::Search(char * szModule, char * szName)
{
    int         i = 0;
    int         j = GetUpperBound();
    int         k;
    ListItem *  pli;

    if (j == -1) {
        return 0;
    }

    while ( i < j ) {
        k = (i + j) / 2;
        pli = (ListItem *) GetAt(k);

        if (pli->strModule() < szModule) {
            i = k + 1;
        } else if (pli->strModule() > szModule) {
            j = k - 1;
        } else if (pli->strFunction() < szName) {
            i = k + 1;
        } else if (pli->strFunction() > szName) {
            j = k - 1;
        } else {
            return k;
        }
    }

    pli = (ListItem *) GetAt(i);
    if (pli->strModule() < szModule) {
        i += 1;
    } else if ((pli->strModule() == szModule) &&
               (pli->strFunction() < szName)) {
        i += 1;
    }

    return i;
}                               /* ListArray::Search() */

void ListHead::AddTiming(char * szModule, char * szName, int cCalls,
                         TIMETYPE timeAll, TIMETYPE timeFunc)
{
    int         i;
    ListItem *  pli1;

    /*
     *  Keep track of the total tree time from the root
     */

    timeTotal += timeFunc;

    /*
     *  Find if the item is in the array already
     */

    i = rgAlpha.Search(szModule, szName);
    if (i <= rgAlpha.GetUpperBound()) {
        pli1 = (ListItem *) rgAlpha[i];

        if ((pli1->strModule() == szModule) &&
            (pli1->strFunction() == szName)) {

            pli1->AddTime(cCalls, timeAll, timeFunc);
            return;
        }
    }

    pli1 = new ListItem(szModule, szName, cCalls, timeAll, timeFunc);
    rgAlpha.InsertAt(i, pli1);

    return;
}                               /* ListHead::AddTiming() */


int _CRTAPI1 a(const void * pv1, const void * pv2)
{
    ListItem *	p1 = *(ListItem **) pv1;
    ListItem *  p2 = *(ListItem **) pv2;

    if (p1->Time() > p2->Time()) {
        return -1;
    } else if (p2->Time() > p1->Time()) {
    	return  1;
    }
    return 0;
}

void ListHead::SortByTotalTime()
{
    rgRoutineTime.SetSize(rgAlpha.GetSize());

    for (int i=0; i < rgAlpha.GetSize(); i++) {
        rgRoutineTime[i] = rgAlpha[i];
    }

    rgRoutineTime.Sort(a);
    return;
}                               /* ListHead::SortByTotalTime() */


int _CRTAPI1 b(const void * pv1, const void * pv2)
{
    ListItem *	p1 = *(ListItem **) pv1;
    ListItem *  p2 = *(ListItem **) pv2;

    if (p1->TimePerCall() > p2->TimePerCall()) {
        return -1;
    } else if (p2->TimePerCall() > p1->TimePerCall()) {
    	return  1;
    }
    return 0;
}

void ListHead::SortByPerTime()
{
    rgRoutineTimePer.SetSize(rgAlpha.GetSize());

    for (int i=0; i < rgAlpha.GetSize(); i++) {
        rgRoutineTimePer[i] = rgAlpha[i];
    }
    rgRoutineTimePer.Sort(b);
    return;
}                               /* ListHead::SortByPerTime() */


int _CRTAPI1 c(const void * pv1, const void * pv2)
{
    ListItem *	p1 = *(ListItem **) pv1;
    ListItem *  p2 = *(ListItem **) pv2;

    if (p1->Calls() > p2->Calls()) {
        return -1;
    } else if (p2->Calls() > p1->Calls()) {
    	return  1;
    }
    return 0;
}

void ListHead::SortByCallCount()
{
    rgCalls.SetSize(rgAlpha.GetSize());

    for (int i=0; i < rgAlpha.GetSize(); i++) {
        rgCalls[i] = rgAlpha[i];
    }
    rgCalls.Sort(c);
    return;
}                               /* ListHead::SortByCallCount() */

int _CRTAPI1 d(const void * pv1, const void * pv2)
{
    ListItem *	p1 = *(ListItem **) pv1;
    ListItem *  p2 = *(ListItem **) pv2;

    if (p1->TimeAll() > p2->TimeAll()) {
        return -1;
    } else if (p2->TimeAll() > p1->TimeAll()) {
    	return  1;
    }
    return 0;
}

void ListHead::SortByTimeAll()
{
    rgTreeTime.SetSize(rgAlpha.GetSize());

    for (int i=0; i < rgAlpha.GetSize(); i++) {
        rgTreeTime[i] = rgAlpha[i];
    }
    rgTreeTime.Sort(d);
    return;
}                               /* ListHead::SortByTimeAll() */


int _CRTAPI1 e(const void * pv1, const void * pv2)
{
    ListItem *	p1 = *(ListItem **) pv1;
    ListItem *  p2 = *(ListItem **) pv2;

    if (p1->TimeAllPerCall() > p2->TimeAllPerCall()) {
        return -1;
    } else if (p2->TimeAllPerCall() > p1->TimeAllPerCall()) {
    	return  1;
    }
    return 0;
}

void ListHead::SortByTPerTime()
{
    rgTreeTimePer.SetSize(rgAlpha.GetSize());

    for (int i=0; i < rgAlpha.GetSize(); i++) {
        rgTreeTimePer[i] = rgAlpha[i];
    }
    rgTreeTimePer.Sort(e);
    return;
}                               /* ListHead::SortByTPerTime() */



