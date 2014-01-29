"""
M. Littlefield
"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

import os
import libxml2
import libxslt
import math
import numpy

class CADImport: #pylint: disable = R0903
    """
    @Class CADImport, GDML parser class.
    
    This class has been designed to parse XML(GDML) files in a number of ways.
    The first way applies an XSLT stylesheet to a GDML file which re writes the
    file into MICE module format. The output can be a text file. The second 
    parsing method is where the XSLT stylesheet is applied to two GDMLs and 
    appends the MICE Information of one GDML to the end of the geometry GMDL 
    file, taken from fastRad in order to merge geometrical information and
    field information needed for G4. This outputs the a GDML file.
    """

    def __init__(self, xmlin1, xsl=None, xmlin2=None, \
             output=None, mergein=None, mergeout=None): 
             #pylint: disable = R0912, R0913
        """
        @Method Class constructor

        This method initialises the class and takes 6 parameters. 
        The later 4 parameters are optional.
        All arguments should be file names or paths.

        @param xmlin1   first xml file name/path, used to apply an XSLT to.
        @param xsl      xslt file name/path, xslt stylesheet used to 
                        re write xml(GDML)
        @param xmlin2   second xml file name/path,  
                        used to hold the MICE info to append to geometry
        @param output   output file name/path
        @param mergein  template XSLT for append, this is the file name/path  
                        to Merge.xsl.in which is the stylesheet used to append
                        the MICE info to geometry info. This file must be 
                        altered to set the putout file name/path.
        @param mergeout file name/path to be inserted into merge.xsl.in.
        """
        if type(xmlin1) != str:
            raise TypeError(xmlin1 + " must be a string(file name/path)")
        if xmlin1[-4:] != '.xml' and xmlin1[-5:] != '.gdml':
            raise IOError(xmlin1 + " must be an xml or gdml file")
        else: 
            self.xml_in_1 = xmlin1

        if xsl == None: 
            self.xsl = None        
        elif type(xsl) != str:
            raise TypeError(xsl + " must be a string(file name/path")
        elif xsl[-4:] != '.xsl':
            raise IOError(xsl + " must be an xsl file")
        else: 
            self.xsl = xsl
        
        if xmlin2 == None: 
            self.xml_in_2 = None
        elif type(xmlin2) != str:
            raise TypeError(xmlin2 + " must be a string(file name/path)")
        elif xmlin2[-4:] != '.xml' and xmlin2[-5:] != '.gdml':
            raise IOError(xmlin2 + " must be an xml or gdml file")
        else: 
            self.xml_in_2 = xmlin2
            
        if output == None: 
            self.output = None
        elif type(output) != str:
            raise TypeError(output + " must be a string(file name/path)")
        else: 
            self.output = output

        if mergein == None: 
            maus_root = os.environ['MAUS_ROOT_DIR'] 
            self.merge_in = maus_root + \
            '/src/common_py/geometry/xsltScripts/Merge.xsl.in'
        elif type(mergein) != str:
            raise TypeError(mergein + " input must be a string(file name/path)")
        else: 
            self.merge_in = mergein

        if mergeout == None: 
            path = os.environ['MAUS_ROOT_DIR'] + \
            '/src/common_py/geometry/xsltScripts/Merge.xsl'
            self.merge_out = path
        elif type(mergeout) != str:
            raise TypeError(mergeout + "output must be a string filename/path)")
        elif mergeout[-4:] != '.xsl':
            raise IOError(mergeout + " must be an xsl file")
        else: 
            self.merge_out = mergeout


    def parse_xslt(self):
        """
        @Method parse_xslt to parse an XML(GDML) to text or another XML (GDML)

        This method will execute an XSLT stylesheet and produce either a text
        or another XML(GDML) according to the request.
        """
        styledoc = libxml2.parseFile(self.xsl)
        style = libxslt.parseStylesheetDoc(styledoc)
        doc = libxml2.parseFile(self.xml_in_1)
        result = style.applyStylesheet(doc, None)
        style.saveResultToFilename(self.output, result, 0)
        style.freeStylesheet()
        doc.freeDoc()
        result.freeDoc()


    def translate_gdml(self):

        '''
        
        @Method translate_gdml to parse an XML(GDML) to a MICE
        modules text file.
        
        This python algorithm explicitly translates a gdml
        compliant file into a MICE module compliant file. The
        reason for this approach is that xslt appears to be an
        extremely limiting protocol in terms of making use of
        internal references that are used in the GDML schema --- a
        schema must exist somewhere but it seems simpler to use
        libxml2 python bindings directly.
        
        '''

        # Parse the target gdml file
        datafile = libxml2.parseFile(self.xml_in_1)
        # extract the top level object.
        world = datafile.xpathEval("gdml/setup/world")
        # get the name of the name of the world volume (there can be only one)
        moduleName = world[0].prop("ref")
        # initialize result record
        result = []
        # recursively generate a MICE module based on the world volume
        self.AccessModule(datafile, result, moduleName)
        result.append("}\n")
        # free the memory allocated to the xml file 
        datafile.freeDoc()
        # write the resulting MICE module to a text file.
        mmfile = open(self.output, 'w+')
        for l in result:
            mmfile.write(l)
        mmfile.close()
    
    def AccessModule(self, datafile, result, moduleName, volumenumber=-1):
        '''
        @Method AccessModule to recursively generate a MICE module from a gdml file
        '''
        # Record the module name
        if volumenumber > -1:
            result.append("Module "+moduleName+str(volumenumber)+'\n')
        else:
            result.append("Module "+moduleName+'\n')
        result.append("{\n")
        # loop over all volumes in the datafile to find the target volume
        system = datafile.xpathEval("gdml/structure/volume")
        for vol in system:
            if vol.prop('name') == moduleName:
                # find the reference to the solid used to generate the volume
                solid      = vol.xpathEval("solidref")
                # This solid corresponds to a box, tube, or other, that we do not know a priori
                box        = datafile.xpathEval("gdml/solids/box")
                for elem in box:
                    # Extract the properties of a box corresponding to the solid reference
                    if elem.prop('name') == solid[0].prop('ref'):
                        result.append('Volume Box\n')
                        result.append('Dimensions '+elem.prop('x')+' '+elem.prop('y')+' '+\
                                      elem.prop('z')+' '+elem.prop('lunit')+'\n')
                        break
                tube       = datafile.xpathEval("gdml/solids/tube")
                for elem in tube:
                    # Extract the properties of a tube corresponding to the solid reference
                    if elem.prop('name') == solid[0].prop('ref'):
                        if elem.prop('rmin'):
                            result.append('Volume Tube\n')
                            result.append('Dimensions '+elem.prop('rmin')+' '+elem.prop('rmax')+' '+\
                                          elem.prop('z')+' '+elem.prop('lunit')+'\n')
                        else:
                            result.append('Volume Cylinder\n')
                            result.append('Dimensions '+elem.prop('rmax')+' '+\
                                          elem.prop('z')+' '+elem.prop('lunit')+'\n')
                        break
                intersection = datafile.xpathEval("gdml/solids/intersection")
                for elem in intersection:
                    # if the solid name matches the element name, we leave the solid
                    # definition to the auxiliary elements
                    if elem.prop('name') == solid[0].prop('ref'):
                        result.append('Volume Boolean')
                        break
                subtraction = datafile.xpathEval("gdml/solids/subtraction")
                for elem in subtraction:
                    if elem.prop('name') == solid[0].prop('ref'):
                        # do nothing
                        break
                     
                aux        = vol.xpathEval('auxiliary')
                for elem in aux:
                    type, value = elem.prop('auxtype'), elem.prop('auxvalue')
                    unit = "cm"
                    if type=='unit': unit = value
                    if type=='RedColour' or type=='BlueColour' or type=='GreenColour' \
                           or type=='G4StepMax' \
                           or type=='Thickness[mm]' \
                           or type=='DiameterOfPmt[mm]'\
                           or type=='DiameterOfGate[mm]'\
                           or type=='XoffsetOfPmt[mm]'\
                           or type=='ZoffsetOfPmt[mm]':
                        result.append('PropertyDouble '+type+' '+value+"\n")
                    if type=='ActiveRadius' or type=='Pitch' or type=='FibreDiameter' \
                           or type=='CoreDiameter' or type=='CentralFibre' \
                           or type=='GlueThickness' or type=='FibreLength' \
                           or type=='FibreSpacingY' or type=='FibreSpacingZ':
                        result.append('PropertyDouble '+type+' '+value+' '+unit+'\n')
                    elif type=='numPlanes' or type=='Station' or type=='numPMTs' \
                             or type=='Plane' or type=='Tracker' or type=='Cell' \
                             or type=='CkovPmtNum':
                        result.append('PropertyInt '+type+' '+value+"\n")
                    elif type=='SensitiveDetector' \
                             or type=='Detector' \
                             or type=='G4Detector' \
                             or type=='BaseModule' \
                             or type=='BooleanModule1' \
                             or type=='BooleanModule1Type'\
                             or type=='BooleanModule2' \
                             or type=='BooleanModule2Type':
                        result.append('PropertyString '+type+' '+value+"\n")
                    elif type=='Invisible' or type=='UseDaughtersInOptics':
                        result.append('PropertyBool'+type+' '+value+"\n")
                    elif type=='Pmt1PosX':
                        result.append('PropertyHep3Vector Pmt1Pos '+value+' 0.0 0.0 cm\n')
                    elif type=='Pmt2PosX':
                        result.append('PropertyHep3Vector Pmt2Pos '+value+' 0.0 0.0 cm\n')
                    elif type=='Pmt1PosY':
                        result.append('PropertyHep3Vector Pmt1Pos 0.0 '+value+' 0.0 cm\n')
                    elif type=='Pmt2PosY':
                        result.append('PropertyHep3Vector Pmt2Pos 0.0 '+value+' 0.0 cm\n')
                    elif type=='BooleanModule1Pos' \
                         or type=='BooleanModule1Rot' \
                         or type=='BooleanModule2Pos' \
                         or type=='BooleanModule2Rot':
                        result.append('PropertyHep3Vector '+type+' '+value)
                    
                material = vol.xpathEval("materialref")
                result.append("PropertyString Material "+material[0].prop("ref")+"\n")
                physvol = vol.xpathEval("physvol")
                for elem in physvol:
                    self.AccessModule(datafile, result, \
                                     elem.xpathEval("volumeref")[0].prop('ref'), volumenumber)
                    position = elem.xpathEval("position")[0]
                    result.append("Position "+position.prop('x')+" "+position.prop('y')+' '+\
                                  position.prop('z')+" "+position.prop('unit') + '\n')
                    rotlist = elem.xpathEval("rotation")
                    if len(rotlist):
                        rotation = elem.xpathEval("rotation")[0]
                        result.append("Rotation "+rotation.prop('x')+" "+rotation.prop('y')+' '+\
                                      rotation.prop('z')+" "+rotation.prop('unit')+'\n')
                    else:
                        result.append("Rotation 0.0 0.0 0.0 degree\n")
                    if volumenumber > -1:
                        result.append("} // End"+elem.xpathEval("volumeref")[0].prop('ref')\
                                      +str(volumenumber)+"\n\n")
                    else:
                        result.append("} // End"+elem.xpathEval("volumeref")[0].prop('ref')+"\n\n")
                replicavol = vol.xpathEval("replicavol")
                for instance in replicavol:
                    basename = instance.xpathEval("volumeref")[0].prop('ref')
                    nvolumes = float(instance.prop('number'))
                    replica   = instance.xpathEval("replicate_along_axis")
                    deltapos  = float(replica[0].xpathEval("width")[0].prop('value'))
                    deltaunit = replica[0].xpathEval("width")[0].prop('unit')
                    repOffset = deltapos * math.floor(nvolumes / 2.0)
                    minOffset = -repOffset
                    maxOffset = repOffset
                    if int(nvolumes) % 2 == 0:
                        minOffset += deltapos * 0.5
                        maxOffset += deltapos * 0.5
                    elif int(nvolumes) % 2 ==1:
                        maxOffset += deltapos 
                    volumenumber = 0
                    for pos in numpy.arange(minOffset, maxOffset, deltapos):
                        self.AccessModule(datafile, result, basename, volumenumber)
                        result.append("PropertyInt Slab "+str(volumenumber)+"\n")
                        if replica[0].xpathEval("direction")[0].prop('x') == '1':
                            result.append("Position "+str(pos)+" 0.0 0.0 "+deltaunit+"\n")
                        elif replica[0].xpathEval("direction")[0].prop('y') == '1':
                            result.append("Position 0.0 "+str(pos)+" 0.0 "+deltaunit+"\n")
                        result.append("Rotation 0.0 0.0 0.0 degree\n")
                        result.append("} // End"+basename+str(volumenumber)+"\n\n")
                        volumenumber += 1
