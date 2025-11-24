/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

// PPINFO.H 
//
// Prototypes for private functions in PPINFO.H
//

DWORD _I_calc_prq3_size(PPRQINFO pOld);
LPSTR _I_build_prq3(PPRQINFO pOld,PPRQINFO3 pNew,LPSTR lpString);

PPRQINFO3 _I_map_enum_prq_to_prq3(PPRQINFO pOld,
                                  DWORD cbNumRecs,
                                  LPDWORD cbSize);

PPRQINFO3 _I_map_prq_to_prq3(PPRQINFO pOld,LPDWORD pcbSize);

PPRQINFO3 _I_net_enum_queues(LPSTR szServer, 
                           LPDWORD EntryCount,
                           LPDWORD cbSize);

BOOL _I_enum_printers_level1(PPRQINFO3 pNetInfo,
                             LPBYTE pBuffer,
                             DWORD NumQueues,
                             DWORD cbBufSize,
                             LPDWORD pcbNeeded,
                             LPDWORD pcbReturned);

BOOL _I_enum_printers_level2(LPSTR lpServerName,
                             PPRQINFO3 pNetInfo,
                             LPBYTE pBuffer,
                             DWORD NumQueues,
                             DWORD cbBufSize,
                             LPDWORD pcbNeeded,
                             LPDWORD pcbReturned);

PPRQINFO3 _I_net_get_info(HANDLE hPrinter,LPDWORD cbSize,LPDEVMODE *ppDevmode);

DWORD _I_gp1_calc_size(PPRQINFO3 pInfo);

LPSTR _I_gp1_build_record(PPRQINFO3 pInfo,
                          PPRINTER_INFO_1 pPrinter,
                          LPSTR lpString) ;

BOOL _I_get_printer_level1(PPRQINFO3 pInfo,
                           LPBYTE pBuffer,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded);

DWORD _I_gp2_calc_size(LPSTR lpServerName,
                       PPRQINFO3 pNetInfo,
                       LPDEVMODE pDevmode);

LPSTR _I_gp2_build_record(LPSTR lpServerName,
                         PPRQINFO3 pNetInfo,
                         PPRINTER_INFO_2 pPrinter,
                         LPSTR lpString,
                         LPDEVMODE pDevmode);


BOOL _I_get_printer_level2(PNETPRINTERQUEUE pHandleInfo,
                           PPRQINFO3 pNetInfo,
                           LPDEVMODE pDevmode,
                           LPBYTE pBuffer,
                           DWORD cbBuf,
                           LPDWORD pcbNeeded);


LPBYTE _I_map_pinfo1_to_prqinfo(PPRINTER_INFO_1 pInfo,LPDWORD pcbSize);
LPBYTE _I_map_pinfo2_to_prqinfo(PPRINTER_INFO_2 pInfo,LPDWORD pcbSize);

BOOL _I_do_set_printer_level1(LPSTR lpServer,
                              LPSTR lpQueue,
                              LPBYTE pNetQ,
                              DWORD NetQSize);

BOOL _I_do_set_printer_level2(LPSTR lpServer,
                              LPSTR lpQueue,
                              LPBYTE pNetQ,
                              DWORD NetQSize);


