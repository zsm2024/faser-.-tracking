#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, tempfile, pathlib
from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from TruthIO.TruthIOConf import HepMCReadFromFile

def get_file_skip_events(ConfigFlags):
    "Create a file with events from ConfigFlags.Exec.SkipEvents to ConfigFlags.Exec.SkipEvents + ConfigFlags.Exec.MaxEvents"

    usetemp = True
    #usetemp = False

    skip = ConfigFlags.Exec.SkipEvents
    fname = ConfigFlags.Input.Files[0]
    evtMax = ConfigFlags.Exec.MaxEvents

    if skip == 0:
        return fname

    print(f"get_file_skip_events skipping {skip} events with max {evtMax}")

    if usetemp:
        fout = tempfile.NamedTemporaryFile("w", delete = False)
        foutname = fout.name
    else:
        infile = pathlib.Path(fname)
        # Put this in current working directory
        if evtMax > 0:
            end = skip+evtMax
        else:
            end = 'all'

        foutname = f"{infile.stem}=evts{skip}-{end}.{infile.suffix}"
        #foutname, fext = ".".join(fname.split('.')[:-1]), fname.split('.')[-1]
        #foutname = f"{foutname}-evts{skip}-{skip+evtMax}{fext}"        
        fout = open(foutname, "w")

    fout.write("HepMC::Version 2.06.09\nHepMC::IO_GenEvent-START_EVENT_LISTING\n")
    
    ievt = 0
    with open(fname) as fin:
        for l in fin:
            if l.startswith("E "):
                ievt += 1

            if evtMax > 0 and ievt > skip + evtMax:
                break
                
            if ievt > skip:
                #print(f"Writing event {ievt}")
                fout.write(l)
            # else:
                # print(f"Skipping event {ievt}")

    fout.write("HepMC::IO_GenEvent-END_EVENT_LISTING\n")
    fout.close()

    #print(f"Wrote to file {foutname}")

    return foutname
                
def HepMCReaderCfg(ConfigFlags, **kwargs) :
#
# Alg sequence now managed in caller
#    
    cfg = ComponentAccumulator()
    from TruthIO.TruthIOConf import HepMCReadFromFile
    hepmc = CompFactory.HepMCReadFromFile(name = kwargs.setdefault("name", "FASERHepMCReader"))
    hepmc.InputFile = get_file_skip_events(ConfigFlags) # ConfigFlags.Input.Files[0] 
    hepmc.McEventKey = kwargs.setdefault("McEventKey", "BeamTruthEvent")
    
    # cfg.addEventAlgo(hepmc, sequenceName = "AthBeginSeq", primary = True) # to run *before* G4
    cfg.addEventAlgo(hepmc, primary = True) # to run *before* G4

    return cfg
