/* InetSDK Applies To Code */

// General comments are stored in this string.
var sGeneralComments="";

// Style comments are stored in this string.
var sStyleComments="";

// The CSV is stored in this array.
var aATData=new Array();
// Help is turned on by default.
var bShowHelp=true;
var sHelpText="Move the mouse cursor over an element in the Applies To List to display availability information for the listed platforms.";

// Initialize the list by adding the CSV, setting display values.
// No parameters, no return value, called from PostGBInit in common.js.
function fnATInit(){
fnCreateStyleSheets();
fnInsCSV();
oATRecordSet=oATAcqSource.recordset;

}
function fnCreateStyleSheets(){
document.createStyleSheet("/workshop/advSDKATIE4.css");
}
// No parameters, no return value, called from fnAcqData.
function fnPostATInit(){
// Setup table for IE 4 and 5.
oATData.width="85%";
oPlatData.parentElement.style.display="block";
oATData.onmouseover=fnATData;
oATData.onmouseout=fnATClear;
oATC.innerHTML=sHelpText;
oATC.style.display="block";
var oHasFocus=document.activeElement;
if(oHasFocus.parentElement==oATData){
fnUnPack(oHasFocus);
}
else{
oATHelp.style.display="block";
}
}


// Called from mouseover, no parameters, no return value.
function fnATClear(){
var oHasFocus=document.activeElement;
if(oHasFocus.parentElement==oATData){
fnUnPack(oHasFocus);
}
}



/* Once the CSV is loaded, check to verify it has information and then push it into an associative script array.
 Called from ondatasetcomplete event, accepts the recordset as a parameter, no return value.
 CE data is commented out.
*/
function fnAcqData(oATR){
var sElementName="";
var sOS1=""; // 16-bit Windows Platform
var sOS2=""; // 32-bit Windows Platform
var sOS3=""; // Macintosh
var sOS4=""; // Unix
var sOS5=""; // Windows CE
oATR.MoveFirst();

/* Start error checking */
var bLoadFail=false;
var oTemp=oATR.fields(0).value;
/* Check for 3 things: 1) the first field in the first row of every applies to list is "general_comments", 2) make sure there are no HTML tags by checking for a <, and 3) check for an empty string. */
if((oTemp.indexOf("general_comments")==-1)||(oTemp.indexOf("<")>0)||(oTemp=="")){
bLoadFail=true;
}
/* End error checking */

// Failure to open the TDC returns an empty web page.  Checking for the "404" determines if the TDC failed to load by virtue of non-existence.
if(bLoadFail==false){
for (var i=0;i < oATR.RecordCount;i++){
var aField=oATR.fields;
var sObjectName=aField("object").value;
if(sObjectName=="general_comments"){
sGeneralComments=aField("comments").value;
if(sGeneralComments!=""){
sGeneralComments+="<br>";
}
}
if(sObjectName=="style_comments"){
sStyleComments=aField("comments").value;
if(sStyleComments!=""){
sStyleComments+="<br>";
}
}
if((sObjectName!="general_comments")&&(sObjectName!="style_comments")){
sElementName=sObjectName;
sOS1=aField("win16").value;
sOS2=aField("win32").value
sOS3=aField("mac").value;
sOS4=aField("unix").value;
sOS5=aField("ce").value;
aATData[sElementName]=new fnPopulateData(sOS1,sOS2,sOS3,sOS4,sOS5);
}
oATR.MoveNext();
}
var oATNodes=oATData.all.tags("A");
for(var i=0;i<oATNodes.length;i++){
var oNode=oATNodes[i];
if(oNode.tagName=="A"){
oNode.onfocus=fnATData;
oNode.onblur=fnATClear;
var oData=aATData[oNode.innerText];
if(typeof(oData)=="undefined"){
oNode.setAttribute("ATWIN16V","N/A");
oNode.setAttribute("ATWIN32V","N/A");
oNode.setAttribute("ATMV","N/A");
oNode.setAttribute("ATUV","N/A");
oNode.setAttribute("ATCEV","N/A");

oNode.setAttribute("hasPlatformComments",false);
}
else{
var oWin16=oData.WIN16;
var sWin16V=oWin16.split("~")[1];
var sWin16=oWin16.split("~")[2];
var oWin32=oData.WIN32;
var sWin32V=oWin32.split("~")[1];
var sWin32=oWin32.split("~")[2];
var oM=oData.M;
var sMV=oM.split("~")[1];
var sM=oM.split("~")[2];
var oU=oData.U;
var sUV=oU.split("~")[1];
var sU=oU.split("~")[2];
var oCE=oData.CE;
var sCEV=oCE.split("~")[1];
var sCE=oCE.split("~")[2];

sWin16V=fnCheckVersion(sWin16V);
sWin32V=fnCheckVersion(sWin32V);
sCEV=fnCheckVersion(sCEV);
sMV=fnCheckVersion(sMV);
sUV=fnCheckVersion(sUV);

oNode.setAttribute("ATWIN16V",sWin16V);
oNode.setAttribute("ATWIN16M",sWin16);
oNode.setAttribute("ATWIN32V",sWin32V);
oNode.setAttribute("ATWIN32M",sWin32);
oNode.setAttribute("ATCEV",sCEV);
oNode.setAttribute("ATCEM",sCE);
oNode.setAttribute("ATMV",sMV);
oNode.setAttribute("ATMM",sM);
oNode.setAttribute("ATUV",sUV);
oNode.setAttribute("ATUM",sU);
if((sWin16!="")|(sWin32!="")|(sM!="")|(sU!="")){
oNode.setAttribute("hasPlatformComments",true);
}
else{
oNode.setAttribute("hasPlatformComments",false);
}
}
}
}
fnPostATInit();
}
}
function fnCheckVersion(sString){
if(sString.length>1){
sString=sString.substring(0,1) + "." + sString.substring(1,sString.length);
}
if(sString==""){
sString="N/A";
}
return sString;
}
// Called from fnAcqData, accepts the four platform fields, no return value.
function fnPopulateData(sWin16,sWin32,sMac,sUnix,sCE){
this.WIN16=sWin16;
this.WIN32=sWin32;
this.M=sMac;
this.U=sUnix;
this.CE=sCE;
}

