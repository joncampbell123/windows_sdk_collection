;*****************************************************************************
;*
;*   File: RDLOG001.S - Script file to test the ReadErrorLog primitive.
;*
;*   Description: If ReadErrorLog is supported verify its operation under
;*                normal conditions.
;*		  Tests include:
;*		     - Check for OpenAdapter supported
;*		     - For Token Ring
;*			Check ReadErrorLog for SUCCESS
;*			  - If SUCCESS, the error log will be dumped to the
;*			    screen
;*			If ReadErrorLog does not return SUCCESS, check
;*			for NOT_SUPPORTED
;*		     - For Ethernet
;*			Check ReadErrorLog for NOT_SUPPORTED
;*			If ReadErrorLog does not return NOT_SUPPORTED, check
;*			for SUCCESS
;*			  - If SUCCESS, the error log will be dumped to the
;*			    screen
;*
;*   Date: 6/11/90
;*
;*   Pre-conditions: Adapter is closed.
;*
;*   Post-conditions: Adapter is closed.
;*
;*****************************************************************************

  -LOG 3 Starting ReadErrorLog Tests

  -LOG 3 If this MAC supports OpenAdapter exec. OA
*** Check the Service Specific Characteristics Table to see if the
*** OpenAdapter command is supported by this MAC.
  -CheckMacTable SC 41 m 08
  -JFAIL SkipOA

  -OpenAdapter,SUCCESS
  -JPASS SkipOA

   Quit script because state of MAC is uncertain.
  -LOG 3 OpenAdapter failed.  Exiting test.
  -JUMP abort

:SkipOA
  Check for Token Ring
  -CheckMacTable SC 2 e 38 30 32 2e 35
  -JFAIL EtherMC

  -ReadErrorLog,SUCCESS
  -JFAIL NotSupported

  -LOG 3 ReadErrorLog returned SUCCESS.
  -LOG 3 SQLKEY:RDLOG001.1 PASS
  -JUMP end

:NotSupported
;*** ReadErrorLog did not return SUCCESS.  Is it not supported?
  -ReadErrorLog,NOT_SUPPORTED
  -JFAIL NotSupportedFail

  -LOG 3 ReadErrorLog is NOT_SUPPORTED for this MAC.  Disreguard script error.
  -LOG 3 SQLKEY:RDLOG001.1 PASS
  -JUMP end

:NotSupportedFail
  -LOG 3 ReadErrorLog did not return SUCCESS or NOT_SUPPORTED
  -LOG 3 SQLKEY:RDLOG001.1 FAIL
  -JUMP end

:EtherMC
*** Check for rdlog NOT_SUPPORTED first
  -ReadErrorLog,NOT_SUPPORTED
  -JFAIL Rdlogtst

  -LOG 3 ReadErrorLog returned a Not Supported
  -LOG 3 SQLKEY:RDLOG001.2 PASS
  -JUMP end

:Rdlogtst
  -ReadErrorLog,SUCCESS
  -JFAIL RELogFail1

  -LOG 3 Rderrlog returned a success for Ethernet.  Disreguard script error.
  -LOG 3 SQLKEY:RDLOG001.2 PASS
  -JUMP end

:RELogFail1
  -LOG 3 ReadErrorLog did not return SUCCESS or NOT_SUPPORTED.
  -LOG 3 SQLKEY:RDLOG001.2 FAIL
  -JUMP end

:abort
  -LOG 3 Aborting RDLOG001 because state of MAC is uncertain.
  -Script ABORT ABORT
  -SetFailure
  -JUMP end2

:end
  -LOG 3 Finished ReadErrorLog Tests
  -CloseAdapter
  -Wait 2

:end2


