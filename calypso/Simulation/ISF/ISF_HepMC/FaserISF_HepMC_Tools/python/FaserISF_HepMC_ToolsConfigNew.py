# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

ISF__FaserTruthStrategy, ISF__FaserGenParticlePositionFilter, ISF__FaserGenParticleGenericFilter=CompFactory.getComps("ISF::FaserTruthStrategy","ISF::FaserGenParticlePositionFilter","ISF::FaserGenParticleGenericFilter")

# from AthenaCommon.SystemOfUnits import MeV, mm
from AthenaCommon.SystemOfUnits import MeV

#Functions yet to be migrated:
#getParticleSimWhiteList, getParticlePositionFilterMS
#getTruthStrategyGroupID, getTruthStrategyGroupIDHadInt, getTruthStrategyGroupCaloMuBrem_MC15, getTruthStrategyGroupCaloDecay, getValidationTruthStrategy, getLLPTruthStrategy

#--------------------------------------------------------------------------------------------------
## GenParticleFilters

# def ParticleFinalStateFilterCfg(ConfigFlags, name="ISF_ParticleFinalStateFilter", **kwargs):
#     result = ComponentAccumulator()
#     G4NotInUse = not ConfigFlags.Sim.UsingGeant4
#     G4NotInUse = G4NotInUse and ConfigFlags.Sim.ISFRun
#     # use CheckGenInteracting==False to allow GenEvent neutrinos to propagate into the simulation
#     kwargs.setdefault("CheckGenSimStable", G4NotInUse)
#     kwargs.setdefault("CheckGenInteracting", G4NotInUse)

#     result.setPrivateTools(ISF__GenParticleFinalStateFilter(name, **kwargs))
#     return result

# placeholder

def FaserParticlePositionFilterCfg(ConfigFlags, name="ISF_FaserParticlePositionFilter", **kwargs):
    result = ComponentAccumulator()

    # accGeoID = FaserGeoIDSvcCfg(ConfigFlags)
    # result.merge(accGeoID)

    # ParticlePositionFilter
    kwargs.setdefault('GeoIDService' , 'ISF_FaserGeoIDSvc'    ) 
    # kwargs.setdefault('GeoIDService' , result.getService('ISF_FaserGeoIDSvc')   ) 

    result.setPrivateTools(ISF__FaserGenParticlePositionFilter(name, **kwargs))
    return result

# def ParticlePositionFilterIDCfg(ConfigFlags, name="ISF_ParticlePositionFilterID", **kwargs):
#     # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
#     import ROOT, cppyy
#     cppyy.loadDictionary('AtlasDetDescrDict')
#     AtlasRegion = ROOT.AtlasDetDescr

#     kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID ] )
#     return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

# def ParticlePositionFilterCaloCfg(ConfigFlags, name="ISF_ParticlePositionFilterCalo", **kwargs):
#     # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
#     import ROOT, cppyy
#     cppyy.loadDictionary('AtlasDetDescrDict')
#     AtlasRegion = ROOT.AtlasDetDescr

#     kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
#                                             AtlasRegion.fAtlasForward,
#                                             AtlasRegion.fAtlasCalo ] )
#     return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

# def ParticlePositionFilterMSCfg(name="ISF_ParticlePositionFilterMS", **kwargs):
#     # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
#     import ROOT, cppyy
#     cppyy.loadDictionary('AtlasDetDescrDict')
#     AtlasRegion = ROOT.AtlasDetDescr

#     kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
#                                             AtlasRegion.fAtlasForward,
#                                             AtlasRegion.fAtlasCalo,
#                                             AtlasRegion.fAtlasMS ] )
#     return ParticlePositionFilterCfg(name, **kwargs)

# def ParticlePositionFilterWorldCfg(ConfigFlags, name="ISF_ParticlePositionFilterWorld", **kwargs):
#     # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
#     import ROOT, cppyy
#     cppyy.loadDictionary('AtlasDetDescrDict')
#     AtlasRegion = ROOT.AtlasDetDescr
#     kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
#                                             AtlasRegion.fAtlasForward,
#                                             AtlasRegion.fAtlasCalo,
#                                             AtlasRegion.fAtlasMS,
#                                             AtlasRegion.fAtlasCavern ] )
#     return ParticlePositionFilterCfg(ConfigFlags, name, **kwargs)

# def ParticlePositionFilterDynamicCfg(ConfigFlags, name="ISF_ParticlePositionFilterDynamic", **kwargs):
#     # automatically choose the best fitting filter region
    
