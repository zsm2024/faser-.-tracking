#!/usr/bin/env python3

"""
    Copyright (C) 2002-2022 CERN for the benefit of the FASER collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Move NtupleDumperAlgCfg to python/NtupleDumperConfig
from NtupleDumper.NtupleDumperConfig import NtupleDumperAlgCfg

if __name__ == "__main__":

    import glob
    import sys
    import ROOT


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

    fileList=["/afs/cern.ch/work/s/sazhang/public/FASER/alma9_test/run/FaserMC-MC22_FS_Aee_050MeV_1Em6-110023-00010-00019-s0008-xAOD.root"]
    #fileList=["/afs/cern.ch/work/x/xai/public/FASER/develop/run/WithoutDuplicateFix/FaserMC-MC22_PG_mupl_fasernu_5mrad_flukaE-100044-00000-00009-s0010-xAOD.root"]
    outName=f"FaserMC-MC22_FS_Aee_050MeV_1Em6-110023-00010-00019-s0008-PHYS.root"

    # Configure
    configFlags = initConfigFlags()
    configFlags.Input.Files = fileList
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersionS
    configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
    configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig
    configFlags.Input.isMC = False                               # Needed to bypass autoconfig
    configFlags.GeoModel.FaserVersion   = "FASERNU-03"           # FASER geometry
    configFlags.Common.isOnline = False
    #configFlags.GeoModel.Align.Dynamic = False
    configFlags.Beam.NumberOfCollisions = 0.
    configFlags.Detector.GeometryFaserSCT = True

    configFlags.lock()

    # Core components
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))

    # algorithm
    acc.merge(NtupleDumperAlgCfg(configFlags, outName))

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
    if False:
        logging.getLogger('forcomps').setLevel(VERBOSE)
        acc.foreach_component("*").OutputLevel = VERBOSE
        acc.foreach_component("*ClassID*").OutputLevel = INFO
        acc.getCondAlgo("FaserSCT_AlignCondAlg").OutputLevel = VERBOSE
        acc.getCondAlgo("FaserSCT_DetectorElementCondAlg").OutputLevel = VERBOSE
        acc.getService("StoreGateSvc").Dump = True
        acc.getService("ConditionStore").Dump = True
        acc.printConfig(withDetails=True)
        configFlags.dump()

    # Execute and finish
    sc = acc.run(maxEvents=-1)

    # Success should be 0
    sys.exit(not sc.isSuccess())    
