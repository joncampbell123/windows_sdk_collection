//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1999
//
//  File:       CertSrv.h
//  Contents:   Main Certificate Server header
//              Also includes .h files for the COM interfaces
//
//----------------------------------------------------------------------------

#if !defined( _CERTSRV_H_ )
#define _CERTSRV_H_

#include <certadm.h>
#include <certbcli.h>
#include <certcli.h>
#include <certenc.h>
#include <certexit.h>
#include <certif.h>
#include <certpol.h>
#include <certmod.h>
#include <certview.h>

#ifndef DBG_CERTSRV
# if defined(_DEBUG)
#  define DBG_CERTSRV     1
# elif defined(DBG)
#  define DBG_CERTSRV     DBG
# else
#  define DBG_CERTSRV     0
# endif
#endif

#define wszSERVICE_NAME		TEXT("CertSvc")

#define wszREGKEYNOSYSTEMCERTSVCPATH \
				TEXT("CurrentControlSet\\Services\\") \
				wszSERVICE_NAME

#define wszREGKEYCERTSVCPATH	TEXT("SYSTEM\\") wszREGKEYNOSYSTEMCERTSVCPATH
#define wszREGKEYBASE		wszREGKEYCERTSVCPATH	// obsolete definition

//======================================================================
// Full path to "CertSvc\Configuration\":
#define wszREGKEYCONFIGPATH	wszREGKEYCERTSVCPATH TEXT("\\") wszREGKEYCONFIG
#define wszREGKEYCONFIGPATH_BS	wszREGKEYCONFIGPATH TEXT("\\")
#define wszREGKEYCONFIGCANAME	wszREGKEYCONFIGPATH_BS	// obsolete definition

//======================================================================
// Full path to "CertSvc\Configuration\RestoreInProgress":
#define wszREGKEYCONFIGRESTORE wszREGKEYCONFIGPATH_BS wszREGKEYRESTOREINPROGRESS

//======================================================================
// Key Under "CertSvc":
#define wszREGKEYCONFIG		TEXT("Configuration")

//======================================================================
// Values Under "CertSvc\Configuration":
#define wszREGACTIVE		     TEXT("Active")
#define wszREGDIRECTORY		     TEXT("ConfigurationDirectory")
#define wszREGDBDIRECTORY            TEXT("DBDirectory")
#define wszREGDBLOGDIRECTORY         TEXT("DBLogDirectory")
#define wszREGDBSYSDIRECTORY         TEXT("DBSystemDirectory")
#define wszREGDBTEMPDIRECTORY        TEXT("DBTempDirectory")
#define wszREGDBSESSIONCOUNT	     TEXT("DBSessionCount")
#define wszREGWEBCLIENTCAMACHINE     TEXT("WebClientCAMachine")
#define wszREGWEBCLIENTCANAME        TEXT("WebClientCAName")
#define wszREGWEBCLIENTCATYPE        TEXT("WebClientCAType")

// Default value for wszREGDBSESSIONCOUNT
#define DBSESSIONCOUNTDEFAULT	     20

// Keys Under "CertSvc\Configuration":
#define wszREGKEYRESTOREINPROGRESS   TEXT("RestoreInProgress")

//======================================================================
// Values Under "CertSvc\Configuration\<CAName>":
#define wszREGCADESCRIPTION          TEXT("CADescription")
#define wszREGCACERTHASH	     TEXT("CACertHash")
#define wszREGCASERIALNUMBER	     TEXT("CACertSerialNumber")
#define wszREGCATYPE		     TEXT("CAType")
#define wszREGCERTENROLLCOMPATIBLE   TEXT("CertEnrollCompatible")
#define wszREGENFORCEX500NAMELENGTHS TEXT("EnforceX500NameLengths")
#define wszREGCOMMONNAME	     TEXT("CommonName")
#define wszREGCLOCKSKEWMINUTES	     TEXT("ClockSkewMinutes")
#define wszREGCRLNEXTPUBLISH         TEXT("CRLNextPublish")
#define wszREGCRLPERIOD		     TEXT("CRLPeriod")
#define wszREGCRLUNITS		     TEXT("CRLPeriodUnits")
#define wszREGCRLOVERLAPPERIOD   TEXT("CRLOverlapPeriod")
#define wszREGCRLOVERLAPUNITS    TEXT("CRLOverlapUnits")
#define wszREGCRLPATH                TEXT("CRLPath")
#define wszREGCRLEDITFLAGS	     TEXT("CRLEditFlags")
#define wszREGCRLATTEMPTREPUBLISH    TEXT("CRLAttemptRepublish")
#define wszREGENABLED		     TEXT("Enabled")
#define wszREGFORCETELETEX           TEXT("ForceTeletex")
#define wszREGLOGLEVEL		     TEXT("LogLevel")
#define wszREGPOLICYFLAGS	     TEXT("PolicyFlags")
#define wszREGNAMESEPARATOR          TEXT("SubjectNameSeparator")
#define wszREGSUBJECTTEMPLATE	     TEXT("SubjectTemplate")
#define wszREGCAUSEDS		     TEXT("UseDS")
#define wszREGDURATIONPERIOD	     TEXT("ValidityPeriod")
#define wszREGDURATIONUNITS	     TEXT("ValidityPeriodUnits")
#define wszREGPARENTCAMACHINE        TEXT("ParentCAMachine")
#define wszREGPARENTCANAME           TEXT("ParentCAName")
#define wszREGREQUESTFILENAME        TEXT("RequestFileName")
#define wszREGREQUESTID              TEXT("RequestId")
#define wszREGREQUESTKEYCONTAINER    TEXT("RequestKeyContainer")
#define wszREGREQUESTKEYINDEX        TEXT("RequestKeyIndex")
#define wszREGCASERVERNAME           TEXT("CAServerName")
#define wszREGCACERTFILENAME         TEXT("CACertFileName")
#define wszREGCASECURITY             TEXT("Security")

