#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Run with:
# ./faser_digi_merge.py dirpath 
# 
# filepath - fully qualified path, to the directory with input HITS file
#   example: "/eos/experiment/faser/sim/tb21/particle_gun/000100/rdo/test"
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

parser.add_argument("dir_path",
                    help="Fully qualified path of the input file directory")
parser.add_argument("-p", "--partial", action="store_true",
                    help="Allow partial merge (default: all specified files required)")
parser.add_argument("-g", "--geom", default="TI12MC",
                    help="Specify geometry (default: TI12MC, alt: TestBeamMC)")
parser.add_argument("-s", "--slice", type=int, default=0,
                    help="Specify file slice to produce")
parser.add_argument("-f", "--files", type=int, default=5,
                    help="Specify number of input files to run in one batch")
parser.add_argument("--complete", action="store_true",
                    help="This is the complete run, remove segment numbers")
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

dirpath = Path(args.dir_path)

# runtype has been provided
runtype=args.geom

# Does the directory exist?
if not (dirpath.exists() and dirpath.is_dir()):
    print(f"Problem with directory {args.dir_path}")
    sys.exit(1)

# Create segment list
seglist = list(range(args.slice*args.files, (args.slice+1)*args.files))

# Now build file list
filelist = []
dirlist = list(dirpath.glob('FaserMC-*-HITS.root'))
if len(dirlist) == 0:
    print(f"No HITS file found in directory {args.dir_path}")
    sys.exit(1)

print("HITS files available:")
[print(file) for file in dirlist]

# Get run number for pattern matching
stem = dirlist[0].stem
spl = stem.split('-')
short = spl[1]
run = spl[2]

for seg in seglist:
    # No longer assume all HITS files are complete
    #if seg >= len(dirlist):
    #    print(f"Requested file segment {seg} but only {len(dirlist)} files found")
    #    if args.partial:
    #        break
    #    else:
    #        sys.exit(1)  # Abort this job

    # Check if segment number exists in hits file (this is not perfect)
    segstr = f"FaserMC-{short}-{run}-{seg:05d}*HITS.root"
    matchlist = list(dirpath.glob(segstr))
    if len(matchlist) == 0:
        print(f"Segment {segstr} not found!")
        if not args.partial: sys.exit(1) # abort
        continue
    elif len(matchlist) > 1:
        print(f"Segment {segstr} matched {len(matchlist)} files!")
        sys.exit(1)  # Should never happen!
    else:
        print(f"Segment {segstr} found!")

    filelist.append(matchlist[0])  

if len(filelist) == 0:
    # Asked for range that doesn't exist
    print(f"No files found for slice {args.slice} with Nfiles={args.files}")
    sys.exit(1)

# Figure out the file pattern for the output
stem = filelist[0].stem
spl = stem.split('-')
short = spl[1]
run = spl[2]
seglo = int(spl[3])
# Can be multiple tags
tagstr = ''
for tag in spl[4:]:
    if tag == "HITS": break
    if len(tagstr) > 0:
        tagstr += "-"
    tagstr += tag

# Also find the largest file number
stem = filelist[-1].stem
spl = stem.split('-')
seghi = int(spl[3])

# append to short name?
if len(args.short) > 0:
    short += args.short

# Shouldn't normally remove segments with MC as we may always add
# more files later.  --complete overrides this if wanted

# Build output filename
if args.complete and (seglo == 0) and ((seghi+1) == len(dirlist)):  # Full run
    outfile = f"FaserMC-{short}-{run}"
elif seglo == seghi:  # Single segment
    outfile = f"FaserMC-{short}-{run}-{seglo:05}"
else:
    outfile = f"FaserMC-{short}-{run}-{seglo:05}-{seghi:05}"

# Add existing tag
if len(tagstr) > 0:
    outfile += f"-{tagstr}"

# Was a tag requested?  
if len(args.tag) > 0:
    if args.tag in tagstr:
        print(f"Not adding tag {args.tag} to file {filelist[0]}")
    else:
        outfile += f"-{args.tag}"

# Finish output file
outfile += "-RDO.root"

print(f"Found files from {seglo} to {seghi}")
print(f"Starting digitization of outfile {outfile} with type {runtype}")
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


# Try just passing the filelist
if args.dir_path[:22] == '/eos/experiment/faser/':
    configFlags.Input.Files = [f"root://eospublic.cern.ch/{str(file)}" for file in filelist]
else:
    configFlags.Input.Files = [str(file) for file in filelist]

configFlags.Output.RDOFileName = outfile

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
