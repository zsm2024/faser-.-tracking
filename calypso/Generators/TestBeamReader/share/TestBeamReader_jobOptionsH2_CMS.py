#
#  Usage: athena.py -c'INPUT=["/eos/project/f/faser-preshower/simulations/H2_beamline/1M_events/FASER_e-_v46_100GeV.root"]; OUTPUT="100GeV.H2.EVNT.pool.root"' TestBeamReader/TestBeamReader_jobOptionsH2_CMS.py
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
svcMgr.EventSelector.TupleName = "t"

from TestBeamReader.TestBeamReaderAlgH2_CMS import TestBeamReader

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += TestBeamReader()

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
ostream = AthenaPoolOutputStream( "StreamEVGEN" , OUTPUT, noTag=True )
ostream.ItemList.remove("EventInfo#*")
ostream.ItemList += [ "EventInfo#McEventInfo", 
                      "McEventCollection#*" ]
print(ostream.ItemList)
if not 'EVTMAX' in dir():
    EVTMAX = -1

theApp.EvtMax = EVTMAX
