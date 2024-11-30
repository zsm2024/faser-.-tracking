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
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    from G4FaserAlg.G4FaserAlgConfig import G4FaserAlgCfg
    from G4FaserServices.G4FaserServicesConfig import G4GeometryNotifierSvcCfg
    from G4FaserServices.G4FaserUserActionConfigNew import UserActionMaterialStepRecorderSvcCfg
#
# Set up logging and new style config
#
    log.setLevel(VERBOSE)
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.Enums import ProductionStep
    configFlags = initConfigFlags()
    configFlags.Common.ProductionStep = ProductionStep.Simulation
    configFlags.Sim.ReleaseGeoModel     = False
    configFlags.Input.Files = [""]
    configFlags.Input.isMC = True
    configFlags.Input.RunNumber = [12345]
    configFlags.Input.OverrideRunNumber = True
    configFlags.Input.LumiBlockNumber = [1]
    Configurable.configurableRun3Behavior = 1
    configFlags.Common.isOnline = False
    configFlags.Beam.Type = "collisions"
    configFlags.Beam.Energy = 7*TeV                              # Informational, does not affect simulation
    configFlags.GeoModel.FaserVersion = "FASERNU-03"               # Always needed
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-02"             # Always needed; must match FaserVersion
    configFlags.addFlag("Input.InitialTimeStamp", 0)
#
# Output settings
#
    configFlags.Output.HITSFileName = "MaterialStepCollection.root"
    configFlags.GeoModel.GeoExportFile = "faserGeo.db" # Optional dump of geometry for browsing in vp1light
    #configFlags.GeoModel.Align.Dynamic  = False
#
# Geometry-related settings
# Do not change!
#
    detectors = ['Veto', 'Trigger', 'Preshower', 'FaserSCT', 'Dipole', 'Ecal']
    from CalypsoConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(configFlags, detectors, toggle_geometry=True)
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
    pg.sampler.pid = 999
    pg.sampler.mom = PG.EThetaMPhiSampler(energy=1*TeV, theta=[0, pi/200], phi=[0, 2*pi])
    pg.sampler.pos = PG.PosSampler(x=[-150, 150], y=[-150, 150], z=-2100.0, t=0.0)
    acc.addEventAlgo(pg, "AthBeginSeq", primary =  True) # to run *before* G4
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
			      "Trk::MaterialStepCollection#*",
                              "ScintHitCollection#*",
                              "FaserSiHitCollection#*"
                            ], disableEventTag=True))
    acc.getEventAlgo(outputStreamName("HITS")).AcceptAlgs = ["G4FaserAlg"]               # optional
    acc.getEventAlgo(outputStreamName("HITS")).WritingTool.ProcessingTag = "StreamHITS"  # required
#
#  Here is the configuration of the Geant4 pieces
#    
    acc.merge(FaserGeometryCfg(configFlags))
    acc.merge(UserActionMaterialStepRecorderSvcCfg(configFlags))
    acc.merge(G4FaserAlgCfg(configFlags))
    acc.addService(G4FaserGeometryNotifierSvcCfg(configFlags, ActivateLVNotifier=True))
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
    sys.exit(int(acc.run(maxEvents=1000000).isFailure()))