#define wszREGSETUPSTATUS            TEXT("SetupStatus")

#define wszPFXFILENAMEEXT	     TEXT(".p12")
#define wszDATFILENAMEEXT	     TEXT(".dat")
#define wszLOGFILENAMEEXT	     TEXT(".log")
#define wszPATFILENAMEEXT	     TEXT(".pat")
#define wszDBFILENAMEEXT	     TEXT(".edb")
#define szDBBASENAMEPARM	     "edb"
#define wszDBBASENAMEPARM	     TEXT(szDBBASENAMEPARM)
#define wszLOGPATH		     TEXT("CertLog")
#define wszDBBACKUPSUBDIR	     TEXT("DataBase")
#define wszDBBACKUPCERTBACKDAT	     TEXT("certback.dat")


#ifndef __ENUM_CATYPES__
#define __ENUM_CATYPES__

// Values for wszREGCATYPE:
typedef enum {
    ENUM_ENTERPRISE_ROOTCA = 0,
    ENUM_ENTERPRISE_SUBCA = 1,
    //ENUM_UNUSED2 = 2,
    ENUM_STANDALONE_ROOTCA = 3,
    ENUM_STANDALONE_SUBCA = 4,
    ENUM_UNKNOWN_CA = 5,
} ENUM_CATYPES;

#endif __ENUM_CATYPES__

// Default value for wszREGCLOCKSKEWMINUTES
#define CCLOCKSKEWMINUTESDEFAULT	10

// Values for wszREGLOGLEVEL:
#define CERTLOG_MINIMAL		(DWORD) 0
#define CERTLOG_TERSE		(DWORD) 1
#define CERTLOG_ERROR		(DWORD) 2
#define CERTLOG_WARNING		(DWORD) 3
#define CERTLOG_VERBOSE		(DWORD) 4


// Values for wszREGSETUPSTATUS:
#define SETUP_SERVER_FLAG		0x00000001	// server installed
#define SETUP_CLIENT_FLAG		0x00000002	// client installed
#define SETUP_SUSPEND_FLAG		0x00000004	// incomplete install
#define SETUP_REQUEST_FLAG		0x00000008	// new cert requested
#define SETUP_ONLINE_FLAG		0x00000010	// requested online
#define SETUP_DENIED_FLAG		0x00000020	// request denied
#define SETUP_CREATEDB_FLAG		0x00000040	// create new DB
#define SETUP_ATTEMPT_VROOT_CREATE	0x00000080	// try to create vroots
#define SETUP_FORCECRL_FLAG		0x00000100	// force new CRL(s)


//======================================================================
// Keys Under "CertSvc\Configuration\<CAName>":
#define wszREGKEYCSP			TEXT("CSP")
#define wszREGKEYEXITMODULES		TEXT("ExitModules")
#define wszREGKEYPOLICYMODULES	        TEXT("PolicyModules")
#define wszSECUREDATTRIBUTES		TEXT("SignedAttributes")

#define wszDEFAULTSIGNEDATTRIBUTES      TEXT("RequesterName\0") \
                                        TEXT("RequesterAddress\0") \
                                        TEXT("RequesterToken\0")

