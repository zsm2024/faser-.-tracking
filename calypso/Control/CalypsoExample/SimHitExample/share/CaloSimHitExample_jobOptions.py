from AthenaCommon.GlobalFlags import globalflags

globalflags.InputFormat.set_Value_and_Lock('pool')

import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = ["my.HITS.pool.root"]

alg = CfgMgr.CaloSimHitAlg()
athAlgSeq += alg

theApp.EvtMax=-1
alg.McEventCollection = "TruthEvent"

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["HIST DATAFILE='myHistoFile.root' OPT='RECREATE'"]
