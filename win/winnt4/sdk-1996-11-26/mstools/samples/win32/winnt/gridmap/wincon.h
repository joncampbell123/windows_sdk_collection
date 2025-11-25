// Keisen support on console

#define NLS_DBCSCHAR          0x00010000 // DBCS for JPN: SBCS/DBCS mode.
#define NLS_ALPHANUMERIC      0x00000000 // DBCS for JPN: Alphanumeric mode.
#define NLS_KATAKANA          0x00020000 // DBCS for JPN: Katakana mode.
#define NLS_HIRAGANA          0x00040000 // DBCS for JPN: Hiragana mode.
#define NLS_ROMAN             0x00400000 // DBCS for JPN: Roman/Noroman mode.
#define NLS_IME_CONVERSION    0x00800000 // DBCS for JPN: IME conversion.
#define NLS_IME_DISABLE       0x20000000 // DBCS for JPN: IME enable/disable.

#define COMMON_LVB_LEADING_BYTE    0x0100 // Leading Byte of DBCS
#define COMMON_LVB_TRAILING_BYTE   0x0200 // Trailing Byte of DBCS
#define COMMON_LVB_GRID_HORIZONTAL 0x0400 // DBCS: Grid attribute: top horizontal.
#define COMMON_LVB_GRID_LVERTICAL  0x0800 // DBCS: Grid attribute: left vertical.
#define COMMON_LVB_GRID_RVERTICAL  0x1000 // DBCS: Grid attribute: right vertical.
#define COMMON_LVB_REVERSE_VIDEO   0x4000 // DBCS: Reverse fore/back ground attribute.
#define COMMON_LVB_UNDERSCORE      0x8000 // DBCS: Underscore.

#define COMMON_LVB_SBCSDBCS        0x0300 // SBCS or DBCS flag.


