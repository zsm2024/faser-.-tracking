# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def FaserEventSelectorByteStreamCfg(configFlags, **kwargs):
    result = ComponentAccumulator()

    # from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoSelectorTool 
    # infoTool = CompFactory.xAODMaker.EventInfoSelectorTool
    # xconv = infoTool()

# Load ByteStreamCnvSvc
    byteSvc = CompFactory.FaserByteStreamCnvSvc
    byteSvcInstance = byteSvc(name = "FaserByteStreamCnvSvc")
    byteSvcInstance.InitCnvs += [ "xAOD::FaserTriggerDataAux" , "xAOD::FaserTriggerData"  ]
    result.addService(byteSvcInstance)

# Load EventSelectorByteStream
    bsSelector = CompFactory.FaserEventSelectorByteStream
    selector = bsSelector("FaserEventSelector")
    selector.ByteStreamInputSvc = "FaserByteStreamInputSvc"
    # selector.HelperTools += [xconv]
    selector.Input = configFlags.Input.Files
    result.addService(selector)
    result.setAppProperty("EvtSel",selector.getFullJobOptName())

    EvtPersistencySvc=CompFactory.EvtPersistencySvc
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[byteSvcInstance.getFullJobOptName(),]))

# Properties
    # rn = configFlags.Input.RunNumber
    # if isinstance(rn, type([])):
    #     rn = rn[0]
    # selector.RunNumber = rn
    # selector.InitialTimeStamp = configFlags.Input.InitialTimeStamp

    for k, v in kwargs.items():
        setattr (selector, k, v)

    return result

def FaserByteStreamCnvSvcCfg(configFlags, **kwargs):
    result=ComponentAccumulator()

    from FaserByteStreamCnvSvcBase.FaserByteStreamCnvSvcBaseConfig import FaserByteStreamCnvSvcBaseCfg
    result.merge(FaserByteStreamCnvSvcBaseCfg(configFlags))

# Configure WaveformByteStream converter
    from WaveByteStream.WaveByteStreamConfig import WaveByteStreamCfg
    result.merge(WaveByteStreamCfg(configFlags))

# Configure TrackerByteStream converter
    from TrackerByteStream.TrackerByteStreamConfig import TrackerByteStreamCfg
    result.merge(TrackerByteStreamCfg(configFlags))

# Load ByteStreamEventStorageInputSvc
    bsInputSvc = CompFactory.FaserByteStreamInputSvc
    result.addService(bsInputSvc(name = "FaserByteStreamInputSvc"))

# Load ROBDataProviderSvc
    robProvider = CompFactory.FaserROBDataProviderSvc
    result.addService(robProvider("FaserROBDataProviderSvc"))

# Event selector
    result.merge(FaserEventSelectorByteStreamCfg(configFlags))

# Add in ByteStreamAddressProviderSvc
    byteStreamAddress = CompFactory.FaserByteStreamAddressProviderSvc
    result.addService(byteStreamAddress("FaserByteStreamAddressProviderSvc"))

# Load ProxyProviderSvc
    proxyProvider = CompFactory.ProxyProviderSvc
    result.addService(proxyProvider(ProviderNames = ["FaserByteStreamAddressProviderSvc"]))

    return result

