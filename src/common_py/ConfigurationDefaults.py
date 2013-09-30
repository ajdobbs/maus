"""
The Configuration defaults

These are the default values for MAUS and its components. Please DO NOT CHANGE
this unless you want to change this quantity for everybody. Values can be
overridden by setting configuration_file parameter on the comamnd line, for
example

bin/simulate_mice.py --configuration_file my_configuration.py
"""


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

# NOTE: please use lower_case_and_underscores for new configuration cards

import os

type_of_dataflow = 'pipeline_single_thread'

input_json_file_name = "maus_input.json"
input_json_file_type = "text"
output_json_file_name = "maus_output.json"
output_json_file_type = "text"

input_root_file_name = "maus_input.root"
output_root_file_name = "maus_output.root"

# Used, for now, to determine what level of
# c++ log messages are reported to the user:
# 0 = debug info (and std::cout)
# 1 = run info
# 2 = warnings
# 3 = errors (and std::cerr)
# 4 = fatal
# >4 = silent
# Doesnt effect python
verbose_level = 1
errors_to_stderr = None # None = from verbose_level; else True or False
errors_to_json = True
on_error = 'none' # none, halt or raise
will_do_stack_trace = verbose_level < 1 # set to True to make stack trace on C++
                                        # exception

# set how headers and footers are handled - "append" will set to
# append headers and footers to the output; dont_append will set to not append
# headers and footers to the output. Affects JobHeader, JobFooter, RunHeader and
# RunFooter
#
# For example, if the user wants to copy data from one format to another, he
# should set to dont_append to avoid header and footer information being taken
# to the output
header_and_footer_mode = "append" #append or dont_append

# Used by MapPyRemoveTracks.
keep_only_muon_tracks = False

# Used by MapCppSimulation
keep_tracks = False # set to true to keep start and end point of every track
keep_steps = False # set to true to keep start and end point of every track and
                   # every step point
simulation_geometry_filename = "Test.dat" # geometry used by simulation - default is a liquid Hydrogen box
check_volume_overlaps = False
maximum_number_of_steps = 500000 # particles are killed after this number of
                                 # steps (assumed to be stuck in the fields)
simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":-0.0, "z":-6400.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":-13, "energy":226.0, "time":0.0, "random_seed":10
}
everything_special_virtual = False

# geant4 physics model
physics_model = "QGSP_BERT" # Physics package loaded by MAUS to set default values; modifications can be made
reference_physics_processes = "mean_energy_loss" # controls the physics processes of the reference particle. Set to "none" to disable all physics processes; or "mean_energy_loss" to make the reference particle see deterministic energy loss only
physics_processes = "standard" # controls the physics processes of normal particles. Set to "none" to disable all physics processes; "mean_energy_loss" to enable deterministic energy loss only; or "standard" to enable all physics processes
particle_decay = True # set to true to activate particle decay, or False to inactivate particle decay
charged_pion_half_life = -1. # set the pi+, pi- half life [ns]. Negative value means use geant4 default
muon_half_life = -1. # set the mu+, mu- half life [ns]. Negative value means use geant4 default
production_threshold = 0.5 # set the threshold for delta ray production [mm]
kinetic_cutoff=1.0 # set minimum kinetic energy of a track at birth [MeV/c]
default_keep_or_kill = True
# map of string pdg pids; always keep particles on creation if their pdg maps to True; always kill particles on creation if their pdg maps to False. Default comes from default_keep_or_kill
keep_or_kill_particles = {"mu+":True, "mu-":True,   
                          "nu_e":False, "anti_nu_e":False,
                          "nu_mu":False, "anti_nu_mu":False,
                          "nu_tau":False, "anti_nu_tau":False,
}
max_step_length = 100. # default maximum step size during tracking (override with G4StepMax in MiceModule)
max_track_time = 1.e9 # kill tracks with time above max_time (override with G4TimeMax in MiceModule)
max_track_length = 1.e8 # kill tracks with track length above max_track_length (override with G4TrackMax in MiceModule)
kinetic_energy_threshold = 0.1 # kill tracks with initial kinetic energy below energy_threshold (override with G4KinMin in MiceModule)
field_tracker_absolute_error = 1.e-4 # set absolute error on MAUS internal stepping routines - used by e.g. VirtualPlanes to control accuracy of interpolation
field_tracker_relative_error = 1.e-4 # set relative error on MAUS internal stepping routines - used by e.g. VirtualPlanes to control accuracy of interpolation