//======================================================================
// Values Under "CertSvc\Configuration\RestoreInProgress":
#define wszREGBACKUPLOGDIRECTORY	TEXT("BackupLogDirectory")
#define wszREGCHECKPOINTFILE		TEXT("CheckPointFile")
#define wszREGHIGHLOGNUMBER		TEXT("HighLogNumber")
#define wszREGLOWLOGNUMBER		TEXT("LowLogNumber")
#define wszREGLOGPATH			TEXT("LogPath")
#define wszREGRESTOREMAPCOUNT		TEXT("RestoreMapCount")
#define wszREGRESTOREMAP		TEXT("RestoreMap")
#define wszREGDATABASERECOVERED		TEXT("DatabaseRecovered")
#define wszREGRESTORESTATUS		TEXT("RestoreStatus")

// values under \Configuration\PolicyModules in nt5 beta 2
#define wszREGB2ICERTMANAGEMODULE   TEXT("ICertManageModule")
// values under \Configuration in nt4 sp4
#define wszREGSP4DEFAULTCONFIGURATION  TEXT("DefaultConfiguration")
// values under ca in nt4 sp4
#define wszREGSP4KEYSETNAME            TEXT("KeySetName")
#define wszREGSP4SUBJECTNAMESEPARATOR  TEXT("SubjectNameSeparator")
#define wszREGSP4NAMES                 TEXT("Names")
#define wszREGSP4QUERIES               TEXT("Queries")
// both nt4 sp4 and nt5 beta 2
#define wszREGNETSCAPECERTTYPE         TEXT("NetscapeCertType")
#define wszNETSCAPEREVOCATIONTYPE      TEXT("Netscape")


//======================================================================
// Values Under "CertSvc\Configuration\<CAName>\CSP":
#define wszREGPROVIDERTYPE    TEXT("ProviderType")
#define wszREGPROVIDER        TEXT("Provider")
#define wszHASHALGORITHM      TEXT("HashAlgorithm")
#define wszMACHINEKEYSET      TEXT("MachineKeyset")


//======================================================================
// Value strings for "CertSvc\Configuration\<CAName>\SubjectNameSeparator":
#define szNAMESEPARATORDEFAULT   "\n"
#define wszNAMESEPARATORDEFAULT   TEXT(szNAMESEPARATORDEFAULT)


//======================================================================
// Value strings for "CertSvc\Configuration\<CAName>\ValidityPeriod":
#define wszDURATIONUNITSYEARS	TEXT("Years")
#define wszDURATIONUNITSMONTHS	TEXT("Months")
#define wszDURATIONUNITSWEEKS	TEXT("Weeks")
#define wszDURATIONUNITSDAYS	TEXT("Days")
#define wszDURATIONUNITSHOURS	TEXT("Hours")
#define wszDURATIONUNITSMINUTES	TEXT("Minutes")
#define wszDURATIONUNITSSECONDS	TEXT("Seconds")

//======================================================================
// Values Under "CertSvc\Configuration\<CAName>\PolicyModules\<ProgId>":
#define wszREGISSUERCERTURLFLAGS    TEXT("IssuerCertURLFlags")
#define wszREGEDITFLAGS		    TEXT("EditFlags")
#define wszREGSUBJECTALTNAME	    TEXT("SubjectAltName")
#define wszREGSUBJECTALTNAME2	    TEXT("SubjectAltName2")
#define wszREGREQUESTDISPOSITION    TEXT("RequestDisposition")
#define wszREGCAPATHLENGTH	    TEXT("CAPathLength")
#define wszREGREVOCATIONTYPE	    TEXT("RevocationType")

#define wszREGLDAPREVOCATIONCRLURL  TEXT("LDAPRevocationCRLURL")
#define wszREGREVOCATIONCRLURL	    TEXT("RevocationCRLURL")
#define wszREGFTPREVOCATIONCRLURL   TEXT("FTPRevocationCRLURL")
#define wszREGFILEREVOCATIONCRLURL  TEXT("FileRevocationCRLURL")

#define wszREGREVOCATIONURL	    TEXT("RevocationURL")

#define wszREGLDAPISSUERCERTURL	    TEXT("LDAPIssuerCertURL")
#define wszREGISSUERCERTURL         TEXT("IssuerCertURL")
#define wszREGFTPISSUERCERTURL      TEXT("FTPIssuerCertURL")
#define wszREGFILEISSUERCERTURL     TEXT("FileIssuerCertURL")

#define wszREGENABLEREQUESTEXTENSIONLIST  TEXT("EnableRequestExtensionList")
#define wszREGDISABLEEXTENSIONLIST  TEXT("DisableExtensionList")

// wszREGCAPATHLENGTH Values:
#define CAPATHLENGTH_INFINITE		0xffffffff

