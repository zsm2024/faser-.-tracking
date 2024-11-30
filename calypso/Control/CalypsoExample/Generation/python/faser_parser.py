#
# Copyright (C) 2022 CERN for the benefit of the ATLAS collaboration
# Copyright (C) 2022 CERN for the benefit of the FASER collaboration
#
# Parser function for particle gun samples
#

def float_or_none(arg):
    if arg is None or arg == "None":
        return None

    return float(arg)

def faser_pgparser():

    import sys
    import json
    import argparse

    parser = argparse.ArgumentParser(description="Run FASER ParticleGun Simulation")

    parser.add_argument("--conf", action='append',
                        help="Specify configuration file with default values")
    parser.add_argument("--geom", default="TI12MC",
                        help="Specify geomtery to simulation (default: TI12MC, alt: TestBeamMC)")

    parser.add_argument("--run", default=123456, type=int,
                        help="Run number to generate")
    parser.add_argument("--segment", default=00000, type=int,
                        help="Segment number to generate")
    parser.add_argument("--file_length", default=1000, type=int,
                        help="Total events per file segement")

    parser.add_argument("--short", default="PG_logE",
                        help="Short description for filename")
    parser.add_argument("--tag", default=None,
                        help="Generator tag (e.g.: g0000)")

    parser.add_argument("--pid", default=[-13, 13], type=int, nargs='*',
                        help="Specify PDG ID of particle (note plus/minus different) or list (e.g.: --pid -13 13)")
    parser.add_argument("--mass", default=105.66, type=float,
                        help="Specify particle mass (in MeV)")
    parser.add_argument("--radius", default=100., type=float, nargs="*",
                        help="Specify radius (in mm)")
    parser.add_argument("--angle", default=0.005, type=float_or_none,
                        help="Specify angular width (in Rad)")
    parser.add_argument("--xpos", default=None, type=float,
                        help="Specify x position of particles (in mm)")
    parser.add_argument("--ypos", default=None, type=float,
                        help="Specify y position of particles (in mm)")
    parser.add_argument("--zpos", default=None, type=float,
                        help="Specify z position of particles (in mm) (helpful to avoid FASERnu)")

    parser.add_argument("--pidd1", default=None, type=int, 
                        help="Specify PDG ID of daugther 1 for DIF generator")
    parser.add_argument("--pidd2", default=None, type=int, 
                        help="Specify PDG ID of daugther 2 for DIF generator")


    parser.add_argument("--sampler", default="log",
                        help="Specify energy sampling (log, lin, const, hist, hist2D)")
    parser.add_argument("--hist_name", default="",
                        help="Specify energy sampling name for hist sampler file.root:hist")
    
    parser.add_argument("--minE", default=10., type=float,
                        help="Minimum energy in GeV (for log or lin sampler)")
    parser.add_argument("--maxE", default=1000., type=float,
                        help="Maximum energy (or constant) in GeV")

    parser.add_argument("--nevts", default=-1, type=int,
                        help="Number of events to generate (for debugging)")
    parser.add_argument("--dump", action='store_true',
                        help="Write out full configuration")
    parser.add_argument("--noexec", action='store_true',
                        help="Exit after parsing configuration (no execution)")

    pg_args = parser.parse_args()

    # Get defaults
    if pg_args.conf is not None:
        for conf_fname in pg_args.conf:
            with open(conf_fname, 'r') as f:
                parser.set_defaults(**json.load(f))

        # Reload arguments to override config file with command line
        pg_args = parser.parse_args()

    # Print out configuration if requested
    if pg_args.dump:
        tmp_args = vars(pg_args).copy()
        del tmp_args['dump']  # Don't dump the dump value
        del tmp_args['conf']  # Don't dump the conf file name either
        del tmp_args['nevts'] # Debugging, not part of configuration
        del tmp_args['noexec'] # Debugging, not part of configuration
        print("Configuration:")
        print(json.dumps(tmp_args, indent=4, sort_keys=False))

    if pg_args.noexec:
        sys.exit(0)

    #
    # Add some derived quantities
    #

    # Create the file name also (could add gentag here)
    pg_args.outfile = f"FaserMC-{pg_args.short}-{pg_args.run:06}-{pg_args.segment:05}"

    if pg_args.tag:
        pg_args.outfile += f"-{pg_args.tag}"

    pg_args.outfile += "-HITS.root"

    return pg_args

# All done

#
# Parser function for Foresee samples
#
def faser_fsparser():

    import sys
    import json
    import argparse

    parser = argparse.ArgumentParser(description="Run FASER Foresee Simulation")

    parser.add_argument("--conf", action='append',
                        help="Specify configuration file with default values")
    parser.add_argument("--run", default=123456, type=int,
                        help="Run number to generate")
    parser.add_argument("--segment", default=00000, type=int,
                        help="Segment number to generate")
    parser.add_argument("--file_length", default=1000, type=int,
                        help="Total events per file segement")

    parser.add_argument("--model", help="Model name")
    parser.add_argument("--model_path", help="Path to model phase space file")

    parser.add_argument("--short", default="PG_logE",
                        help="Short description for filename")
    parser.add_argument("--tag", default=None,
                        help="Generator tag (g0000)")

    parser.add_argument("--pid", default=[-13, 13], type=int, nargs=2,
                        help="Specify PDG ID of daughter particles")
    parser.add_argument("--mass", default=105.66, type=float,
                        help="Specify particle mass (in MeV)")

    parser.add_argument("--nevts", default=-1, type=int,
                        help="Number of events to generate (for debugging)")
    parser.add_argument("--dump", action='store_true',
                        help="Write out full configuration")
    parser.add_argument("--noexec", action='store_true',
                        help="Exit after parsing configuration (no execution)")

    fs_args = parser.parse_args()

    # Get defaults
    if fs_args.conf is not None:
        for conf_fname in fs_args.conf:
            with open(conf_fname, 'r') as f:
                parser.set_defaults(**json.load(f))

        # Reload arguments to override config file with command line
        fs_args = parser.parse_args()

    # Print out configuration if requested
    if fs_args.dump:
        tmp_args = vars(fs_args).copy()
        del tmp_args['dump']  # Don't dump the dump value
        del tmp_args['conf']  # Don't dump the conf file name either
        del tmp_args['nevts'] # Debugging, not part of configuration
        del tmp_args['noexec'] # Debugging, not part of configuration
        print("Configuration:")
        print(json.dumps(tmp_args, indent=4, sort_keys=False))

    if fs_args.noexec:
        sys.exit(0)

    #
    # Add some derived quantities
    #

    # Create the file name also (could add gentag here)
    fs_args.outfile = f"FaserMC-{fs_args.short}-{fs_args.run:06}-{fs_args.segment:05}"

    if fs_args.tag:
        fs_args.outfile += f"-{fs_args.tag}"

    fs_args.outfile += "-HITS.root"

    return fs_args

# All done
