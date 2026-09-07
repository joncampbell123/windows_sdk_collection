//return the persistence wrapper
function EnsurePersistence()
{
if (typeof(EnsurePersistence.oPersistence) == 'object')
{
return EnsurePersistence.oPersistence;
}
else
{
EnsurePersistence.oPersistence = new objPersistence();
return EnsurePersistence.oPersistence;
}
}

//Constructor for the persistence object
function objPersistence()
{
this.bXMLDocLoadedFlag=false;//Indicates if the XML Document has been loaded yet.
this.bEnabled = false;//Indicates that the browser supports the persistent feature
this.sPersistedMember;//The current selection from of the members drop down
this.sStoreName="INETSDKStore";//The name of the persistent data store
this.sSectionPath="";//The path to the section objects in the XML doc
this.sMemberPath="";//The path to the members object in the XML doc
this.LoadXML=LoadXML;//Initialization routine for most of the object properties
this._oHostElem = document.body.children.tags('DIV')[0]; // element that hosts the userdata behavior
this.getObjectHierarchy=getObjectHierarchy;
this.getObject=getObject;
this.insertObject=insertObject;
this.removeObject=removeObject;
this.persistSection=persistSection;
this.persistHilite=persistHilite;
this.persistMembers=persistMembers;
this.getPersistedMembers=getPersistedMembers;
this.getPersistedSection=getPersistedSection;
}

//Persists the users choice of a show/hide section
function persistSection(oSection, sText, sShowHide)
{
if (this.bEnabled)
{
var oSectionNode = getObject(this.XMLDoc,this.sSectionPath + oSection.id);
var bSave = false;
if (sShowHide == "show" && oSectionNode == null)
{
oSectionNode=insertObject(this.XMLDoc,this.sSectionPath + oSection.id);
if (oSectionNode)
{
oSectionNode.setAttribute("sValue",sText);
bSave = true;
}
}
else if (sShowHide == "hide" && oSectionNode != null)
{
if (removeObject(this.XMLDoc,this.sSectionPath + oSection.id))
bSave = true;
}

if (bSave)
this._oHostElem.save(this.sStoreName);
}
}

//Persists what text should be highlited in example code
function persistHilite(oCode, sToken, oSample)
{
if (this.bEnabled)
{
var oSectionNode = getObject(this.XMLDoc,this.sSectionPath + oSample.id);
if (oSectionNode!=null)
{
oSectionNode.setAttribute("sToken",sToken);
oSectionNode.setAttribute("sCode",oCode.id);
this._oHostElem.save(this.sStoreName);
}
}
}

//Persists the user's selection from the members drop down
function persistMembers(sSelection)
{
// Persist the current filter
if (this.bEnabled)
{
var oMemberNode = getObject(this.XMLDoc,this.sMemberPath);
if (oMemberNode == null)
{
oMemberNode=insertObject(this.XMLDoc,this.sMemberPath);
}
if (oMemberNode != null)
{
oMemberNode.setAttribute("sValue",sSelection);
this._oHostElem.save(this.sStoreName);
this.sPersistedMember=sSelection
}
}
}

//Returns the user's last selection from the members drop down
function getPersistedMembers()
{
this.LoadXML();
if (this.bEnabled)
{
if (!this.sPersistedMember)
{
var oNode=getObject(this.XMLDoc,this.sMemberPath);
if (oNode != null)
{
this.sPersistedMember=oNode.getAttribute("sValue");
}
}
return this.sPersistedMember;
}
else
{
return null;
}
}

//Expands a section that has been stored.
function getPersistedSection(oElem, sCaption)
{
this.LoadXML();
if (this.bEnabled)
{
var oSectionNode = getObject(this.XMLDoc,this.sSectionPath + oElem.id);
if (oSectionNode != null)
{
sCaption=oSectionNode.getAttribute("sValue");
ShowHideSection(oElem,sCaption);
if (oSectionNode.getAttribute("sToken") && oSectionNode.getAttribute("sCode"))
{
HiliteText(document.all(oSectionNode.getAttribute("sCode")), oSectionNode.getAttribute("sToken"));
}
}
}
}

var gRegBS = new RegExp("[\\\\]", "g");
function JustFName(sPath)
{
sPath = sPath.replace(gRegBS, "/")
return sPath.substring(sPath.lastIndexOf("/")+1).split('.')[0]
}

//Provides TWO pieces of functionality.
//Loads the persistent XML document if needed.
//Determines if this is a post beta1 build.
function LoadXML()
{
if (!this.bXMLDocLoadedFlag)
{
this.bXMLDocLoadedFlag=true;//set flag regardless to prevent repeated initialization
this.bEnabled = (oBD.browser == "MSIE" && oBD.majorVer == 5);
if (this.bEnabled)
{
this._oHostElem.addBehavior("#default#userData");
var sLoad = "try { this._oHostElem.load(this.sStoreName); } catch(e) { this.bEnabled = false; }";
eval(sLoad); // protect <= JScript 3 from try/catch

if (!this.bEnabled)
return;

if (!this._oHostElem.XMLDocument)
{
this._oHostElem.save(this.sStoreName);
}
this.XMLDoc=this._oHostElem.XMLDocument;
var oRegBadXMLChars = new RegExp("[~]", "g"); // strip chars that are illegal for XML node names
var sFName = JustFName(document.location.pathname).replace(oRegBadXMLChars, "").toLowerCase();
this.sSectionPath= sFName + "/sections/";
this.sMemberPath= sFName + "/members";
}
}

return;
}

