var g_aImages = new Array("unknown", "attribute", "method", "prop_ro", "prop_rw", "event", "collection", "object", "behavior");
var g_memberImages = new Array(g_aImages.length);
var g_sGExt = ".gif";

var g_sFC = null; // optimize by caching current friendly column

var g_sObject = null;
var g_isPostIE5B1 = false;

//PreLoadMemberIcons();

// insert object-page-specific data-bound tables into the page. IE4/Windows only  
// vDefView - optional string parameter specifying default filter for members table.
// acceptable values: all, methods, properties, events, attributes (default), or collections
function AddObjTables(vDefView)
{
	var sDataPath = "../data/"; // using relative path allows this code to be used by ANY reference
	
	var oReg = new RegExp("\\\\", "g");
	var sPath = location.pathname.replace(oReg, "/");
	aPath = sPath.split("/");
	g_sObject = aPath[aPath.length-1].split(".")[0];

	if (typeof(divMembers) == 'object')
	{
		// Enable persistence
		if (g_isPostIE5B1 = (g_isIE && g_iMaj == 5 && typeof(divMembers.style.pixelRight) == 'number'))
		{
			//BUGBUG (IE5 B1) behaviors cannot be defined in-line
			document.styleSheets[0].addRule(".userData","behavior:url(#_IE_)");
			divMembers.className="userData";
			divMembers.load("INETSDKStore");
			var sTemp=divMembers.getAttribute(g_sObject);
			if (sTemp)
			{
				vDefView=sTemp;
			}
		}

		// coerce to a sensible default if nothing passed
		if (typeof(vDefView) == "undefined")
		{
			vDefView = (location.pathname.lastIndexOf('collections') == -1) ? 0 : -1;
		}
		else if (typeof(vDefView) == "string")
		{
			vDefView = MapStringToView(vDefView);
		}

		cFilter = GetFilterExpr(vDefView);

		g_sFC = GetFriendlyCol(vDefView);
		var cImgCol = GetImgCol(vDefView);

		sMembers = '<OBJECT classid="clsid:333C7BC4-460F-11D0-BC04-0080C7055A83" ' +
			'ID=tdcMembers HEIGHT=0 WIDTH=0>' +
			'<PARAM NAME="DataURL" VALUE="' + sDataPath + g_sObject + '_members.csv">' +
			'<PARAM NAME="UseHeader" VALUE="True">' +
			'<PARAM NAME="TextQualifier" VALUE="|">' +
			'<PARAM NAME="CaseSensitive" VALUE="False">' +
			'<PARAM NAME="Sort" VALUE="' + g_sFC + '">' +
			(cFilter == '' ? '' : '<PARAM NAME="Filter" VALUE="' + cFilter + '">') +
			'</OBJECT>'

		document.body.insertAdjacentHTML('afterBegin', sMembers);

		divMembers.innerHTML = "<TABLE ID=tblMembers DATASRC=#tdcMembers>" +
			"<THEAD><TR VALIGN=TOP BGCOLOR=#DDDDDD><TH>" + 
			BuildViewCombo() +
			"</TH><TH>&nbsp;</TH><TH>Description</TH></TR></THEAD>" +
			"<TBODY><TR><TD NOWRAP><A DATAFLD=ref_link>" +

			"<SPAN ID=ref_friendly DATAFLD=" + g_sFC + "></SPAN></A></TD>" +

			"<TD><IMG ID=ref_icon DATAFLD=" + cImgCol + " onload='ModifyImageTitle(this)'></TD>" +
			"<TD><SPAN DATAFLD=ref_desc DATAFORMATAS=html></TD></TR></TABLE>";

		cboInvokeKind.selectedIndex = MapViewToOption(vDefView);
	}

	if (typeof(divCSS) == 'object') // CSS is optional if object doesn't support it.
	{
		sCSS = '<OBJECT WIDTH=0 HEIGHT=0 id=tdcCSS CLASSID="clsid:333C7BC4-460F-11D0-BC04-0080C7055A83">' +
			'<PARAM NAME="DataURL" VALUE="' + sDataPath + g_sObject + '_css.csv">' +
			'<PARAM NAME="UseHeader" VALUE="True">' +
			'<PARAM NAME="TextQualifier" VALUE="|">' +
			'<PARAM NAME="Sort" VALUE="cssattr">' +
			'</OBJECT>'

		document.body.insertAdjacentHTML('afterBegin', sCSS);

		divCSS.innerHTML = "<TABLE DATASRC=#tdcCSS onreadystatechange='fixTable()'>" +
			"<THEAD><TR VALIGN=TOP BGCOLOR=#DDDDDD><TH>CSS attribute</TH><TH>Style property</TH><TH>Description</TH></TR>" +
			"<TBODY><TR><TD><A DATAFLD=propurl><SPAN DATAFLD=cssattr></SPAN></A></TD>" +
			"<TD><SPAN DATAFLD=prop></SPAN></TD>" +
			"<TD><SPAN DATAFORMATAS=html DATAFLD=css_desc></SPAN></TD></TR>" +
			"</TABLE>";
	}
}

