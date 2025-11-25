//*---------------------------------------------------------------------------------
//|  ODBC System Administrator
//|
//|  This code is furnished on an as-is basis as part of the ODBC SDK and is
//|  intended for example purposes only.
//|
//|	Title:	RESOURCE.H
//*---------------------------------------------------------------------------------

#if defined(RC_INVOKED) || defined(APSTUDIO_INVOKED)
#include <windows.h>
#include <sqlext.h>
#endif

// Icon constants.
#define BASEICON				128
#define RSLTICON        		129
#define CONNECTICON     		130

// Menu constants.
#define BASEMENU				1000

#define IDM_WINDOW_POS		  	6
#define	IDM_FIRSTCHILD			200

#define IDM_FILE_MENU			0
#define	IDM_CONNECT				100
#define	IDM_DISCONNECT			101
#define	IDM_EXECUTE_FILE		102
#define	IDM_OPEN				103
#define	IDM_CLOSE				104
#define	IDM_SAVE				105
#define	IDM_SAVEAS				106
#define	IDM_EXIT				107

#define	IDM_EDIT_MENU			1
#define	IDM_UNDO				108
#define	IDM_CUT					109
#define	IDM_COPY				110
#define	IDM_PASTE				111
#define	IDM_SELECT_ALL			112

#define	IDM_EXECUTE_MENU		2
#define	IDM_EXECUTE_NOW			113
#define	IDM_COMMIT				115
#define	IDM_ROLLBACK			116

#define	IDM_PIPE_MENU			3
#define IDM_PIPE_EDIT			120
#define IDM_PIPE_DO				121

#define	IDM_INFO_MENU			4
#define IDM_MANAGE_DSNS			125
#define	IDM_DATA_SOURCES		126
#define	IDM_DATA_TYPES			127
#define	IDM_FUNCTIONS			128
#define	IDM_ODBC_INFO			129

#define	IDM_LIST_MENU			5
#define	IDM_TABLES				131
#define	IDM_COLUMNS				132
#define	IDM_STATISTICS			133
#define	IDM_PRIMARY_KEYS		134
#define	IDM_FOREIGN_KEYS		135
#define	IDM_TABLE_PRIVS			136
#define	IDM_COLUMN_PRIVS		137
#define	IDM_SPECIAL_COLUMNS		138
#define	IDM_PROCEDURES			139
#define	IDM_PROC_COLUMNS		140


#define	IDM_WINDOW_MENU			6
#define	IDM_FONT				150
#define	IDM_CASCADE				151
#define	IDM_TILE				152
#define	IDM_ARRANGE				153
#define	IDM_CLOSEALL			154

#define	IDM_ABOUT_MENU			7
#define IDM_HELP				160
#define	IDM_ABOUT				161

#define	IDM_MOVE_WINDOW			165			// F6 key


// String constants.
#define IDS_ASLMERROR                   			1045
#define idsAbout									7999
#define idsFileClosed								8000
#define idsSuccessConnect							8001
#define idsDisconnectSuccess						8002
#define idsErrorString								8003
#define idsMsgErrorString							8004
#define idsCloseFile								8005
#define idsOpenFileFailed							8006
#define idsSaveFileFailed							8007
#define idsSQLAllocEnvFailed						8008
#define idsSQLAllocConnectFailed					8009
#define idsStmtProcessed							8010
#define idsAffectedRows								8011
#define idsNoDataFound								8012
#define idsNumTooSmall								8013
#define idsTerminatorNotFound						8014
#define idsExecutingFile							8015
#define idsInvalidTerminator						8016
#define idsPipeNotFound								8017
#define idsInvalidParamValue						8018
#define idsOutOfMemory								8019
#define idsInvalidCmdLine							8020
#define idsCmdLineTooBig							8021
#define idsDftFileFilter                            8022
#define idsExpression								8023
#define idsInvalidInt								8024
#define idsTooManyParms								8025
#define idsInvalidName								8026
#define idsTooManyConnects							8027
#define idsMaxRecords								8028

// SQL String constants.

