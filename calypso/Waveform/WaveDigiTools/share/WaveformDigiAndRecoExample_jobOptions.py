#!/usr/bin/env python

import sys

if __name__ == "__main__":

    fileroot = "output"
    if len(sys.argv) > 1:
       filename = sys.argv[1]

    doRDO = False
    if len(sys.argv) > 2:
       filename = sys.argv[2]

    from AthenaCommon.Logging import log, logging
    from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaCommon.Configurable import Configurable
        
    Configurable.configurableRun3Behavior = True

    log.setLevel(VERBOSE)
    
    configFlags = initConfigFlags()
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersion
    configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
    configFlags.Input.ProjectName = "mc21"                       # Needed to bypass autoconfig
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
    configFlags.GeoModel.FaserVersion     = "FASER-TB00"         # FASER geometry
    configFlags.Common.isOnline = False
    #configFlags.GeoModel.Align.Dynamic = False
    
    configFlags.Input.Files = [
        "/eos/project-f/faser-commissioning/Simulation/Test/TB.Elec.200GeV.SIM.root"
        ]

    if doRDO:
        configFlags.Output.RDOFileName = f"{fileroot}.RDO.root"
    else:
        configFlags.addFlag("Output.xAODFileName", f"{fileroot}.xAOD.root")
        configFlags.Output.ESDFileName = f"{fileroot}.ESD.root"

    configFlags.lock()

    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(configFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg

    acc.merge(PoolReadCfg(configFlags))
    acc.merge(PoolWriteCfg(configFlags))

    if doRDO:
         from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
         itemList = [
             "RawWaveformContainer#*"
             ]
         acc.merge(OutputStreamCfg(configFlags, "RDO", itemList,disableEventTag=True))


    else:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
        itemList = [
            "xAOD::EventInfo#*",
            "xAOD::WaveformHitContainer#*",
            "xAOD::WaveformHitAuxContainer#*",
            ]

        acc.merge(OutputStreamCfg(configFlags, "xAOD", itemList, disableEventTag=True))

    from ScintDigiAlgs.ScintDigiAlgsConfig import ScintWaveformDigitizationCfg
    acc.merge(ScintWaveformDigitizationCfg(configFlags))

    from CaloDigiAlgs.CaloDigiAlgsConfig import CaloWaveformDigitizationCfg
    acc.merge(CaloWaveformDigitizationCfg(configFlags))

    if not doRDO:
        from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionCfg
        acc.merge(WaveformReconstructionCfg(configFlags))

    #acc.foreach_component("*").OutputLevel = VERBOSE

    # Execute and finish
    sc = acc.run(maxEvents=100)

    # Success should be 0
    sys.exit(not sc.isSuccess())
