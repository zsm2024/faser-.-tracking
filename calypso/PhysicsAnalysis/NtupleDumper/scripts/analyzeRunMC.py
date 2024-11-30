#!/usr/bin/env python3

"""
    Copyright (C) 2002-2022 CERN for the benefit of the FASER collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg


def NtupleDumperAlgCfg(flags, OutName, **kwargs):
    # Initialize GeoModel
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    acc = FaserGeometryCfg(flags)

    acc.merge(MagneticFieldSvcCfg(flags))
    # acc.merge(FaserActsTrackingGeometrySvcCfg(flags))
    # acc.merge(FaserActsAlignmentCondAlgCfg(flags))

    actsExtrapolationTool = CompFactory.FaserActsExtrapolationTool("FaserActsExtrapolationTool")
    actsExtrapolationTool.MaxSteps = 10000
    actsExtrapolationTool.TrackingGeometryTool = CompFactory.FaserActsTrackingGeometryTool("TrackingGeometryTool")

    NtupleDumperAlg = CompFactory.NtupleDumperAlg("NtupleDumperAlg",**kwargs)
    NtupleDumperAlg.ExtrapolationTool = actsExtrapolationTool
    acc.addEventAlgo(NtupleDumperAlg)

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += [f"HIST2 DATAFILE='{OutName}' OPT='RECREATE'"]
    acc.addService(thistSvc)

    return acc

if __name__ == "__main__":

    import glob
    import sys
    import ROOT

    runno=int(sys.argv[1])
    num=int(sys.argv[2])
    filesPerJob=int(sys.argv[3])

    ptag="p0008"

    from AthenaCommon.Logging import log, logging
    from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    # from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    # Set up logging and new style config
    log.setLevel(INFO)
    Configurable.configurableRun3Behavior = True

    #fName = dataDir+"FaserMC-MDC_FS_Aee_100MeV_1Em5_shift-110001-00000-00009-s0007-r0009-xAOD.root"


    dataDir=f"/eos/experiment/faser/sim/mdc/foresee/{ptag}/rec/r0009/"
    files=sorted(glob.glob(f"{dataDir}/FaserMC-MDC*"))
    fileListInitial=files[num*filesPerJob:(num+1)*filesPerJob]
    fileList=[]
    for fName in fileListInitial:
        try:
            fh=ROOT.TFile(fName)
            fileList.append(fName)
        except OSError:
            print("Warning bad file: ",fName)

    log.info(f"Analyzing Run {runno} files {num*filesPerJob} to {(num+1)*filesPerJob} (num={num})")
    log.info(f"Got {len(fileList)} files out of {len(fileListInitial)}")

    outName=f"MC-tuple-{runno:06d}-{num:05d}-{filesPerJob}.root"

    # Configure
    configFlags = initConfigFlags()
    configFlags.Input.Files = fileList
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersionS
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
    acc.merge(NtupleDumperAlgCfg(configFlags, outName, UseFlukaWeights=True, CaloConfig=run_config))

    AthenaEventLoopMgr = CompFactory.AthenaEventLoopMgr()
    AthenaEventLoopMgr.EventPrintoutInterval=1000
    acc.addService(AthenaEventLoopMgr)

    # # Hack to avoid problem with our use of MC databases when isMC = False
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
