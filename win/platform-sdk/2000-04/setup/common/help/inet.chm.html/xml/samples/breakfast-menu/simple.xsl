<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <BODY STYLE="font-family:Arial, helvetica, sans-serif; font-size:12pt;
            background-color:#EEEEEE">
        <xsl:for-each select="breakfast-menu/food">
          <DIV STYLE="background-color:teal; color:white; padding:4px">
            <SPAN STYLE="font-weight:bold; color:white"><xsl:value-of select="name"/></SPAN>
            - <xsl:value-of select="price"/>
          </DIV>
          <DIV STYLE="margin-left:20px; margin-bottom:1em; font-size:10pt">
            <xsl:value-of select="description"/>
            <SPAN STYLE="font-style:italic">
              (<xsl:value-of select="calories"/> calories per serving)
            </SPAN>
          </DIV>
        </xsl:for-each>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>
