//cl.cpp
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

#include "stdafx.h"
#include <afx.h>
#include <sqlext.h>
#include "cl.h"
#include "SendTime.h"

ComLoc::ComLoc()                                        //empty constructor
{
}


ComLoc::~ComLoc()                                       //destructor
{
}


void ComLoc::AllocEnv()                                 //allocate the environment handle
{
        cl_rc = SQLAllocEnv(&cl_henv_handle);
}


void ComLoc::FreeEnv()                                  //free the environment handle
{
        cl_rc = SQLFreeEnv(&cl_henv_handle);
}


void ComLoc::Open()                                     //connect to datasource
{
        UCHAR cl_sz_DSN[] = "ComLoc";
        UCHAR cl_sz_UID[] = "sa";
        UCHAR cl_sz_PWD[] = "";

        //Allocate connection handle
        cl_rc = SQLAllocConnect(
                        cl_henv_handle,                 //environment handle
                        &cl_hdbc_connection);           //connection handle

        //Connect to data source
        cl_rc = SQLConnect(
                        cl_hdbc_connection,             //connection handle
                        cl_sz_DSN,                      //data source
                        SQL_NTS,                        //length of data source
                        cl_sz_UID,                      //user identifier
                        SQL_NTS,                        //length of identifier
                        cl_sz_PWD,                      //password
                        SQL_NTS);                       //length of password

        //Allocate the allRows statement and set the options
        cl_rc = SQLAllocStmt(
                        cl_hdbc_connection,             //connection handle
                        &cl_hstmt_allRows);             //pointer to storage for statement handle

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_allRows,               //statement handle
                        SQL_CONCURRENCY,                //option to set
                        SQL_CONCUR_ROWVER);             //set lowest lock level

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_allRows,               //statement handle
                        SQL_CURSOR_TYPE,                //option to set
                        SQL_CURSOR_DYNAMIC);            //set dynamic cursor

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_allRows,               //statement handle
                        SQL_BIND_TYPE,                  //option to set
                        sizeof(str_cl));                //specify row-wise binding

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_allRows,               //statement handle
                        SQL_ROWSET_SIZE,                //option to set
                        1);                             //set rowset size

        //Bind the allRows columns
        cl_rc = SQLBindCol(
                        cl_hstmt_allRows,               //statement handle
                        1,                              //column number of result data
                        SQL_C_CHAR,                     //C data type of column
                        cl.cl_machine,                  //pointer to storage for data
                        cb_CL_MACHINE + 1,              //maximum length of buffer
                        NULL);

        cl_rc = SQLBindCol(
                        cl_hstmt_allRows,               //statement handle
                        2,                              //column number of result data
                        SQL_C_CHAR,                     //C data type of column
                        cl.cl_progid,                   //pointer to storage for data
                        cb_CL_PROGID + 1,               //maximum length of buffer
                        NULL);

        //Execute the showactivity stored procedure     //NOTE: This messages returns
        UCHAR cl_sz_showactivity[] = "showactivity";    //SQL_SUCCESS_WITH_INFO

        cl_rc = SQLExecDirect(
                        cl_hstmt_allRows,               //statement handle
                        cl_sz_showactivity,             //SQL statement
                        SQL_NTS);                       //length of statement

        UCHAR szSqlState[6];                            //Retrieve showactivity PRINT message
        UCHAR szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
        SDWORD pfNativeError;
        SWORD pcbErrorMsg;
        cl_rc = SQLError(
                        cl_henv_handle,
                        cl_hdbc_connection,
                        cl_hstmt_allRows,
                        szSqlState,
                        &pfNativeError,
                        szErrorMsg,
                        (SQL_MAX_MESSAGE_LENGTH - 1),
                        &pcbErrorMsg);

        AfxMessageBox((char*)szErrorMsg);              //display PRINT message


        //Select all
        UCHAR cl_sz_allRows[] = "SELECT * FROM ComLoc";

        cl_rc = SQLExecDirect(
                        cl_hstmt_allRows,               //statement handle
                        cl_sz_allRows,                  //SQL statement
                        SQL_NTS);                       //length of statement

        //Get first row
        cl_rc = SQLExtendedFetch(
                        cl_hstmt_allRows,               //statement handle
                        SQL_FETCH_FIRST,                //type of fetch
                        1,                              //number of row to fetch
                        &cl_pcrow_allRows,              //number of rows actually fetched
                        cl_rgf_RowStatus);              //status

        this->Store();                                  //store row

        cl_f_allRowsIsActive = TRUE;

        //If not found, display error message
        if (cl_rc != 0)
        {
                char szMessageText[40];
                wsprintf(
                szMessageText,
                        "First record in table not found");
                AfxMessageBox(szMessageText);
        }

        //Allocate the findRow statement and set the options
        cl_rc = SQLAllocStmt(
                        cl_hdbc_connection,             //connection handle
                        &cl_hstmt_findRow);             //pointer to storage for statement handle

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_findRow,               //statement handle
                        SQL_CONCURRENCY,                //option to set
                        SQL_CONCUR_ROWVER);             //set lowest lock level

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_findRow,               //statement handle
                        SQL_CURSOR_TYPE,                //option to set
                        SQL_CURSOR_DYNAMIC);            //set dynamic cursor

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_findRow,               //statement handle
                        SQL_BIND_TYPE,                  //option to set
                        sizeof(str_cl));                //specify row-wise binding

        cl_rc = SQLSetStmtOption(
                        cl_hstmt_findRow,               //statement handle
                        SQL_ROWSET_SIZE,                //option to set
                        1);                             //set rowset size

        //Prepare the findRow statement
        UCHAR cl_sz_findRow[] =
        "SELECT * FROM ComLoc WHERE cl_machine = ?";

        cl_rc = SQLPrepare(
                        cl_hstmt_findRow,               //statement handle
                        cl_sz_findRow,                  //SQL statement
                        sizeof(cl_sz_findRow));         //size of statement

        //Bind the findRow parameters
        cl_rc = SQLBindParameter(
                        cl_hstmt_findRow,               //statement handle
                        1,                              //parameter number
                        SQL_PARAM_INPUT,                //parameter type
                        SQL_C_CHAR,                     //C data type of parameter
                        SQL_CHAR,                       //SQL data type of parameter
                        cb_CL_MACHINE,                  //length of column (precision)
                        0,                              //scale of column
                        cl_machineFind,                 //ptr to a buffer with parameter data
                        0,                              //maximum length of buffer
                        NULL);                          //ptr to buffer for parameter's length

        //Bind the findRow columns
        cl_rc = SQLBindCol(
                        cl_hstmt_findRow,               //statement handle
                        1,                              //column number of result data
                        SQL_C_CHAR,                     //C data type of column
                        cl.cl_machine,                  //pointer to storage for data
                        cb_CL_MACHINE + 1,              //maximum length of buffer
                        &cl.cb_cl_machine);             //pointer to storage for data length

        cl_rc = SQLBindCol(
                        cl_hstmt_findRow,               //statement handle
                        2,                              //column number of result data
                        SQL_C_CHAR,                     //C data type of column
                        cl.cl_progid,                   //pointer to storage for data
                        cb_CL_PROGID + 1,               //maximum length of buffer
                        &cl.cb_cl_progid);              //pointer to storage for data length

        //Set the first time through flag
        cl_f_findRowFirstTime = TRUE;
}


