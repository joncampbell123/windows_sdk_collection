/*++

  typeperf.c

  program to demonsrate the use of the PDH performance data collection DLL's
  
    this program is a Window NT console app that accepts as arguments
    PDH counter paths and then samples the valid counter paths at a 1 second
    interval and writes the output to the console output with a timestamp in 
    the format of a Comma Separated Variable file.

--*/
#if UNICODE
#ifndef _UNICODE
#define _UNICODE            1
#endif
#define tmain   wmain
#else
#define tmain   main
#endif

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>
#include <tchar.h>
#include <pdh.h>

#define SAMPLE_INTERVAL_MS  1000

void
DisplayCommandLineHelp ()
{
    _tprintf (TEXT("\n\nOne or more valid counter paths must be specified as a command line"));
    _tprintf (TEXT("\nargument."));
    _tprintf (TEXT("\nThe default sample interval is one second and the default output is"));
    _tprintf (TEXT("\na Comma Separated Variable (CSV) string"));
    return;
}

int
tmain (int argc,
      LPTSTR argv[])
{
    HQUERY          hQuery;
    HCOUNTER        *pCounterArray;
    HCOUNTER        *pThisCounterHandle;
    PDH_STATUS      pdhStatus;
    PDH_FMT_COUNTERVALUE   fmtValue;
    DWORD           ctrType;
    SYSTEMTIME      stSampleTime;
    
    int             nThisArg;
    int             nValidCounters;

    // the command line arguments are the counters to sample. the 
    // sample interval is 1 second.

    // count the arguments in the command line
    if (argc < 2) {
        // not enough arguments in the command line so
        // display explanation and exit
        DisplayCommandLineHelp ();
        return 1;
    }
    // there's at least one argument (counter) in the command line
    // so process it/them

    // open the PDH query object

    pdhStatus = PdhOpenQuery (0, 0, &hQuery);
    assert (pdhStatus == ERROR_SUCCESS);
    // allocate the counter handle array. allocate room for 
    // one handle per command line arg, not including the 
    // executable file name
    pCounterArray = (HCOUNTER *)GlobalAlloc(GPTR, 
        (sizeof(HCOUNTER) * (argc -1)));
    assert (pCounterArray != NULL);
    
    nValidCounters = 0;
    _tprintf (TEXT("\n\"Sample Time\""));
    for (nThisArg = 1; nThisArg < argc; nThisArg++) {
        pdhStatus = PdhAddCounter (hQuery,
            argv[nThisArg], 0, &pCounterArray[nThisArg-1]);
        if (pdhStatus != ERROR_SUCCESS) {
            // the counter didn't get added to the query, probably because
            // the path wasn't specified correctly or is not present
            printf ("\n\"%s\" is not a valid counter path", argv[nThisArg]);
            pCounterArray[nThisArg-1] = NULL;
        } else {
            // print counter name in heading line
            _tprintf (TEXT(",\"%s\""), argv[nThisArg]);
            nValidCounters++;
        }
    }

    // if there is at least one valid counter, then loop 
    //  until a key is pressed
    if (nValidCounters > 0) {
        // "prime" counters that need 2 values to display a formatted value
        pdhStatus = PdhCollectQueryData (hQuery);
        assert (pdhStatus == ERROR_SUCCESS);
        // loop until completion event occurs, in this case it's a 
        // key press.
        while (!_kbhit()) {
            // wait one interval....
            Sleep(SAMPLE_INTERVAL_MS);
            // get sample time
            GetLocalTime (&stSampleTime);
            // get the current data values
            pdhStatus = PdhCollectQueryData (hQuery);
            assert (pdhStatus == ERROR_SUCCESS);
            // print time stamp
            _tprintf (TEXT("\n\"%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d.%3.3d\""),
                stSampleTime.wMonth, stSampleTime.wDay, stSampleTime.wYear, 
                stSampleTime.wHour, stSampleTime.wMinute, stSampleTime.wSecond,
                stSampleTime.wMilliseconds);
            // find first valid counter in the list
            nThisArg = 0; 
            pThisCounterHandle = &pCounterArray[nThisArg]; 
            while ((*pThisCounterHandle == NULL) && (nThisArg < argc)) {
                nThisArg++;
                pThisCounterHandle++;
            }
            // pThisCounterHandle should point to the first valid counter
            // in the counter handle array.
            // so go from here to the end of the array and print the current
            // daa values

            while (nThisArg < (argc-1)) {
                if (*pThisCounterHandle != NULL) {
                    // get the current value for this counter
                    pdhStatus = PdhGetFormattedCounterValue (
                        *pThisCounterHandle,
                        PDH_FMT_DOUBLE,
                        &ctrType,
                        &fmtValue);
                    if (pdhStatus == ERROR_SUCCESS) {
                        _tprintf (TEXT(",\"%.20g\""), fmtValue.doubleValue);
                    } else {
                        _tprintf (TEXT(".\"-1\"")); // error value
                    }
                } 
                pThisCounterHandle++;
                nThisArg++;
            }
        }
    } else {
        printf ("\nNo counters to monitor.");
    }

    // clean up PDH interface and leave
    pdhStatus = PdhCloseQuery (hQuery);
    assert (pdhStatus == ERROR_SUCCESS);
    return 0;
}