stepping_algorithm = "ClassicalRK4" # numerical integration routine
delta_one_step = -1. # Geant4 step accuracy parameter
delta_intersection = -1.
epsilon_min = -1.
epsilon_max = -1.
miss_distance = -1.
maximum_module_depth = 10 # maximum depth for MiceModules as used by the simulation

# geant4 visualisation (not event display)
geant4_visualisation = False
visualisation_viewer = "VRML2FILE"  # only supported option
visualisation_theta = 90.
visualisation_phi = 90.
visualisation_zoom = 1.
accumulate_tracks = 0 # this accumulates the tracks into one vrml file 1 = yes, 0 = no
#particle colour alterations
default_vis_colour = {"red":0. , "green":100.  ,"blue":0.} #green
pi_plus_vis_colour = {"red":255. , "green":250.  ,"blue":240.} # white
pi_minus_vis_colour = {"red":105. , "green":105.  ,"blue":105.} # grey
mu_plus_vis_colour = {"red":25. , "green":25.  ,"blue":112.} # dark blue
mu_minus_vis_colour = {"red":135. , "green":206.  ,"blue":250.} # light blue
e_plus_vis_colour = {"red":250. , "green":0.  ,"blue":0.} # red
e_minus_vis_colour = {"red":250. , "green":69.  ,"blue":0.} # orange red
gamma_vis_colour = {"red":255. , "green":20.  ,"blue":147.} # pink
neutron_vis_colour = {"red":139. , "green":69.  ,"blue":19.} # brown
photon_vis_colour = {"red":255. , "green":255.  ,"blue":0.} # yellow

# used by InputPySpillGenerator to determine the number of empty spills that
# will be generated by the simulation
spill_generator_number_of_spills = 100
# used by MapPyBeamMaker to generate input particle data
# This is a sample beam distribution based on guesses by Chris Rogers of what
# an optimised beam might look like
beam = {
    "particle_generator":"binomial", # routine for generating empty primaries
    "binomial_n":50, # number of coin tosses
    "binomial_p":0.5, # probability of making a particle on each toss
    "random_seed":5, # random seed for beam generation; controls also how the MC
                     # seeds are generated
    "definitions":[
    ##### MUONS #######
    {
       "reference":simulation_reference_particle, # reference particle
       "random_seed_algorithm":"incrementing_random", # algorithm for seeding MC
       "weight":90., # probability of generating a particle
       "transverse":{
          "transverse_mode":"constant_solenoid", # transverse distribution matched to constant solenoid field
          "emittance_4d":6., # 4d emittance
          "normalised_angular_momentum":0.1, # angular momentum from diffuser
          "bz":4.e-3 # magnetic field strength for angular momentum calculation
       },
       "longitudinal":{"longitudinal_mode":"sawtooth_time", # longitudinal distribution sawtooth in time
                   "momentum_variable":"p", # Gaussian in total momentum (options energy, pz)
                   "sigma_p":25., # RMS total momentum
                   "t_start":-1.e6, # start time of sawtooth
                   "t_end":+1.e6}, # end time of sawtooth
       "coupling":{"coupling_mode":"none"} # no dispersion
    },
    ##### PIONS #####
    { # as above...
       "reference":{
           "position":{"x":0.0, "y":-0.0, "z":-6400.0},
           "momentum":{"x":0.0, "y":0.0, "z":1.0},
           "particle_id":211, "energy":285.0, "time":0.0, "random_seed":10
       },
       "random_seed_algorithm":"incrementing_random",
       "weight":2.,
       "transverse":{"transverse_mode":"constant_solenoid", "emittance_4d":6.,
            "normalised_angular_momentum":0.1, "bz":4.e-3},
       "longitudinal":{"longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"p",
                   "sigma_p":25.,
                   "t_start":-1.e6,
                   "t_end":+1.e6},
       "coupling":{"coupling_mode":"none"}
    },
    ##### ELECTRONS #####
    { # as above...
        "reference":{
            "position":{"x":0.0, "y":-0.0, "z":-6400.0},
            "momentum":{"x":0.0, "y":0.0, "z":1.0},
            "particle_id":-11, "energy":200.0, "time":0.0, "random_seed":10
        },
        "random_seed_algorithm":"incrementing_random",
        "weight":8.,
       "transverse":{"transverse_mode":"constant_solenoid", "emittance_4d":6.,
            "normalised_angular_momentum":0.1, "bz":4.e-3},
       "longitudinal":{"longitudinal_mode":"sawtooth_time",
                   "momentum_variable":"p",
                   "sigma_p":25.,
                   "t_start":-1.e6,
                   "t_end":+1.e6},
        "coupling":{"coupling_mode":"none"}
    }]
}