void ComLoc::Close()                        //free handles and disconnect
{
        cl_rc = SQLFreeStmt(
                        cl_hstmt_allRows,               //statement handle
                        SQL_DROP);                      //type of free

        cl_rc = SQLFreeStmt(
                        cl_hstmt_findRow,               //statement handle
                        SQL_DROP);                      //type of free

        cl_rc = SQLDisconnect(
                        cl_hdbc_connection);            //connection handle

        cl_rc = SQLFreeConnect(
                        cl_hdbc_connection);            //connection handle
}

short int ComLoc::First()                   //get first row
{
        //Get first row
        cl_rc = SQLExtendedFetch(
                        cl_hstmt_allRows,               //statement handle
                        SQL_FETCH_FIRST,                //type of fetch
                        1,                              //number of row to fetch
                        &cl_pcrow_allRows,              //number of rows actually fetched
                        cl_rgf_RowStatus);              //status

        cl_f_allRowsIsActive = TRUE;

        return cl_rc;
}


void ComLoc::Last()                         //get last row
{
        //Get last row
        cl_rc = SQLExtendedFetch(
                        cl_hstmt_allRows,               //statement handle
                        SQL_FETCH_LAST,                 //type of fetch
                        1,                              //number of row to fetch
                        &cl_pcrow_allRows,              //number of rows actually fetched
                        cl_rgf_RowStatus);              //status

        cl_f_allRowsIsActive = TRUE;
}


