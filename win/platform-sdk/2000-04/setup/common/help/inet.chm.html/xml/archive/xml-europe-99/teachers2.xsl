<?xml-stylesheet type="text/xsl" href="raw-xml.xsl"?>
<Result>
  <xsl:for-each select = "id(Data/StudentBody/Student[@name='Adam']/@attends)" xmlns:xsl="http://www.w3.org/TR/WD-xsl">
    <Class>
      <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
      <xsl:for-each select="id(@taughtBy)/@name">
        <xsl:attribute name="taughtBy"><xsl:value-of /></xsl:attribute>
      </xsl:for-each>
    </Class>
  </xsl:for-each>
</Result>
