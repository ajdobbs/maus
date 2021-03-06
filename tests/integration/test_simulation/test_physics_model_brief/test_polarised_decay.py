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
High level tests for polarised decay; run a beam, look for decays, check the
distribution looks sensible
"""

# pylint: disable=E1101

import unittest
import subprocess
import os
import math
import libMausCpp # pylint: disable = W0611
import xboa.Common
import ROOT
ROOT.gROOT.SetBatch(True) # batch job (override if running from command line)

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
PLOT_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                    "plots", "physics_list")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                "test_simulation", "test_physics_model_brief")
SETUP_DONE = False
HIST_PERSIST = []
IS_MAIN = __name__ == "__main__"

class PolarisedDecayModelTest(unittest.TestCase): # pylint: disable = R0904
    """
    Test that physics model datacards work correctly
    """
    @classmethod
    def setUpClass(cls): # pylint: disable=C0103
        try:
            os.makedirs(PLOT_DIR)
        except OSError:
            pass # probably the directory already exists

    def setUp(self): # pylint: disable=C0103
        self.file = "${MAUS_ROOT_DIR}/tmp/test_polarised_decay"

    def run_simulation(self):
        """
        Check that polarised beam decays correctly
        """
        print "Running polarised decay simulation"
        log_file = open(os.path.expandvars(self.file+".log"), "w")
        config = os.path.join(TEST_DIR, 'polarised_decay_config.py')
        proc = subprocess.Popen([SIM_PATH, '--configuration_file', config],
                                        stdout=log_file,
                                        stderr=subprocess.STDOUT)
        proc.wait()

    def hack_data(self, pid): # pylint: disable = R0914
        """Do some simple analysis"""
        root_file = ROOT.TFile(self.file+".root", "READ")
        data = ROOT.MAUS.Data()
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)
        pol_x_list, pol_y_list, pol_z_list, cos_theta_list = [], [], [], []
        print "  Number of spills", tree.GetEntries()
        for i in range(tree.GetEntries()):
            tree.GetEntry(i)
            spill = data.GetSpill()
            for j in range(spill.GetMCEvents().size()):
                spill_event = spill.GetMCEvents()[j]
                primary = spill_event.GetPrimary()
                if primary.GetParticleId() != pid:
                    continue
                for track in spill_event.GetTracks():
                    mom = track.GetInitialMomentum()
                    ptrans = (mom.x()**2 + mom.y()**2)**0.5
                    if abs(track.GetParticleId()) == 11:
                        theta = math.atan2(ptrans, mom.z())
                        cos_theta = math.cos(theta)
                        cos_theta_list.append(cos_theta)
                for hit in spill_event.GetVirtualHits():
                    if abs(hit.GetParticleId()) == 13:
                        pol_x_list.append(hit.GetSpin().x())
                        pol_y_list.append(hit.GetSpin().y())
                        pol_z_list.append(hit.GetSpin().z())
        return pol_x_list, pol_y_list, pol_z_list, cos_theta_list 

    def check_polarisation(self, pol_x_list, pol_y_list, pol_z_list):
        """Check polarisations"""
        print "  Mean polarisation (",
        for i, a_list in enumerate([pol_x_list, pol_y_list, pol_z_list]):
            pol_mean = sum(a_list)/len(a_list)
            print pol_mean,
            if i == 0 or i == 1:
                self.assertLess(abs(pol_mean), 1e-3)
            if i == 2:
                self.assertLess(abs(1.-abs(pol_mean)), 1e-3)
        print ")"

    def make_plot(self, pid, cos_theta_list):
        """
        Check that we have dF/d(cos(f)) = 1-Pcos(f)/3 and plot
        """
        canvas = xboa.Common.make_root_canvas("cos(phi) "+str(pid))
        canvas.Draw()
        n_bins = 100
        HIST_PERSIST.append(ROOT.TH2D("hist1"+str(pid), "cos(#phi): "+str(pid),
                                      n_bins, -1.001, 1.001,
                                      n_bins, 0., 3.))
        HIST_PERSIST[-1].SetStats(False)
        HIST_PERSIST[-1].Draw()
        hist = ROOT.TH1D("hist2"+str(pid), "cos(#phi): "+str(pid),
                         n_bins, -1.001, 1.001)
        HIST_PERSIST.append(hist)
        for i, cos_theta in enumerate(cos_theta_list):
            hist.Fill(cos_theta)
        print "  Number of angles", len(cos_theta_list),
        for i in range(n_bins+2):
            if hist.GetSumOfWeights() < 1.:
                break
            content = n_bins*hist.GetBinContent(i)/len(cos_theta_list)
            hist.SetBinContent(i, content)
        hist.SetStats(False)
        hist.Draw()
        fit = ROOT.TF1("line fit", "pol1", -1., 1.)
        hist.Fit(fit, "N")
        fit.Draw("SAME")
        canvas.Update()
        parameters = [1., abs(pid)/pid*0.333]
        for i, par in enumerate(parameters):
            par_range = [par-5.*fit.GetParError(i), par+5.*fit.GetParError(i)]
            self.assertGreater(fit.GetParameter(i), par_range[0])
            self.assertLess(fit.GetParameter(i), par_range[1])
            self.assertLess(fit.GetChisquare(), 100.)
        print
        plot_name = "test_polarised_decay_p="+str(abs(pid)/pid)
        canvas.Print(PLOT_DIR+"/"+plot_name+".png")
        return hist

    def test_run(self):
        """
        Run the simulation, check that particle decays have appropriate dist
        """
        self.run_simulation()
        for pid in [-13, 13]:
            print "Analysing polarised decay data for pid", pid
            pol_x_list, pol_y_list, pol_z_list, cos_theta_list = \
                                                            self.hack_data(pid)
            self.check_polarisation(pol_x_list, pol_y_list, pol_z_list)
            self.make_plot(pid, cos_theta_list)
        if IS_MAIN:
            raw_input()

if __name__ == "__main__":
    ROOT.gROOT.SetBatch(False)
    unittest.main()

