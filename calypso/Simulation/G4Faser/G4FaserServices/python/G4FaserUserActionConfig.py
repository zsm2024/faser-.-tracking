# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
# from FaserMCTruthBase.FaserMCTruthBaseConfigNew import MCTruthSteppingActionToolCfg

G4UA__FaserUserActionSvc=CompFactory.G4UA.FaserUserActionSvc

from G4FaserUserActions.G4FaserUserActionsConfig import CalypsoStackingActionToolCfg, CalypsoTrackingActionToolCfg 
from TrkG4UserActions.TrkG4UserActionsConfig import MaterialStepRecorderCfg


# New function for all user action types
def getDefaultActions(ConfigFlags):
    result = ComponentAccumulator()

    actions = []

    #
    # System stacking action
    # This is used to apply cuts to the particles that are processed
    # potentially ignoring some particles
    #
    actions += [result.popToolsAndMerge( CalypsoStackingActionToolCfg(ConfigFlags)  )]
    
    #
    # Tracking action
    # This creates a trajectory that's updated with each step, and registers
    # secondary particles into the truth record.
    #
    actions += [result.popToolsAndMerge( CalypsoTrackingActionToolCfg(ConfigFlags) )]

    # Some truth handling actions (and timing)
    # if not ConfigFlags.Sim.ISFRun:
    #     actions += [
    #                 #result.popToolsAndMerge( AthenaTrackingActionToolCfg(ConfigFlags) ),
    #                 result.popToolsAndMerge( MCTruthSteppingActionToolCfg(ConfigFlags) )
    #                 #'G4UA::G4SimTimerTool']
    #                 ]
    # # Track counter
    # #actions += ['G4UA::G4TrackCounterTool']

    # # Cosmic Perigee action
    # if ConfigFlags.Beam.Type == 'cosmics' and ConfigFlags.Sim.CavernBG:
    #     actions += ['G4UA::CosmicPerigeeActionTool']
    # # Cosmic filter
    # if ConfigFlags.Beam.Type == 'cosmics' and not ConfigFlags.Sim.ISFRun:
    #     actions += ['G4UA::G4CosmicFilterTool']
    # if ConfigFlags.Sim.StoppedParticleFile:
    #     actions += ['G4UA::StoppedParticleFilterTool',
    #                 'G4UA::StoppedParticleActionTool']
    # # Hit wrapper action
    # if ConfigFlags.Sim.CavernBG == 'Read':
    #     actions += ['G4UA::HitWrapperTool']
    # # Photon killer
    # if ConfigFlags.Sim.PhysicsList == 'QGSP_BERT_HP':
    #     actions += ['G4UA::PhotonKillerTool']
    # # Calo calibration default processing
    # if ConfigFlags.Sim.CalibrationRun == 'LAr+Tile':
    #     actions+=['G4UA::CaloG4::CalibrationDefaultProcessingTool']

    return actions

# add MaterialStepRecorderTool for Geantino scan and MaterialMapping
def getMaterialStepRecorderActions(ConfigFlags):
    result = ComponentAccumulator()

    actions = []

    actions += [result.popToolsAndMerge( CalypsoStackingActionToolCfg(ConfigFlags)  )]

    actions += [result.popToolsAndMerge( MaterialStepRecorderCfg(ConfigFlags)  )]
    
    actions += [result.popToolsAndMerge( CalypsoTrackingActionToolCfg(ConfigFlags) )]

    return actions

def UserActionMaterialStepRecorderSvcCfg(ConfigFlags, name="G4UA::FaserUserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """
    result = ComponentAccumulator()

    #how to convert this flag?
    # from G4AtlasApps.SimFlags import simFlags
    # optActions = simFlags.OptionalUserActionList.get_Value()
    # new user action tools
    kwargs.setdefault('UserActionTools',
                      getMaterialStepRecorderActions(ConfigFlags))

    # placeholder for more advanced config, if needed
    result.addService ( G4UA__FaserUserActionSvc(name, **kwargs) )

    return result
    

def FaserUserActionSvcCfg(ConfigFlags, name="G4UA::FaserUserActionSvc", **kwargs):
    """
    Get the standard UA svc configurable with all default actions added.
    This function is normally called by the configured factory, not users.
    """
    result = ComponentAccumulator()

    #how to convert this flag?
    # from G4AtlasApps.SimFlags import simFlags
    # optActions = simFlags.OptionalUserActionList.get_Value()
    # new user action tools
    kwargs.setdefault('UserActionTools',
                      getDefaultActions(ConfigFlags))

    # placeholder for more advanced config, if needed
    result.addService ( G4UA__FaserUserActionSvc(name, **kwargs) )

    return result
    