// wszREGREQUESTDISPOSITION Values:
#define REQDISP_PENDING			0x00000000
#define REQDISP_ISSUE			0x00000001
#define REQDISP_DENY			0x00000002
#define REQDISP_USEREQUESTATTRIBUTE	0x00000003
#define REQDISP_MASK			0x000000ff
#define REQDISP_PENDINGFIRST		0x00000100
#define REQDISP_DEFAULT_STANDALONE	(REQDISP_PENDINGFIRST | REQDISP_ISSUE)
#define REQDISP_DEFAULT_ENTERPRISE	(REQDISP_ISSUE)

// wszREGREVOCATIONTYPE Values:
#define REVEXT_CDPLDAPURL		0x00000001
#define REVEXT_CDPHTTPURL		0x00000002
#define REVEXT_CDPFTPURL		0x00000004
#define REVEXT_CDPFILEURL		0x00000008
#define REVEXT_CDPURLMASK		0x000000ff
#define REVEXT_CDPENABLE		0x00000100
#define REVEXT_ASPENABLE		0x00000200
#define REVEXT_DEFAULT_NODS	(REVEXT_CDPENABLE | \
				 REVEXT_CDPLDAPURL | \
				 REVEXT_CDPHTTPURL | \
				 REVEXT_CDPFILEURL)

// Suppress FILE URLs if a DS is available, as LDAP access within the
// enterprise should suffice, and http: should work outside the enterprise.
// Certs with too many URLs don't always fit on smart cards.

#define REVEXT_DEFAULT_DS	(REVEXT_CDPENABLE | \
				 REVEXT_CDPLDAPURL | \
				 REVEXT_CDPHTTPURL)

// wszREGISSUERCERTURLFLAGS Values:
#define ISSCERT_LDAPURL			0x00000001
#define ISSCERT_HTTPURL			0x00000002
#define ISSCERT_FTPURL			0x00000004
#define ISSCERT_FILEURL			0x00000008
#define ISSCERT_URLMASK			0x000000ff
#define ISSCERT_ENABLE			0x00000100
#define ISSCERT_DEFAULT_NODS	(ISSCERT_ENABLE | \
				 ISSCERT_LDAPURL | \
				 ISSCERT_HTTPURL | \
				 ISSCERT_FILEURL)

// Suppress FILE URLs if a DS is available, as LDAP access within the
// enterprise should suffice, and http: should work outside the enterprise.
// Certs with too many URLs don't always fit on smart cards.

#define ISSCERT_DEFAULT_DS	(ISSCERT_ENABLE | \
				 ISSCERT_LDAPURL | \
				 ISSCERT_HTTPURL)

// wszREGEDITFLAGS Values:				   Defaults:
#define EDITF_ENABLEREQUESTEXTENSIONS	0x00000001	// neither
#define EDITF_REQUESTEXTENSIONLIST	0x00000002	// Standalone
#define EDITF_DISABLEEXTENSIONLIST	0x00000004	// both
#define EDITF_ADDOLDKEYUSAGE		0x00000008	// both
#define EDITF_ADDOLDCERTTYPE		0x00000010	// neither
#define EDITF_ATTRIBUTEENDDATE		0x00000020	// Standalone
#define EDITF_BASICCONSTRAINTSCRITICAL	0x00000040	// Standalone
#define EDITF_BASICCONSTRAINTSCA	0x00000080	// Standalone
#define EDITF_ENABLEAKIKEYID		0x00000100	// both
#define EDITF_ATTRIBUTECA		0x00000200	// Standalone
#define EDITF_IGNOREREQUESTERGROUP      0x00000400	// both
#define EDITF_ENABLEAKIISSUERNAME	0x00000800	// both
#define EDITF_ENABLEAKIISSUERSERIAL	0x00001000	// both
#define EDITF_ENABLEAKICRITICAL		0x00002000	// both

#define EDITF_DEFAULT_STANDALONE	(EDITF_REQUESTEXTENSIONLIST | \
					 EDITF_DISABLEEXTENSIONLIST | \
					 EDITF_ADDOLDKEYUSAGE | \
					 EDITF_ATTRIBUTEENDDATE | \
					 EDITF_BASICCONSTRAINTSCRITICAL | \
					 EDITF_BASICCONSTRAINTSCA | \
					 EDITF_ENABLEAKIKEYID | \
					 EDITF_ATTRIBUTECA | \
					 EDITF_ENABLEAKIISSUERNAME | \
					 EDITF_ENABLEAKIISSUERSERIAL)

#define EDITF_DEFAULT_ENTERPRISE	(EDITF_REQUESTEXTENSIONLIST | \
					 EDITF_DISABLEEXTENSIONLIST | \
                                         EDITF_BASICCONSTRAINTSCRITICAL | \
                                         EDITF_ENABLEAKIKEYID | \
					 EDITF_ADDOLDKEYUSAGE | \
					 EDITF_ENABLEAKIISSUERNAME | \
					 EDITF_ENABLEAKIISSUERSERIAL)


