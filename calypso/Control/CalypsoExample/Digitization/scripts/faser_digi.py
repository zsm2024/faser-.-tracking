#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Run with:
# ./faser_digi.py filepath 
# 
# filepath - fully qualified path, including url if needed, to the input HITS file
#   example: "root://eospublic.cern.ch//eos/experiment/faser/sim/GeniePilot/HITS/1/faser.150fbInv.1.001.HITS.pool.root"
# 
# Options:
# --geom=runtype - flag to specify the data type (TI12OldMC or TI12MC or TestBeamMC).
#   default to TI12MC
#
import sys
import time
import argparse

a = time.time()

parser = argparse.ArgumentParser(description="Run FASER digitization")

parser.add_argument("file_path",
                    help="Fully qualified path of the raw input file")
parser.add_argument("-g", "--geom", default="TI12MC",
                    help="Specify geometry (default: TI12MC, alt: TestBeamMC)")
parser.add_argument("-t", "--tag", default="",
                    help="Specify digi tag (to append to output filename)")
parser.add_argument("--subtractTime", type=float, default=-999.,
                    help="Subtract time parameter for SCT RDOs")
parser.add_argument("--simpleTiming", action="store_true",
                    help="Use simple waveform time digitization")
parser.add_argument("--digiTag", default="",
                    help="Specify tag for waveform digi folder")
parser.add_argument("--short", default="",
                    help="Extend short description")
parser.add_argument("-n", "--nevts", type=int, default=-1,
                    help="Specify number of events to process (default: all)")
parser.add_argument("-v", "--verbose", action='store_true', 
                    help="Turn on DEBUG output")

args = parser.parse_args()

from pathlib import Path

filepath=Path(args.file_path)

# runtype has been provided
runtype= args.geom

print(f"Starting digitization of {filepath.name} with type {runtype}")
if args.nevts > 0:
    print(f"Reconstructing {args.nevts} events by command-line option")

# Start digitization

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
configFlags.Digitization.TruthOutput = True

# TI12 old geometry
if runtype == "TI12OldMC":
    configFlags.GeoModel.FaserVersion = "FASER-01" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"

# Testbeam setup 
elif runtype == "TestBeamMC" :
    configFlags.GeoModel.FaserVersion = "FASER-TB01" # New geometry
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-TB01"

# New TI12 geometry (ugh)
elif runtype == "TI12MC" or runtype == "TI12MC04":
    configFlags.GeoModel.FaserVersion = "FASERNU-04" # New geometry
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04"

elif runtype == "TI12MC03":
    configFlags.GeoModel.FaserVersion = "FASERNU-03" # Old geometry
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"

else:
    print("Invalid geometry type found:", runtype)
    print("Specify correct geometry or update list")
    sys.exit(-1)

# Must use original input string here, as pathlib mangles double // in path names
configFlags.Input.Files = [ args.file_path ]

filestem = filepath.stem
# Remove any filetype modifier
if filestem[-5:] == "-HITS":
    filestem = filestem[:-5]

if len(args.short) > 0:
    words = []
    numbers = []
    first = True

    print(f"Modifying {filestem} to ", end='')

    # Split by - and separate out the run/segment numbers from everything else
    tokens = filestem.split('-')
    for word in reversed(tokens):
        if first and word.isnumeric():
            numbers.insert(0, word)
        else:
            first = False
            words.insert(0, word)

    # Now, build it back together, adding the args.short extension
    filestem = ""
    for word in words:
        filestem += f"{word}-"

    # Tack on the extension
    filestem = filestem[:-1]
    filestem += f"{args.short}-"

    # Now add the numbers
    for word in numbers:
        filestem += f"{word}-"

    filestem = filestem[:-1]

    print(filestem)

if len(args.tag) > 0:
    print(f"{args.tag} in {filestem}?")
    if args.tag in filestem:
        print(f"Not adding tag {args.tag} to file {filestem}")
    else:
        filestem += f"-{args.tag}"

configFlags.Output.RDOFileName = f"{filestem}-RDO.root"
#
# Play around with this?
# configFlags.Concurrency.NumThreads = 2
# configFlags.Concurrency.NumConcurrentEvents = 2
configFlags.lock()

#
# Configure components
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    
acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))
acc.merge(PoolWriteCfg(configFlags))

#
# Needed, or move to MainServicesCfg?
from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
acc.merge(FaserGeometryCfg(configFlags))

# Set up algorithms
from FaserSCT_Digitization.FaserSCT_DigitizationConfigNew import FaserSCT_DigitizationCfg
acc.merge(FaserSCT_DigitizationCfg(configFlags))

# Set the time offset for SCT RDOs
pualg = acc.getEventAlgo("StandardPileUpToolsAlg")
pualg.PileUpTools['FaserSCT_DigitizationTool'].SurfaceChargesGenerator.SubtractTime = args.subtractTime

# Pass something to set folder tag
from CaloDigiAlgs.CaloDigiAlgsConfig import CaloWaveformDigitizationCfg
acc.merge(CaloWaveformDigitizationCfg(configFlags, digiTag=args.digiTag, AdvancedTiming=(not args.simpleTiming)))

from ScintDigiAlgs.ScintDigiAlgsConfig import ScintWaveformDigitizationCfg
acc.merge(ScintWaveformDigitizationCfg(configFlags, digiTag=args.digiTag, AdvancedTiming=(not args.simpleTiming)))

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
