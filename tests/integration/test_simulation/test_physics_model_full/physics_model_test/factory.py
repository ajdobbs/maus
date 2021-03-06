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
test_factory contains TestFactory class used for generating new tests which can
subsequently be run against new datasets.

Conceptually, test data is self-describing, so given a bit of help (i.e. a test
runner), we can regenerate tests based on the input test data. The test_factory
module and TestFactory class generates some reference tests in the first place.

TestFactory generates both geometries and the tests associated with each
geometry. At the moment we have a one-to-many relationship of geometries and
tests, so the TestFactory job is to generate a geometry and for each geometry a
list of tests that run against that geometry.

The idea is to make the tests fairly independent. Note that I did end up with
some test-dependent code in build_geometry (need to fettle the axes for KSTest)
"""


import operator
import sys
import copy

import xboa.Common as Common

from physics_model_test import geometry

class TestFactory:
    """
    Class to generate reference test data from a list of substitutions.

    The reference dataset contains sufficient information to then rerun on each
    new version.
    """
    def __init__(self, code, my_tests):
        self._code = code
        self._tests  = my_tests
        self._conversions = {
                '__thickness__':self._code.convert_position,
                '__step__':self._code.convert_position,
                '__momentum__':self._code.convert_momentum,
                '__material__':self._code.convert_material,
                '__pid__':self._code.convert_pid,
                '__nev__':(lambda x: x),
                '__seed__':(lambda x: x),
               }

    def code_convert(self, config):
        """
        In order to use a common configuration for different codes, we do
        unit and other conversions here. Conversions are
            - position units: icool uses m
            - momentum units: icool uses GeV/c
            - material names: icool and g4bl have different material names
            - pid: icool uses it's own PID codes
        """
        for key, value in config.iteritems():
            if key in self._conversions.keys():
                config[key] = self._conversions[key](value)
            else:
                raise(KeyError('Nothing known about key '+str(key)) )
        return config

    def make_name(self, config):
        """Make a name for this test from substitution keys"""
        _dt  = str(config['__thickness__'])
        _mat = str(config['__material__'])
        _nev = str(config['__nev__'])
        _pz  = str(config['__momentum__'])
        _pid = Common.pdg_pid_to_name[config['__pid__']]
        _dz  = str(config['__step__'])
        return str(self._code)+': '+_dt+' mm '+_mat+' with '+_nev+' '+\
                _pz+' MeV/c '+_pid+' '+_dz+' mm steps'

    def code_parameters(self):
        """Controls how each executable will be called by MAUS"""
        code_parameters = (
            self._code.get_executable(),
            self._code.get_parameters(),
            self._code.get_substitutions()
        )
        return code_parameters

    def _get_min_max(var, bunch): # pylint: disable=R0914
        """
        Return minimum and maximum bins
        
        Choose bin based on mean_x-n_sigma_bins*sigma_x; but we calculate
        sigma_x and mean_x after applying a cut on the tails to remove outliers 
        (at mean_x-n_sigma_bins*sigma_x. var is string variable, bunch is the
        bunch.
        """
        # iterates over histogrammed bunch and looks for impurity
        # better to use raw data...
        impurity = 500./bunch.bunch_weight()
        #if impurity > 0.01:
            #impurity = 0.01
        mean = bunch.mean([var])[var]
        sigma = bunch.moment([var, var])**0.5
        print "mean", mean
        print "sigma" , sigma
        n_bins = len(bunch)/500
        min_max = [mean-7.*sigma, mean+7.*sigma]##
        for count in range(2): # pylint: disable=W0612
            bins = [min_max[0]+i*(min_max[1]-min_max[0])/float(n_bins) \
                                                         for i in range(n_bins)]
            hist = bunch.histogram_var_bins(var, bins, '')
            values = [item[0] for item in hist[0]] # get rid of numpy array
            min_max = [None, None]
            sum_wt = 0.
            for index, val in enumerate(values):
                sum_wt += val/sum(values)
                if min_max[0] == None and sum_wt > impurity/2.:
                    min_max[0] = bins[index]
                if min_max[1] == None and sum_wt > 1.-impurity/2.:
                    min_max[1] = bins[index+1]
            if min_max[0] == None:
                min_max[0] = bins[0]
            if min_max[1] == None:
                min_max[1] = bins[-1]
            delta = min_max[1] - min_max[0]
            min_max[1] += delta*0.1
            min_max[0] -= delta*0.1
            
        #min_max[1]=min_max[1]*10
        #min_max[0]=min_max[0]*10
        print "min max", min_max
        return min_max
      
    _get_min_max = staticmethod(_get_min_max)

    def build_ks_test(self, test, config, bunch):
        """
        Build a ks test based on a configuration and bunch data
        """
        pid = config['__pid__']
        test.pid = pid
        bunch.conditional_remove({'pid':pid}, operator.ne)
        print len(bunch)
        [xmin, xmax] = self._get_min_max(test.variable, bunch)
        xmin *= Common.units[test.units]
        xmax *= Common.units[test.units]
        test.n_bins = config['__nev__']/500
        test.contents = [index for index in range(test.n_bins)]
        
        test.bins = [(xmin+(xmax-xmin)*x/float(test.n_bins)) \
                      for x in range(test.n_bins+1)] 
 
        
        print "bins:", test.bins, "content:", test.contents
    
        return test

    
    def build_chi2_test(self, test, config, bunch):\
        #pylint:disable=R0902,C0111,R0201

        pid = config['__pid__']
        test.pid = pid
        bunch.conditional_remove({'pid':pid}, operator.ne)
        
        
        test.n_bins = config['__nev__']/500
        test.contents = [index for index in range(test.n_bins)]
        test.errors = [index*0 for index in range(test.n_bins)]
        test.bins = [-19.8, -16.13, -12.97, -10.27, -7.96, -5.97, \
                       -4.25, -2.79, -1.54,\
                       -0.468, 0.468, 1.54, 2.79, 4.25, 5.97, \
                        7.96, 10.27, 12.97, 16.13, 19.8]
        print "bins:", test.bins, "content:", test.contents  
    
        return test    
            

    def _get_min_max_compare(var, bunch): # pylint: disable=R0914
        """
        Return minimum and maximum bins
        
        Choose bin based on mean_x-n_sigma_bins*sigma_x; but we calculate
        sigma_x and mean_x after applying a cut on the tails to remove outliers 
        (at mean_x-n_sigma_bins*sigma_x. var is string variable, bunch is the
        bunch.
        """
        # iterates over histogrammed bunch and looks for impurity
        # better to use raw data...
        impurity = 100./bunch.bunch_weight()
       # if impurity > 0.01:
            #impurity = 0.01
        mean = bunch.mean([var])[var]
        sigma = bunch.moment([var, var])**0.5
        n_bins = len(bunch)/100
        min_max = [mean-8*sigma, mean+8*sigma]
        for count in range(2): # pylint: disable=W0612
            bins = [min_max[0]+i*(min_max[1]-min_max[0])/float(n_bins) \
                                                         for i in range(n_bins)]
            hist = bunch.histogram_var_bins(var, bins, '')
            values = [item[0] for item in hist[0]] # get rid of numpy array
            min_max = [None, None]
            sum_wt = 0.
            for index, val in enumerate(values):
                sum_wt += val/sum(values)
                if min_max[0] == None and sum_wt > impurity/2.:
                    min_max[0] = bins[index]
                if min_max[1] == None and sum_wt > 1.-impurity/2.:
                    min_max[1] = bins[index+1]
            if min_max[0] == None:
                min_max[0] = bins[0]
            if min_max[1] == None:
                min_max[1] = bins[-1]
            delta = min_max[1] - min_max[0]
            min_max[1] += delta*0.1
            min_max[0] -= delta*0.1
        return min_max
    _get_min_max_compare = staticmethod(_get_min_max_compare)

    

    def build_geometry(self, config):
        """Build and run a set of tests for a specific geometry set up"""
        geo = geometry.Geometry()
        #first setup the geometry basics
        geo.name = self.make_name(config)
        geo.code_model = self.code_parameters()
        geo.substitutions = self.code_convert(copy.deepcopy(config))
        geo.bunch_index = self._code.get_bunch_index()
        geo.bunch_read = self._code.get_bunch_read_keys()
        geo.tests = self._tests
        #now run a batch job to get axis range right on tests
        geo.run_mc()
        bunch = geo.read_bunch()
        #adjust the geometry
        for i, test in enumerate(geo.tests):
            test = self.build_chi2_test(test, config, bunch)
            geo.tests[i]  = test.run_test(bunch)
        print geo.name
        sys.stdout.flush()
        return geo

    def build_test_data(self, configurations):
        """
        Build and run a full reference data set from a list of configurations
        """
        fout_name = geometry.ref_data(self._code.get_output_filename())
        fout = open(fout_name, 'w')
        print >> fout, '[\n'
        for config in configurations:
            print config
            try:
                geo = self.build_geometry(config)
                print >> fout, repr(geo),','
            except Exception: #pylint: disable=W0703
                print 'Caught exception in configuration ', config, \
                                                        'code', str(self._code)
                sys.excepthook(*sys.exc_info())
        print >> fout, '\n]\n'
        fout.close()

