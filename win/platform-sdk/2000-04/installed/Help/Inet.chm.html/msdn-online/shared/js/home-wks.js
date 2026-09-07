
// -------------------------------------------------------
// Array of bucket descriptions
// -------------------------------------------------------

var aCatDesc = new Array(15);
aCatDesc[0] = '<H2 CLASS="clsBlue">ESSENTIALS</H2><P>This section contains core information and references, including information on authoring for different browsers and platforms, end-to-end examples of working Web sites, slides from conferences, specs, and comprehensive links to references and standards.</P>';
aCatDesc[1] = '<H2 CLASS="clsBlue">Component Development</H2><P>This section contains information you\'ll need to create components for your Web pages, using either ActiveX or DHTML scriptlet technology, as well as  related information about COM, ActiveX Scripting, Active Documents, and offline browsing.</P>';
aCatDesc[2] = '<H2 CLASS="clsBlue">Content &amp; Component Delivery</H2><P>This section includes information on Active Desktop, software update channels, channel definition format (CDF), Internet component downloads, hit logging, and information delivery APIs.</P>';
aCatDesc[3] = '<H2 CLASS="clsBlue">Data Access &amp; Databases</H2><P>This section includes information on ActiveX Data Objects (ADO), Remote Data Service (RDS), and the Tabular Data Control (TDC). You can also read about the MSHTML data binding interfaces, which provide COM-based access to data binding functionality.</P>';
aCatDesc[4] = '<H2 CLASS="clsBlue">Design</H2><P>In this section you\'ll learn what constitutes good Web page design, including information on layout, typography, and color. You\'ll also find designers of Web pages sharing their views and tips about page layout and design.</P>';
aCatDesc[5] = '<H2 CLASS="clsBlue">DHTML, HTML &amp; CSS</H2><P>This section contains information about Dynamic HTML, authoring with HTML and CSS, and programming with the DHTML object model. You\'ll also learn how to use dynamic styles and content, positioning, data binding, and filters and transitions to go beyond basic functionality on your Web pages.</P>';
aCatDesc[6] = '<H2 CLASS="clsBlue">Languages &amp; Development Tools</H2><P>In this section you\'ll find information about scripting and programming languages, including VBScript, JScript, and Java. Also included is information about authoring, debugging, and source management tools.</P>';
aCatDesc[7] = '<H2 CLASS="clsBlue">Messaging &amp; Collaboration</H2><P>This section contains information on communicating over the Internet by using mail, chat, or technologies such as NetMeeting.</P>';
aCatDesc[8] = '<H2 CLASS="clsBlue">Networking, Protocols<BR>&amp; Data Formats</H2><P>This section includes information on the WinInet API, Windows Sockets, ISAPI, monikers, and other networking topics. Also covered are pluggable and predefined protocols, data formats such as Common Internet File System (CIFS), and the HTML Clipboard format.</P>';
aCatDesc[9] = '<H2 CLASS="clsBlue">Reusing Browser Technology</H2><P>Internet Explorer (versions 4.0 and later) encompasses technologies that perform functions like interpreting HTML and providing a browser interface. Much of this technology is available to application writers, and this section will teach you how to reuse these browser technologies.</P>';
aCatDesc[10] = '<H2 CLASS="clsBlue">Security &amp; Cryptography</H2><P>This section provides general information about security, as well as more specific details about Authenticode, URL security zones, and Internet ratings. Also included is information about cryptography and CryptoAPI.</P>';
aCatDesc[11] = '<H2 CLASS="clsBlue">Server Technologies</H2><P>This section covers server issues and server-side technologies such as Active Server Pages (ASP) and server-side components. You\'ll also find links to all related Microsoft server products and technologies.</P>';
aCatDesc[12] = '<H2 CLASS="clsBlue">Streaming &amp; Interactive Media</H2><P>In this section you\'ll learn how to add interactive multimedia effects to your Web pages, using DirectAnimation, ActiveMovie and other DirectX Media technologies, Microsoft Agent, and Interactive Music. Also included is information on streaming media using Windows Media Technologies.</P>';
aCatDesc[13] = '<H2 CLASS="clsBlue">Web Content Management</H2><P>This section provides information you\'ll need to publish and administer your site, including publishing tools, software setup technologies, strategies for starting up your site, testing and redesign tips, technology tradeoffs, and international issues.</P>';
aCatDesc[14] = '<H2 CLASS="clsBlue">Extensible Markup Language</H2><P>In this section you\'ll find everything you ever wanted to know about the Extensible Markup Language (XML) and the Extensible Stylesheet Language (XSL), including language and object model specifications, articles, demos, and tools.</P>';

// -------------------------------------------------------
// Event Handlers
// -------------------------------------------------------

function LeftNav_mouseover()
{
if ("complete" == document.readyState && "A" == (eSrc = window.event.srcElement).tagName.toUpperCase())
{
var sId = eSrc.id;
var iCatNum = sId.substring(sId.lastIndexOf("-")+1);
if (eCatDiv = document.all("divCatDesc")) eCatDiv.innerHTML = aCatDesc[iCatNum];
}
}

function LeftNav_click() {
   if ("A" == (eSrc = window.event.srcElement).tagName.toUpperCase() && null == eSrc.getAttribute("expNoToc")) {
      if (location.protocol == "http:") 
         eSrc.href = "/" + eSrc.pathname.substring(0,eSrc.pathname.indexOf("/")) + "/c-frame.htm#/" + eSrc.pathname;
      else if (location.protocol == "file:")
         eSrc.href = eSrc.pathname.substring(0, eSrc.pathname.indexOf("\\", eSrc.pathname.indexOf("\\") +1 )) + "\\c-frame.htm#" + eSrc.pathname;
   }
}

// -------------------------------------------------------
// Event-binding
// -------------------------------------------------------

if (eLeftNav = document.all("tblLeftNav"))
{
tblLeftNav.onmouseover = LeftNav_mouseover;
tblLeftNav.onclick = LeftNav_click;
}
