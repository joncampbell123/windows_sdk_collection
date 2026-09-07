
// -----------------------------------------------------------
// InitNavLinks()
// Sets link text, URL and icon for NavLinks Show based on framed state
// -----------------------------------------------------------

function InitNavLinks()
{
if ("object" != typeof(lnkShowText) || "object" != typeof(lnkShowImg) || "object" != typeof(imgShow))
{
return false;
}

var bIsFramed = ((top != self) && ("TOC" == top.frames[0].name));

if (bIsFramed)
{
lnkShowText.innerText = "hide toc";
lnkShowText.href = self.location.href;
lnkShowImg.href = self.location.href;
imgShow.src = "/msdn-online/shared/graphics/icons/nl-hide-0.gif";
}
else
{
lnkShowText.innerText = "show toc";
var sPath = self.location.pathname;

// WATCH OUT FOR INSTANCES OF MSDN-ONLINE !!!
if (location.protocol == "http:") {
   sPath = sPath.replace("/msdn-online","");
   var sFrameHref = sPath.match(/[/][^/]+[/]/) + "c-frame.htm#" + sPath;
   }
else {
                     sPath = sPath.replace("\\msdn-online","");
                     var sFrameHref = sPath.match(/[\\][^\\]+[\\]/) + "c-frame.htm#" + sPath;
                  }
                  lnkShowText.href = sFrameHref;
lnkShowImg.href = sFrameHref;
imgShow.src = "/msdn-online/shared/graphics/icons/nl-show-0.gif";
}
}

// -----------------------------------------------------------
// NavLinks_hover()
// DHTML script for NavLinks mouseover and mouseout.
// -----------------------------------------------------------

function NavLinks_hover(eContainer)
{
var eSrc = window.event.srcElement;
var sEventType = window.event.type;
while (eSrc != eContainer)
{
if ("clsLeftMenu" == eSrc.className)
{
window.event.cancelBubble = true;

if ("IMG" == eSrc.tagName)
{
var eImg = eSrc;
var eLink = document.all[eSrc.id.replace("img","lnk") + "Text"];
}
else if ("A" == eSrc.tagName)
{
var eImg = document.all[eSrc.id.substring(0, eSrc.id.length - 4).replace("lnk","img")];
var eLink = eSrc;
}

if ("mouseover" == sEventType)
{
if ("object" == typeof(eImg)) eImg.src = eImg.src.substring(0,eImg.src.length - 5) + "1.gif";
if ("object" == typeof(eLink)) eLink.style.color = "red";
}
else if ("mouseout" == sEventType)
{
if ("object" == typeof(eImg)) eImg.src = eImg.src.substring(0,eImg.src.length - 5) + "0.gif";
if ("object" == typeof(eLink)) eLink.style.color = "";
}
}
eSrc = eSrc.parentElement;
}
}

if ("object" == typeof(tblNavLinks))
{
tblNavLinks.onmouseover = tblNavLinks.onmouseout = new Function("NavLinks_hover(this)");
}

if ("object" == typeof(tblPrevNext))
{
tblPrevNext.onmouseover = tblPrevNext.onmouseout = new Function("NavLinks_hover(this)");
}

// -----------------------------------------------------------
// PreloadNavLinksImages()
// Preloads mouseover images.
// -----------------------------------------------------------

function PreloadNavLinksImages()
{
var sRootPath = new String("/msdn-online/shared/graphics/icons/");
var aImages = new Array("show","hide","sync","index","search","prev","next","up");
for (var i=0;i<aImages.length;i++)
{
var eImg = new Image();
eImg.src = sRootPath + "nl-" + aImages[i] + "-1.gif";
}
}
if (oBD.getsNavBar) PreloadNavLinksImages();
