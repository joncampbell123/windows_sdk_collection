//This is the master JavaScript file for the HTML Help documentation.

/* These functions (doSection, noSection) are used to make sidebars appear and disappear.
*/

function doSection (secNum){
//display the section if it's not displayed; hide it if it is displayed
if (secNum.style.display=="none"){secNum.style.display=""}
else{secNum.style.display="none"}
}

function noSection (secNum){
//remove the section when user clicks in the opened DIV
if (secNum.style.display==""){secNum.style.display="none"}
}

function doExpand(paraNum,arrowNum){
//expand the paragraph and rotate the arrow; collapse and rotate it back
if (paraNum.style.display=="none"){paraNum.style.display="";arrowNum.src="arrowdn2.gif"}
else{paraNum.style.display="none";arrowNum.src="arrowrt2.gif"}
}

//Insert new functions here. Please use unique identifiers and comment liberally.