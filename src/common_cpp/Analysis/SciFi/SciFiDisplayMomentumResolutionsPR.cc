/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// C++ headers
#include <vector>
#include <sstream>

// ROOT headers
#include "TAxis.h"
#include "TH1D.h"
#include "TF1.h"

// MAUS headers
#include "src/common_cpp/Analysis/SciFi/SciFiDisplayMomentumResolutionsPR.hh"
#include "src/common_cpp/Plotting/SciFi/SciFiAnalysisTools.hh"
#include "src/common_cpp/Recon/SciFi/SciFiLookup.hh"
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/MCEvent.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiCluster.hh"
#include "src/common_cpp/DataStructure/SciFiDigit.hh"
#include "src/common_cpp/DataStructure/Hit.hh"
#include "src/common_cpp/DataStructure/ThreeVector.hh"

namespace MAUS {

SciFiDisplayMomentumResolutionsPR::SciFiDisplayMomentumResolutionsPR() : mOf1(NULL),
                                                                         mTree(NULL),
                                                                         mSpillData(NULL),
                                                                         _t1_pt_resol_pt_mc(NULL),
                                                                         _t2_pt_resol_pt_mc(NULL),
                                                                         _t1_pz_resol_pt_mc(NULL),
                                                                         _t2_pz_resol_pt_mc(NULL),
                                                                         _t1_pt_resol_pz_mc(NULL),
                                                                         _t2_pt_resol_pz_mc(NULL),
                                                                         _t1_pz_resol_pz_mc(NULL),
                                                                         _t2_pz_resol_pz_mc(NULL),
                                                                         _n_pt_bins(200),
                                                                         _n_pz_bins(200),
                                                                         _n_points(9),
                                                                         _pt_fit_min(-50),
                                                                         _pt_fit_max(50),
                                                                         _pz_fit_min(-50),
                                                                         _pz_fit_max(50),
                                                                         _upper_bound_pzmc(250.0),
                                                                         _lower_bound_pzmc(150.0),
                                                                         _resol_lower_bound(0.0),
                                                                         _resol_upper_bound(90.0),
                                                                         _cut_pz_rec(500.0) {
  // Do nothing
}

SciFiDisplayMomentumResolutionsPR::~SciFiDisplayMomentumResolutionsPR() {
  if (mTree) delete mTree;
  if (mSpillData) delete mSpillData;
  if (_t1_pt_resol_pt_mc) delete _t1_pt_resol_pt_mc;
  if (_t2_pt_resol_pt_mc) delete _t2_pt_resol_pt_mc;
  if (_t1_pz_resol_pt_mc) delete _t1_pz_resol_pt_mc;
  if (_t2_pz_resol_pt_mc) delete _t2_pz_resol_pt_mc;
  if (_t1_pz_resol_pz_mc) delete _t1_pz_resol_pz_mc;
  if (_t2_pz_resol_pz_mc) delete _t2_pz_resol_pz_mc;
  if (mOf1) {
    mOf1->Close();
    delete mOf1;
  }
}

bool SciFiDisplayMomentumResolutionsPR::calc_resolution(const std::string& residual,
                                                        const TCut cut, double &res,
                                                        double &res_err) {
  if (mTree) {
    mOf1->cd();
    TCanvas lCanvas;
    TH1D* hResidual(NULL);

    // Create a histogram of the input residual for the mc variable interval defined by input cut
    std::string htitle = residual + " " + cut.GetTitle();
    hResidual = new TH1D("hResidual", htitle.c_str(), _n_pz_bins, _pz_fit_min, _pz_fit_max);
    mTree->Draw((residual + ">>hResidual").c_str(), cut);

    if (!hResidual) {
      std::cerr << "SciFiDisplayMomentumResolutionsPR::calc_resolution: Invalid tracker #\n";
      return false;
    }

    // Fit a gaussian to the histogram
    hResidual->Fit("gaus", "", "", _pz_fit_min, _pz_fit_max);
    TF1 *fit1 = hResidual->GetFunction("gaus");
    if (!fit1) {
      std::cerr << "SciFiDisplayMomentumResolutionsPR::calc_resolution: Fit failed\n";
      return false;
    }

    // Extract the sigma of the gaussian fit (equivalent to the pt resolution for this interval)
    res = fit1->GetParameter(2);
    res_err = fit1->GetParError(2);

    hResidual->Write();

    return true;
  } else {
    std::cerr << "SciFiDisplayMomentumResolutionsPR::Tree pointer invalid" << std::endl;
    return false;
  }
}

void SciFiDisplayMomentumResolutionsPR::Fill() {
  mOf1->cd();
  if (mTree) {
    // Loop over the data for each track in the spill and fill the tree with reduced data
    for ( size_t i = 0; i < mSpillData->mData.size(); ++i ) {
      mTrackData = mSpillData->mData[i];
      mTree->Fill();
      if (mTrackData.TrackerNumber == 0) {
        // TODO: Do I want anything here?
      } else if (mTrackData.TrackerNumber == 1) {
      }
    }
  } else {
    std::cerr << "SciFiDisplayMomentumResolutionsPR: Warning, local ROOT Tree not setup\n";
  }
}

TCut SciFiDisplayMomentumResolutionsPR::form_tcut(const std::string &var, const std::string &op,
                                                  double value) {
  TCut cut = "";
  std::stringstream ss1;
  TString s1;
  ss1 << var << op << value;
  ss1 >> s1;
  cut = s1;
  return cut;
}

void SciFiDisplayMomentumResolutionsPR::make_ptpt_resolutions() {

  double x_range = _resol_upper_bound - _resol_lower_bound;  // Range of the pz resolution graph
  double resolution_error =  x_range / ( _n_points * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = _resol_lower_bound + resolution_error;

  std::vector<TCut> vCuts(_n_points);          // The cuts defining the pt_mc intervals
  std::vector<double> vPtMc(_n_points);        // The centre of the pt_mc intervals
  std::vector<double> vPtMcErr(_n_points);     // The width of the intervals
  std::vector<double> vPtRes_t1(_n_points);    // The resultant pt resolutions
  std::vector<double> vPtResErr_t1(_n_points); // The errors associated with the pt res
  std::vector<double> vPtRes_t2(_n_points);    // The resultant pt resolutions
  std::vector<double> vPtResErr_t2(_n_points); // The errors associated with the pt res

  // Cuts in pt_mc
  std::string s1 = "PtMc>=";
  std::string s2 = "&&PtMc<";
  for (int i = 0; i < _n_points; ++i) {
    std::stringstream ss1;
    double point_lower_bound = _resol_lower_bound + (resolution_error * 2 * i);
    double point_upper_bound = point_lower_bound + (resolution_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < _n_points; ++i) {
    vPtMc[i] = first_resolution_point + (resolution_error * 2 * i);
    vPtMcErr[i] = resolution_error;
    std::cerr << "vPtMc[" << i << "] = " << vPtMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pt TCut
  // TCut tcut_ptrec = form_tcut("TMath::Abs(pt_rec)", "<", _cutPtRec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT1&&tcut_ptrec;
    TCut input_cut = vCuts[i]&&cutT1;
    calc_resolution(residual, input_cut, vPtRes_t1[i], vPtResErr_t1[i]);
  }
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT2&&tcut_ptrec;
    TCut input_cut = vCuts[i]&&cutT2;
    calc_resolution(residual, input_cut, vPtRes_t2[i], vPtResErr_t2[i]);
  }

  // Create the resultant resolution plots
  _t1_pt_resol_pt_mc = new TGraphErrors(_n_points, &vPtMc[0], &vPtRes_t1[0],
                                        &vPtMcErr[0], &vPtResErr_t1[0]);
  _t2_pt_resol_pt_mc = new TGraphErrors(_n_points, &vPtMc[0], &vPtRes_t2[0],
                                        &vPtMcErr[0], &vPtResErr_t2[0]);
}

void SciFiDisplayMomentumResolutionsPR::make_pzpt_resolutions() {

  double pz_range = _resol_upper_bound - _resol_lower_bound;  // Range of the pz resolution graph
  double resolution_error =  pz_range / ( _n_points * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = _resol_lower_bound + resolution_error;
  std::vector<TCut> vCuts(_n_points);          // The cuts defining the pt_mc intervals
  std::vector<double> vPtMc(_n_points);        // The centre of the pt_mc intervals
  std::vector<double> vPtMcErr(_n_points);     // The width of the intervals
  std::vector<double> vPzRes_t1(_n_points);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t1(_n_points); // The errors associated with the pz res
  std::vector<double> vPzRes_t2(_n_points);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t2(_n_points); // The errors associated with the pz res

  // Cuts in pt_mc
  std::string s1 = "PtMc>=";
  std::string s2 = "&&PtMc<";
  for (int i = 0; i < _n_points; ++i) {
    std::stringstream ss1;
    double point_lower_bound = _resol_lower_bound + (resolution_error * 2 * i);
    double point_upper_bound = point_lower_bound + (resolution_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < _n_points; ++i) {
    vPtMc[i] = first_resolution_point + (resolution_error * 2 * i);
    vPtMcErr[i] = resolution_error;
    std::cerr << "vPtMc[" << i << "] = " << vPtMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pz TCut
  TCut tcut_pzrec = form_tcut("TMath::Abs(PzRec)", "<", _cut_pz_rec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PzMc+Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT1&&tcut_pzrec;
    calc_resolution(residual, input_cut, vPzRes_t1[i], vPzResErr_t1[i]);
  }
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PzMc-Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT2&&tcut_pzrec;
    calc_resolution(residual, input_cut, vPzRes_t2[i], vPzResErr_t2[i]);
  }

  // Create the resultant resolution plots
  _t1_pz_resol_pt_mc = new TGraphErrors(_n_points, &vPtMc[0], &vPzRes_t1[0],
                                        &vPtMcErr[0], &vPzResErr_t1[0]);
  _t2_pz_resol_pt_mc = new TGraphErrors(_n_points, &vPtMc[0], &vPzRes_t2[0],
                                        &vPtMcErr[0], &vPzResErr_t2[0]);
}

void SciFiDisplayMomentumResolutionsPR::make_ptpz_resolutions() {

  double _upper_bound_pzmc = 250.0;
  double _lower_bound_pzmc = 150.0;
  double x_range = _upper_bound_pzmc - _lower_bound_pzmc;  // Range of the pz resolution graph
  double x_error =  x_range / ( _n_points * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = _lower_bound_pzmc + x_error;
  std::vector<TCut> vCuts(_n_points);          // The cuts defining the pt_mc intervals
  std::vector<double> vPzMc(_n_points);        // The centre of the pt_mc intervals
  std::vector<double> vPzMcErr(_n_points);     // The width of the intervals
  std::vector<double> vPtRes_t1(_n_points);    // The resultant pz resolutions
  std::vector<double> vPtResErr_t1(_n_points); // The errors associated with the pz res
  std::vector<double> vPtRes_t2(_n_points);    // The resultant pz resolutions
  std::vector<double> vPtResErr_t2(_n_points); // The errors associated with the pz res

  // Cuts in pt_mc
  std::string s1 = "PzMc>=";
  std::string s2 = "&&PzMc<";
  for (int i = 0; i < _n_points; ++i) {
    std::stringstream ss1;
    double point_lower_bound = _lower_bound_pzmc + (x_error * 2 * i);
    double point_upper_bound = point_lower_bound + (x_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < _n_points; ++i) {
    vPzMc[i] = first_resolution_point + (x_error * 2 * i);
    vPzMcErr[i] = x_error;
    std::cerr << "vPzMc[" << i << "] = " << vPzMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pz TCut
  // TCut tcut_pzrec = form_tcut("TMath::Abs(PzRec)", "<", _cut_pz_rec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT1&&tcut_pzrec;
    TCut input_cut = vCuts[i]&&cutT1;
    calc_resolution(residual, input_cut, vPtRes_t1[i], vPtResErr_t1[i]);
  }
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PtMc-PtRec");
    // TCut input_cut = vCuts[i]&&cutT2&&tcut_pzrec;
    TCut input_cut = vCuts[i]&&cutT2;
    calc_resolution(residual, input_cut, vPtRes_t2[i], vPtResErr_t2[i]);
  }

  // Create the resultant resolution plots
  _t1_pt_resol_pz_mc = new TGraphErrors(_n_points, &vPzMc[0], &vPtRes_t1[0],
                                        &vPzMcErr[0], &vPtResErr_t1[0]);
  _t2_pt_resol_pz_mc = new TGraphErrors(_n_points, &vPzMc[0], &vPtRes_t2[0],
                                        &vPzMcErr[0], &vPtResErr_t2[0]);
}

void SciFiDisplayMomentumResolutionsPR::make_pzpz_resolutions() {

  double x_range = _upper_bound_pzmc - _lower_bound_pzmc;  // Range of the pz resolution graph
  double x_error =  x_range / ( _n_points * 2 );    // Error in pt_mc of each data point
  // The mid pt_mc value of the first data point
  double first_resolution_point = _lower_bound_pzmc + x_error;
  std::vector<TCut> vCuts(_n_points);          // The cuts defining the pt_mc intervals
  std::vector<double> vPzMc(_n_points);        // The centre of the pt_mc intervals
  std::vector<double> vPzMcErr(_n_points);     // The width of the intervals
  std::vector<double> vPzRes_t1(_n_points);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t1(_n_points); // The errors associated with the pz res
  std::vector<double> vPzRes_t2(_n_points);    // The resultant pz resolutions
  std::vector<double> vPzResErr_t2(_n_points); // The errors associated with the pz res

  // Cuts in pt_mc
  std::string s1 = "PzMc>=";
  std::string s2 = "&&PzMc<";
  for (int i = 0; i < _n_points; ++i) {
    std::stringstream ss1;
    double point_lower_bound = _lower_bound_pzmc + (x_error * 2 * i);
    double point_upper_bound = point_lower_bound + (x_error * 2);
    ss1 << s1 << point_lower_bound << s2 << point_upper_bound;
    vCuts[i] = ss1.str().c_str();
    std::cerr << "vCuts[" << i << "] = " << vCuts[i] << std::endl;
  }

  // The central MC momentum & error associated with the MC momentum (just the interval half width)
  for (int i = 0; i < _n_points; ++i) {
    vPzMc[i] = first_resolution_point + (x_error * 2 * i);
    vPzMcErr[i] = x_error;
    std::cerr << "vPzMc[" << i << "] = " << vPzMc[i] << std::endl;
  }

  // Cuts for to select each tracker
  TCut cutT1 = "TrackerNumber==0";
  TCut cutT2 = "TrackerNumber==1";

  // Form the reconstructed pz TCut
  TCut tcut_pzrec = form_tcut("TMath::Abs(PzRec)", "<", _cut_pz_rec);

  // Loop over the momentum intervals and calculate the resolution for each
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PzMc+Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT1&&tcut_pzrec;
    calc_resolution(residual, input_cut, vPzRes_t1[i], vPzResErr_t1[i]);
  }
  for (int i = 0; i < _n_points; ++i) {
    std::string residual("PzMc-Charge*PzRec");
    TCut input_cut = vCuts[i]&&cutT2&&tcut_pzrec;
    calc_resolution(residual, input_cut, vPzRes_t2[i], vPzResErr_t2[i]);
  }

  // Create the resultant resolution plots
  _t1_pz_resol_pz_mc = new TGraphErrors(_n_points, &vPzMc[0], &vPzRes_t1[0],
                                        &vPzMcErr[0], &vPzResErr_t1[0]);
  _t2_pz_resol_pz_mc = new TGraphErrors(_n_points, &vPzMc[0], &vPzRes_t2[0],
                                        &vPzMcErr[0], &vPzResErr_t2[0]);
}

void SciFiDisplayMomentumResolutionsPR::Plot(TCanvas* aCanvas) {
  mOf1->cd();
  // If canvas if passed in use it, otherwise initialise the member canvas
  TCanvas* lCanvas(NULL);
  if ( aCanvas ) {
    lCanvas = aCanvas;
  } else {
    if (mCanvas) {
      mCanvas->Clear();
    } else {
      mCanvas = new TCanvas("cResolutions", "Momentum Resolutions");
    }
    lCanvas = mCanvas;
  }
  // Prepare the TCanvas
  lCanvas->cd();
  lCanvas->Divide(4, 2);

  // Create the graphs
  make_ptpt_resolutions();
  make_pzpt_resolutions();
  make_ptpz_resolutions();
  make_pzpz_resolutions();

  // Draw the graphs
  if (_t1_pt_resol_pt_mc) {
    std::cout << "Drawing pt resolution as function of pt_mc in T1\n";
    lCanvas->cd(1);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t1_pt_resol_pt_mc->SetName("t1_ptpt_resol");
    _t1_pt_resol_pt_mc->SetTitle("T1 p_{t} Resolution");
    _t1_pt_resol_pt_mc->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    _t1_pt_resol_pt_mc->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    _t1_pt_resol_pt_mc->Draw("AP");
  }
  if (_t2_pt_resol_pt_mc) {
    std::cout << "Drawing pt resolution as function of pt_mc in T2\n";
    lCanvas->cd(5);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t2_pt_resol_pt_mc->SetName("t2_ptpt_resol");
    _t2_pt_resol_pt_mc->SetTitle("T2 p_{t} Resolution");
    _t2_pt_resol_pt_mc->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    _t2_pt_resol_pt_mc->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    _t2_pt_resol_pt_mc->Draw("AP");
  }
  if (_t1_pz_resol_pt_mc) {
    std::cout << "Drawing pz resolution as function of pt_mc in T1\n";
    lCanvas->cd(2);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t1_pz_resol_pt_mc->SetName("t1_pzpt_resol");
    _t1_pz_resol_pt_mc->SetTitle("T1 p_{z} Resolution");
    _t1_pz_resol_pt_mc->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    _t1_pz_resol_pt_mc->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    _t1_pz_resol_pt_mc->Draw("AP");
  }
  if (_t2_pz_resol_pt_mc) {
    std::cout << "Drawing pz resolution as function of pt_mc in T2\n";
    lCanvas->cd(6);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t2_pz_resol_pt_mc->SetName("t2_pzpt_resol");
    _t2_pz_resol_pt_mc->SetTitle("T2 p_{z} Resolution");
    _t2_pz_resol_pt_mc->GetXaxis()->SetTitle("p_{t}^{MC} (MeV/c)");
    _t2_pz_resol_pt_mc->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    _t2_pz_resol_pt_mc->Draw("AP");
  }
  if (_t1_pt_resol_pz_mc) {
    std::cout << "Drawing pt resolution as function of pz_mc in T1\n";
    lCanvas->cd(3);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t1_pt_resol_pz_mc->SetName("t1_ptpz_resol");
    _t1_pt_resol_pz_mc->SetTitle("T1 p_{t} Resolution");
    _t1_pt_resol_pz_mc->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    _t1_pt_resol_pz_mc->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    _t1_pt_resol_pz_mc->Draw("AP");
  }
  if (_t2_pt_resol_pz_mc) {
    std::cout << "Drawing pt resolution as function of pz_mc in T2\n";
    lCanvas->cd(7);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t2_pt_resol_pz_mc->SetName("t2_ptpz_resol");
    _t2_pt_resol_pz_mc->SetTitle("T2 p_{t} Resolution");
    _t2_pt_resol_pz_mc->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    _t2_pt_resol_pz_mc->GetYaxis()->SetTitle("p_{t} Resolution (MeV/c)");
    _t2_pt_resol_pz_mc->Draw("AP");
  }
  if (_t1_pz_resol_pz_mc) {
    std::cout << "Drawing pz resolution as function of pz_mc in T1\n";
    lCanvas->cd(4);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t1_pz_resol_pz_mc->SetName("t1_pzpz_resol");
    _t1_pz_resol_pz_mc->SetTitle("T1 p_{z} Resolution");
    _t1_pz_resol_pz_mc->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    _t1_pz_resol_pz_mc->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    _t1_pz_resol_pz_mc->Draw("AP");
  }
  if (_t2_pz_resol_pz_mc) {
    std::cout << "Drawing pz resolution as function of pz_mc in T2\n";
    lCanvas->cd(8);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    _t2_pz_resol_pz_mc->SetName("t2_pzpz_resol");
    _t2_pz_resol_pz_mc->SetTitle("T2 p_{z} Resolution");
    _t2_pz_resol_pz_mc->GetXaxis()->SetTitle("p_{z}^{MC} (MeV/c)");
    _t2_pz_resol_pz_mc->GetYaxis()->SetTitle("p_{z} Resolution (MeV/c)");
    _t2_pz_resol_pz_mc->Draw("AP");
  }
}

void SciFiDisplayMomentumResolutionsPR::Save() {
  if (mOf1) {
    mOf1->cd();
    if (mTree) mTree->Write();
    if (mCanvas) mCanvas->Write();
    if (_t1_pt_resol_pt_mc) _t1_pt_resol_pt_mc->Write();
    if (_t2_pt_resol_pt_mc) _t2_pt_resol_pt_mc->Write();
    if (_t1_pz_resol_pt_mc) _t1_pz_resol_pt_mc->Write();
    if (_t2_pz_resol_pt_mc) _t2_pz_resol_pt_mc->Write();
    if (_t1_pt_resol_pz_mc) _t1_pt_resol_pz_mc->Write();
    if (_t2_pt_resol_pz_mc) _t2_pt_resol_pz_mc->Write();
    if (_t1_pz_resol_pz_mc) _t1_pz_resol_pz_mc->Write();
    if (_t2_pz_resol_pz_mc) _t2_pz_resol_pz_mc->Write();
    mOf1->Close();
  }
}

SciFiDataBase* SciFiDisplayMomentumResolutionsPR::SetUp() {
  SciFiDataBase* data = SetUpSciFiData();
  SetUpRoot();
  return data;
}

SciFiDataBase* SciFiDisplayMomentumResolutionsPR::SetUpSciFiData() {
  mSpillData = new SciFiDataMomentumPR();
  return mSpillData;
}

bool SciFiDisplayMomentumResolutionsPR::SetUpRoot() {
  // Setup the output TFile
  mOf1 = new TFile("SciFiDisplayMomentumResolutionsPROutput.root", "recreate");
  // Set up the TTree
  mTree = new TTree("mTree", "SciFi Momentum Data");
  mTree->Branch("TrackerNumber", &mTrackData.TrackerNumber, "TrackerNumber/I");
  mTree->Branch("NumberOfPoints", &mTrackData.NumberOfPoints, "NumberOfPoints/I");
  mTree->Branch("Charge", &mTrackData.Charge, "Charge/I");
  mTree->Branch("PtMc", &mTrackData.PtMc, "PtMc/D");
  mTree->Branch("PzMc", &mTrackData.PzMc, "PtMz/D");
  mTree->Branch("PtRec", &mTrackData.PtRec, "PtRec/D");
  mTree->Branch("PzRec", &mTrackData.PzRec, "PtRec/D");
  // Set up the resolution graphs

  return true;
}
} // ~namespace MAUS
