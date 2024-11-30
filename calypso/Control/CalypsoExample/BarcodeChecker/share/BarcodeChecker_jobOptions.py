#
#  Usage: athena.py -c'INPUT=["faser.1.gfaser.root","faser.2.gfaser.root"]' BarcodeChecker/BarcodeChecker_jobOptions.py >& BarcodeChecker.log
#
#  INPUT is mandatory (INPUT can be a list, as shown above)
#

if not 'INPUT' in dir():
    print("Missing INPUT parameter")
    exit()

if not isinstance(INPUT, (list,tuple)):
    INPUT = [INPUT]
    pass

from AthenaCommon.GlobalFlags import globalflags

globalflags.InputFormat.set_Value_and_Lock('pool')

import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = INPUT

from BarcodeChecker.BarcodeCheckerAlg import BarcodeCheckerAlg

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += BarcodeCheckerAlg()

theApp.EvtMax = -1
