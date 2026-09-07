/*
routines added for IE5B2:
window_load - call showSelected from load handler (persistence)
document_keyup - kb handling when any key is released (accessibility)
HandleKeyForHeading - kb handling when section heading has focus (accessibility)
HandleKeyForItem - kb handling when article has focus (accessibility)
*/

// Accessibility key definitions
var DHTML_ARROW_MIN = 37;
var DHTML_ARROW_MAX = 40;
var DHTML_LEFT_ARROW = 37; // left-arrow
var DHTML_UP_ARROW = 38; // up-arrow
var DHTML_RIGHT_ARROW = 39;  // right-arrow
var DHTML_DOWN_ARROW = 40;   // down-arrow

//-------------------------------------------------------------------------
//Declare necessary variables
//eSelected: the currently selected item in the TOC
//sCurrentDoc: the path of the document currently displayed in the
//content frame. This string is passed as a hash value
//-------------------------------------------------------------------------

var eSelected = null;
//var sCurrentDoc = window.location.hash.substring(1) + location.search;
var sCurrentDoc = window.location.hash.substring(1);

//-------------------------------------------------------------------------
//getContainer()
//Purpose: finds the container element for a given child
//Called: from the viewElement function
//-------------------------------------------------------------------------

function getContainer(eSrc)
{
while ("BODY" != eSrc.tagName)
{
if ("clsShowHide" != eSrc.className) return eSrc;
eSrc = eSrc.parentElement;
}
}

//-------------------------------------------------------------------------
//newToc()
//Purpose: takes a path to a contents page as input and
//returns a properly-formed c-frame URL which loads the contents page
//w/ the appropriate TOC.
//Called: in the onclick event of any TOC item that should load a new TOC
//(signified by the expNewTOC expando property).
//-------------------------------------------------------------------------

function newToc()
{
var sTargetVRoot = "";
var sTargetPath = window.event.srcElement.pathname;
var sTopPath = top.location.pathname;

if (sTopPath.substring(1, sTopPath.indexOf("/")) != sTargetPath.substring(0, sTargetPath.indexOf("/")))
{
sTargetVRoot = "/" +  sTargetPath.substring(0, sTargetPath.indexOf("/") + 1);
}

return hackURL(sTargetVRoot + "c-frame.htm#/" + sTargetPath);
}

//-------------------------------------------------------------------------
//hackURL()
//Purpose: Inserts a unique search value into a c-frame URL. This value
//is obtained by applying the getTime method to the Date object.
//Called: from the newToc() function
//-------------------------------------------------------------------------

function hackURL(sURL)
{
dDate = new Date().getTime();
sFrameURL = sURL.substring(0,sURL.indexOf("#"));
sHash = sURL.substring(sURL.indexOf("#"));

if ("" != sFrameURL)
{
if (location.protocol.indexOf("file:") > -1) return sHash.substring(1);
return sFrameURL + "?" + dDate + sHash;
}
}

//-------------------------------------------------------------------------
//contentsHeading_click()
//
//Purpose:For TOC items w/ no children
//1) Navigates content frame
//2) Highlights item as selected
//3) Shows stylesheet samples in new window, if necessary
//
//For TOC items w/ children
//1) Shows/Hides children
//2) Swaps +/- list item icons
//
//Called: from the global onclick event handler. All items that are
//classed as either "clsShowHide" or "clsTOCHeading" will have this event
//handler defined.
//-------------------------------------------------------------------------

function contentsHeading_click(eSrc)
{
if ("noexpand" == eSrc.parentElement.className)
{
if (document.all["chkNewWin"])
{
if (document.all["chkNewWin"].checked) eSrc.target="_blank";
}

if ("" == eSrc.target)
{
eSrc.target = "TEXT";
}

if (null != eSelected)  eSelected.className = "clsTOCItem";
eSrc.className = "clsTOCItemSelect";
return eSelected = eSrc;
}

// offset from LI to section heading (A)
if ("clsShowHide" == eSrc.className) eSrc = document.all[eSrc.sourceIndex + 1];

var iNumElements = document.all.length;

for (var i = eSrc.sourceIndex + 1; i < iNumElements ; i++)
{
var eTemp = document.all[i];

if ("clsTOCHeading" == eTemp.className) return;
else if ("clsItemsHide" == eTemp.className)
{
eTemp.className = "clsItemsShow";
eSrc.style.listStyleImage = "url('/msdn-online/shared/graphics/minus.gif')";
//return eSrc.blur();
}
else if("clsItemsShow" == eTemp.className)
{
eTemp.className = "clsItemsHide";
eSrc.style.listStyleImage = "url('/msdn-online/shared/graphics/plus.gif')";
//return eSrc.blur();
}
}
}

