#!/usr/bin/env python
# 
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
#
# Run with
# faser_ntuple_maker.py [options] 
#
# Options:
#   --isMC - needed to reconstruct MC data
#
import sys
import time
import argparse

a = time.time()

parser = argparse.ArgumentParser(description="Run PHYS ntuple production")

parser.add_argument("path",
                    help="Fully qualified input path (directory or file)")

parser.add_argument("--slice", type=int, default=0,
                    help="Specify ordinal output file to produce")
parser.add_argument("--files", type=int, default=1,
                    help="Specify reco files per slice")
parser.add_argument("--merge", type=int, default=1,
                    help="Specify merged files per reco file (MC only)")
parser.add_argument("--last", type=int, default=0,
                    help="Specify last file in slice (normally --files)")
parser.add_argument("-g", "--geom", default="",
                    help="Specify geometry (if it can't be parsed from run number\n Values: Ti12Data03 (2022 TiT12)")
parser.add_argument("--outfile", default="",
                    help="Override output file name")
                    
parser.add_argument("-v", "--verbose", action='store_true', 
                    help="Turn on DEBUG output")
parser.add_argument("-n", "--nevents", type=int, default=-1,
                    help="Specify number of events to process (default: all)")

parser.add_argument("-t", "--tag", default="",
                    help="Specify tag (to append to output filename)")

parser.add_argument("--isMC", action='store_true',
                    help="Running on digitised MC rather than data")
parser.add_argument("--partial", action='store_true',
                    help="Allow partial input files")

parser.add_argument("-c", "--cond", default="",
                    help="Specify global conditions tag (default: OFLCOND-FASER-04)")
parser.add_argument("--backward", action='store_true',
                    help="Use backward CKF tracks (default: forward)")

parser.add_argument("--trigFilt", action='store_true',
                    help="apply trigger event filter")
parser.add_argument("--scintFilt", action='store_true',
                    help="apply scintillator event filter")
parser.add_argument("--NoTrackFilt", action='store_true',
                    help="Don't apply track event filter (default: do)")
parser.add_argument("--no_stable", action='store_true',
                    help="Don't apply stable beam requirement (default: do)")

parser.add_argument("--unblind", action='store_true',
                    help="Don't apply signal blinding (default: do)")
parser.add_argument("--onlyblind", action='store_true',
                    help="Only store events that were blinded (will override unblind arg)")

parser.add_argument("--grl", default="",
                    help="Specify GRL to apply")
 
parser.add_argument("--fluka", action='store_true',
                    help="Add FLUKA weights to ntuple")
parser.add_argument("--genie", action='store_true',
                    help="Add Genie weights to ntuple")


args = parser.parse_args()

from pathlib import Path

# Must figure out what we are doing here
filepath = Path(args.path)

filelist = []
# If this is a directory, need to create file list
if filepath.is_dir():

    # Use expected data pattern to find files (data only)
    runstr = filepath.stem

    # Make list of segments to search for
    seglist = []
    if args.merge > 1:
        start = args.slice * args.files * args.merge

        # Number of files to combine
        if args.last > 0:
            num = args.last
        else:
            num = args.files

        # Make segment list
        for i in range(start, start+num*args.merge, args.merge):
            seg = f"{i:05d}-{(i+args.merge-1):05d}"
            seglist.append(seg)

    else:
        start = args.slice * args.files
        if args.last > 0:
            end = start + args.last
        else:
            end = start + args.files

        seglist = [f'{seg:05d}' for seg in range(start, end)]


    for seg in seglist:

        if args.isMC:
            searchstr = f"FaserMC-*-{seg}-*xAOD.root"
        else:
            searchstr = f"Faser-Physics-{runstr}-{seg}-*xAOD.root"

        flist = list(filepath.glob(searchstr))
        if len(flist) == 0:
            print(f"Didn't find file {searchstr}!")
            if args.partial: continue
            sys.exit(1)

        elif len(flist) > 1:
            print(f"Found multiple matches for {searchstr}!")
            print(flist)
            sys.exit(1)

        filestr = str(flist[0].resolve())
        # Use proper EOS file path here?
        if filestr[:4] == '/eos':
            filestr = f"root://eospublic.cern.ch/{filestr}"
        filelist.append(filestr)
    # End of loop over segments

    # Parse name to create outfile
    firstfile = Path(filelist[0])
    firststem = str(firstfile.stem)
    firstfaser = firststem.split('-')[0]
    firstshort = firststem.split('-')[1]
    runstr = firststem.split('-')[2]
    firstseg = firststem.split('-')[3]
    if args.merge > 1:
        firstseg2 = firststem.split('-')[4]

    lastfile = Path(filelist[-1])
    laststem = str(lastfile.stem)
    lastseg = laststem.split('-')[3]
    if args.merge > 1:
        lastseg = laststem.split('-')[4]

    print(f"Faser = {firstfaser}")
    print(f"Short = {firstshort}")
    print(f"Run   = {runstr}")
    print(f"First = {firstseg}")
    print(f"Last  = {lastseg}")
    print(f"Tag  = {args.tag}")

    # Find any tags
    tagstr = firststem.replace(f"{firstfaser}-{firstshort}-{runstr}-{firstseg}", "")
    if args.merge > 1:
        tagstr = tagstr.replace(f"-{firstseg2}", "")

    tagstr = tagstr.replace("-xAOD", "")

    print(f"Tag   = {tagstr}")

    # Build output name
    outfile = f"{firstfaser}-{firstshort}-{runstr}-{firstseg}-{lastseg}"

    # This will include the leading -
    if len(tagstr) > 0:
        outfile += f"{tagstr}"

    if len(args.tag) > 0 and args.tag not in tagstr:
        outfile += f"-{args.tag}"

    outfile += "-PHYS.root"


