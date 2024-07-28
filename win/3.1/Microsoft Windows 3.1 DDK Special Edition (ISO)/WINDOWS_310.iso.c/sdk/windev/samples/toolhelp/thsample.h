/**************************************************************************
 *  TEST.H
 *
 *      Header file for the TOOLHELP.DLL test program
 *
 **************************************************************************/

/* Menu Symbols */
#define ID_MENU         1
#define IDM_TEST_1      1001
#define IDM_TEST_2      1002
#define IDM_TEST_3      1003
#define IDM_TEST_4      1004
#define IDM_TEST_5      1005
#define IDM_TEST_6      1006
#define IDM_TEST_7      1007
#define IDM_TEST_8      1008
#define IDM_TEST_9      1009
#define IDM_TEST_10     1010
#define IDM_TEST_11     1011
#define IDM_TEST_12     1012
#define IDM_EXIT        1100

#define IDM_FAULT_1     2001
#define IDM_FAULT_2     2002
#define IDM_FAULT_3     2003
#define IDM_FAULT_4     2004
#define IDM_FAULT_5     2005
#define IDM_FAULT_6     2006
#define IDM_FAULT_7     2007
#define IDM_FAULT_8     2008
#define IDM_FAULT_9     2009

#define IDM_NOTIFY_ENABLE   3001
#define IDM_FILTER_ENABLE   3002
#define IDM_NOTIFY_CLEAR    3003

#define IDD_FAULT       1
#define IDC_KILL        1
#define IDC_RESTART     101
#define IDC_CHAIN       102
#define IDC_CSIP        200
#define IDC_FAULTNUM    201
#define IDC_HFAULT      202
#define IDC_HPROGRAM    203
#define IDC_STATIC      300

/* ----- Function prototypes ----- */

    void PASCAL Fault(
        WORD wType);

    void FAR PASCAL MyFaultHandler(void);

