/*This file uses portions of ActiveX™ Data Objects (ADO) made available
through a Tabular Data Control (TDC).*/

var sel_id="";
var sel_id2="";
var sData="";
var bData=false;
var bValidArray=false;
var arrRecordSet="";
var arrData=new Array();


/* InitWN initializes the dynamic What's New Table */

function InitWN(){
objTabDat.innerHTML="";
insControls();
insCSV();
arrRecordSet=acqsource.recordset;
}
// Insert the CSV


/* Add the HTML to create the visual controls for manipulating the table */
function insControls(){
oCon="The following table highlights the new features of Internet Explorer 5.<p>";
oCon+='<form name="dBindForm"><table border=0 cellpadding=0 cellspacing=0>';

//Remove comment to include button that enables CSV regeneration for down-level story.
//Some fields will need to be stripped from regenerated data, including DIVs.
//location.protocol is automatically included in the links, so will need be removed as well.
//oCon+='<tr><td><input style="background-color: #EFEFEF;" type=button value="Make Static" onclick="stattabsrc()"></td></tr>';

oCon+='<tr><td><label for="acqList">Organize What\'s New By This Option:</label></td></tr>';
oCon+='<tr><td><SELECT name="acqList" onChange="switchfilter()" class="lOptions">';
oCon+='<option value="none">Show All';
oCon+='</select></td></tr>';
oCon+='<tr><td><label for="acqName">Only Show This New Feature:</label></td></tr>';
oCon+='<tr><td><span id=popAcqName class="lOptions"><select name="acqName" onChange="switchfilter()">';
oCon+='<option value="none">Show All';
oCon+='</select>';
oCon+='</span></td></tr></table>';
oCon+='</form><p>';
objControls.innerHTML=oCon;
}

/* Add the data source object that provides the connection to the whatsnewdata.csv file */
function insCSV(){
sData='<OBJECT id="acqsource" ondatasetcomplete="arrPopulate(this.recordset);" CLASSID="clsid:333C7BC4-460F-11D0-BC04-0080C7055A83">';
sData+='<PARAM NAME="DataURL" VALUE="whatsnewdata.csv">';
sData+='<PARAM NAME="UseHeader" VALUE="True">';
sData+='<PARAM NAME="TextQualifier" VALUE="|">';
sData+='<param name="filter" value="">';
sData+='</OBJECT>';
document.body.insertAdjacentHTML("afterBegin",sData);
bData=true;
}

// debug function -- do not publish

function stattabsrc(){
tabdat="";
tabdat='<table class="clsStd">';
for(var i=0;i<datTable.rows.length;i++){
tabdat+='\n<tr><th>' + datTable.rows[i].cells[0].innerText + '</th>\n<td>' + datTable.rows[i].cells[1].innerHTML + '</td></tr>';
}
tabdat+='\n</table>';
objTabDat.innerText=tabdat;
}

// Populate an array with names and IDs for convenience with dynamic menus

function arrPopulate(oPopulous){
if(bValidArray==false){
var sNameTemp="";
var sIDTemp="";
for (var i=0;i < oPopulous.RecordCount;i++){
// Must add the empty value to convert it to a string.
sNameTemp=oPopulous.fields.item(0) + "";
sIDTemp=oPopulous.fields.item(1) + "";
arrData[arrData.length]=new popArray(sNameTemp,sIDTemp);
// Populate the That Displays menu now, but the Filter By menu, ironically, has to be filtered.
dBindForm.acqName.options[dBindForm.acqName.options.length]=new Option(sNameTemp,sIDTemp);
oPopulous.MoveNext();
}
bValidArray=true;
}
popSelection();
insTable(1);
}

function popArray(sName,sID){
this.name=sName;
this.id=sID;
}
function popSelection(){
// Populate the Data Types
for (var h=0;h<arrData.length;h++){
bData=false;
for (var h2=0;h2<dBindForm.acqList.options.length;h2++){
if(arrData[h].id==dBindForm.acqList.options[h2].text){
bData=true;
}
}
if(bData==false){
dBindForm.acqList.options[dBindForm.acqList.options.length]=new Option(arrData[h].id,"");
}
}

dBindForm.acqName.selectedIndex=0;
}
function insTable(iShowTable){
if(iShowTable==1){
sTable='<TABLE id=datTable DATASRC=#acqsource class="clsStd">';
sTable+='<tr><th><span DATAFORMATAS=html DATAFLD=tech_name></span><p><span dataformatas=html datafld=tech_image></span></th>';
sTable+='<TD><div DATAFORMATAS=html DATAFLD=new_val_desc></div></TD></TR>';
sTable+='</TABLE>';
objTabDat.innerHTML=sTable;
objTabDat.style.display="block";
bLoadDat=true;
}
}

function switchfilter(){
if(sel_id!=dBindForm.acqList.options[dBindForm.acqList.selectedIndex].text){
sel_id=dBindForm.acqList.options[dBindForm.acqList.selectedIndex].text;
dBindForm.acqName.selectedIndex=0;
}
sel_id2=dBindForm.acqName.options[dBindForm.acqName.selectedIndex].text;
var new_filter=MakeFilter(sel_id,sel_id2);
acqsource.filter=new_filter;
acqsource.reset();
insNewOptions(sel_id2);
}
function MakeFilter(inVal1,inVal2){
var outVal="";
var multiParse=false;
if(inVal1!= "Show All"){
outVal='tech_type=' + (inVal1);
multiParse=true;
}
if(inVal2 != "Show All"){
if(multiParse==true){
outVal+=" & ";
}
outVal+='tech_name=' + (inVal2);
}
return outVal;
}

//Adjust the second menu based on the selection in the first menu.
//Preserve the user selections.
function insNewOptions(fix_id){
if(dBindForm.acqList.options[dBindForm.acqList.selectedIndex].value=="none"){
dBindForm.acqName.options.length=1;
for(var n=0;n<arrData.length;n++){
dBindForm.acqName.options[dBindForm.acqName.options.length]=new Option(arrData[n].name,arrData[n].id);
if(arrData[n].name==fix_id){
dBindForm.acqName.selectedIndex=dBindForm.acqName.options.length -1;
}
}
}
else{
dBindForm.acqName.options.length=1;
for(var m=0;m<arrData.length;m++){
if(dBindForm.acqList.options[dBindForm.acqList.selectedIndex].text==arrData[m].id){
dBindForm.acqName.options[dBindForm.acqName.options.length]=new Option(arrData[m].name,arrData[m].id);
}
if(arrData[m].name==fix_id){
dBindForm.acqName.selectedIndex=dBindForm.acqName.options.length -1;
}
}
}
}