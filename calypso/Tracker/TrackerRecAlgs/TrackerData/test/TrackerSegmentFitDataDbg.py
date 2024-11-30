#!/usr/bin/env python
""""
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from TrackerData.TrackerSegmentFitDataConfig import TrackerSegmentFitDataCfg

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = ["my.RDO.pool.root"]
configFlags.Output.ESDFileName = "TrackerSegmentFitData.ESD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data22"
configFlags.Input.isMC = True
configFlags.GeoModel.FaserVersion = "FASER-01"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.addFlag("Output.xAODFileName", f"TrackerSegmentFitData_xAOD.root")
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))
acc.merge(PoolWriteCfg(configFlags))

acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="SCT_RDOs"))
acc.merge(SegmentFitAlgCfg(configFlags, SharedHitFraction=0.5, MinClustersPerFit=5, TanThetaCut=0.25))
acc.merge(TrackerSegmentFitDataCfg(configFlags))
acc.getEventAlgo("Tracker::TrackerSegmentFitDataAlg").OutputLevel = DEBUG
itemList = ["xAOD::EventInfo#*",
            "xAOD::EventAuxInfo#*",
            "xAOD::FaserTriggerData#*",
            "xAOD::FaserTriggerDataAux#*",
            "FaserSCT_RDO_Container#*",
            "Tracker::FaserSCT_ClusterContainer#*",
            "TrackCollection#*"
            ]
acc.merge(OutputStreamCfg(configFlags, "ESD", itemList))

sc = acc.run(maxEvents=-1)
sys.exit(not sc.isSuccess())
