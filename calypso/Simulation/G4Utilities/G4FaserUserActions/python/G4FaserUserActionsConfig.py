# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
G4UA__CalypsoStackingActionTool=CompFactory.G4UA.CalypsoStackingActionTool
G4UA__CalypsoTrackingActionTool=CompFactory.G4UA.CalypsoTrackingActionTool
# G4UA__AthenaMaterialStepRecorderTool=CompFactory.G4UA.MaterialStepRecorderTool

# this is a bit cumbersome, but it seems ike it is a lot easier to separate
# the getter functionality from all the rest (i.e. adding the action).
# This way, e.g., after the getter is called the tool is automatically added
# to the ToolSvc and can be assigned to a ToolHandle by the add function.
# Also, passing arguments to the getter (like "this is a system action") is not straightforward

def CalypsoStackingActionToolCfg(ConfigFlags, name='G4UA::CalypsoStackingActionTool', **kwargs):

    result = ComponentAccumulator()
    ## Killing neutrinos
    if "FASER" in ConfigFlags.GeoModel.Layout:
        kwargs.setdefault('KillAllNeutrinos',  True)
    ## Neutron Russian Roulette
    #need to check if it exists?
    #if ConfigFlags.hasFlag('Sim.NRRThreshold') and ConfigFlags.hasFlag('Sim.NRRWeight'):
    # if ConfigFlags.Sim.NRRThreshold and ConfigFlags.Sim.NRRWeight:
    #     if ConfigFlags.Sim.CalibrationRun:
    #         raise NotImplementedError("Neutron Russian Roulette should not be used in Calibration Runs.")
    #     kwargs.setdefault('NRRThreshold',  ConfigFlags.Sim.NRRThreshold)
    #     kwargs.setdefault('NRRWeight',  ConfigFlags.Sim.NRRWeight)
    # ## Photon Russian Roulette
    # if ConfigFlags.Sim.PRRThreshold and ConfigFlags.Sim.PRRWeight:
    #     if ConfigFlags.Sim.CalibrationRun:
    #         raise NotImplementedError("Photon Russian Roulette should not be used in Calibration Runs.")
    #     kwargs.setdefault('PRRThreshold',  ConfigFlags.Sim.PRRThreshold)
    #     kwargs.setdefault('PRRWeight',  ConfigFlags.Sim.PRRWeight)
    kwargs.setdefault('IsISFJob', ConfigFlags.Sim.ISFRun)

    result.setPrivateTools( G4UA__CalypsoStackingActionTool(name,**kwargs) )
    return result


def CalypsoTrackingActionToolCfg(ConfigFlags, name='G4UA::CalypsoTrackingActionTool', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('SecondarySavingLevel', 2)
    
    #
    # subDetLevel will need to be increased to 2 if the cavern infrastructure is simulated
    # (adding another layer of volumes between Faser and subdetectors)
    #
    subDetLevel=1
    # if "ATLAS" in ConfigFlags.Sim.Layout and \
    # (ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.CavernBG != 'Signal' ):
    #     subDetLevel=2

    kwargs.setdefault('SubDetVolumeLevel', subDetLevel)
    result.setPrivateTools( G4UA__CalypsoTrackingActionTool(name,**kwargs) )
    return result

# def AthenaMaterialStepRecorderToolCfg(ConfigFlags, name='G4UA::MaterialStepRecorderTool', **kwargs):
#     result = ComponentAccumulator()

#     result.setPrivateTools( G4UA__AthenaMaterialStepRecorderTool(name,**kwargs) )
#     return result