//Takes a path, converts to lowercase, strips leading slash and creates an array. 
//Assumes "/" character is the delimiter.
var gRegLS = new RegExp("^[/]")
function getObjectHierarchy(sPath)
{
sPath=sPath.replace(gRegLS, "");
return sPath.split("/");
}

//Returns the XML object at the position in the document indicated by sPath.
function getObject(docObject, sPath)
{
return docObject.documentElement.selectSingleNode(sPath);
}

//Inserts an object in the XML document indicated by sPath
function insertObject(docObject, sPath)
{
var aHierarchy = getObjectHierarchy(sPath);
var oCurNode=docObject.documentElement;
var iDepth = 0;
var bFound=false;
while (iDepth<aHierarchy.length)
{
bFound=false;
for (var i=0;i<oCurNode.childNodes.length;i++)
{
if (oCurNode.childNodes.item(i).nodeName==aHierarchy[iDepth])
{
oCurNode=oCurNode.childNodes.item(i);
bFound=true;
break;
}
}
if (!bFound)
{
var newNode=docObject.createElement(aHierarchy[iDepth])
oCurNode.insertBefore(newNode,null);
oCurNode=newNode;
}
iDepth++;
}
return oCurNode;
}

//Removes the object from the XML document indicated by sPath.
//If the removal of the object causes the parent to have no children then the parent is also removed.
//This process continues recursively up to the but not including the docuement root
function removeObject(docObject, sPath)
{
var oCurNode=getObject(docObject,sPath);
if (oCurNode==null)
return false;
var oParent=oCurNode.parentNode;
oParent.removeChild(oCurNode);
while (oParent.childNodes.length==0 && oParent != docObject.documentElement)
{
oCurNode=oParent;
oParent=oCurNode.parentNode;
oParent.removeChild(oCurNode);
}
return true;
}

// Hilites the text in the code sample 
// oStart - reference to start of block
// sText - string to find and hilite
function HiliteText(oStart, sText)
{
if (!oBD.getsNavBar)
{
// IE4/Mac doesn't support text ranges
return false;
}

var oRng = document.body.createTextRange();
oRng.moveToElementText(oStart);
var oRngFixed = oRng.duplicate();

if (typeof(HiliteText.tokens) == 'undefined')
{
HiliteText.tokens = new Array(1);
}
else
{
for (i = 0; i < HiliteText.tokens.length; i++)
{
if (HiliteText.tokens[i].m_sSectionID == oStart.id && HiliteText.tokens[i].m_sToken == sText)
{
return true;
}
}

HiliteText.tokens.length++;
}


while (oRng.findText(sText, 1000000, 6) && oRngFixed.inRange(oRng)) 
{    
oRng.execCommand('bold');
oRng.collapse(false);
}

HiliteText.tokens[HiliteText.tokens.length-1] = new CHilitedToken(oStart.id, sText);

return true;
}

// a tuple representing the id of the section and the token to be hilited
// the object is stored in an array to prevent the code from running twice on the same section
function CHilitedToken(sSectionID, sToken)
{
this.m_sSectionID = sSectionID;
this.m_sToken = sToken;
}


// Toggles the display of the content contained within oCode
// oCode - reference to code block
// sToken - string to bolden
function ToggleSample(oCode, sToken)
{
if (ShowHideSection(window.event.srcElement, 'Sample Code'))
{
HiliteText(oCode, sToken);
var oPersistence = EnsurePersistence();
oPersistence.persistHilite(oCode, sToken, window.event.srcElement);
}
}

// If hidden, show. If shown, hide. Modify the caption of the element appropriately
// Returns true if showing on return, false if hidden on return
function ShowHideSection(oHead, sText)
{
var bRet = false;
var oChild = document.all(oHead.getAttribute('child', false));

if (typeof(oChild) == null)
{
return bRet;
}

var sClass = oChild.className;
var sAction = "Show";
if (sClass == "clsCollapsed")
{
sAction = "Hide";
bRet = true; // we'll be showing upon return, so return true
var oPersistence = EnsurePersistence();
oPersistence.persistSection(oHead, sText, "show")
}
else
{
var oPersistence = EnsurePersistence();
oPersistence.persistSection(oHead, sText, "hide")
}
sAction = sAction + ' ' + sText;
oChild.className = (sClass == "clsCollapsed" ? "clsExpanded" : "clsCollapsed");
oHead.innerText = sAction;
return bRet;
}

