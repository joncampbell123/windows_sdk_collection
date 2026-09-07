
if (4 <= parseInt(navigator.appVersion))
{

// TURN DIVIDER LINE WHITE

var aMenus = new Array("voice","lib","comm","download","guide");
for (var i=0;i<aMenus.length;i++)
{
if (eMenu = document.all["ICP_" + aMenus[i] + "Menu"])
{
if (eHR = eMenu.children(0))
{
if ("HR" == eHR.tagName.toUpperCase())
{
eHR.style.color = "white";
}
}
}
}

//SET WIDTH FOR EXPANDED LIBRARIES MENU (SEE LOCAL.JS)

   if ("object" == typeof(ICP_libMenu)) ICP_libMenu.style.width = "250px";

}

// REMOVE LAST PIPE (AFTER SEARCH) ON ICP TB

function RemoveLastPipe()
{
if ((eLink = document.all["AM_ICP_searchMenu"]) && (eSpan = document.all[eLink.sourceIndex + 1]))
{
if (eSpan.innerHTML && eSpan.innerHTML=="&nbsp;|") eSpan.innerHTML = "";
}
}