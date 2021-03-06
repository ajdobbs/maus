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

"""
Tests the Beam class
"""

#warning/error associated with access to privates
#pylint: disable = W0212
#erroneous complaint because python changes private function names
#pylint: disable = E1101

import unittest
import beam
import xboa.hit
from xboa.hit import Hit
import copy
import numpy

MU_MASS = xboa.common.pdg_pid_to_mass[13]

TEST_PRIM_P = {
    "position":{"x":1.0, "y":2.0, "z":3.0},
    "momentum":{"x":4.0, "y":5.0, "z":6.0},
    "particle_id":2212,
    "energy":1000.0,
    "time":2.0,
    "random_seed":0
}

TEST_PRIM_MU = {
    "position":{"x":0.0, "y":0.0, "z":3.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13,
    "energy":226.0,
    "time":0.0,
    "random_seed":0
}

TEST_PRIM_MU_NEG =  {
    "position":{"x":0.0, "y":0.0, "z":3.0},
    "momentum":{"x":0.0, "y":0.0, "z":-1.0}, #going backwards
    "particle_id":-13,
    "energy":226.0,
    "time":0.0,
    "random_seed":0
}

TEST_PRIM_F1 = {
    "position":{"x":0.0, "y":0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":0.0}, # momentum vector = 0
    "particle_id":-13,
    "energy":226.0,
    "time":0.0,
    "random_seed":0
}

TEST_PRIM_F2 = {
    "position":{"x":0.0, "y":0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13,
    "energy":100.0, # energy < mass
    "time":0.0,
    "random_seed":0
}

#####################################################################

#TEST_POL = {

#"beam_polarisation" : "flat", "mean_x" : 4.0, "sigma_x" : 1.0,
#"mean_y" : 5.0, "sigma_y" : 2.0, "mean_z" : 6.0, "sigma_z" : 3.0

#}

######################################################################

TEST_REF = {'local_weight': 1.0, 'energy': 1000.0, 'pid': 2212, 'ey': 0.0, 
'ex': 0.0, 'ez': 0.0, 'e_dep': 0.0, 'pz': 236.51264524548657, 'event_number': 0,
'px': 157.67509683032438, 'py': 197.09387103790547, 'charge': 1.0, 'station': 0,
'global_weight': 1.0, 'status': 0, 'proper_time': 0.0, 'path_length': 0.0,
'particleNumber': 0, 'particle_number': 0, 'bx': 0.0, 'by': 0.0, 'bz': 0.0,
'sz': 0.0, 'sy': 0.0, 'sx': 0.0, 'eventNumber': 0, 'mass': 938.271996, 't': 2.0,
'y': 2.0, 'x': 1.0, 'z': 3.0}

TEST_PENN = {"transverse_mode":"penn", "emittance_4d":6., "beta_4d":333.,
             "alpha_4d":1., "normalised_angular_momentum":2.,
             "bz":4.e-3}

TEST_CONSTANT_SOL = {"transverse_mode":"constant_solenoid", "emittance_4d":6.,
             "normalised_angular_momentum":2., "bz":4.e-3}
TEST_CONSTANT_SOL_NEG = {"transverse_mode":"constant_solenoid",
             "emittance_4d":6., "normalised_angular_momentum":2., "bz":-4.e-3}

TEST_PENN_F1 = {"transverse_mode":"penn", "emittance_4d":-6., "beta_4d":333.,
             "alpha_4d":1., "normalised_angular_momentum":1.,
             "bz":4.e-3}

TEST_PENN_F2 = {"transverse_mode":"penn", "emittance_4d":6., "beta_4d":-333.,
             "alpha_4d":1., "normalised_angular_momentum":1.,
             "bz":4.e-3}

TEST_TWISS = {"transverse_mode":"twiss",
              'beta_x':333.0, 'alpha_x':1.,'emittance_x':6.,
              'beta_y':200.0, 'alpha_y':-1., 'emittance_y':2.}

TEST_TWISS_F1 = {"transverse_mode":"twiss",
              'beta_x':-333.0, 'alpha_x':1., 'emittance_x':6.,
              'beta_y':200.0, 'alpha_y':-1., 'emittance_y':2.}

TEST_TWISS_F2 = {"transverse_mode":"twiss",
              'beta_x':333.0, 'alpha_x':1., 'emittance_x':-6.,
              'beta_y':200.0, 'alpha_y':-1., 'emittance_y':2.}

TEST_TWISS_L = {"longitudinal_mode":"twiss", "momentum_variable":"p",
                "beta_l":10., "alpha_l":-1., "emittance_l":0.1}

TEST_TWISS_L_F1 = {"longitudinal_mode":"twiss", "momentum_variable":"p",
                "beta_l":-10., "alpha_l":-1., "emittance_l":0.1}

TEST_TWISS_L_F2 = {"longitudinal_mode":"twiss", "momentum_variable":"p",
                "beta_l":10., "alpha_l":-1., "emittance_l":-0.1}

TEST_GAUSSIAN_L1 = {"longitudinal_mode":"gaussian", "momentum_variable":"pz",
                "sigma_t":1.e6, "sigma_pz":100.}

TEST_GAUSSIAN_L2 = {"longitudinal_mode":"gaussian", "momentum_variable":"p",
                "sigma_t":100., "sigma_p":100., "cov(t,p)":9999.}

TEST_GAUSSIAN_L_F3 = {"longitudinal_mode":"gaussian", "momentum_variable":"pz",
                "sigma_t":100., "sigma_pz":100., "cov(t,pz)":10001.}


TEST_AP_1 = {"magnitude":0.1, "momentum_variable":"p"}

# "momentum_variable" defaults to "longitudinal_mode"
TEST_AP_2 = {"magnitude":0.2}

# these should throw - malformed inputs
TEST_AP_F1 = {"magnitude":0.1, "momentum_variable":"bad"}

TEST_AP_F2 = {"magnitude":"bad", "momentum_variable":"p"}

TEST_AP_F3 = {"momentum_variable":"p"}

TEST_SAWTOOTH_T = {"longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"energy",
                   "sigma_energy":100.,
                   "t_start":-100.,
                   "t_end":-75.}

TEST_UNIFORM_T = {"longitudinal_mode":"uniform_time",
                   "momentum_variable":"energy",
                   "sigma_energy":100.,
                   "t_start":-100.,
                   "t_end":750.}

TEST_UNIFORM_T_F1 = {"longitudinal_mode":"uniform_time",
                   "momentum_variable":"energy",
                   "sigma_energy":-100.,
                   "t_start":-100.,
                   "t_end":750.}

TEST_POL = {
         "beam_polarisation" : "flat" ,
         "beam_mean_x" :1.0 , "beam_mean_y" : 2.0 , "beam_mean_z" : 3.0,
         "beam_sigma_x" : 4.0 , "beam_sigma_y" : 5.0, "beam_sigma_z" : 6.0
} 

TEST_SPIN = { 'sx' : 0.0 , 'sy' : 0.0 , 'sz' : 1.0}

TEST_BIRTH = {
  "weight":0.5,
  "random_seed":10,
  "random_seed_algorithm":"incrementing_random",
  "reference":TEST_PRIM_MU,
  "transverse":TEST_PENN,
  "longitudinal":TEST_TWISS_L,
  "coupling":{"coupling_mode":"none"},
}

TEST_GAUSSIAN_UNITS = {
  "weight":0.5,
  "random_seed":10,
  "random_seed_algorithm":"incrementing_random",
  "reference":TEST_PRIM_MU,
  "transverse":TEST_PENN,
  "longitudinal":TEST_TWISS_L,
  "coupling":{"coupling_mode":"none"},
  "beam_polarisation":{"polarisation_mode": "gaussian_unit_vectors",
             "beam_sigma_x":1.,
             "beam_mean_x":2.,
             "beam_sigma_y":3.,
             "beam_mean_y":4.,
             "beam_sigma_z":30.,
             "beam_mean_z":20.} 
}

TEST_FORWARD = {
  "weight":0.5,
  "random_seed":10,
  "random_seed_algorithm":"incrementing_random",
  "reference":TEST_PRIM_MU,
  "transverse":TEST_PENN,
  "longitudinal":TEST_TWISS_L,
  "coupling":{"coupling_mode":"none"},
  "beam_polarisation":{"polarisation_mode": "gaussian_unit_vectors",
             "beam_sigma_x":1.,
             "beam_mean_x":0.,
             "beam_sigma_y":1.,
             "beam_mean_y":0.,
             "beam_sigma_z":1.,
             "beam_mean_z":200.}
  
}

TEST_NO_POL = {
  "weight":0.5,
  "random_seed":10,
  "random_seed_algorithm":"incrementing_random",
  "reference":TEST_PRIM_MU,
  "transverse":TEST_PENN,
  "longitudinal":TEST_TWISS_L,
  "coupling":{"coupling_mode":"none"}, 
}

class TestBeam(unittest.TestCase):  #pylint: disable = R0904
    """
    Tests the Beam class
    """
    @classmethod
    def setUp(cls): #pylint: disable = C0103
        """Set up tests"""
        cls._beam._Beam__birth_reference_particle({"reference":TEST_PRIM_MU})

    def test_birth_part_gen_1(self):
        """Test __birth_particle_generator for counter mode"""
        self._beam._Beam__birth_particle_generator(
                        {"n_particles_per_spill":1, 
                         "random_seed_algorithm":"incrementing_random"},
                         "counter")
        self.assertRaises(ValueError, 
                          self._beam._Beam__birth_particle_generator,
                          {"n_particles_per_spill":0,
                          "random_seed_algorithm":"incrementing_random"}, 
                          "counter")
        self._beam._Beam__birth_particle_generator({
                    "n_particles_per_spill":1,
                    "random_seed_algorithm":"incrementing_random"},
                    "counter")
        self.assertEqual(self._beam.n_particles_per_spill, 1)

    def test_birth_part_gen_2(self):
        """Test __birth_particle_generator for weighting mode"""
        self.assertRaises(ValueError,
                          self._beam._Beam__birth_particle_generator,
                          {"weight":0.}, "binomial")
        self.assertRaises(ValueError,
                          self._beam._Beam__birth_particle_generator,
                          {"weight":-1.}, "overwrite_existing")
        self._beam._Beam__birth_particle_generator({"weight":2.,
                     "random_seed_algorithm":"incrementing_random"}, "binomial")
        self.assertAlmostEqual(self._beam.weight, 2.)

    def test_birth_part_gen_3(self):
        """Test __birth_particle_generator for random number assignment"""
        seed = self._beam.beam_seed
        self._beam._Beam__birth_particle_generator({
                    "n_particles_per_spill":1,
                    "random_seed_algorithm":"incrementing_random"},
                    "counter")
        self.assertTrue(abs(self._beam.beam_seed-seed) > 5)

        self.assertRaises( ValueError, 
                    self._beam._Beam__birth_particle_generator, 
                    {"n_particles_per_spill":1,
                    "random_seed_algorithm":"bob"},
                    "counter")

    def test_birth_reference_particle(self):
        
        """Test __birth_reference_particle"""
        self._beam_no_ref._Beam__birth_reference_particle \
                                                     ({"reference":TEST_PRIM_P})
        ref = xboa.hit.Hit.new_from_dict(TEST_REF)
        self.assertEqual(
                self._beam_no_ref.reference,
                ref)
        test_broken = copy.deepcopy(TEST_PRIM_P)
        test_broken["position"] = 0
        #self.assertRaises( ValueError,
                           #self._beam_no_ref._Beam__birth_reference_particle,
                         #  ({"reference":test_broken}) )
        self._beam_no_ref._Beam__birth_reference_particle \
                                                    ({"reference":TEST_PRIM_MU})
        self.assertEqual( self._beam_no_ref.reference['pid'], -13)

    def test_birth_transverse_no_mode(self):
        """Test for bad input"""
        self.assertRaises(KeyError,
                          self._beam._Beam__birth_transverse_ellipse, {})
        self.assertRaises(ValueError,
                          self._beam._Beam__birth_transverse_ellipse,
                          {"transverse_mode":"bob"})

    def test_birth_transverse_pencil(self):
        """Test for pencil beam"""
        self._beam._Beam__birth_transverse_ellipse({"transverse_mode":"pencil"})
        self.assertTrue( numpy.all(numpy.equal(
                                   self._beam.beam_matrix[0:4, 0:4],
                                   numpy.diag([1.]*4)
                       )))
        self.assertEqual( self._beam.transverse_mode, "pencil" )

    def test_birth_transverse_penn(self):
        """Beam transverse Penn mode"""
        test = \
        [[1.05668599e+03, -6.33950201e+02,  0.00000000e+00,  6.34327423e+02],
         [-6.33950201e+02, 1.14145263e+03, -6.34327423e+02,  0.00000000e+00],
         [0.00000000e+00, -6.34327423e+02,  1.05668599e+03, -6.33950201e+02],
         [6.34327423e+02,  0.00000000e+00, -6.33950201e+02,  1.14145263e+03]]
        test_array = numpy.array(test)
        self._beam._Beam__birth_transverse_ellipse(TEST_PENN)
        self.assertRaises(ValueError,
              self._beam._Beam__birth_transverse_ellipse, TEST_PENN_F1)
        self.assertRaises(ValueError,
              self._beam._Beam__birth_transverse_ellipse, TEST_PENN_F2)
        self.assertEqual( self._beam.transverse_mode, "penn" )
        self.__cmp_matrix(test_array, self._beam.beam_matrix[0:4, 0:4])

    # TEST_CONSTANT_SOL_NEG is a test for issue #1211
    def test_birth_transverse_const_sol(self):
        """Beam transverse constant solenoid mode"""
        self._beam._Beam__birth_transverse_ellipse(TEST_CONSTANT_SOL)
        matrix_const_sol = self._beam.beam_matrix[0:4, 0:4]
        self._beam._Beam__birth_transverse_ellipse(TEST_CONSTANT_SOL_NEG)
        matrix_const_sol_neg = self._beam.beam_matrix[0:4, 0:4]
        for i in range(4):
            matrix_const_sol_neg[i, i-4] = -matrix_const_sol_neg[i, i-4]
        penn_mod = copy.deepcopy(TEST_PENN)
        penn_mod["alpha_4d"] = 0.
        self._beam._Beam__birth_transverse_ellipse(penn_mod)
        matrix_penn = self._beam.beam_matrix[0:4, 0:4]
        self.__cmp_matrix(matrix_penn, matrix_const_sol)
        self.__cmp_matrix(matrix_penn, matrix_const_sol_neg)

    def test_birth_transverse_twiss(self):
        """Beam transverse twiss mode"""
        self._beam._Beam__birth_transverse_ellipse(TEST_TWISS)
        test_matrix = self._beam.beam_matrix[0:4, 0:4]
        test_ref = numpy.array(
                  [[1.05668599e+03, -6.33950201e+02, 0.00000000e+00, 0.00e+00], 
                   [-6.33950201e+02, 7.60666579e+02, 0.00000000e+00, 0.00e+00], 
                   [0.000e+00, 0.00000000e+00, 2.11548746e+02, 2.11316734e+02], 
                   [0.000e+00, 0.00000000e+00, 2.11316734e+02, 4.22169951e+02]])
        self.__cmp_matrix(test_ref, test_matrix)
        self.assertRaises(ValueError, 
              self._beam._Beam__birth_transverse_ellipse, TEST_TWISS_F1)
        self.assertRaises(ValueError, 
              self._beam._Beam__birth_transverse_ellipse, TEST_TWISS_F2)
        self.assertEqual( self._beam.transverse_mode, "twiss" )

    def test_birth_longitudinal_no_mode(self):
        """Beam longitudinal - bad user input"""
        self.assertRaises( KeyError,
                               self._beam._Beam__birth_longitudinal_ellipse, {})
        self.assertRaises( KeyError,
                           self._beam._Beam__birth_longitudinal_ellipse, 
                           {"longitudinal_mode":"pencil"})
        self.assertRaises( ValueError,
                           self._beam._Beam__birth_longitudinal_ellipse, 
                           {"longitudinal_mode":"pencil", 
                            "momentum_variable":"bob"})
        self.assertRaises( ValueError,
                           self._beam._Beam__birth_longitudinal_ellipse, 
                           {"longitudinal_mode":"bob", "momentum_variable":"p"})

    def test_birth_longitudinal_pencil(self):
        """Beam longitudinal - pencil mode"""
        for mom_var in ["p", "pz", "energy"]:
            self._beam._Beam__birth_longitudinal_ellipse(
                {"longitudinal_mode":"pencil", "momentum_variable":mom_var})
        self.assertTrue( numpy.all(numpy.equal(self._beam.beam_matrix,
                                               numpy.diag([1.]*6) )) )
        self.assertEqual( self._beam.longitudinal_mode, "pencil" )

    def test_birth_long_twiss(self):
        """Beam longitudinal - twiss mode"""
        self._beam._Beam__birth_longitudinal_ellipse(TEST_TWISS_L)
        self.assertEqual( self._beam.longitudinal_mode, "twiss" )
        ref_matrix = numpy.array([[+5.28871865e-01, +1.05658367e+01],
                                   [+1.05658367e+01, +4.22169951e+02]])
        self.__cmp_matrix(ref_matrix, self._beam.beam_matrix[4:, 4:])
        self.assertRaises(ValueError,
                          self._beam._Beam__birth_longitudinal_ellipse,
                          TEST_TWISS_L_F1)
        self.assertRaises(ValueError,
                          self._beam._Beam__birth_longitudinal_ellipse,
                          TEST_TWISS_L_F2)

    def test_birth_long_gaussian(self):
        """Beam longitudinal - gaussian mode"""
        self._beam._Beam__birth_longitudinal_ellipse(TEST_GAUSSIAN_L1)
        self.assertEqual(self._beam.longitudinal_mode, "gaussian")
        equality_matrix = numpy.equal(self._beam.beam_matrix,
                                      numpy.diag([1.]*4+[1.e12, 1.e4]))
        self.assertTrue(numpy.all(equality_matrix))
        self._beam._Beam__birth_longitudinal_ellipse(TEST_GAUSSIAN_L2)
        self.assertEqual( self._beam.longitudinal_mode, "gaussian" )
        test_matrix = numpy.diag([1.]*4+[1.e4, 1.e4])
        test_matrix[4, 5] = 100.*100.-1.
        test_matrix[5, 4] = 100.*100.-1.
        equality_matrix = numpy.equal(self._beam.beam_matrix, test_matrix)
        self.assertTrue(numpy.all(equality_matrix))
        self.assertRaises(ValueError,
                          self._beam._Beam__birth_longitudinal_ellipse,
                          TEST_GAUSSIAN_L_F3)

    def test_birth_long_t_dist(self):
        """Beam longitudinal - sawtooth/uniform time distributions"""
        self._beam._Beam__birth_longitudinal_ellipse(TEST_UNIFORM_T)
        self.assertEqual(self._beam.longitudinal_mode, "uniform_time")
        self.assertAlmostEqual(self._beam.t_start, TEST_UNIFORM_T["t_start"])
        self.assertAlmostEqual(self._beam.t_end, TEST_UNIFORM_T["t_end"])
        self.assertAlmostEqual(self._beam.beam_matrix[5, 5], 1.e4)
        self.assertRaises(ValueError, 
                          self._beam._Beam__birth_longitudinal_ellipse,
                          TEST_UNIFORM_T_F1)

        self._beam._Beam__birth_longitudinal_ellipse(TEST_SAWTOOTH_T)
        self.assertEqual(self._beam.longitudinal_mode, "sawtooth_time")
        self.assertAlmostEqual(self._beam.t_start, TEST_SAWTOOTH_T["t_start"])
        self.assertAlmostEqual(self._beam.t_end, TEST_SAWTOOTH_T["t_end"])
        self.assertAlmostEqual(self._beam.beam_matrix[5, 5], 1.e4)

    def test_birth_trans_long_coupling(self):
        """Beam transverse - longitudinal coupling"""
        self._beam._Beam__birth_trans_long_coupling({"coupling_mode":"none"})
        self.assertRaises(NotImplementedError,
                          self._beam._Beam__birth_trans_long_coupling,
                          {"coupling_mode":"bob"})

    def test_birth_beam_mean(self):
        """Beam mean setup"""
        for mom_var in ["p", "pz", "energy"]:
            self._beam._Beam__birth_longitudinal_ellipse(
                {"longitudinal_mode":"pencil", "momentum_variable":mom_var})
            self._beam._Beam__birth_beam_mean()
            for i, key in enumerate(['x', 'px', 'y', 'py', 't']):
                self.assertAlmostEqual(self._beam.reference[key],
                                       self._beam.beam_mean[i])
            self.assertAlmostEqual(self._beam.reference[mom_var],
                                   self._beam.beam_mean[5])

    def test_birth_beam_polarisation(self):
        """tests polarisation"""
        self._beam._Beam__birth_beam_polarisation(TEST_BIRTH)
        self.assertEquals(self._beam.beam_polarisation['polarisation_mode'],
                          'flat')
        self.assertEquals(self._beam.beam_mean_x, 0.0)
        self.assertEquals(self._beam.beam_sigma_x, 1.0)
        self.assertEquals(self._beam.beam_mean_y, 0.0)
        self.assertEquals(self._beam.beam_sigma_y, 1.0)
        self.assertEquals(self._beam.beam_mean_z, 0.0)
        self.assertEquals(self._beam.beam_sigma_z, 1.0)
        print "polarization test_beam", self._beam.beam_polarisation
        self._beam._Beam__birth_beam_polarisation(TEST_GAUSSIAN_UNITS)
        self.assertEquals(self._beam.beam_polarisation['polarisation_mode'],
                         'gaussian_unit_vectors')
        self.assertEquals(self._beam.beam_mean_x, 2.0)
        self.assertEquals(self._beam.beam_sigma_x, 1.0)
        self.assertEquals(self._beam.beam_mean_y, 4.0)
        self.assertEquals(self._beam.beam_sigma_y, 3.0)
        self.assertEquals(self._beam.beam_mean_z, 20.0)
        self.assertEquals(self._beam.beam_sigma_z, 30.0)
        print "polarization test_beam", self._beam.beam_polarisation

    def test_birth_a_p_correlation(self):
        """Amplitude momentum correlation  - at birth"""
        self._beam.momentum_defined_by = "energy"
        self._beam._Beam__birth_a_p_correlation({"a-p_correlation":TEST_AP_1})
        self.assertEquals(self._beam.a_p_correlation["magnitude"], 0.1)
        self.assertEquals(self._beam.a_p_correlation["momentum_variable"], "p")

        self._beam._Beam__birth_a_p_correlation({"a-p_correlation":TEST_AP_2})
        self.assertEquals(self._beam.a_p_correlation["magnitude"], 0.2)
        self.assertEquals(self._beam.a_p_correlation["momentum_variable"],
                          "energy")

        for test, exc in [(TEST_AP_F1, KeyError),
                          (TEST_AP_F2, ValueError),
                          (TEST_AP_F3, KeyError)]:
            try:
                test_def = {"a-p_correlation":test}
                self._beam._Beam__birth_a_p_correlation(test_def)
                raise(RuntimeError("should have thrown"))
            except exc:
                pass

    def test_birth(self):
        """ Overall check birth works """
        a_beam = beam.Beam()
        self.assertRaises(KeyError, a_beam.birth, TEST_BIRTH, "counter", 2)
        
    def test_process_array_to_primary(self):
        """Check function that converts from an array to a primary particle"""
        mean = numpy.array([10., 20., 30., 40., 50., 600.])
        mass = xboa.common.pdg_pid_to_mass[13]
        self._beam.beam_seed = 10
        self._beam.particle_seed_algorithm = "beam_seed"
        primary_hit = self._beam._Beam__process_array_to_hit(mean, 13, 'p')
        primary = self._beam._Beam__process_hit_to_primary(primary_hit)
        self.assertAlmostEqual(primary["position"]["x"], 10.)
        self.assertAlmostEqual(primary["position"]["y"], 30.)
        self.assertAlmostEqual(primary["position"]["z"], 3.) # from ref
        self.assertAlmostEqual(primary["momentum"]["x"], 20.)
        self.assertAlmostEqual(primary["momentum"]["y"], 40.)
        self.assertAlmostEqual(primary["momentum"]["z"],
                          (600.**2.-20**2.-40.**2.)**0.5)
        self.assertAlmostEqual(primary["energy"], (600.**2.+mass**2.)**0.5)
        self.assertAlmostEqual(primary["time"], 50.)
        self.assertEqual(primary["particle_id"], 13)
        self.assertEqual(primary["random_seed"], 10)

        primary_hit = self._beam._Beam__process_array_to_hit(mean, 13, 'pz')
        primary = self._beam._Beam__process_hit_to_primary(primary_hit)
        self.assertAlmostEqual(primary["momentum"]["x"], 20.)
        self.assertAlmostEqual(primary["momentum"]["y"], 40.)
        self.assertAlmostEqual(primary["momentum"]["z"], 600.)
        self.assertAlmostEqual(primary["energy"],
                          (600.**2.+20.**2.+40.**2.+mass**2.)**0.5)

        primary_hit = self._beam._Beam__process_array_to_hit(mean,
                                                                  -13, 'energy')
        primary = self._beam._Beam__process_hit_to_primary(primary_hit)
        self.assertAlmostEqual(primary["momentum"]["x"], 20.)
        self.assertAlmostEqual(primary["momentum"]["y"], 40.)
        self.assertAlmostEqual(primary["momentum"]["z"],
              (600.**2.-xboa.common.pdg_pid_to_mass[13]**2.-20.**2-40.**2)**0.5)
        self.assertAlmostEqual(primary["energy"], 600.)

    def test_process_array_to_primary_sign(self): # pylint: disable = C0103
        """Check function that converts from an array to a primary particle"""
        mean = numpy.array([10., 20., 30., 40., 50., 600.])
        self._beam._Beam__birth_reference_particle \
                                                ({"reference":TEST_PRIM_MU_NEG})
        self._beam.beam_seed = 10
        self._beam.particle_seed_algorithm = "beam_seed"
        primary_hit = self._beam._Beam__process_array_to_hit(mean, 13, 'p')
        primary = self._beam._Beam__process_hit_to_primary(primary_hit)
        self.assertLess(primary["momentum"]["z"], 0.)
        primary_hit = self._beam._Beam__process_array_to_hit(mean, 13, 'pz')
        primary = self._beam._Beam__process_hit_to_primary(primary_hit)
        self.assertLess(primary["momentum"]["z"], 0.)
        primary_hit = self._beam._Beam__process_array_to_hit(mean, 13, 'energy')
        primary = self._beam._Beam__process_hit_to_primary(primary_hit)
        self.assertLess(primary["momentum"]["z"], 0.)


    def test_make_one_primary_gaus(self):
        """Check function that throws a particle - for gaussian distribution"""
        a_beam = beam.Beam()
        a_beam.birth(TEST_BIRTH, "binomial", 2)
        for i in range(1000): # pylint: disable = W0612
            primary = a_beam.make_one_primary()
            hit = xboa.hit.factory.MausJsonHitFactory.\
                                     hit_from_maus_object('primary', primary, 0)
            self.assertTrue(hit.check())

    def test_make_one_primary_pencil(self):
        """Check function that throws a particle - for pencil beam"""
        a_beam = beam.Beam()
        test_birth = {
          "weight":0.5,
          "random_seed":10,
          "random_seed_algorithm":"incrementing_random",
          "reference":TEST_PRIM_MU,
          "transverse":{"transverse_mode":"pencil"},
          "longitudinal":{"longitudinal_mode":"pencil","momentum_variable":"p"},
          "coupling":{"coupling_mode":"none"},
        }
        a_beam.birth(test_birth, "binomial", 2)
        for i in range(10): #pylint: disable = W0612
            primary = a_beam.make_one_primary()
            self.assertEqual(a_beam.reference,
                  xboa.hit.factory.MausJsonHitFactory.
                               hit_from_maus_object('primary', primary, 0))

    def test_make_one_primary_sawtooth(self):
        """Check function that throws a particle - for sawtooth time dist"""
        a_beam = beam.Beam()
        test_birth = {
          "weight":0.5,
          "random_seed":10,
          "random_seed_algorithm":"incrementing_random",
          "reference":TEST_PRIM_MU,
          "transverse":{"transverse_mode":"pencil"},
          "longitudinal":TEST_SAWTOOTH_T,
          "coupling":{"coupling_mode":"none"},
        }
        a_beam.birth(test_birth, "binomial", 2)
        for i in range(1000): #pylint: disable = W0612
            primary = a_beam.make_one_primary()
            hit = xboa.hit.factory.MausJsonHitFactory.\
                                hit_from_maus_object('primary', primary, 0)
            self.assertGreater(hit['t'], TEST_SAWTOOTH_T["t_start"])
            self.assertLess(hit['t'], TEST_SAWTOOTH_T["t_end"])
            self.assertGreater(hit['energy'], xboa.common.pdg_pid_to_mass[13])
            self.assertTrue(hit.check())
                  
    def test_make_one_primary_uniform(self):
        """Check function that throws a particle - for uniform time dist"""
        a_beam = beam.Beam()
        test_birth = {
          "weight":0.5,
          "random_seed":10,
          "random_seed_algorithm":"incrementing_random",
          "reference":TEST_PRIM_MU,
          "transverse":{"transverse_mode":"pencil"},
          "longitudinal":TEST_UNIFORM_T,
          "coupling":{"coupling_mode":"none"},
        #  "beam_polarisation" : "Gaussian_Unit_Vectors"
        }
        a_beam.birth(test_birth, "binomial", 2)
        for i in range(1000): #pylint: disable = W0612
            primary = a_beam.make_one_primary()
            hit = xboa.hit.factory.MausJsonHitFactory.\
                                hit_from_maus_object('primary', primary, 0)
            self.assertGreater(hit['t'], TEST_UNIFORM_T["t_start"])
            self.assertLess(hit['t'], TEST_UNIFORM_T["t_end"])
            self.assertGreater(hit['energy'], xboa.common.pdg_pid_to_mass[13])
            self.assertTrue(hit.check())

    def test_process_get_seed(self):
        """Check we generate the seed correctly"""
        self._beam.beam_seed = 9
        self._beam.particle_seed_algorithm = "beam_seed"
        self.assertEqual(self._beam._Beam__process_get_seed(), 
                         self._beam.beam_seed)

        self._beam.particle_seed_algorithm = "random"
        self.assertNotEqual(self._beam._Beam__process_get_seed(), 
                            self._beam._Beam__process_get_seed())
        self.assertTrue(abs(self._beam._Beam__process_get_seed()-\
                            self._beam._Beam__process_get_seed()) > 5)

        self._beam.beam_seed = 9
        self._beam.particle_seed_algorithm = "incrementing"
        self.assertEqual(self._beam._Beam__process_get_seed(), 9)
        self.assertEqual(self._beam._Beam__process_get_seed(), 10)
        self.assertEqual(self._beam._Beam__process_get_seed(), 11)
        # assigns beam_seed to random number elsewhere
        self._beam.particle_seed_algorithm = "incrementing_random"
        self.assertEqual(self._beam._Beam__process_get_seed(), 12)
        self.assertEqual(self._beam._Beam__process_get_seed(), 13)
        self.assertEqual(self._beam._Beam__process_get_seed(), 14)

    def test_process_beam_polarisation(self):
       
        """tests polarisation"""
        a_beam = beam.Beam()
        array = []
       
        self._beam._Beam__birth_beam_polarisation(TEST_FORWARD)    
        array = self._beam._Beam__process_beam_polarisation()  
        self.assertAlmostEqual(
              ((array[0]**2)+ (array[1]**2)+(array[2]**2))**0.5, 1.0)
        self.assertTrue(array[0] < array[2] and array[1] < array[2])
        flag_more_x = False
        flag_less_x = False
        flag_more_y = False
        flag_less_y = False
        flag_more_z = False
        flag_less_z = False
        for i in range(100): # pylint: disable=W0612
            
            self._beam._Beam__birth_beam_polarisation(TEST_NO_POL)  
            array = self._beam._Beam__process_beam_polarisation()  
            
            if array[0] > 0.0:
                flag_more_x = True
            if array[0] < 0.0:
                flag_less_x = True
            if array[1] > 0.0:
                flag_more_y = True
            if array[1] < 0.0:
                flag_less_y = True
            if array[2] > 0.0:
                flag_more_z = True
            if array[2] < 0.0:
                flag_less_z = True
          

            self.assertEqual(flag_more_x or flag_less_x, True) 
            self.assertEqual(flag_more_y or flag_less_y, True) 
            self.assertEqual(flag_more_z or flag_less_z, True) 
            a_beam.make_one_primary()

    def test_process_a_p_correlation(self):
        """Test beam.py a-p correlation routine"""
        self._beam.beam_matrix = numpy.diag([0.1]*6)
        self._beam.beam_mean = numpy.zeros((6))
        self._beam.a_p_correlation = {"momentum_variable":"pz", "magnitude":0.5}
        ref_pz = 200.
        hit1 = Hit.new_from_dict({"pz":ref_pz, "mass":105.658}, "energy")
        hit2  = hit1.deepcopy()
        self._beam._Beam__process_a_p_correlation(hit2)
        self.assertEqual(hit1, hit2)
        for matrix in [numpy.diag([0.1]*6), numpy.diag([0.2]*6)]:
            self._beam.beam_matrix = matrix
            hit2 = hit1.deepcopy()
            hit3 = hit1.deepcopy()
            hit2['x'] = 2.
            hit3['px'] = 2.
            self._beam._Beam__process_a_p_correlation(hit2)
            self._beam._Beam__process_a_p_correlation(hit3)
            self.assertAlmostEqual(hit2['pz'], hit3['pz'])
            self.assertAlmostEqual(hit2['pz']-ref_pz, ref_pz*0.5*4.*105.658**2)

    def __cmp_matrix(self, ref_matrix, test_matrix):
        """Compare to numpy matrices"""
        self.assertEqual(ref_matrix.shape, test_matrix.shape)
        msg = "\nReference:\n"+str(ref_matrix)+\
              "\nIn Test:\n"+str(test_matrix)
        for i in range(ref_matrix.shape[0]):
            for j in range(test_matrix.shape[1]):
                self.assertAlmostEqual(ref_matrix[i, j], 
                                       test_matrix[i, j], 3, msg)

    _beam = beam.Beam()
    _beam_no_ref = beam.Beam()

if __name__ == "__main__":
    unittest.main()


