# Copyright (C) 2020-2021 CERN for the benefit of the FASER collaboration

from re import VERBOSE
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

from WaveformConditionsTools.WaveformCableMappingConfig import WaveformCableMappingCfg
from WaveformConditionsTools.WaveformDigitizationConfig import WaveformDigitizationCfg

from IOVDbSvc.IOVDbSvcConfig import addFolders

# One stop shopping for normal FASER data
def CaloWaveformDigitizationCfg(flags, **kwargs):
    """ Return all algorithms and tools for Waveform digitization """
    acc = ComponentAccumulator()

    if not flags.Input.isMC:
        return acc

    # Define folder and tag
    # dbInstance = kwargs.get("dbInstance", "TRIGGER_OFL")
    # dbFolder = kwargs.get("dbFolder", "/WAVE/Digitization")
    # caloDigiTag = kwargs.pop("digiTag", "")

    # if len(caloDigiTag) > 0:
    #     print(f"Found digiTag={caloDigiTag}")
    #     acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection", tag=caloDigiTag))
    # else:
    #     acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection"))

    acc.merge(WaveformDigitizationCfg(flags, **kwargs))
    kwargs.pop("digiTag")  # Remove this if it is specified

    acc.merge(CaloWaveformDigiCfg(flags, "CaloWaveformDigiAlg", **kwargs))
    acc.merge(CaloWaveformDigitizationOutputCfg(flags))
    acc.merge(WaveformCableMappingCfg(flags))

    return acc

# Return configured digitization algorithm from SIM hits
def CaloWaveformDigiCfg(flags, name="CaloWaveformDigiAlg", **kwargs):

    acc = ComponentAccumulator()

    tool = CompFactory.WaveformDigitisationTool(name="CaloWaveformDigtisationTool")
    kwargs.setdefault("WaveformDigitisationTool", tool)

    tool = CompFactory.WaveformDigiConditionsTool(name="CaloDigiConditionsTool", Detector="Calo")
    kwargs.setdefault("DigiConditionsTool", tool)
    
    kwargs.setdefault("CaloHitContainerKey", "EcalHits")
    kwargs.setdefault("WaveformContainerKey", "CaloWaveforms")

    digiAlg = CompFactory.CaloWaveformDigiAlg(name, **kwargs)
    acc.addEventAlgo(digiAlg)

    return acc

def CaloWaveformDigitizationOutputCfg(flags, **kwargs):
    """ Return ComponentAccumulator with output for Waveform Digi"""
    acc = ComponentAccumulator()
    ItemList = [
        "RawWaveformContainer#*"
    ]
    acc.merge(OutputStreamCfg(flags, "RDO"))
    ostream = acc.getEventAlgo("OutputStreamRDO")
    # ostream.TakeItemsFromInput = True # Copies all data from input file to output
    # ostream.TakeItemsFromInput = False
    # Try turning this off
    ostream.ItemList += ItemList
    return acc
