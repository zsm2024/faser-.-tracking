#
#  DEPRECATED - do not use or copy this file for use
#
# Read in bytestream and write Raw Data Objects to xAOD
#
# Useful to set this to see ERS messages (low-level file reading):
# export TDAQ_ERS_DEBUG_LEVEL=1
#
# Set debug level up to 5 for even more output
#

#
# Configure ByteStream Input 
from FaserByteStreamCnvSvc import ReadByteStream

# Provide converters
include( "FaserByteStreamCnvSvcBase/BSAddProvSvc_jobOptions.py" )

# Read single file
svcMgr.FaserEventSelector.Input = [ "cosmics.raw" ]

# Read multiple files
#svcMgr.FaserEventSelector.Input = [ "data/Faser-Physics-000000-00000.raw",
#                                    "data/Faser-Physics-000000-00001.raw",
#                                    "data/Faser-Physics-000000-00002.raw"]

# Uncomment to dump out each raw event as read
svcMgr.FaserByteStreamInputSvc.DumpFlag = True

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1

from AthenaCommon.AlgSequence import AthSequencer, AlgSequence
topSequence = AthSequencer("AthAlgSeq")
job = AlgSequence()

# Produces lots of output...
MessageSvc.OutputLevel = DEBUG

svcMgr.FaserEventSelector.OutputLevel = DEBUG
svcMgr.FaserByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.FaserByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.FaserByteStreamAddressProviderSvc.OutputLevel = DEBUG

svcMgr.FaserByteStreamCnvSvc.InitCnvs += [
    "xAOD::FaserTriggerData"
]

from FaserTrigCnv.FaserTrigCnvConf import FaserTriggerDataAccess
tda = FaserTriggerDataAccess("FaserTriggerDataAccess")
tda.OutputLevel = DEBUG
job += tda

from WaveByteStream.WaveByteStreamConf import RawWaveformAccess
wfm = RawWaveformAccess("RawWaveformAccess")
wfm.OutputLevel = DEBUG
job += wfm

from TrackerByteStream.TrackerByteStreamConf import TrackerDataAccess
trk = TrackerDataAccess("TrackerDataAccess")
trk.OutputLevel = DEBUG
job += trk

# For debugging CLID errors
svcMgr.ClassIDSvc.OutputLevel = DEBUG

# Try writing out xAOD
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD_test.pool.root" )
xaodStream.AddItem( "xAOD::EventInfo#*" )
xaodStream.AddItem( "xAOD::EventAuxInfo#*" )
# Currently, this doesn't write the Aux data...
xaodStream.AddItem( "xAOD::FaserTriggerData#*")
xaodStream.AddItem( "xAOD::FaserTriggerDataAux#*")
# Tracker RDOs
# xaodStream.AddItem( "FaserSCT_RDO_Container#*" )
xaodStream.Print()

#ServiceMgr.StoreGateSvc.Dump=True
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr(EventPrintoutInterval = 100)
ServiceMgr.MessageSvc.defaultLimit = 1000
