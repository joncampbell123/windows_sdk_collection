function ShowDI(doc,xmlid,xslid)
{
    var w = window.open("","","resizable,scrollbars,width=640,height=480");
    w.title = "XML Data Island";
    w.document.open();
    w.document.write("<STYLE>");
w.document.write("  .html    {color:blue; font-weight:bold; font-size:9pt;font-family:Arial}");
    w.document.write("  .xml    {font-size:9pt;font-family:Arial}");
    w.document.write("  .tag    {color:red; font-weight:bold; }");
    w.document.write("  .attribute{color:blue; font-weight:bold; }");
    w.document.write("  .attrvalue{color:darkorchid; font-weight:bold; }");
    w.document.write("  .comment{color:green; font-weight:bold; }");
    w.document.write("</STYLE>");
w.document.write("<DL class=html>&lt;HTML&gt;</DL>");
w.document.write("<DL class=html>&lt;HEAD&gt;</DL>");
w.document.write("<DL class=html>&lt;TITLE&gt;HTML Document with Data Island&lt;/TITLE&gt;</DL>");
w.document.write("<DL class=html>&lt;/HEAD&gt;</DL>");
    w.document.write("<DL class=html>&lt;BODY&gt;</DL>");
w.document.write("<DL class=xml><DD class=tag>&lt;XML ID='" + xmlid + "'&gt;</DD></DL>");
var xml = doc.documentElement.transformNode(xslid.documentElement);
w.document.write(xml);
w.document.write("<DL class=xml><DD class=tag>&lt;/XML&gt;</DD></DL>");
w.document.write("<DL class=html>&lt;/BODY&gt;</DL>");
w.document.write("<DL class=html>&lt;/HTML&gt;</DL>");
    w.document.close();
}

function ShowXML(doc)
{
    var w = window.open("","","resizable,scrollbars,width=640,height=480");
    w.title = "XML Data";
    w.document.open();
    w.document.write("<STYLE>");
    w.document.write("  .xml    {font-size:9pt;font-family:Arial}");
    w.document.write("  .tag    {color:red; font-weight:bold; }");
    w.document.write("  .attribute{color:blue; font-weight:bold; }");
    w.document.write("  .attrvalue{color:darkorchid; font-weight:bold; }");
    w.document.write("  .comment{color:green; font-weight:bold; }");
    w.document.write("</STYLE>");
    var xml = DumpTree(doc,0);
    w.document.write(xml);
    w.document.close();
}

function qname(node)
{
    var ns = node.nameSpace;
    if (ns == null)
        return node.nodeName;

    var prefix = ns.getAttribute("prefix");
    return prefix + ":" + node.nodeName;
}

function aqname(node, attr)
{
    var ns2 = attr.nameSpace;
    if (ns2 == null)
        return attr.nodeName;

    var ns = node.nameSpace;
    if (ns && (ns.getAttribute("ns") == ns2.getAttribute("ns")))
        return attr.nodeName;

    var prefix;
    prefix = ns2.getAttribute("prefix");

    return prefix + ":" + node.nodeName;
}

function DumpTree(node,i,ns)
{
    if (node == null) return "";

    var type = node.nodeType;
result = "<DL class=xml><DD>";

if (type == 1 || type == 6 || type == 8 )
{
result += node.nodeValue;
result += "</DD></DL>";
return result;
}
if (type == 2) {
result += "<span class=comment>&lt;!--" 
+ node.nodeValue + "--&gt;</span>";
result += "</DD></DL>"
return result;
}
if (type == 3)
{
var en = node.childNodes;
for (var child = en.currentNode(); child != null; child = en.nextNode())
{
if (child.nodeType != 9)
result += DumpTree(child,i);
}
result += "</DD></DL>"
return result;
}
if (type == 5)
{
result += "<span class=pi>&lt;?" + node.nodeName + " "
+ node.nodeValue + "?&gt;</span>";
result += "</DD></DL>"
return result;
}
    if (type == 4)
    {
        result += "<span class=decl>&lt;!DOCTYPE " + qname(node);
        var pubid = node.getAttribute("publicid");
        var sys = node.getAttribute("SYSTEM");
        if (pubid != "" && pubid != null)
            result += " PUBLIC " + pubid;
        else if (sys != "" && sys != null)
            result += " SYSTEM";
        if (sys != "" && sys != null) 
            result += " " + sys;

        var value = ""; // node.nodeValue;
        if (value != "")
        {
            result += " [" + value + "]";
        }
        result += "&gt;</span></DD></DL>";
        return result;
    }

var num = node.childNodes.length;
    var children = node.childNodes;
    var num = children.length;
    var empty = "";
    if (num == 0) 
    {
        empty = "/";
        if (type == 11 || type == 7)
        {
            empty = "?";
        }
    }
    if (type != 3)
    {
        result += "<span class=tag>&lt;";
        if (type == 11 || type == 7) result += "?"
        result = result + qname(node) + "</span>";
    }

    if (type == 0 || type == 7 || type == 11)
    {
        var attrs = node.attributes;
        var na = attrs.length;
        for (i = 0; i < na; i++) {
            var a = attrs.item(i);
            result += " <span class=attribute>" + aqname(node,a) + "</span>=<span class=attrvalue>\"" + a.nodeValue + "\"</span>";
        }
    }
    if (type != 3)
        result += "<span class=tag>" + empty + "&gt;</span>";

    if (num > 0) {
        for (var child = children.currentNode(); child != null; child = children.nextNode())
        {
            result += "\n";
            if (child.nodeType != 9)
                result += DumpTree(child,i+1);
        }
        if (type != 3)
            result +=  "<span class=tag>&lt;/" + qname(node) + "&gt;</span>\n";
    }
    result += "</DD></DL>"
    return result;
}

