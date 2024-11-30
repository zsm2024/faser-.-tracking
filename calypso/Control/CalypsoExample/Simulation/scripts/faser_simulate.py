#!/usr/bin/env python
"""
Produce simulated hits from input 4-vectors
Derived from G4FaserAlgConfig

Usage:
faser_simulate.py filepath outfile

filepath - full path, including url if needed, to the input 4-vector file
outfile - output filename, parameters will be inferred from this name

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations
"""

if __name__ == '__main__':

    import sys
    import time
    a = time.time()
#
# Parse command-line options
#
    import argparse

    parser = argparse.ArgumentParser(description="Run FASER simulation")

    parser.add_argument("file_path",
                        help="Fully qualified path of the raw input file")

    parser.add_argument("output",
                        help="Output file name")

    #parser.add_argument("--run", type=int, default=123456,
    #                    help="Specify run number to use in simulated data")

    parser.add_argument("--geom", default="TI12MC",
                        help="Specify geomtery to simulation (default: TI12MC, alt: TestBeamMC)")


    parser.add_argument("--xangle", type=float, default=0.0,
                        help="Specify H crossing angle (in Radians)")
    parser.add_argument("--yangle", type=float, default=0.0,
                        help="Specify V crossing angle (in Radians)")
    parser.add_argument("--xshift", type=float, default=0.0,
                        help="Specify H shift of events wrt FASER (in mm)")
    parser.add_argument("--yshift", type=float, default=0.0,
                        help="Specify V shift of events wrt FASER (in mm)")

    parser.add_argument("-t", "--tag", default="",
                        help="Specify sim tag (to append to output filename)")
    parser.add_argument("-s", "--skip", type=int, default=0,
                        help="Specify number of events to skip (default: none)")
    parser.add_argument("-n", "--nevents", type=int, default=-1,
                        help="Specify number of events to process (default: all)")
    parser.add_argument("-v", "--verbose", action='store_true', 
                        help="Turn on DEBUG output")

    args = parser.parse_args()

    from pathlib import Path

    filepath = Path(args.file_path)

#
# Parse input file
#
    print(f"Starting digitization of {filepath.name}")

    filestem = filepath.stem
    if len(args.tag) > 0:
        if args.tag in filestem:
            print(f"Not adding tag {args.tag} to {filestem}")
        else:
            filestem += f"-{args.tag}"

    if args.output:
        # Just directly specify the output filename
        outfile = args.output

        spl = outfile.split('-')
        if len(spl) < 4:
            print(f"Can't get run number from {outfile}!")
            sys.exit(1)

        runnum = int(spl[2])
        segnum = int(spl[3])

    else:
        outfile = f"{filestem}-HITS.root"
        print(f"Output file name not specified")
        sys.exit(1)

    print(f"Outfile: {outfile}")

#
# Figure out events to run
#
    if args.skip > 0:
        print(f"Skipping {args.skip} events by command-line option")

    if args.nevents > 0:
        print(f"Reconstructing {args.nevents} events by command-line option")
#
# Set up logging and config behaviour
#
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG, VERBOSE
    from AthenaCommon.Configurable import Configurable
    from Campaigns.Utils import Campaign
    from AthenaConfiguration.Enums import BeamType
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = 1
#
# Import and set config flags
#
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.Enums import ProductionStep
    configFlags = initConfigFlags()
    configFlags.Common.ProductionStep = ProductionStep.Simulation
#
# All these must be specified to avoid auto-configuration
#
    configFlags.Input.RunNumbers = [ runnum ] 
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumbers = [ (segnum+1) ]
    configFlags.Input.TimeStamps = [0]
    configFlags.Input.isMC = True
#
# Input file name
# 
    # Path mangles // in url, so use direct file_path here
    configFlags.Input.Files = [ args.file_path ]
#
# Skip events
#
    configFlags.Exec.SkipEvents = args.skip
    configFlags.Exec.MaxEvents = args.nevents
#
# Output file name
# 
    configFlags.Output.HITSFileName = outfile
#
# More config flags (R24)
#
    configFlags.Common.MsgSourceLength = 30
    configFlags.Input.MCCampaign = Campaign.Unknown
    configFlags.Input.TypedCollections = []
    configFlags.Input.MetadataItems = []
    configFlags.Beam.Type = BeamType.Collisions
#
# Sim configFlags
#
    configFlags.GeoModel.Layout = "FASER"
    configFlags.Sim.PhysicsList = "FTFP_BERT"
    configFlags.Sim.ReleaseGeoModel = False
    configFlags.Sim.IncludeParentsInG4Event = True # Controls whether BeamTruthEvent is written to output HITS file
