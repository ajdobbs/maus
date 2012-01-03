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

import shutil
import os
from xml.dom import minidom

SCHEMA_PATH = os.environ["MAUS_ROOT_DIR"]+\
              "/src/common_py/geometry/GDML_3_0_0/schema/gdml.xsd"

class Formatter: #pylint: disable = R0902
    """
    @Class formatter this class formats the raw outputted fastrad files so their
           schema material file paths are correct

    This class opens the files which need the correct paths added and edits the
    file in situ.
    """
    def __init__(self, path_in, path_out, path_tmp):
        """
        @Method Class Constructor

        This is the class constructor which sets up the variables so they point
        at the correct files ready to edit.
        @Param Path The path to the directory which contains the fastrad
                    outputted GDML files
        """
        self.path_in = path_in
        self.path_out = path_out
        self.path_tmp = path_tmp
        self.beamline_file = None
        self.field_file = None
        self.configuration_file = None
        self.material_file = None
        self.stepfiles = []
        self.formatted = False
        self.txt_file = ""
        self.schema = SCHEMA_PATH
        gdmls = os.listdir(self.path_in)
        for fname in gdmls:
            if fname[-5:] != '.gdml' and fname[-4:] != '.xml':
                print fname+' not a gdml or xml file - ignored'
            elif fname.find('materials') >= 0:
                self.material_file = fname
            elif fname.find('fastrad') >= 0:
                self.configuration_file = fname
            elif fname.find('Field') >= 0 or fname.find('field') >= 0:
                self.field_file = fname
            elif fname.find('Beamline') >= 0:
                self.beamline_file = fname
            else:
                self.stepfiles.append(fname)
            shutil.copy(os.path.join(self.path_in, fname), 
                        os.path.join(self.path_tmp, fname) )
        if self.field_file == None:
            raise IOError('Please write a Field.gdml file which ' + \
                        'contains MAUS_info. See README for details')
        
    def format_schema_location(self, gdmlfile):
        """
        @method format_schema_location
        
        This method parses the GDML file into memory and alters the location of 
        the schema location. It then rewrites the GDML with the valid schema.
        
        @param gdmlfile absolute path to the file which will have its schema
                        location altered.
        """
        xmldoc = minidom.parse(os.path.join(self.path_tmp, gdmlfile))
        for node in xmldoc.getElementsByTagName("gdml"):
            if node.hasAttribute("xsi:noNamespaceSchemaLocation"):
                node.attributes['xsi:noNamespaceSchemaLocation'] = self.schema
        fout = open(os.path.join(self.path_tmp, gdmlfile), 'w')
        xmldoc.writexml( fout)
        fout.close()
       
    def merge_maus_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the field information to the configuration GDML.
        """
        config = minidom.parse(os.path.join(self.path_tmp, gdmlfile))
        field = minidom.parse(os.path.join(self.path_tmp, self.field_file))
        for node in field.getElementsByTagName("MICE_Information"):
            maus_info = node
        root_node = config.childNodes[2]
        root_node.insertBefore(maus_info, root_node.childNodes[9])
        fout = open(os.path.join(self.path_tmp, gdmlfile), 'w')
        root_node.writexml(fout)
        fout.close()
        
    def merge_run_info(self, gdmlfile):
        """
        @method merge_maus_info
        
        This method adds the run information to the configuration GDML.
        """
        run_info = False
        fin = open(os.path.join(self.path_tmp, gdmlfile))
        for lines in fin.readlines():
            if lines.find('run') >= 0:
                run_info = True
        fin.close()
        if run_info == False:
            field = minidom.parse(os.path.join(self.path_tmp, gdmlfile))
            beamline = minidom.parse(self.beamline_file)
            for node in beamline.getElementsByTagName("run"):
                maus_info = node
            root_node = field.childNodes[0].childNodes[1].childNodes[1]
            root_node.insertBefore(maus_info, root_node.childNodes[0])
            fout = open(os.path.join(self.path_tmp, gdmlfile), 'w')
            field.writexml(fout)
            fout.close()
        print 'Run information merged!'
     
    def format_materials(self, gdmlfile):
        """
        @method Format Materials
        
        This method opens a new GDML file in memory and creates a new Document
        Type Definition (DTD) which contains the correct location of the
        materials reference file produced by fastRad.
       
        @param GDMLFile The name of the file which will have its materials
               reference file location altered.
        """
        impl = minidom.getDOMImplementation()
        docstr = 'gdml [<!ENTITY materials SYSTEM "' \
        + self.material_file + '">]'
        doctype = impl.createDocumentType(docstr, None, None)
        newdoc = impl.createDocument(None, "MAUS", doctype)
        config = minidom.parse(os.path.join(self.path_tmp, gdmlfile))
        for node in config.getElementsByTagName("gdml"):
            rootelement = node
        for node2 in newdoc.getElementsByTagName("MAUS"):
            oldelement = node2
        newdoc.replaceChild(rootelement, oldelement)
        self.txt_file = os.path.join(self.path_tmp, gdmlfile[:-5] + '.txt')
        fout = open(self.txt_file, 'w')
        newdoc.writexml(fout)
        fout.close()
               
    def insert_materials_ref(self, inputfile): #pylint: disable = R0201
        """
        @method Insert Materials Reference
        
        This method opens the GDML file and replaces the materials file
        reference call which is lost during the parsing of the GDML's in other
        methods.
        
        @param GDMLFile The name of the file which will have its materials
                        reference replaced.
        """
        fin = open(inputfile, 'r')
        gdmlfile = inputfile[:-4] + '.gdml'
        fout = open(os.path.join(self.path_tmp, gdmlfile), 'w')
        for line in fin.readlines():
            if line.find('<!-- Materials definition CallBack -->')>=0:
                matdef = '<!-- Materials definition CallBack -->'
                new_line = line.replace(matdef, matdef+'&materials;')
                print >> fout, new_line
            else:
                print >> fout, line
        print >> fout, '<!-- Formatted for MAUS -->'
        fin.close()
        fout.close()
        os.remove(inputfile)
            
    def format_check(self, gdmlfile):
        """
        @method Format Check
        
        This method checks to see whether the file passed to it
        has already been formatted but looking for the string
        <!-- Formatted for MAUS --> which will have been put in the file
        if it had already been formatted. This is to stop multiple 
        materials reference calls being added as it cannot be added by parsing
        the GDML.
        
        @param GDML File The file to be checked.
        """
        self.formatted = False
        fin = open(os.path.join(self.path_tmp, gdmlfile))
        for lines in fin.readlines():
            if lines.find('<!-- Formatted for MAUS -->') >= 0:
                print gdmlfile + ' already formatted!'
                self.formatted = True
        fin.close()

    def copy_tmp_to_out(self, filename):
        """
        Copy from the temporary directory to the actual output directory
        """
        if self.path_tmp != self.path_out:
            shutil.copy(os.path.join(self.path_tmp, filename), \
                        os.path.join(self.path_out, filename) )

    def format(self):
        """
        @method Format
        
        This method calls all the other methods to format the 
        necessary parts of the file. It will run through all of the
        fastRad outputted files within the folder location given to
        the class constructor.
        """
        self.format_check(self.configuration_file)
        if self.material_file != None:
            self.copy_tmp_to_out(self.material_file)


        if self.beamline_file != None:
            self.merge_run_info(self.field_file)
            self.copy_tmp_to_out(self.field_file)

        if self.formatted == False:
            self.format_schema_location(self.configuration_file)
            self.merge_maus_info(self.configuration_file)
            self.format_materials(self.configuration_file)
            self.insert_materials_ref(self.txt_file)
            self.copy_tmp_to_out(self.configuration_file)
        print "Formatted Configuration File"
        
        noofstepfiles = len(self.stepfiles)
        for num in range(0, noofstepfiles):
            self.format_check(self.stepfiles[num])
            if self.formatted == False:
                self.format_schema_location(self.stepfiles[num])
                self.format_materials(self.stepfiles[num])
                self.insert_materials_ref(self.txt_file)
                self.copy_tmp_to_out(self.stepfiles[num])
            print "Formatted " + str(num+1) + \
            " of " + str(noofstepfiles) + " Geometry Files"
        print "Format Complete"
