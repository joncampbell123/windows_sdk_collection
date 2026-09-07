<?xml-stylesheet type="text/xsl" href="raw-xml.xsl"?>
<Result>
  <xsl:for-each select="Data/ClassList/Class" order-by="@name" xmlns:xsl="http://www.w3.org/TR/WD-xsl">
    <Class>
      <Name><xsl:value-of select="@name" /></Name>
    </Class>
  </xsl:for-each>
</Result>
