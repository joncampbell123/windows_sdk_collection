// Notes:
// - this code will also run on
// all flavors of Netscape 3.x, 4.x and IE 3.x, 4.x.


// these variables can be used globally by the included page
var bMSIE = false;
var bMSIE3 = false, bMSIE4 = false, bMSIE4_beta = false, bMSIE4_01 = false, bMSIE5 = false;
var bNetscape = false;
var bNetscape_2 = false, bNetscape_3 = false, bNetscape_4 = false;

function browser()
{
    var ua = navigator.userAgent;
    var an = navigator.appName;

    // is it IE?
    bMSIE = (ua.indexOf("MSIE")>=1);
    if (bMSIE)
    {
        // IE3
        bMSIE3 = (ua.indexOf("MSIE 3.0")>=1);

        // IE4
        var iMSIE4 = ua.indexOf("MSIE 4.0");
        bMSIE4 = (iMSIE4>=1);
        if (bMSIE4)
        {
            var sMinorVer = ua.charAt(iMSIE4+8);
            // some folks are still running an IE4 beta!
            // (the Mac IE team used a 'p' to mark their beta)
            bMSIE4_beta = bMSIE4 && ((sMinorVer == "b") || (sMinorVer == "p"));
            bMSIE4_01 = bMSIE4 && (sMinorVer == "1");
        }
        // IE5
        bMSIE5 = (ua.indexOf("MSIE 5.0")>=1);
    }
    else if (an == "Netscape")
    {
        bNetscape = true;
        appVer = parseInt(navigator.appVersion);
        if (appVer >= 4)
            bNetscape_4 = true;
        else if (appVer >= 3)
            bNetscape_3 = true;
        else
            bNetscape_2 = true;
    }
    // else please send your working sniffers for other browsers to michaele@microsoft.com
}
