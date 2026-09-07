<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <TABLE STYLE="border:1px solid black">
      <TR  STYLE="font-size:12pt; font-family:Verdana; font-weight:bold; text-decoration:underline">
        <TD>Price</TD>
        <TD STYLE="background-color:lightgrey">Time</TD>
        <TD>Bidder</TD>
      </TR>
      <xsl:for-each select="AUCTIONBLOCK/ITEM/BIDS/BID" order-by="BIDDER">
        <TR STYLE="font-family:Verdana; font-size:12pt; padding:0px 6px">
          <TD>$<xsl:value-of select="PRICE"/></TD>
          <TD STYLE="background-color:lightgrey"><xsl:value-of select="TIME"/></TD>
          <TD><xsl:value-of select="BIDDER"/></TD>
        </TR>
      </xsl:for-each>
    </TABLE>
  </xsl:template>
</xsl:stylesheet>
