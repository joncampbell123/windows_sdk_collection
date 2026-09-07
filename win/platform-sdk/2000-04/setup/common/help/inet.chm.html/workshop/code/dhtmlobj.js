var g_aImages = new Array("unknown", "attribute", "method", "prop_ro", "prop_rw", "event", "collection", "object", "behavior");
var g_memberImages = new Array(g_aImages.length);
var g_sGR = "/workshop/graphics/"; 
var g_sGExt = ".gif";

var g_sFC = null; // optimize by caching current friendly column

var g_sObject = null;

//PreLoadMemberIcons();

/*
Insert object-page-specific data-bound tables into the page. IE4/Windows only  

vCtx - hash containing context information for the current binding. Valid keys include:
defView - default filter (acceptable values: all, methods, properties, events, attributes (default), or collections)
dataPath - location of the data set to which the DSOs are to bind
 - for legacy reasons, this parameter may also specify a string representing the default view
specifying default filter for members table.

sCSSLink - optional string specifying name of field to surround with hyperlink
           acceptable values: cssattr (default), prop
*/
function AddObjTables(vCtx, sCSSLink)
{
if (!oBD.getsNavBar) // so that down-level links show up in Mac and Unix
{
if (typeof(divMembers) == 'object')
{
divMembers.children(0).className = "clsExpanded";
}
if (typeof(divCSS) == 'object')
{
divCSS.children(0).className = "clsExpanded";
}
return false;
}

var vDefView = "";
var sDataPath = "../data/"; // using relative path allows this code to be used by ANY reference

// dig data out of the context object
if (typeof(vCtx) == 'object' && vCtx != null)
{
if (typeof(vCtx.defView) == 'string')
{
vDefView = vCtx.defView;
}

if (typeof(vCtx.dataPath) == 'string')
{
sDataPath = vCtx.dataPath;
}
}
else if (typeof(vCtx) == 'string')
{
vDefView = vCtx;
}

var oReg = new RegExp("[\\\\]", "g");
var sPath = location.pathname.replace(oReg, "/"); // Normalize the path on or off-line
aPath = sPath.split("/");
g_sObject = aPath[aPath.length-1].split(".")[0];

if (typeof(divMembers) == 'object')
{
var oPersistence = EnsurePersistence();
var sPM = oPersistence.getPersistedMembers();
if (sPM)
{
vDefView = sPM;
}

// coerce to a sensible default if nothing passed
if (!vDefView)
{
// if nothing explicit is passed, and it's a collection, show all; otherwise show attribs only 
vDefView = (IsCollection(document)) ? -1 : 0;
}
else if (typeof(vDefView) == "string")
{
vDefView = MapStringToView(vDefView);
}

cFilter = GetFilterExpr(vDefView);

g_sFC = GetFriendlyCol(vDefView);
var cImgCol = GetImgCol(vDefView);

sMembers = '<OBJECT classid="clsid:333C7BC4-460F-11D0-BC04-0080C7055A83" ' +
'ID=tdcMembers ondatasetcomplete="handle_members_dsc()" HEIGHT=0 WIDTH=0>' +
'<PARAM NAME="DataURL" VALUE="' + sDataPath + g_sObject + '_members.csv">' +
'<PARAM NAME="UseHeader" VALUE="True">' +
'<PARAM NAME="TextQualifier" VALUE="|">' +
'<PARAM NAME="CaseSensitive" VALUE="False">' +
'<PARAM NAME="Sort" VALUE="' + g_sFC + '">' +
(cFilter == '' ? '' : '<PARAM NAME="Filter" VALUE="' + cFilter + '">') +
'</OBJECT>';

document.body.insertAdjacentHTML('afterBegin', sMembers);

divMembers.innerHTML = "<TABLE CLASS='clsStd' STYLE='table-layout:fixed' ID=tblMembers DATASRC=#tdcMembers onreadystatechange='handle_members_rsc()'>" +
"<COL ID=colMembersSel WIDTH='20%'><COL WIDTH='20px'><COL WIDTH='*'>" +
"<THEAD><TR VALIGN=TOP BGCOLOR=#DDDDDD><TH>" + 
BuildViewCombo() +
"</TH><TH>&nbsp;</TH><TH>Description</TH></TR></THEAD>" +
"<TBODY><TR><TD NOWRAP><A DATAFLD=ref_link>" +

"<SPAN ID=ref_friendly onmouseover='handle_member_hover()' DATAFLD=" + g_sFC + "></SPAN></A></TD>" +

"<TD><IMG ID=ref_icon DATAFLD=" + cImgCol + "></TD>" +
"<TD><SPAN DATAFLD=ref_desc DATAFORMATAS=html></SPAN></TD></TR></TABLE>" +
"<DIV ID='divProposed' STYLE='display:none'>* denotes an extension to the W3C DOM.</DIV>";
if(cboInvokeKind.offsetWidth > colMembersSel.offsetWidth){
colMembersSel.width = cboInvokeKind.offsetWidth;
}
cboInvokeKind.selectedIndex = MapViewToOption(vDefView);
}

if (typeof(divCSS) == 'object') // CSS is optional if object doesn't support it.
{
AddCSSTable(divCSS, sDataPath + g_sObject + '_css.csv', sCSSLink);
SetExpandableCaption(pStyles, "Show Styles", true); // pull this call out of object pages
}

return true;
}

