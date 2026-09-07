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
    Adam attends the following classes:
    <xsl:apply-templates select = "id(Data/StudentBody/Student[@name='Adam']/@attends)" order-by="+@name"/>
    This means that his assignments sorted by due due are:
    <xsl:apply-templates select = "id(Data/StudentBody/Student[@name='Adam']/@attends)/Assignment" order-by="@date"/>
  </Result>
</xsl:template>

</xsl:stylesheet>