# this is used by reconstruction; if set to an empty string, automatically
# acquires simulation_geometry_filename
reconstruction_geometry_filename = ""

# scifi tracker digitization
#SciFiDeadChanFName = ""
SciFiMUXNum = 7
SciFiFiberDecayConst = 2.7
SciFiFiberConvFactor =  3047.1
SciFiFiberTrappingEff = 0.056
SciFiFiberMirrorEff = 0.6
SciFiFiberTransmissionEff = 0.8
SciFiMUXTransmissionEff = 1.0
SciFivlpcQE = 0.8
SciFivlpcEnergyRes = 4.0 # MeV
SciFivlpcTimeRes = 0.2 # ns
SciFiadcFactor = 6.0
SciFitdcBits = 16
SciFitdcFactor = 1.0
SciFinPlanes = 3
SciFinStations = 5
SciFinTrackers = 2
SciFiNPECut = 2.0 # photoelectrons
SciFiClustExcept = 100 # exception is thrown
SciFi_sigma_tracker0_station5 = 0.4298 # mm
SciFi_sigma_triplet = 0.3844 # mm
SciFi_sigma_z = 0.081 # mm
SciFi_sigma_duplet =  0.6197 # mm
SciFiPRHelicalOn = True # Flag to turn on the tracker helical pattern recognition
SciFiPRStraightOn = True # Flag to turn on the tracker straight pattern recognition
SciFiSD1To4 = 0.3844 # Position error associated with stations 1 t0 4 (mm)
SciFiSD5 = 0.4298 # Position error associated with station 5 (mm)
SciFiRadiusResCut = 150.0 # Helix radius cut (mm)
SciFiPerChanFlag = 0
SciFiNoiseFlag = 0
SciFiDigitNPECut = 1.5 # photoelectrons
SciFiCrossTalkSigma = 50.0
SciFiCrossTalkAmplitude = 1.5
SciFiDarkCountProababilty = 0.017 #probability of dark count due to thermal electron
SciFiChannelCalibList = "%s/files/calibration/SciFiChanCal.txt" % os.environ.get("MAUS_ROOT_DIR")
SciFiParams_Z = 5.61291
SciFiParams_Plane_Width = 0.6523
SciFiParams_Radiation_Legth = 424.0
SciFiParams_Density = 1.06
SciFiParams_Mean_Excitation_Energy = 68.7
SciFiParams_A = 104.15
SciFiParams_Pitch = 1.4945
SciFiParams_Station_Radius = 160.
SciFiParams_RMS = 370.
SciFiSeedCovariance = 1000 # Error estimate for Seed values of the Kalman Fit
SciFiKalmanOn = True # Flag to turn on the tracker Kalman Fit
SciFiKalman_use_MCS = True # flag to add MCS to the Kalman Fit
SciFiKalman_use_Eloss = True # flag to add Eloss to the Kalman Fit
SciFiUpdateMisalignments = False # Do Misalignment Search & Update
SciFiKalmanVerbose  = False # Dump information per fitted track

# configuration database
cdb_upload_url = "http://cdb.mice.rl.ac.uk/cdb/" # target URL for configuration database uploads TestServer::http://rgma19.pp.rl.ac.uk:8080/cdb/
cdb_download_url = "http://cdb.mice.rl.ac.uk/cdb/" # target URL for configuration database downloads

