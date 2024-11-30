"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

def OverlayRDOAlgCfg(flags, **kwargs):

    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    acc = FaserGeometryCfg(flags)
    OverlayRDOAlg = CompFactory.OverlayRDOAlg("OverlayRDOAlg",**kwargs)
    acc.addEventAlgo(OverlayRDOAlg)

    ItemList = []
    # ItemList += ["xAOD::EventInfo#" + chargePrefix + "EventInfo"]
    # ItemList += ["xAOD::EventAuxInfo#" + chargePrefix + "EventInfoAux."]
    ItemList += ["xAOD::EventInfo#EventInfo"]
    ItemList += ["xAOD::EventAuxInfo#EventInfoAux."]
    ItemList += ["TrackCollection#Orig_CKFTrackCollectionWithoutIFT"]
    ItemList += ["FaserSCT_RDO_Container#SCT_RDOs"]
    ItemList += ["FaserSCT_RDO_Container#SCT_EDGEMODE_RDOs"]
    # ItemList += ["Tracker::FaserSCT_ClusterContainer#" + chargePrefix + "SCT_ClusterContainer"]

    acc.merge(OutputStreamCfg(flags,"RDO", ItemList=ItemList, disableEventTag=True))



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
    # from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

    # Set up logging and new style config
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = True

    # Configure
    configFlags = initConfigFlags()
    configFlags.Overlay.DataOverlay = False
    configFlags.Input.Files = [ 'Pos_RDO.pool.root']
    configFlags.Input.SecondaryFiles = [ 'Neg_RDO.pool.root' ]
    # configFlags.Input.Files = [ '/eos/experiment/faser/rec/2022/r0013/009171/Faser-Physics-009171-00006-r0013-xAOD.root']
    # configFlags.Input.SecondaryFiles = [ '/eos/experiment/faser/rec/2022/r0013/009166/Faser-Physics-009166-00485-r0013-xAOD.root' ]
    configFlags.Output.RDOFileName = "Overlay.RDO.pool.root"
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
    acc.merge(OverlayRDOAlgCfg(configFlags))

    # from SGComps.AddressRemappingConfig import AddressRemappingCfg
    # acc.merge(AddressRemappingCfg([
    #     "xAOD::EventInfo#EventInfo->" + configFlags.Overlay.SigPrefix + "EventInfo",
    #     "xAOD::EventAuxInfo#EventInfoAux.->" + configFlags.Overlay.SigPrefix + "EventInfoAux.",
    # ]))

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
