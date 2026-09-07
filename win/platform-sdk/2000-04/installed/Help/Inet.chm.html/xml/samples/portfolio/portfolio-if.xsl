<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <BODY>
        <TABLE BORDER="2">
          <TR>
            <TD>Symbol</TD>
            <TD>Name</TD>
            <TD>Price</TD>
          </TR>
          <xsl:for-each select="portfolio/stock">
            <TR>
              <TD>
                <xsl:value-of select="symbol"/>
                <xsl:if test="@exchange[.='nasdaq']">*</xsl:if>
              </TD>
              <TD><xsl:value-of select="name"/></TD>
              <TD><xsl:value-of select="price"/></TD>
            </TR>
          </xsl:for-each>
        </TABLE>
        <P>* Listed on Nasdaq stock exchange</P>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>