// Filter the members displayed supplied by a TDC
// oSelect - reference to a select
// oTDC - reference to a TDC
function FilterMembers(oSelect, oTDC)
{
	var cFilter='';
	var iValue = oSelect.options(oSelect.selectedIndex).value;

	// Persist the current filter
	if (g_isPostIE5B1)
	{
		var sText = oSelect.options(oSelect.selectedIndex).text;
		divMembers.setAttribute(g_sObject,sText);
		divMembers.save("INETSDKStore");
	}

	cFilter = GetFilterExpr(iValue);

	if (RebindMTCols(tblMembers, GetFriendlyCol(iValue), GetImgCol(iValue)))
	{
		oTDC.object.Sort = g_sFC;
	}
		
	oTDC.object.Filter = cFilter;
	oTDC.Reset();
}

// Rebind the friendly column to the appropriate field in the data set but only if necessary
function RebindMTCols(oTable, sNewFCol, sNewImgCol)
{
	// Don't bother rebinding to the same column
	if (g_sFC == sNewFCol)
	{
		return false;
	}

	var sDSO = oTable.dataSrc;
	oTable.dataSrc = '';
	ref_friendly.dataFld = sNewFCol;
	ref_icon.dataFld = sNewImgCol;
	oTable.dataSrc = sDSO;

	g_sFC = sNewFCol;
	return true;
}

// Fired when a member image loads. Allows us to set the tooltip
// oImg - reference to an image object
// no need to specify a full vroot here.

g_aMemberImg2Str = new Array();
g_aMemberImg2Str["method"] = "Method";
g_aMemberImg2Str["prop_ro"] = "Read-Only Property";
g_aMemberImg2Str["prop_rw"] = "Read/Write Property";
g_aMemberImg2Str["event"] = "Event";
g_aMemberImg2Str["collection"] = "Collection";
g_aMemberImg2Str["object"] = "Object";
g_aMemberImg2Str["attribute"] = "Attribute";
g_aMemberImg2Str["behavior"] = "Behavior";

g_oRegImgTitle = new RegExp("(method|prop_ro|prop_rw|event|collection|object|attribute|behavior)\.gif");
g_oRegProp = new RegExp("(prop_..|attribute)");

function ModifyImageTitle(oImg)
{
	var sImgPath = oImg.href.toLowerCase();

	var aMatch = sImgPath.match(g_oRegImgTitle);

	oImg.title = (null != aMatch ? g_aMemberImg2Str[aMatch[1]] : "Member");

	if (g_oRegProp.test((null != aMatch ? aMatch[1] : sImgPath)))
	{
		ModPropAttr(tblMembers.rows[oImg.recordNumber].children(0).all(1));
	}
}

// Builds a string and sets the tooltip for the cell
// When All is selected, the property rather than the attribute is displayed
// When Attrib is selected, the attrib is displayed and the tooltip is arranged appropriately
// When property is selected, the property is displayed ...
// oPropCell - the cell to be tipped
function ModPropAttr(oPropCell)
{
	if (typeof('tdcMembers') != 'object')
	{
		return;
	}

	tdcMembers.recordset.AbsolutePosition = oPropCell.recordNumber;
	
	var iFilter = cboInvokeKind.options(cboInvokeKind.selectedIndex).value;

	var sPropValue = null, sAttrValue = null;
	
	with (tdcMembers.recordset.fields)
	{
		sPropValue = item('ref_dynamic').value;
		sAttrValue = item('ref_persistent').value;
		if (sPropValue == 'null' || sAttrValue == 'null')
		{
			return;
		}
	}

	var sProp = sPropValue + ' property';
	var sAttr = sAttrValue + ' attribute';
	var sTip = 'The ' + (iFilter==0 ? sAttr : sProp) + ' corresponds to the ' + (iFilter==0 ? sProp : sAttr) + '.';
	oPropCell.className = "propattr";
	oPropCell.title = sTip;
}

