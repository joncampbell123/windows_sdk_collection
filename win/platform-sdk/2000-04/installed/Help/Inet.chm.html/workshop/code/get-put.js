<SCRIPT LANGUAGE="JSCRIPT">
var bToggle_bit=0;
function document_onload(){
   var sURLHash=window.location.href;
   get_section.style.display="none";
   put_section.style.display="none";

   var re = new RegExp("(_get_)","ig");
   var arr = re.exec(sURLHash);
   if( arr != null){
      get_section.style.display="";
      elem_put_onbeforecut.style.color="black";
      elem_get_onbeforecut.style.color="blue";
      bToggle_bit=0;
   }
   else 
   {
      put_section.style.display="";
      elem_put_onbeforecut.style.color="blue";
      elem_get_onbeforecut.style.color="black";
      bToggle_bit=1;
   }
}
function toggle()
{
   if ( bToggle_bit ) {
      get_section.style.display="";
      put_section.style.display="none";
      elem_get_onbeforecut.style.color="blue";
      elem_put_onbeforecut.style.color="black";
      bToggle_bit= 0;
   }
   else {
      get_section.style.display="none";
      elem_put_onbeforecut.style.color="blue";
      elem_get_onbeforecut.style.color="black";
      put_section.style.display="";
      bToggle_bit= 1;
   }
}

function showall(){
   get_section.style.display="";
   put_section.style.display="";
   elem_get_onbeforecut.style.color="blue";
   elem_put_onbeforecut.style.color="blue";
   bToggle_bit= 0;
}
</SCRIPT>

