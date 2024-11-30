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

    runno=int(sys.argv[1])
    num=int(sys.argv[2])
    filesPerJob=int(sys.argv[3])
    ptag=str(sys.argv[4])   # "p0008"

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

    dataDir=f"/eos/experiment/faser/rec/2022/{ptag}/{runno:06d}"

    files=sorted(glob.glob(f"{dataDir}/Faser*"))
    if filesPerJob <= 0: filesPerJob = len(files)

    start = num*filesPerJob
    end = (num+1)*filesPerJob
    if end > len(files):
        end = len(files)
    fileListInitial=files[start:end]
    fileList=[]
    for fName in fileListInitial:
        #if fName[:4] == '/eos':
        #    fName = f'root:/{fName}'

        try:
            fh=ROOT.TFile(fName)
            fileList.append(fName)
        except OSError:
            print("Warning bad file: ",fName)

    log.info(f"Analyzing Run {runno} files {start} to {end} (num={num})")
    log.info(f"Got {len(fileList)} files out of {len(fileListInitial)}")

    if start == 0 and end == len(files):
        outName=f"Faser-Physics-{runno:06d}-{ptag}-PHYS.root"
    else:
        outName=f"Faser-Physics-{runno:06d}-{start:05d}-{(end-1):05d}-{ptag}-PHYS.root"

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
