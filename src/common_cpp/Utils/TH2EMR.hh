/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
 */

#ifndef _SRC_COMMON_CPP_UTILS_TH2EMR_HH_
#define _SRC_COMMON_CPP_UTILS_TH2EMR_HH_

#include <iostream>
#include <vector>
#include <algorithm>

#include "TStyle.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH2Poly.h"

namespace MAUS {

/** @class 	TH2Poly class that knows the geometrical arrangement of the EMR,
 *         	usefull to plot proper EMR event displays.
 *
 *  @var h	<-- TH2Poly histogram fitting the triangular bar geometry of the EMR -->
 *  @var v	<-- vector of values in the histogram to set the axes correctly -->
 */

class TH2EMR {
  public:

    /** @brief Default constructor - initialises to 0/NULL */
    TH2EMR();

    /** @brief Copy constructor - any pointers are deep copied */
    TH2EMR(const TH2EMR& _th2emr);

    /** @brief Equality operator - any pointers are deep copied */
    TH2EMR& operator=(const TH2EMR& _th2emr);

    /** @brief Constructor with name and title - Define the TH2Poly with the EMR structure */
    TH2EMR(const char* name, const char* title);

    /** @brief Destructor - any member pointers are deleted */
    virtual ~TH2EMR();

    /** @brief Fills the bin corresponding to bar j of plane i an additional hit */
    void Fill(int i, int j);

    /** @brief Fills the bin corresponding to bar j of plane i with tot w */
    void Fill(int i, int j, double w);

    /** @brief Fills the i plane charge with charge w */
    void FillPlane(int i, double w);

    /** @brief Draws histograms in COLZ by default */
    void Draw();

    /** @brief Returns the bin content corresponding to bar j of plane i an additional hit */
    double GetBinContent(int i, int j);

    /** @brief Sets the bin content corresponding to bar j of plane i an additional hit */
    void SetBinContent(int i, int j, double w);

    /** @brief Returns the content of the maximum bin */
    double GetMaximum()     { return _h->GetMaximum(); }

    /** @brief Returns the content of the maximum bin */
    double GetMinimum()     { return _h->GetMinimum(); }

    /** @brief Returns the number of entries in the histogram */
    size_t GetEntries()	    { return _v.size(); }

    /** @brief Returns a pointer to the TH2Poly created by this class */
    TH2Poly* GetHistogram() { return _h; }

    /** @brief Save the histogram to the root file **/
    void Write()            { _h->Write(); }

  private:

    TH2Poly*		_h;
    std::vector<int>	_v;
};
} // namespace MAUS

#endif // #define _SRC_COMMON_CPP_UTILS_TH2EMR_HH_