//---- Core Conformance -------------------------------------
#define idsSQLAllocConnect							100
#define idsSQLAllocEnv								101
#define idsSQLAllocStmt								102
#define idsSQLBindCol								103
#define idsSQLCancel								104
#define idsSQLColAttributes							105
#define idsSQLConnect								106
#define idsSQLDescribeCol							107
#define idsSQLDisconnect							108
#define idsSQLError									109
#define idsSQLExecDirect							110
#define idsSQLExecute								111
#define idsSQLFetch									112
#define idsSQLFreeConnect							113
#define idsSQLFreeEnv								114
#define idsSQLFreeStmt								115
#define idsSQLGetCursorName							116
#define idsSQLNumResultCols							117
#define idsSQLPrepare								118
#define idsSQLRowCount								119
#define idsSQLSetCursorName							120
#define idsSQLSetParam								121
#define idsSQLTransact								122
//---- Level 1 Conformance ----------------------------------	
#define idsSQLColumns								123
#define idsSQLDriverConnect							124
#define idsSQLGetConnectOption						125
#define idsSQLGetData								126
#define idsSQLGetFunctions							127
#define idsSQLGetStmtOption							128
#define idsSQLGetTypeInfo							129
#define idsSQLParamData								130
#define idsSQLPutData								131
#define idsSQLSetConnectOption						132
#define idsSQLSetStmtOption							133
#define idsSQLSpecialColumns						134
#define idsSQLStatistics							135
#define idsSQLTables								136
//---- Level 2 Conformance ----------------------------------
#define idsSQLBrowseConnect							137
#define idsSQLColumnPrivileges						138
#define idsSQLDataSources							139
#define idsSQLDescribeParam							140
#define idsSQLExtendedFetch							141
#define idsSQLForeignKeys							142
#define idsSQLMoreResults							143
#define idsSQLNativeSql								144
#define idsSQLNumParams								145
#define idsSQLParamOptions							146
#define idsSQLPrimaryKeys							147
#define idsSQLProcedureColumns						148
#define idsSQLProcedures							150
#define idsSQLSetPos								151
#define idsSQLSetScrollOptions						152
#define idsSQLTablePrivileges						153

//
// SQLGetInfo Types
//
#define idsGetInfoBase				  				200

#define idsSQL_FD_FETCH_NEXT  						300
#define idsSQL_FD_FETCH_FIRST 						301
#define idsSQL_FD_FETCH_LAST  						302
#define idsSQL_FD_FETCH_PREV          				303
#define idsSQL_FD_FETCH_ABSOLUTE      				304
#define idsSQL_FD_FETCH_RELATIVE      				305
#define idsSQL_FD_FETCH_RESUME        				306

#define idsSQL_CONFORM_NONE							307
#define idsSQL_LEVEL1_NONE							308
#define idsSQL_LEVEL2_NONE							309

#define idsSQL_NONSAG								310
#define idsSQL_SAG									311

#define	idsCONFORM_MINIMUM							312
#define	idsCONFORM_CORE								313
#define	idsCONFORM_EXTENDED							314

#define	idsCONCAT_0									315
#define	idsCONCAT_1									316

#define	idsCOMMIT_0									317
#define	idsCOMMIT_1									318
#define	idsCOMMIT_2									319

#define	idsROLLBACK_0								320
#define	idsROLLBACK_1								321
#define	idsROLLBACK_2								322

#define	idsUPPERCASE								323
#define	idsLOWERCASE								324
#define	idsCASESENSITIVE							325
#define	idsNOTCASESENSITIVE							326

#define	idsSQL_TXN_READ_UNCOMMITTED					327
#define	idsSQL_TXN_READ_COMMITTED					328
#define	idsSQL_TXN_REPEATABLE_READ					329
#define	idsSQL_TXN_SERIALIZABLE						330
#define	idsSQL_TXN_VERSIONING						331

#define	idsSQL_SCCO_READ_ONLY						332
#define	idsSQL_SCCO_LOCK							333
#define	idsSQL_SCCO_OPT_TIMESTAMP					334
#define	idsSQL_SCCO_OPT_VALUES						335

#define	idsSQL_SO_FORWARD_ONLY						336
#define	idsSQL_SO_KEYSET_DRIVEN						337
#define	idsSQL_SO_DYNAMIC							338
#define	idsSQL_SO_MIXED								339

#define	idsNOTRANS									340
#define	idsDMLSUPPORT								341
#define	idsDDLSUPPORT								342

#define idsSQL_FN_CVT_CONVERT						343

#define	idsSQL_FN_NUM_ABS							344
#define	idsSQL_FN_NUM_ACOS            				345
#define	idsSQL_FN_NUM_ASIN            				346
#define	idsSQL_FN_NUM_ATAN            				347
#define	idsSQL_FN_NUM_ATAN2           				348
#define	idsSQL_FN_NUM_CEILING      					349
#define	idsSQL_FN_NUM_COS           				350
#define	idsSQL_FN_NUM_COT           				351
#define	idsSQL_FN_NUM_EXP           				352
#define	idsSQL_FN_NUM_FLOOR         				353
#define	idsSQL_FN_NUM_LOG           				354
#define	idsSQL_FN_NUM_MOD           				355
#define	idsSQL_FN_NUM_RAND          				356
#define	idsSQL_FN_NUM_PI            				357
#define	idsSQL_FN_NUM_SIGN          				358
#define	idsSQL_FN_NUM_SIN           				359
#define	idsSQL_FN_NUM_SQRT          				360
#define	idsSQL_FN_NUM_TAN           				361

