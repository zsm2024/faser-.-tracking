#!/usr/bin/env python
"""
Produce particle gun samples
Derived from G4FaserAlgConfig

Usage:
faserMDC_particlegun.py --conf=<config_file>

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations
"""

if __name__ == '__main__':

    import sys
    import time
    a = time.time()
#
# Parse command-line options
#
    from Generation.faserMDC_parser import faserMDC_pgparser
    args = faserMDC_pgparser()
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
    print(f"Generating {nevents} in file {args.outfile}")
#
# Set up logging and config behaviour
#
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG, VERBOSE
    from AthenaCommon.Configurable import Configurable
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
    configFlags.Input.RunNumber = [args.run] 
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumber = [(args.segment+1)]
    configFlags.Input.isMC = True
#
# Output file name
# 
    configFlags.Output.HITSFileName = args.outfile
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

    configFlags.GeoModel.FaserVersion = "FASERNU-03"   # Geometry set-up
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"   # Conditions set-up
    configFlags.addFlag("Input.InitialTimeStamp", 0)   # To avoid autoconfig 
    #configFlags.GeoModel.Align.Dynamic = False
#
# Preset particle gun parameters
#
    import ParticleGun as PG
    from AthenaCommon.SystemOfUnits import GeV, TeV, cm, m
    from AthenaCommon.PhysicalConstants import pi

    if isinstance(args.pid, list):
        # Note args.pid is a list, must make this a set for ParticleGun
        pidarg = set(args.pid)
    else:
        # Just pass a single value
        pidarg = args.pid

    print(f"Using pid: {args.pid} => {pidarg}")

    # Create the simgun dictionary
    # Negative radius gives uniform sampling
    # Positive radius gives Gaussian sampling
    sg_dict = {
        "Generator" : "SingleParticle", 
        "pid" : pidarg, "mass" : args.mass, 
        "theta" :  PG.GaussianSampler(0, args.angle, oneside = True), 
        "phi" : [0, 2*pi], "radius" : args.radius, 
        "randomSeed" : args.outfile 
    }

    # Note the nominal z position is -3.75m, which is a bit upstream of vetoNu
    # The decay volume is approximately -1.5 - 0 m, so -1m is safely inside
    # To get all the material currently defined in front, specify -5m.
    # Note zpos is in mm!
    if args.zpos:
        sg_dict["z"] = args.zpos

    # Determine energy sampling
    if args.sampler == "lin":
        sg_dict["energy"] = PG.UniformSampler(args.minE*GeV, args.maxE*GeV)
    elif args.sampler == "log":
        sg_dict["energy"] = PG.LogSampler(args.minE*GeV, args.maxE*GeV)
    elif args.sampler == "const":
        sg_dict["energy"] = PG.ConstSampler(args.maxE*GeV)
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
# MDC geometry configuration
#
    detectors = ['Veto', 'VetoNu', 'Preshower', 'FaserSCT', 'Ecal', 'Trigger', 'Dipole', 'Emulsion', 'Trench']
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
# Output file
#
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolWriteCfg(configFlags))

#
# Shift LOS
#

    if doShiftLOS:
        import McParticleEvent.Pythonizations
        from GeneratorUtils.ShiftLOSConfig import ShiftLOSCfg

        cfg.merge(ShiftLOSCfg(configFlags, 
                              xcross = configFlags.Sim.Beam.xangle, 
                              ycross = configFlags.Sim.Beam.yangle,
                              xshift = configFlags.Sim.Beam.xshift,
                              yshift = configFlags.Sim.Beam.yshift))

    
#
# Add the G4FaserAlg
#
    from G4FaserAlg.G4FaserAlgConfig import G4FaserAlgCfg
    cfg.merge(G4FaserAlgCfg(configFlags))
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
#
# Success should be 0
#
    sys.exit(not sc.isSuccess())

