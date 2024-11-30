#!/usr/bin/env python
"""
Produce particle gun samples
Derived from G4FaserAlgConfig

Usage:
faser_particlegun.py --conf=<config_file>

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations
"""

if __name__ == '__main__':

    import sys
    import time
    a = time.time()
#
# Parse command-line options
#
    from Generation.faser_parser import faser_pgparser
    args = faser_pgparser()
#
# Figure out events to run and skip
#
    nskipped = args.segment*args.file_length
    if args.nevts > 0:
        nevents = args.nevts
    else:
        nevents = args.file_length
#
# Print out what we are doing
#
    print(f"Generating {nevents} evnts into file {args.outfile}")
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
    
    configFlags = initConfigFlags()
    configFlags.Exec.MaxEvents = nevents
    configFlags.Exec.SkipEvents = nskipped
    from AthenaConfiguration.Enums import ProductionStep
    configFlags.Common.ProductionStep = ProductionStep.Simulation
#
# All these must be specified to avoid auto-configuration
#
    configFlags.Input.RunNumbers = [args.run] 
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumbers = [(args.segment+1)]
    configFlags.Input.isMC = True
    configFlags.IOVDb.DatabaseInstance = "OFLP200"   # Use MC conditions
#
# Output file name
# 
    configFlags.Output.HITSFileName = args.outfile
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
    configFlags.addFlag("Sim.Gun",{"Generator" : "SingleParticle"})  # Property bag for particle gun keyword:argument pairs
    configFlags.addFlag("Sim.Beam.xangle", 0)  # Potential beam crossing angles
    configFlags.addFlag("Sim.Beam.yangle", 0)    
    configFlags.addFlag("Sim.Beam.xshift", 0)  # Potential beam shift
    configFlags.addFlag("Sim.Beam.yshift", 0)        

    if args.geom == "TI12MC" or args.geom == "TI12MC04":
        # New TI12 geometry
        configFlags.GeoModel.FaserVersion = "FASERNU-04"  # Geometry set-up
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04"  # Conditions set-up
        # TI12 detectors
        detectors = ['Veto', 'VetoNu', 'Preshower', 'FaserSCT', 'Ecal', 'Trigger', 
                     'Dipole', 'Emulsion', 'Trench']

    elif args.geom == "TI12MC03":
        # 2022 TI12 geometry
        configFlags.GeoModel.FaserVersion = "FASERNU-03"  # Geometry set-up
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"  # Conditions set-up
        # TI12 detectors
        detectors = ['Veto', 'VetoNu', 'Preshower', 'FaserSCT', 'Ecal', 'Trigger', 
                     'Dipole', 'Emulsion', 'Trench']

    elif args.geom == "TestBeamMC":
        # Define 2021 test beam geometry
        configFlags.GeoModel.FaserVersion = "FASER-TB01"   # Geometry set-up
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-TB01"   # Conditions set-up
        # Testbeam detectors (trigger layers are actually veto counters)
        detectors = ['Veto', 'Preshower', 'FaserSCT', 'Ecal']

    else:
        print(f"Unknown geometry {args.geom}!")
        sys.exit(1)

    configFlags.addFlag("Input.InitialTimeStamp", 0) # To avoid autoconfig 
    #configFlags.GeoModel.Align.Dynamic = False
