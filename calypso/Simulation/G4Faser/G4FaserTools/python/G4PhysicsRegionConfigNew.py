# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

RegionCreator=CompFactory.RegionCreator

#  Values identical to ATLAS SCT

def NeutrinoPhysicsRegionToolCfg(ConfigFlags, name="NeutrinoPhysicsRegionTool", **kwargs):
    kwargs.setdefault("RegionName", "Neutrino")
    volumeList = ['Emulsion::FrontFilm', 'Emulsion::BackFilm', 'Emulsion::Baseboard', 'Emulsion::EmulsionStationA']
    kwargs.setdefault("VolumeList", volumeList)
    kwargs.setdefault("ElectronCut", 0.01)
    kwargs.setdefault("PositronCut", 0.01)
    kwargs.setdefault("GammaCut",    0.01)
    return RegionCreator(name, **kwargs)

def ScintillatorPhysicsRegionToolCfg(ConfigFlags, name='ScintillatorPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'Scintillator')
    volumeList = ['Veto::Plate' , 'Trigger::Plate', 'Preshower::Plate', 'VetoNu::Plate']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def TrackerPhysicsRegionToolCfg(ConfigFlags, name="TrackerPhysicsRegionTool", **kwargs):
    kwargs.setdefault("RegionName", "Tracker")
    volumeList = ['SCT::BRLSensor']
    kwargs.setdefault("VolumeList", volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

# Calo Regions
def EcalPhysicsRegionToolCfg(ConfigFlags, name='EcalPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'Ecal')
    volumeList = ['Ecal::Ecal']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeEMB = 0.03
    # from G4AtlasApps.SimFlags import simFlags
    if '_EMV' not in ConfigFlags.Sim.PhysicsList and '_EMX' not in ConfigFlags.Sim.PhysicsList:
        rangeEMB = 0.1    
    kwargs.setdefault("ElectronCut", rangeEMB)
    kwargs.setdefault("PositronCut", rangeEMB)
    kwargs.setdefault("GammaCut",    rangeEMB)
    return RegionCreator(name, **kwargs)

def CavernPhysicsRegionToolCfg(ConfigFlags, name='CavernPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'Cavern')
    volumeList = ['Trench::Trench']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