var g_FilterMap = new Array();
g_FilterMap[-1] = 'ref_dynamic <> null'; // show all dynamic member
g_FilterMap[2] = '((invoke_kind=2 | invoke_kind=4 | invoke_kind=6) & ref_dynamic <> null)';
g_FilterMap[4] = g_FilterMap[2];
g_FilterMap[6] = g_FilterMap[2];

//'(ref_friendly = * \/ *) | invoke_kind = 0'; // old attribute filter
g_FilterMap[0] = "invoke_kind=0 | ((invoke_kind=2 | invoke_kind=6) & ref_persistent <> null)"; // look for text pattern; requires a space followed by slash followed by space to match

// Map an invoke_kind to a filter expression for use by the TDC
// With the exception of -1 (show all) vValue corresponds to the invoke_kind field from the data set
function GetFilterExpr(iValue)
{
	return (typeof(g_FilterMap[iValue]) == 'string' ? g_FilterMap[iValue] : '(invoke_kind=' + iValue + ')');
}

function GetFriendlyCol(iValue)
{
	return (iValue == 0) ? 'ref_persistent' : 'ref_dynamic';
}

function GetImgCol(iValue)
{
	return (iValue == 0) ? 'pers_icon' : 'dyn_icon';
}

var g_aMemberViewByName = new Array();
g_aMemberViewByName["all"] = -1;
g_aMemberViewByName["attributes"] = 0;
g_aMemberViewByName["properties"] = 6;
g_aMemberViewByName["methods"] = 1;
g_aMemberViewByName["events"] = 8;
g_aMemberViewByName["collections"] = 32;
g_aMemberViewByName["behaviors"] = 128;

// Map the string to its numeric equivalent
function MapStringToView(sView)
{
	sView = sView.toLowerCase();

	// if the string isn't a valid key, return an acceptable default
	return (typeof(g_aMemberViewByName[sView]) == 'number' ? g_aMemberViewByName[sView] : -1);
}

var g_aMemberViewByNum = new Array();
g_aMemberViewByNum[-1] = 0;
g_aMemberViewByNum[0] = 1;
g_aMemberViewByNum[2] = 2;
g_aMemberViewByNum[4] = 2;
g_aMemberViewByNum[6] = 2;
g_aMemberViewByNum[1] = 3;
g_aMemberViewByNum[8] = 4;
g_aMemberViewByNum[32] = 5;
g_aMemberViewByNum[128] = 6;
	
// map the specified view to an index into a SELECT defined below
function MapViewToOption(vView)
{
	return (typeof(g_aMemberViewByNum[vView]) == 'number' ? g_aMemberViewByNum[vView] : 0);
}

// return a string representing the SELECT to be injected into the data bound table
function BuildViewCombo()
{
		return "<SELECT ID=cboInvokeKind onchange='FilterMembers(this, tdcMembers)'>" +
		"<OPTION VALUE=-1>All" +
		"<OPTION VALUE=0>Attributes" +
		"<OPTION VALUE=6>Properties" +
		"<OPTION VALUE=1>Methods" +
		"<OPTION VALUE=8>Events" +
		"<OPTION VALUE=32>Collections" +
		"<OPTION VALUE=128>Behaviors" + 
		"</SELECT>";
}

// Preload the icons used in the member table
function PreLoadMemberIcons()
{
	// preload the member images
	for (i = 0; i < g_aImages.length; i++)
	{
		var sImage = g_sGR + g_aImages[i] + g_sGExt;
		g_memberImages[g_aImages[i]] = new Image();
		g_memberImages[g_aImages[i]].src = sImage;
	}
}

function fixTable()
{
	var tbl=event.srcElement
	if (tbl.readyState=="complete")
	{
		for (i=0;i<tbl.rows.length;i++)
		{
			if (tbl.rows(i).cells(0).innerText=="behavior")
			{
				tbl.rows(i).cells(0).insertAdjacentText("beforeEnd"," (proposed)");
				break;
			}
		}	
	}
}
