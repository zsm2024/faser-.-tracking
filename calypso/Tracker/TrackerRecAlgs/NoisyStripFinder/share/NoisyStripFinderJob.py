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
parser.add_argument("--outfile", "-o", default="NoisyStripFinderHist.root", help="Specify output file")

parser.add_argument("--verbose", "-v", action="store_true", help="Debug output")
args = parser.parse_args()

if args.verbose:
    log.setLevel(DEBUG)
else:
    log.setLevel(INFO)

Configurable.configurableRun3Behavior = True

#
filelist = []
for filename in args.file:
    if filename[:22] == "/eos/experiment/faser/":
        filelist.append(f"root://eospublic.cern.ch/{filename}")
    else:
        filelist.append(filename)

configFlags = initConfigFlags()
configFlags.Input.Files = args.file
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04"
#configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.IOVDb.DatabaseInstance = "CONDBR3"
configFlags.Input.ProjectName = "data22"
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
acc.merge(NoisyStripFinderCfg(configFlags, OutputHistRootName=args.outfile))

# Hack to avoid problem with our use of MC databases when isMC = False
replicaSvc = acc.getService("DBReplicaSvc")
replicaSvc.COOLSQLiteVetoPattern = ""
replicaSvc.UseCOOLSQLite = True
replicaSvc.UseCOOLFrontier = False
replicaSvc.UseGeomSQLite = True

# Don't print out every event
from AthenaConfiguration.ComponentFactory import CompFactory
eventLoop = CompFactory.AthenaEventLoopMgr()
eventLoop.EventPrintoutInterval = 1000
acc.addService(eventLoop)

if args.verbose:
    acc.foreach_component("*").OutputLevel = VERBOSE
    acc.printConfig()
else:
    acc.foreach_component("*").OutputLevel = INFO

sc = acc.run(maxEvents=args.nevents)
print(f"Job finished with {sc.isSuccess()} => {not sc.isSuccess()}")
sys.exit(not sc.isSuccess())
