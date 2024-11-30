# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Service configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from BarcodeServices.BarcodeServicesConfig import BarcodeSvcCfg
from ISF_HepMC_Tools.ISF_HepMC_ToolsConfig import ParticleFinalStateFilterCfg, GenParticleInteractingFilterCfg
# from FaserISF_HepMC_Tools.FaserISF_HepMC_ToolsConfigNew import FaserTruthStrategyCfg, FaserDipoleTruthStrategyCfg
from FaserISF_HepMC_Tools.FaserISF_HepMC_ToolsConfigNew import TruthStrategyGroupCfg, TrenchStrategyGroupCfg

ISF__FaserTruthSvc, ISF__FaserGeoIDSvc, ISF__FaserInputConverter = CompFactory.getComps("ISF::FaserTruthSvc","ISF::FaserGeoIDSvc","ISF::FaserInputConverter")

def GenParticleFiltersToolCfg(ConfigFlags):
    result = ComponentAccumulator()

    #acc1 = ParticlePositionFilterDynamicCfg(ConfigFlags)
    genParticleFilterList = []

    acc1 = ParticleFinalStateFilterCfg(ConfigFlags)
    genParticleFilterList += [result.popToolsAndMerge(acc1)]

    # acc2 = ParticlePositionFilterDynamicCfg(ConfigFlags)
    # genParticleFilterList += [result.popToolsAndMerge(acc2)]

    # acc3 = EtaPhiFilterCfg(ConfigFlags)
    # genParticleFilterList += [result.popToolsAndMerge(acc3)]

    # acc4 = GenParticleInteractingFilterCfg(ConfigFlags)
    # genParticleFilterList += [result.popToolsAndMerge(acc4)]

    result.setPrivateTools(genParticleFilterList)
    return result


def FaserInputConverterCfg(ConfigFlags, name="ISF_FaserInputConverter", **kwargs):
    result = ComponentAccumulator()

    #just use this barcodeSvc for now. TODO - make configurable
    #from G4AtlasApps.SimFlags import simFlags
    #kwargs.setdefault('BarcodeSvc', simFlags.TruthStrategy.BarcodeServiceName())
    result = ComponentAccumulator()
    # kwargs.setdefault('BarcodeSvc', result.getPrimaryAndMerge(BarcodeSvcCfg(ConfigFlags)).name) 

    kwargs.setdefault("UseGeneratedParticleMass", False)

    acc_GenParticleFiltersList = GenParticleFiltersToolCfg(ConfigFlags)
    kwargs.setdefault("GenParticleFilters", result.popToolsAndMerge(acc_GenParticleFiltersList) )

    result.addService(ISF__FaserInputConverter(name, **kwargs))
    return result

#
# Generic Truth Service Configurations
#
def FaserTruthServiceCfg(ConfigFlags, name="FaserISF_TruthService", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault('BarcodeSvc', result.getPrimaryAndMerge(BarcodeSvcCfg(ConfigFlags)).name) 
    
    # acc = FaserTruthStrategyCfg(ConfigFlags)
    # acc2= FaserDipoleTruthStrategyCfg(ConfigFlags)
    # kwargs.setdefault('TruthStrategies',[result.popToolsAndMerge(acc), result.popToolsAndMerge(acc2)])
    acc = TruthStrategyGroupCfg(ConfigFlags)
    # FaserNu hack
    acc2 = TrenchStrategyGroupCfg(ConfigFlags)
    kwargs.setdefault('TruthStrategies', [result.popToolsAndMerge(acc), result.popToolsAndMerge(acc2)])

    kwargs.setdefault('SkipIfNoChildren', True)
    kwargs.setdefault('SkipIfNoParentBarcode', True)

    import PyUtils.RootUtils as rootUtils
    ROOT = rootUtils.import_root()

    import cppyy
    cppyy.load_library('FaserDetDescrDict')
    from ROOT.FaserDetDescr import FaserRegion

    kwargs.setdefault('ForceEndVtxInRegions', [FaserRegion.fFaserNeutrino,
                                               FaserRegion.fFaserScintillator,
                                               FaserRegion.fFaserTracker,
                                               FaserRegion.fFaserDipole,
                                               FaserRegion.fFaserCalorimeter,
                                               FaserRegion.fFaserCavern,
                                               FaserRegion.fFaserTrench])
                                               
    #long_lived_simulators = ['LongLived', 'longLived', 'QS']
    #from ISF_Config.ISF_jobProperties import ISF_Flags
    #is_long_lived_simulation = any(x in ISF_Flags.Simulator() for x in long_lived_simulators) #FIXME this should be set in a nicer way.
    is_long_lived_simulation = True
    if is_long_lived_simulation:
        kwargs.setdefault('QuasiStableParticlesIncluded', True)

    result.addService(ISF__FaserTruthSvc(name, **kwargs))
    return result
