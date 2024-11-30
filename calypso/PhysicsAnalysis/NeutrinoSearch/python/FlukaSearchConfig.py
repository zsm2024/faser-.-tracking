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
    thistSvc.Output += ["HIST2 DATAFILE='FlukaSearch.root' OPT='RECREATE'"]
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
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00011-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00003-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00001-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00015-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00010-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00014-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00009-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00000-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00008-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00012-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00002-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00004-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00013-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00007-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00005-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210002/rec/r0009/./FaserMC-MDC_Fluka_unit30_Pm_71m_m3750_v3-210002-00006-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00017-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00020-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00010-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00012-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00001-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00018-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00004-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00019-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00007-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00003-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00008-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00005-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00022-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00011-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00006-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00013-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00021-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00015-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00026-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00027-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00014-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00024-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00000-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00009-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00025-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00016-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00023-s0007-r0009-xAOD.root',
        '/run/media/dcasper/Data/faser/fluka/210001/rec/r0009/./FaserMC-MDC_Fluka_unit30_Nm_71m_m3750_v3-210001-00002-s0007-r0009-xAOD.root'
    ]
    # Update this for samples with new field map
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
    acc.merge(NeutrinoSearchAlgCfg(configFlags, UseFlukaWeights=True))

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
