<?xml version="1.0"?>
<xsl:stylesheet  xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <STYLE>
        TD {font-size:9pt}
      </STYLE>
      <BODY STYLE="font:9pt Verdana">
        <H3>Invoices</H3>
        <TABLE BORDER="1">
          <TR>
            <TD><B>Qty</B></TD>
            <TD><B>Description</B></TD>
            <TD><B>Price</B></TD>
            <TD><B>Discount</B></TD>
            <TD><B>Total</B></TD>
          </TR>
          <xsl:for-each select="invoices/invoice">
            <TR>
              <TD COLSPAN="5" STYLE="border:none; background-color:#DDDDDD">
                Invoice #<xsl:value-of select="@id"/>,
                for customer: <xsl:value-of select="/invoices/customers/customer[@id=context()/customer/@ref]"/>
              </TD>
            </TR>
            <xsl:for-each select="items/item">
              <TR>
                <TD>
                  <xsl:value-of select="qty"/>
                </TD>
                <TD>
                  <xsl:value-of select="description"/>
                </TD>
                <TD>
                  $<xsl:value-of select="price"/>
                </TD>
                <TD> <!-- 10% volume discount -->
                  <xsl:if test="qty[.$ge$10]">
                    <xsl:for-each select="price">
                      <xsl:eval>formatNumber(this.nodeTypedValue*.10, "$#,##0.00")</xsl:eval>
                    </xsl:for-each>
                  </xsl:if>
                </TD>
                <TD STYLE="text-align:right"> <!-- line total -->
                  <xsl:eval>formatNumber(lineTotal(this), "$#,##0.00")</xsl:eval>
                </TD>
              </TR>
            </xsl:for-each>
            <TR>
              <TD COLSPAN="4"></TD>
              <TD STYLE="text-align:right; border:none; border-top:1px solid black">
                <xsl:eval>formatNumber(invoiceTotal(this), "$#,##0.00")</xsl:eval>
              </TD>
            </TR>
            <TR/>
          </xsl:for-each>
        </TABLE>
      </BODY>
    </HTML>
  </xsl:template>
  
  <xsl:script><![CDATA[
    function invoiceTotal(invoice)
    {
      items = invoice.selectNodes("items/item");
      var sum = 0;
      for (var item = items.nextNode(); item; item = items.nextNode())
      {
        var price = item.selectSingleNode("price").nodeTypedValue;
        var qty = item.selectSingleNode("qty").nodeTypedValue;
        if (qty >= 10)
          price = 0.9*price;
        sum += price * qty;
      }
      return sum;
    }

    function lineTotal(item)
    {
      var price = item.selectSingleNode("price").nodeTypedValue;
      var qty = item.selectSingleNode("qty").nodeTypedValue;
      if (qty >= 10)
        price = 0.9*price;
      return qty*price;
    }
  ]]></xsl:script>
</xsl:stylesheet>

