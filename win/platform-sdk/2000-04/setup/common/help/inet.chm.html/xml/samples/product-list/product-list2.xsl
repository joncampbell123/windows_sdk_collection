<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <BODY STYLE="font:10pt Arial">
        <DIV><B>Comma separated list:</B></DIV>
        <DIV>
          <xsl:for-each select="products/product">
            <xsl:value-of /><xsl:if test="context()[not(end())]">, </xsl:if>
          </xsl:for-each>
        </DIV>
        <BR/>
        
        <DIV><B>Comma separated and sorted list:</B></DIV>
        <DIV>
          <xsl:for-each select="products/product" order-by="-.">
            <xsl:value-of /><xsl:if test="context()[not(end())]">, </xsl:if>
          </xsl:for-each>
        </DIV>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>