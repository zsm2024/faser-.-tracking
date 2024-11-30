#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# import sys
# from AthenaCommon.AlgSequence import AthSequencer
import ParticleGun as PG

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
# from AthenaConfiguration.ComponentFactory import CompFactory
# from AthenaCommon.Constants import VERBOSE, INFO
from AthenaCommon.SystemOfUnits import TeV, MeV
from AthenaCommon.PhysicalConstants import pi

### add radial pos sampler ### with gaussian beam implemented
from FaserParticleGun.RadialPosSampler import RadialPosSampler



def FaserParticleGunCommonCfg(ConfigFlags, **kwargs) :
    cfg = ComponentAccumulator()

    # have to do this in a clunky way, since ParticleGun does not recognize all keywords we might get passed
    pg = PG.ParticleGun( name = kwargs.setdefault("name", "ParticleGun"),
                         randomSeed = kwargs.setdefault("randomSeed", 12345),
                         randomSvcName = kwargs.setdefault("randomSvcName", "AtRndmGenSvc"),
                         randomStream = kwargs.setdefault("randomStream", "ParticleGun") )

    pg.McEventKey = kwargs.setdefault("McEventKey", "BeamTruthEvent")
    cfg.addEventAlgo(pg, primary=True) 

    return cfg


def FaserParticleGunSingleParticleCfg(ConfigFlags, **kwargs) :
    cfg = FaserParticleGunCommonCfg(ConfigFlags, **kwargs)

    pg = cfg.getPrimary()

    pg.sampler.n   = kwargs.setdefault("n", 1)
    pg.sampler.pid = kwargs.setdefault("pid", -13)
    pg.sampler.mom = PG.EThetaMPhiSampler(energy = kwargs.setdefault("energy", 1*TeV),
                                          theta = kwargs.setdefault("theta", [0, pi/20]), 
                                          phi = kwargs.setdefault("phi", [0, 2*pi]), 
                                          mass = kwargs.setdefault("mass", 0.0) )

    if "radius" in kwargs:
        pg.sampler.pos =  RadialPosSampler(x = kwargs.setdefault("x", 0.0), 
                                           y = kwargs.setdefault("y", 0.0), 
                                           z = kwargs.setdefault("z", -3750.0),
                                           r = kwargs.setdefault("radius", 1.0),
                                           t = kwargs.setdefault("t", 0.0),
                                           axialTiming = kwargs.setdefault("axialTiming", True)  )
    else:
        pg.sampler.pos = PG.PosSampler(x = kwargs.setdefault("x", [-5, 5]), 
                                       y = kwargs.setdefault("y", [-5, 5]), 
                                       z = kwargs.setdefault("z", -3750.0), 
                                       t = kwargs.setdefault("t", 0.0),
                                       axialTiming = kwargs.setdefault("axialTiming", True) )

    return cfg

def FaserParticleGunSingleEcalParticleCfg(ConfigFlags, **kwargs) :
    cfg = FaserParticleGunCommonCfg(ConfigFlags, **kwargs)

    pg = cfg.getPrimary()

    pg.sampler.pid = kwargs.setdefault("pid", 13)
    pg.sampler.mom = PG.EThetaMPhiSampler(energy = kwargs.setdefault("energy", 1*TeV),
                                          theta = kwargs.setdefault("theta", 0.0), 
                                          phi = kwargs.setdefault("phi", 0.0), 
                                          mass = kwargs.setdefault("mass", 105.7) )    

    if "radius" in kwargs:
        pg.sampler.pos =  RadialPosSampler(x = kwargs.setdefault("x", 0.0), 
                                   y = kwargs.setdefault("y", 0.0), 
                                   z = kwargs.setdefault("z", 0.0),
                                   r = kwargs.setdefault("radius", 1.0),
                                   t = kwargs.setdefault("t", 0.0),
                                   axialTiming = kwargs.setdefault("axialTiming", True) )
    else:
        pg.sampler.pos = PG.PosSampler(x = kwargs.setdefault("x", 0.0), 
                                   y = kwargs.setdefault("y", 0.0), 
                                   z = kwargs.setdefault("z", 0.0),
                                   t = kwargs.setdefault("t", 0.0),
                                   axialTiming = kwargs.setdefault("axialTiming", True) )

    return cfg

'''
def FaserParticleGunSingleEcalParticleCfg(ConfigFlags, **kwargs) :
    cfg = FaserParticleGunCommonCfg(ConfigFlags, **kwargs)

    pg = cfg.getPrimary()

    pg.sampler.pid = kwargs.setdefault("pid", 11)
    pg.sampler.mom = PG.EThetaMPhiSampler(energy = kwargs.setdefault("energy", 1*TeV),
                                          theta = kwargs.setdefault("theta", [0, pi/200]), 
                                          phi = kwargs.setdefault("phi", [0, 2*pi]), 
                                          mass = kwargs.setdefault("mass", 0.0) )
    pg.sampler.pos = PG.PosSampler(x = kwargs.setdefault("x", [-5, 5]), 
                                   y = kwargs.setdefault("y", [-5, 5]), 
                                   z = kwargs.setdefault("z", 2730.0), 
                                   t = kwargs.setdefault("t", 0.0),
                                   axialTiming = kwargs.setdefault("axialTiming", True) )

    return cfg
'''
def FaserParticleGunCosmicsCfg(ConfigFlags, **kwargs) :
    # Supported keyword arguments:
    #
    # chargeRatio       (default: 1.27)
    # maxMuonEnergyGeV  (default: 10000)
    # thetaMinDegree    (default: 0)
    # thetaMaxDegree    (default: 80)
    # targetDepthMeters (default: 85)
    # targetDxMm        (default: 600)  - width of target slab
    # targetDyMm        (default: 600)  - height of target slab
    # targetDzMm        (default: 7000) - length of target slab
    # x0Mm              (default: 0)    - X center of target slab in FASER coordinate system
    # y0Mm              (default: 0)    - Y center of target slab in FASER coordinate system
    # z0Mm              (default: 150)  - Z center of target slab in FASER coordinate system
    #
    
    cfg = FaserParticleGunCommonCfg(ConfigFlags, **kwargs)

    pg = cfg.getPrimary()

    from FaserCosmicGenerator import CosmicSampler
    pg.sampler = CosmicSampler(**kwargs)

    return cfg