#
# Preset particle gun parameters
#
    import ParticleGun as PG
    from AthenaCommon.SystemOfUnits import GeV, TeV, cm, m
    from AthenaCommon.PhysicalConstants import pi

    if isinstance(args.pid, list):
        # Note args.pid is a list, must make this a set for ParticleGun if have > 1 
        if len(args.pid) > 1:
            pidarg = set(args.pid)
        else:
            pidarg = args.pid[0]
    else:
        # Just pass a single value
        pidarg = args.pid

    print(f"Using pid: {args.pid} => {pidarg}")

    if isinstance(args.radius, list) and len(args.radius) == 1:
        args.radius = args.radius[0]
        

    # Create the simgun dictionary
    # Negative radius gives uniform sampling
    # Positive radius gives Gaussian sampling

    if args.pidd1:
        if args.pidd2 is None:
            args.pidd2 = -args.pidd1
            
        sg_dict = {
            "Generator" : "DecayInFlight", 
            "mother_pid" : pidarg,
            "daughter1_pid" : args.pidd1,
            "daughter2_pid" : args.pidd2,        
            "mass" : args.mass, 
            "theta" :  PG.GaussianSampler(0, args.angle, oneside = True) if args.angle is not None and args.angle != "None" else None,
            "phi" : [0, 2*pi], "radius" : args.radius, 
            "randomSeed" : args.outfile 
            }

    else:

        sg_dict = {
            "Generator" : "SingleParticle", 
            "pid" : pidarg, "mass" : args.mass, 
            "theta" :  PG.GaussianSampler(0, args.angle, oneside = True) if args.angle is not None and args.angle != "None" else None,
            "phi" : [0, 2*pi], "radius" : args.radius, 
            "randomSeed" : args.outfile 
            }


    # -1000 is safely upstream of detector (to be checked)
    # Note zpos is in mm!
    if args.zpos != None:
        sg_dict["z"] = args.zpos

    if args.xpos != None:
        sg_dict["x"] = args.xpos

    if args.ypos != None:
        sg_dict["y"] = args.ypos

    # Determine energy sampling
    if args.sampler == "lin":
        sg_dict["energy"] = PG.UniformSampler(args.minE*GeV, args.maxE*GeV)
    elif args.sampler == "log":
        sg_dict["energy"] = PG.LogSampler(args.minE*GeV, args.maxE*GeV)
    elif args.sampler == "const":
        sg_dict["energy"] = PG.ConstSampler(args.maxE*GeV)
    elif args.sampler == "hist":
        nargs = len(args.hist_name.split(":"))
        if nargs == 2:
            fname, hname = args.hist_name.split(":")
            sg_dict["energy"] = PG.TH1Sampler(fname, hname)
        elif nargs == 3:
            fname, hname, scale = args.hist_name.split(":")
            sg_dict["energy"] = PG.TH1Sampler(fname, hname, scale)
        else:
            print(f"Can't parse histogram {args.hist_name}!")
            sys.exit(1)

    elif args.sampler == "hist2D":
        nargs = len(args.hist_name.split(":"))
        if nargs == 2:
            fname, hname = args.hist_name.split(":")
            sg_dict["energy"] = PG.TH2Sampler(fname, hname)
        elif nargs == 4:
            fname, hname, scalex, scaley = args.hist_name.split(":")
            sg_dict["energy"] = PG.TH2Sampler(fname, hname, scalex, scaley)
        else:
            print(f"Can't parse histogram {args.hist_name}!")
            sys.exit(1)
    else:
        print(f"Sampler {args.sampler} not known!")
        sys.exit(1)

    # Pass this in one go to configFlags
    configFlags.Sim.Gun = sg_dict

    doShiftLOS = (configFlags.Sim.Beam.xangle or configFlags.Sim.Beam.yangle or
                  configFlags.Sim.Beam.xshift or configFlags.Sim.Beam.yshift)

    if doShiftLOS:
        pgConfig = configFlags.Sim.Gun
        pgConfig["McEventKey"] = "BeamTruthEvent_ATLASCoord"
        configFlags.Sim.Gun = pgConfig
#
# By being a little clever, we can steer the geometry setup from the command line using GeoModel.FaserVersion
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
# Configure the particle gun as requested, or using defaults
#

#
# Particle gun generators - the generator, energy, angle, particle type, position, etc can be modified by passing keyword arguments
#
    from FaserParticleGun.FaserParticleGunConfig import FaserParticleGunCfg
    cfg.merge(FaserParticleGunCfg(configFlags))
    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(configFlags))
#
# Shift LOS for particle gun (into AthBeginSeq)
#
    if doShiftLOS:

        import McParticleEvent.Pythonizations
        from GeneratorUtils.ShiftLOSConfig import ShiftLOSCfg
        cfg.merge(ShiftLOSCfg(configFlags, 
                            xcross = configFlags.Sim.Beam.xangle, ycross = configFlags.Sim.Beam.yangle,
                            xshift = configFlags.Sim.Beam.xshift, yshift = configFlags.Sim.Beam.yshift),
                            sequenceName = "AthBeginSeq")
# #
# # Shift LOS
# #

#     if doShiftLOS:
#         import McParticleEvent.Pythonizations
#         from GeneratorUtils.ShiftLOSConfig import ShiftLOSCfg

#         cfg.merge(ShiftLOSCfg(configFlags, 
#                               xcross = configFlags.Sim.Beam.xangle, 
#                               ycross = configFlags.Sim.Beam.yangle,
#                               xshift = configFlags.Sim.Beam.xshift,
#                               yshift = configFlags.Sim.Beam.yshift))
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

    #cfg.foreach_component("*").OutputLevel = "INFO"  # Use warning for production

    sc = cfg.run()

    b = time.time()
    log.info("Run G4FaserAlg in " + str(b-a) + " seconds")

# Signal errors
if sc.isSuccess():
    log.info("Execution succeeded")
    sys.exit(0)
else:
    log.info("Execution failed, return 1")
    sys.exit(1)

