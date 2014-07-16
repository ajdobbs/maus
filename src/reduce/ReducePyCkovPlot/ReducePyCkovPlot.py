"""
Ckov Online Plots
"""
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

#Turns off errors generated by ROOT
#pylint: disable = E1101
#pylint: disable=C0103,E1101,C0301

import ROOT
from ReducePyROOTHistogram import ReducePyROOTHistogram
class ReducePyCkovPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """Class Plots for Ckovs"""
    def __init__(self):
                
        ReducePyROOTHistogram.__init__(self)
    
        self._refresh_rate = 1
        self.root_batch_mode = 1
        ROOT.gErrorIgnoreLevel = 1001

        self._tof_cut_e = 27.00
        self._tof_cut_mu = 30.00
       
        self._hcharge = None
        self._htime = None
        self._htof = None
        self._htof_A = None
        self._htof_B = None

        self.canvas_charge = None
        self.canvas_time = None
        self.canvas_tof = None
        self.canvas_tof_A = None
        self.canvas_tof_B = None
        self.run_ended = False
        
    def _configure_at_birth(self, config_doc):
        """Set refresh for Online """
        if config_doc:
            keys = []
            keys.append("tof_cut_e")
            keys.append("tof_cut_mu")
            keys.append("reduce_plot_refresh_rate")

            for i in range(len(keys)):
                if keys[i] in config_doc:
                    self._tof_cut_e = -1e4
                    self._tof_cut_mu = -1e4
                    self._refresh_rate = int(config_doc[str(keys[2])])
        #if "refresh_rate" in config_doc:
        #    self._refresh_rate = int(config_doc["refresh_rate"])
        
        self.__init_histos()
        self.run_ended = False
        return True
    
    def _update_histograms(self, spill):
        """Update the Histograms """
        if (spill.has_key("daq_event_type") and
            spill["daq_event_type"] == "end_of_run"):
            if (not self.run_ended):
                self.update_histos()
                self.run_ended = True
                return self.get_histogram_images()
            else:
                return []
        
        if not self.get_space_points(spill):
            raise ValueError("space points not in spill")

        # Get charge points & fill histograms.
        if not self.get_charge(spill):
            raise ValueError("charge not in spill")
        
        # Refresh canvases at requested frequency.
        if self.spill_count % self._refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []

    def get_space_points(self, spill):#pylint: disable=R0911,R0912,R0914
        """Get space points from JSON"""
        
        if 'recon_events' not in spill:
            return False
        recon = spill['recon_events']

        for event in range(len(recon)):
            if 'tof_event' not in recon[event]:
                return False
            tof_event = recon[event]['tof_event']

            if 'tof_space_points' not in tof_event:
                return False

            # check for NoneType --there are events with no reconstructed SP
            space_points = tof_event['tof_space_points']
            
            if 'tof0' not in space_points:
                return False
            sp_tof0 = space_points['tof0']

            if 'tof1' not in space_points:
                return False
            sp_tof1 = space_points['tof1']

            #CKOV RECON
            if 'ckov_event' not in recon[event]:
                return False
            ckov_event = recon[event]['ckov_event']
            
            if 'ckov_digits' not in ckov_event:
                return False
            digits = ckov_event['ckov_digits']
            
            #Number of photoelectrons in CKOVa and CKOVb
            PE_B = digits[0]["B"]["number_of_pes"]
            PE_A = digits[0]["A"]["number_of_pes"]
            
            #TOF vs. Total Charge

            # check that sp_tof1 exists for this event
            if sp_tof1:
                # loop over tof1 space points in this event
                for i in range(len(sp_tof1)):
                    # check that sp_tof0 exists for this event
                    if sp_tof0 is not None:
                        # check that there is exactly one tof0 sp & 1 tof1 sp
                        if len(sp_tof0) == 1 and len(sp_tof1) == 1:
                            t_0 = sp_tof0[i]["time"]
                            t_1 = sp_tof1[i]["time"]
                            TOF = t_1 - t_0
                            self._htof.Fill(TOF)
                            #fill Histos
                            if PE_B > 0:
                                self._htof_B.Fill(PE_B, TOF)
                                
                            if PE_A > 0:
                                self._htof_A.Fill(PE_A, TOF)
        return True

    def get_charge(self, spill):
        """Get Ckov digits """

        if 'recon_events' not in spill:
            return False
        recon = spill['recon_events']

        for event in range(len(recon)):
            if 'ckov_event' not in recon[event]:
                return False
            ckov_event = recon[event]['ckov_event']
        
            if 'ckov_digits' not in ckov_event:
                return False
            digits = ckov_event['ckov_digits']

            for pmt in range(0, 8):
                pulse = "pulse_%d" % (pmt)
                arrival_time = "arrival_time_%d" % (pmt)
                
                #Fill Charge and Arrival Time Histograms for CKOVa and CKOVb
                if pmt <= 3:
                    ckov_sta = 'A'
                if pmt >= 4:
                    ckov_sta = 'B'

                charge = digits[0][ckov_sta][pulse]
                        
                if charge > -1000:
                    self._hcharge[pmt].Fill(charge)

                    time = digits[0][ckov_sta][arrival_time]
                if time < 255:
                    self._htime[pmt].Fill(time)
        
        return True
    
    def __init_histos(self): #pylint: disable=R0201,R0914,R0915
        """Draw Canvases and Histograms """

        # have root run quietly without verbose informationals
        ROOT.gErrorIgnoreLevel = 1001

        # white canvas
        ROOT.gROOT.SetStyle("Plain")
        
        #turn off stat box
        ROOT.gStyle.SetOptStat(0)
        
        #sensible color palette
        ROOT.gStyle.SetPalette(1)
        
        # xy grid on canvas
        ROOT.gStyle.SetPadGridX(1)
        ROOT.gStyle.SetPadGridY(1)

        self._hcharge = []
        
        #c_x = 800

        #Make Canvas for TOF
        
        self.canvas_tof_A = ROOT.TCanvas("tof_A", "tof_A", 600, 600)
        self._htof_A = ROOT.TH2F("tof_A", "tof_A", 100, 0, 50, 200, 15, 35)
        self._htof_A.GetXaxis().SetTitle("CkovA PE")
        self._htof_A.GetYaxis().SetTitle("t(TOF0 to TOF1) (ns)")
        self.canvas_tof_A.cd()
        self._htof_A.Draw()

        self.canvas_tof_B = ROOT.TCanvas("tof_B", "tof_B", 600, 600)
        self._htof_B = ROOT.TH2F("tof_B", "tof_B", 100, 0, 50, 200, 15, 35)
        self._htof_B.GetXaxis().SetTitle("CkovB PE")
        self._htof_B.GetYaxis().SetTitle("t(TOF0 to TOF1) (ns)")
        self.canvas_tof_B.cd()
        self._htof_B.Draw()


        self.canvas_tof = ROOT.TCanvas("tof", "tof", 600, 600)
        self._htof =  ROOT.TH1F("htof", "htof", 100, 10, 40)
        self._htof.GetXaxis().SetTitle("TOF(ns)")
        self.canvas_tof.cd()
        self._htof.Draw()
                                        

        #Make Canvas for PMT Charges
        self.canvas_charge = ROOT.TCanvas("charge", "charge", 1200, 800)
        self.canvas_charge.Divide(4, 2)
              
        for j in range(0, 8):
            #ROOT.gPad.SetLogy()

            hname = "hPMT%d" % (j+1)
            htitle = "PMT%d" % (j+1)
            nbins = 500
            x_lo = -50
            x_hi = 500

            self._hcharge.append(ROOT.TH1F(hname, htitle, nbins, x_lo, x_hi))
            self._hcharge[j].GetXaxis().SetTitle("Charge")
            self.canvas_charge.cd(j+1)
            ROOT.gPad.SetLogy()
            self._hcharge[j].Draw()

        #Make Canvas for Arrival Times

        self._htime = []
        
        self.canvas_time = ROOT.TCanvas("time", "time", 1200, 800)
        self.canvas_time.Divide(4, 2)
            
        for j in range(0, 8):
            #ROOT.gPad.SetLogy()
            
            hname = "harr_time%d" % (j+1)
            htitle = "Arrival Times%d" % (j+1)
            nbins = 200
            x_lo = 0
            x_hi = 200
            
            self._htime.append(ROOT.TH1F(hname, htitle, nbins, x_lo, x_hi))
            self._htime[j].GetXaxis().SetTitle("Uncalibrated Time (ns)")
            self.canvas_time.cd(j+1)
            self._htime[j].Draw()
        
    def update_histos(self):
        """Update Online Histos """
        self.canvas_charge.Update()
        self.canvas_time.Update()
        self.canvas_tof.Update()
        self.canvas_tof_A.Update()
        self.canvas_tof_B.Update()
        
    def get_histogram_images(self):
        """
        Get histograms as JSON documents.
        @param self Object reference.
        @returns list of 3 JSON documents containing the images.
        """
        image_list = []
        # ROOT
        # file label = PTM1-8.eps
        histos = [self._htof, self._htof_A, self._htof_B]+\
                  self._hcharge+self._htime
        tag = __name__
        content = __name__
        doc = ReducePyROOTHistogram.get_root_doc(self, [], content, tag, histos)
        image_list.append(doc)


        # PMT Charge
        # file label = PTM1-8.eps
        keywords = ['ckov', 'charge', 'pmt']
        tag = "CkovCharge_PMT1-8"
        content = "CkovCharge_PMT1-8"
        doc = ReducePyROOTHistogram.get_image_doc(self, keywords, content, tag, self.canvas_charge)
        image_list.append(doc)

        #ArrivalTimes
        #file label = ArrivalTime.eps
        keywords = ['ckov', 'time', 'pmt']
        tag = "CkovArrivalTimes_PMT1-8"
        content = "CkovArrivalTimes_PMT1-8"
        doc = ReducePyROOTHistogram.get_image_doc(self, keywords, content, tag, self.canvas_time)
        image_list.append(doc)

        #Rogers - disabled; duplicates tof reducer output
        #TOF
        #file label = "TOF"
        #keywords = ['ckov', 'tof', 'tof1-tof0']
        #tag = "TOF1-TOF0"
        #content = "TOF1-TOF0"
        #doc = ReducePyROOTHistogram.get_image_doc(self, keywords, content, tag, self.canvas_tof)
        #image_list.append(doc)                                

        
        #TOFA and Charge
        #file label = "TOF_A"
        keywords = ['ckov', 'tof', 'ckova']
        tag = "CkovPE_A_vs_TOF_0_to_1"
        content = "CkovPE_A_vs_TOF_0_to_1"
        doc = ReducePyROOTHistogram.get_image_doc(self, keywords, content, tag, self.canvas_tof_A)
        image_list.append(doc)

        #TOFB and Charge
        #file label = "TOF_B"
        keywords = ['ckov', 'tof', 'ckovb']
        tag = "CkovPE_B_vs_TOF_0_to_1"
        content = "CkovPE_B_vs_TOF_0_to_1"
        doc = ReducePyROOTHistogram.get_image_doc(self, keywords, content, tag, self.canvas_tof_B)
        image_list.append(doc)

        return image_list
                                                                            

    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()
