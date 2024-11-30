#
#  Usage: athena.py -c'INPUT=["faser.1.gfaser.root","faser.2.gfaser.root"]; OUTPUT="gFaser.EVNT.pool.root"; EVTMAX=100' GenieReader/GenieReader_jobOptions.py >& GenieReader.log
#
#  INPUT and OUTPUT are mandatory (INPUT can be a list, as shown above)
#  EVTMAX can be omitted; then all input files are read to the end
#

if not 'INPUT' in dir():
    print("Missing INPUT parameter")
    exit()

if not isinstance(INPUT, (list,tuple)):
    INPUT = [INPUT]
    pass

if not 'OUTPUT' in dir():
    print("Missing OUTPUT parameter")
    exit()

import AthenaRootComps.ReadAthenaRoot

svcMgr.EventSelector.InputCollections = INPUT
svcMgr.EventSelector.TupleName = "gFaser"

from GenieReader.GenieReaderAlg import GenieReader

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += GenieReader()

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
ostream = AthenaPoolOutputStream( "StreamEVGEN" , OUTPUT, noTag=True )
ostream.ItemList.remove("EventInfo#*")
ostream.ItemList += [ "EventInfo#McEventInfo", 
                      "McEventCollection#*" ]
print(ostream.ItemList)
if not 'EVTMAX' in dir():
    EVTMAX = -1

theApp.EvtMax = EVTMAX
