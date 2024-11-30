from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

if __name__ == "__main__":

    import sys
    from AthenaCommon.Logging import log, logging
    from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg

    # Set up logging and new style config
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = True

    configFlags = initConfigFlags()
    configFlags.Input.Files = [
        "/eos/experiment/faser/rec/2022/p0008/007984/Faser-Physics-007984-00000-p0008-xAOD.root"
        ]

    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersionS
    configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
    configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig
    configFlags.Input.isMC = False                               # Needed to bypass autoconfig
    configFlags.GeoModel.FaserVersion     = "FASERNU-03"         # FASER geometry
    configFlags.Common.isOnline = False
    #configFlags.GeoModel.Align.Dynamic = False
    configFlags.Beam.NumberOfCollisions = 0.
    configFlags.Detector.GeometryFaserSCT = True

    for stream in ["STREAM1", "STREAM2", "STREAM3"]:
        configFlags.addFlag(f"Output.AOD{stream}FileName", f"my.{stream}.xAOD.root")

    configFlags.lock()

    # Core components
    cfg = MainServicesCfg(configFlags)
    cfg.merge(PoolReadCfg(configFlags))
    cfg.merge(PoolWriteCfg(configFlags))
    cfg.merge(FaserGeometryCfg(configFlags))

    # Derivations

    from DerivationAlgs.DerivationAlgsConfig import FullyConfiguredStream

    name = "STREAM1"

    
    cfg.merge(FullyConfiguredStream(configFlags, stream = name,
                                    tools = [CompFactory.ExampleDerivationTool(name + "_TestTool", SaveFraction = 10.)])
              )

    name = "STREAM2"
    cfg.merge(FullyConfiguredStream(configFlags, stream = name,
                                    tools = [ CompFactory.ExampleDerivationTool(name + "_TestTool", SaveFraction = 90.)],
                                    items = [ "xAOD::EventInfo#*"
                                              , "xAOD::EventAuxInfo#*"
                                              , "xAOD::FaserTriggerData#*"
                                              , "xAOD::FaserTriggerDataAux#*"
                                              ])                            
              )

    name = "STREAM3"
    cfg.merge(FullyConfiguredStream(configFlags, stream = name,
                                    tools = [CompFactory.TriggerStreamTool(name + "_TriggerTool")], 
                                    items = [ "xAOD::EventInfo#*"
                                              , "xAOD::EventAuxInfo#*"
                                              , "xAOD::FaserTriggerData#*"
                                              , "xAOD::FaserTriggerDataAux#*"
                                              ])                            
              )              
                                                  

    # Hack to avoid problem with our use of MC databases when isMC = False
    replicaSvc = cfg.getService("DBReplicaSvc")
    replicaSvc.COOLSQLiteVetoPattern = ""
    replicaSvc.UseCOOLSQLite = True
    replicaSvc.UseCOOLFrontier = False
    replicaSvc.UseGeomSQLite = True

    # Execute and finish
    cfg.printConfig(withDetails = True, summariseProps = True, printDefaults = True)

    
    sc = cfg.run(maxEvents=1000)

    # Success should be 0
    sys.exit(not sc.isSuccess())  