function contentsItem_click(eSrc)
{
if (document.all["styleView"]) eSrc.target = document.all["styleView"].checked ? "_blank" : "TEXT";
else if ("" == eSrc.target) eSrc.target = "TEXT"
if(null != eSelected) eSelected.className = "clsTOCItem";
eSrc.className = "clsTOCItemSelect";
eSelected = eSrc;
}

function bucketName_click(eSrc)
{
eSrc.target = "TEXT";
if(null != eSelected) eSelected.className = "clsTOCItem";
}

//-------------------------------------------------------------------------
//GLOBAL EVENT HANDLERS
//-------------------------------------------------------------------------

function document_mouseover()
{
var eSrc = window.event.srcElement;
if ("clsTOCHeading" == eSrc.className && "noexpand" != eSrc.parentElement.className) eSrc.style.color = "red";
}

function document_mouseout()
{
var eSrc = window.event.srcElement;
if ("clsTOCHeading" == eSrc.className && "noexpand" != eSrc.parentElement.className) eSrc.style.color = "black";
}

function document_click()
{
var eSrc = window.event.srcElement;

window.event.cancelBubble = true;

while ("BODY" != eSrc.tagName)
{
if (null != eSrc.getAttribute("expResetToc")) return bucketName_click(eSrc);
else if ("clsTOCItem" == eSrc.className || "clsTOCItemSelect" == eSrc.className) return contentsItem_click(eSrc);
else if (null != eSrc.getAttribute("expNewToc"))
{
eSrc.target="_top";
eSrc.href = newToc();
return;
}
else if ("clsShowHide" == eSrc.className || "clsTOCHeading" == eSrc.className) return contentsHeading_click(eSrc);
else if("showAll" == eSrc.id) return eSrc.target = "TOC";
eSrc = eSrc.parentElement;
}
}

function showSelected()
{
if (null != eSelected)
{
if (eSelected.offsetTop > document.body.offsetHeight) getContainer(eSelected).scrollIntoView();
}
}

function window_load()
{
   showSelected();
}

// --------------------------------------------------------------
// Accessibility routines based on Windows Explorer functionality
// --------------------------------------------------------------

function document_keyup()
{
var iKey = window.event.keyCode;

// only trap arrow keys
if (iKey < DHTML_ARROW_MIN || iKey > DHTML_ARROW_MAX)
{
return;
}

// BUGBUG: IE4 returns BODY instead of element with the focus. Use event object instead
//var oActive = document.activeElement;
var oActive = window.event.srcElement;

// Handle up/down regardless of the type of selected anchor
// Handle right/left differently dependent on section / article level
if (DHTML_UP_ARROW == iKey || DHTML_DOWN_ARROW == iKey)
{
//HandleUpDown(iKey, oActive); // disabled for IE5B2 because it conflicts with scrollbar
}
else if ('clsTOCItem' == oActive.className || ('clsTOCHeading' == oActive.className && "noexpand" == oActive.parentElement.className))
{
// handle articles and headings that don't expand/collapse
HandleKeyForItem(oActive, iKey);
}
else if ('clsTOCHeading' == oActive.className)
{
// handle headings that expand/collapse
HandleKeyForHeading(oActive, iKey);
}

return;
}