//======================================================================
// Values Under "CertSvc\Configuration\<CAName>\ExitModules\<ProgId>":

// LDAP based CRL and URL issuance
#define wszREGLDAPREVOCATIONDN	   TEXT("LDAPRevocationDN")
#define wszREGLDAPREVOCATIONDNTEMPLATE	   TEXT("LDAPRevocationDNTemplate")
#define wszCRLPUBLISHRETRYCOUNT    TEXT("CRLPublishRetryCount")
#define wszREGCERTPUBLISHFLAGS     TEXT("PublishCertFlags")

// wszREGCERTPUBLISHFLAGS Values:
#define EXITPUB_FILE			0x00000001
#define EXITPUB_ACTIVEDIRECTORY		0x00000002
#define EXITPUB_EMAILNOTIFYALL		0x00000004
#define EXITPUB_EMAILNOTIFYSMARTCARD	0x00000008
#define EXITPUB_REMOVEOLDCERTS   	0x00000010

#define EXITPUB_DEFAULT_ENTERPRISE	EXITPUB_ACTIVEDIRECTORY

#define EXITPUB_DEFAULT_STANDALONE	EXITPUB_FILE


#define wszCLASS_CERTADMIN	  TEXT("CertificateAuthority.Admin")
#define wszCLASS_CERTCONFIG	  TEXT("CertificateAuthority.Config")
#define wszCLASS_CERTGETCONFIG	  TEXT("CertificateAuthority.GetConfig")
#define wszCLASS_CERTENCODE	  TEXT("CertificateAuthority.Encode")
#define wszCLASS_CERTREQUEST	  TEXT("CertificateAuthority.Request")
#define wszCLASS_CERTSERVEREXIT   TEXT("CertificateAuthority.ServerExit")
#define wszCLASS_CERTSERVERPOLICY TEXT("CertificateAuthority.ServerPolicy")
#define wszCLASS_CERTVIEW	  TEXT("CertificateAuthority.View")

// class name templates
#define wszMICROSOFTCERTMODULE_PREFIX  TEXT("CertificateAuthority_MicrosoftDefault") 
#define wszCERTEXITMODULE_POSTFIX	TEXT(".Exit")
#define wszCERTMANAGEEXIT_POSTFIX	TEXT(".ExitManage")
#define wszCERTPOLICYMODULE_POSTFIX	TEXT(".Policy")
#define wszCERTMANAGEPOLICY_POSTFIX	TEXT(".PolicyManage")

// actual policy/exit manage class names
#define wszCLASS_CERTMANAGEEXITMODULE   wszMICROSOFTCERTMODULE_PREFIX wszCERTMANAGEEXIT_POSTFIX 

#define wszCLASS_CERTMANAGEPOLICYMODULE wszMICROSOFTCERTMODULE_PREFIX wszCERTMANAGEPOLICY_POSTFIX 

// actual policy/exit class names
#define wszCLASS_CERTEXIT	wszMICROSOFTCERTMODULE_PREFIX wszCERTEXITMODULE_POSTFIX

#define wszCLASS_CERTPOLICY	wszMICROSOFTCERTMODULE_PREFIX wszCERTPOLICYMODULE_POSTFIX



//+--------------------------------------------------------------------------
// Name properties:

#define wszPROPDISTINGUISHEDNAME   TEXT("DistinguishedName")
#define wszPROPRAWNAME             TEXT("RawName")
#define wszPROPNAMETYPE            TEXT("NameType")

#define wszPROPCOUNTRY             TEXT("Country")
#define wszPROPORGANIZATION        TEXT("Organization")
#define wszPROPORGUNIT             TEXT("OrgUnit")
#define wszPROPCOMMONNAME          TEXT("CommonName")
#define wszPROPLOCALITY            TEXT("Locality")
#define wszPROPSTATE               TEXT("State")
#define wszPROPTITLE               TEXT("Title")
#define wszPROPGIVENNAME           TEXT("GivenName")
#define wszPROPINITIALS            TEXT("Initials")
#define wszPROPSURNAME             TEXT("SurName")
#define wszPROPDOMAINCOMPONENT     TEXT("DomainComponent")
#define wszPROPEMAIL               TEXT("EMail")
#define wszPROPSTREETADDRESS       TEXT("StreetAddress")
#define wszPROPUNSTRUCTUREDNAME    TEXT("UnstructuredName")
#define wszPROPUNSTRUCTUREDADDRESS TEXT("UnstructuredAddress")
#define wszPROPDEVICESERIALNUMBER  TEXT("DeviceSerialNumber")

//+--------------------------------------------------------------------------
// Subject Name properties:

