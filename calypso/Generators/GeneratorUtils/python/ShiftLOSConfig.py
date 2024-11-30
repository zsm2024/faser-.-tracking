#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# import sys
# from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator #, ConfigurationError
# from AthenaConfiguration.ComponentFactory import CompFactory

from GeneratorUtils.ShiftLOS import ShiftLOS


def ShiftLOSCfg(ConfigFlags, **kwargs) :
    # import McParticleEvent.Pythonizations

    cfg = ComponentAccumulator()

    shift = ShiftLOS(name = kwargs.setdefault("name", "ShiftLOS"))
    shift.InputMCEventKey =  kwargs.setdefault("InputMCEventKey", "BeamTruthEvent_ATLASCoord")
    shift.OutputMCEventKey =  kwargs.setdefault("OutputMCEventKey", "BeamTruthEvent")    
    shift.xcross = kwargs.setdefault("xcross", 0)
    shift.ycross = kwargs.setdefault("ycross", 0)
    shift.xshift = kwargs.setdefault("xshift", 0)
    shift.yshift = kwargs.setdefault("yshift", 0)    

    #
    #  Alg sequence is now managed in caller
    #
    # try:
    #     # Run for PG
    #     cfg.addEventAlgo(shift, sequenceName = "AthBeginSeq", primary = True) # to run *before* G4
    # except ConfigurationError:
    #     # Run for pool or hepmc
    #     cfg.addEventAlgo(shift, sequenceName = "AthAlgSeq", primary = True) # to run *before* G4        

    cfg.addEventAlgo(shift, primary = True)

    return cfg
