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

/** @class PatternRecognition
 *
 * Pattern Recognition algorithms encapsulated in a class
 *
 */

#ifndef PATTERNRECOGNITION_HH
#define PATTERNRECOGNITION_HH

// C++ headers
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

// ROOT headers
#include "TFile.h"
#include "TH1D.h"
#include "TMatrixD.h"

// Third party library headers
#include "gtest/gtest_prod.h"

// MAUS headers
#include "src/common_cpp/Recon/SciFi/SciFiTools.hh"
#include "src/common_cpp/Recon/SciFi/SimpleLine.hh"
#include "src/common_cpp/Recon/SciFi/SimpleCircle.hh"
#include "src/common_cpp/Recon/SciFi/SimpleHelix.hh"
#include "src/common_cpp/DataStructure/SciFiEvent.hh"
#include "src/common_cpp/DataStructure/SciFiSpacePoint.hh"
#include "src/common_cpp/DataStructure/SciFiStraightPRTrack.hh"
#include "src/common_cpp/DataStructure/SciFiHelicalPRTrack.hh"

namespace MAUS {

/** @class PatternReconition
 *  @author A. Dobbs
 *  @brief Pattern Recognition associates tracker spacepoints to tracks
 *
 *  Pattern Recognition associates tracker spacepoints to tracks. Linear least squares fitting
 *  is used to determine which spacepoints come from the same parent track. Both straight and
 *  helical tracks may be searched for.
 */
class PatternRecognition {
  public:

    /** Macros to allow friendship with the gtests */
    FRIEND_TEST(PatternRecognitionTest, test_constructor);
    FRIEND_TEST(PatternRecognitionTest, test_set_parameters_to_default);
    FRIEND_TEST(PatternRecognitionTest, test_setup_debug);

    /** @brief Default constructor. Initialise variables, using globals if available,
               otherwise local defaults
     */
    PatternRecognition();

    /** @brief Default destructor */
    ~PatternRecognition();

    /** @brief Set the member variables using the Global singleton class */
    bool LoadGlobals();

    /** @brief Top level function to begin Pattern Recognition
      * @param evt The SciFi event
      */
    void process(SciFiEvent &evt) const;

     /** @brief Small function to add trks to a SciFiEvent & to set the tracker number for them
      *  @param[in] strks The straight tracks vector
      *  @param[in] htrks The helical tracks vector
      *  @param[out] evt The SciFi event
      */
    void add_tracks(std::vector<SciFiStraightPRTrack*> &strks,
                    std::vector<SciFiHelicalPRTrack*> &htrks, SciFiEvent &evt) const;

    /** @brief Function template which selects which tracks out of the candidates are used
     *
     * Function template which takes either SciFiStraightPRTrack* or SciFiHelicalPRTrack* and
     * selects which tracks to use out of all the different candidates based on the highest chisq
     *  @tparam <T> Either a SciFiStraightPRTrack* or SciFiHelicalPRTrack*
     *  @param[in] trks A vector containing either SciFiStraightPRTrack* or SciFiHelicalPRTrack*
     *  @return Returns the selected tracks
     */
    template<typename T>
    std::vector<T*> select_tracks(std::vector<T*> &trks) const;