# geometry download
geometry_download_wsdl = "geometry?wsdl" # name of the web service used for downloads
geometry_download_directory   = "%s/files/geometry/download" % os.environ.get("MAUS_ROOT_DIR") # name of the local directory where downloads will be placed
geometry_download_by = 'id' # choose 'run_number' to download by run number, 'current' to use
                                    # the currently valid geometry or 'id' to use the cdb internal id 
                                    # (e.g. if it is desired to access an old version of a particular
                                    # geometry)
geometry_download_run_number = 0
geometry_download_id = 3
geometry_download_cleanup = True # set to True to clean up after download
g4_step_max = 5.0 # this is the value which shall be placed in the Mice Modules which have been translated from CAD

# geometry upload
geometry_upload_wsdl = "geometrySuperMouse?wsdl" # name of the web service used for uploads
geometry_upload_directory = "%s/files/geometry/upload" % os.environ.get("MAUS_ROOT_DIR") # name of the local directory where uploads are drawn from
geometry_upload_note = "" # note, pushed to the server to describe the geometry. A note must be specified here (default will throw an exception).
geometry_upload_valid_from = "" # date-time in format like: that the specified installation was made in the experiment. A date-time must be specified here (default will throw an exception).
technical_drawing_name = "" #name and version of the technical drawing from which the CAD model came from.
geometry_upload_cleanup = True # set to True to clean up after upload
technical_drawing_name = "" #name and version of the technical drawing from which the CAD model came from.

#dates need to get geomtry ids
get_ids_start_time = "2012-05-08 09:00:00"
get_ids_stop_time = "2012-05-22 15:47:34.856000"
get_ids_create_file = True

#get beamline information
# This executable will give the run numbers of the runs which the CDB has information on.
# The information is the magnet currents, reasons for run and other information which
# is specific to that run. When downloading a geometry by run number the beamline 
# information is merged with the geometrical infomation. Options for querying
# beamline information are; 'all_entries' returns a list of all run numbers with beamline info.
#                           'run_number'  prints whether info is held for this run number or not.
#                           'dates'       returns a list of run numbers with info during specified time period.
get_beamline_by = "all_entries" 
get_beamline_run_number = ""
get_beamline_start_time = ""
get_beamline_stop_time = ""

# this is used by ImputCppRealData
Number_of_DAQ_Events = -1
Phys_Events_Only = False
Calib_Events_Only = False
Enable_V1290_Unpacking = True
Enable_V1731_Unpacking = True
Enable_V1724_Unpacking = True
Enable_V830_Unpacking = True
Enable_VLSB_Unpacking = True
Enable_VLSB_C_Unpacking = True
Enable_DBB_Unpacking = True
Do_V1731_Zero_Suppression = False
V1731_Zero_Suppression_Threshold = 100
Do_V1724_Zero_Suppression = True
V1724_Zero_Suppression_Threshold = 100
Do_VLSB_Zero_Suppression = False
VLSB_Zero_Suppression_Threshold = 60
Do_VLSB_C_Zero_Suppression = True
VLSB_C_Zero_Suppression_Threshold = 60
Enable_TOF = True
Enable_EMR = True
Enable_KL = True
Enable_CKOV = True
DAQ_cabling_file = "/files/cabling/DAQChannelMap.txt"
DAQ_hostname = 'miceraid1a'
DAQ_monitor_name = 'MICE_Online_Monitor'
daq_online_file = '' # set to a file name to force InputCppDAQOnlineData to take
                     # data from a file - mock-up of online for testing, not for
                     # production use (use offline recon here)
daq_online_spill_delay_time = 0. # delay in seconds between daq reads, intended
                                 # for mocking MICE target pulses

# tof digitization
TOFconversionFactor = 0.005 # MeV
TOFpmtTimeResolution = 0.1 # nanosecond
TOFattenuationLength = 140 * 10 # mm
TOFadcConversionFactor = 0.125
TOFtdcConversionFactor = 0.025 # nanosecond
TOFpmtQuantumEfficiency = 0.25
TOFscintLightSpeed =  170.0 # mm/ns

