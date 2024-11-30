# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

DetectorGeometrySvc, G4AtlasSvc, G4FaserGeometryNotifierSvc, FaserPhysicsListSvc=CompFactory.getComps("DetectorGeometrySvc","G4AtlasSvc","G4FaserGeometryNotifierSvc","FaserPhysicsListSvc",)
from G4FaserTools.G4GeometryToolConfig import G4AtlasDetectorConstructionToolCfg
from G4StepLimitation.G4StepLimitationConfig import G4StepLimitationToolCfg
from Pythia8Decayer.Pythia8DecayerConfigNew import Pythia8DecayerPhysicsToolCfg

def DetectorGeometrySvcCfg(ConfigFlags, name="DetectorGeometrySvc", **kwargs):
    result = ComponentAccumulator()
    detConstTool = result.popToolsAndMerge(G4AtlasDetectorConstructionToolCfg(ConfigFlags))
    result.addPublicTool(detConstTool)
    kwargs.setdefault("DetectorConstruction", result.getPublicTool(detConstTool.name))

    result.addService(DetectorGeometrySvc(name, **kwargs))
    return result


def G4AtlasSvcCfg(ConfigFlags, name="G4AtlasSvc", **kwargs):
    if ConfigFlags.Concurrency.NumThreads > 0:
        is_hive = True
    else:
        is_hive = False
    kwargs.setdefault("isMT", is_hive)
    kwargs.setdefault("DetectorGeometrySvc", 'DetectorGeometrySvc')
    return G4AtlasSvc(name, **kwargs)


# def G4GeometryNotifierSvcCfg(ConfigFlags, name="G4GeometryNotifierSvc", **kwargs):
#     kwargs.setdefault("ActivateLVNotifier", True)
#     kwargs.setdefault("ActivatePVNotifier", False)
#     return G4GeometryNotifierSvc(name, **kwargs)


def FaserPhysicsListSvcCfg(ConfigFlags, name="FaserPhysicsListSvc", **kwargs):
    result = ComponentAccumulator()
    PhysOptionList = [ result.popToolsAndMerge(G4StepLimitationToolCfg(ConfigFlags)) ]    
    #PhysOptionList += ConfigFlags.Sim.PhysicsOptions # FIXME Missing functionality
    PhysOptionList += [ result.popToolsAndMerge(Pythia8DecayerPhysicsToolCfg(ConfigFlags)) ]
    PhysDecaysList = []
    kwargs.setdefault("PhysOption", PhysOptionList)
    kwargs.setdefault("PhysicsDecay", PhysDecaysList)
    kwargs.setdefault("PhysicsList", ConfigFlags.Sim.PhysicsList)
    if 'PhysicsList' in kwargs:
        if kwargs['PhysicsList'].endswith('_EMV') or kwargs['PhysicsList'].endswith('_EMX'):
            raise RuntimeError( 'PhysicsList not allowed: '+kwargs['PhysicsList'] )

    kwargs.setdefault("GeneralCut", 1.)
    kwargs.setdefault("NeutronTimeCut", ConfigFlags.Sim.NeutronTimeCut)
    kwargs.setdefault("NeutronEnergyCut", ConfigFlags.Sim.NeutronEnergyCut)
    kwargs.setdefault("ApplyEMCuts", ConfigFlags.Sim.ApplyEMCuts)
    ## from AthenaCommon.SystemOfUnits import eV, TeV
    ## kwargs.setdefault("EMMaxEnergy"     , 7*TeV)
    ## kwargs.setdefault("EMMinEnergy"     , 100*eV)
    """ --- ATLASSIM-3967 ---
        these two options are replaced by SetNumberOfBinsPerDecade
        which now controls both values.
    """
    ## kwargs.setdefault("EMDEDXBinning"   , 77)
    ## kwargs.setdefault("EMLambdaBinning" , 77)
    svc = CompFactory.FaserPhysicsListSvc(name, **kwargs)
    from AthenaCommon.Constants import VERBOSE
    svc.OutputLevel = VERBOSE
    result.addService(svc, primary = True)
    # result.addService(CompFactory.PhysicsListSvc(name, **kwargs), primary = True)
    return result
