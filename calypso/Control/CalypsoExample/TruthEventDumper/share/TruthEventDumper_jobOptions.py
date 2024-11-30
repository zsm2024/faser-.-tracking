#
#  Usage: athena.py -c'INPUT=["faser.1.gfaser.root","faser.2.gfaser.root"]; COLLECTION="TruthEvent"; MAXEVT=-1; SKIPEVT=0' TruthEventDumper/TruthEventDumper_jobOptions.py >& TruthEventDumper.log
#
#  INPUT is mandatory (INPUT can be a list, as shown above)
#  COLLECTION would normally be either "TruthEvent" (Geant4 particles) or "BeamTruthEvent" (generator particles)
#  MAXEVT and SKIPEVT are self-explanatory and optional
#

if not 'INPUT' in dir():
    print("Missing INPUT parameter")
    exit()

if not 'MAXEVT' in dir():
    MAXEVT = -1

if not 'SKIPEVT' in dir():
    SKIPEVT = 0

if not 'COLLECTION' in dir():
    COLLECTION = "TruthEvent"

if not isinstance(INPUT, (list,tuple)):
    INPUT = [INPUT]
    pass

from AthenaCommon.GlobalFlags import globalflags

globalflags.InputFormat.set_Value_and_Lock('pool')

import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = INPUT

from TruthEventDumper.TruthEventDumperAlg import TruthEventDumperAlg

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += TruthEventDumperAlg(MCEventKey=COLLECTION)

theApp.EvtMax = MAXEVT
theApp.SkipEvents = SKIPEVT