#     #if ConfigFlags.Detector.SimulateMuon:
#     if True:
#       return ParticlePositionFilterWorldCfg(ConfigFlags, name, **kwargs)
#     elif ConfigFlags.Detector.SimulateCalo:
#       return ParticlePositionFilterCaloCfg(ConfigFlags, name, **kwargs)
#     elif ConfigFlags.Detector.SimulateID:
#       return ParticlePositionFilterIDCfg(ConfigFlags, name, **kwargs)
#     else:
#       return ParticlePositionFilterWorldCfg(ConfigFlags, name, **kwargs)

# def GenParticleInteractingFilterCfg(ConfigFlags, name="ISF_GenParticleInteractingFilter", **kwargs):
#     result = ComponentAccumulator()

#     #todo (dnoel) - add this functionality
#     #from G4AtlasApps.SimFlags import simFlags
#     #simdict = simFlags.specialConfiguration.get_Value()
#     #if simdict is not None and "InteractingPDGCodes" in simdict:
#     #    kwargs.setdefault('AdditionalInteractingParticleTypes', eval(simdict["InteractingPDGCodes"]))
#     #if simdict is not None and "NonInteractingPDGCodes" in simdict:
#     #    kwargs.setdefault('AdditionalNonInteractingParticleTypes', eval(simdict["InteractingNonPDGCodes"]))

#     result.setPrivateTools(ISF__GenParticleInteractingFilter(name, **kwargs))
#     return result

# def EtaPhiFilterCfg(ConfigFlags, name="ISF_EtaPhiFilter", **kwargs):
#     result = ComponentAccumulator()
#     # EtaPhiFilter
#     EtaRange = 7.0 if ConfigFlags.Detector.SimulateLucid else 6.0
#     kwargs.setdefault('MinEta' , -EtaRange)
#     kwargs.setdefault('MaxEta' , EtaRange)
#     kwargs.setdefault('MaxApplicableRadius', 30*mm)

#     result.setPrivateTools(ISF__GenParticleGenericFilter(name, **kwargs))
#     return result

# placeholder