// deal with clipping of member cells
function handle_member_hover()
{
var oSpan = window.event.srcElement;
var oTD = oSpan.parentElement.parentElement;
if (IsClipped(oTD) && oSpan.title == "")
oSpan.title = oSpan.innerText;
}

function IsClipped(o)
{
return (o.scrollWidth > o.offsetWidth ? true : false);
}

// Add the data bound CSS table to the document
function AddCSSTable(oContainer, sPathToData, sLinkFld)
{
// wrap the CSS attribute name in a link unless prop is explicitly specified
// sort by the field surrounded by the link
if (!sLinkFld || sLinkFld != "cssattr")
{
sLinkFld = "prop";
}

var sCSS = '<OBJECT WIDTH=0 HEIGHT=0 id=tdcCSS CLASSID="clsid:333C7BC4-460F-11D0-BC04-0080C7055A83">' +
'<PARAM NAME="DataURL" VALUE="' + sPathToData + '">' +
'<PARAM NAME="UseHeader" VALUE="True">' +
'<PARAM NAME="TextQualifier" VALUE="|">' +
'<PARAM NAME="Sort" VALUE="' + sLinkFld + '">' +
'</OBJECT>'

document.body.insertAdjacentHTML('afterBegin', sCSS);

oContainer.innerHTML = "<TABLE CLASS='clsStd' STYLE='table-layout:fixed' DATASRC=#tdcCSS onreadystatechange='fixTable()'>" +
"<COL WIDTH='20%'><COL WIDTH='15%'><COL WIDTH='*'>" +
"<THEAD><TR VALIGN=TOP BGCOLOR=#DDDDDD><TH>Style property</TH><TH>Style attribute</TH><TH>Description</TH></TR>" +
"<TBODY><TR>" +
GenBoundCell('prop', null, null, (sLinkFld == "prop" ? "<A DATAFLD=propurl>" : null), (sLinkFld == "prop" ? "</A>" : null)) +
GenBoundCell('cssattr', null, null, (sLinkFld == "cssattr" ? "<A DATAFLD=propurl>" : null), (sLinkFld == "cssattr" ? "</A>" : null)) +
GenBoundCell('css_desc', null, 'html') +
"</TR>" +
"</TABLE>";

return true;
}


// Return true if the document represents a collection
function IsCollection(oDoc)
{
var oReg = new RegExp("Collection");
return (oDoc.title.match(oReg) ? true : false);
}


/*
Return a string representing a data bound table cell
cField - name of the field in the data set
cElem - name of element to bind
cFormat - format of data
cPre - prefix wrapper 
cPost - suffix wrapper
*/
function GenBoundCell(cField, cElem, cFormat, cPre, cPost)
{
if (!cElem)
{
cElem = 'SPAN';
}

if (!cFormat)
{
cFormat="text"
}

if (!cPre)
{
cPre = '';
}

if (!cPost)
{
cPost = '';
}

return '<TD>' + cPre + '<' + cElem + ' DATAFLD=' + cField + ' DATAFORMATAS=' + cFormat + '>' + '</' + cElem + '>' + cPost + '</TD>';
}

// Filter the members displayed supplied by a TDC
// oSelect - reference to a select
// oTDC - reference to a TDC
function FilterMembers(oSelect, oTDC)
{
var cFilter='';
var iValue = oSelect.options(oSelect.selectedIndex).value;

cFilter = GetFilterExpr(iValue);

if (RebindMTCols(tblMembers, GetFriendlyCol(iValue), GetImgCol(iValue)))
{
oTDC.object.Sort = g_sFC;
}

oTDC.object.Filter = cFilter;
oTDC.Reset();
var oPersistence = EnsurePersistence();
oPersistence.persistMembers(oSelect.options(oSelect.selectedIndex).text);

return true;
}