short int ComLoc::Next()                    //get next row
{
        //Get next row
        cl_rc = SQLExtendedFetch(
                        cl_hstmt_allRows,               //statement handle
                        SQL_FETCH_NEXT,                 //type of fetch
                        1,                              //number of row to fetch
                        &cl_pcrow_allRows,              //number of rows actually fetched
                        cl_rgf_RowStatus);              //status

        cl_f_allRowsIsActive = TRUE;

        return cl_rc;
}


void ComLoc::Prev()                         //get previous row
{
        //Get previous row
        cl_rc = SQLExtendedFetch(
                        cl_hstmt_allRows,               //statement handle
                        SQL_FETCH_PREV,                 //type of fetch
                        1,                              //number of row to fetch
                        &cl_pcrow_allRows,              //number of rows actually fetched
                        cl_rgf_RowStatus);              //status

        cl_f_allRowsIsActive = TRUE;
}

void ComLoc::Find()                         //get specific row
{
        //Free the previous cursor and results set
        if (!cl_f_findRowFirstTime)
        {
                cl_rc = SQLFreeStmt(
                                cl_hstmt_findRow,           //statement handle
                                SQL_CLOSE);                 //type of free
        }
        else
        {
                cl_f_findRowFirstTime = FALSE;
        }

        //Execute the statement
        cl_rc = SQLExecute(cl_hstmt_findRow);   //statement handle

        //Fetch the row
        cl_rc = SQLExtendedFetch(
                        cl_hstmt_findRow,               //statement handle
                        SQL_FETCH_NEXT,                 //type of fetch
                        1,                              //number of rows to fetch
                        &cl_pcrow_findRow,              //number of rows actually fetched
                        cl_rgf_RowStatus);              //status

        //Display text
        if (cl_rc == 0)
        {
                cl_f_allRowsIsActive = FALSE;
        }
        else
        {
                char szMessageText[40];             //if not found, display error message
                wsprintf(szMessageText,
                                "Record not found: %s",
                                (char*)cl_hold.cl_progid);
                AfxMessageBox(szMessageText);
        }
}


void ComLoc::Insert()                       //insert a new a row
{
        //Insert this row
        cl_rc = SQLSetPos(
                        cl_hstmt_allRows,               //statement handle
                        1,                              //position of row
                        SQL_ADD,                        //type of operation
                        SQL_LOCK_NO_CHANGE);            //type of locking
}


void ComLoc::Update()                       //update a row
{
        //Update the row
        if (cl_f_allRowsIsActive)
        {
                                cl_rc = SQLSetPos(
                                cl_hstmt_allRows,           //statement handle
                                1,                          //position of row
                                SQL_UPDATE,                 //type of operation
                                SQL_LOCK_NO_CHANGE);        //type of locking
        }
        else
        {
                cl_rc = SQLSetPos(
                                cl_hstmt_findRow,           //statement handle
                                1,                          //position of row
                                SQL_UPDATE,                 //type of operation
                                SQL_LOCK_NO_CHANGE);        //type of locking
        }
}


void ComLoc::Delete()                       //delete a row
{
        //Delete the current row
        if (cl_f_allRowsIsActive)
        {
                cl_rc = SQLSetPos(
                                cl_hstmt_allRows,           //statement handle
                                1,                          //position of row
                                SQL_DELETE,                 //type of operation
                                SQL_LOCK_NO_CHANGE);        //type of locking
        }
        else
        {
                cl_rc = SQLSetPos(
                                cl_hstmt_findRow,           //statement handle
                                1,                          //position of row
                                SQL_DELETE,                 //type of operation
                                SQL_LOCK_NO_CHANGE);        //type of locking
        }
}


void ComLoc::Store()                        //store row data into hold area
{
        //Store row data
        strcpy(
                (char*) cl_hold.cl_progid,
                (char*) cl.cl_progid);
        strcpy(
                (char*) cl_hold.cl_machine,
                (char*) cl.cl_machine);
}


void ComLoc::Restore()                      //restore row data from hold area
{
        //Restore row data
        strcpy(
                (char*) cl.cl_progid,
                (char*) cl_hold.cl_progid);
        strcpy(
                (char*) cl.cl_machine,
                (char*) cl_hold.cl_machine);
}
