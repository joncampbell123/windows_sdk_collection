
/****************************************************************************************************************
FUNCTION: NewSample()
DESC: Adds individual sample data to arSamples collection.
PARAM: 
sDAObjID,string, unique ID for the DA Control.
nWidth,int, DA Control width.
nHeight,int, DA Control height.
oSampID,object, container object ID: this is where the DA control will be innerHTML-ed for sample display.
oCodeID,object, container object ID: this is where the DA Source code is innerText-ed for code display.
oBtnSampID,object, container object ID: this is the button calling show/hide sample.
oBtnCodeID,object, container object ID: this is thew button calling the show/hide code.
sBtnSampValueOn,string, sample button value shifts to this string when the sample is playing.
sBtnCodeValueOn,string, code button value shifts to this value when the source code is displayed.
oDACodeID,object, container object ID: this is where the DA source code shown to the user resides.
sAction,string, this string is evaluated to executable code when the show sample button is clicked. Very usefull when you need to start a sample (most DA samples need this).

The initial value of the button value is saved, that is the sample/code off button value.
Added IDX (required) new custom attribute for Sample and Code buttons.
Different from the old model.
Now you have to call the NewSample() function to add the sample to the arSamples collection. Yes NewSample() does
have a long list of parameters but you gained more control in the overall.

EXAMPLE:


<!-- this button (for onclick event) will display the DA sample -->
<p><INPUT IDX TYPE="button" VALUE="Show Sample" STYLE="width:150" ID="btnSAMP1" OnClick="SHSamp(this)"> 
<!-- This span will host the DA control -->
<DIV align="center" ID="hostSAMP1" CLASS="clsSample">&nbsp;</DIV>
<!-- this button (for onclick event) will display the Source code -->
<INPUT IDX TYPE="button" VALUE="Show Code" STYLE="width:150" ID="btnCODE1" OnClick="SHCode(this)">
<!-- This span will host the source code -->
<DIV ID="hostCODE1" CLASS="clsSampleCode"></DIV>
</P>

<!--Display the inner HTML of this <SPAN> as source code along with the DA control --> 
<SPAN ID="codeSAMP1">
<SCRIPT LANGUAGE="JSCRIPT">
function SAMP1(){
   // The DirectAnimation library
   m = DAControl_1.MeterLibrary;

   stringImg1 = m.StringImageAnim(m.LocalTime.ToString(3),
m.defaultfont.size(60).color(m.Red));

   stringImg2 = m.StringImageAnim(m.LocalTime.ToString(3),
m.defaultfont.size(60).color(m.Green));

   changingString = m.Until(stringImg1, m.LeftButtonDown, stringImg2);

   //Assign the animation back to the control and start it
   DAControl_1.Image = m.Overlay(changingString, m.SolidColorImage(m.White));
   DAControl_1.Start();
}
</SCRIPT></SPAN>

<!-- Allways add the sample to the collection after you created the sample invironment --> 
<SCRIPT LANGUAGE="JSCRIPT">
NewSample("DAControl_1", 300, 300, hostSAMP1, hostCODE1, btnCODE1, btnSAMP1, "Hide Sample", "Hide Code", codeSAMP1,"SAMP1()");
</SCRIPT>

TEMPLATE:

<p><INPUT IDX TYPE="button" VALUE="Show Sample" STYLE="width:150" ID="btnSAMP1" OnClick="SHSamp(this)"> 
<DIV align="center" ID="hostSAMP1">&nbsp;</DIV>
<INPUT IDX TYPE="button" VALUE="Show Code" STYLE="width:150" ID="btnCODE1" OnClick="SHCode(this)">
<DIV ID="hostCODE1">&nbsp;</DIV>
</P>

<SPAN ID="codeSAMP1">
<SCRIPT LANGUAGE="JSCRIPT">
function SAMP1(){
   //Insert DA Code here 

}
</SCRIPT>
</SPAN>
<SCRIPT LANGUAGE="JSCRIPT">
NewSample("DAControl_1", 300, 300, hostSAMP1, hostCODE1, btnCODE1, btnSAMP1, "Hide Sample", "Hide Code", codeSAMP1,"SAMP1()");
</SCRIPT>



*****************************************************************************************************************/ 



var arSamples = new Array;