// Rebind the friendly column to the appropriate field in the data set but only if necessary
function RebindMTCols(oTable, sNewFCol, sNewImgCol)
{
// Don't bother rebinding to the same column
if (g_sFC == sNewFCol)
{
return false;
}

var sDSO = oTable.dataSrc;
oTable.dataSrc = '';
ref_friendly.dataFld = sNewFCol;
ref_icon.dataFld = sNewImgCol;
oTable.dataSrc = sDSO;

g_sFC = sNewFCol;
return true;
}

// Fired when a member image loads. Allows us to set the tooltip
// oImg - reference to an image object
// no need to specify a full vroot here.

var g_aMemberImg2Str = new Array();
g_aMemberImg2Str["method"] = "Method";
g_aMemberImg2Str["prop_ro"] = "Read-Only Property";
g_aMemberImg2Str["prop_rw"] = "Read/Write Property";
g_aMemberImg2Str["event"] = "Event";
g_aMemberImg2Str["collection"] = "Collection";
g_aMemberImg2Str["object"] = "Object";
g_aMemberImg2Str["attribute"] = "Attribute";
g_aMemberImg2Str["behavior"] = "Behavior";

g_oRegImgTitle = new RegExp("(method|prop_ro|prop_rw|event|collection|object|attribute|behavior)\.gif");
g_oRegProp = new RegExp("(prop_..|attribute)");

var g_fMembersDSC = false; // indicates the data set is complete (DSO)
var g_fMembersRSC = false; // indicates the table is complete (TABLE)

function handle_members_dsc()
{
g_fMembersDSC = true;
window.setTimeout("ModifyMemberRows()", 500);
}

function AddProposedAdorner(oTbl, oRow, oRS)
{
//BUGBUG: Remove this test when all data files are synchronized to include 'proposed' column
if (oRS.fields.count != 9) return false;

oRS.AbsolutePosition = oRow.recordNumber;
if (oRS.fields("proposed").value == 1)
{
oRow.cells(0).innerHTML += "*";
return 1;
}
return 0;
}

function ModifyMemberRows()
{
if (g_fMembersDSC && g_fMembersRSC)
{
var iProposed = 0;
for (var i = 1; i < tblMembers.rows.length; i++)
{
ModifyMemberRow(tblMembers.rows[i].cells(1).children(0), tdcMembers);
iProposed += AddProposedAdorner(tblMembers, tblMembers.rows[i], tdcMembers.recordset);
}
divProposed.style.display = (iProposed ? "inline" : "none");
}
else
{
window.setTimeout("ModifyMemberRows()", 500);
}
}

function handle_members_rsc()
{
// see if the table is complete
if (window.event.srcElement.readyState == 'complete')
{
g_fMembersRSC = true;
}
}

function ModifyMemberRow(oImg, oTDC)
{
var sImgPath = oImg.href.toLowerCase();

var aMatch = sImgPath.match(g_oRegImgTitle);

oImg.title = (null != aMatch ? g_aMemberImg2Str[aMatch[1]] : "Member");

if (g_oRegProp.test((null != aMatch ? aMatch[1] : sImgPath)))
{
ModPropAttr(tblMembers.rows[oImg.recordNumber].children(0).all(1), oTDC);
}

return true;
}

// Builds a string and sets the tooltip for the cell
// When All is selected, the property rather than the attribute is displayed
// When Attrib is selected, the attrib is displayed and the tooltip is arranged appropriately
// When property is selected, the property is displayed ...
// oPropCell - the cell to be tipped
function ModPropAttr(oPropCell, oTDC)
{
// added robustness and stack var to handle quirk on some machines
if (typeof(oTDC) != 'object')
{
return false;
}

var oRS = oTDC.recordset;
if (!oRS)
{
return false;
}

oRS.AbsolutePosition = oPropCell.recordNumber;

var iFilter = cboInvokeKind.options(cboInvokeKind.selectedIndex).value;

var sPropValue = null, sAttrValue = null;

with (oRS.fields)
{
sPropValue = item('ref_dynamic').value;
sAttrValue = item('ref_persistent').value;
if (sPropValue == 'null' || sAttrValue == 'null')
{
return false;
}
}

var sProp = sPropValue + ' property';
var sAttr = sAttrValue + ' attribute';
var sTip = 'The ' + (iFilter==0 ? sAttr : sProp) + ' corresponds to the ' + (iFilter==0 ? sProp : sAttr) + '.';
oPropCell.className = "clsPropattr";
oPropCell.title = sTip;

return true;
}
var g_FilterMap = new Array();
g_FilterMap[-1] = 'ref_dynamic <> null'; // show all dynamic member
g_FilterMap[2] = '((invoke_kind=2 | invoke_kind=4 | invoke_kind=6) & ref_dynamic <> null)';
g_FilterMap[4] = g_FilterMap[2];
g_FilterMap[6] = g_FilterMap[2];