# If this is a single file, just process that
# Could be a url, so don't check if this is a file
else:
    filelist.append(args.path)

    # Build output name
    filestem = str(filepath.stem)

    # Remove -xAOD from end
    if filestem[-5:] == "-xAOD":
        filestem = filestem[:-5]

    # Do we want to add a tag?
    if len(args.tag) > 0 and args.tag not in filestem:
        filestem += f"-{args.tag}"

    outfile = f"{filestem}-PHYS.root"

# Print out what we found
if len(filelist) == 0:
    printf("Found no files for {args.path}!")
    sys.exit(1)
elif len(filelist) == 1:
    print("Processing file:")
else:
    print("Processing files:")
for f in filelist:
    print(f)

# Override output file?
if len(args.outfile) > 0:
    outfile = args.outfile

print("Output file:")
print(outfile)

print(f"Trigger Filter  = {args.trigFilt}")
print(f"Scintillator Filter  = {args.scintFilt}")
print(f"Track Filter = {not args.NoTrackFilt}")
print(f"Blind = {not args.unblind}")
print(f"OnlyBlinded = {args.onlyblind}")
print(f"Stable Beams = {not args.no_stable}")
print(f"Backward = {args.backward}")
print(f"GRL = {args.grl}")

# OK, lets run the job here
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags

from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

Configurable.configurableRun3Behavior = True

# Configure
configFlags = initConfigFlags()
configFlags.Input.Files = filelist

if len(args.geom) > 0:
    runtype = args.geom
else:
    if args.isMC:
        runtype = "TI12MC04"
    else:
        runtype = "TI12Data04"

if runtype in ["TI12Data04", "TI12MC04"]:
    configFlags.GeoModel.FaserVersion = "FASERNU-04" # FASER geometry
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04"   

elif runtype == "TI12Data03":
    configFlags.GeoModel.FaserVersion = "FASERNU-03" # FASER geometry
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04" # Pick up new alignment   

elif runtype == "TI12MC03":
    configFlags.GeoModel.FaserVersion = "FASERNU-03" # FASER geometry
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
    
else:
    print(f"Unknown: --geom {runtype}!")
    print("Specify correct type or update list")
    sys.exit(1)

# Check if we override the global conditions tag
if len(args.cond):
    print(f"Unknown: --geom {runtype}!")
    print("Specify correct type or update list")
    sys.exit(1)

# Check if we override the global conditions tag
if len(args.cond):
    print(f"Setting ConfigFlags.IOVDb.GlobalTag = {args.cond}")
    configFlags.IOVDb.GlobalTag = args.cond

configFlags.Input.isMC = args.isMC
if args.isMC:
    configFlags.IOVDb.DatabaseInstance = "OFLP200"           # Use MC conditions 
else:
    configFlags.IOVDb.DatabaseInstance = "CONDBR3"           # Use data conditions

configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig

configFlags.Common.isOnline = False
configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.lock()

# Core components
acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))

# Create kwargs for NtupleDumper
grl_kwargs = {}
if args.grl:
    grl_kwargs['GoodRunsList'] = args.grl
    grl_kwargs['ApplyGoodRunsList'] = True

mc_kwargs = {}
if args.genie:
    mc_kwargs['UseGenieWeights'] = True
if args.fluka:
    mc_kwargs['UseFlukaWeights'] = True

# Use backward tracks for ntuple
# Default for this parameter is "CKFTrackCollectionWithoutIFT"
if args.backward:
    if args.isMC:
        mc_kwargs['TrackCollectionWithoutIFT'] = "CKFTrackCollectionBackwardWithoutIFT"
    else:
        grl_kwargs['TrackCollectionWithoutIFT'] = "CKFTrackCollectionBackwardWithoutIFT"

# algorithm
from NtupleDumper.NtupleDumperConfig import NtupleDumperAlgCfg
if args.isMC:
    acc.merge(NtupleDumperAlgCfg(configFlags, outfile, **mc_kwargs))

else:
    acc.merge(NtupleDumperAlgCfg(configFlags, outfile, DoBlinding=(not args.unblind), OnlyBlinded=args.onlyblind, DoScintFilter = args.scintFilt, DoTrackFilter = (not args.NoTrackFilt), DoTrigFilter = args.trigFilt, StableOnly = (not args.no_stable), **grl_kwargs) )

if not args.verbose:
    from AthenaConfiguration.ComponentFactory import CompFactory
    AthenaEventLoopMgr = CompFactory.AthenaEventLoopMgr()
    AthenaEventLoopMgr.EventPrintoutInterval=1000
    acc.addService(AthenaEventLoopMgr)

else:
    nd = acc.getEventAlgo("NtupleDumperAlg")
    nd.OutputLevel = VERBOSE

# Hack to avoid problem with our use of MC databases when isMC = False
if not args.isMC:
    replicaSvc = acc.getService("DBReplicaSvc")
    replicaSvc.COOLSQLiteVetoPattern = ""
    replicaSvc.UseCOOLSQLite = True
    replicaSvc.UseCOOLFrontier = False
    replicaSvc.UseGeomSQLite = True

if args.verbose:
    log.setLevel(VERBOSE)
    acc.printConfig(withDetails=True)
    configFlags.dump()
else:
    log.setLevel(INFO)

acc.getService("MessageSvc").Format = "% F%40W%S%7W%R%T %0W%M"

# Execute and finish
sc = acc.run(maxEvents=args.nevents)

b = time.time()
log.info(f"Finish execution in {b-a} seconds")

# Success should be 0
if sc.isSuccess():
    log.info("Execution succeeded")
    sys.exit(0)
else:
    log.info("Execution failed, return 1")
    sys.exit(1)

