//

var FLAG_SAMP1_NotStarted = true;
var FLAG_CODE1_NotVisible = true;
var FLAG_SAMP2_NotStarted = true;
var FLAG_CODE2_NotVisible = true;
var FLAG_SAMP3_NotStarted = true;
var FLAG_CODE3_NotVisible = true;
var FLAG_SAMP4_NotStarted = true;
var FLAG_CODE4_NotVisible = true;
var FLAG_SAMP5_NotStarted = true;
var FLAG_CODE5_NotVisible = true;

//

function makeControl(num) {
	var DA_OBJ =  '<OBJECT ID="DAControl_' + num + '" STYLE="width:455; height:300; z-index:-1" \n' +
  			' CLASSID="CLSID:B6FFC24C-7E13-11D0-9B47-00C04FC2F51D"> \n' +
			'</OBJECT>';
	return DA_OBJ;
}

function dispControl(num){
	//
		
	if (num == 1) if (FLAG_SAMP1_NotStarted) divSAMP1.innerHTML = makeControl(num);
	if (num == 2) if (FLAG_SAMP2_NotStarted) divSAMP2.innerHTML = makeControl(num);
	if (num == 3) if (FLAG_SAMP3_NotStarted) divSAMP3.innerHTML = makeControl(num);
	if (num == 4) if (FLAG_SAMP4_NotStarted) divSAMP4.innerHTML = makeControl(num);
	if (num == 5) if (FLAG_SAMP5_NotStarted) divSAMP5.innerHTML = makeControl(num);
}
//

function samp(num){
    if ((g_isIE) && (g_iMaj >= 4)){
	if (num == 1) if (FLAG_SAMP1_NotStarted) {dispControl(num); runSamp(num); onSamp(num)} else {offSamp(num)}
	if (num == 2) if (FLAG_SAMP2_NotStarted) {dispControl(num); runSamp(num); onSamp(num)} else {offSamp(num)}
	if (num == 3) if (FLAG_SAMP3_NotStarted) {dispControl(num); runSamp(num); onSamp(num)} else {offSamp(num)}
	if (num == 4) if (FLAG_SAMP4_NotStarted) {dispControl(num); runSamp(num); onSamp(num)} else {offSamp(num)}
	if (num == 5) if (FLAG_SAMP5_NotStarted) {dispControl(num); runSamp(num); onSamp(num)} else {offSamp(num)}
    } else{alert("Internet Explorer version 4.0 and above required!")}
}
//

function runSamp(num){
	if (num == 1) SAMP1();
	if (num == 2) SAMP2();
	if (num == 3) SAMP3();
	if (num == 4) SAMP4();
	if (num == 5) SAMP5();
}

//
 
function onSamp(num){
	if (num == 1) {FLAG_SAMP1_NotStarted = false ; btnSAMP1.value="Hide Sample" }
	if (num == 2) {FLAG_SAMP2_NotStarted = false ; btnSAMP2.value="Hide Sample" }
	if (num == 3) {FLAG_SAMP3_NotStarted = false ; btnSAMP3.value="Hide Sample" }
	if (num == 4) {FLAG_SAMP4_NotStarted = false ; btnSAMP4.value="Hide Sample" }
	if (num == 5) {FLAG_SAMP5_NotStarted = false ; btnSAMP5.value="Hide Sample" }
}
//
 
function offSamp(num){
	if (num == 1) {FLAG_SAMP1_NotStarted = true ; divSAMP1.innerHTML = '' ; btnSAMP1.value="Show Sample"}
	if (num == 2) {FLAG_SAMP2_NotStarted = true ; divSAMP2.innerHTML = '' ; btnSAMP2.value="Show Sample"}
	if (num == 3) {FLAG_SAMP3_NotStarted = true ; divSAMP3.innerHTML = '' ; btnSAMP3.value="Show Sample"}
	if (num == 4) {FLAG_SAMP4_NotStarted = true ; divSAMP4.innerHTML = '' ; btnSAMP4.value="Show Sample"}
	if (num == 5) {FLAG_SAMP5_NotStarted = true ; divSAMP5.innerHTML = '' ; btnSAMP5.value="Show Sample"}
}

function code(num){
    if ((g_isIE) && (g_iMaj >= 4)){
	if (num == 1) if (FLAG_CODE1_NotVisible) {onCode(num)} else {offCode(num)}
	if (num == 2) if (FLAG_CODE2_NotVisible) {onCode(num)} else {offCode(num)}
	if (num == 3) if (FLAG_CODE3_NotVisible) {onCode(num)} else {offCode(num)}
	if (num == 4) if (FLAG_CODE4_NotVisible) {onCode(num)} else {offCode(num)}
	if (num == 5) if (FLAG_CODE5_NotVisible) {onCode(num)} else {offCode(num)}
    }else{alert("Internet Explorer version 4.0 and above required!")}
}
//

function onCode(num){
	if (num == 1) {divCODE1.innerText = makeControl(num) + '\n' + SAMP1_SCRIPT.innerHTML; FLAG_CODE1_NotVisible = false ; btnCODE1.value = "Hide Sample Code"}
	if (num == 2) {divCODE2.innerText = makeControl(num) + '\n' + SAMP2_SCRIPT.innerHTML; FLAG_CODE2_NotVisible = false ; btnCODE2.value = "Hide Sample Code"}
	if (num == 3) {divCODE3.innerText = makeControl(num) + '\n' + SAMP3_SCRIPT.innerHTML; FLAG_CODE3_NotVisible = false ; btnCODE3.value = "Hide Sample Code"}
	if (num == 4) {divCODE4.innerText = makeControl(num) + '\n' + SAMP4_SCRIPT.innerHTML; FLAG_CODE4_NotVisible = false ; btnCODE4.value = "Hide Sample Code"}
	if (num == 5) {divCODE5.innerText = makeControl(num) + '\n' + SAMP5_SCRIPT.innerHTML; FLAG_CODE5_NotVisible = false ; btnCODE5.value = "Hide Sample Code"}
}
//

function offCode(num){
	if (num == 1) {divCODE1.innerText = '' ; FLAG_CODE1_NotVisible = true ; btnCODE1.value = "Show Sample Code"}
	if (num == 2) {divCODE2.innerText = '' ; FLAG_CODE2_NotVisible = true ; btnCODE2.value = "Show Sample Code"}
	if (num == 3) {divCODE3.innerText = '' ; FLAG_CODE3_NotVisible = true ; btnCODE3.value = "Show Sample Code"}
	if (num == 4) {divCODE4.innerText = '' ; FLAG_CODE4_NotVisible = true ; btnCODE4.value = "Show Sample Code"}
	if (num == 5) {divCODE4.innerText = '' ; FLAG_CODE5_NotVisible = true ; btnCODE5.value = "Show Sample Code"}
}