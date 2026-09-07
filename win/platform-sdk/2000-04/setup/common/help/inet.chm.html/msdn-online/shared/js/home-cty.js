
//-------------------------------------------------------------------------
//HackURL()
//Purpose: Inserts a unique search value into a c-frame URL. This value
//is obtained by applying the getTime method to the Date object.
//Called: from the NavTbl_click event handler
//-------------------------------------------------------------------------

function HackURL(sURL)
{
var iTime = new Date().getTime();
var sFrameURL = sURL.substring(0,sURL.indexOf("#"));
var sHash = sURL.substring(sURL.indexOf("#"));
if ("" != sFrameURL)
{
return sHash.substring(1);
}
}

// -------------------------------------------------------
// Event Handler
// -------------------------------------------------------

function NavTbl_click() {
   if ("A" == (eSrc = window.event.srcElement).tagName.toUpperCase() && null == eSrc.getAttribute("expNoToc")) {
      if (location.protocol == "http:") {
         eSrc.href = HackURL("/" + eSrc.pathname.substring(0,eSrc.pathname.indexOf("/")) + "/c-frame.htm#/" + eSrc.pathname);
         eSrc.target = "_top";
      }
      else if (location.protocol == "file:") {
         eSrc.href = HackURL("\\" + eSrc.pathname.substring(0,eSrc.pathname.indexOf("\\", 3)) + "\\c-frame.htm#" + eSrc.pathname);
         eSrc.target = "_top";
      }
   }
}

// -------------------------------------------------------
// Event-binding
// -------------------------------------------------------

if ("object" == typeof(tblLeftNav))
{
tblLeftNav.onclick = NavTbl_click;
}

if ("object" == typeof(tblRtNav))
{
tblRtNav.onclick = NavTbl_click;
}

