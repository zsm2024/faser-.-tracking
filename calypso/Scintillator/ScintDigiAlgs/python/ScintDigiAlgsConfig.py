# Copyright (C) 2020-2021 CERN for the benefit of the FASER collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

from WaveformConditionsTools.WaveformCableMappingConfig import WaveformCableMappingCfg
from WaveformConditionsTools.WaveformDigitizationConfig import WaveformDigitizationCfg

# One stop shopping for normal FASER data
def ScintWaveformDigitizationCfg(flags, **kwargs):
    """ Return all algorithms and tools for Waveform digitization """
    acc = ComponentAccumulator()

    if not flags.Input.isMC:
        return acc

    acc.merge(WaveformDigitizationCfg(flags, **kwargs))
    kwargs.pop("digiTag")

    if "TB" in flags.GeoModel.FaserVersion:
        acc.merge(ScintWaveformDigiCfg(flags, "VetoWaveformDigiAlg", "Veto", **kwargs))
        acc.merge(ScintWaveformDigiCfg(flags, "PreshowerWaveformDigiAlg", "Preshower", **kwargs))
    else:
        acc.merge(ScintWaveformDigiCfg(flags, "TriggerWaveformDigiAlg", "Trigger", **kwargs))
        acc.merge(ScintWaveformDigiCfg(flags, "VetoWaveformDigiAlg", "Veto", **kwargs))
        acc.merge(ScintWaveformDigiCfg(flags, "VetoNuWaveformDigiAlg", "VetoNu", **kwargs))
        acc.merge(ScintWaveformDigiCfg(flags, "PreshowerWaveformDigiAlg", "Preshower", **kwargs))

    acc.merge(ScintWaveformDigitizationOutputCfg(flags))
    acc.merge(WaveformCableMappingCfg(flags))

    return acc

# Return configured digitization algorithm from SIM hits
# Specify data source (Veto, Trigger, Preshower)
def ScintWaveformDigiCfg(flags, name="ScintWaveformDigiAlg", source="", **kwargs):

    acc = ComponentAccumulator()

    tool = CompFactory.WaveformDigitisationTool(name=source+"WaveformDigtisationTool")
    kwargs.setdefault("WaveformDigitisationTool", tool)
    
    kwargs.setdefault("ScintHitContainerKey", source+"Hits")
    kwargs.setdefault("WaveformContainerKey", source+"Waveforms")

    if "TB" in flags.GeoModel.FaserVersion and source == "Veto":
        # The testbeam counters were actually VetoNu, so use those parameters 
        source = "VetoNu"

    tool = CompFactory.WaveformDigiConditionsTool(name=source+"CaloDigiConditionsTool", Detector=source)
    kwargs.setdefault("DigiConditionsTool", tool)

    digiAlg = CompFactory.ScintWaveformDigiAlg(name, **kwargs)     
    acc.addEventAlgo(digiAlg)

    return acc

def ScintWaveformDigitizationOutputCfg(flags, **kwargs):
    """ Return ComponentAccumulator with output for Waveform Digi"""
    acc = ComponentAccumulator()
    ItemList = [
        "RawWaveformContainer#*"
    ]
    acc.merge(OutputStreamCfg(flags, "RDO"))
    ostream = acc.getEventAlgo("OutputStreamRDO")
    # ostream.TakeItemsFromInput = True # Copies all data from input file to output
    # ostream.TakeItemsFromInput = False
    ostream.ItemList += ItemList
    return acc
