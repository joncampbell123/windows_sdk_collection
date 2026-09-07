<?xml version="1.0"?>
<?xml-stylesheet type="text/xsl" href="showxsl.xsl"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <xsl:pi name="xml">version="1.0"</xsl:pi>
    <xsl:pi name="xml-stylesheet">type="text/xsl" href="style.xsl"</xsl:pi>
    <xsl:comment>Style sheet converted automatically to &lt;xsl:element&gt; syntax</xsl:comment>
    <xsl:apply-templates select="comment()"/>
    <xsl:apply-templates select="*"/>
  </xsl:template>

  <!-- Copy text, comments and pis -->
  <xsl:template match="comment() | pi() | text()">
    <xsl:copy>
      <xsl:apply-templates />
    </xsl:copy>
  </xsl:template>

  <!-- Convert non-XSL elements to <xsl:element> syntax -->
  <xsl:template match="*">
    <xsl:element name="xsl:element">
      <xsl:attribute name="name"><xsl:node-name/></xsl:attribute>
      <xsl:apply-templates select="@*"/> <!-- consolidate -->
      <xsl:apply-templates select="node()"/>
    </xsl:element>
  </xsl:template>

  <!-- Convert non-XSL attribute to <xsl:attribute> syntax -->
  <xsl:template match="@*">
    <xsl:element name="xsl:attribute">
      <xsl:attribute name="name"><xsl:node-name/></xsl:attribute>
      <xsl:value-of/>
    </xsl:element>
  </xsl:template>

  <!-- Copy namespace attributes -->
  <xsl:template match="@xmlns:*">
    <xsl:copy><xsl:value-of/></xsl:copy>
  </xsl:template>

  <!-- Copy XSL elements and their attributes -->
  <xsl:template match="xsl:*">
    <xsl:copy>
      <xsl:for-each select="@*">
        <xsl:copy><xsl:value-of/></xsl:copy>
      </xsl:for-each>
      <xsl:apply-templates select="node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
