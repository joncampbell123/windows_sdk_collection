<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <BODY>
        <xsl:for-each select="portfolio/stock">
          <DIV STYLE="font-size:smaller">Symbol: <xsl:value-of select="symbol"/>,
            Company: <xsl:value-of select="name"/>,
            Price: $<xsl:value-of select="price"/>
          </DIV>
          <DIV>
            <!-- select the children of the logo element -->
            <xsl:apply-templates select="logo/*">
              <!-- recursively apply this template to them -->
              <xsl:template>
                <xsl:copy>
                  <xsl:apply-templates select="@* | * | comment() | pi() | text()"/>
                </xsl:copy>
              </xsl:template>
            </xsl:apply-templates>
          </DIV>
          <HR/>
        </xsl:for-each>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>