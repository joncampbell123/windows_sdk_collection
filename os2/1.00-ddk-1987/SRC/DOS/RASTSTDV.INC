;      SCCSID = @(#)raststdv.inc	5.1 87/03/06

;
; <<==++ Copyright (c) Microsoft Corporation  1986 ++==>>
;


;;
;;************************* START OF SPECIFICATIONS ******************/
;;								     */
;;     SUBROUTINE NAME:   TestDD				     */
;;								     */
;;     DESCRIPTIVE NAME:  Macro to test major event code	     */
;;								     */
;;     FUNCTION: For performance sensitive device drivers to include */
;;		 for querying the enable/disable status of the major */
;;		 event code for System Trace. The Major Event Code   */
;;		 array in the global InfoSeg  area is used to	     */
;;		 check the status of the event code.		     */
;;								     */
;;     NOTES:							     */
;;								     */
;;		  Should use ASSUME es:nothing before invocation     */
;;		  of MACRO and restore ES afterwards		     */
;;								     */
;;		  The Device Driver must use this Macro before	     */
;;		  calling the Trace Dev Help.  The Dev Help	     */
;;		  calls the Entry Point in the Trace which	     */
;;		  skip re-checking if Tracing is ON/OFF 	     */
;;								     */
;;								     */
;;     ENTRY POINT:  TestDD					     */
;;	  LINKAGE:  Macro with parameter N			     */
;;								     */
;;     INPUT: N = the caller's major event code (0 - 255)            */
;;	      ES = Segment/Selector to Global Info Segment	     */
;;								     */
;;	     REFERENCED - RAS System Trace Major Event Code Array    */
;;								     */
;;     EXIT-NORMAL: Zero Flag: 1 = event disabled or no System	     */
;;				     Tracing			     */
;;			       0 = event enabled		     */
;;								     */
;;     EXIT-ERROR:						     */
;;	     assembly error will result if 'N' parameter             */
;;	     is missing or out of range; an assembly error may result*/
;;	     if 'ASSUME' not set up correctly by user                */
;;								     */
;;     EFFECTS: THE STATUS FLAGS MAY BE ALTERED.		     */
;;								     */
;;     INTERNAL REFERENCES: NONE				     */
;;								     */
;;     EXTERNAL REFERENCES: NONE				     */
;;								     */
;;************************** END OF SPECIFICATIONS *******************/
;;


TraceDD MACRO mec
;;macro to test if specified major code is currently enabled - return
;;zero flag = zero if code is enabled or zero flag = 1 if disabled
;;or System Tracing doesn't exist.
  .ERRB <mec>			  ;;assembler error if mec blank
  .ERRNZ mec LT 0 OR mec GT 255   ;;assembler error if mec out of range
;test if specified major code is currently enabled - return in zero flag
  test es:[SIS_mec_table]+(mec SHR 3), 80h SHR (mec AND 7)
endm
