#!/usr/bin/env python
"""Run tests on G4FaserAlgConfig

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations
"""

if __name__ == '__main__':

    import time
    a = time.time()
#
# Set up logging and config behaviour
#
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG, VERBOSE
    from AthenaCommon.Configurable import Configurable
    from Campaigns.Utils import Campaign
    from AthenaConfiguration.Enums import BeamType
    log.setLevel(VERBOSE)
    Configurable.configurableRun3Behavior = 1
#
# Import and set config flags
#
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    configFlags = initConfigFlags()
    configFlags.Exec.MaxEvents = 4  # can be overridden from command line with --evtMax=<number>
    configFlags.Exec.SkipEvents = 0 # can be overridden from command line with --skipEvents=<number>
    from AthenaConfiguration.Enums import ProductionStep
    configFlags.Common.ProductionStep = ProductionStep.Simulation
    configFlags.Common.MsgSourceLength = 30
#
# All these must be specified to avoid auto-configuration
#
    configFlags.Input.RunNumbers = [1000001]
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumbers = [1]
    configFlags.Input.isMC = True
    configFlags.Input.MCCampaign = Campaign.Unknown
    configFlags.Input.TypedCollections = []
    configFlags.Input.MetadataItems = []
    configFlags.Beam.Type = BeamType.Collisions
#
# Output file name
# 
    configFlags.Output.HITSFileName = "my.HITS.pool.root" # can be overridden from command line with Output.HITSFileName=<name>
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

    configFlags.GeoModel.FaserVersion = "FASERNU-04"             # Geometry set-up
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Conditions set-up
    configFlags.addFlag("Input.InitialTimeStamp", 0)             # To avoid autoconfig 
    #configFlags.GeoModel.Align.Dynamic = False
#
# Override flags above from command line
#
    import sys
    configFlags.fillFromArgs(sys.argv[1:])

    doShiftLOS = (configFlags.Sim.Beam.xangle or configFlags.Sim.Beam.yangle or
                  configFlags.Sim.Beam.xshift or configFlags.Sim.Beam.yshift)

#     from math import atan
#     from AthenaCommon.SystemOfUnits import GeV, TeV, cm, m
#     from AthenaCommon.PhysicalConstants import pi
#     import ParticleGun as PG
#     configFlags.Sim.Gun = {"Generator" : "SingleParticle", "pid" : 11, "energy" : PG.LogSampler(10*GeV, 1*TeV), "theta" :
#                            PG.GaussianSampler(0, atan((10*cm)/(7*m)), oneside = True), "phi" : [0, 2*pi], "mass" : 0.511, "radius" : -10*cm, "randomSeed" : 12345}

    if doShiftLOS:
        pgConfig = configFlags.Sim.Gun
        pgConfig["McEventKey"] = "BeamTruthEvent_ATLASCoord"
        configFlags.Sim.Gun = pgConfig


#
# By being a little clever, we can steer the geometry setup from the command line using GeoModel.FaserVersion
#
# Start with minimal configuration for Testbeam
#
    detectors = ['Veto', 'Preshower', 'FaserSCT', 'Ecal']
    if configFlags.GeoModel.FaserVersion.count("TB") == 0 :
        detectors += ['Trigger', 'Dipole']
    if configFlags.GeoModel.FaserVersion.count("FASERNU") > 0 :
        detectors += ['Emulsion']
    if configFlags.GeoModel.FaserVersion.count("FASERNU-03") > 0 or configFlags.GeoModel.FaserVersion.count("FASERNU-04") > 0:
        detectors += ['VetoNu', 'Trench']
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
# Check whether a real input file was specified
#
    if configFlags.Input.Files and configFlags.Input.Files != ["_CALYPSO_GENERIC_INPUTFILE_NAME_"] :
        print("Input.Files = ",configFlags.Input.Files)

#
# If so, and only one file that ends in .events or .hepmc read as HepMC
#
        if len(configFlags.Input.Files) == 1 and (configFlags.Input.Files[0].endswith(".events") or configFlags.Input.Files[0].endswith(".hepmc")):

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
# Shift LOS for input file (into AthAlgSeq)
#
    if doShiftLOS:

        import McParticleEvent.Pythonizations
        from GeneratorUtils.ShiftLOSConfig import ShiftLOSCfg
        cfg.merge(ShiftLOSCfg(configFlags, 
                              xcross = configFlags.Sim.Beam.xangle, ycross = configFlags.Sim.Beam.yangle,
                              xshift = configFlags.Sim.Beam.xshift, yshift = configFlags.Sim.Beam.yshift),
                              sequenceName = "AthAlgSeq")
#
# If not, configure the particle gun as requested, or using defaults
#
    else :
#
# Particle gun generators - the generator, energy, angle, particle type, position, etc can be modified by passing keyword arguments
#
        from FaserParticleGun.FaserParticleGunConfig import FaserParticleGunCfg
        cfg.merge(FaserParticleGunCfg(configFlags), sequenceName="AthBeginSeq")
        
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

        from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
        cfg.merge(McEventSelectorCfg(configFlags))

# #
# # Shift LOS (original version where the configuration tool handled the AlgSequence)
# #

#     if doShiftLOS:

#         import McParticleEvent.Pythonizations
#         from GeneratorUtils.ShiftLOSConfig import ShiftLOSCfg
#         cfg.merge(ShiftLOSCfg(configFlags, 
#                               xcross = configFlags.Sim.Beam.xangle, ycross = configFlags.Sim.Beam.yangle,
#                               xshift = configFlags.Sim.Beam.xshift, yshift = configFlags.Sim.Beam.yshift))

#
# Unclear if we want/need this
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
# Uncomment to check volumes for overlap - will cause CTest to fail due to overwriting file
#
#    from G4DebuggingTools.G4DebuggingToolsConfigNew import VolumeDebugger
#    cfg.merge(VolumeDebugger(configFlags, name="G4UA::FaserUserActionSvc", TargetVolume="", Verbose=True))
#
# Dump config
#
    cfg.getService("G4FaserGeometryNotifierSvc").OutputLevel=VERBOSE
    from AthenaConfiguration.ComponentFactory import CompFactory
    cfg.addEventAlgo(CompFactory.JobOptsDumperAlg(FileName="G4FaserTestConfig.txt"))
    cfg.getService("StoreGateSvc").Dump = True
    cfg.getService("ConditionStore").Dump = True
    cfg.getService("MetaDataStore").Dump = True
    cfg.printConfig(withDetails=True, summariseProps = False)  # gags on ParticleGun if summariseProps = True?

    configFlags.dump()
    f = open("test.pkl","wb")
    cfg.store(f)
    f.close()
#
# Execute and finish
#
    #cfg.foreach_component("*").OutputLevel = VERBOSE

    sc = cfg.run()

    b = time.time()
    log.info("Run G4FaserAlg in " + str(b-a) + " seconds")
#
# Success should be 0
#
    sys.exit(not sc.isSuccess())

