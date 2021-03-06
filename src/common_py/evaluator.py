# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""
Workaround for memory leak in CLHEP Evaluator (redmine issue #42)
"""

import copy
import math

UNITS =  {'':1.,
  'mum':1.e-3, 'mm':1., 'cm':10., 'm':1.e3, 'km':1.e6, 
  'ns':1., 'mus':1.e3, 'ms':1e6, 's':1.e9, 
  'eV':1e-6, 'keV':1e-3, 'MeV':1., 'GeV':1.e3, 'TeV':1e6, 
  'eV/c':1e-6, 'keV/c':1e-3, 'MeV/c':1., 'GeV/c':1.e3, 'TeV/c':1e6, 
  'eV/c2':1e-6, 'keV/c2':1e-3, 'MeV/c2':1., 'GeV/c2':1.e3, 'TeV/c2':1e6, 
  'Gauss':1.e-7, 'mT':1.e-6, 'T':1.e-3, 'kT':1.,
  'V':1.e-6, 'kV':1.e-3, 'MV':1., 'GV':1.e3, 
  'kHz':1.e-6, 'MHz':1.e-3, 'GHz':1.,
  'GV/m':1.,'GV/mm':1.e3,
  'kW':6.24150974e6, 'MW':6.24150974e9, 'GW':6.24150974e12,
  'degrees':2.*math.pi/360., 'radians':1.,
  'deg':2.*math.pi/360., 'rad':1., 
  'degree':2.*math.pi/360., 'radian':1.,
  'echarge':1., 'Coulomb':6.24150974*10.**18.
}

class Evaluator:
    """
    Evaluator class to evaluate mathematical expressions 

    Able to see many simple math expressions and some common units; the
    standard geant4 system of units is enabled.
    """

    def __init__(self):
        """
        Initialise the evaluator with math functions and units
        """
        self.variables = {}
        self.reset()

    def evaluate(self, formula):
        """
        Evaluate a string expression given by formula
        """
        out = 1.
        if formula != "":
            out = eval(formula, {"__builtins__":None}, self.variables)
        return out

    def set_variable(self, name, value):
        """
        Set a variable:value pair

        \item name string giving the variable name
        \item value gives the variable value - should be able to evaluate as a
              float
        """
        self.variables[str(name)] = float(value)

    def reset(self):
        """
        Remove all variables and start from scratch
        """
        self.variables = {}
        self.variables = copy.deepcopy(UNITS)
        my_math = ['acos', 'acosh', 'asin', 'asinh', 'atan', 'atan2', 'atanh',
                   'ceil', 'copysign', 'cos', 'cosh', 'degrees', 'e', 'erf',
                   'erfc', 'exp', 'expm1', 'fabs', 'factorial', 'floor',
                   'fmod', 'frexp', 'fsum', 'gamma', 'hypot', 'isinf',
                   'isnan', 'ldexp', 'lgamma', 'log', 'log10', 'log1p',
                   'modf', 'pi', 'pow', 'radians', 'sin', 'sinh',
                   'sqrt', 'tan', 'tanh', 'trunc']
        for item in my_math:
            self.variables[item] = math.__dict__[item]


