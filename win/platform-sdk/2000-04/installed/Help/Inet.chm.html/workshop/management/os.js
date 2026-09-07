// you should include browser.js and call the browser() function first

// these variables can be used globally by the included page
var bWin16 = false; // includes Win31 and WinNT 3.51
var bWin95 = false;
var bWinNT = false;
var bMac = false, bMac68K = false, bMacPPC = false;
var bMSSun = false;

function os()
{
    var ua = navigator.userAgent;

// bMSIE is defined in browser.js
if (bMSIE)
{
//IE supported OS's
bWin95 = (ua.indexOf("Windows 95")>=1);
bWinNT = (ua.indexOf("Windows NT")>=1);
bWin16 = (ua.indexOf("Windows 3.1")>=1);
bMac = (ua.indexOf("Mac")>=1);
bMac68K = (ua.indexOf("Mac_68000")>=1);
bMacPPC = (ua.indexOf("Mac_PowerPC")>=1);
bMSSun = (ua.indexOf("SunOS")>=1)
} 
else if (bNetscape)
{
// NSCP supporte OS's
bWin95 = (ua.indexOf("Win95")>=1);
bWinNT = (ua.indexOf("WinNT")>=1);
bWin16 = (ua.indexOf("Win16")>=1);
bMac = (ua.indexOf("Mac")>=1);
if (bMac)
{
bMac68K = (ua.indexOf("68K")>=1);
bMacPPC = (ua.indexOf("PPC")>=1);
}
// BUGBUG: this doesn't find any of Netscape's Unix versions
}
}
