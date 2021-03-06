#!/usr/bin/env python

#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
Configuration file for simulate particles in grid configuration. Fires particles
in a (x, y) grid and looks to see how they are imaged on the far side of a
single solenoid.
"""

import os

def get_beam_def(x, y):
    """
    Makes a "pencil" beam definition i.e. no transverse or longitudinal
    distribution.
    """
    import xboa.Common
    total_energy = 0.95*xboa.Common.units['GeV']
    x *= xboa.Common.units['mm']
    y *= xboa.Common.units['mm']
    beam_def = {
          "reference":{
              "position":{"x":x, "y":y, "z":-2000.-1.e-9},
              "momentum":{"x":0., "y":0.0, "z":1.},
              "particle_id":2212, "energy":total_energy, "time":0.0,
              "random_seed":1,
           },
          "random_seed_algorithm":"incrementing_random",
          "n_particles_per_spill":1,
          "transverse":{
              "transverse_mode":"pencil",
          },
          "longitudinal":{
              "longitudinal_mode":"pencil",
              "momentum_variable":"p",
          },
          "coupling":{"coupling_mode":"none"}
    }
    return beam_def

# for loop to generate multiple beam definitions
beam_defs = []
for x in range(-100, 101, 10):
    for y in range(-100, 101, 10):
        beam_defs.append(get_beam_def(float(x), float(y)))

simulation_geometry_filename = os.path.join(os.environ['MAUS_ROOT_DIR'],
                                 'bin', 'examples',
                                 'simulate_particles_in_grid', "solenoid.dat")
check_overlaps = True
verbose_level = 1
spill_generator_number_of_spills = 1


# here we register the individual beam definitions 
beam = {
    "particle_generator":"counter", # generate a fixed number of primaries per
                                    # beam definition per spill
    "random_seed":0, # random seed
    "definitions":beam_defs
}

