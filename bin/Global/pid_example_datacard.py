"""
Datacard to use when performing PID. The precursors to this mapper are
MapCppGlobalReconImport and MapCppGlobalTrackMatching, and data passed to
the Global PID must first have been reconstructed through these mappers

Probability density functions (PDFs) for the standard MICE physics grid are
in the process of being produced, and will eventually be packaged with MAUS.
Should they become available before the next MAUS release, they will be made
available online, and pointed to by the MAUS workbook

Until then, users are still able to produce PDFs from their own simulations,
using pid_pdf_generator.py, found in bin/Global, and run PID against those
"""
# pylint: disable=C0103, W0611

import os

# The input root file containing global tracks you wish to PID
input_root_file_name = "maus_output.root"

# Output root file with track pid information included
output_root_file_name = "output_Global_PID.root"

# Path to PDF file. The one currently listed (on 16/02/2016) is outdated
# and will be replaced, and this card updated.
# Users should enter the path to their own PDFs file here.
PID_PDFs_file =  '%s/src/map/MapCppGlobalPID/PIDhists.root'\
                 % os.environ.get("MAUS_ROOT_DIR")

# PID MICE configuration, 'step_4' for Step IV running,
# 'commissioning' for field free commissioning data (straight tracks)
pid_config = "step_4"

# PID running mode - selects which PID variables are used. 'online' corresponds
# to less beam (momentum) dependent variables, 'offline' uses all variables and
# requires that specific PDFs for the beam already exist. 'custom' allows user
# to choose which variables to use, and these should then be set as datacards.
# However it is not recommended to use the custom setting unless you are the
# person currently developing the Global PID.
pid_mode = "offline"

# In case you ignore the advice about not using a custom set, here at least
# are the instructions for you to use it properly.
# If pid_mode = "custom", variables to use should be set here as a space
# separated list, i.e. custom_pid_set = "PIDVarA PIDVarC PIDVarD". 
custom_pid_set = "PIDVarA"

# Tag used by both MapCppGlobalPID and ReduceCppGlobalPID, determines which
# PDFs to perform PID against. A typical tag here
# would be the emittance and momentum, e.g. 3-140, 6-200, etc. Alternatively,
# users may choose to enter the run number they are simulating for here.
# The tag used must match the tag that was provided during PDF production.
pid_beam_setting = "6-200"

# Polarity of running mode, used by pid to select whether to run pid against
# positive or negative particles, value can be "positive" or "negative".
pid_beamline_polarity = "positive"

# PID confidence level- set the margin (in %) between the confidence levels of
# competing pid hypotheses before they are selected as the correct hypothesis
pid_confidence_level = 10

# PID track selection- select which tracks from TrackMatching to perform PID on. 
# Can perform PID on all tracks by setting to "all" which will give you PID'd 
# through and unmatched US and DS tracks, 
# on "through" tracks only (constituent tracks will be PID'd, so this excludes orphans) 
# with through or on all upstream and downstream tracks (ignoring whether tracks have 
# been through-matched) with "us\_and\_ds"
pid_track_selection = "all"
