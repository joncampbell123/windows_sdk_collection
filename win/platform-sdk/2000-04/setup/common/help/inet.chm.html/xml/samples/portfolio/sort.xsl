<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <!-- Identity transformation template -->
  <xsl:template><xsl:copy><xsl:apply-templates select="@* | * | comment() | pi() | text()"/></xsl:copy></xsl:template>
  
  <!-- Filter out stocks not listed on the nasdaq stock exchange -->
  <xsl:template match="stock[@exchange != 'nasdaq']" />

  <!-- Sort stocks by price -->
  <xsl:template match="portfolio"><xsl:copy><xsl:apply-templates select="@*"/><xsl:apply-templates select="stock" order-by="price"/></xsl:copy></xsl:template>
</xsl:stylesheet>