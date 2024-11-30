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
from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from TrackerData.TrackerSegmentFitDataConfig import TrackerSegmentFitDataCfg

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = ['/home/tboeckh/tmp/Faser-Physics-006470-00093.raw_middleStation.SPs']
configFlags.Output.ESDFileName = "TrackerSegmentFitData.ESD.pool.root"
configFlags.addFlag("Output.xAODFileName", f"TrackerSegmentFitData_xAOD.root")
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data21"
configFlags.Input.isMC = False
configFlags.GeoModel.FaserVersion = "FASER-02"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(FaserByteStreamCnvSvcCfg(configFlags))
acc.merge(FaserSCT_ClusterizationCfg(configFlags, name="LevelClustering", DataObjectName="SCT_LEVELMODE_RDOs", ClusterToolTimingPattern="X1X"))
acc.merge(SegmentFitAlgCfg(configFlags, name=f"LevelFit", MaxClusters=44))
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
acc.merge(OutputStreamCfg(configFlags, "xAOD", itemList))

replicaSvc = acc.getService("DBReplicaSvc")
replicaSvc.COOLSQLiteVetoPattern = ""
replicaSvc.UseCOOLSQLite = True
replicaSvc.UseCOOLFrontier = False
replicaSvc.UseGeomSQLite = True

sc = acc.run(maxEvents=-1)
sys.exit(not sc.isSuccess())
