
<?xml version="1.0"?>

<!--
Workarounds and hacks (5.0.708.600)
1) Using xsl:attribute until {} is implemented
2) BUGBUG: Can't get xsl:doctype to work.
3) Purposely avoiding merge-time script (xsl:eval, xsl:script)
4) FIXED: Using entities (&lt;, &amp;) for SCRIPT until CDATA is supported.
5) Since transformation outputs XML, non-container tags (e.g. META, IMG) contain a trailing slash. Acc. PM this is By Design.
6) Can we combine blocks of commonly used templates and call them through a function?
Bugs that need to be resolved:
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">

	<xsl:template match="/">
<!--	
	Insert the standard DOCTYPE element
	Per PM, xsl:doctype has been pushed
	<xsl:doctype HTML PUBLIC="-//IETF//DTD HTML//EN" />
-->
	<xsl:eval no-entities="1">'&lt;!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN"&gt;'</xsl:eval>
   <HTML>
   <HEAD>
   <TITLE>SBN Workshop - <xsl:value-of select="TOC/TOCINFO/TITLE" /> Topic Listing</TITLE>
   <META NAME="robots" CONTENT="noindex" />
   <META HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=iso-8859-1" />

   <SCRIPT>
   //<xsl:comment><![CDATA[
	var sUA = window.navigator.userAgent;
	var bIsIE4 = -1 != sUA.indexOf("MSIE 4"); 
	var bIsIE5 = -1 != sUA.indexOf("MSIE 5"); 
	var bIsOpera = -1 != sUA.indexOf("Opera");
	var bIsMac = sUA.indexOf("Mac") > -1;
	var bDoesAll = (bIsIE4 || bIsIE5) && !bIsMac;

	var sSheet = "/sitebuilder/shared/css/";
	if (bDoesAll || bIsOpera || bIsMac) 
	{
		if (bIsMac || bIsOpera) sSheet += "ie4Mac-toc.css";
		else sSheet += "ie4-toc.css";
	}
	else 
	{
		sSheet += "all-toc.css";
	}
	document.write('<LINK REL="stylesheet" TYPE="text/css" HREF="' + sSheet + '">');

   function newItem(sDatePosted) {
	  // have 7 days elapsed since post?
      if (Date.parse(Date()) < Date.parse(new Date(sDatePosted)) + 604800000)	{
         document.write("<IMG SRC='/sitebuilder/graphics/new1.gif' ALIGN='middle' WIDTH='19' HEIGHT='9' BORDER='0' ALT='New'>");
      }
   }

   function updatedItem(sDateUpdated) {
	  // have 7 days elapsed since update?
      if (Date.parse(Date()) < Date.parse(new Date(sDateUpdated)) + 604800000)	{
         document.write("<IMG SRC='/sitebuilder/graphics/rev1.gif' WIDTH='19' HEIGHT='9' BORDER='0' ALT='Updated'>");
      }
   }

   // do nothing but enable items to receive focus
   function NoOp()
   {
      return;
   }   

   //]]></xsl:comment>
   </SCRIPT>

   </HEAD>
   <BODY BGCOLOR="white" TOPMARGIN="0" LEFTMARGIN="0" MARGINHEIGHT="0" MARGINWIDTH="0" LINK="#000066" VLINK="#666666" TEXT="#000000">

   <TABLE CELLPADDING="5" WIDTH="100%">
   <TR>
      <TD ALIGN="center"><A ID="lnkToWorkshop" HREF="/workshop/toc.htm"><IMG ALIGN="top" ID="imgUp" HEIGHT="13" WIDTH="17" BORDER="0" TITLE="Workshop TOC" SRC="/workshop/graphics/icons/nl-up-0.gif" /><B>workshop toc</B></A></TD>
   </TR>
   </TABLE>

   <TABLE CELLPADDING="7" CELLSPACING="0" WIDTH="100%">
   <TR>
      <TD ALIGN="left" VALIGN="middle" BGCOLOR="#99CCFF"><A ID="lnkBucketTitle" expResetToc="true"><xsl:attribute name="HREF"><xsl:value-of select="TOC/TOCINFO/HREF"/></xsl:attribute><IMG ID="imgBucketIcon" HEIGHT="32" WIDTH="31" BORDER="0"><xsl:attribute name="SRC"><xsl:value-of select="TOC/TOCINFO/ICON"/></xsl:attribute><xsl:attribute name="ALT"><xsl:value-of select="TOC/TOCINFO/TITLE"/></xsl:attribute></IMG></A></TD>
      <TD ALIGN="left" VALIGN="middle" WIDTH="100%" BGCOLOR="#99CCFF"><A ID="lnkBucketTitle" expResetToc="true"><xsl:attribute name="HREF"><xsl:value-of select="TOC/TOCINFO/HREF"/></xsl:attribute><B><xsl:value-of select="TOC/TOCINFO/TITLE" /> TOC</B></A></TD>
   </TR>
   </TABLE>


   <FONT SIZE="1" FACE="Verdana,Arial,Helvetica">
   <UL STYLE="behavior:url('/sitebuilder/shared/htc/tocstate.htc')">
   <xsl:for-each select="TOC/SECTIONS/SECTION[not(@OBSOLETE)]">

	 <xsl:if match="SECTION[ARTICLES/ARTICLE[not(@OBSOLETE)]/HREF $or$ ARTICLES/ARTICLE/VERSION[end() $and$ not(@OBSOLETE)]]">
		  <xsl:apply-templates select="SECTIONINFO" />
		  <xsl:for-each select="ARTICLES">
			<xsl:apply-templates select="." />
		  </xsl:for-each>
	 </xsl:if>

	 <xsl:if match="SECTION[SECTIONINFO/HREF $or$ SECTIONINFO/VERSION]">
		  <xsl:apply-templates select="SECTIONINFO" />
	 </xsl:if>

   </xsl:for-each>
   </UL>
   </FONT>

   <SCRIPT>
   //<xsl:comment><![CDATA[
   if (bDoesAll) {
      document.write("<SCR" + "IPT LANGUAGE='javascript' SRC='/sitebuilder/shared/js/toc.js'></SCR" + "IPT>");
   }
   //]]></xsl:comment>
   </SCRIPT>
   </BODY>
   </HTML>

   </xsl:template>


	<xsl:template match="ARTICLES[ARTICLE[HREF $and$ not(@OBSOLETE)] | ARTICLE/VERSION[end() $and$ not(@OBSOLETE)]]">
         <UL CLASS="clsItemsHide">

         <xsl:for-each select="ARTICLE[HREF $and$ not(@OBSOLETE)] | ARTICLE/VERSION[end() $and$ not(@OBSOLETE)]">

         <!-- Process all ARTICLEs with VERSIONs as immediate children -->
		 <!-- Process all ARTICLEs with HREFs as immediate children -->

            <LI><A CLASS="clsTOCItem"><xsl:choose>
            <xsl:when match="*[TARGET]">
              <xsl:attribute name="TARGET"><xsl:value-of select="TARGET" /></xsl:attribute>
            </xsl:when>
            <xsl:when match="*[LINKTYPE]">
              <xsl:attribute name="TARGET">_top</xsl:attribute>
            </xsl:when>
           </xsl:choose>

           <xsl:if match="*[ABSTRACT]">
              <xsl:attribute name="TITLE"><xsl:value-of select="ABSTRACT" /></xsl:attribute>
           </xsl:if>

           <xsl:attribute name="HREF"><xsl:value-of select="HREF"/></xsl:attribute><xsl:value-of select="TITLE"/></A>
           <xsl:apply-templates select="LINKTYPE" />
           <xsl:apply-templates select="UPDATED" />
           <xsl:apply-templates select="POSTED" />
		   </LI>
        </xsl:for-each> <!-- ARTICLE | ARTICLES/VERSION -->

         </UL>		
	</xsl:template>

  <!-- Process all vanilla SECTIONINFOs which are siblings of ARTICLES collections -->
  <xsl:template match="SECTIONINFO">
     <LI CLASS="clsShowHide"><A HREF="javascript:NoOp()" CLASS="clsTOCHeading"><xsl:value-of select="TITLE"/></A>
	 <xsl:apply-templates select="UPDATED" />
	 <xsl:apply-templates select="POSTED" />
     </LI>
  </xsl:template>

	  <!-- BUGBUG: Need a clean way to set expNewToc correctly -->
      <!-- In 5.0.708.600 this works correctly, but it feels 'hackish' -->
	  <!-- Process all SECTIONINFOs with HREF subelements -->
      <xsl:template match="SECTIONINFO[HREF]">
         <LI CLASS="noexpand"><A CLASS="clsTOCHeading">
		 <xsl:attribute name="HREF"><xsl:value-of select="HREF"/></xsl:attribute>

		<xsl:choose>
		   <xsl:when match="*[TARGET]">
		      <xsl:attribute name="TARGET"><xsl:value-of select="TARGET" /></xsl:attribute>
		   </xsl:when>
		   <xsl:when match="*[LINKTYPE]">
		      <xsl:attribute name="TARGET">_top</xsl:attribute>
		   </xsl:when>
		</xsl:choose>

		 <xsl:value-of select="TITLE"/></A>
		 <xsl:apply-templates select="LINKTYPE" />
		 <xsl:apply-templates select="UPDATED" />
		 <xsl:apply-templates select="POSTED" />
         </LI>
      </xsl:template>

	  <!-- Process all SECTIONINFOs with NEWTOC attributes -->
	  <!-- Don't generate a TARGET for these although a LINKTYPE might be interesting -->
      <xsl:template match="SECTIONINFO[@NEWTOC]">
         <LI CLASS="noexpand"><A expNewToc="true" CLASS="clsTOCHeading">

		<xsl:if match="*[ABSTRACT]"><xsl:attribute name="TITLE"><xsl:value-of select="ABSTRACT" /></xsl:attribute></xsl:if>
		 <xsl:attribute name="HREF"><xsl:value-of select="HREF"/></xsl:attribute><xsl:value-of select="TITLE"/></A>
		 <xsl:apply-templates select="LINKTYPE" />
		 <xsl:apply-templates select="UPDATED" />
		 <xsl:apply-templates select="POSTED" />
		 </LI>
      </xsl:template>

      <xsl:template match="SECTIONINFO[VERSION]">
         <LI CLASS="noexpand"><A CLASS="clsTOCHeading">
		 <xsl:attribute name="HREF"><xsl:value-of select="VERSION[end()]/HREF"/></xsl:attribute>
		 <xsl:value-of select="VERSION[end()]/TITLE"/></A>
		 <xsl:apply-templates select="VERSION[end()]/LINKTYPE" />
		 <xsl:apply-templates select="VERSION[end()]/UPDATED" />
		 <xsl:apply-templates select="VERSION[end()]/POSTED" />
         </LI>
      </xsl:template>

      <xsl:template match="UPDATED">
         <SCRIPT LANGUAGE="javascript">//<xsl:comment><![CDATA[
         updatedItem("]]><xsl:value-of select='.'/><![CDATA[")
        //]]></xsl:comment></SCRIPT>
      </xsl:template>

      <xsl:template match="POSTED">
         <SCRIPT LANGUAGE="javascript">//<xsl:comment><![CDATA[
         newItem("]]><xsl:value-of select='.'/><![CDATA[")
         //]]></xsl:comment></SCRIPT>
      </xsl:template>

	  <!-- The following rule handles plugging in the non-ms and non-sbn icon 
	  Between the two icons, three attributes differ: width, alt, and image name
	  -->

	  <xsl:template match="LINKTYPE[.='leave-sbn']">
      <xsl:eval no-entities="1">'&amp;nbsp;'</xsl:eval><IMG BORDER="0" HEIGHT="11" WIDTH="17" ALT="Non-SBN link" SRC="/sitebuilder/graphics/leave-site.gif"/>
      </xsl:template>
    
	  <!-- <xsl:entityref name="nbsp" /> Doesn't work in recent builds 5012.4 -->
	  <xsl:template match="LINKTYPE[.='leave-ms']">
      <xsl:eval no-entities="1">'&amp;nbsp;'</xsl:eval><IMG BORDER="0" HEIGHT="11" WIDTH="33" ALT="Non-MS link" SRC="/sitebuilder/graphics/leave-ms.gif"/>
      </xsl:template>	
</xsl:stylesheet>

