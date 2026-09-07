
// -------------------------------------------------------
// Event Handler
// -------------------------------------------------------

function LeftNav_click()
{
      
if ("A" == (eSrc = window.event.srcElement).tagName.toUpperCase() && null == eSrc.getAttribute("expNoToc")) {
   if (location.protocol == "http:") 
      eSrc.href = "/" + eSrc.pathname.substring(0,eSrc.pathname.indexOf("/")) + "/c-frame.htm#/" + eSrc.pathname;
   else if (location.protocol == "file:")
      eSrc.href = eSrc.pathname.substring(0,eSrc.pathname.indexOf("\\", 3)) + "\\c-frame.htm#" + eSrc.pathname;
}
}

// -------------------------------------------------------
// Event-binding 
// -------------------------------------------------------

if (eLeftNav = document.all("tblLeftNav"))
{
tblLeftNav.onclick = LeftNav_click;
}
