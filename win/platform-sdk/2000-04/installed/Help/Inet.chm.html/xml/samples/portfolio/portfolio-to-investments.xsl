<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <investments>
      <xsl:for-each select="portfolio/stock">
        <item type="stock">
          <xsl:attribute name="exch"><xsl:value-of select="@exchange"/></xsl:attribute>
          <xsl:attribute name="symbol"><xsl:value-of select="symbol"/></xsl:attribute>
          <xsl:attribute name="company"><xsl:value-of select="name"/></xsl:attribute>
          <xsl:attribute name="price"><xsl:value-of select="price"/></xsl:attribute>
        </item>
      </xsl:for-each>
    </investments>
  </xsl:template>
</xsl:stylesheet>
