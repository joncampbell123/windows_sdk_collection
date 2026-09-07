<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <BODY STYLE="font:bold 10pt Arial">
        <DIV>Two column table:</DIV>
        <TABLE BORDER="1">
          <xsl:for-each select="products/product">
            <xsl:if expr="(childNumber(this) % 2) == 1">
              <TR>
                <TD><xsl:value-of /></TD>
                <TD><xsl:value-of select="../product[index() $gt$ context()!index()][0]"/></TD>
              </TR>
            </xsl:if>
          </xsl:for-each>
        </TABLE>

        <BR/>
        <DIV>Three column table:</DIV>
        <TABLE BORDER="1">
          <xsl:for-each select="products/product">
            <xsl:if expr="(childNumber(this) % 3) == 1">
              <TR>
                <TD><xsl:value-of /></TD>
                <TD><xsl:value-of select="../product[index() $gt$ context()!index()][0]"/></TD>
                <TD><xsl:value-of select="../product[index() $gt$ context()!index()][1]"/></TD>
              </TR>
            </xsl:if>
          </xsl:for-each>
        </TABLE>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>