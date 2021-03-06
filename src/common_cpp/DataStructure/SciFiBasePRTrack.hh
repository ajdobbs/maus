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



#ifndef  _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIBASEPRTRACK_HH_
#define _SRC_COMMON_CPP_DATASTRUCTURE_SCIFIBASEPRTRACK_HH_

// C++ headers
#include <vector>

// ROOT headers
#include "TRef.h"
#include "TRefArray.h"
#include "TObject.h"

// MAUS headers
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/Utils/VersionNumber.hh"

namespace MAUS {

typedef std::vector<double> DoubleArray;

/** @class SciFiBasePRTrack
 *  @author A. Dobbs
 *  @author C. Hunt
 *  @brief Base class for scifi pattern recognition tracks
 */
class SciFiBasePRTrack : public TObject {
  public:
    /** @brief Default constructor */
    SciFiBasePRTrack();

    /** @brief Construct some variables */
    explicit SciFiBasePRTrack(DoubleArray cov);

    /** @brief Construct some variables */
    SciFiBasePRTrack(int charge, DoubleArray cov, SciFiSpacePointPArray spoints);

    /** @brief Copy constructor */
    SciFiBasePRTrack(const SciFiBasePRTrack& track);

    /** @brief Default destructor */
    virtual ~SciFiBasePRTrack();

    /** @brief Assignment operator */
    SciFiBasePRTrack& operator=(const SciFiBasePRTrack& track);

    /** @brief Combine the different chi sqs into one final value */
    virtual void form_total_chi_squared() {}

    /** Get the charge */
    int get_charge() const { return _charge; }

    /** Set the charge */
    void set_charge(int charge) { _charge = charge; }

    /** Get the covariance matrix elements of the least square fit parameters */
    std::vector<double> get_covariance() const { return _covariance; }

    /** Set the covariance matrix elements of the least square fit parameters */
    void set_covariance(const std::vector<double>& covariance) { _covariance = covariance; }

    /** Get the vector holding pointers to the spacepoints used by the track */
    TRefArray* get_spacepoints() const { return _spoints; }

    /** Set the vector holding pointers to the spacepoints
     * used by the track from a TRefArray */
    void set_spacepoints(TRefArray* spoints);

   /** Return a vector holding pointers to the spacepoints
     * (requires extracting them from the TRefArray holder 
     */
    SciFiSpacePointPArray get_spacepoints_pointers() const;

    /** @brief Add a spacepoint by pointer to the track, track does not take ownership of memory */
    void add_spacepoint_pointer(SciFiSpacePoint* sp);

    /** Set the vector holding pointers to the spacepoints
     *  used by the track from a vector  of spacepoint pointers */
    void set_spacepoints_pointers(const SciFiSpacePointPArray &spoints);


    /** Set the reconstructed reference frame position */
    void set_reference_position(ThreeVector pos) { _position = pos; }

    /** Get the reconstructed reference frame position */
    ThreeVector get_reference_position() const { return _position; }

    /** Set the reconstructed reference frame momentum */
    void set_reference_momentum(ThreeVector mom) { _momentum = mom; }

    /** Get the reconstructed reference frame momentum */
    ThreeVector get_reference_momentum() const { return _momentum; }


    /** Set the reconstructed seed frame position */
    void set_seed_position(ThreeVector pos) { _seed_position = pos; }

    /** Get the reconstructed seed frame position */
    ThreeVector get_seed_position() const { return _seed_position; }

    /** Set the reconstructed seed frame momentum */
    void set_seed_momentum(ThreeVector mom) { _seed_momentum = mom; }

    /** Get the reconstructed seed frame momentum */
    ThreeVector get_seed_momentum() const { return _seed_momentum; }

    /** Get the global chi-squared value */
    double get_chi_squared() const { return _chi_sq; }

    /** Set the global chi-squared value */
    void set_chi_squared(double chi_sq) { _chi_sq = chi_sq; }

    /** Get the Number of Degrees of Freedom */
    int get_ndf() const { return _ndf; }

    /** Set the Number of Degrees of Freedom */
    void set_ndf(int ndf) { _ndf = ndf; }

    /** Get the number of spacepoints used in the fit */
    int get_n_fit_points() const { return _n_fit_points; }

    /** Set the number of spacepoints which was used in the fit */
    void set_n_fit_points(int n_fit_points) { _n_fit_points = n_fit_points; }

    /** Get the number of points in a track (method recommended by ROOT */
    int get_num_points() const { return (_spoints->GetLast() + 1); }

    /** Get the number of triplet spacepoints */
    int get_num_triplets() const;

  private:
    TRefArray* _spoints; /** The spacepoints associated with the track (track does not own mem) */

    /**
     *  Vector of covariances from the least squared fits.
     *  In the helical track case these are the covariances of:
     *  alpha-alpha, alpha-beta, alpha-gamma, beta-alpha, beta-beta, beta-gamma,
     *  gamma-alpha, gamma-beta, gamma-gamma, c_sz-c_sz, c_sz-m_sz, m_sz-c_sz, m_sz-m_sz.
     *  In the straight track case these are the covariances of:
     *  c_x-c_x, c_x-m_x, m_x-c_x, m_x-m_x, c_y-c_y, c_y-m_y, m_y-c_y, m_y-m_y.
     */
    DoubleArray _covariance;
    ThreeVector _position;   /** Reconstructed reference plane position */
    ThreeVector _momentum;   /** Reconstructed reference plane momentum */
    ThreeVector _seed_position;   /** Reconstructed seed plane position */
    ThreeVector _seed_momentum;   /** Reconstructed seed plane momentum */
    double _chi_sq;          /** The chisq of the fit/s which formed the track */
    int _ndf;                /** The number of degrees of freedom */
    int _charge;             /** The track charge, 0 for straights */
    int _n_fit_points;       /** The number of spacepoints used in the fit */

  MAUS_VERSIONED_CLASS_DEF(SciFiBasePRTrack)
};

typedef std::vector<SciFiBasePRTrack*> SciFiBasePRTrackPArray;
typedef std::vector<SciFiBasePRTrack> SciFiBasePRTrackArray;

} // ~namespace MAUS

#endif

