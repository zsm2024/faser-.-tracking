#!/usr/bin/env python
if __name__ == "__main__":
    import os
    import sys
    import GaudiPython
    import ParticleGun as PG
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaCommon.AppMgr import *
    from AthenaCommon.Logging import log, logging
    from AthenaCommon.SystemOfUnits import TeV
    from AthenaCommon.PhysicalConstants import pi
    from AthenaCommon.Constants import VERBOSE, INFO
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
#    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
#    from G4FaserAlg.G4FaserAlgConfig import G4FaserAlgCfg
#    from G4FaserServices.G4FaserServicesConfig import G4GeometryNotifierSvcCfg
#
# Set up logging and new style config
#
    log.setLevel(VERBOSE)
    Configurable.configurableRun3Behavior = True
#
# Input settings (Generator file)
#
    configFlags = initConfigFlags()
#   from AthenaConfiguration.TestDefaults import defaultTestFiles
#   configFlags.Input.Files = defaultTestFiles.EVNT
#
# Alternatively, these must ALL be explicitly set to run without an input file
# (if missing, it will try to read metadata from a non-existent file and crash)
#
    configFlags.Input.Files = [""]
    configFlags.Input.isMC = True
    configFlags.Input.RunNumber = 12345
    configFlags.Input.Collections = [""]
    configFlags.Input.ProjectName = "mc19"
    configFlags.Common.isOnline = False
    configFlags.Beam.Type = "collisions"
    configFlags.Beam.Energy = 7*TeV                              # Informational, does not affect simulation
    configFlags.GeoModel.FaserVersion = "FASER-01"               # Always needed
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
# Workaround for bug/missing flag; unimportant otherwise 
    configFlags.addFlag("Input.InitialTimeStamp", 0)
# Workaround to avoid problematic ISF code
    configFlags.GeoModel.Layout = "Development"
#
# Output settings
#
    configFlags.Output.HITSFileName = "my.EVT.pool.root"
#    configFlags.GeoModel.GeoExportFile = "faserGeo.db" # Optional dump of geometry for browsing in vp1light
#
# Geometry-related settings
# Do not change!
#
#    detectors = ['Veto', 'Trigger', 'Preshower', 'FaserSCT', 'Dipole', 'Ecal']
#    from CalypsoConfiguration.DetectorConfigFlags import setupDetectorsFromList
#    setupDetectorsFromList(configFlags, detectors, toggle_geometry=True)
    #configFlags.GeoModel.Align.Dynamic  = False
    configFlags.Sim.ReleaseGeoModel     = False
#
# All flags should be set before calling lock
#
    configFlags.lock()
#
# Construct ComponentAccumulator
#
    acc = MainServicesCfg(configFlags)
#
# Particle Gun generator (comment out to read generator file)
# Raw energies (without units given) are interpreted as MeV
#
    pg = PG.ParticleGun()
    pg.McEventKey = "BeamTruthEvent"
    pg.randomSeed = 123456
    pg.sampler.pid = -13
    pg.sampler.mom = PG.EThetaMPhiSampler(energy=1*TeV, theta=[0, pi/20], phi=[0, 2*pi], mass=105.71)
    pg.sampler.pos = PG.PosSampler(x=[-5, 5], y=[-5, 5], z=-2100.0, t=0.0)
    acc.addEventAlgo(pg, "AthBeginSeq") # to run *before* G4
#
# Only one of these two should be used in a given job
# (MCEventSelectorCfg for generating events with no input file,
#  PoolReadCfg when reading generator data from an input file)
#    
    acc.merge(McEventSelectorCfg(configFlags))
    # acc.merge(PoolReadCfg(configFlags))
#
#  Output stream configuration
#
    acc.merge(OutputStreamCfg(configFlags, 
                              "HITS", 
                             ["EventInfo#*",
                              "McEventCollection#TruthEvent",
                              "McEventCollection#BeamTruthEvent",
                              "ScintHitCollection#*",
                              "FaserSiHitCollection#*"
                            ], disableEventTag=True))
#    acc.getEventAlgo("OutputStreamHITS").AcceptAlgs = ["G4FaserAlg"]               # optional
    acc.getEventAlgo(outputStreamName("HITS")).WritingTool.ProcessingTag = "StreamHITS"  # required
#
#  Here is the configuration of the Geant4 pieces
#    
#    acc.merge(FaserGeometryCfg(configFlags))
#    acc.merge(G4FaserAlgCfg(configFlags))
#    acc.addService(G4FaserGeometryNotifierSvcCfg(configFlags, ActivateLVNotifier=True))
#
# Verbosity
#
#    configFlags.dump()
#    logging.getLogger('forcomps').setLevel(VERBOSE)
#    acc.foreach_component("*").OutputLevel = VERBOSE
#    acc.foreach_component("*ClassID*").OutputLevel = INFO
#    acc.getService("StoreGateSvc").Dump=True
#    acc.getService("ConditionStore").Dump=True
#    acc.printConfig()
    f=open('FaserG4AppCfg_EVNT.pkl','wb')
    acc.store(f)
    f.close()
#
# Execute and finish
#
    sys.exit(int(acc.run(maxEvents=500).isFailure()))
