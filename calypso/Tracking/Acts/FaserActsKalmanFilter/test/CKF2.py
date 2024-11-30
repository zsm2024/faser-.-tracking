#!/usr/bin/env python

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from FaserActsKalmanFilter.GhostBustersConfig import GhostBustersCfg
from FaserActsKalmanFilter.CKF2Config import CKF2Cfg
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("file", nargs="+", help="full path to input file")
parser.add_argument("--nevents", "-n", default=-1, type=int, help="Number of events to process")
args = parser.parse_args()

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = args.file
configFlags.addFlag("Output.xAODFileName", f"CKF.xAOD.root")
configFlags.Output.ESDFileName = "CKF.ESD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
configFlags.GeoModel.FaserVersion = "FASERNU-03"
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.TrackingGeometry.MaterialSource = "geometry-maps.json"
configFlags.Input.isMC = True
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(FaserGeometryCfg(configFlags))
acc.merge(PoolReadCfg(configFlags))
acc.merge(PoolWriteCfg(configFlags))

acc.merge(FaserSCT_ClusterizationCfg(configFlags))
acc.merge(TrackerSpacePointFinderCfg(configFlags))
acc.merge(SegmentFitAlgCfg(configFlags, SharedHitFraction=0.61, MinClustersPerFit=5, TanThetaXZCut=0.083))
acc.merge(GhostBustersCfg(configFlags))
acc.merge(CKF2Cfg(configFlags))
# acc.getEventAlgo("CKF2").OutputLevel = DEBUG

# logging.getLogger('forcomps').setLevel(VERBOSE)
# acc.foreach_component("*").OutputLevel = VERBOSE
# acc.foreach_component("*ClassID*").OutputLevel = VERBOSE
# acc.getService("StoreGateSvc").Dump = True
# acc.getService("ConditionStore").Dump = True
# acc.printConfig(withDetails=True)
# configFlags.dump()

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
itemList = [
    "xAOD::EventInfo#*",
    "xAOD::EventAuxInfo#*",
    "xAOD::FaserTriggerData#*",
    "xAOD::FaserTriggerDataAux#*",
    "FaserSCT_RDO_Container#*",
    "Tracker::FaserSCT_ClusterContainer#*",
    "TrackCollection#*",
]
acc.merge(OutputStreamCfg(configFlags, "xAOD", itemList))

acc.getService("MessageSvc").Format = "% F%40W%S%7W%R%T %0W%M"

sc = acc.run(maxEvents=args.nevents)
sys.exit(not sc.isSuccess())
