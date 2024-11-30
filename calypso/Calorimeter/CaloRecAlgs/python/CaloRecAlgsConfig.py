""" 
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

Define methods used to instantiate configured Calorimeter Calibration reconstruction tools and algorithms
"""


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
#from IOVDbSvc.IOVDbSvcConfig import addFolders
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

from CaloRecTools.CaloRecToolConfig import CaloRecToolCfg

# One stop shopping for normal FASER data
def CalorimeterReconstructionCfg(flags, **kwargs):
    """ Return all algorithms and tools for Waveform reconstruction """
    acc = ComponentAccumulator()

    kwargs.setdefault("CaloWaveHitContainerKey", "CaloWaveformHits")
    kwargs.setdefault("Calo2WaveHitContainerKey", "Calo2WaveformHits")
    kwargs.setdefault("PreshowerWaveHitContainerKey", "PreshowerWaveformHits")
    
    kwargs.setdefault("CaloHitContainerKey", "CaloHits")
    kwargs.setdefault("Calo2HitContainerKey", "Calo2Hits")
    kwargs.setdefault("PreshowerHitContainerKey", "PreshowerHits")

    acc.merge(CaloRecToolCfg(flags, **kwargs))
    kwargs.pop("MC_calibTag")  # Remove this if it is specified so it does not get pased to CaloRecAlg

    recoAlg = CompFactory.CaloRecAlg("CaloRecAlg", isMC = flags.Input.isMC, **kwargs)
    acc.addEventAlgo(recoAlg)

    return acc

def CalorimeterReconstructionOutputCfg(flags, **kwargs):
    """ Return ComponentAccumulator with output for Calorimeter Reco"""
    acc = ComponentAccumulator()
    ItemList = [
        "xAOD::CalorimeterHitContainer#*"
        , "xAOD::CalorimeterHitAuxContainer#*"
    ]
    acc.merge(OutputStreamCfg(flags, "xAOD", ItemList, disableEventTag=True))
    # ostream = acc.getEventAlgo("OutputStreamRDO")
    # ostream.TakeItemsFromInput = True # Don't know what this does
    return acc

