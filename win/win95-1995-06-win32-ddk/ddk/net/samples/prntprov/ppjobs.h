/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

// PPJOBS.H
//
// Prototypes for private functions in PPJOBS.C
//

DWORD _I_map_job_status(USHORT uIn);

LPBYTE _I_net_enum_jobs(HANDLE hPrinter,LPDWORD cbSize,DWORD Level);

DWORD _I_ej1_calc_size(PNETPRINTERQUEUE pQueue,PPRJINFO pJob);

DWORD _I_ej2_calc_size(PNETPRINTERQUEUE pQueue,PPRJINFO pJob);

LPSTR _I_ej1_build_record(PNETPRINTERQUEUE pQueue,
                          PPRJINFO pNetJob,
                          PJOB_INFO_1 pJobInfo,
                          LPSTR lpString);

LPSTR _I_ej2_build_record(PNETPRINTERQUEUE pPrinter,
                          PPRQINFO pNetQueue,
                          PPRJINFO pNetJob,
                          PJOB_INFO_2 pJobInfo,
                          LPSTR lpString);

LPBYTE _I_net_get_job(LPSTR lpServer,
                      DWORD JobId,
                      DWORD Level,
                      LPDWORD pcbSize);

BOOL _I_enum_jobs_level1(PNETPRINTERQUEUE pQueue,
                         LPBYTE pInfo,
                         DWORD FirstJob,
                         DWORD NumJobs,
                         LPBYTE pBuffer,
                         DWORD  cbBuffer,
                         LPDWORD pcbNeeded,
                         LPDWORD pcbReturned);

BOOL _I_enum_jobs_level2(PNETPRINTERQUEUE pQueue,
                         LPBYTE pInfo,
                         DWORD FirstJob,
                         DWORD NumJobs,
                         LPBYTE pBuffer,
                         DWORD cbBuffer,
                         LPDWORD pcbNeeded,
                         LPDWORD pcbReturned);

BOOL _delete_job(HANDLE hPrinter,DWORD JobId) ; 
BOOL _pause_job(HANDLE hPrinter, DWORD JobId) ;

BOOL _set_job_level1(PNETPRINTERQUEUE pPrinter,
                     DWORD dwJobId,
                     PJOB_INFO_1 pJob);

BOOL _set_job_level2(PNETPRINTERQUEUE pPrinter,
                     DWORD dwJobId,
                     PJOB_INFO_2 pJob);
//
// Structure for print job status translation table
//
typedef struct _job_status_xlat_tag {
  USHORT in;
  DWORD  out;
} JOBSTATUSXLAT;
