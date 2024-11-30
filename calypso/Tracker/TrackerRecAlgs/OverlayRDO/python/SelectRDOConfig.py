"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

chargePrefix = "Pos_"

def SelectRDOAlgCfg(flags, **kwargs):

    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    acc = FaserGeometryCfg(flags)
    SelectRDOAlg = CompFactory.SelectRDOAlg("SelectRDOAlg",**kwargs)
    SelectRDOAlg.AcceptPositive = ("Pos_" in chargePrefix)
    acc.addEventAlgo(SelectRDOAlg)
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin0RDO",Xmin=-66, Xmax=0,  Ymin=-66, Ymax=0))
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin1RDO",Xmin=0,   Xmax=66, Ymin=-66, Ymax=0))
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin2RDO",Xmin=-66, Xmax=0,  Ymin=0,   Ymax=66))
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin3RDO",Xmin=0,   Xmax=66, Ymin=0,   Ymax=66))

    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin4RDO",Xmin=-33, Xmax=33,  Ymin=-33, Ymax=33))
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin5RDO",Xmin=-33, Xmax=33,  Ymin=33,  Ymax=99))
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin6RDO",Xmin=-33, Xmax=33,  Ymin=-99, Ymax=-33))
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin7RDO",Xmin=-99, Xmax=-33, Ymin=-33, Ymax=33))
    acc.addEventAlgo(CompFactory.BinRDOAlg("Bin8RDO",Xmin=33,  Xmax=99,  Ymin=-33, Ymax=33))

    ItemList = []
    # ItemList += ["xAOD::EventInfo#" + chargePrefix + "EventInfo"]
    # ItemList += ["xAOD::EventAuxInfo#" + chargePrefix + "EventInfoAux."]
    ItemList += ["xAOD::EventInfo#EventInfo"]
    ItemList += ["xAOD::EventAuxInfo#EventInfoAux."]
    ItemList += ["TrackCollection#" + chargePrefix + "CKFTrackCollectionWithoutIFT"]
    ItemList += ["FaserSCT_RDO_Container#" + chargePrefix + "SCT_RDOs"]
    ItemList += ["FaserSCT_RDO_Container#" + chargePrefix + "SCT_EDGEMODE_RDOs"]
    # ItemList += ["Tracker::FaserSCT_ClusterContainer#" + chargePrefix + "SCT_ClusterContainer"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin0RDO", ItemList=ItemList, disableEventTag=True))
    osrdo0 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin0RDO")
    osrdo0.RequireAlgs += ["SelectRDOAlg","Bin0RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin1RDO", ItemList=ItemList, disableEventTag=True))
    osrdo1 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin1RDO")
    osrdo1.RequireAlgs += ["SelectRDOAlg","Bin1RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin2RDO", ItemList=ItemList, disableEventTag=True))
    osrdo2 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin2RDO")
    osrdo2.RequireAlgs += ["SelectRDOAlg","Bin2RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin3RDO", ItemList=ItemList, disableEventTag=True))
    osrdo3 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin3RDO")
    osrdo3.RequireAlgs += ["SelectRDOAlg","Bin3RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin4RDO", ItemList=ItemList, disableEventTag=True))
    osrdo4 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin4RDO")
    osrdo4.RequireAlgs += ["SelectRDOAlg","Bin4RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin5RDO", ItemList=ItemList, disableEventTag=True))
    osrdo5 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin5RDO")
    osrdo5.RequireAlgs += ["SelectRDOAlg","Bin5RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin6RDO", ItemList=ItemList, disableEventTag=True))
    osrdo6 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin6RDO")
    osrdo6.RequireAlgs += ["SelectRDOAlg","Bin6RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin7RDO", ItemList=ItemList, disableEventTag=True))
    osrdo7 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin7RDO")
    osrdo7.RequireAlgs += ["SelectRDOAlg","Bin7RDO"]

    acc.merge(OutputStreamCfg(flags,chargePrefix+"Bin8RDO", ItemList=ItemList, disableEventTag=True))
    osrdo8 = acc.getEventAlgo("OutputStream" + chargePrefix + "Bin8RDO")
    osrdo8.RequireAlgs += ["SelectRDOAlg","Bin8RDO"]

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

    # Set up logging and new style config
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = True

    # Configure
    configFlags = initConfigFlags()
    configFlags.Input.Files = [ #'/eos/experiment/faser/rec/2022/r0013/009171/Faser-Physics-009171-00006-r0013-xAOD.root' 
                                '/eos/experiment/faser/rec/2022/r0013/009166/Faser-Physics-009166-00485-r0013-xAOD.root'
                              ]
    # configFlags.Input.Files = [ 'Faser-Physics-009171-00006-r0013-xAOD.root', 
    #                             'Faser-Physics-009166-00485-r0013-xAOD.root']
    # configFlags.Output.RDOFileName = chargePrefix + "RDO.pool.root"
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersionS
    configFlags.IOVDb.DatabaseInstance = "CONDBR3"               # Use data conditions for now
    configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig
    configFlags.Input.isMC = False                                # Needed to bypass autoconfig
    configFlags.GeoModel.FaserVersion     = "FASERNU-03"           # FASER geometry
    configFlags.Common.isOnline = False
    #configFlags.GeoModel.Align.Dynamic = False
    configFlags.Beam.NumberOfCollisions = 0.
    configFlags.Detector.GeometryFaserSCT = True

    configFlags.lock()

    # Core components
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))

    # algorithm
    acc.merge(SelectRDOAlgCfg(configFlags, TrackCollection = "CKFTrackCollectionWithoutIFT", OutputTrackCollection = chargePrefix + "CKFTrackCollectionWithoutIFT"))

    from SGComps.AddressRemappingConfig import AddressRemappingCfg
    acc.merge(AddressRemappingCfg([
        # "xAOD::EventInfo#EventInfo->" + chargePrefix + "EventInfo",
        # "xAOD::EventAuxInfo#EventInfoAux.->" + chargePrefix + "EventInfoAux.",
        # "TrackCollection#CKFTrackCollectionWithoutIFT->" + chargePrefix + "CKFTrackCollectionWithoutIFT",
        "FaserSCT_RDO_Container#SCT_RDOs->" + chargePrefix + "SCT_RDOs",
        "FaserSCT_RDO_Container#SCT_EDGEMODE_RDOs->" + chargePrefix + "SCT_EDGEMODE_RDOs",
        # "Tracker::FaserSCT_ClusterContainer#SCT_ClusterContainer->" + chargePrefix + "SCT_ClusterContainer"
    ]))

    # Hack to avoid problem with our use of MC databases when isMC = False
    replicaSvc = acc.getService("DBReplicaSvc")
    replicaSvc.COOLSQLiteVetoPattern = ""
    replicaSvc.UseCOOLSQLite = True
    replicaSvc.UseCOOLFrontier = False
    replicaSvc.UseGeomSQLite = True

    # Timing
    #acc.merge(MergeRecoTimingObjCfg(configFlags))

    # Dump config
    # logging.getLogger('forcomps').setLevel(VERBOSE)
    # acc.foreach_component("*").OutputLevel = VERBOSE
    # acc.foreach_component("*ClassID*").OutputLevel = INFO
    # acc.getCondAlgo("FaserSCT_AlignCondAlg").OutputLevel = VERBOSE
    # acc.getCondAlgo("FaserSCT_DetectorElementCondAlg").OutputLevel = VERBOSE
    # acc.getService("StoreGateSvc").Dump = True
    # acc.getService("ConditionStore").Dump = True
    # acc.printConfig(withDetails=True)
    # configFlags.dump()

    # Execute and finish
    sc = acc.run(maxEvents=-1)

    # Success should be 0
    sys.exit(not sc.isSuccess())    