def FaserParticleGenericFilterCfg(ConfigFlags, name="ISF_FaserGenericFilter", **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(ISF__FaserGenParticleGenericFilter(name, **kwargs))
    return result


#--------------------------------------------------------------------------------------------------
## Truth Strategies

# Brems: fBremsstrahlung (3)
# Conversion: fGammaConversion (14), fGammaConversionToMuMu (15), fPairProdByCharged (4)
# Decay: 201-298, fAnnihilation(5), fAnnihilationToMuMu (6), fAnnihilationToHadrons (7)
# Ionization: fIonisation (2), fPhotoElectricEffect (12)
# Hadronic: (111,121,131,141,151,161,210)
# Compton: fComptonScattering (13)
# G4 process types:
#  http://www-geant4.kek.jp/lxr/source//processes/management/include/G4ProcessType.hh
# G4 EM sub types:
#  http://www-geant4.kek.jp/lxr/source//processes/electromagnetic/utils/include/G4EmProcessSubType.hh
# G4 HadInt sub types:
#  http://www-geant4.kek.jp/lxr/source//processes/hadronic/management/include/G4HadronicProcessType.hh#L46


def TruthStrategyGroupCfg(ConfigFlags, name="ISF_MCTruthStrategyGroupID", **kwargs):

    import ROOT, cppyy
    cppyy.load_library('FaserDetDescrDict')
    FaserRegion = ROOT.FaserDetDescr.FaserRegion

    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinEkin", 100.*MeV)
    kwargs.setdefault("ChildMinEkin" , 100.*MeV)
    kwargs.setdefault("VertexTypes", [3, 14, 15, 4, 5, 6, 7, 2, 12, 13, 111, 121, 131, 132, 141, 151, 152, 161, 310])  # EM *and* nuclear
    kwargs.setdefault("VertexTypeRangeLow"  , 201)  # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh" , 298)  # ...
    kwargs.setdefault("Regions", [FaserRegion.fFaserNeutrino,
                                  FaserRegion.fFaserScintillator,
                                  FaserRegion.fFaserTracker,
                                  FaserRegion.fFaserCalorimeter,
                                  FaserRegion.fFaserCavern])
    result.setPrivateTools(CompFactory.ISF.FaserTruthStrategy(name, **kwargs))
    return result

# FaserNu hack
def TrenchStrategyGroupCfg(ConfigFlags, name="ISF_TrenchStrategyGroupID", **kwargs):

    import ROOT, cppyy
    cppyy.load_library('FaserDetDescrDict')
    FaserRegion = ROOT.FaserDetDescr.FaserRegion

    result = ComponentAccumulator()
    kwargs.setdefault("ParentMinEkin", 10000.*MeV)
    kwargs.setdefault("ChildMinEkin" , 10000.*MeV)
    kwargs.setdefault("VertexTypes", [3, 14, 15, 4, 5, 6, 7, 2, 12, 13, 111, 121, 131, 132, 141, 151, 152, 161, 310])  # EM *and* nuclear
    kwargs.setdefault("VertexTypeRangeLow"  , 201)  # All kinds of decay processes
    kwargs.setdefault("VertexTypeRangeHigh" , 298)  # ...
    kwargs.setdefault("Regions", [FaserRegion.fFaserTrench])
    result.setPrivateTools(CompFactory.ISF.FaserTruthStrategy(name, **kwargs))
    return result


# def FaserDipoleTruthStrategyCfg(ConfigFlags, name="ISF_FaserDipoleTruthStrategy", **kwargs):
#     result = ComponentAccumulator()

#     import ROOT, cppyy
#     cppyy.load_library('FaserDetDescrDict')
#     FaserRegion = ROOT.FaserDetDescr.FaserRegion
#     #
#     # Save truth in Dipole region
#     #
#     kwargs.setdefault('Regions', [FaserRegion.fFaserDipole,
#                                   FaserRegion.fFaserNeutrino,
#                                   FaserRegion.fFaserCavern])
#     kwargs.setdefault('ParentMinEkin', 1000.0*MeV)
#     kwargs.setdefault('ChildMinEkin', 1000.0*MeV)
#     result.setPrivateTools(ISF__FaserTruthStrategy(name, **kwargs))
#     return result


# def FaserTruthStrategyCfg(ConfigFlags, name="ISF_FaserTruthStrategy", **kwargs):
#     result = ComponentAccumulator()

#     import ROOT, cppyy
#     cppyy.load_library('FaserDetDescrDict')
#     FaserRegion = ROOT.FaserDetDescr.FaserRegion
#     #
#     # Save truth in all regions except Dipole
#     #
#     kwargs.setdefault('Regions', [
#                                 #   FaserRegion.fFaserNeutrino,
#                                   FaserRegion.fFaserScintillator,
#                                   FaserRegion.fFaserTracker])
#                                 #   FaserRegion.fFaserDipole,
#                                 #   FaserRegion.fFaserCalorimeter,
#                                 #   FaserRegion.fFaserCavern])
#     # kwargs.setdefault('ParentMinEkin', 0.1*MeV)
#     # kwargs.setdefault('ChildMinEkin', 0.1*MeV)
#     result.setPrivateTools(ISF__FaserTruthStrategy(name, **kwargs))
#     return result

# def TruthStrategyGroupID_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupID_MC15", **kwargs):
#     result = ComponentAccumulator()
#     kwargs.setdefault('ParentMinPt'         , 100.*MeV)
#     kwargs.setdefault('ChildMinPt'          , 300.*MeV)
#     kwargs.setdefault('VertexTypes'         , [ 3, 14, 15, 4, 5, 6, 7, 2, 12, 13 ])
#     kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
#     kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
#     kwargs.setdefault('Regions', [1,2]) # Could import AtlasDetDescr::AtlasRegion enum as in TruthService CfgGetter methods here
#     result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
#     return result

# def TruthStrategyGroupIDHadInt_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupIDHadInt_MC15", **kwargs):
#     result = ComponentAccumulator()
#     kwargs.setdefault('ParentMinPt'                       , 100.*MeV)
#     kwargs.setdefault('ChildMinPt'                        , 300.*MeV)
#     kwargs.setdefault('VertexTypes'                       , [ 111, 121, 131, 141, 151, 161, 210 ])
#     kwargs.setdefault('AllowChildrenOrParentPassKineticCuts' , True)
#     kwargs.setdefault('Regions', [1])
#     result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
#     return result

# def TruthStrategyGroupCaloMuBremCfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloMuBrem", **kwargs):
#     result = ComponentAccumulator()
#     kwargs.setdefault('ParentMinEkin'       , 500.*MeV)
#     kwargs.setdefault('ChildMinEkin'        , 100.*MeV)
#     kwargs.setdefault('VertexTypes'         , [ 3 ])
#     kwargs.setdefault('ParentPDGCodes'      , [ 13, -13 ])
#     kwargs.setdefault('Regions', [3])
#     result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
#     return result

# def TruthStrategyGroupCaloDecay_MC15Cfg(ConfigFlags, name="ISF_MCTruthStrategyGroupCaloDecay_MC15", **kwargs):
#     result = ComponentAccumulator()
#     kwargs.setdefault('ParentMinEkin'       , 1000.*MeV)
#     kwargs.setdefault('ChildMinEkin'        , 500.*MeV)
#     kwargs.setdefault('VertexTypes'         , [ 5, 6, 7 ])
#     kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
#     kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
#     kwargs.setdefault('Regions', [3])
#     result.setPrivateTools(ISF__GenericTruthStrategy(name, **kwargs))
#     return result
