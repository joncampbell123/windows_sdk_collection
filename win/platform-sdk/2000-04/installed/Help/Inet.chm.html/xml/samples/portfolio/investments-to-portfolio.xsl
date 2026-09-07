<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <portfolio xmlns:dt="urn:schemas-microsoft-com:datatypes">
      <xsl:for-each select="investments/item[@type='stock']">
        <stock>
          <xsl:attribute name="exchange"><xsl:value-of select="@exch"/></xsl:attribute>
          <name><xsl:value-of select="@company"/></name>
          <symbol><xsl:value-of select="@symbol"/></symbol>
          <price>
            <xsl:attribute name="dt:dt">number</xsl:attribute>
            <xsl:value-of select="@price"/></price>
        </stock>
      </xsl:for-each>
    </portfolio>
  </xsl:template>
</xsl:stylesheet>