//'(ref_friendly = * \/ *) | invoke_kind = 0'; // old attribute filter
g_FilterMap[0] = "invoke_kind=0 | ((invoke_kind=2 | invoke_kind=6) & ref_persistent <> null)"; // look for text pattern; requires a space followed by slash followed by space to match

// Map an invoke_kind to a filter expression for use by the TDC
// With the exception of -1 (show all) vValue corresponds to the invoke_kind field from the data set
function GetFilterExpr(iValue)
{
return (typeof(g_FilterMap[iValue]) == 'string' ? g_FilterMap[iValue] : '(invoke_kind=' + iValue + ')');
}

function GetFriendlyCol(iValue)
{
return (iValue == 0) ? 'ref_persistent' : 'ref_dynamic';
}

function GetImgCol(iValue)
{
return (iValue == 0) ? 'pers_icon' : 'dyn_icon';
}

var g_aMemberViewByName = new Array();
g_aMemberViewByName["all"] = -1;
g_aMemberViewByName["attributes"] = 0;
g_aMemberViewByName["properties"] = 6;
g_aMemberViewByName["methods"] = 1;
g_aMemberViewByName["events"] = 8;
g_aMemberViewByName["collections"] = 32;
g_aMemberViewByName["behaviors"] = 128;

// Map the string to its numeric equivalent
function MapStringToView(sView)
{
sView = sView.toLowerCase();

// if the string isn't a valid key, return an acceptable default
return (typeof(g_aMemberViewByName[sView]) == 'number' ? g_aMemberViewByName[sView] : -1);
}

var g_aMemberViewByNum = new Array();
g_aMemberViewByNum[-1] = 0;
g_aMemberViewByNum[0] = 1;
g_aMemberViewByNum[2] = 2;
g_aMemberViewByNum[4] = 2;
g_aMemberViewByNum[6] = 2;
g_aMemberViewByNum[1] = 3;
g_aMemberViewByNum[8] = 4;
g_aMemberViewByNum[32] = 5;
g_aMemberViewByNum[128] = 6;

// map the specified view to an index into a SELECT defined below
function MapViewToOption(vView)
{
return (typeof(g_aMemberViewByNum[vView]) == 'number' ? g_aMemberViewByNum[vView] : 0);
}

// return a string representing the SELECT to be injected into the data bound table
function BuildViewCombo()
{
return "<SELECT ID=cboInvokeKind onchange='FilterMembers(this, tdcMembers)'>" +
"<OPTION VALUE=-1>All" +
"<OPTION VALUE=0>Attributes" +
"<OPTION VALUE=6>Properties" +
"<OPTION VALUE=1>Methods" +
"<OPTION VALUE=8>Events" +
"<OPTION VALUE=32>Collections" +
"<OPTION VALUE=128>Behaviors" + 
"</SELECT>";
}

// Preload the icons used in the member table
function PreLoadMemberIcons()
{
// preload the member images
for (i = 0; i < g_aImages.length; i++)
{
var sImage = g_sGR + g_aImages[i] + g_sGExt;
g_memberImages[g_aImages[i]] = new Image();
g_memberImages[g_aImages[i]].src = sImage;
}

return true;
}

// BUGBUG: code assumes that run-time column is 0 and that this is the linked column
function fixNA()
{
var tblStyles = divCSS.children(0);
var iLen = tblStyles.rows.length;
for (var i = 1; i< iLen; i++)
{
var oRow = tblStyles.rows(i);
var oProp = oRow.children(0);
var sText = oProp.innerText;
if (sText == "N/A")
{
var sHREF = oProp.children(0).href;
var oAttr = oRow.children(1);
var sAttrText = oRow.children(1).innerText;
oAttr.innerHTML = "<A HREF='" + sHREF + "'>" + sAttrText + "</A>";
oProp.innerHTML = sText;
}
else
{
break;
}
}

return true;
}

// behavior attribute is only proposed, so append some text to that effect
function addProposed()
{
var tbl=divCSS.children(0);
for (i=0;i<tbl.rows.length;i++)
{
if (tbl.rows(i).cells(1).innerText=="behavior")
{
tbl.rows(i).cells(1).insertAdjacentText("beforeEnd"," (proposed)");
break;
}
}

return true;
}

// patch the style table
function fixTable()
{
var tbl=event.srcElement;
if (tbl.readyState=="complete")
{
window.setTimeout("addProposed()", 0);
window.setTimeout("fixNA()", 0);
}

return true;
}
