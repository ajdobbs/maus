/* This file is part of MAUS: http://  micewww.pp.rl.ac.uk:8080/projects/maus
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
 * along with MAUS.  If not, see <http://  www.gnu.org/licenses/>.
 */

#include "Recon/Global/PIDBase1D.hh"
#include "Utils/Exception.hh"

namespace MAUS {
namespace recon {
namespace global {

  PIDBase1D::PIDBase1D(std::string variable, std::string hypothesis,
		   std::string unique_identifier, int XminBin, int XmaxBin,
		   int XnumBins) : PIDBase(variable, hypothesis,
       unique_identifier, XminBin, XmaxBin,
       XnumBins, YminBin, YmaxBin, YnumBins) {

        _varhyp = variable + "_" + hypothesis;

        _hist = new TH1F(_varhyp.c_str(), _varhyp.c_str(),
		     XnumBins, XminBin, XmaxBin);
  };

  PIDBase1D::PIDBase1D(TFile* file, std::string variable,
		   std::string hypothesis)
    : PIDBase(file, variable, hypothesis) {
    std::string histname = variable + "_" + hypothesis;
    if (!file || file->IsZombie()) {
      throw(Exception(Exception::recoverable,
		      "File containing MC PID histograms not found.",
		      "Recon::Global::PIDBase1D::PIDBase1D()"));
    }
    _hist = static_cast<TH1F*>(file->Get(histname.c_str()));
    if (!_hist) {
      throw(Exception(Exception::recoverable,
		      "Histogram not found in file.",
		      "Recon::Global::PIDBase1D::PIDBase1D()"));
    }
    _XminBin = _hist->GetXaxis()->GetXmin();
    _XmaxBin = _hist->GetXaxis()->GetXmax();
  };

  PIDBase1D::~PIDBase1D() {
    if (_writeFile) {
      if (!_hist) {
	throw(Exception(Exception::recoverable,
			"Can't write histogram to file.",
			"Recon::Global::PIDBase1D::~PIDBase1D()"));
      }
      if (_nonZeroHistEntries) {
	int bins = _hist->GetSize();
	_addToAllBins = 1/(static_cast<double>(bins));
	for (int i = 0; i < bins; i++) {
	  int entries = _hist->GetBinContent(i);
	  _hist->SetBinContent(i, (static_cast<double>(entries)
				   + _addToAllBins));
	}
      }

      Double_t scale = 1/_hist->Integral();
      _hist->Scale(scale);
      _writeFile->cd();
      _hist->Write();
      _writeFile->Close();
      }
  };

    TH1F* PIDBase1D::Get_hist() {
    return PIDBase1D::_hist;
    }

  double PIDBase1D::logL(MAUS::DataStructure::Global::Track* track) {
    double var = (Calc_Var(track)).first;
    if (var < _XminBin || var > _XmaxBin) {
      Squeak::mout(Squeak::error) << "Missing/invalid measurements for  " <<
	"TOF0/TOF1 times, Recon::Global::PIDBase1D::logL()" << std::endl;
      return 1;
    }
    int bin = _hist->FindBin(var);
    double entries = _hist->GetBinContent(bin);
    if (entries <=0) {
      Squeak::mout(Squeak::error) << "Corresponding bin content in PDF is " <<
	"not greater than zero, Recon::Global::PIDBase1D::logL()" << std::endl;
      return 1;
    }
    double probLL = TMath::Log(entries);
    return probLL;
  }

  void PIDBase1D::Fill_Hist(MAUS::DataStructure::Global::Track* track) {
    double var = (Calc_Var(track)).first;
    if (var < _XminBin || var > _XmaxBin) {
       Squeak::mout(Squeak::error) << "Calc_Var returned invalid value of "
				   << "PID variable, not added to histogram, "
				   << "Recon::Global::PIDBase1D::Fill_Hist()"
				   << std::endl;
    } else {
      _hist->Fill(var);
    }
  }
}
}
}