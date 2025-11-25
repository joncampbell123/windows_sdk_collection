// common sniffing code  
g_sUA = window.navigator.appVersion;

if (parseInt(g_sUA.indexOf("MSIE ")) >= 0) // Check if IE
{
	g_isIE = true;
	g_iMaj = parseInt(g_sUA.substring(g_sUA.indexOf("MSIE ")+5,g_sUA.indexOf (".",g_sUA.indexOf("MSIE "))));
	if (g_sUA.lastIndexOf("Win") >= 0)
		g_sPlat = "Win";
}
else if(parseInt(navigator.appName.lastIndexOf("Netscape"))>=0)
{
	g_isNav = true;
	g_iMaj = parseInt(g_sUA.substring(0, g_sUA.indexOf('.')));
}

g_isMac = g_sUA.indexOf("Mac") != -1;
g_isGB = (g_isIE && g_iMaj >= 4 && g_sPlat == "Win");
