<?xml version="1.0"?>
<stylesheet xmlns="http://www.w3.org/TR/WD-xsl">
  <!-- Identity transformation template -->
  <template><copy><apply-templates select="@* | * | comment() | pi() | text()"/></copy></template>
  
  <!-- Rename stocks to security -->
  <template match="stock" >
    <element name="security"><apply-templates select="@* | * | comment() | pi() | text()"/></element>
  </template>
</stylesheet>
