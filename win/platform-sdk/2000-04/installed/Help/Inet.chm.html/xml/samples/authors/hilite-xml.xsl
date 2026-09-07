<?xml version="1.0"?>

<!-- Generic stylesheet for viewing XML -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <DIV STYLE="font-family:Courier; font-size:10pt; margin-bottom:2em">
      <xsl:apply-templates />
    </DIV>
  </xsl:template>  
  
  <xsl:template match="*">
    <DIV STYLE="margin-left:1em; color:gray">
      <xsl:attribute name="id"><xsl:eval>makeId(this)</xsl:eval></xsl:attribute>
      &lt;<xsl:node-name/><xsl:apply-templates select="@*"/>/&gt;
    </DIV>
  </xsl:template>

  <xsl:template match="*[node()]">
    <DIV STYLE="margin-left:1em">
      <SPAN STYLE="color:gray">
        <xsl:attribute name="id"><xsl:eval>makeId(this)</xsl:eval></xsl:attribute>
        &lt;<xsl:node-name/><xsl:apply-templates select="@*"/>&gt;</SPAN><xsl:apply-templates select="node()"/><SPAN STYLE="color:gray">&lt;/<xsl:node-name/>&gt;</SPAN>
    </DIV>
  </xsl:template>

  <xsl:template match="@*" xml:space="preserve">
    <SPAN STYLE="color:navy"><xsl:attribute name="id"><xsl:eval>makeId(this)</xsl:eval></xsl:attribute>
    <xsl:node-name/>="<SPAN STYLE="color:black"><xsl:value-of /></SPAN>"</SPAN>
  </xsl:template>

  <xsl:template match="pi()">
    <DIV STYLE="margin-left:1em; color:maroon"><xsl:attribute name="id"><xsl:eval>makeId(this)</xsl:eval></xsl:attribute>&lt;?<xsl:node-name/><xsl:apply-templates select="@*"/>?&gt;</DIV>
  </xsl:template>

  <xsl:template match="cdata()"><pre><xsl:attribute name="id"><xsl:eval>makeId(this)</xsl:eval></xsl:attribute>&lt;![CDATA[<xsl:value-of />]]&gt;</pre></xsl:template>

  <xsl:template match="textNode()"><SPAN><xsl:attribute name="id"><xsl:eval>makeId(this)</xsl:eval></xsl:attribute><xsl:value-of /></SPAN></xsl:template>
  
  <xsl:script>
      function makeId(e)
      {
        if (e)
          return makeId(e.selectSingleNode("..")) + 
            absoluteChildNumber(e) + (e.nodeType == 2 ? "@" : "_");
        else
          return "";
      }
  </xsl:script>
</xsl:stylesheet>
