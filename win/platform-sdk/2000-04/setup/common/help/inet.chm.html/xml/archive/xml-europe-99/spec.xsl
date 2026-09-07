<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">

  <xsl:template><xsl:value-of/></xsl:template>

  <xsl:template match="/">
    <HTML>
      <HEAD>
        <TITLE><xsl:value-of select="spec/head/title"/></TITLE>
        <META NAME="ROBOTS" CONTENT="NOINDEX" />
      </HEAD>
      <BODY BGCOLOR="#FFFFFF" LINK="#000066" VLINK="#666666" TEXT="#000000">
        <BR/>
        <FONT FACE="verdana,arial,helvetica" SIZE="2">
        <xsl:for-each select="spec/head">
          <H2><xsl:value-of select="title"/></H2>

          <xsl:apply-templates select="history/creation[date]"/>
          <xsl:if match="head[history/change]">
            <A href="#_revisions">Revision history</A>
          </xsl:if>
          <xsl:apply-templates select="authors"/>
          <xsl:apply-templates select="contributors"/>
           <xsl:for-each select="abstract">
            <P STYLE="font-weight:bold; font-size:medium; margin-top:1em">
              <A name="Abstract">Abstract</A>
            </P>
            <P>
              <xsl:apply-templates/>
            </P>
          </xsl:for-each>
        </xsl:for-each>

        <HR/>
        <A NAME="top"> </A><H3>Contents</H3>
        <xsl:apply-templates select="spec/body/section">
            <xsl:template match="section[title]">
              <DIV STYLE="margin-left:1em"><xsl:eval>sectionNum(this)</xsl:eval>
                <A><xsl:attribute name="HREF">#<xsl:value-of select="title"/></xsl:attribute><xsl:value-of select="title"/></A>
                <xsl:apply-templates select="section"/>
              </DIV>
            </xsl:template>
        </xsl:apply-templates>
        <xsl:for-each select="spec/appendices//appendix[title]">
          <DIV STYLE="margin-left:1em">Appendix <xsl:eval>appendixNum(this)</xsl:eval>
            <A><xsl:attribute name="HREF">#<xsl:value-of select="title"/></xsl:attribute><xsl:value-of select="title"/></A>
          </DIV>
        </xsl:for-each>

        <HR/>

        <xsl:apply-templates select="spec/body"/>
        <xsl:apply-templates select="spec/appendices"/>
        <TABLE width="100%">
          <TR>
            <TD width="100%"><HR/></TD>
            <TD NOWRAP="true"><A HREF="#top"><FONT size="1"><I>Back to contents</I></FONT></A></TD>
          </TR>
        </TABLE>

        <xsl:for-each select="spec/head/history[change/date]">
          <A name="_revisions"><xsl:eval/></A>
          <TABLE STYLE="border:1px solid gray; margin:0">
            <TR><TD><B>Revisions:</B></TD><TD/></TR>
            <xsl:for-each select="change[date]">
              <TR>
                <TD VALIGN="top" STYLE="font-size:8pt;border-top:1px solid gray; margin:0px"><xsl:value-of select="date"/></TD>
                <TD STYLE="font-size:8pt;border-top:1px solid gray; margin:0px"><xsl:for-each select="extent"><xsl:value-of/><BR/> </xsl:for-each></TD>
              </TR>
            </xsl:for-each>
          </TABLE>
        </xsl:for-each>

        </FONT>
      </BODY>
    </HTML>
  </xsl:template>

  <!-- Header templates -->
  <!-- Construct header from structured data within the <head> element -->
  <xsl:template match="creation[date]">
    <DIV><B>Created: </B><xsl:value-of select="date"/></DIV>
  </xsl:template>

  <!-- Author templates -->
  <xsl:template match="authors">
    <DL>
    <DT><B>Author<xsl:if match="authors[author[1]]">s</xsl:if>:</B></DT>
    <DD><xsl:for-each select="author">
      <A><xsl:attribute name="HREF">mailto:<xsl:value-of select="email"/></xsl:attribute>
        <xsl:value-of select="name"/></A><xsl:if match="author[company]">, <xsl:value-of select="company"/></xsl:if>
      <BR/>
    </xsl:for-each></DD>
    </DL>
  </xsl:template>

  <!-- Contributor templates -->
  <xsl:template match="contributors">
    <DL>
    <DT><B>Contributor<xsl:if match="contributors[contributor[1]]">s</xsl:if>:</B></DT>
    <DD><xsl:for-each select="contributor">
      <A><xsl:attribute name="HREF">mailto:<xsl:value-of select="email"/></xsl:attribute>
        <xsl:value-of select="name"/></A><xsl:if match="contributor[company]">, <xsl:value-of/></xsl:if>
      <BR/>
    </xsl:for-each></DD>
    </DL>
  </xsl:template>

  <!-- Body element templates -->
  <xsl:template match="*[nodeName()='body' $or$ nodeName()='appendices']"><xsl:apply-templates/></xsl:template>
    
  <xsl:template match="*[nodeName()='section' $or$ nodeName()='appendix']">
    <xsl:apply-templates/>
    <TABLE width="100%">
      <TR>
        <TD width="100%"><HR/></TD>
        <TD NOWRAP="true"><A HREF="#top"><FONT size="1"><I>Back to contents</I></FONT></A></TD>
      </TR>
    </TABLE>
  </xsl:template>

  <xsl:template match="section[end()]"><xsl:apply-templates/></xsl:template>
  <xsl:template match="appendix[end()]"><xsl:apply-templates/></xsl:template>

  <!-- section title and appendix title templates -->
  <xsl:template match="section/title">
    <H3><xsl:eval>sectionNum(this.parentNode)</xsl:eval>
    <A><xsl:attribute name="name"><xsl:value-of/></xsl:attribute><xsl:value-of/></A></H3>
  </xsl:template>

  <xsl:template match="section/section/title">
    <H4><xsl:eval>sectionNum(this.parentNode)</xsl:eval>
    <A><xsl:attribute name="name"><xsl:value-of/></xsl:attribute><xsl:value-of/></A></H4>
  </xsl:template>

  <xsl:template match="section/section/section/title">
    <H5><xsl:eval>sectionNum(this.parentNode)</xsl:eval>
    <A><xsl:attribute name="name"><xsl:value-of/></xsl:attribute><xsl:value-of/></A></H5>
  </xsl:template>

  <xsl:template match="section/section/section/section/title">
    <H6><xsl:eval>sectionNum(this.parentNode)</xsl:eval>
    <A><xsl:attribute name="name"><xsl:value-of/></xsl:attribute><xsl:value-of/></A></H6>
  </xsl:template>

  <xsl:template match="appendix/title">
    <H3>Appendix <xsl:eval>appendixNum(this.parentNode)</xsl:eval>
    <A><xsl:attribute name="name"><xsl:value-of/></xsl:attribute><xsl:value-of/></A></H3>
  </xsl:template>  

  <!-- document elements templates -->

  <xsl:template match="p">
    <P><xsl:apply-templates/></P>
  </xsl:template>

  <xsl:template match="issue">
    <P><FONT color="red"><SMALL><B>Open issue: <xsl:apply-templates/></B></SMALL></FONT></P>
  </xsl:template>

  <xsl:template match="note">
    <P><SMALL><I>Note: <xsl:apply-templates/></I></SMALL></P>
  </xsl:template>

  <xsl:template match="hint">
    <P><SMALL><I>Hint: <xsl:apply-templates/></I></SMALL></P>
  </xsl:template>

  <!-- Code example templates -->

  <xsl:template match="example">
    <BLOCKQUOTE><xsl:if match="example[@color]"><xsl:attribute name="STYLE">background-color:<xsl:value-of/></xsl:attribute></xsl:if><FONT SIZE="3">
      <PRE><xsl:value-of/></PRE>
    </FONT></BLOCKQUOTE>
  </xsl:template>

  <xsl:template match="appendix/example">
    <FONT SIZE="3">
      <PRE><xsl:apply-templates/></PRE>
    </FONT>
  </xsl:template>

  <xsl:template match="example/description"/>

  <xsl:template match="sample">
    <xsl:apply-templates>
      <xsl:template>
        <xsl:copy>
          <xsl:for-each select="@*"><xsl:attribute><xsl:value-of/></xsl:attribute></xsl:for-each>
          <xsl:apply-templates/>
        </xsl:copy>
      </xsl:template>
    </xsl:apply-templates>
  </xsl:template>

  <!-- List templates -->
  <xsl:template match="bullet-list">
    <UL>
      <xsl:for-each select="item">
        <LI><xsl:apply-templates/></LI>
      </xsl:for-each>
    </UL>
  </xsl:template>

  <xsl:template match="number-list">
    <OL>
      <xsl:for-each select="item">
        <LI><xsl:apply-templates/></LI>
      </xsl:for-each>
    </OL>
  </xsl:template>

  <xsl:template match="definition-list">
    <DL>
      <xsl:apply-templates select="*[nodeName='term' $or$ nodeName='definition']">
          <xsl:template match="term">
            <DT><xsl:apply-templates/></DT>
          </xsl:template>
          <xsl:template match="definition">
            <DD><xsl:apply-templates/></DD>
          </xsl:template>
      </xsl:apply-templates>
    </DL>
  </xsl:template>

  <xsl:template match="history-list">
    <DL>
      <xsl:apply-templates />
    </DL>
  </xsl:template>

  <xsl:template match="step-list">
    <DL>
      <xsl:for-each select="item">
        <DT><BR/><B>Step <eval>formatNumber(childNumber(this), "1")</eval>:
        <xsl:value-of select="step-name"/></B></DT>
        <DD><xsl:value-of select="step-text"/></DD>
      </xsl:for-each>
    </DL>
  </xsl:template>

  <xsl:template match="history-list/item">
    <DD>
      <xsl:apply-templates/>
    </DD>
  </xsl:template>
    
  <xsl:template match="history-list/item">
    <DT><B><eval>getAttribute("date")</eval></B></DT>
    <DD>
      <xsl:apply-templates/>
    </DD>
  </xsl:template>

  <!-- FAQ -->

  <xsl:template match="faq">
    <BLOCKQUOTE>
      <xsl:apply-templates/>
    </BLOCKQUOTE>
  </xsl:template>

  <xsl:template match="question">
      <DT>
    <B>Question: </B>
    </DT>
    <DD>
      <I><xsl:apply-templates/></I>
    </DD>
  </xsl:template>

  <xsl:template match="answer">
    <DT>
      <B>Answer: </B>
    </DT>
    <DD>
      <xsl:apply-templates/>
    </DD>
  </xsl:template>

  <!-- figures and graphics -->

  <xsl:template match="figure">
      <CENTER>
        <xsl:apply-templates select="graphic"/>
        <BR/>
        <xsl:apply-templates select=".//name"/>
      </CENTER>
  </xsl:template>

  <xsl:template match="graphic">
    <IMG><xsl:attribute name="SRC"><xsl:value-of/></xsl:attribute></IMG>
  </xsl:template>

  <xsl:template match="figure/name">
      <I><xsl:apply-templates/></I>
  </xsl:template>  

  <!-- method templates -->

  <xsl:template match="*[nodeName()='method' $or$ nodeName()='property' $or$ nodeName()='function']">
    <FONT SIZE="4"><B><xsl:value-of select="name"/></B></FONT>
    <DL>
      <DT><B>Description:</B></DT>
      <DD><xsl:value-of select="purpose"/></DD>
      <DT><B>Prototype:</B></DT>
      <DD><FONT FACE="Courier"><xsl:value-of select="proto"/></FONT><BR/></DD>
      <xsl:for-each select="args">
        <DT><B>Arguments:</B></DT>
        <DD><TABLE>
          <TBODY>
            <TR><TD width="10px"></TD></TR>
            <xsl:for-each select="arg">
              <TR>
                <TD VALIGN="top">
                  <FONT FACE="verdana,arial,helvetica" size="2"><I><xsl:value-of select="name"/></I></FONT>
                </TD>
                <TD width="10px"></TD>
                <TD VALIGN="top">
                  <FONT FACE="verdana,arial,helvetica" size="2"><xsl:apply-templates select="usage"/></FONT>
                </TD>
              </TR>
            </xsl:for-each>
          </TBODY>
        </TABLE></DD>
      </xsl:for-each>
      <xsl:for-each select="example">
        <DT><B>Examples:</B></DT>
        <DD><FONT SIZE="3">
          <PRE><xsl:value-of/></PRE>
        </FONT></DD>
      </xsl:for-each>
    </DL>
  </xsl:template>
    
  <xsl:template match="usage">
      <xsl:apply-templates/>
  </xsl:template>  


  <!-- link templates -->

  <xsl:template match="external-link">
    <A>
      <xsl:attribute name="HREF"><xsl:value-of/></xsl:attribute>
      <xsl:if match="external-link[@replace='yes']">
        <xsl:attribute name="TARGET">_top</xsl:attribute>
      </xsl:if>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="external-link[@to]">
    <A>
      <xsl:attribute name="HREF"><xsl:value-of select="@to"/></xsl:attribute>
      <xsl:if match="external-link[@replace='yes']">
        <xsl:attribute name="TARGET">_top</xsl:attribute>
      </xsl:if>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="internal-link">
    <A>
      <xsl:attribute name="HREF"><xsl:value-of/></xsl:attribute>
      <xsl:if match="internal-link[@replace='yes']">
        <xsl:attribute name="TARGET">_top</xsl:attribute>
      </xsl:if>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="internal-link[@to]">
    <A>
      <xsl:attribute name="HREF"><xsl:value-of select="@to"/></xsl:attribute>
      <xsl:if match="internal-link[@replace='yes']">
        <xsl:attribute name="TARGET">_top</xsl:attribute>
      </xsl:if>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="nearby-link">
    <A>
      <xsl:attribute name="HREF"><xsl:value-of/></xsl:attribute>
      <xsl:if match="nearby-link[@replace='yes']">
        <xsl:attribute name="TARGET">_top</xsl:attribute>
      </xsl:if>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="nearby-link[@to]">
    <A>
      <xsl:attribute name="HREF"><xsl:value-of select="@to"/></xsl:attribute>
      <xsl:if match="nearby-link[@replace='yes']">
        <xsl:attribute name="TARGET">_top</xsl:attribute>
      </xsl:if>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="local-link">
    <A>
      <xsl:attribute name="HREF">#<xsl:value-of/></xsl:attribute>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="local-link[@to]">
    <A>
      <xsl:attribute name="HREF">#<xsl:value-of select="@to"/></xsl:attribute>
      <xsl:value-of/>
    </A>
  </xsl:template>

  <xsl:template match="download">
    <A>
      <xsl:attribute name="HREF"><xsl:value-of select="from"/></xsl:attribute>
      <IMG SRC="/sitebuilder/graphics/icodownl.gif" WIDTH="16" HEIGHT="20" BORDER="0" ALT="Download"/>
      <xsl:apply-templates/></A>
  </xsl:template>

  <!-- Style rules -->
  <xsl:template match="*[nodeName()='emph' $or$ nodeName()='key-term' $or$ nodeName()='code']"><I><xsl:apply-templates/></I></xsl:template>

  <xsl:template match="code"><FONT face="Courier"><B><xsl:apply-templates/></B></FONT></xsl:template>

  <xsl:template match="*[nodeName()='function-name' $or$ nodeName()='property-name' $or$ nodeName()='method-name' $or$ nodeName()='attribute-name']"><B><xsl:apply-templates/></B></xsl:template>

  <xsl:template match="tag"><B>&lt;<xsl:apply-templates/>&gt;</B></xsl:template>

  <xsl:template match="filename"><xsl:apply-templates/></xsl:template>

  <xsl:script><![CDATA[
    function sectionNum(e) {
      if (e)
      {
        return sectionNum(e.selectSingleNode("ancestor(section)")) +
               formatIndex(childNumber(e), "1") + ".";
      }
      else
      {
        return "";
      }
    }

    function appendixNum(e) {
      if (e)
      {
        return appendixNum(e.selectSingleNode("ancestor(appendix)")) +
               formatIndex(childNumber(e), "A") + ".";
      }
      else
      {
        return "";
      }
    }
  ]]></xsl:script>

</xsl:stylesheet>
