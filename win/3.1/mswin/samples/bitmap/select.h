/* These defines determine the meaning of the fFlags variable.  The low byte
 * is used for the various types of "boxes" to draw.  The high byte is
 * available for special commands.
 */

#define SL_BOX    1             /* Draw a solid border around the rectangle  */
#define SL_BLOCK  2             /* Draw a solid rectangle                    */

#define SL_EXTEND 256           /* Extend the current pattern                */

#define SL_TYPE    0x00FF       /* Mask out everything but the type flags    */
#define SL_SPECIAL 0xFF00       /* Mask out everything but the special flags */

void StartSelection(HWND, POINT, LPRECT, int);
void UpdateSelection(HWND, POINT, LPRECT, int);
void EndSelection(POINT, LPRECT);
void ClearSelection(HWND, LPRECT, int);
