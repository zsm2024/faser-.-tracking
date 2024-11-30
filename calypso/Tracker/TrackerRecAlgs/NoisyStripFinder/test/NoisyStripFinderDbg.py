#!/usr/bin/env python
"""
    Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from NoisyStripFinder.NoisyStripFinderConfig import NoisyStripFinderCfg
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("file", nargs="+", help="full path to input file")
parser.add_argument("--nevents", "-n", default=-1, type=int, help="Number of events to process")
args = parser.parse_args()

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = args.file
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data21"
configFlags.Input.isMC = False
configFlags.GeoModel.FaserVersion = "FASERNU-03"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(PoolWriteCfg(configFlags))
acc.merge(FaserByteStreamCnvSvcCfg(configFlags, OccupancyCut=-1))
acc.merge(FaserSCT_ClusterizationCfg(
    configFlags,
    DataObjectName="SCT_RDOs",
    ClusterToolTimingPattern="XXX"))
acc.merge(NoisyStripFinderCfg(configFlags))

# Hack to avoid problem with our use of MC databases when isMC = False
replicaSvc = acc.getService("DBReplicaSvc")
replicaSvc.COOLSQLiteVetoPattern = ""
replicaSvc.UseCOOLSQLite = True
replicaSvc.UseCOOLFrontier = False
replicaSvc.UseGeomSQLite = True

sc = acc.run(maxEvents=args.nevents)
sys.exit(not sc.isSuccess())
