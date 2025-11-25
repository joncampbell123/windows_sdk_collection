var sFileName = ''; 		// File Name to load in Buffer
var sAnchor ='';		// Glossary entry to select from Buffer;
var eSrc = new Object;		// Event source object.

window.self.document.write('<DIV ID="idToolTip" STYLE="position:absolute; visibility:hidden ; border:1 solid black; width:400px; padding:3px; background-color:lemonchiffon; font:8pt Tahoma; line-height:8pt;"></DIV>');
function initGloss(){
	for (var i=0 ; i < document.all.length; i++) {
	    eTarget = document.all[i];
	    if (eTarget.tagName == "A" && eTarget.getAttribute("href")) {
		if (eTarget.className == "Glossary") {
		    eTarget.onmouseover = TrapEvent;
		    eTarget.onblur = lnkOut;
		}
	    }
	}
}
// Get the event from the <A> link
function TrapEvent(){ eSrc = window.event.srcElement }

// Extract the file name from the string passed to Glossary() function
function getFileName(str){
sFileName = '';
    if (str.indexOf("#") > 0){
    	for (var i = 0 ; i < str.indexOf("#"); i++){
	    sFileName += str.charAt(i);
	}
    }
    else{
    	sFileName = str;
    }
}


// Extract the anchor from the string passed to Glossary() function
function getAnchor(str){
sAnchor = '';
    if (str.indexOf("#") > 0){
    	for (var i = (str.indexOf("#") + 1 ) ; i < str.length; i++){
       	    sAnchor += str.charAt(i);
   	}
	sAnchor = 'GLS_' + sAnchor
    }
}

// Get the word definition from the file in the buffer
function GetFromBuffer(){ 
    // Anchor is valid, get the word definition
    //alert(sAnchor);
    if (sAnchor != '') {
    	if (typeof(eval('document.frames.G_L_S.document.all.' + sAnchor)) != 'undefined') { // Get word definition
	    	    var wordDefinition = eval('document.frames.G_L_S.document.all.' + sAnchor + '.innerText');
		    SetDiv(wordDefinition);
    	}
	else { ShowHelp(sFileName); }
    }
    // Anchor not present
    //else { if (sFileName != '') ShowHelp(sFileName)}
    // NOTE: ShowHelp function is defined in gloss.js
}

// Position the DIV
function SetDiv(str){
    var parent = eSrc;
    var OffSetTop = 0;
    var OffSetLeft = 0;
    // Get the cumulative value for the offset position 
    while (parent.tagName != 'BODY'){
       	if (parent.tagName == 'TBODY') {parent = parent.parentElement; continue; };	// for whatever weird reason  this return a value that needs to be killed.
	OffSetTop += parent.offsetParent.offsetTop;
	OffSetLeft += parent.offsetParent.offsetLeft;
	parent = parent.parentElement;
    }
    OffSetTop += eSrc.offsetTop;
    OffSetLeft += eSrc.offsetLeft;
    
    // If position runs off the screen to the right
    if (((OffSetLeft + 420) > document.body.offsetWidth) && (document.body.offsetWidth > 420 )) {
    	OffSetLeft = document.body.offsetWidth - 420;
    }
    
    with (idToolTip){
	style.visibility = "hidden";
      	innerHTML = '<B>' + eSrc.innerText + '</B><P>' + str;
      	style.left = OffSetLeft;
      	style.top = (OffSetTop + 16);
	style.visibility = "visible"; 
    }
}

// Load glossary file if not loaded or if new file is requested
function lnkOver(path){
    getFileName(path); getAnchor(path);
    if ((document.all.G_L_S.src != sFileName) && (sAnchor != '')) {
	document.all.G_L_S.src = sFileName;
    }
    else {
    	GetFromBuffer();
    }
}

// Display the DIV
function showTip() { with (idToolTip) { style.visibility = "visible";}}

// Hide the DIV
function lnkOut() {with (idToolTip) { style.visibility = "hidden"; style.left = 0; style.bottom = 0; }}