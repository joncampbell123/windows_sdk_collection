//
// Structure for net error message translation
//
typedef struct _error_xlat_tag {

  USHORT in;
  DWORD  out;

} ERRORXLAT;

//
// Function prototypes
//
DWORD MapError(UINT NetErr);