// Set the caption of the specified element
// oElem - reference to element to modify. Typically a Hn
// sCaption - New caption for the element
// bShow - boolean indicating whether or not the element should be made visible
function SetExpandableCaption(oElem, sCaption, bShow)
{
oElem.innerText = sCaption;
if (bShow) oElem.style.display = 'inline';
var oPersistence = EnsurePersistence();
oPersistence.getPersistedSection(oElem, sCaption);
}

function CheckCAB(n)
{
return true;
}

// Perform universal document post-processing at load time.
function PostGBInit()
{
   if (oBD.getsNavBar)
   {
     if ("function" == typeof(CommonLoad)) CommonLoad();

      SetShowMes();

      if (typeof(oATTable) == 'object' && typeof(oATC) == 'object')
     fnATInit();
   }
}

// Walk the images collection and turn install icons into show me buttons
function SetShowMes()
{
var oImages = document.images;
var aContainers = new Array();

// collect references to DIVs that contain qualifying IMGs
for (i = oImages.length-1; i >= 0 ; i--)
{
with (oImages[i])
{
if ((!(iMinVer = getAttribute('MINVER', 1))) || 
(oBD.majorVer < parseFloat(iMinVer)) || 
(!getAttribute('SAMPLEPATH', 1)) ||
(parentElement.tagName != "A") || 
(src.lastIndexOf("ieget_animated.gif") == -1) ||
(parentElement.parentElement.tagName != "DIV") ||
(parentElement.parentElement.children(0).tagName != "DIV")
)
{
loop;
}
else
{
aContainers[aContainers.length] = parentElement.parentElement;
}
}
}

var sShowMeClass = "clsShowme";
if (oBD.majorVer >= 5)
{
sShowMeClass += "5";
}

// walk the containing DIVs
for (i = 0; i < aContainers.length; i++)
{
with (aContainers[i])
{
// gather data
var sToolTip, sClickCode;
with (children(1).children(0))
{
var sSamplePath = SAMPLEPATH;
sToolTip = (!getAttribute('SAMPLETEXT', 1) ? "Click here to see a demonstration of this technology." : SAMPLETEXT);

var oReg = new RegExp("^direct$", "i");
if (sSamplePath.match(oReg))
{
sClickCode = getAttribute('CODE', 1);
}
else
{
sClickCode = "window.open(\'" + sSamplePath + "\'" + (getAttribute('FEATURES', 1) ? ", null, \'" + FEATURES + "\'" : "") + ")";
}
}

// change the innerHTML of the containing DIV to a BUTTON
// BUGBUG: Removed CLASS="clsShowme" to SPAN per 22222
innerHTML = '<BUTTON CLASS="' + sShowMeClass + '" TITLE="' + sToolTip + '" onclick="' + sClickCode + '"><SPAN>Show Me</SPAN></BUTTON>';
}
}
}

// Function that adds the "view-source:" prefix to the specified vroot
// View-source syntax:
//    view-source:http://sitebuilder.microsoft.com/workshop/ul.sct
//
// Note that view-source only takes an absolute path, and thus the function below 
// does the munging of the protocol + host to the vroot, resulting in an absolute path.
// 
// Sample Usage:
//    <A HREF="javascript:HandleViewSource('/workshop/samples/components/scriptoid/calc/engine.sct')">engine.sct</A>
function HandleViewSource(sURL)
{
   location.href = "view-source:" + location.protocol + "//" + location.host + sURL;
}

// Generate an ID unique to the page
function GenerateID(sPrefix)
{
if ("MSIE" == oBD.browser && oBD.majorVer >= 5)
{
return (sPrefix ? sPrefix : '') + document.uniqueID;
}
else
{
for (iTry = 0; iTry < 3; iTry++)
{
var sUniqueID = (sPrefix ? sPrefix : '') + Math.round(Math.random()*100000000);
if (document.all(sUniqueID) == null) // verify that the ID is not already in use
{
return sUniqueID;
}
}
}
return null;
}

// Add client caps to the specified element on the page
// returns the element to which the behavior was added
// oElem - optional. The element to which the behavior was added (BODY is the default)
function EnableClientCaps(oElem)
{
if (!oBD.getsNavBar || oBD.majorVer < 5) // new for IE5
{
return null;
}

if (typeof(EnableClientCaps.oCaps) == 'object') // already cached? return it
{
return EnableClientCaps.oCaps;
}

if (typeof(oElem) != 'object') // validate params
{
oElem = document.body;
}

// addBehavior came in IE5 B2, so check a B2 property before making call
if (typeof(oElem.behaviorUrns) != 'object')
{
return null;
}

/* var iBehaviorID = */ oElem.addBehavior("#default#clientcaps");

if (typeof(oElem.platform) == 'string')
{
EnableClientCaps.oCaps = oElem;
return oElem;
}
else
{
EnableClientCaps.oCaps = null;
return null;
}
}
