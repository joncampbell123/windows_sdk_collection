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
              <xsl:attribute name="STYLE">color:
                <xsl:choose>
                  <xsl:when test="price[. $le$ 25]">green</xsl:when>
                  <xsl:when test="price[. $le$ 50]">blue</xsl:when>
                  <xsl:otherwise>red</xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
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
        <P>Price key: <SPAN STYLE="color:green">less than 25</SPAN>, 
            <SPAN STYLE="color:blue">25-50</SPAN>, 
            <SPAN STYLE="color:red">50+</SPAN>.</P>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>
