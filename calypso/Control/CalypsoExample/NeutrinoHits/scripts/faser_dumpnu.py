#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Run with:
# ./faser_dumpnu.py filepath 
# 
# filepath - fully qualified path, including url if needed, to the input HITS file
#   example: "root://eospublic.cern.ch//eos/experiment/faser/sim/GeniePilot/HITS/1/faser.150fbInv.1.001.HITS.pool.root"
# 
# Options:
#
import sys
import time
import argparse

a = time.time()

parser = argparse.ArgumentParser(description="Run FASER NeutrinoRecAlgs")

parser.add_argument("file_path",
                    help="Fully qualified path of the raw input file")
parser.add_argument("-t", "--tag", default="",
                    help="Specify digi tag (to append to output filename)")
parser.add_argument("-n", "--nevts", type=int, default=-1,
                    help="Specify number of events to process (default: all)")
parser.add_argument("-v", "--verbose", action='store_true', 
                    help="Turn on DEBUG output")

args = parser.parse_args()

from pathlib import Path

filepath=Path(args.file_path)

print(f"Starting processing of {filepath.name}")
if args.nevts > 0:
    print(f"Processing {args.nevts} events by command-line option")

# Start ntuple dumping

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import VERBOSE, INFO

from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags

Configurable.configurableRun3Behavior = True
    
# Flags for this job
configFlags = initConfigFlags()
configFlags.Input.isMC = True                    # Needed to bypass autoconfig
configFlags.IOVDb.DatabaseInstance = "OFLP200"   # Use MC conditions for now

configFlags.Input.ProjectName = "mc20"
#configFlags.GeoModel.Align.Dynamic    = False
configFlags.Beam.NumberOfCollisions = 0.

configFlags.GeoModel.FaserVersion = "FASERNU-03" 
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"

configFlags.Detector.GeometryEmulsion = True
configFlags.Detector.GeometryTrench   = True

# Must use original input string here, as pathlib mangles double // in path names
configFlags.Input.Files = [ args.file_path ]

filestem = filepath.stem
# Remove any filetype modifier
if filestem[-5:] == "-HITS":
    filestem = filestem[:-5]

if len(args.tag) > 0:
    print(f"{args.tag} in {filestem}?")
    if args.tag in filestem:
        print(f"Not adding tag {args.tag} to file {filestem}")
    else:
        filestem += f"-{args.tag}"

outfile = f"{filestem}-NTUP.root"
print(f'Writing output file {outfile}')

configFlags.lock()

#
# Configure components
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    
acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))

# Algorithm
from NeutrinoRecAlgs.NeutrinoRecAlgsConfig import NeutrinoRecAlgsCfg 
acc.merge(NeutrinoRecAlgsCfg(configFlags, McEventCollection = "TruthEvent", OutputFile=outfile))

# Configure verbosity    
if args.verbose:
    acc.foreach_component("*").OutputLevel = VERBOSE
    configFlags.dump()

else:
    acc.foreach_component("*").OutputLevel = INFO

acc.foreach_component("*ClassID*").OutputLevel = INFO

acc.getService("MessageSvc").Format = "% F%40W%S%7W%R%T %0W%M"

# Execute and finish
sc = acc.run(maxEvents=args.nevts)

b = time.time()
from AthenaCommon.Logging import log
log.info(f"Finish execution in {b-a} seconds")

sys.exit(not sc.isSuccess())
