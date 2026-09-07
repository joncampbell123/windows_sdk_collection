var ToolBar_Supported = ToolBar_Supported ;
if (ToolBar_Supported != null && ToolBar_Supported == true)
{
//To Turn on/off Frame support, set Frame_Supported = true/false.
Frame_Supported = false;

// Customize default ICP menu color - bgColor, fontColor, mouseoverColor
setDefaultICPMenuColor("99CCFF", "#000000", "#FF3300");

// Customize toolbar background color
setToolbarBGColor("#FFFFFF");

// display ICP Banner
//***** Add ICP menus *****

if (location.protocol == "http:") {  //Web Based Menus

setICPBanner("/msdn-online/start/images/bnr/msdn-online-banner.gif","/msdn-online/default.asp","MSDN Online") ;

//HOME
addICPMenu("HomeMenu", " HOME ", "","/default.asp");

//VOICES
addICPMenu("voiceMenu", " VOICES ", "","/voices/default.asp");
addICPSubMenu("voiceMenu"," Voices Home","/voices/default.asp");
addICPSubMenu("voiceMenu"," Ask Jane","/voices/jane.asp");
addICPSubMenu("voiceMenu"," Code Corner","/voices/corner.asp");
addICPSubMenu("voiceMenu"," Deep C++","/voices/deep.asp");
addICPSubMenu("voiceMenu"," Design Discussion","/voices/site.asp");
addICPSubMenu("voiceMenu"," DHTML Dude","/voices/dude.asp");
addICPSubMenu("voiceMenu"," Dr. GUI","/voices/drgui.asp");
addICPSubMenu("voiceMenu"," Duwamish Sample App","/voices/sampleapp.asp");
addICPSubMenu("voiceMenu"," Extreme XML","/voices/xml.asp");
addICPSubMenu("voiceMenu"," Geek Speak Decoded","/voices/geek.asp");
addICPSubMenu("voiceMenu"," Letters to MSDN","/voices/letters.asp");
addICPSubMenu("voiceMenu"," More or Hess","/voices/hess.asp");
addICPSubMenu("voiceMenu"," The Newspaper","/voices/news/default.asp");
addICPSubMenu("voiceMenu"," Office Talk","/voices/office.asp");
addICPSubMenu("voiceMenu"," Q&amp;A: Interviews","/voices/qa.asp");
addICPSubMenu("voiceMenu"," Scripting Clinic","/voices/scripting.asp");
addICPSubMenu("voiceMenu"," Servin' It Up","/voices/server.asp");
addICPSubMenu("voiceMenu"," Stone's Way","/voices/stone.asp");
addICPSubMenu("voiceMenu"," Web Men Talking","/voices/webmen.asp");

//LIBRARIES
addICPMenu("libMenu", " LIBRARIES ", "","/resources/libraries.asp");
//addICPSubMenu("libMenu", " Home ","/resources/libraries.asp");
addICPSubMenu("libMenu"," Library Home ","/library/default.asp");
addICPSubMenu("libMenu"," Web Workshop Home ","/workshop/default.asp");

// UNCOMMENT FOR EXPANDED LIBRARIES MENU
/*
addICPSubMenu("libMenu"," &nbsp; Essentials","/workshop/c-frame.asp#/workshop/essentials/default.asp");
addICPSubMenu("libMenu"," &nbsp; Component Development","/workshop/c-frame.asp#/workshop/components/default.asp");
addICPSubMenu("libMenu"," &nbsp; Content &amp; Component Delivery","/workshop/c-frame.asp#/workshop/delivery/default.asp");
addICPSubMenu("libMenu"," &nbsp; Data Access &amp; Databases","/workshop/c-frame.asp#/workshop/database/default.asp");
addICPSubMenu("libMenu"," &nbsp; Design","/workshop/c-frame.asp#/workshop/design/default.asp");
addICPSubMenu("libMenu"," &nbsp; DHTML, HTML &amp; CSS","/workshop/c-frame.asp#/workshop/author/default.asp");
addICPSubMenu("libMenu"," &nbsp; Languages &amp; Development Tools","/workshop/c-frame.asp#/workshop/languages/default.asp");
addICPSubMenu("libMenu"," &nbsp; Messaging &amp; Collaboration","/workshop/c-frame.asp#/workshop/messaging/default.asp");
addICPSubMenu("libMenu"," &nbsp; Networking, Protocols &amp; Data Formats","/workshop/c-frame.asp#/workshop/networking/default.asp");
addICPSubMenu("libMenu"," &nbsp; Reusing Browser Technology","/workshop/c-frame.asp#/workshop/browser/default.asp");
addICPSubMenu("libMenu"," &nbsp; Security &amp; Cryptography","/workshop/c-frame.asp#/workshop/security/default.asp");
addICPSubMenu("libMenu"," &nbsp; Server Technologies","/workshop/c-frame.asp#/workshop/server/default.asp");
addICPSubMenu("libMenu"," &nbsp; Streaming &amp; Interactive Media","/workshop/c-frame.asp#/workshop/imedia/default.asp");
addICPSubMenu("libMenu"," &nbsp; Web Content Management","/workshop/c-frame.asp#/workshop/management/default.asp");
addICPSubMenu("libMenu"," &nbsp; XML (Extensible Markup Language)","/workshop/c-frame.asp#/xml/default.asp");
*/

//COMMUNITY
addICPMenu("commMenu", " COMMUNITY ", "","/community/default.asp");
addICPSubMenu("commMenu", " Community Home ","/community/default.asp");
addICPSubMenu("commMenu"," Join","/community/join.asp");
addICPSubMenu("commMenu"," Members Helping Members","/community/c-frame.asp#/community/mhm/default.asp");
addICPSubMenu("commMenu"," Offers","/community/c-frame.asp#/community/offers/default.asp");
addICPSubMenu("commMenu"," OSIGs","/osig/c-frame.asp#/osig/default.asp");
addICPSubMenu("commMenu"," Member Gazette","/community/c-frame.asp#/community/gazette/default.asp");
addICPSubMenu("commMenu"," Training","/training/default.asp");
addICPSubMenu("commMenu"," Ask the Expert Chats" ,"/training/c-frame.asp#/training/chats/default.asp");

//DOWNLOADS
addICPMenu("downloadMenu", " DOWNLOADS ", "","/downloads/default.asp");
addICPSubMenu("downloadMenu", " Downloads Home ","/downloads/default.asp");
addICPSubMenu("downloadMenu"," Images","/downloads/c-frame.asp#/downloads/images/default.asp");
addICPSubMenu("downloadMenu"," Samples","/downloads/c-frame.asp#/downloads/samples/default.asp");
addICPSubMenu("downloadMenu"," Sounds","/downloads/c-frame.asp#/downloads/sounds/default.asp");
addICPSubMenu("downloadMenu"," Tools","/downloads/c-frame.asp#/downloads/tools/default.asp");
addICPSubMenu("downloadMenu"," Subscriber Downloads","/downloads/c-frame.asp#/downloads/tools/subscriber/default.asp");

//SITE GUIDE
addICPMenu("guideMenu", " SITE GUIDE ", "","/siteguide/default.asp");
addICPSubMenu("guideMenu", " Site Guide Home ","/siteguide/default.asp");
addICPSubMenu("guideMenu", " About MSDN","/siteguide/about.asp");
addICPSubMenu("guideMenu", " Using This Site","/siteguide/using.asp");
addICPSubMenu("guideMenu", " Site Map","/siteguide/sitemap.asp");
addICPSubMenu("guideMenu", " Recently Posted","/siteguide/recent.asp");
addICPSubMenu("guideMenu", " Glossaries","/siteguide/glossaries.asp");




addICPSubMenu("guideMenu", " Write Us","/siteguide/write-us.asp");

//SEARCH MSDN
addICPMenu("searchMenu", " SEARCH MSDN ", "","/isapi/gosearch.asp?target=/us/dev/");
}

else { //CD Based Menus

// display ICP Banner
setICPBanner("/msdn-online/start/images/bnr/msdn-online-banner.gif","/msdn-online/default.htm","MSDN Online") ;

//***** Add ICP menus *****
//HOME
addICPMenu("HomeMenu", " HOME ", "","/default.htm");

//VOICES
addICPMenu("voiceMenu", " VOICES ", "","/voices/default.htm");
addICPSubMenu("voiceMenu"," Voices Home","/voices/default.htm");
addICPSubMenu("voiceMenu"," Ask Jane","/voices/jane.htm");
addICPSubMenu("voiceMenu"," Code Corner","/voices/corner.htm");
addICPSubMenu("voiceMenu"," Deep C++","/voices/deep.htm");
addICPSubMenu("voiceMenu"," Design Discussion","/voices/site.htm");
addICPSubMenu("voiceMenu"," DHTML Dude","/voices/dude.htm");
addICPSubMenu("voiceMenu"," Dr. GUI","/voices/drgui.htm");
addICPSubMenu("voiceMenu"," Duwamish Sample App","/voices/sampleapp.htm");
addICPSubMenu("voiceMenu"," Extreme XML","/voices/xml.htm");
addICPSubMenu("voiceMenu"," Geek Speak Decoded","/voices/geek.htm");
addICPSubMenu("voiceMenu"," Letters to MSDN","/voices/letters.htm");
addICPSubMenu("voiceMenu"," More or Hess","/voices/hess.htm");
addICPSubMenu("voiceMenu"," The Newspaper","/voices/news/default.htm");
addICPSubMenu("voiceMenu"," Office Talk","/voices/office.htm");
addICPSubMenu("voiceMenu"," Q&amp;A: Interviews","/voices/qa.htm");
addICPSubMenu("voiceMenu"," Scripting Clinic","/voices/scripting.htm");
addICPSubMenu("voiceMenu"," Servin' It Up","/voices/server.htm");
addICPSubMenu("voiceMenu"," Stone's Way","/voices/stone.htm");
addICPSubMenu("voiceMenu"," Web Men Talking","/voices/webmen.htm");

//LIBRARIES
addICPMenu("libMenu", " LIBRARIES ", "","/resources/libraries.htm");
//addICPSubMenu("libMenu", " Home ","/resources/libraries.htm");
addICPSubMenu("libMenu"," Library Home ","/library/default.htm");
addICPSubMenu("libMenu"," Web Workshop Home ","/workshop/default.htm");

// UNCOMMENT FOR EXPANDED LIBRARIES MENU
addICPSubMenu("libMenu"," &nbsp; Essentials","/workshop/c-frame.htm#/workshop/essentials/default.htm");
addICPSubMenu("libMenu"," &nbsp; Component Development","/workshop/c-frame.htm#/workshop/components/default.htm");
addICPSubMenu("libMenu"," &nbsp; Content &amp; Component Delivery","/workshop/c-frame.htm#/workshop/delivery/default.htm");
addICPSubMenu("libMenu"," &nbsp; Data Access &amp; Databases","/workshop/c-frame.htm#/workshop/database/default.htm");
addICPSubMenu("libMenu"," &nbsp; Design","/workshop/c-frame.htm#/workshop/design/default.htm");
addICPSubMenu("libMenu"," &nbsp; DHTML, HTML &amp; CSS","/workshop/c-frame.htm#/workshop/author/default.htm");
addICPSubMenu("libMenu"," &nbsp; Languages &amp; Development Tools","/workshop/c-frame.htm#/workshop/languages/default.htm");
addICPSubMenu("libMenu"," &nbsp; Messaging &amp; Collaboration","/workshop/c-frame.htm#/workshop/messaging/default.htm");
addICPSubMenu("libMenu"," &nbsp; Networking, Protocols &amp; Data Formats","/workshop/c-frame.htm#/workshop/networking/default.htm");
addICPSubMenu("libMenu"," &nbsp; Reusing Browser Technology","/workshop/c-frame.htm#/workshop/browser/default.htm");
addICPSubMenu("libMenu"," &nbsp; Security &amp; Cryptography","/workshop/c-frame.htm#/workshop/security/default.htm");
addICPSubMenu("libMenu"," &nbsp; Server Technologies","/workshop/c-frame.htm#/workshop/server/default.htm");
addICPSubMenu("libMenu"," &nbsp; Streaming &amp; Interactive Media","/workshop/c-frame.htm#/workshop/imedia/default.htm");
addICPSubMenu("libMenu"," &nbsp; Web Content Management","/workshop/c-frame.htm#/workshop/management/default.htm");
addICPSubMenu("libMenu"," &nbsp; XML (Extensible Markup Language)","/workshop/c-frame.htm#/xml/default.htm");

//COMMUNITY
addICPMenu("commMenu", " COMMUNITY ", "","/community/default.htm");
addICPSubMenu("commMenu", " Community Home ","/community/default.htm");
addICPSubMenu("commMenu"," Join","/community/join.htm");
addICPSubMenu("commMenu"," Members Helping Members","/community/c-frame.htm#/community/mhm/default.htm");
addICPSubMenu("commMenu"," Offers","/community/c-frame.htm#/community/offers/default.htm");
addICPSubMenu("commMenu"," OSIGs","/osig/c-frame.htm#/osig/default.htm");
addICPSubMenu("commMenu"," Member Gazette","/community/c-frame.htm#/community/gazette/default.htm");
addICPSubMenu("commMenu"," Training","/training/default.htm");
addICPSubMenu("commMenu"," Ask the Expert Chats" ,"http://msdn.microsoft.com/training/c-frame.htm#/training/chats/default.asp");

//DOWNLOADS
addICPMenu("downloadMenu", " DOWNLOADS ", "","/downloads/default.htm");
addICPSubMenu("downloadMenu", " Downloads Home ","/downloads/default.htm");
addICPSubMenu("downloadMenu"," Images","http://msdn.microsoft.com/downloads/c-frame.htm#/downloads/images/default.asp");
addICPSubMenu("downloadMenu"," Samples","http://msdn.microsoft.com/downloads/c-frame.htm#/downloads/samples/default.asp");
addICPSubMenu("downloadMenu"," Sounds","http://msdn.microsoft.com/downloads/c-frame.htm#/downloads/sounds/default.asp");
addICPSubMenu("downloadMenu"," Tools","http://msdn.microsoft.com/downloads/c-frame.htm#/downloads/tools/default.asp");
addICPSubMenu("downloadMenu"," Subscriber Downloads","http://msdn.microsoft.com/downloads/c-frame.htm#/downloads/tools/subscriber/default.asp");

//SITE GUIDE
addICPMenu("guideMenu", " SITE GUIDE ", "","http://msdn.microsoft.com/siteguide/default.asp");
addICPSubMenu("guideMenu", " Site Guide Home ","/siteguide/default.htm");
addICPSubMenu("guideMenu", " About MSDN","http://msdn.microsoft.com/siteguide/about.asp");
addICPSubMenu("guideMenu", " Using This Site","http://msdn.microsoft.com/siteguide/using.asp");
addICPSubMenu("guideMenu", " Site Map","http://msdn.microsoft.com/siteguide/sitemap.asp");
addICPSubMenu("guideMenu", " Recently Posted","http://msdn.microsoft.com/siteguide/recent.asp");
addICPSubMenu("guideMenu", " Glossaries","http://msdn.microsoft.com/siteguide/glossaries.asp");




addICPSubMenu("guideMenu", " Write Us","http://msdn.microsoft.com/siteguide/write-us.aso");

//SEARCH MSDN
addICPMenu("searchMenu", " SEARCH MSDN ", "","http://msdn.microsoft.com/isapi/gosearch.asp?target=/us/dev/");
}
}