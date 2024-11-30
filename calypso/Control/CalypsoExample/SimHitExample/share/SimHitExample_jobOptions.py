
# Note: this job configuration method is obsolete
# Please see python/SimHitAlgConfig.py for the correct method to use

from AthenaCommon.GlobalFlags import globalflags

globalflags.InputFormat.set_Value_and_Lock('pool')

import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = ["my.HITS.pool.root"]

alg = CfgMgr.SimHitAlg()
athAlgSeq += alg

theApp.EvtMax=-1
alg.McEventCollection = "TruthEvent"

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["HIST DATAFILE='myHistoFile.root' OPT='RECREATE'"]