#
# Figure out configuration
#
    if args.geom == "TI12MC" or args.geom == "TI12MC04":
        # New TI12 geometry
        configFlags.GeoModel.FaserVersion = "FASERNU-04"  # Geometry set-up
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04"  # Conditions set-up
        # TI12 detectors
        detectors = ['Veto', 'VetoNu', 'Preshower', 'FaserSCT', 'Ecal', 
                     'Trigger', 'Dipole', 'Emulsion', 'Trench']

    elif args.geom == "TI12MC03":
        # Old TI12 geometry
        configFlags.GeoModel.FaserVersion = "FASERNU-03"  # Geometry set-up
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"  # Conditions set-up
        # TI12 detectors
        detectors = ['Veto', 'VetoNu', 'Preshower', 'FaserSCT', 'Ecal', 
                     'Trigger', 'Dipole', 'Emulsion', 'Trench']

    elif args.geom == "TestBeamMC":
        # Define 2021 test beam geometry
        configFlags.GeoModel.FaserVersion = "FASER-TB01"   # Geometry set-up
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-TB01" # Conditions set-up
        # Testbeam detectors (trigger layers are actually veto counters)
        detectors = ['Veto', 'Preshower', 'FaserSCT', 'Ecal']

    else:
        print(f"Unknown geometry {args.geom}!")
        sys.exit(1)


    #
    # Units are radians and mm
    # Probably should only be allowed in TI12, but leave it here for now
    configFlags.addFlag("Sim.Beam.xangle", args.xangle)  # Potential beam crossing angles
    configFlags.addFlag("Sim.Beam.yangle", args.yangle)    
    configFlags.addFlag("Sim.Beam.xshift", args.xshift)  # Potential beam shift
    configFlags.addFlag("Sim.Beam.yshift", args.yshift)    

    configFlags.addFlag("Input.InitialTimeStamp", 0)     # To avoid autoconfig
    #configFlags.GeoModel.Align.Dynamic = False

    # import sys
    # configFlags.fillFromArgs(sys.argv[1:])

    doShiftLOS = (configFlags.Sim.Beam.xangle or configFlags.Sim.Beam.yangle or
                  configFlags.Sim.Beam.xshift or configFlags.Sim.Beam.yshift)
#
# Setup detector flags
#
    from CalypsoConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(configFlags, detectors, toggle_geometry=True)
#
# Finalize flags
#
    configFlags.lock()
#
# Initialize a new component accumulator
#
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(configFlags)
#
# Check whether a known input file was specified
#
    if configFlags.Input.Files[0].endswith(".events") or configFlags.Input.Files[0].endswith(".hepmc"):

        from HEPMCReader.HepMCReaderConfig import HepMCReaderCfg

        if doShiftLOS:
            cfg.merge(HepMCReaderCfg(configFlags, McEventKey = "BeamTruthEvent_ATLASCoord"), sequenceName = "AthBeginSeq")
        else:
            cfg.merge(HepMCReaderCfg(configFlags), sequenceName = "AthBeginSeq")

        from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
        cfg.merge(McEventSelectorCfg(configFlags))
#
# Else, set up to read it as a pool.root file
#
    else:
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        cfg.merge(PoolReadCfg(configFlags))

        if doShiftLOS:
            from SGComps.AddressRemappingConfig import InputOverwriteCfg
            # Rename old truth collection to add ATLAS coord to can still use BeamTruthEvent for the one in FASER Coords
            cfg.merge(InputOverwriteCfg("McEventCollection", "BeamTruthEvent", "McEventCollection", "BeamTruthEvent_ATLASCoord"))

#
# Shift LOS
#
    if doShiftLOS:

        import McParticleEvent.Pythonizations
        from GeneratorUtils.ShiftLOSConfig import ShiftLOSCfg
        cfg.merge(ShiftLOSCfg(configFlags, 
                              xcross = configFlags.Sim.Beam.xangle, ycross = configFlags.Sim.Beam.yangle,
                              xshift = configFlags.Sim.Beam.xshift, yshift = configFlags.Sim.Beam.yshift),
                              sequenceName = "AthAlgSeq")
#
# Header conversion seems necessary to write events
#
    from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
    cfg.merge(EventInfoCnvAlgCfg(configFlags, disableBeamSpot=True))
#
# Add the G4FaserAlg
#
    from G4FaserAlg.G4FaserAlgConfig import G4FaserAlgCfg
    cfg.merge(G4FaserAlgCfg(configFlags))
#
# Output file
#
    AcceptAlgNames = ['G4FaserAlg']
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    from G4FaserAlg.G4FaserAlgConfig import getG4FaserAlgItemList
    cfg.merge(OutputStreamCfg(configFlags, "HITS", ItemList=getG4FaserAlgItemList(configFlags), disableEventTag=True, AcceptAlgs=AcceptAlgNames))
    # Add in-file MetaData
    from xAODMetaDataCnv.InfileMetaDataConfig import SetupMetaDataForStreamCfg
    cfg.merge(SetupMetaDataForStreamCfg(configFlags, "HITS", AcceptAlgs=AcceptAlgNames))
#
# Dump config
#
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addEventAlgo(CompFactory.JobOptsDumperAlg(FileName="G4FaserTestConfig.txt"))
    cfg.getService("StoreGateSvc").Dump = True
    cfg.getService("ConditionStore").Dump = True
    cfg.printConfig(withDetails=True, summariseProps = False)  # gags on ParticleGun if summariseProps = True?

    configFlags.dump()
    #f = open("test.pkl","wb")
    #cfg.store(f)
    #f.close()
#
# Execute and finish
#

    # This fails with ShiftLOSCfg...
    #if args.verbose:
    #    cfg.foreach_component("*").OutputLevel = "DEBUG" 
    #else:
    #    cfg.foreach_component("*").OutputLevel = "INFO"  

    sc = cfg.run(maxEvents=args.nevents)

    b = time.time()
    log.info("Finish execution in " + str(b-a) + " seconds")
#
# Success should be 0
#
    sys.exit(int(sc.isFailure()))

