// Generate an ID unique to the page
function GenerateID(sPrefix)
{
for (iTry = 0; iTry < 3; iTry++)
{
var sUniqueID = sPrefix + Math.round(Math.random()*100000000);
if (document.all(sUniqueID) == null) // verify that the ID is not already in use
{
return sUniqueID;
}
}

return null;
}

// Add a vertical scrollbar to the page
// oTarget - existing widget to the right of which the vscroll bar should appear
// iWidth - width of the vscroll bar
// sUserCode - user-defined callback
function AddVSB(oTarget, iWidth, sColor, sUserCode)
{
// sUserCode must represent a function for callbacks from the scrollbar
if (typeof(eval(sUserCode)) != 'function')
{
return false;
}

iWidth = ( (parseInt(iWidth) >= 10) ? iWidth : 10); // specify a width for the vscroll bar
sColor = (typeof(sColor) == 'string' ? sColor : 'red'); // specify a color for the vscroll bar

// create a unique ID for the bar
var sVSID = null;
if (!(sVSID = GenerateID('VS')))
{
return false;
}

// add the widget to the page
sNewObj = '<DIV ID=' + sVSID + ' STYLE="position:absolute;width:200;top:0;left:0;height:0;width:10;background-color:' + sColor + '"></DIV>';
document.body.insertAdjacentHTML('beforeEnd', sNewObj);

// position the widget, and add features
var oVSB = document.all(sVSID);

// explicitly set the height of the host. 
// This prevents auto-sizing and eliminates the need for the scrollbar to resize itself
oTarget.style.height = oTarget.offsetHeight;

with (oVSB.style)
{
posTop = oTarget.offsetTop;
posLeft = oTarget.offsetLeft + oTarget.offsetWidth;
posHeight = oTarget.offsetHeight;
posWidth = iWidth;

// add the scroll up button
// BUGBUG: Should remove the objects you create in the event of failure...
if (!AddVScrollButton(oVSB, posTop, posLeft, posWidth, posWidth, 'HandleVScrollClick(this)', -1, sUserCode))
{
return false;
}

// add the scroll down button
if (!AddVScrollButton(oVSB, posTop+posHeight-posWidth, posLeft, posWidth, posWidth, 'HandleVScrollClick(this)', 1, sUserCode))
{
return false;
}
}

//divBefore.innerText = '(' + oTarget.offsetTop + ', ' + oTarget.offsetLeft + ', ' + oTarget.offsetHeight + ', ' + oTarget.offsetWidth + ')';

// disable resize handling code until its needed
//oTarget.onresize = new Function("return HandleResize(" + sVSID + ")");

oTarget.VSB = sVSID;
}

// 'system' defined handler which takes care of internal business and dispatches to callback
function HandleVScrollClick(oCtl)
{
var vData = oCtl.SCROLLDATA;
var sUserCode = oCtl.USERCODE;
eval(sUserCode + '(vData)');
}

function AddVScrollButton(oParent, iTop, iLeft, iHeight, iWidth, sCode, vData, sUserCode)
{
var sBtnID = null;
if (!(sBtnID = GenerateID('VS')))
{
return null;
}

var sBtn = '<BUTTON ID="' + sBtnID + '" USERCODE="' + sUserCode + '" SCROLLDATA="' + vData + '" onclick="' + sCode + '" STYLE="position:absolute;top:' + iTop + ';left:' + iLeft + ';height:' + iHeight + ';width:' + iWidth + '"></BUTTON>';
document.body.insertAdjacentHTML('beforeEnd', sBtn);

return sBtnID;
}
