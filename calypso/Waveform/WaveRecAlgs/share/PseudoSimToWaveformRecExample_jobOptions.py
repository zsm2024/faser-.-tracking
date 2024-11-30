#!/usr/bin/env python

import sys

if __name__ == "__main__":

    fileroot = "SimToRec"
    naive = True

    from AthenaCommon.Logging import log, logging
    from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaCommon.Configurable import Configurable
        
    Configurable.configurableRun3Behavior = True

    log.setLevel(INFO)
    
    configFlags = initConfigFlags()
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-TB00"             # Always needed; must match FaserVersion
    configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
    configFlags.Input.ProjectName = "mc21"                       # Needed to bypass autoconfig
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
    configFlags.GeoModel.FaserVersion     = "FASER-TB00"         # FASER geometry
    configFlags.Common.isOnline = False
    #configFlags.GeoModel.Align.Dynamic = False
    
    configFlags.Input.Files = [
        "my.HITS.pool.root"
        #"/bundle/data/FASER/LC_output/BatchOutput/TestBeam/TB.Elec.8.r5.e100.SIM.root"
        ]

    
    configFlags.addFlag("Output.xAODFileName", f"{fileroot}.xAOD.root")
    configFlags.Output.ESDFileName = f"{fileroot}.ESD.root"

    configFlags.lock()

    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg

    acc = MainServicesCfg(configFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg

    acc.merge(PoolReadCfg(configFlags))
    acc.merge(PoolWriteCfg(configFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    itemList = [
        "xAOD::EventInfo#*",
        "xAOD::WaveformHitContainer#*",
        "xAOD::WaveformHitAuxContainer#*",
        ]
    
    acc.merge(OutputStreamCfg(configFlags, "xAOD", itemList, disableEventTag=True))

    from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionCfg
    acc.merge(WaveformReconstructionCfg(configFlags, naive))

    #acc.foreach_component("*").OutputLevel = VERBOSE

    # Execute and finish
    sc = acc.run(maxEvents=1000)

    # Success should be 0
    sys.exit(not sc.isSuccess())