#define	idsSQL_FN_STR_ASCII							362
#define	idsSQL_FN_STR_CHAR							363
#define	idsSQL_FN_STR_CONCAT						364
#define	idsSQL_FN_STR_INSERT						365
#define	idsSQL_FN_STR_LEFT							366
#define	idsSQL_FN_STR_LTRIM							367
#define	idsSQL_FN_STR_LENGTH						368
#define	idsSQL_FN_STR_LOCATE						369
#define	idsSQL_FN_STR_LCASE							370
#define	idsSQL_FN_STR_REPEAT						371
#define	idsSQL_FN_STR_REPLACE						372
#define	idsSQL_FN_STR_RIGHT							373
#define	idsSQL_FN_STR_RTRIM							374
#define	idsSQL_FN_STR_SUBSTRING						375
#define	idsSQL_FN_STR_UCASE							376

#define	idsSQL_FN_SYS_USERNAME						377
#define	idsSQL_FN_SYS_DBNAME						378
#define	idsSQL_FN_SYS_IFNULL						379

#define	idsSQL_FN_TD_NOW							380
#define	idsSQL_FN_TD_CURDATE						381
#define	idsSQL_FN_TD_DAYOFMONTH						382
#define	idsSQL_FN_TD_DAYOFWEEK						383
#define	idsSQL_FN_TD_DAYOFYEAR						384
#define	idsSQL_FN_TD_MONTH							385
#define	idsSQL_FN_TD_QUARTER						386
#define	idsSQL_FN_TD_WEEK							387
#define	idsSQL_FN_TD_YEAR							388
#define	idsSQL_FN_TD_CURTIME						389
#define	idsSQL_FN_TD_HOUR							390
#define	idsSQL_FN_TD_MINUTE							391
#define	idsSQL_FN_TD_SECOND							392

#define	idsCVT_BIGINT								393
#define	idsCVT_BINARY								394
#define	idsCVT_BIT									395
#define	idsCVT_CHAR									396
#define	idsCVT_DATE									397
#define	idsCVT_DECIMAL								398
#define	idsCVT_DOUBLE								399
#define	idsCVT_FLOAT								400
#define	idsCVT_INTEGER								401
#define	idsCVT_LONGVARBINARY						402
#define	idsCVT_LONGVARCHAR							403
#define	idsCVT_NUMERIC								404
#define	idsCVT_REAL									405
#define	idsCVT_SMALLINT								406
#define	idsCVT_TIME									407
#define	idsCVT_TIMESTAMP							408
#define	idsCVT_TINYINT								409
#define	idsCVT_VARBINARY							410
#define	idsCVT_VARCHAR								411

#define	IDD_EDIT_PIPE				500
#define	IDC_NAME					501
#define	IDB_NEW						502
#define	IDB_DELETE					503
#define	IDE_SQL						504
#define	IDR_VALUE					505
#define	IDR_ADDRESS					506
#define	IDE_PARMS					507
#define IDX_DELETE				  	508

#define	IDD_NEW_PIPE				530
#define IDD_DO_PIPE				  	550


// Accelerator constant.
#define BASEACCEL				1000

// Dialog constants.
#define	IDD_ABOUTBOX				2000
#define	IDD_SQLTABLES				3000
#define	IDD_SQLCOLUMNS				3001
#define	IDD_SQLSPECIALCOLUMNS		3002
#define	IDD_SQLSTATISTICS			3003
#define	IDD_SQLPRIMARYKEYS			3004
#define	IDD_SQLFOREIGNKEYS			3005
#define	IDD_SQLTABLEPRIVILEGES		3006
#define	IDD_SQLCOLUMNPRIVILEGES		3007
#define	IDD_SQLPROCEDURES			3008
#define	IDD_SQLPROCEDURECOLUMNS		3009

#define	IDT_TABLE					301
#define	IDT_TYPE					303
#define	IDT_COLUMN					304

#define	IDE_TABLE					331
#define	IDE_TYPE					333
#define	IDE_COLUMN					334


#define IDE_QUALIFIER				151
#define	IDE_OWNER					153
#define	IDE_NAME					155

#define IDL_LIST					157

#define	IDX_ALL						158
#define	IDX_TABLES					159
#define	IDX_VIEWS					160
#define	IDL_PIPES					162


#define	IDX_BEST_ROWID				340
#define	IDX_ROWVER					341
#define	IDX_NO_NULLS				342
#define	IDX_NULLABLE				343
#define	IDX_SCOPE_CURROW			344
#define	IDX_SCOPE_TRANSACTION		345
#define	IDX_SCOPE_SESSION			346

#define	IDX_INDEX_UNIQUE			360
#define	IDX_INDEX_ALL				361
#define	IDX_ENSURE					362
#define	IDX_QUICK					363

#define	IDT_QUALIFIER2				365
#define	IDT_OWNER2					366
#define	IDT_TABLE2					367
#define	IDE_QUALIFIER2				368
#define	IDE_OWNER2					369
#define	IDE_TABLE2					370

#define	IDT_PROCEDURE				375
#define	IDE_PROCEDURE				376



