<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"

 xmlns:xd="http://www.oxygenxml.com/ns/doc/xsl" exclude-result-prefixes="xd" version="1.0">

 <xd:doc scope="stylesheet">

  <xd:desc>

   <xd:p><xd:b>Created on:</xd:b> Jan 10, 2011</xd:p>

   <xd:p><xd:b>Author:</xd:b> Matt</xd:p>

   <xd:p/>

  </xd:desc>

 </xd:doc>







 <xsl:output method="xml"/>

 <xsl:template match="gdml">

  <xsl:copy>

   <xsl:copy-of select="."/>

   <xsl:text>

   

  </xsl:text>

   <xsl:copy-of

    select="document('/home/matt/maus-littlefield/src/common_py/geometry/testCases/testGeometry/FieldInfoTest.xml')/gdml/MICE_Information"/>

   <xsl:apply-templates/>

  </xsl:copy>

 </xsl:template>

</xsl:stylesheet>

