function printHidden(url) {
  document.body.insertAdjacentHTML("beforeEnd",
    "<iframe name=printHiddenFrame width=0 height=0></iframe>");
  var doc = printHiddenFrame.document;
  doc.open();
  doc.write("<body onload=\"setTimeout('parent.onprintHiddenFrame()', 0)\">");
  doc.write("<iframe name=printMe width=0 height=0 src=\"" + url + "\"></iframe>");
  doc.write("</body>");
  doc.close();
}

function onprintHiddenFrame() {
  function onfinish() {
    printHiddenFrame.outerHTML = "";
    if ( window.onprintcomplete ) window.onprintcomplete();
  }
  printFrame(printHiddenFrame.printMe, onfinish);
}