# this is used by the reconstuction of the TOF detectors
TOF_trigger_station = "tof1"

# this sets the source for the calibrations
# by default it is from CDB
# set it to 'file' if you want to load local files
# if you set file, then uncomment the calib files below
TOF_calib_source = "CDB"

#TOF_cabling_file = "/files/cabling/TOFChannelMap.txt"
#TOF_TW_calibration_file = "/files/calibration/tofcalibTW_dec2011.txt"
#TOF_T0_calibration_file = "/files/calibration/tofcalibT0_trTOF1_dec2011.txt"
#TOF_T0_calibration_file = "/files/calibration/tofcalibT0_trTOF0.txt"
#TOF_Trigger_calibration_file = "/files/calibration/tofcalibTrigger_trTOF1_dec2011.txt"
#TOF_Trigger_calibration_file = "/files/calibration/tofcalibTrigger_trTOF0.txt"

TOF_findTriggerPixelCut = 0.5 # nanosecond
TOF_makeSpacePiontCut = 0.5 # nanosecond

# the date for which we want the cabling and calibration
# date can be 'current' or a date in YYYY-MM-DD hh:mm:ss format
TOF_calib_date_from = 'current'
TOF_cabling_date_from = 'current'

Enable_timeWalk_correction = True
Enable_triggerDelay_correction = True
Enable_t0_correction = True

# this is used by the reconstuction of the KL detectors
KL_cabling_file = "/files/cabling/KLChannelMap.txt"

daq_data_path = '%s/src/input/InputCppDAQData' % os.environ.get("MAUS_ROOT_DIR") # path to daq data. Multiple locations can be specified with a space
daq_data_file = '02873.003' # file name for daq data; if this is just a integer string, MAUS assumes this is a run number. Multiple entries can be specified separated by a space

maus_version = "" # set at runtime - do not edit this (changes are ignored)
configuration_file = "" # should be set on the command line only (else ignored)

doc_store_class = "docstore.MongoDBDocumentStore.MongoDBDocumentStore"
doc_collection_name = "spills" # Default document collection name. Only needed if using multi_process mode. If "auto" then a collection name will be auto-generated for spills output by input-transform workflows.
doc_store_event_cache_size = 10**8 # Maximum size of the Mongo cache to cache at any one time in multiprocessing mode, as used by e.g. online code. Corresponds to ~ n/3 spills.

mongodb_host = "localhost" # Default MongoDB host name. Only needed if using MongoDBDocumentStore.
mongodb_port = 27017 # Default MongoDB port. Only needed if using MongoDBDocumentStore.
mongodb_database_name = "mausdb" # Default MongoDB database name. Only needed if using MongoDBDocumentStore.
mongodb_collection_name = "spills" # Default MongoDB collection name. Only needed if using MongoDBDocucmentStore.

# in multiprocessing mode, the timeout after which reconstruction of an event will be abandonded [s]
reconstruction_timeout = 10
# refresh rate for refreshing plots
reduce_plot_refresh_rate = 5
# Default OutputPyImage image directory. MAUS web application directory.
image_directory = os.environ.get("MAUS_WEB_MEDIA_RAW") if (os.environ.get("MAUS_WEB_MEDIA_RAW") != None) else os.getcwd()
# Default OutputPyImage image directory for end of run data. Will end up as image_directory+"/end_of_run/"
end_of_run_image_directory = ''
# Default OutputPyFile output directory. MAUS web application directory.
output_file_directory = os.environ.get("MAUS_WEB_MEDIA_RAW") if (os.environ.get("MAUS_WEB_MEDIA_RAW") != None) else os.getcwd()

PolynomialOpticsModel_order = 1
PolynomialOpticsModel_algorithms = ["LeastSquares",
                    "ConstrainedLeastSquares", "ConstrainedChiSquared",
                    "SweepingChiSquared", "SweepingChiSquaredWithVariableWalls"]
PolynomialOpticsModel_algorithm = "LeastSquares"
# deltas for numerical derivative calculation of Optics transfer maps
TransferMapOpticsModel_Deltas = {"t":0.01, "E":0.1,
                                 "x":0.1, "Px":0.1,
                                 "y":0.1, "Py":0.01}

