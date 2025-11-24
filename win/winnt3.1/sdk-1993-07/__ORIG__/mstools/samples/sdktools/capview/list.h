/*
 *   LIST.H
 */

enum {
    LIST_ALPHA=3,
    LIST_TIME=1,
    LIST_PER_TIME=4,
    LIST_CALLS=2,
    LIST_TIME_TOTAL=LIST_TIME+8,
    LIST_PER_TIME_TOTAL=LIST_PER_TIME+8
};

class ListItem
{
    TIMETYPE    totalTime;
    TIMETYPE    timeAll;
    CString     strMod;
    CString     strFunc;
    int         calls;

public:
    ListItem(char * szModule, char * szName, int cCalls, TIMETYPE time, TIMETYPE timeAll);
    ~ListItem(){;};

    TIMETYPE    Time() { return totalTime; };
    TIMETYPE    TimeAll() { return timeAll; };
    int         Calls() { return calls; };
    TIMETYPE    TimePerCall()
    {
        TIMETYPE t;
        if (calls < 2) return totalTime;
        t = totalTime / calls;
        return t;
    }
    TIMETYPE    TimeAllPerCall()
    {
        TIMETYPE t;
        if (calls < 2) return timeAll;
        t = timeAll / calls;
        return t;
    }
    CString& strModule() { return strMod;};
    CString& strFunction() { return strFunc;};
    void AddTime(int callsx, TIMETYPE time2, TIMETYPE timeFunc)
    {
        totalTime += timeFunc;
        timeAll += time2;
        calls += callsx;
        return;
    }
};                               /* Class ListItem */


class ListArray : public CPtrArray
{
public:
    int Search(char * szModule, char * szName);
    void Sort(int (_CRTAPI1 * fnCmp)(const void *, const void *))
    {
    	qsort(m_pData, GetSize(), sizeof(int *), fnCmp);
    }
};



class ListHead
{
    ListArray           rgAlpha;
    ListArray           rgCalls;
    ListArray           rgRoutineTime;
    ListArray           rgTreeTime;
    ListArray           rgRoutineTimePer;
    ListArray           rgTreeTimePer;

    TIMETYPE            timeTotal;

public:

    ListHead() { timeTotal = (int) 0; return; }

    void AddTiming(char * szModule, char * szName, int cCalls,
                   TIMETYPE time, TIMETYPE timeAll);

    int GetCount() { return rgAlpha.GetSize(); };
    TIMETYPE TotalTime() { return timeTotal; };

    ListItem * GetPosition(int pos, int list = LIST_ALPHA)
    {
        switch( list ) {
        case LIST_ALPHA:                return (ListItem *) rgAlpha[pos];
        case LIST_CALLS:                return (ListItem *) rgCalls[pos];
        case LIST_TIME:                 return (ListItem *) rgRoutineTime[pos];
        case LIST_PER_TIME:             return (ListItem *) rgRoutineTimePer[pos];
        case LIST_TIME_TOTAL:           return (ListItem *) rgTreeTime[pos];
        case LIST_PER_TIME_TOTAL:       return (ListItem *) rgTreeTimePer[pos];
        default:                        return NULL;
        }
    }

    void SortByTotalTime();
    void SortByPerTime();
    void SortByCallCount();
    void SortByTimeAll();
    void SortByTPerTime();
};                              /* class ListHead */
