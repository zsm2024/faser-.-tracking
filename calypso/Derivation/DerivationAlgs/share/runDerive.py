
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg


def DerivationAlgCfg(flags, name, frac, **kwargs):

    acc = ComponentAccumulator()

    tool = CompFactory.ExampleDerivationTool(name + "_TestTool", SaveFraction = frac)
    print ("ZEBRA", tool.SaveFraction)

    kwargs.setdefault("Tools", [tool])
    acc.addEventAlgo(CompFactory.Derive(name, **kwargs))

    return acc

def DerivationAlgCfg2(flags, name, **kwargs):

    acc = ComponentAccumulator()

    tool = CompFactory.TriggerStreamTool(name + "_TriggerSteamTool")
    kwargs.setdefault("Tools", [tool])
    acc.addEventAlgo(CompFactory.Derive(name, **kwargs))

    return acc


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

    # Set up logging and new style config
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = True

    configFlags = initConfigFlags()
    configFlags.Input.Files = [
        "/eos/experiment/faser/rec/2022/p0008/007984/Faser-Physics-007984-00000-p0008-xAOD.root"
        #"/bundle/data/FASER/Ti12data/filter/r0008/007983/Faser-Physics-007983-TrigMask08-r0008-xAOD.root"
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

    configFlags.addFlag("Output.AODSTREAM1FileName", "my.STREAM1.xAOD.root")
    configFlags.addFlag("Output.AODSTREAM2FileName", "my.STREAM2.xAOD.root")
    configFlags.addFlag("Output.AODSTREAM3FileName", "my.STREAM3.xAOD.root")        
    #configFlags.Output.STREAM1FileName = fileName

    configFlags.lock()

    # Core components
    cfg = MainServicesCfg(configFlags)
    cfg.merge(PoolReadCfg(configFlags))
    cfg.merge(PoolWriteCfg(configFlags))

    # Derivation alg
    cfg.merge(DerivationAlgCfg(configFlags, "DerivationAlg1", 10))
    cfg.merge(DerivationAlgCfg(configFlags, "DerivationAlg2", 90))
    cfg.merge(DerivationAlgCfg2(configFlags, "DerivationAlg3"))        

    # Writing
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    streamName1 = "AODSTREAM1"  # Needs to have AOD in name
    itemList1 = [ "xAOD::EventInfo#*"
             , "xAOD::EventAuxInfo#*"
             , "xAOD::FaserTriggerData#*"
             , "xAOD::FaserTriggerDataAux#*"
             , "FaserSiHitCollection#*"  # Strip hits, do we want this?
             , "FaserSCT_RDO_Container#*" 
             , "xAOD::WaveformHitContainer#*"
             , "xAOD::WaveformHitAuxContainer#*"
             , "xAOD::WaveformClock#*"
             , "xAOD::WaveformClockAuxInfo#*"
#             , "FaserSCT_SpacePointContainer#*"  # Crashes
#              , "Tracker::FaserSCT_ClusterContainer#*"
#              , "TrackCollection#*"
                  ]
    
    cfg.merge(OutputStreamCfg(configFlags, streamName1, itemList1)) #, disableEventTag = True))
    cfg.getEventAlgo("OutputStreamAODSTREAM1").AcceptAlgs = ["DerivationAlg1"]
    #cfg.getEventAlgo("OutputStreamAODSTREAM1").TakeItemsFromInput = True


    # Writing
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    streamName2 = "AODSTREAM2"  # Needs to have AOD in name
    itemList2 = [ "xAOD::EventInfo#*"
             , "xAOD::EventAuxInfo#*"
             , "xAOD::FaserTriggerData#*"
             , "xAOD::FaserTriggerDataAux#*"
                  ]
    cfg.merge(OutputStreamCfg(configFlags, streamName2, itemList2)) #, disableEventTag = True))
    cfg.getEventAlgo("OutputStreamAODSTREAM2").AcceptAlgs = ["DerivationAlg2"]

    # Writing
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    streamName3 = "AODSTREAM3"  # Needs to have AOD in name
    itemList3 = [ "xAOD::EventInfo#*"
             , "xAOD::EventAuxInfo#*"
             , "xAOD::FaserTriggerData#*"
             , "xAOD::FaserTriggerDataAux#*"
                  ]
    cfg.merge(OutputStreamCfg(configFlags, streamName3, itemList3)) #, disableEventTag = True))
    cfg.getEventAlgo("OutputStreamAODSTREAM3").AcceptAlgs = ["DerivationAlg3"]


    
#     from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#     streamName = "STREAM1"
#     fileName = "streaming.STREAM1.root"
#     testStream = MSMgr.NewPoolRootStream(streamName, fileName)
#     testStream.AcceptAlgs(["DerivationAlg1"])
#     cfg.addEventAlgo(testStream)

#     # Hack to avoid problem with our use of MC databases when isMC = False
#     replicaSvc = acc.getService("DBReplicaSvc")
#     replicaSvc.COOLSQLiteVetoPattern = ""
#     replicaSvc.UseCOOLSQLite = True
#     replicaSvc.UseCOOLFrontier = False
#     replicaSvc.UseGeomSQLite = True

    # Execute and finish
    cfg.printConfig()

    
    sc = cfg.run(maxEvents=1000)

    # Success should be 0
    sys.exit(not sc.isSuccess())  
