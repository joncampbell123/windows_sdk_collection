
// expandiv.js

if (oBD.getsNavBar) iNumStyleSheets = document.styleSheets.length;

if (oBD.getsNavBar && iNumStyleSheets > 0) 
{
document.styleSheets[iNumStyleSheets - 1].addRule(".clsExpandoBlurb","display:none; font-size:10pt; line-height:12pt;");
document.styleSheets[iNumStyleSheets - 1].addRule(".clsExpandoHeader","color:black; cursor:hand; font-size:10pt; font-weight:bold; line-height:12pt; margin-top:5px;");
document.styleSheets[iNumStyleSheets - 1].addRule(".clsExpandoToggle","cursor:hand;");
}

function HideAllBlurbs() 
{
for (var i=0; i<iNumDivs; i++) 
{
var eDiv = cDIV[i];
if ("clsExpandoBlurb" == eDiv.className) eDiv.style.display = "none";
}
}

function ShowAllBlurbs() 
{
for (var i=0; i<iNumDivs; i++) 
{
var eDiv = cDIV[i];
if ("clsExpandoBlurb" == eDiv.className) eDiv.style.display = "block";
}
}

function Expander_click() 
{
var eSrc = window.event.srcElement;
if ("idExpandoShowAll" == eSrc.id) ShowAllBlurbs();
else if ("idExpandoHideAll" == eSrc.id) HideAllBlurbs();   
else if ("clsExpandoHeader" == eSrc.className) 
{
var sID = "idExpandoBlurb" + eSrc.id.substring(eSrc.id.length-2);
if (eBlurb = document.all["idExpandoBlurb" + eSrc.id.substring(eSrc.id.length-2)])
{
eBlurb.style.display = ("block" == eBlurb.style.display ? "none" : "block");
}
}
window.event.cancelBubble = true;
}
  
function Expander_mouseover() 
{
    var eSrc = window.event.srcElement;
    if ("clsExpandoHeader" == eSrc.className || "clsExpandoToggle" == eSrc.className) eSrc.style.color= "#6666FF";
  }

  function Expander_mouseout() 
{
    var eSrc = window.event.srcElement;
    if ("clsExpandoHeader" == eSrc.className || "clsExpandoToggle" == eSrc.className) eSrc.style.color= "#000000";
  }

function ExpanDiv() 
{
window.cDIV = document.all.tags("DIV");
iNumDivs = cDIV.length;
document.onclick = Expander_click;
document.onmouseover = Expander_mouseover;
document.onmouseout = Expander_mouseout;
}
