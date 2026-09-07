<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <HEAD>
        <SCRIPT LANGUAGE="JSCRIPT"><xsl:comment><![CDATA[
          function hiLite()
          {
            e = window.event.srcElement;
            if (e.style.backgroundColor != 'yellow')
              e.style.backgroundColor = 'yellow';
            else
              e.style.backgroundColor = 'white';
          }
        ]]></xsl:comment></SCRIPT>
      </HEAD>
      <BODY>
        <xsl:for-each select="grocery-list/item">
          <DIV STYLE="background-color:yellow" onClick="hiLite()">
            <xsl:value-of/>
          </DIV>
        </xsl:for-each>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>