#define wszPROPSUBJECTDOT	    TEXT("Subject.")
#define wszPROPSUBJECTDISTINGUISHEDNAME \
				    wszPROPSUBJECTDOT wszPROPDISTINGUISHEDNAME
#define wszPROPSUBJECTRAWNAME       wszPROPSUBJECTDOT wszPROPRAWNAME
#define wszPROPSUBJECTNAMETYPE      wszPROPSUBJECTDOT wszPROPNAMETYPE

#define wszPROPSUBJECTCOUNTRY       wszPROPSUBJECTDOT wszPROPCOUNTRY
#define wszPROPSUBJECTORGANIZATION  wszPROPSUBJECTDOT wszPROPORGANIZATION
#define wszPROPSUBJECTORGUNIT       wszPROPSUBJECTDOT wszPROPORGUNIT
#define wszPROPSUBJECTCOMMONNAME    wszPROPSUBJECTDOT wszPROPCOMMONNAME
#define wszPROPSUBJECTLOCALITY      wszPROPSUBJECTDOT wszPROPLOCALITY
#define wszPROPSUBJECTSTATE         wszPROPSUBJECTDOT wszPROPSTATE
#define wszPROPSUBJECTTITLE	    wszPROPSUBJECTDOT wszPROPTITLE
#define wszPROPSUBJECTGIVENNAME	    wszPROPSUBJECTDOT wszPROPGIVENNAME
#define wszPROPSUBJECTINITIALS	    wszPROPSUBJECTDOT wszPROPINITIALS
#define wszPROPSUBJECTSURNAME	    wszPROPSUBJECTDOT wszPROPSURNAME
#define wszPROPSUBJECTDOMAINCOMPONENT wszPROPSUBJECTDOT wszPROPDOMAINCOMPONENT
#define wszPROPSUBJECTEMAIL	    wszPROPSUBJECTDOT wszPROPEMAIL
#define wszPROPSUBJECTSTREETADDRESS wszPROPSUBJECTDOT wszPROPSTREETADDRESS
#define wszPROPSUBJECTUNSTRUCTUREDNAME wszPROPSUBJECTDOT wszPROPUNSTRUCTUREDNAME
#define wszPROPSUBJECTUNSTRUCTUREDADDRESS wszPROPSUBJECTDOT wszPROPUNSTRUCTUREDADDRESS
#define wszPROPSUBJECTDEVICESERIALNUMBER wszPROPSUBJECTDOT wszPROPDEVICESERIALNUMBER

#define wszPROPSUBJECTCOUNTRYOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_COUNTRY_NAME)

#define wszPROPSUBJECTORGANIZATIONOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_ORGANIZATION_NAME)

#define wszPROPSUBJECTORGUNITOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_ORGANIZATIONAL_UNIT_NAME)

#define wszPROPSUBJECTCOMMONNAMEOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_COMMON_NAME)

#define wszPROPSUBJECTLOCALITYOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_LOCALITY_NAME)

#define wszPROPSUBJECTSTATEOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_STATE_OR_PROVINCE_NAME)

#define wszPROPSUBJECTTITLEOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_TITLE)

#define wszPROPSUBJECTGIVENNAMEOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_GIVEN_NAME)

#define wszPROPSUBJECTINITIALSOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_INITIALS)

#define wszPROPSUBJECTSURNAMEOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_SUR_NAME)

#define wszPROPSUBJECTDOMAINCOMPONENTOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_DOMAIN_COMPONENT)

#define wszPROPSUBJECTEMAILOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_RSA_emailAddr)

#define wszPROPSUBJECTSTREETADDRESSOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_STREET_ADDRESS)

#define wszPROPSUBJECTUNSTRUCTUREDNAMEOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_RSA_unstructName)

#define wszPROPSUBJECTUNSTRUCTUREDADDRESSOBJID \
    wszPROPSUBJECTDOT TEXT(szOID_RSA_unstructAddr)

#define wszPROPSUBJECTDEVICESERIALNUMBEROBJID \
    wszPROPSUBJECTDOT TEXT(szOID_DEVICE_SERIAL_NUMBER)



//+--------------------------------------------------------------------------
// Request properties:
#define wszPROPREQUESTDOT	            TEXT("Request.")

