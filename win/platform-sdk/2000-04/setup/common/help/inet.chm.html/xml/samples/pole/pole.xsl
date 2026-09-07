<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <HEAD>
        <TITLE><xsl:value-of select="document/title"/></TITLE>
      </HEAD>
      <BODY>
        <H1><xsl:value-of select="document/title"/></H1>
        <xsl:apply-templates select="document/section"/>
      </BODY>
    </HTML>
  </xsl:template>

  <xsl:template match="section">
    <DIV>
      <H2><xsl:value-of select="title"/></H2>
      <xsl:apply-templates />
    </DIV>
  </xsl:template>

  <xsl:template match="p">
    <P><xsl:apply-templates /></P>
  </xsl:template>

  <xsl:template match="list">
    <UL>
      <xsl:for-each select="item">
        <LI><xsl:apply-templates /></LI>
      </xsl:for-each>
    </UL>
  </xsl:template>

  <xsl:template match="emph">
    <I><xsl:apply-templates /></I>
  </xsl:template>  

  <xsl:template match="text()"><xsl:value-of /></xsl:template>
  
</xsl:stylesheet>