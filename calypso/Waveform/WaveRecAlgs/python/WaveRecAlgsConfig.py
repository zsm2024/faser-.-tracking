""" Define methods used to instantiate configured Waveform reconstruction tools and algorithms

Copyright (C) 2020 CERN for the benefit of the FASER collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from WaveformConditionsTools.WaveformTimingConfig import WaveformTimingCfg

WaveformReconstructionTool = CompFactory.WaveformReconstructionTool
ClockReconstructionTool = CompFactory.ClockReconstructionTool

# One stop shopping for normal FASER data
def WaveformReconstructionCfg(flags):
    """ Return all algorithms and tools for Waveform reconstruction """
    acc = ComponentAccumulator()

    if flags.Input.isMC:
        print("ClockRecAlg diabled for MC in WaveformReconstructionCfg!")
    else:
        acc.merge(WaveformClockRecCfg(flags, "ClockRecAlg"))

    if "TB" in flags.GeoModel.FaserVersion:
        acc.merge(WaveformHitRecCfg(flags, "CaloWaveformRecAlg", "Calo"))

        # Make preshower/veto window 200 ns wide (100 digitizer clock ticks)
        acc.merge(WaveformHitRecCfg(flags, "VetoWaveformRecAlg", "Veto", FitWindowWidth=100 ))
        acc.merge(WaveformHitRecCfg(flags, "PreshowerWaveformRecAlg", "Preshower", FitWindowWidth=100 ))

    else:
        acc.merge(WaveformHitRecCfg(flags, "CaloWaveformRecAlg", "Calo"))
        if flags.Input.isMC:
            print("Calo2WaveformRecAlg diabled for MC in WaveformReconstructionCfg!")
        else:
            acc.merge(WaveformHitRecCfg(flags, "Calo2WaveformRecAlg", "Calo2"))

        # Make preshower window 200 ns wide (value in digitizer clock ticks)
        acc.merge(WaveformHitRecCfg(flags, "VetoWaveformRecAlg", "Veto", FitWindowWidth=100 ))
        acc.merge(WaveformHitRecCfg(flags, "PreshowerWaveformRecAlg", "Preshower", FitWindowWidth=100 ))
        acc.merge(WaveformHitRecCfg(flags, "TriggerWaveformRecAlg", "Trigger", FitWindowWidth=100))
        acc.merge(WaveformHitRecCfg(flags, "VetoNuWaveformRecAlg", "VetoNu", FitWindowWidth=100))

    acc.merge(WaveformTimingCfg(flags))

    return acc

# Return configured WaveformClock reconstruction algorithm
def WaveformClockRecCfg(flags, name="ClockRecAlg", **kwargs):

    acc = ComponentAccumulator()

    tool = ClockReconstructionTool(name="ClockReconstructionTool")
    # tool.CheckResult = True
    kwargs.setdefault("ClockReconstructionTool", tool)

    recoAlg = CompFactory.WaveClockRecAlg(name, **kwargs)
    recoAlg.ClockReconstructionTool = tool
    acc.addEventAlgo(recoAlg)

    return acc

# Return configured WaveformHit reconstruction algorithm
# Specify data source (Veto, VetoNu, Trigger, Preshower, Calo, Test)
def WaveformHitRecCfg(flags, name="WaveformRecAlg", source="", **kwargs):

    acc = ComponentAccumulator()

    #if flags.Input.isMC:
    #    kwargs.setdefault("PeakThreshold", 5)


    tool = WaveformReconstructionTool(name=source+"WaveformRecTool", **kwargs)

    # Remove arguments intended for WaveRecTool
    if "PeakThreshold" in kwargs: kwargs.pop("PeakThreshold")
    if "FitWindowWidth" in kwargs: kwargs.pop("FitWindowWidth")

    kwargs.setdefault("WaveformContainerKey", source+"Waveforms")
    kwargs.setdefault("WaveformHitContainerKey", source+"WaveformHits")
    kwargs.setdefault("WaveformReconstructionTool", tool)
              
    recoAlg = CompFactory.RawWaveformRecAlg(name, **kwargs)
    recoAlg.WaveformReconstructionTool = tool
    acc.addEventAlgo(recoAlg)

    return acc

def WaveformReconstructionOutputCfg(flags, **kwargs):
    """ Return ComponentAccumulator with output for Waveform Reco"""
    acc = ComponentAccumulator()
    ItemList = [
        "xAOD::WaveformHitContainer#*"
        , "xAOD::WaveformHitAuxContainer#*"
        , "xAOD::WaveformClock#*"
        , "xAOD::WaveformClockAuxInfo#*"
    ]
    print(flags)
    acc.merge(OutputStreamCfg(flags, "xAOD", ItemList, disableEventTag=True))
    # ostream = acc.getEventAlgo("OutputStreamRDO")
    # ostream.TakeItemsFromInput = True # Don't know what this does
    return acc

# Return configured "reconstruction" algorithm from scint SIM hits
# Specify data source (Veto, Trigger, Preshower)
def PseudoScintHitToWaveformRecCfg(flags, name="PseudoScintHitToWaveformRecAlg", source="", **kwargs):

    acc = ComponentAccumulator()

    tool = CompFactory.PseudoSimToWaveformRecTool(name=source+"PseudoSimToWaveformRecTool", **kwargs)
    
    kwargs.setdefault("ScintHitContainerKey", source+"Hits")
    kwargs.setdefault("WaveformHitContainerKey", source+"WaveformHits")
    kwargs.setdefault("PseudoSimToWaveformRecTool", tool)

    
    recoAlg = CompFactory.PseudoScintHitToWaveformRecAlg(name, **kwargs)
    recoAlg.PseudoSimToWaveformRecTool = tool

    acc.addEventAlgo(recoAlg)

    return acc

# Return configured "reconstruction" algorithm from calo SIM hits
def PseudoCaloHitToWaveformRecCfg(flags, name="PseudoCaloHitToWaveformRecAlg", **kwargs):

    acc = ComponentAccumulator()

    tool = CompFactory.PseudoSimToWaveformRecTool(name="CaloPseudoSimToWaveformRecTool", **kwargs)
    
    kwargs.setdefault("CaloHitContainerKey", "EcalHits")
    kwargs.setdefault("WaveformHitContainerKey", "CaloWaveformHits")
    kwargs.setdefault("PseudoSimToWaveformRecTool", tool)
    
    recoAlg = CompFactory.PseudoCaloHitToWaveformRecAlg(name, **kwargs)
    recoAlg.PseudoSimToWaveformRecTool = tool

    acc.addEventAlgo(recoAlg)

    return acc
