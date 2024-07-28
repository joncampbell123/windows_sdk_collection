/**[f******************************************************************
* loaderr.h -
*
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/* define Loader error code */
#define ld_err_arg      99   /* Invalid input argument(s)                   */
#define ld_err_mem     100   /* Insufficient memory                         */
#define ld_err_1       101   /* Illegal library type                        */
#define ld_err_2       102   /* Cg font diskette (FONTINDX.FI) not found    */
#define ld_err_3       103   /* Error reading Cg font disk (FONTINDX.FI)    */
#define ld_err_4       104   /* Maximum files limit exceeded                */
#define ld_err_5       105   /* Typeface/Complement number not found        */
#define ld_err_6       106   /* Can't open font file                        */
#define ld_err_7       107   /* HiQ Data segment not found                  */
#define ld_err_8       108   /* Invalid HiQ type code                       */
#define ld_err_9       109   /* Global Character Data segment not found     */
#define ld_err_10      110   /* Cg Character Number Data segment not found  */
#define ld_err_11      111   /* Can't open Attribute file                   */
#define ld_err_12      112   /* Invalid file code                           */
#define ld_err_13      113   /* Font Header segment not found               */
#define ld_err_14      114   /* Invalid FAIS format version                 */
#define ld_err_15      115   /* Character Width segment not found           */
#define ld_err_16      116   /* Typeface already in library                 */
#define ld_err_17      117   /* End Of HiQ segment not found                */
#define ld_err_18      118   /* Font character array not ordered            */
#define ld_err_19      119   /* Maximum character size exceeded             */
#define ld_err_20      120   /* Character size <= minimum size              */
#define ld_err_21      121   /* Inconsistancy in character data             */
#define ld_err_22      122   /* Library does not contain a file segment     */
#define ld_err_23      123   /* File directory space exhausted              */
#define ld_err_24      124   /* Font file read error                        */
#define ld_err_25      125   /* Invalid hiQ character data format           */
#define ld_err_26      126   /* Write library file error, may be disk full  */
#define ld_err_27      127   /* Write map file error, may be disk full      */
#define ld_err_28      128   /* Write temp_data_file error, may be disk full*/
  
/* define Loader warning code */
#define ld_wrn_1      1001   /* Display Header segment not found            */
#define ld_wrn_2      1002   /* Font file contains Pi characters            */
#define ld_wrn_3      1003   /* Track Kerning segment not found             */
#define ld_wrn_4      1004   /* Attribute Header segment not found          */
#define ld_wrn_5      1005   /* Text Kerning segment not found              */
#define ld_wrn_6      1006   /* Text/Designer kerning not Cg standard       */
#define ld_wrn_7      1007   /* Designer Kerning segment not found          */
#define ld_wrn_8      1008   /* Typeface Header segment not found           */
#define ld_wrn_9      1009   /* Compound Character Data segment not found   */
#define ld_wrn_10     1010   /* Compound Character Metric Data seg not found*/
#define ld_wrn_11     1011   /* Compound Character Cg Numbers seg not found */
#define ld_wrn_12     1012   /* Complement Header segment not found         */
#define ld_wrn_13     1013   /* Fontalias Data segment not found            */
#define ld_wrn_14     1014   /* Global Intellifont Data segment not found   */
#define ld_wrn_15     1015   /* Missing character in Font file              */