def FaserParticleGunDecayInFlightCfg(ConfigFlags, **kwargs) :
    # Supported keyword arguments:
    #
    # daughter1_pid (default:  11)
    # daughter2_pid (default: -11)
    # mother_pid    (default: none)
    # mother_mom    (default: PG.EThetaMPhiSampler(sqrt((1*TeV)**2 + (10*MeV)**2),0,10*MeV,0))
    # mother_pos    (default: CylinderSampler([0, 100**2],[0, 2*pi],[-1500, 0],0))
    #
    # Note that ALL of these can be samplers themselves - either the simple, "literal" variety or a sampler object configured by the caller
    #

    cfg = FaserParticleGunCommonCfg(ConfigFlags, **kwargs)

    pg = cfg.getPrimary()

    from DIFGenerator import DIFSampler

    if "radius" in kwargs:
        kwargs["mother_pos"]  =  RadialPosSampler(x = kwargs.setdefault("x", 0.0), 
                                                  y = kwargs.setdefault("y", 0.0), 
                                                  z = kwargs.setdefault("z", -3750.0),
                                                  r = kwargs.setdefault("radius", 1.0),
                                                  t = kwargs.setdefault("t", 0.0),
                                                  axialTiming = kwargs.setdefault("axialTiming", True) )
    else:
        from DIFGenerator.DIFSampler import CylinderSampler
        kwargs["mother_pos"]  = CylinderSampler([0, 100**2], [0, 2*pi], [-1500, 0], 0, True)
    
    if "mother_mom" not in kwargs:
        kwargs["mother_mom"] = PG.EThetaMPhiSampler(kwargs.setdefault("energy", ((1*TeV)**2 + (500*MeV)**2)**0.5),
                                                    kwargs.setdefault("theta", [0, 0.0002]),
                                                    kwargs.setdefault("mass", 500*MeV),
                                                    kwargs.setdefault("phi", [0, 2*pi]) )                                  
        

        
    pg.sampler = DIFSampler(kwargs.setdefault("daughter1_pid", 13),
                            kwargs.setdefault("daughter2_pid", -13),
                            kwargs.setdefault("mother_pid", None),
                            kwargs["mother_mom"],
                            kwargs["mother_pos"] )


    return cfg


def FaserParticleGunForeseeCfg(ConfigFlags, **kwargs) :
    # Supported keyword arguments:
    # model_path    (detault: $PWD)
    # model_name   (default: DarkPhoton)
    # mother_mass   (default: 0.01 GeV)
    # com_energy    (default: 14 TeV)
    # daughter1_pid (default:  11)
    # daughter2_pid (default: -11)
    # mother_pid    (default: none)
    # mother_pos    (default: CylinderSampler([0, 100**2],[0, 2*pi],[-1500, 0],0))
    #
    # Note that ALL of these can be samplers themselves - either the simple, "literal" variety or a sampler object configured by the caller
    #

    cfg = FaserParticleGunCommonCfg(ConfigFlags, **kwargs)

    pg = cfg.getPrimary()

    from ForeseeGenerator.ForeseeSampler import ForeseeNumpySampler
    mother_part = ForeseeNumpySampler(
        model_path = kwargs.get("model_path", "."),
        model_name = kwargs.get("model_name", "DarkPhoton"),
        com_energy = kwargs.get("com_energy", "14"),
        mother_mass = kwargs.get("mother_mass", 0.01),
        mother_pid = kwargs.get("mother_pid", None),
        daughter1_pid = kwargs.get("daughter1_pid", 11),
        daughter2_pid = kwargs.get("daughter2_pid", -11),
        randomSeed = kwargs.get("randomSeed", None)        
        )

    from DIFGenerator import DIFSampler
    pg.sampler = DIFSampler(
        daughter1_pid = kwargs.get("daughter1_pid", 11),
        daughter2_pid = kwargs.get("daughter2_pid", -11),
        )
        
    pg.sampler.mother_sampler = mother_part

    return cfg

def FaserParticleGunCfg(ConfigFlags) :
    generator = ConfigFlags.Sim.Gun.setdefault("Generator", "SingleParticle")
    # generator = ConfigFlags.Sim.Gun.setdefault("Generator", "DecayInFlight")

    kwargs = ConfigFlags.Sim.Gun
    del kwargs["Generator"]
    
    if generator == "SingleEcalParticle" :
        return FaserParticleGunSingleEcalParticleCfg(ConfigFlags, **kwargs)
    elif generator == "Cosmics" :
        return FaserParticleGunCosmicsCfg(ConfigFlags, **kwargs)
    elif generator == "DecayInFlight" :
        return FaserParticleGunDecayInFlightCfg(ConfigFlags, **kwargs)
    elif generator == "Foresee" :
        return FaserParticleGunForeseeCfg(ConfigFlags, **kwargs)    
    else :
        return FaserParticleGunSingleParticleCfg(ConfigFlags, **kwargs )