#define wszPROPREQUESTREQUESTID		    TEXT("RequestID")
#define wszPROPREQUESTRAWREQUEST	    TEXT("RawRequest")
#define wszPROPREQUESTRAWOLDCERTIFICATE	    TEXT("RawOldCertificate")
#define wszPROPREQUESTATTRIBUTES	    TEXT("RequestAttributes")
#define wszPROPREQUESTTYPE		    TEXT("RequestType")
#define wszPROPREQUESTFLAGS		    TEXT("RequestFlags")
#define wszPROPREQUESTSTATUSCODE	    TEXT("StatusCode")
#define wszPROPREQUESTDISPOSITION	    TEXT("Disposition")
#define wszPROPREQUESTDISPOSITIONMESSAGE    TEXT("DispositionMessage")
#define wszPROPREQUESTSUBMITTEDWHEN	    TEXT("SubmittedWhen")
#define wszPROPREQUESTRESOLVEDWHEN	    TEXT("ResolvedWhen")
#define wszPROPREQUESTREVOKEDWHEN	    TEXT("RevokedWhen")
#define wszPROPREQUESTREVOKEDEFFECTIVEWHEN  TEXT("RevokedEffectiveWhen")
#define wszPROPREQUESTREVOKEDREASON	    TEXT("RevokedReason")
#define wszPROPREQUESTERNAME		    TEXT("RequesterName")

//+--------------------------------------------------------------------------
// Request attribute properties:

#define wszPROPCHALLENGE		TEXT("Challenge")
#define wszPROPEXPECTEDCHALLENGE	TEXT("ExpectedChallenge")

#define wszPROPDISPOSITION		TEXT("Disposition")
#define wszPROPDISPOSITIONDENY		TEXT("Deny")
#define wszPROPDISPOSITIONPENDING	TEXT("Pending")

#define wszPROPVALIDITYPERIOD		TEXT("ValidityPeriod")
#define wszPROPVALIDITYPERIODUNITS	TEXT("ValidityPeriodUnits")

#define wszPROPCERTTYPE			TEXT("CertType")
#define wszPROPCERTTEMPLATE		TEXT("CertificateTemplate")

#define wszPROPREQUESTOSVERSION		TEXT("RequestOSVersion")
#define wszPROPREQUESTCSPPROVIDER       TEXT("RequestCSPProvider")

//+--------------------------------------------------------------------------
// Hardcoded properties
// ".#" means ".0", ".1", ".2" ... may be appended to the property name to
// collect context specific values.  For some properties, the suffix selects
// the CA certificate context.  For others, it selects the the CA CRL context.

#define wszPROPCATYPE                   TEXT("CAType")
#define wszPROPSANITIZEDCANAME          TEXT("SanitizedCAName")
#define wszPROPSANITIZEDSHORTNAME       TEXT("SanitizedShortName")
#define wszPROPMACHINEDNSNAME           TEXT("MachineDNSName")
#define wszPROPMODULEREGLOC             TEXT("ModuleRegistryLocation")
#define wszPROPREQUESTERCAACCESS	TEXT("RequesterCAAccess")
#define wszPROPUSEDS                    TEXT("fUseDS")

// CA Certificate properties: (all ".#" extensible except wszPROPCERTCOUNT)

#define wszPROPCERTCOUNT                TEXT("CertCount")
#define wszPROPRAWCACERTIFICATE         TEXT("RawCACertificate")
#define wszPROPCERTSTATE                TEXT("CertState")
#define wszPROPCERTSUFFIX               TEXT("CertSuffix")

// CA CRL properties: (all ".#" extensible)

#define wszPROPRAWCRL                   TEXT("RawCRL")
#define wszPROPCRLINDEX                 TEXT("CRLIndex")
#define wszPROPCRLSTATE                 TEXT("CRLState")
#define wszPROPCRLSUFFIX                TEXT("CRLSuffix")

// Values for wszPROPCERTSTATE (see certadm.h):
//   CA_DISP_REVOKED    // This Cert has been revoked.
//   CA_DISP_VALID      // This Cert is still valid
//   CA_DISP_INVALID    // This Cert has expired.
//   CA_DISP_ERROR      // Never returned.

// Values for wszPROPCRLSTATE (see certadm.h):
//   CA_DISP_REVOKED	// All unexpired certs using this Cert's CRL have been
//			// revoked.
//   CA_DISP_VALID	// This Cert is still publishing CRLs as needed.
//   CA_DISP_INVALID    // All certs using this Cert's CRL are expired.
//   CA_DISP_ERROR      // This Cert's CRL is managed by another Cert.

//+--------------------------------------------------------------------------
// Certificate properties:

#define wszPROPCERTIFICATEREQUESTID	    TEXT("RequestID")
#define wszPROPRAWCERTIFICATE		    TEXT("RawCertificate")
#define wszPROPCERTIFICATEHASH		    TEXT("CertificateHash")
#define wszPROPCERTIFICATETYPE		    TEXT("CertificateType")
#define wszPROPCERTIFICATESERIALNUMBER	    TEXT("SerialNumber")
#define wszPROPCERTIFICATENOTBEFOREDATE	    TEXT("NotBefore")
#define wszPROPCERTIFICATENOTAFTERDATE	    TEXT("NotAfter")
#define wszPROPCERTIFICATERAWPUBLICKEY	    TEXT("RawPublicKey")
#define wszPROPCERTIFICATEPUBLICKEYALGORITHM TEXT("PublicKeyAlgorithm")
#define wszPROPCERTIFICATERAWPUBLICKEYALGORITHMPARAMETERS \
    TEXT("RawPublicKeyAlgorithmParameters")