    /** @brief Final processing for the tracks before adding to the SciFiEvent
     *
     *  Final processing for the tracks before adding to the SciFiEvent. Does the following:
     *    - sets the correct tracker number
     *    - seraches for any close spoints which helical fit missed
     *    - calculates the pulls in x-y projection of seed spacepoints from the helical track fit
     *  @param[in,out] strks Straight tracks to be processed
     *  @param[in,out] htrks Helical tracks to be processed
     */
    void track_processing(const int trker_no, const int n_points,
                          std::vector<SciFiSpacePoint*> &spnts,
                          std::vector<SciFiStraightPRTrack*> &strks,
                          std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Call the least squares fitter and perform the straight track fit to the spacepoints
     *  @param[in] n_points Number of spacepoints in the track
     *  @param[in] spnts A vector holding pointers to the spacepoints
     *  @return Returns the fitted track, NULL if the fit failed
     */
    SciFiStraightPRTrack* fit_straight_track(const int n_points,
                                             std::vector<SciFiSpacePoint*> spnts) const;

    /** @brief A function to call all the different make_tracks rountines.
     *
     *  A function to call all the different make_tracks rountines. Calls make_5tracks, 
     *  make_4tracks and make_3tracks for helical and straight tracks. Also calls select_tracks,
     *  track_processing and add_tracks.
     *  @param[in] track_type Helical or Straight. 0 = Straight, 1 = Helical
     *  @param[in] trker_no[in] The tracker number, 0 for TKU, 1 for TKD
     *  @param[in] spnts_by_station 2D vector of SciFiSpacePoint*, first index is (station number - 1)
     *  @param evt The SciFiEvent which will be populated with the tracks
     */
    void make_all_tracks(const bool track_type, const int trker_no,
                         SpacePoint2dPArray &spnts_by_station, SciFiEvent &evt) const;


    /** @brief Make Pattern Recognition track candidates with 5 spacepoints
     *
     *  Make Pattern Recognition track candidates with 5 spacepoints. All possible combinations of
     *  spacepoints are tried. Any which pass the fit are added as track candidates.
     *  The select_tracks function may then be used to decided which of these we actually want to
     *  pick and add to the SciFiEvent before sending to the final kalman track fit.
     * 
     *  @param[in] track_type Boolean, 0 for straight tracks, 1 for helical tracks
     *  @param[in] trker_no The tracker number, 0 for TKU, 1 for TKD
     *  @param[in] spnts_by_station A 2D vector of all the input spacepoints ordered by station
     *  @param[out] strks A vector of the output Pattern Recognition straight tracks
     *  @param[out] htrks A vector of the output Pattern Recognition helical tracks
     */
    void make_5tracks(const bool track_type, const int trker_no,
                      SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack*> &strks,
                      std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Make Pattern Recognition tracks with 4 spacepoints
     *
     *  Make Pattern Recognition track candidates with 4 spacepoints. All possible combinations of
     *  spacepoints are tried. Any which pass the fit are added as track candidates.
     *  The select_tracks function may then be used to decided which of these we actually want to
     *  pick and add to the SciFiEvent before sending to the final kalman track fit.
     *  @param[in] track_type Boolean, 0 for straight tracks, 1 for helical tracks
     *  @param[in] trker_no The tracker number, 0 for TKU, 1 for TKD
     *  @param[in] spnts_by_station A 2D vector of all the input spacepoints, first index is the
     *             station number - 1
     *  @param[out] strks A vector of the output Pattern Recognition straight tracks
     *  @param[out] htrks A vector of the output Pattern Recognition helical tracks
     */
    void make_4tracks(const bool track_type, const int trker_no,
                      SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack*> &strks,
                      std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Make Pattern Recognition tracks with 3 spacepoints (straight only)
     * 
     *  Make Pattern Recognition straight track candidates with 3 spacepoints. All possible
     *  combinations of spacepoints are tried. Any which pass the fit are added as track candidates.
     *  The select_tracks function may then be used to decided which of these we actually want to
     *  pick and add to the SciFiEvent before sending to the final kalman track fit. Note only
     *  straight tracks may contain 3 points, for helical the minimum allowed is 4 points.
     *  @param[in] trker_no The tracker number (0 or 1)
     *  @param[in] spnts_by_station A 2D vector of all the input spacepoints, first index is the
     *             station number - 1
     *  @param[out] strks A vector of the output Pattern Recognition straight tracks
     */
    void make_3tracks(const int trker_no, SpacePoint2dPArray &spnts_by_station,
                      std::vector<SciFiStraightPRTrack*> &strks) const;

    /** @brief Fits a straight track for a given set of stations
     * 
     *  @param[in] ignore_stations Int vector specifying which stations are not to be used for
     *             the track fit. 0 - 4 represent stations 1 - 5 respectively,
     *             while -1 means use *all* the stations (ignore none of them).
     *             The size of the vector should be 0 for a 5pt track,
     *             1 for a 4pt track, and 2 for a 3pt track.
     *  @param[in] spnts_by_station A 2D vector of all the input spacepoints ordered by station
     *  @param[out] trks - A vector of the output Pattern Recognition tracks
     */
    void make_straight_tracks(const int num_points, const int trker_no,
                              const std::vector<int> ignore_stations,
                              SpacePoint2dPArray &spnts_by_station,
                              std::vector<SciFiStraightPRTrack*> &strks) const;

    /** @brief Make a helical track from spacepoints
     *
     *  Recursive function holding the looping structure for making helical tracks from spacepoints.
     *  Once looping has identified candidate spacepoints, calls form_track which performs the 
     *  circle fit in x-y projection and then the line fit in the s-z projection.
     *
     *  @param[in] num_points The number of points in the track (4 or 5)
     *  @param[in,out] stat_num The current station number
     *  @param[in] ignore_stations Int vector specifying which stations are not to be used for
     *             the track fit. 0 - 4 represent stations 1 - 5 respectively,
     *             while -1 means use *all* the stations (ignore none of them).
     *             The size of the vector should be 0 for a 5pt track or 1 for a 4pt track
     *  @param[in,out] current_spnts The spacepoints assembled so far for the trial track
     *  @param[in] spnts_by_station A 2D vector of all the input spacepoints, first index is the
     *             station number - 1
     *  @param[out] htrks Vectors of tracks holding the initial helix parameters and spacepoints used
     *
     */
    void make_helix(const int n_points, const int stat_num, const std::vector<int> ignore_stations,
             std::vector<SciFiSpacePoint*> &current_spnts,
             SpacePoint2dPArray &spnts_by_station, std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Attempt to fit a helical track to given spacepoints
     * 
     *  Attempt to fit a helical track to given spacepoints. Two part process: (1) circle fit in the
     *  x-y projection, (2) a line fit in the s-z projection. Returns a pointer to the found
     *  track if successful, otherwise returns a NULL pointer.
     * 
     *  @param[in] n_points The number of points in the track (4 or 5)
     *  @param[in] spnts Vector holding pointers to the spacepoints
     *  @return Returns a pointer to the track created
     * 
     * */
    SciFiHelicalPRTrack* form_track(const int n_points, std::vector<SciFiSpacePoint*> spnts ) const;

    /** @brief Find the ds/dz of a helical track
     *
     * Find the ds/dz of a helical track. Output is the turning angles of the spacepoints
     * and a line of s vs z, the slope of which is dsdz = 1/tan(lambda).
     *
     * @param[in] n_points Number of points in the current track (used for the chi_sq cut)
     * @param[in] spnts A vector of all the input spacepoints
     * @param[in] circle The circle fit of spacepoints from x-y projection
     * @param[in] sigma_s The standard deviation on each value of s
     * @param[out] phi_i Vector containing the output turning angles of the spacepoints
     * @param[out] line_sz The output fitted line in s-z projection
     * @param[out] cov_sz Output covariance matrix from the s-z fit
     * @return Boolean indicating success or failure or the algorithm
     */
    bool find_dsdz(int n_points, std::vector<SciFiSpacePoint*> &spnts, const SimpleCircle &circle,
          const std::vector<double>& sigma_s, std::vector<double> &phi_i, SimpleLine &line_sz,
          TMatrixD& cov_sz, int &handedness) const;

    /** @brief Find the number of 2pi rotations that occured between each station
     *
     * Find the number of 2pi rotations that occured between each stations. This is
     * necessary in order to later evaluate s, the track path length in x-y, used to find ds/dz.
     *
     * @param[in] z The z coord of each spacepoint in the order seen by the beam
     * @param[in] phi The turning angle between successive spacepoints in the order seen by the beam
     * @param[out] true_phi The corrected turing angles
     * @return Boolean indicating success or failure or the algorithm
     */
    bool find_n_turns(const std::vector<double> &z, const std::vector<double> &phi,
                      std::vector<double> &true_phi, int &handedness) const;

    /** @brief Checks that the spacepoints in trial track fall within longest acceptable time range.
     *         Not used at present.
     *
     *  Tracker timing resolution cable of ~2ns. Longest acceptable time of flight through tracker
     *  was calculated offline for straight and helical tracks. Not used at present.
     *
     */
    bool check_time_consistency(const std::vector<SciFiSpacePoint*>, int tracker_id) const;

    /** @brief Determine which two stations the initial line should be drawn between
     * 
     *  The initial line is to be drawn between the two outermost stations being used.
     *  This in turn depends on which stations are presently being ignored
     *  e.g. for a 5 pt track, station 5 and station 1 are always  the outer and inner
     *  stations respectively.  This function returns the correct outer and inner
     *  station numbers, given which stations are presently being ignored.
     * 
     *  NB Stations are number 0 - 4 in the code, not 1 - 5 as in the outside world
     *
     *  Returns true if successful, false if fails (due to a bad argument being passed)
     *
     *  @param ignore_stations Vector of ints, holding which stations should be ignored
     *  @param o_stat_num The outermost station number used for a given track fit
     *  @param i_stat_num The innermost station number used for a given track fit
     *
     */
    bool set_end_stations(const std::vector<int> ignore_stations, int &o_stat_num,
                          int &i_stat_num) const;

    /** @brief Determine which three stations the initial circle should be fit to
     *
     *  The initial circle is to be fit between the two outermost stations being used, and a middle
     *  station needs to be picked as well (need three points for a circle fit).
     *  This in turn depends on which stations are presently being ignored
     *  e.g. for a 5 pt track, station 5 and station 1 are always  the outer and inner
     *  stations respectively, and station 3 is the middle station.
     *  This function returns the correct outer, inner, and middle
     *  station numbers, given which stations are presently being ignored.
     *
     *  NB Stations are number 0 - 4 in the code, not 1 - 5 as in the outside world
     *
     *  @param ignore_stations Vector of ints, holding which stations should be ignored
     *  @param o_stat_num The outermost station number used for a given track fit
     *  @param i_stat_num The innermost station number used for a given track fit
     *  @param mid_stat_num The middle station number used for a given track fit
     *
     */
    bool set_seed_stations(const std::vector<int> ignore_stations, int &o_stat_num,
                           int &i_stat_num, int &mid_stat_num) const;

    /** @brief Decide which tracker stations to ignore for this part of pat rec algorithm
     *  @param[in] ignore_stations Input vector holding the stations to be ignored
     *  @param[out] ignore_station_1 The first station to be ignored
     *  @param[out] ignore_station_2 The second station to be ignored
     */
    bool set_ignore_stations(const std::vector<int> &ignore_stations,
                             int &ignore_station_1, int &ignore_station_2) const;

    /** @brief Return the value for the Bz field in the upstream tracker, in tracker coords */
    double get_bz_t1() const { return _bz_t2; }

    /** @brief Set the value for the Bz field in the upstream tracker, in tracker coords */
    void set_bz_t1(double bz_t1) { _bz_t1 = bz_t1; }

    /** @brief Return the value for the Bz field in the downstream tracker, in tracker coords */
    double get_bz_t2() const { return _bz_t2; }

    /** @brief Set the value for the Bz field in the downstream tracker, in tracker coords */
    void set_bz_t2(double bz_t2) { _bz_t2 = bz_t2; }

    /** @brief Return the whether straight pat rec is on in TkUS */
    bool get_up_straight_pr_on() { return _up_straight_pr_on; }

    /** @brief Set whether or not to use straight pat rec in TkUS */
    void set_up_straight_pr_on(const bool up_straight_pr_on)
      { _up_straight_pr_on = up_straight_pr_on; }

    /** @brief Return the whether straight pat rec is on in TkDS */
    bool get_down_straight_pr_on() { return _down_straight_pr_on; }

    /** @brief Set whether or not to use straight pat rec in TkDS */
    void set_down_straight_pr_on(const bool down_straight_pr_on)
      { _down_straight_pr_on = down_straight_pr_on; }

    /** @brief Return the whether helical pat rec is on in TkUS */
    bool get_up_helical_pr_on() { return _up_helical_pr_on; }

    /** @brief Set whether or not to use helical pat rec in TkUS */
    void set_up_helical_pr_on(const bool up_helical_pr_on) { _up_helical_pr_on = up_helical_pr_on; }

    /** @brief Return the whether helical pat rec is on TkDS */
    bool get_down_helical_pr_on() { return _down_helical_pr_on; }

    /** @brief Set whether or not to use helical pat rec in TkDS */
    void set_down_helical_pr_on(const bool down_helical_pr_on)
      { _down_helical_pr_on = down_helical_pr_on; }

    /** @brief Return the boolean controlling if we search for missed helical seed spacepoints */
    bool get_sp_search_on() const { return _sp_search_on; }

    /** @brief Set the boolean controlling if we search for missed helical seed spacepoints */
    void set_sp_search_on(bool sp_search_on) { _sp_search_on = sp_search_on; }

    /** @brief Return the line fit method */
    int get_line_fitter() { return _line_fitter; }

    /** @brief Set line fit method */
    void set_line_fitter(int line_fitter) { _line_fitter = line_fitter; }

    /** @brief Get the circle fit method */
    int get_circle_fitter() { return _circle_fitter; }

    /** @brief Set circle fit method */
    void set_circle_fitter(int circle_fitter) { _circle_fitter = circle_fitter; }

    /** @brief Return the verbosity level */
    bool get_verbosity() { return _verb; }

    /** @brief Set the verbosity level */
    void set_verbosity(const bool verb) { _verb = verb; }

    /** @brief A function to set all the internal parameters to their default values (for tests) */
    void set_parameters_to_default();

    /** @brief Convenience function to set the tracker number on vectors of tracks 
     *  @param[in] trker_no The tracker number, 0 for TKU, 1 for TKD
     *  @param[out] strks A vector of straight tracks
     *  @param[out] htrks A vector of helical tracks
     */
    void set_tracker_number(const int trker_no, std::vector<SciFiStraightPRTrack*> &strks,
                            std::vector<SciFiHelicalPRTrack*> &htrks) const;

    /** @brief Place the different cut value currently being used into the variables supplied */
    void get_cuts(double& res_cut, double& straight_chisq_cut, double& R_res_cut,
       double& circle_chisq_cut, double& n_turns_cut, double& sz_chisq_cut);

    /** @brief Set the various cuts used in Pattern Recognition */
    void set_cuts(double res_cut, double straight_chisq_cut, double R_res_cut,
        double circle_chisq_cut, double n_turns_cut, double sz_chisq_cut);

    /** @brief Activate debug mode (set up the output ROOT file, histos, etc) */
    void setup_debug(std::string debug_fname = "pattern_recognition_debug.root");

    /** @brief Calculate the the standard deviation on the s coordinate for a given spacepoint */
    double sigma_on_s(const SimpleCircle& circ, const TMatrixD& cov_circ,
                      const SciFiSpacePoint* const spnt) const;

    /** @brief Look for any spacepoints which are not used, but are close to the track and from
               a station for which the track does not have a spacepoint */
    bool missing_sp_search_helical(std::vector<SciFiSpacePoint*>& spnts,
                                   SciFiHelicalPRTrack* trk) const;

  private:
    bool _debug;                /** Run in debug mode */
    bool _up_straight_pr_on;    /** Upstream straight pattern recogntion on or off */
    bool _down_straight_pr_on;  /** Downstream straight pattern recogntion on or off */
    bool _up_helical_pr_on;     /** Upstream Helical pattern recogntion on or off */
    bool _down_helical_pr_on;   /** Downstream Helical pattern recogntion on or off */
    bool _sp_search_on;         /** Do we seach for seed spoints missed by helical fit? */
    int _s_error_method;        /** How to calc error on s, 0 = station res, 1 = error prop */
    int _line_fitter;           /** Line fitter, 0 = custom lsq, 1 = ROOT */
    int _circle_fitter;         /** Circle fitter, 0 = custom lsq, 1 = MINUIT */
    int _verb;                  /** Verbosity: 0=little, 1=more couts */
    int _n_trackers;            /** Number of trackers */
    int _n_stations;            /** Number of stations per tracker */
    double _bz_t1;              /** Bz field in upstream tracker */
    double _bz_t2;              /** Bz field in downstream tracker */
    double _sd_1to4;            /** Position error associated with stations 1 t0 4 */
    double _sd_5;               /** Position error associated with station 5 */
    double _sd_phi_1to4;        /** Rotation error associated with stations 1 t0 4 */
    double _sd_phi_5;           /** Rotation error associated with station 5 */
    double _res_cut;            /** Road cut for linear fit in mm */
    double _straight_chisq_cut; /** Cut on the chi^2 of the least sqs fit in mm */
    double _R_res_cut;          /** Cut on the radius of the track helix in mm */
    double _circle_chisq_cut;   /** Cut on the chi^2 of the circle least sqs fit in mm */
    double _n_turns_cut;        /** Cut to decide if a given n turns value is good */
    double _sz_chisq_cut;       /** Cut on the sz chi^2 from least sqs fit in mm */
    double _circle_error_w;     /** Weight to artificially scale the error going to xy fit */
    double _sz_error_w;         /** Weight to artificially scale the error going to sz fit */
    double _Pt_max;             /** MeV/c max Pt for h tracks (given by R_max = 150mm) */
    double _Pz_min;             /** MeV/c min Pz for helical tracks (this is a guess) */
    double _missing_sp_cut;     /** Dist (mm) below which a missing spoint should be added to trk*/

    TFile* _rfile;        /** A ROOT file pointer for dumping residuals to in debug mode */
    TH1D* _hx_line;       /** histo of x residuals taken during straight road cut stage */
    TH1D* _hy_line;       /** histo of y residuals taken during straight road cut stage */
    TH1D* _hxchisq_line;  /** histo of chisq of every x-z straight least sq fit tried */
    TH1D* _hychisq_line;  /** histo of chisq of every y-z straight least sq fit tried */
    TH1D* _hxychisq_line; /** histo of chisq of every x-y circle least sq fit tried */
    TH1D* _hszchisq_line; /** histo of chisq of every s-z least sq fit tried */

    TH1D* _fail_helix_tku;  /** Where did helix 5pt fit fail? 1-circle fit, 2-n_turns, 3-sz fit */
    TH1D* _fail_helix_tkd;  /** Where did helix 5pt fit fail? 1-circle fit, 2-n_turns, 3-sz fit */
};


// Predicate function to sort tracks by their combined fit chisq
template<typename T>
bool compare_tracks_ascending_chisq(const T *trk1, const T *trk2) {
  return (trk1->get_chi_squared() < trk2->get_chi_squared());
}

template<typename T>
std::vector<T*> PatternRecognition::select_tracks(std::vector<T*> &trks) const {

  // Sort the tracks by combined chisq (cannot do in place due need to preserve constness)
  std::vector<T*> sorted_tracks = trks;
  std::sort(sorted_tracks.begin(), sorted_tracks.end(), compare_tracks_ascending_chisq<T>);

  // Now loop over tracks and pull out highest chisq distinct tracks
  std::vector<T*> accepted_tracks;
  for (auto trk : sorted_tracks) {
    std::vector<SciFiSpacePoint*> spoints = trk->get_spacepoints_pointers();
    int n_used = 0;
    for (auto sp : spoints) {
      if (sp->get_used()) ++n_used;
    }
    // Accept the track if it has enough unused spacepoints
    if (n_used == 0) {
      // Set the spacepoints to used (they are pointers, so applies to all tracks which hold them)
      for (auto sp : spoints) {
        sp->set_used(true);
      }
      accepted_tracks.push_back(trk);
    } else {
      delete trk; // This track is not for use, so free the memory
      trk = NULL;
    }
  }
  return accepted_tracks;
}

} // ~namespace MAUS

#endif
