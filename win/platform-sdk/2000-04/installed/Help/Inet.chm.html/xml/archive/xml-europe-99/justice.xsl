<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">

<xsl:template><xsl:value-of/></xsl:template>

<xsl:template match = "Profref">
<xsl:apply-templates select="id(@dueto)"/>
</xsl:template>

<xsl:template match="p">
  <p><xsl:apply-templates/></p>
</xsl:template>

<xsl:template match="Class"><p>Class:<xsl:value-of select="@name"/></p></xsl:template>
<xsl:template match="Student"> Student:<xsl:value-of select="@name"/></xsl:template>
<xsl:template match="Teacher"> Teacher:<xsl:value-of select="@name"/></xsl:template>

<xsl:template match="Workgroup">
  <xsl:value-of select="@name"/>. Led by <xsl:apply-templates select="id(@leader)"/>
</xsl:template>

<xsl:template match="Assignment">
  <Assignment>
    <date><xsl:value-of select="@date"/></date>
    <xsl:apply-templates select="p"/>
    <xsl:apply-templates select="Assignment"/>
    <Workgroup><xsl:apply-templates select="id(@workgroup)"/></Workgroup>
  </Assignment>
</xsl:template>

<xsl:template match="/">
  <Result>
    The assignments across all classes that are relevant to Justin are:
    <xsl:apply-templates select = "Data/ClassList/Class/Assignment[p/Profref/id(@dueto)/@name = 'Justin']"/>
  </Result>
</xsl:template>


</xsl:stylesheet>
