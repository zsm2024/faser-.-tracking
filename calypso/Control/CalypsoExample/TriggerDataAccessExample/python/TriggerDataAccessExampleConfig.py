#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import VERBOSE, INFO

Cmake_working_dir = sys.argv[1]
print("Cmake_working_dir = ", Cmake_working_dir)

def TriggerDataAccessExampleCfg(flags, name="TriggerDataAccessExampleAlg", **kwargs):

    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    a = FaserGeometryCfg(flags)

    TriggerDataAccessExampleAlg = CompFactory.FaserTriggerDataAccess
    a.addEventAlgo(TriggerDataAccessExampleAlg(name, **kwargs))
    
    from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
    a.merge(FaserByteStreamCnvSvcCfg(flags))

    # thistSvc = CompFactory.THistSvc()
    # thistSvc.Output += ["HIST DATAFILE='rdoReadHist.root' OPT='RECREATE'"]
    # a.addService(thistSvc)
    
    return a


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags

    Configurable.configurableRun3Behavior = True
    
# Flags for this job
    configFlags = initConfigFlags()
    configFlags.Input.isMC = False                               # Needed to bypass autoconfig
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
    configFlags.GeoModel.FaserVersion     = "FASER-01"           # Default FASER geometry
    configFlags.Input.ProjectName = "data20"
    #configFlags.GeoModel.Align.Dynamic    = False
    configFlags.Input.Files = [f"{Cmake_working_dir}/../rawdata/Faser-Physics-001920-filtered.raw"] #path is set to test data for ctest
    configFlags.Output.RDOFileName = "trigger.RDO.pool.root"
    configFlags.lock()

# Configure components
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolWriteCfg(configFlags))

# Configure output
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    itemList = [ "xAOD::EventInfo#*",
                 "xAOD::EventAuxInfo#*",
                 "xAOD::FaserTriggerData#*",
                 "xAOD::FaserTriggerDataAux#*" ]
    acc.merge(OutputStreamCfg(configFlags, "RDO", itemList))
    ostream = acc.getEventAlgo("OutputStreamRDO")

# Set up algorithm
    acc.merge(TriggerDataAccessExampleCfg(configFlags))

# Hack to avoid problem with our use of MC databases when isMC = False
    replicaSvc = acc.getService("DBReplicaSvc")
    replicaSvc.COOLSQLiteVetoPattern = ""
    replicaSvc.UseCOOLSQLite = True
    replicaSvc.UseCOOLFrontier = False
    replicaSvc.UseGeomSQLite = True

# Configure verbosity    
    # configFlags.dump()
    # logging.getLogger('forcomps').setLevel(VERBOSE)
    acc.foreach_component("*").OutputLevel = VERBOSE
    acc.foreach_component("*ClassID*").OutputLevel = INFO
    # log.setLevel(VERBOSE)

    # acc.getService("FaserByteStreamInputSvc").DumpFlag = True
    acc.getService("FaserEventSelector").OutputLevel = VERBOSE
    acc.getService("FaserByteStreamInputSvc").OutputLevel = VERBOSE
    acc.getService("FaserByteStreamCnvSvc").OutputLevel = VERBOSE
    acc.getService("FaserByteStreamAddressProviderSvc").OutputLevel = VERBOSE
    acc.getService("MessageSvc").Format = "% F%40W%S%7W%R%T %0W%M"
# Execute and finish
    sys.exit(int(acc.run(maxEvents=-1).isFailure()))
