"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def NeutrinoSearchAlgCfg(flags, **kwargs):
    # Initialize GeoModel
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    acc = FaserGeometryCfg(flags)

    acc.merge(MagneticFieldSvcCfg(flags))
    # acc.merge(FaserActsTrackingGeometrySvcCfg(flags))
    # acc.merge(FaserActsAlignmentCondAlgCfg(flags))

    actsExtrapolationTool = CompFactory.FaserActsExtrapolationTool("FaserActsExtrapolationTool")
    actsExtrapolationTool.MaxSteps = 1000
    actsExtrapolationTool.TrackingGeometryTool = CompFactory.FaserActsTrackingGeometryTool("TrackingGeometryTool")

    NeutrinoSearchAlg = CompFactory.NeutrinoSearchAlg("NeutrinoSearchAlg",**kwargs)
    NeutrinoSearchAlg.ExtrapolationTool = actsExtrapolationTool
    acc.addEventAlgo(NeutrinoSearchAlg)

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST2 DATAFILE='GenieSearch.root' OPT='RECREATE'"]
    acc.addService(thistSvc)

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
    configFlags.Input.Files = [
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00040-00047-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00056-00063-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00120-00127-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00112-00119-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00000-00007-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00208-00210-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00200-00207-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00064-00071-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00080-00087-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00024-00031-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00072-00079-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00008-00015-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00176-00183-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00096-00103-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00104-00111-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00032-00039-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00048-00055-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00192-00199-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00088-00095-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00016-00023-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/genie/r0009/rec/./FaserMC-MDC_Genie_all_600fbInv_v1-200003-00168-00175-s0007-r0009-xAOD.root'
    ]
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-02"             # Always needed; must match FaserVersionS
    configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
    configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
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
    acc.merge(NeutrinoSearchAlgCfg(configFlags, UseGenieWeights=True))

    # # Hack to avoid problem with our use of MC databases when isMC = False
    # replicaSvc = acc.getService("DBReplicaSvc")
    # replicaSvc.COOLSQLiteVetoPattern = ""
    # replicaSvc.UseCOOLSQLite = True
    # replicaSvc.UseCOOLFrontier = False
    # replicaSvc.UseGeomSQLite = True

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
