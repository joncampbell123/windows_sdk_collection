Attribute VB_Name = "InsuraceModule"
Option Explicit
Global Const KEY_NAME = "Name"
Global Const KEY_STREET = "Street"
Global Const KEY_CITY = "City"
Global Const KEY_STATE = "State"
Global Const KEY_CLAIM = "Claim"
Global Const CLAIM_AUTO = "AUTO"
Global Const CLAIM_HOME = "HOME"
Global Const CLAIM_BOAT = "BOAT"
Global Const KEY_AMOUNT = "ClaimAmmount"

'---------------------------------------------------
'Stuff that could be auto genreated by a wizard
'---------------------------------------------------
Global Const INSURANCE_CLAIMS_QUEUE_NAME = "sampleInsuranceClaimsQueue3"
Global Const INSURANCE_CLAIMS_QUEUE_LOCATION = ".\sampleInsuranceClaimsQueue3"

Global Const INSURANCE_ACCOUNTING_QUEUE_NAME = "sampleInsuranceAccountingQueue3"
Global Const INSURANCE_ACCOUNTING_QUEUE_LOCATION = ".\sampleInsuranceAccountingQueue3"

'---------------------------------------------------
'Error messages
'---------------------------------------------------
Global Const ERROR_TEXT_RUNADMINTOOL = "The message queue could not be accessed.  Make sure you created the necessary queues using the 'Administer_Step0' project."
