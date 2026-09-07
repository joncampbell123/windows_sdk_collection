<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <HEAD>
        <SCRIPT LANGUAGE="JSCRIPT"><xsl:comment><![CDATA[
          function hiLite(normalColor, hiliteColor)
          {
            e = window.event.srcElement;
            if (e.style.backgroundColor == hiliteColor)
              e.style.backgroundColor = normalColor;
            else
              e.style.backgroundColor = hiliteColor;
          }
        ]]></xsl:comment></SCRIPT>
      </HEAD>
      <BODY>
        <xsl:for-each select="grocery-list/item">
          <DIV>
            <xsl:attribute name="STYLE">
              background-color:<xsl:eval>whichColor(this)</xsl:eval>
            </xsl:attribute>
            <xsl:attribute name="onClick">
              hiLite('<xsl:eval>whichColor(this)</xsl:eval>',
                '<xsl:eval>selectedColor(this)</xsl:eval>')
            </xsl:attribute>
            <xsl:value-of/>
          </DIV>
        </xsl:for-each>
      </BODY>
    </HTML>
  </xsl:template>

  <xsl:script><![CDATA[
    function even(e) {
      return childNumber(e) % 2;
    }

    function whichColor(e) {
      if (even(e))
        return "#ddffdd";
      else
        return "#ffffff";
    }

    function selectedColor(e) {
      if (even(e))
        return "#bbddbb";
      else
        return "#dddddd";
    }
  ]]></xsl:script>
</xsl:stylesheet>