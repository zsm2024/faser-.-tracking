#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory
GlobalFieldManagerTool, DetectorFieldManagerTool=CompFactory.getComps("GlobalFieldManagerTool","DetectorFieldManagerTool",)
#
from G4FaserServices.G4FaserFieldServices import StandardFieldSvcCfg, ConstantFieldSvcCfg
#
# Field Managers
#
def FASERFieldManagerToolCfg(ConfigFlags, name='FaserFieldManager', **kwargs):
    result = StandardFieldSvcCfg(ConfigFlags)
    kwargs.setdefault("IntegratorStepper", ConfigFlags.Sim.G4Stepper)
    kwargs.setdefault("FieldSvc", result.getService("StandardField"))
    kwargs.setdefault("UseTightMuonStepping", False)
    if len(ConfigFlags.Sim.G4EquationOfMotion)>0:
        kwargs.setdefault("EquationOfMotion", ConfigFlags.Sim.G4EquationOfMotion )
    result.setPrivateTools(GlobalFieldManagerTool(name, **kwargs))
    return result

def TightMuonsFASERFieldManagerToolCfg(ConfigFlags, name='TightMuonsFASERFieldManager', **kwargs):
    result = StandardFieldSvcCfg(ConfigFlags)
    kwargs.setdefault("IntegratorStepper", ConfigFlags.Sim.G4Stepper)
    kwargs.setdefault("FieldSvc", result.getService("StandardField"))
    kwargs.setdefault("UseTightMuonStepping",True)
    if len(ConfigFlags.Sim.G4EquationOfMotion)>0:
        kwargs.setdefault("EquationOfMotion", ConfigFlags.Sim.G4EquationOfMotion )
    result.setPrivateTools(GlobalFieldManagerTool(name, **kwargs))
    return result

def BasicDetectorFieldManagerToolCfg(ConfigFlags, name='BasicDetectorFieldManager', **kwargs):
    result = StandardFieldSvcCfg(ConfigFlags)
    kwargs.setdefault("IntegratorStepper", ConfigFlags.Sim.G4Stepper)
    kwargs.setdefault("FieldSvc", result.getService("StandardField"))
    kwargs.setdefault('MuonOnlyField',     False)
    if len(ConfigFlags.Sim.G4EquationOfMotion)>0:
        kwargs.setdefault("EquationOfMotion", ConfigFlags.Sim.G4EquationOfMotion )
    result.setPrivateTools(DetectorFieldManagerTool(name, **kwargs))
    return result
#
#  Simple constant field everywhere
#
def BasicDetectorConstantFieldManagerToolCfg(ConfigFlags, name='BasicDetectorConstantFieldManager', **kwargs):
    result = ConstantFieldSvcCfg(ConfigFlags)
    kwargs.setdefault("IntegratorStepper", ConfigFlags.Sim.G4Stepper)
    kwargs.setdefault("FieldSvc", result.getService("ConstantField"))
    kwargs.setdefault("UseTightMuonStepping", False)
    if len(ConfigFlags.Sim.G4EquationOfMotion)>0:
        kwargs.setdefault("EquationOfMotion", ConfigFlags.Sim.G4EquationOfMotion )
    result.setPrivateTools(GlobalFieldManagerTool(name, **kwargs))
    return result
#
#  Same numerical values as ATLAS Inner Detector
#

def EmulsionFieldManagerToolCfg(ConfigFlags, name='EmulsionFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['Emulsion::EmulsionStationA'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def VetoFieldManagerToolCfg(ConfigFlags, name='VetoFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['Veto::VetoStationA'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def VetoNuFieldManagerToolCfg(ConfigFlags, name='VetoNuFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['VetoNu::VetoNuStationA'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)


def TriggerFieldManagerToolCfg(ConfigFlags, name='TriggerFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['Trigger::TriggerStationA'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)


def PreshowerFieldManagerToolCfg(ConfigFlags, name='PreshowerFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['Preshower::PreshowerStationA'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)


def TrackerFieldManagerToolCfg(ConfigFlags, name='TrackerFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['SCT::Station'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def DipoleFieldManagerToolCfg(ConfigFlags, name='DipoleFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['Dipole::Dipole'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def EcalFieldManagerToolCfg(ConfigFlags, name='EcalFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['Ecal::Ecal'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def TrenchFieldManagerToolCfg(ConfigFlags, name='TrenchFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['Trench::Trench'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

