svcMgr = theApp.serviceMgr()
if not hasattr( svcMgr, "FaserByteStreamAddressProviderSvc" ):
    from FaserByteStreamCnvSvcBase.FaserByteStreamCnvSvcBaseConf import FaserByteStreamAddressProviderSvc
    svcMgr += FaserByteStreamAddressProviderSvc()


# Correnct syntax for non-containers
svcMgr.FaserByteStreamAddressProviderSvc.TypeNames += [ 
    "xAOD::FaserTriggerData/FaserTriggerData",
    "xAOD::FaserTriggerDataAux/FaserTriggerDataAux."
]

# Add the different Scintillator containers
# The assignemnt of digitizer channels to each container is done
# in the ScintByteStream/ScintByteStreamCnv service
# TestWaveforms is configurable for testing purposes
svcMgr.FaserByteStreamAddressProviderSvc.TypeNames += [ 
    "RawWaveformContainer/CaloWaveforms",
    "RawWaveformContainer/VetoWaveforms",
    "RawWaveformContainer/TriggerWaveforms",
    "RawWaveformContainer/PreshowerWaveforms",
    "RawWaveformContainer/TestWaveforms"  
]

# Add tracker RDO containers
svcMgr.FaserByteStreamAddressProviderSvc.TypeNames += [ 
    "FaserSCT_RDO_Container/SCT_RDOs"
]


