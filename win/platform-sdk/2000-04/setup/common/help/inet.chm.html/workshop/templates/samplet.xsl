<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">

<xsl:template match="/">
<xsl:apply-templates select="samplet" />
</xsl:template>

<xsl:template match="samplet">
<xsl:eval no-entities="1">'&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"&gt;'</xsl:eval>
<HTML>
<HEAD>
<TITLE><xsl:value-of select="metadata/title" /></TITLE>

<!-- 
<xsl:for-each select="code[@language='JavaScript' $or$ @language='JScript']"> 
<SCRIPT LANGUAGE="JavaScript"><xsl:if match="*[@defer]"><xsl:attribute name="DEFER">1</xsl:attribute></xsl:if>
-->

<xsl:for-each select="code[@language='VBScript']">
<SCRIPT LANGUAGE="VBScript"><xsl:if match="*[@defer]"><xsl:attribute name="DEFER">1</xsl:attribute></xsl:if>
//<xsl:comment>
<xsl:value-of select="." />
//</xsl:comment>
</SCRIPT>
</xsl:for-each>

<LINK REL="stylesheet" TYPE="text/css" HREF="/msdn-online/shared/css/ie4.css" />

</HEAD>
<BODY TOPMARGIN="0" LEFTMARGIN="0" MARGINHEIGHT="0" MARGINWIDTH="0" BGCOLOR="#FFFFFF">
<xsl:comment> TOOLBAR_START </xsl:comment>
<xsl:comment> TOOLBAR_EXEMPT </xsl:comment>
<xsl:comment> TOOLBAR_END </xsl:comment>

<DIV CLASS="clsDocBody">

<H1><xsl:value-of select="metadata/title" /></H1>
<xsl:if match="metadata/abstract">
<P><xsl:value-of select="metadata/abstract" /></P>
</xsl:if>

<xsl:apply-templates select="content">
<xsl:template><xsl:copy><xsl:apply-templates select="@*"/><xsl:apply-templates /></xsl:copy></xsl:template>
<xsl:template match="content"><xsl:apply-templates /></xsl:template>
<xsl:template match="comment"><xsl:comment> <xsl:value-of /> </xsl:comment></xsl:template>
<xsl:template match="space"><xsl:entityref name="nbsp" /></xsl:template>
<xsl:template match="entity"><xsl:eval no-entities="1">'&amp;'</xsl:eval><xsl:value-of select="@name" />;</xsl:template>
</xsl:apply-templates>

</DIV>

<xsl:comment> Copyright 1998 Microsoft Corporation. All rights reserved. </xsl:comment>

</BODY>
</HTML>
</xsl:template>
	
</xsl:stylesheet>

