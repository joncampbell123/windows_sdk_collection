
// Toggle Glossary Item Show/Hide

function toggleItem() {
var eSrc = window.event.srcElement;
var sSrcTag = eSrc.tagName.toUpperCase();
if (("A"==sSrcTag && "DIV"!=eSrc.parentElement.tagName.toUpperCase()) || "LI" == sSrcTag) {
var iTargetIndex = "A" == sSrcTag ? eSrc.sourceIndex+1 : eSrc.sourceIndex+4;
if (eDiv = document.all[iTargetIndex]) {
var sStyle = eDiv.className=="clsHide" ? "clsShow" : "clsHide";
"A" == sSrcTag ? eSrc.parentElement.parentElement.className = sStyle : eSrc.className = sStyle;
eDiv.className = sStyle;
}
return false;
}
}


// Toggle Glossary Show/Hide

function toggleGlossary() {
var eSrc = window.event.srcElement.parentElement;
var sDivStyle = eSrc.viewState=="show" ? "clsShow" : "clsHide";
if (cDivs = glossary.all.tags("DIV")) {
if (sDivStyle=="clsShow") {
eSrc.viewState = "hide"
eSrc.children[0].src = imgShowAll.src;
eSrc.children[1].innerHTML = "Hide All Definitions";
} else {
eSrc.viewState = "show";
eSrc.children[0].src = imgHideAll.src;
eSrc.children[1].innerHTML = "Show All Definitions"
} 
var iLength = cDivs.length;
for (var i=0; i<iLength; i++) {
var oItem = cDivs[i];
if (sDivStyle != oItem.className) {
oItem.className = sDivStyle;
if ("LI" == oItem.parentElement.tagName.toUpperCase()) oItem.parentElement.className = sDivStyle;
//alert (oItem.parentElement.className);
}
}
return false;
}
}


// Assign event handler for UL with ID glossary

function bindEvents() {
glossary.onclick = toggleItem;
idToggleAll.onclick = toggleGlossary;
}

var imgShowAll = new Image();
imgShowAll.src = "/msdn-online/start/images/showall-on.gif";
var imgHideAll = new Image();
imgHideAll.src = "/msdn-online/start/images/showall-off.gif";
