# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def FaserByteStreamCnvSvcBaseCfg(flags, **kwargs):
    result=ComponentAccumulator()

    adxProvider = CompFactory.FaserByteStreamAddressProviderSvc()
    result.addService(adxProvider)

    adxProvider.TypeNames += [ "xAOD::FaserTriggerData/FaserTriggerData",
                               "xAOD::FaserTriggerDataAux/FaserTriggerDataAux." ]

    adxProvider.TypeNames += [
        "RawWaveformContainer/CaloWaveforms",
        "RawWaveformContainer/Calo2Waveforms",
        "RawWaveformContainer/VetoWaveforms",
        "RawWaveformContainer/VetoNuWaveforms",
        "RawWaveformContainer/TriggerWaveforms",
        "RawWaveformContainer/PreshowerWaveforms",
        "RawWaveformContainer/ClockWaveforms",
        "RawWaveformContainer/TestWaveforms"  
    ]

    adxProvider.TypeNames += [ "FaserSCT_RDO_Container/SCT_RDOs",
                               "FaserSCT_RDO_Container/SCT_LEVELMODE_RDOs",
                               "FaserSCT_RDO_Container/SCT_EDGEMODE_RDOs" ]

    return result