function SampleObject(sDAObj, nWidth, nHeight, oDACodeID, oSampID, oCodeID, sBtnSampValueOn, sBtnCodeValueOn, sBtnSampValueOff, sBtnCodeValueOff, bSampOn, bCodeOn, sAction){
this.sDAObj = sDAObj;
this.nWidth = nWidth;
this.nHeight = nHeight;
this.oDACodeID= oDACodeID;
this.oSampID = oSampID;
this.oCodeID= oCodeID;
this.sBtnSampValueOn= sBtnSampValueOn;
this.sBtnCodeValueOn= sBtnCodeValueOn;
this.sBtnSampValueOff= sBtnSampValueOff;
this.sBtnCodeValueOff= sBtnCodeValueOff;
this.bSampOn = bSampOn;
this.bCodeOn = bCodeOn;
this.sAction = sAction;
}

function NewSample(sDAObjID, nWidth, nHeight, oSampID, oCodeID, oBtnSampID, oBtnCodeID, sBtnSampValueOn, sBtnCodeValueOn, oDACodeID, sAction){
var oSamp = new SampleObject;

oSamp.sDAObj = StrDAObj(sDAObjID, nWidth, nHeight);
oSamp.oDACodeID = oDACodeID;
oSamp.oSampID = oSampID;
oSamp.oCodeID = oCodeID;
oSamp.sBtnSampValueOn= sBtnSampValueOn;
oSamp.sBtnCodeValueOn= sBtnCodeValueOn;
oSamp.sBtnSampValueOff= oBtnSampID.value;
oSamp.sBtnCodeValueOff= oBtnCodeID.value;
oSamp.bSampOn= false;
oSamp.bCodeOn= false;
oSamp.sAction= sAction;

oBtnSampID.IDX = arSamples.length;
oBtnCodeID.IDX = arSamples.length;
arSamples[arSamples.length] = oSamp;
}

//Create string for DA Control with unique ID, Width and height
function StrDAObj(sDAObjID, nWidth, nHeight) {
var DA_OBJ =  '<OBJECT ID="' + sDAObjID + '" STYLE="width:' + nWidth + '; height:' + nHeight + '; z-index:-1" \n' +
  ' CLASSID="CLSID:B6FFC24C-7E13-11D0-9B47-00C04FC2F51D">\n' +
'</OBJECT>';
return FixEOL(DA_OBJ);
}

//Show/Hide Sample Code
function SHSamp(oObjID){
var IDX = oObjID.IDX;

//If visible, hide sample
if(arSamples[IDX].bSampOn){
//Set button value for sample to Off state
oObjID.value = arSamples[IDX].sBtnSampValueOff;
//Remove DA Object from sample container 
arSamples[IDX].oSampID.innerHTML = "";
//Set display flag to on
arSamples[IDX].bSampOn = false;
}
//Display sample
else{
//Save current button value
arSamples[IDX].sBtnSampValueOff = oObjID.value;
//Set button value for sample to On state
oObjID.value = arSamples[IDX].sBtnSampValueOn;
//Insert DA Object in sample container
arSamples[IDX].oSampID.innerHTML = arSamples[IDX].sDAObj;
//Set display flag to on
arSamples[IDX].bSampOn = true;
//run sample 
eval(arSamples[IDX].sAction);
}
}

//Show/Hide Sample Code
function SHCode(oObjID){ 
var IDX = oObjID.IDX;

//If visible, hide sample code
if(arSamples[IDX].bCodeOn){
//Set button value for sample to Off state
oObjID.value = arSamples[IDX].sBtnCodeValueOff;
//Remove DA Object from sample container 
arSamples[IDX].oCodeID.innerText= "";
//Change background color to none
arSamples[IDX].oCodeID.style.background = "";
//Set display flag to on
arSamples[IDX].bCodeOn = false;
}
//Display sample
else{
//Save current button value
arSamples[IDX].sBtnCodeValueOff = oObjID.value;
//Set button value for sample to On state
oObjID.value = arSamples[IDX].sBtnCodeValueOn;
//Insert DA Object Code in the sample code container
arSamples[IDX].oCodeID.innerText = arSamples[IDX].sDAObj + FixEOL(arSamples[IDX].oDACodeID.innerHTML) ;
//Change background color to light gray
arSamples[IDX].oCodeID.style.background = "#EEEEEE";
//Set display flag to on
arSamples[IDX].bCodeOn = true;
}
}

function FixEOL(str){
var newStr = "";
var strLen = str.length;
for(i=0; i<strLen; i++)
str.charAt(i) == "\n" ? (newStr += str.charAt(i) + " ") : (newStr += str.charAt(i));
 return newStr;
}