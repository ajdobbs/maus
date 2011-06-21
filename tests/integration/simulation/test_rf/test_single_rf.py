#!/usr/bin/env python2.7

import unittest
import run_simulation

class SingleRFTestCase(unittest.TestCase):
    def test_single_rf(self):
        my_datacards = """
simulation_geometry_filename = "PillBox.dat"
simulation_reference_particle = {"position":{"x":0.0, "y":0.0, "z":-1000.0}, "momentum":{"x":1.0, "y":0.0, "z":0.0}, "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10}
        """
        my_input = """{"mc":[{"primary":{"position":{"x":0.0, "y":-0.0, "z":0.0}, "momentum":{"x":0.0, "y":0.0, "z":1.0}, "particle_id":-13, "energy":210.0, "time":0.0, "random_seed":10}}]}"""
        run_simulation.run(my_input, my_datacards, 'test_single_rf.json')

if __name__ == '__main__':
    unittest.main()
