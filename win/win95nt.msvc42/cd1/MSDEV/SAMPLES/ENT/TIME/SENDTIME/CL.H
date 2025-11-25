//cl.h
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

//Description
//-----------
//
//This class definition provides standard browse and maintenance
//functionality for the ComLoc table:
//
//  - get first row
//  - get next row
//  - get last row
//  - get prev row
//  - find a specific row
//  - insert a new row
//  - update the current row
//  - delete the current row
//
//Two Statement Handles
//---------------------
//
//This class uses two statements to provide browsing and find
//functionality:
//
//  - hstmt_cl_allRows  Selects all rows for browsing.
//  - hstmt_cl_findRow  Selects a single row matching the parameterized
//                      column value.
//
//The columns are bound by row, and only one row (the current row) is
//fetched into the application.
//
//The Find Method
//---------------
//
//The Find method uses one column (cl_machine) to select a row. This
//implies that the cl_machine column entries should be unique.

#ifndef CL_H
#define CL_H

#include <SQLEXT.H>

class CSendTimeDlg;                             //forward declaration

class ComLoc
{
public:
        ComLoc();                               //empty constructor
        ~ComLoc();                              //destructor
        void AllocEnv();                        //allocate the environment handle
        void FreeEnv();                         //free the environment handle
        void Open();                            //open to the datasource
        void Close();                           //close the datasource
        short int First();                      //get first row
        void Last();                            //get last row
        short int Next();                       //get next row
        void Prev();                            //get previous row
        void Find();                            //get specific row
        void Insert();                          //add a new row
        void Update();                          //update a row
        void Delete();                          //delete a row
        void Store();                           //store row data
        void Restore();                         //restore row data

        CSendTimeDlg* m_pDlg;                   //use forward declaration

        //Define length of each column
        #define cb_CL_MACHINE 32
        #define cb_CL_PROGID 32

        UCHAR cl_machineFind[cb_CL_MACHINE + 1];//value for find parameter
        UDWORD cl_pcrow_allRows;                //number of rows actually fetched

private:
        HENV cl_henv_handle;                    //environment handle
        HDBC cl_hdbc_connection;                //connection handle
        RETCODE cl_rc;                          //return code for ODBC functions
        UWORD cl_rgf_RowStatus[1];              //status for fetched row

        //Miscellaneous variables
        HSTMT cl_hstmt_findRow;                 //statement handle
        UDWORD cl_pcrow_findRow;                //number of rows actually fetched
        BOOL cl_f_findRowFirstTime;             //indicates first time through routine

        HSTMT cl_hstmt_allRows;                 //statement handle
        BOOL cl_f_allRowsIsActive;              //indicates which statement is active

public:
        //Structure for a row of column values and lengths
        typedef struct
        {
                UCHAR   cl_machine[cb_CL_MACHINE + 1];
                SDWORD  cb_cl_machine;
                UCHAR   cl_progid[cb_CL_PROGID + 1];
                SDWORD  cb_cl_progid;
        } str_cl;

        str_cl cl;                              //primary input buffer for row of data
        str_cl cl_hold;                         //holding area for row of data

};

#endif CL_H
