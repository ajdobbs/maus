"""
m. Littlefield
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

from Configuration import Configuration
import json

class Configreader(): #pylint: disable = R0903, R0902
    """
    @class configreader, Configuration File Reader Class
    
    This class reads the information in the Configuration Text File and 
    translates this into variables which can be passed between the other 
    classes.
    """
    def __init__(self):
        """
        @method Class Constructor
        
        This sets up the blank variables ready to be filled by the information
        contained in the Configuration text file.
        """
        inputs = Configuration().getConfigJSON(command_line_args = True)
        config_dict = json.loads(inputs)
        self.cdb_upload_url = config_dict['cdb_upload_url']
        self.geometry_upload_wsdl  = config_dict['geometry_upload_wsdl']
        self.geometry_upload_directory = config_dict \
                                                   ['geometry_upload_directory']
        self.geometry_upload_note = config_dict['geometry_upload_note']
        self.geometry_upload_valid_from = config_dict \
                                                  ['geometry_upload_valid_from']
        self.geometry_upload_cleanup = config_dict['geometry_upload_cleanup']

        self.cdb_download_url = config_dict['cdb_download_url']
        self.geometry_download_wsdl = config_dict['geometry_download_wsdl']
        self.geometry_download_directory = config_dict \
                                                 ['geometry_download_directory']
        self.geometry_download_by = config_dict['geometry_download_by']
        self.geometry_download_run_number = config_dict \
                                                ['geometry_download_run_number']
        self.geometry_download_id = config_dict['geometry_download_id']
        self.geometry_download_cleanup = config_dict \
                                                   ['geometry_download_cleanup']
