#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import INFO, WARNING

Cmake_working_dir = sys.argv[1]
print("Cmake_working_dir = ", Cmake_working_dir)

def TrackerDataAccessExampleCfg(flags, name="TrackerDataAccessExampleAlg", **kwargs):

    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    a = FaserGeometryCfg(flags)

    TrackerDataAccessExampleAlg = CompFactory.TrackerDataAccess
    a.addEventAlgo(TrackerDataAccessExampleAlg(name, **kwargs))
    
    from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
    a.merge(FaserByteStreamCnvSvcCfg(flags))

    # thistSvc = CompFactory.THistSvc()
    # thistSvc.Output += ["HIST DATAFILE='rdoReadHist.root' OPT='RECREATE'"]
    # a.addService(thistSvc)
    
    return a


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaCommon.Logging import logging

    Configurable.configurableRun3Behavior = True
    
# Flags for this job
    configFlags = initConfigFlags()
    configFlags.Input.isMC = False                               # Needed to bypass autoconfig
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    configFlags.IOVDb.DatabaseInstance = "CONDBR3"               # Use MC conditions for now
    configFlags.GeoModel.FaserVersion     = "FASER-01"           # FASER geometry
    configFlags.Input.ProjectName = "data20"                     # Needed to bypass autoconfig
    #configFlags.GeoModel.Align.Dynamic    = False
    configFlags.Input.Files = [f"{Cmake_working_dir}/../rawdata/Faser-Physics-001920-filtered.raw"] #path is set to test data for ctest
    # configFlags.Output.RDOFileName = "my.RDO.pool.root"
    configFlags.lock()
# Configure components
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    # from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    
    acc = MainServicesCfg(configFlags)
    # acc.merge(PoolWriteCfg(configFlags))

# Configure output
    # from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    # itemList = [ "xAOD::EventInfo#*",
    #              "xAOD::EventAuxInfo#*",
    #              "FaserSCT_RDO_Container#*"
    #            ]
    # acc.merge(OutputStreamCfg(configFlags, "RDO", itemList))
    # ostream = acc.getEventAlgo("OutputStreamRDO")

# Set up algorithm
    acc.merge(TrackerDataAccessExampleCfg(configFlags))

# Hack to avoid problem with our use of MC databases when isMC = False
    replicaSvc = acc.getService("DBReplicaSvc")
    replicaSvc.COOLSQLiteVetoPattern = ""
    replicaSvc.UseCOOLSQLite = True
    replicaSvc.UseCOOLFrontier = False
    replicaSvc.UseGeomSQLite = True

# Configure verbosity    
    # configFlags.dump()
    logging.getLogger('forcomps').setLevel(WARNING)
    acc.foreach_component("*").OutputLevel = INFO
    acc.foreach_component("*ClassID*").OutputLevel = INFO
    acc.foreach_component("*TrackerDataAccess*").OutputLevel = WARNING
    # log.setLevel(VERBOSE)

# Execute and finish
    sys.exit(int(acc.run(maxEvents=-1).isFailure()))