#define wszPROPCERTIFICATERAWSMIMECAPABILITIES TEXT("RawSMIMECapabilities")

//+--------------------------------------------------------------------------
// Certificate extension properties:

#define EXTENSION_CRITICAL_FLAG	      0x00000001
#define EXTENSION_DISABLE_FLAG	      0x00000002
#define EXTENSION_POLICY_MASK	      0x0000ffff // Settable by admin+policy

#define EXTENSION_ORIGIN_REQUEST      0x00010000
#define EXTENSION_ORIGIN_POLICY	      0x00020000
#define EXTENSION_ORIGIN_ADMIN	      0x00030000
#define EXTENSION_ORIGIN_SERVER	      0x00040000
#define EXTENSION_ORIGIN_RENEWALCERT  0x00050000
#define EXTENSION_ORIGIN_IMPORTEDCERT 0x00060000
#define EXTENSION_ORIGIN_PKCS7	      0x00070000
#define EXTENSION_ORIGIN_MASK	      0x000f0000

//+--------------------------------------------------------------------------
// GetProperty/SetProperty Flags:
//
// Choose one Type

#define PROPTYPE_LONG		 0x00000001	// Signed long
#define PROPTYPE_DATE		 0x00000002	// Date+Time
#define PROPTYPE_BINARY		 0x00000003	// Binary data
#define PROPTYPE_STRING		 0x00000004	// Unicode String
#define PROPTYPE_MASK		 0x000000ff

// Choose one Caller:

#define PROPCALLER_SERVER	 0x00000100
#define PROPCALLER_POLICY	 0x00000200
#define PROPCALLER_EXIT		 0x00000300
#define PROPCALLER_ADMIN	 0x00000400
#define PROPCALLER_REQUEST	 0x00000500
#define PROPCALLER_MASK		 0x00000f00

// RequestFlags definitions:

#define CR_FLG_FORCETELETEX	0x00000001
#define CR_FLG_RENEWAL		0x00000002
#define CR_FLG_FORCEUTF8	0x00000004

// Disposition property values:

// Disposition values for requests in the queue:
#define DB_DISP_ACTIVE	        8	// being processed
#define DB_DISP_PENDING		9	// taken under submission
#define DB_DISP_QUEUE_MAX	9	// max disposition value for queue view

#define DB_DISP_CA_CERT		15	// CA cert
#define DB_DISP_CA_CERT_CHAIN	16	// CA cert chain

// Disposition values for requests in the log:
#define DB_DISP_LOG_MIN		20	// min disposition value for log view
#define DB_DISP_ISSUED		20	// cert issued
#define DB_DISP_REVOKED	        21	// issued and revoked

// Disposition values for failed requests in the log:
#define DB_DISP_LOG_FAILED_MIN	30	// min disposition value for log view
#define DB_DISP_ERROR		30	// request failed
#define DB_DISP_DENIED		31	// request denied


// VerifyRequest() return values

#define VR_PENDING	0	 // request will be accepted or denied later
#define VR_INSTANT_OK	1	 // request was accepted
#define VR_INSTANT_BAD	2	 // request was rejected


//+--------------------------------------------------------------------------
// Known request Attribute names and Value strings

// RequestType attribute name:
#define wszCERT_TYPE		L"RequestType"	// attribute name

// RequestType attribute values:
// Not specified: 				// Non-specific certificate
#define wszCERT_TYPE_CLIENT	L"Client"	// Client authentication cert
#define wszCERT_TYPE_SERVER	L"Server"	// Server authentication cert
#define wszCERT_TYPE_CODESIGN	L"CodeSign"	// Code signing certificate
#define wszCERT_TYPE_CUSTOMER	L"SetCustomer"	// SET Customer certificate
#define wszCERT_TYPE_MERCHANT	L"SetMerchant"	// SET Merchant certificate
#define wszCERT_TYPE_PAYMENT	L"SetPayment"	// SET Payment certificate


// Version attribute name:
#define wszCERT_VERSION		L"Version"	// attribute name

// Version attribute values:
// Not specified: 				// Whetever is current
#define wszCERT_VERSION_1	L"1"		// Version one certificate
#define wszCERT_VERSION_2	L"2"		// Version two certificate
#define wszCERT_VERSION_3	L"3"		// Version three certificate

#endif // _CERTSRV_H_
