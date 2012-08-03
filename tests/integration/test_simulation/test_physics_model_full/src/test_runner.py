#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""
Framework for Application-level tests for G4MICE

Application-level tests for MAUS simulation (and other monte carlo codes)
usually involve running an application against some geometry files and then
running a set of tests on the output. code_comparison provides a framework for
these sorts of tests. In code_comparison we define a geometry object and
associate with each geometry object a set of test objects. 

* The geometry object knows enough to set-up a geometry and any other input data
   and run the code. Because it's faster, the geometry also reads the output.
* The tests know enough to read in any output files from the application, parse
  them appropriately and calculate appropriate test variables on the output
* eval( repr(geometry) ) should recreate a copy of the geometry. Then storage of
  reference datasets and geometries is done by writing repr(geometry) to disk

In this file I define

* geometry = geometry object for running simulation code as described above
* test     = test abstract data type
* ks_test  = implementation of test that does a Kolmogorov-Smirnov test of some 
             user-defined list of variables against some bunch

This can be run as a standalone program from the command line, in which case it 
takes two arguments

  --in=<some_file> defines input file which contains reference geometry objects 
                   and tests
  --out=<some_file> defines output file; will write updated reference data to
                    out

"""

#TODO: #pylint: disable=W0511 
# * at the moment the read of simulation data is done at geometry level, because
#   this is quicker. But some tests may want to do it per test, so this option
#   should be enabled.
# * add some more test types; moments, Twiss parameters
# * it's uncomfortable that I open the file and do eval(<entire_file>) - can
#   lead to stability issues and a pain to debug. This should probably be
#   improved
# * what happens if I want to do a test that crosses geometries? For example, 
#   check that within a particular version step size doesn't affect material
#   model. Solution - add a test manager class that holds map of (geo_1, 
#   geo_2,...):(test_1, test_2, ...)??? Not sure

#Note: I always use a static function to initialise objects so that I can deal
#      with wanting more than one constructor with the same number of parameters

import sys
import os

from tests import Test
from tests import KSTest
from tests import HitEqualityTest
from geometry import Geometry

def read_geometries(ref_data_in):
    """
    Returns a list of geometry objects generated from a reference data set
      ref_data_in = the file name (string)
    """
    try:
        fin = open(ref_data_in)
    except:
        raise(IOError('Failed to open file '+str(ref_data_in)))
    geometries = eval(fin.read())
    return geometries

def write_geometries(ref_data_out, geometries):
    """
    Writes a list of geometry objects generated from a reference data set
      ref_data_out = the file name (string)
      geometries   = write the geometries
    """
    if ref_data_out != None:
        print 'Writing output to '+ref_data_out
    fout = open(ref_data_out, 'w')
    print >> fout, repr(geometries)
    fout.close()

def run_tests(geometries_in, geometries_out):
    """
    Run all the geometries in the list
      geometry_list = list of geometry objects
    """
    here = os.getcwd()
    (passes, fails, warns)=(0, 0, 0)
    for geo_in in geometries_in: 
        try:
            os.chdir(os.path.expandvars('$MAUS_ROOT_DIR/tmp'))
            geo_out = geo_in.deepcopy()
            (my_passes, my_fails, my_warnings) = geo_out.run_tests()
            passes += my_passes
            fails  += my_fails
            warns  += my_warnings
            geometries_out.append(geo_out)
            sys.stdout.flush()
        except Exception: #pylint: disable=W0703
            print 'Caught exception in geometry ', geo_in.name
            sys.excepthook(*sys.exc_info())
        finally:
            os.chdir(here)
    return (passes, fails, warns)


def code_comparison_test(ref_data_in, ref_data_out):
    """
    Main loop:- parses the input file and calls functions to run the tests and
    write output
    """
    (passes, fails, warns) = (0, 0, 0)
    geometries_out = []
    fin  = open(ref_data_in)
    (passes, fails, warns) = run_tests(eval(fin.read()), geometries_out)  
    write_geometries(ref_data_out, geometries_out)
    for geo in geometries_out:
        print geo
    return (passes, fails, warns)

def main(argv=None):
    """
    If called directly from the command line, parse command line arguments as 
    file names and run the tests.
    """
    if argv == None:
        argv = sys.argv
    arg_dict = {}
    for arg in argv: 
        if( len(arg.split('='))>1 ):
            arg_dict[arg.split('=')[0]] = arg.split('=')[1]
    arg_list = ['--in','--out']
    args = ['', '']
    for i, arg in enumerate(arg_list):
        if arg in arg_dict:
            args[i] = arg_dict[arg]
        else:
            args[i] = None
    (passes, fails, warns) = code_comparison_test(args[0], args[1])
    print '========================='
    print '|| PhysicsModelTest    ||'
    print '========================='
    print '\n'+str(passes)+' passes   '+str(fails)+' fails    '+\
               str(warns)+' warnings\n'
    return fails

if __name__ == "__main__":
    sys.exit(main())



