<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">

<xsl:template><xsl:value-of/></xsl:template>

<xsl:template match="Product">
<TR>
<TD><xsl:value-of select="@company"/></TD>
<TD><xsl:value-of select="@name"/></TD>
</TR>
</xsl:template>

<xsl:template match="/">
<HTML>
<BODY>
<TABLE>
<xsl:apply-templates select="Data/Invoice" />
</TABLE>
</BODY>
</HTML>
</xsl:template>

<xsl:template match="Address">
<TABLE border="2">
<TR><TD>Name:</TD><TD><xsl:value-of select="@name"/></TD></TR>
<TR><TD>Street:</TD><TD><xsl:value-of select="@street"/></TD></TR>
<TR><TD>City:</TD><TD><xsl:value-of select="@city"/></TD></TR>
<TR><TD>State:</TD><TD><xsl:value-of select="@state"/></TD></TR>
<TR><TD>Phone:</TD><TD><xsl:value-of select="@phone"/></TD></TR>
</TABLE>
</xsl:template>

<xsl:template match="Item">
<TR>
<TD><xsl:value-of select="@quantity"/></TD>
<TD><TABLE><xsl:apply-templates select="id(@product)"/></TABLE></TD>
<TD><xsl:value-of select="@price"/></TD>
</TR>
</xsl:template>

<xsl:template match="Invoice">
<TR><TD>
<TABLE><TR>
<TD>Bill To:</TD><TD><xsl:apply-templates select="id(@billto)"/></TD>
<TD>Ship To:</TD><TD><xsl:apply-templates select="id(@shipto)"/></TD>
</TR></TABLE>
</TD></TR>
<TR><TD>
<TABLE border="2">
<TR><TD>Quantity</TD><TD>Product information</TD><TD>Price</TD></TR>
<TR><TD><xsl:apply-templates select="id(@items)" order-by="+@id"/></TD></TR>
</TABLE>
</TD></TR>
<TR></TR>
<TR><TD>Total Amount is <xsl:value-of select="@amount"/></TD></TR>
</xsl:template>


</xsl:stylesheet>