// Queue up the array and ship it off to get unpacked
// Called from onmouseover, no parameters, no return value.
function fnATData(){
var oWorkItem=event.srcElement;
if(oWorkItem.tagName=="A"){
fnUnPack(oWorkItem);
}
}

function fnUnPack(oNode){
if(bShowHelp==true){
bShowHelp=false;
oATHelp.style.display="none";
oPlatData.rows(2).style.display="block";
oPlatData.rows(3).style.display="block";
oPlatData.rows(4).style.display="block";
oPlatData.rows(5).style.display="block";
oPlatData.rows(6).style.display="block";
}
var oWin16=oPlatData.rows(2).cells(1);
var oWin32=oPlatData.rows(3).cells(1);
var oM=oPlatData.rows(4).cells(1);
var oU=oPlatData.rows(5).cells(1);
var oWinCE=oPlatData.rows(6).cells(1);

oWin16.innerText=oNode.ATWIN16V;
oWin32.innerText=oNode.ATWIN32V;
oWinCE.innerText=oNode.ATCEV;
oM.innerText=oNode.ATMV;
oU.innerText=oNode.ATUV;
var sLabelName=oNode.innerText;
if(sLabelName.length>13){
sLabelName=sLabelName.substring(0,13) + "...";
}
oPlatData.rows(0).cells(0).innerText=sLabelName;
oPlatData.rows(0).cells(0).title=oNode.innerText;
if(oNode.hasPlatformComments==true){
oATC.innerHTML="";
if(oNode.ATWIN16M!=""){
oATC.innerHTML+="Win16 Platform: " + oNode.ATWIN16M + "<BR>";
}
if(oNode.ATWIN32M!=""){
oATC.innerHTML+="Win32 Platform: " + oNode.ATWIN32M + "<BR>";
}
if(oNode.ATMM!=""){
oATC.innerHTML+="Macintosh Platform: " + oNode.ATMM + "<BR>";
}
if(oNode.ATUM!=""){
oATC.innerHTML+="Unix Platform: " + oNode.ATUM + "<BR>";
}
oATC.innerHTML+=sGeneralComments + sStyleComments;
}
else{
if((sGeneralComments=="")&&(sStyleComments=="")){
oATC.innerHTML=sHelpText;
}
else{
oATC.innerHTML=sGeneralComments + sStyleComments;
}
}
}


// Add in TDC
// Called from fnATInit, no paramaters, no return value.

function fnInsCSV(){

var sProto=location.protocol.toLowerCase();
var sTempSwitch="/";
if(sProto.indexOf("file")>-1){
sTempSwitch="\\";
}
var sPath=location.pathname.toLowerCase();
var sFileName=sPath.substring(sPath.lastIndexOf(sTempSwitch) + 1,sPath.lastIndexOf(".")) + '_at.csv';
if(sPath.indexOf("/workshop/author/behaviors/library/")>-1){
var sTempPath='"' + sFileName + '"';
}
else{
var sTempPath='"' + sPath.substring(sPath.indexOf(sTempSwitch + "workshop"),sPath.indexOf("reference" + sTempSwitch) + 10) + 'data' + sTempSwitch + 'applies' + sTempSwitch + sFileName + '"';
}

sData='<OBJECT id="oATAcqSource" ondatasetcomplete="fnAcqData(this.recordset);" CLASSID="clsid:333C7BC4-460F-11D0-BC04-0080C7055A83">';
sData+='<PARAM NAME="DataURL" VALUE=' + sTempPath + '>';
sData+='<PARAM NAME="UseHeader" VALUE="TRUE">';
sData+='<PARAM NAME="TextQualifier" VALUE="|">';
sData+='<param name="filter" value="">';
sData+='</OBJECT>';
oATTable.insertAdjacentHTML("beforeBegin",sData);
}