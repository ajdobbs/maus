"""
MAUS Celery tasks.
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

import logging
import re

from celery.registry import tasks
from celery.task import Task

import MAUS

class MausGenericTransformTask(Task):
    """
    MAUS task that creates an instance of each type of map worker
    then uses these to execute pipelines specified by clients.
    """
    
    def __init__(self):
        """ 
        Initialise transform to None.
        @param self Object reference.
        """
        self.__transforms = {}
        for name in dir(MAUS):
            if re.match("Map", name):
                map_class = getattr(MAUS, name)
                # MapPyTOFPlot kicks up a PyROOT canvas so skip for now.
                if name == "MapPyTOFPlot":
                    continue
                self.__transforms[name] = map_class()

    def birth(self, json_config_doc):
        """
        Setup transform then invoke birth with the JSON
        configuration document.
        @param self Object reference.
        @param json_config_doc JSON configuration document.       
        @return result of invoking birth.
        @throws exception if any exceptions arise in birth().
        """
        for name in self.__transforms:
            # MapCppSimulation takes ages to initialise so skip for now.
            if name == "MapCppSimulation":
                continue
            self.__transforms[name].birth(json_config_doc)
        return True

    def run(self, workers, spill):
        """
        Apply the transform to the spill and return the new spill.
        @param self Object reference.
        @param workers List of names of workers to process spill.
        @param spill JSON document string holding spill.
        @return JSON document string holding new spill.
        """
        logger = Task.get_logger()
        for worker in workers:
            if logger.isEnabledFor(logging.INFO):
                logger.info("Executing worker : %s" % worker)
            spill = self.__transforms[worker].process(spill)
        return spill

tasks.register(MausGenericTransformTask) # pylint:disable=W0104, E1101