// handle up and down arrow keys
function HandleUpDown(iKey, oActive)
{
var iIndex = oActive.sourceIndex;

// Handling of down and up arrow is identical regardless of what type of element (section / article) has focus
if (DHTML_UP_ARROW == iKey)
{
// handling is identical regardless of expand/collapse state of heading
// assert(document.links.length > 0);
while (iIndex >= document.links(0).sourceIndex)
{
var oTemp = document.all(--iIndex);
// walk backwards until the next visible anchor is found
if (oTemp.className == "clsTOCItem" || oTemp.className == "clsTOCHeading")
{
// make sure the element is part of a visible list
// only necessary when navigating from section to article
if (oTemp.parentElement.parentElement.className != "clsItemsHide")
{
oTemp.focus();
break;
}
}
}
}
else if (DHTML_DOWN_ARROW == iKey)
{
// Set focus to the next visible link or section
while (iIndex <= document.links(document.links.length-1).sourceIndex)
{
var oTemp = document.all(++iIndex);
if ((oTemp.className == "clsTOCHeading" || oTemp.className == "clsTOCItem") && oTemp.parentElement.parentElement.className != "clsItemsHide")
{
oTemp.focus();
break;
}
}
}
}


function HandleKeyForItem(oItem, iKey)
{
// left-arrow from a child node travels up to the parent node (not the sibling)
if (iKey == DHTML_LEFT_ARROW)
{
// if the user hits the left arrow on an item, set the focus to the immediate parent
// just like Windows Explorer
oItem.parentElement.parentElement.parentElement.children(0).focus();
}
}

// Handle keyboard action on a section
function HandleKeyForHeading(oActive, iKey)
{
// Need to add code to handle
// - up arrow: select previous item or section. Ensure that item is displayed
// - down array: select next item or section. Ensure that item is displayed. Otherwise choose next section

//window.event.cancelBubble = true;
var oParent = oActive.parentElement;

if ('clsItemsHide' == oParent.children(1).className)
{
if (DHTML_RIGHT_ARROW == iKey)
{
contentsHeading_click(oActive);
}
else if (DHTML_LEFT_ARROW == iKey)
{
// navigate to the previous parent section heading if any
if (oParent.parentElement.className != 'clsItemsShow')
{
return;
}

// A->LI->UL->LI->A
var oParentHeading = oParent.parentElement.parentElement.children(0);
oParentHeading.focus();
}
}
else if ('clsItemsShow' == oParent.children(1).className)
{
if (DHTML_LEFT_ARROW == iKey)
{
contentsHeading_click(oActive);
}
else if (DHTML_RIGHT_ARROW == iKey)
{
// would be much simplified if I could simulate a tab key...
// set focus to the next section or article
// A->LI->UL->LI->A
oParent.children(1).children(0).children(0).focus();
}
}
}

document.onmouseover = document_mouseover;
document.onmouseout = document_mouseout;
document.onclick = document_click;
document.onkeyup = document_keyup;
window.onload = window_load;

//---------------------------------------------------------
// PRE-LOAD GRAPHICS
//---------------------------------------------------------

var aImages = new Array(2);
var iImagesLength = aImages.length
for(var i=0;i<iImagesLength;i++) aImages[i] = new Image();

aImages[0].src = "/msdn-online/shared/graphics/minus.gif";
aImages[1].src = "/msdn-online/shared/graphics/arrowstop.gif";

//-------------------------------------------------------------------------
//"SYNC" UP TOC WITH DOCUMENT
//-------------------------------------------------------------------------

var cLinks = document.links;
var iNumLinks = cLinks.length;

for (var i=0;i<iNumLinks;i++)
{
var eSrc = cLinks[i];
var sTempHref = eSrc.href;

if(""  != sCurrentDoc && sTempHref.indexOf(sCurrentDoc) > -1 && "clsBucketName" != eSrc.className)
{
eSrc.className = "clsTOCItemSelect";
eSelected = eSrc;
if("noexpand" != eSelected.parentElement.className)
{
for (var a=eSrc.sourceIndex-1;a>0;a--)
{
var eTemp = document.all[a];
if ("clsItemsHide" == eTemp.className) eTemp.className = "clsItemsShow";
if ("clsTOCHeading" == eTemp.className)
{
eTemp.style.listStyleImage = "url('/msdn-online/shared/graphics/minus.gif')";
break;
}
}
}
  eSelected = eSrc;
  break;
}
}
