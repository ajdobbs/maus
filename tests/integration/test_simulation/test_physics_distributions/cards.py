class physics_list_test:
    import xboa.Common as Common
    momentum = 200.0
    pid = -13
    number_of_events = 100000
    energy = (momentum**2+Common.pdg_pid_to_mass[abs(pid)]**2)**0.5

spill_generator_number_of_spills = 1
simulation_reference_particle = { # used for setting particle phase
    "position":{"x":0.0, "y":-0.0, "z":0.0},
    "momentum":{"x":0.0, "y":0.0, "z":1.0},
    "particle_id":physics_list_test.pid,
    "energy":physics_list_test.energy,
    "time":0.0,
    "random_seed":0
}

beam = {
    "particle_generator":"counter",
    "random_seed":0,
    "definitions":[
    ##### MUONS #######
    {
       "reference":simulation_reference_particle, # reference particle
       "random_seed_algorithm":"incrementing", # algorithm for seeding MC
       "n_particles_per_spill":physics_list_test.number_of_events,
       "transverse":{
          "transverse_mode":"pencil",
       },
       "longitudinal":{
          "longitudinal_mode":"pencil",
          "momentum_variable":"p"
       },
       "coupling":{"coupling_mode":"none"} # no dispersion
    },
    ]
}
simulation_geometry_filename="MaterialBlock.dat"
verbose_level = 1
