/********************************************************************/
/**                     Microsoft LAN Manager                      **/
/**               Copyright(c) Microsoft Corp., 1987-1991          **/
/********************************************************************/


/* 
 * General defines and macros usefull for any net api
 */
 
#define	API_SEM_TIMEOUT		5000L

/* PARM is used for getting a pointer to a SetInfo field when parmnum is
 * defined but may be 0. If parmnum is non zero then the buffer contains
 * the set field, else it contains a structure
 *
 * parameter 1 is the value 'returned' if no parmnum
 * parameter 2 is the value 'returned' if parmnum
 * 
 */
  

#define PARM( x,y) 	(!parmnum ? x : y)	

/* PARMPTR is used where parmnum is defined and returns true if
 * parmnum is non zero OR the pointer passed is not NULL. This is
 * used in SetInfo calls where the caller may pass a NULL pointer
 * for a settable field to leave as is.
 */
 
#define PARMPTR( x)	(parmnum || (x != NULL))


/* FLDCMP is used to compare a structure field which may not be NULL
 * terminated with a string which is. If the field is not full it must be
 * NULL padded or space padded but it may not be terminated if all 
 * characters in the field are significant.
 *
 * FLDCMP (a,b,c,d)
 *
 * where 	a = far pointer to string
 *		b = far pointer to field
 *		c = length of string a (not including '\0')
 *		d = field length
 */
 
#define FLDCMP(a,b,c,d) ((c <= d) && ((strncmpf(a,b,c)==0)&&\
				((c==d) || ((b[c]== ' ') || (b[c] == '\0')))))
				

/* FLDICMP is used to compare a structure field which may not be NULL
 * terminated with a string which is (case not significant). If the 
 * field is not full it must be NULL padded or space padded but it may not 
 * be terminated if all characters in the field are significant.
 *
 * FLDICMP (a,b,c,d)
 *
 * where 	a = far pointer to string
 *		b = far pointer to field
 *		c = length of string a (not including '\0')
 *		d = field length
 */
 
#define	FLDICMP(a,b,c,d) 	( (c <= d) && ((strnicmpf(a,b,c)==0)&&\
				((c==d) || ((b[c]== ' ') || (b[c] == '\0')))))
				
#define	COM_SPEC	"COMSPEC"
#define COM_SPEC_LEN	7
