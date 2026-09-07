function BrowserData()
{
this.userAgent = navigator.userAgent;
var rPattern = /(MSIE)\s(\d+)\.(\d+)((b|p)([^(s|;)]+))?;?(.*(98|95|NT|3.1|32|Mac|X11))?/;
if (this.userAgent.match(rPattern))
{
this.browser = "MSIE";
this.majorVer = parseInt(RegExp.$2) || 0;
this.minorVer = RegExp.$3.toString() || "0";
this.betaVer = RegExp.$6.toString() || "0";
this.platform = RegExp.$8 || "Other";
}
else if (this.userAgent.match(/Mozilla[/].*(95[/]NT|95|NT|98|3.1).*Opera.*(\d+)\.(\d+)/))
{
//"Mozilla/4.0 (Windows NT 5.0;US) Opera 3.60  [en]";
this.browser = "Opera";
this.majorVer = parseInt(RegExp.$2) || parseInt(RegExp.$2) || 0;
this.minorVer = RegExp.$3.toString() || RegExp.$3.toString() || "0";
this.platform = RegExp.$1 || "Other";
}
else if (this.userAgent.match(/Mozilla[/](\d*)\.?(\d*)(.*(98|95|NT|32|16|68K|PPC|X11))?/))
{
//"Mozilla/4.5 [en] (WinNT; I)"
this.browser = "Nav";
this.majorVer = parseInt(RegExp.$1) || 0;
this.minorVer = RegExp.$2.toString() || "0";
this.platform = RegExp.$4 || "Other";
}
else
{
this.browser = "Other";
}
this.getsNavBar = ("MSIE" == this.browser && 4 <= this.majorVer && "Mac" != this.platform && "X11" != this.platform);
this.doesActiveX = ("MSIE" == this.browser && 3 <= this.majorVer && ("95" == this.platform || "98" == this.platform || "NT" == this.platform));
}