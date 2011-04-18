import json
import unittest
from Configuration import Configuration

from MapCppSimulation import MapCppSimulation

class MapCppSimulationTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.float_tolerance = 1e-9
        self.mapper = MapCppSimulation()
        success = self.mapper.Birth("""{"simulation_geometry_filename":"Test.dat", "maximum_number_of_steps":1000}""")
        self.particle = {
                "position":{"x":1.,"y":2.,"z":3.},
                "momentum":{"x":4.,"y":5.,"z":6.},
                "particle_id":-13,
                "random_seed":1,
                "energy":110.,
                "time":7.,
                "statistical_weight":1.
            }
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    @classmethod
    def tearDownClass(self):
        success = self.mapper.Death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

    ######## tests on Process #########
    def test_empty(self):
        result = self.mapper.Process("")
        doc = json.loads(result)
        self.assertIn("errors", doc)
        self.assertIn("bad_json_document", doc["errors"])

    def test_no_mc_branch(self): #mc should exist
        result = self.mapper.Process("{}")
        doc = json.loads(result)
        self.assertIn("errors", doc)
        self.assertIn("MapCppSimulation", doc["errors"])

    def test_mc_bad_type(self): #mc should be a dict
        result = self.mapper.Process("""{"mc" : 0.0}""")
        doc = json.loads(result)
        self.assertIn("errors", doc)
        self.assertIn("MapCppSimulation", doc["errors"])

    def test_mc_good(self): #should write some output
        good_event = {
            "mc":[self.particle,self.particle]
        }
        result = self.mapper.Process(json.dumps(good_event))
        doc = json.loads(result)
        self.assertNotIn("errors", doc)
        ev_0 = doc["mc"][0]["tracks"]["track1"]
        ev_1 = doc["mc"][1]["tracks"]["track1"]
        for ev in [ev_0, ev_1]:
            for ix in ["x", "y", "z"]:
                self.assertAlmostEqual( ev["initial_position"][ix], self.particle["position"][ix], msg=str(ev) )
            energy = (ev["initial_momentum"]["x"]**2+ev["initial_momentum"]["y"]**2+ev["initial_momentum"]["z"]**2+105.658**2)**0.5
            self.assertAlmostEqual( self.particle["energy"], energy, msg=str(ev), delta = 0.01 )
            self.assertIn("final_momentum", ev)
            self.assertIn("final_position", ev)

if __name__ == '__main__':
    unittest.main()

