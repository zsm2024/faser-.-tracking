#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Run with:
# ./faser_reco.py [--geom=runtype] filepath 
# 
# filepath - fully qualified path, including url if needed, to the input raw data file
#   example: "root://hepatl30//atlas/local/torrence/faser/commissioning/TestBeamData/Run-004150/Faser-Physics-004150-00000.raw"
# 
# runtype - optionally specify the data type (TI12Data, TI12Data02, TI12Data03 or TestBeamData).
#
# Options:
#   --isMC - needed to reconstruct MC data
#   --testBeam - shortcut to specify testbeam geometry
#
import sys
import time
import argparse

a = time.time()

parser = argparse.ArgumentParser(description="Run FASER reconstruction")

parser.add_argument("file_path",
                    help="Fully qualified path of the raw input file")
parser.add_argument("-g", "--geom", default="",
                    help="Specify geometry (if it can't be parsed from run number)\n Values: TI12Data03 (2022 TI12)")
parser.add_argument("-c", "--cond", default="",
                    help="Override global conditions tag (old alignment: --cond FASER-03")

parser.add_argument("-r", "--reco", default="",
                    help="Specify reco tag (to append to output filename)")
parser.add_argument("-n", "--nevents", type=int, default=-1,
                    help="Specify number of events to process (default: all)")
parser.add_argument("--skip", type=int, default=0,
                    help="Specify number of events to skip (default: none)")
parser.add_argument("-v", "--verbose", action='store_true', 
                    help="Turn on DEBUG output")
parser.add_argument("--isMC", action='store_true',
                    help="Running on digitised MC rather than data")
parser.add_argument("--MC_calibTag", default="",
                    help="Specify tag used to reconstruct MC calo energy: (WAVE-Calibration-01-LG-nofilt, WAVE-Calibration-01-LG, WAVE-Calibration-01-HG-nofilt, or WAVE-Calibration-01-HG) ")
parser.add_argument("--testBeam", action='store_true',
                    help="Set geometry for 2021 test beam")
parser.add_argument("--isOverlay", action='store_true', 
                    help="Set overlaid data input")
parser.add_argument("--noTracking", action='store_true',
                    help="Turn off tracking (for R24 debugging)")

args = parser.parse_args()

from pathlib import Path

filepath=Path(args.file_path)

# runtype has been provided
if len(args.geom) > 0:
    runtype=args.geom

# Shortcut for testbeam 
elif args.testBeam:
    print(f"Use 2021 TestBeam configuration")
    runtype = "TestBeamData"

else:
    print(f"Assuming 2024 TI12 geometry (TI12Data04)")
    runtype = "TI12Data04"

    # Try to pick correct geometry from run number
    # This won't work for testbeam data, 
    # so lets call this a hack for now
    runname = filepath.parts[-1]
    try:
        runnumber = int(runname.split('-')[2])
    except Exception as e:
        print(f"Failed to find run number in {filepath}")
        print(f"Couldn't parse {runname}")
        print(f"Leave runtype as {runtype}!")
    else:
        if runnumber > 6700: # Not sure if this is exact
            print(f"Found run number {runnumber}, using TI12 configuration with IFT+faserNu")
            runtype = "TI12Data04"
        elif runnumber > 5302: # Last TI12 run on Nov. 23, 2021 without IFT
            print(f"Found run number {runnumber}, using TI12 configuration with IFT")
            runtype = "TI12Data02"
        else:
            print(f"Found run number {runnumber}, using original TI12 configuration")
            runtype = "TI12Data"

print(f"Starting reconstruction of {filepath.name} with type {runtype}")
if args.nevents > 0:
    print(f"Reconstructing {args.nevents} events by command-line option")
if args.skip > 0:
    print(f"Skipping {args.skip} events by command-line option")

# Start reconstruction

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import VERBOSE, INFO

from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags

Configurable.configurableRun3Behavior = True
    
# Flags for this job
configFlags = initConfigFlags()
configFlags.Input.isMC = args.isMC               # Needed to bypass autoconfig
if args.isMC:
    configFlags.IOVDb.DatabaseInstance = "OFLP200"   # Use MC conditions
else:
    configFlags.IOVDb.DatabaseInstance = "CONDBR3" # Use data conditions

configFlags.Input.ProjectName = "data20"
configFlags.Exec.SkipEvents = args.skip

# Flags for later
useCKF = True
useCal = False
useLHC = False

if args.noTracking:
    useCKF = False

# TI12 Cosmics geometry
if runtype == "TI12Data":
    configFlags.GeoModel.FaserVersion = "FASER-01" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"

# Testbeam setup 
elif runtype in ["TestBeamData", "TestBeamMC"]:
    configFlags.GeoModel.FaserVersion = "FASER-TB01" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-TB01"
    print("Set global tag to OFLCOND-FASER-TB01")
    useCKF = False
    useCal = True

# New TI12 geometry (ugh)
elif runtype == "TI12Data02":
    configFlags.GeoModel.FaserVersion = "FASER-02" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"

# Final 2022 TI12 geometry
elif runtype in ["TI12Data03", "TI12MC03"]:
    configFlags.GeoModel.FaserVersion = "FASERNU-03"
    # Ugh, this is a horrible hack
    # and I am not sure it is even needed as there may not be a difference
    # in reco between the MC global tags...
    if args.isMC:
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03" # Consistent with sim/digi
    else:
        configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04" # Updated 2023 alignment
        
    useCal = True
    if not args.isMC:
        useLHC = True

# Updated 2023 TI12 geometry
elif runtype in ["TI12Data04", "TI12MC04"]:
    configFlags.GeoModel.FaserVersion = "FASERNU-04" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04"
    useCal = True
    if not args.isMC:
        useLHC = True

else:
    print("Invalid run type found:", runtype)
    print("Specify correct type or update list")
    sys.exit(1)

# Check if we want to override global conditions tag
if len(args.cond):
    configFlags.IOVDb.GlobalTag = args.cond
    print(f"Override global tag configFlags.IOVDb.GlobalTag = {args.config}") 

if useCKF:
    # Enable ACTS material corrections, this crashes testbeam geometries
    #configFlags.TrackingGeometry.MaterialSource = "/cvmfs/faser.cern.ch/repo/sw/database/DBRelease/current/acts/material-maps.json"
    configFlags.TrackingGeometry.MaterialSource = "/afs/cern.ch/work/x/xai/public/FASER/develop/run/material-maps-alma9.json"

# Must use original input string here, as pathlib mangles double // in path names
configFlags.Input.Files = [ args.file_path ]

filestem = filepath.stem
# Remove any filetype modifier
if filestem[-4:] == "-RDO":
    filestem = filestem[:-4]

if len(args.reco) > 0:
    filestem += f"-{args.reco}"

configFlags.addFlag("Output.xAODFileName", f"{filestem}-xAOD.root")
configFlags.Output.ESDFileName = f"{filestem}-ESD.root"
configFlags.Output.doWriteESD = False

#
# Play around with this?
# configFlags.Concurrency.NumThreads = 2
# configFlags.Concurrency.NumConcurrentEvents = 2
configFlags.lock()

#
# Configure components
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    
acc = MainServicesCfg(configFlags)
acc.merge(PoolWriteCfg(configFlags))

#
# Set up RAW data access

if args.isMC or args.isOverlay:
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(configFlags))
else:    
    from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
    acc.merge(FaserByteStreamCnvSvcCfg(configFlags))

#
# Needed, or move to MainServicesCfg?
from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
acc.merge(FaserGeometryCfg(configFlags))

if useLHC and not args.isOverlay:
    from LHCDataAlgs.LHCDataAlgConfig import LHCDataAlgCfg
    acc.merge(LHCDataAlgCfg(configFlags))

# Set up algorithms
if not args.isOverlay:
    from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionCfg    
    acc.merge(WaveformReconstructionCfg(configFlags))
#
#    # Calorimeter Energy reconstruction
    if useCal:
        from CaloRecAlgs.CaloRecAlgsConfig import CalorimeterReconstructionCfg
        acc.merge(CalorimeterReconstructionCfg(configFlags, MC_calibTag=args.MC_calibTag))

# Tracker clusters
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
# acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="Pos_SCT_RDOs"))
acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="SCT_RDOs", checkBadChannels=True))

#
# SpacePoints
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
acc.merge(TrackerSpacePointFinderCfg(configFlags))

# Try Dave's new fitter
# Loosen ReducedChi2Cut until alignment improves
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
acc.merge(SegmentFitAlgCfg(configFlags,
                           ReducedChi2Cut=25.,  
                           SharedHitFraction=0.61, 
                           MinClustersPerFit=5, 
                           TanThetaXZCut=0.083))

# Turn on CKF track finding 
if useCKF:
    # 
    # Ghost removal
    from FaserActsKalmanFilter.GhostBustersConfig import GhostBustersCfg
    acc.merge(GhostBustersCfg(configFlags))

    #
    # Kalman Filter for tracking

    # Do both forward and backward tracking
    from FaserActsKalmanFilter.CKF2Config import CKF2Cfg
    if not args.isOverlay:
        # 4-station tracking forward
        acc.merge(CKF2Cfg(configFlags, 
                          actsOutputTag=f"{filestem}_4station_forward", 
                          noDiagnostics=False, seedWriter=True, StatesWriter=True, SummaryWriter=True))

        # 4-station tracking backward
        acc.merge(CKF2Cfg(configFlags, name="CKF_Back", 
                          actsOutputTag=f"{filestem}_4station_backward", 
                          OutputCollection="CKFTrackCollectionBackward", 
                          BackwardPropagation=True, 
                          noDiagnostics=False, seedWriter=True, StatesWriter=True, SummaryWriter=True))


    # Add tracking collection with no IFT 
    acc.merge(CKF2Cfg(configFlags, actsOutputTag=f"{filestem}_3station_forward", maskedLayers=[0, 1, 2], name="CKF_woIFT",
                      OutputCollection="CKFTrackCollectionWithoutIFT", 
                      BackwardPropagation=False,
                      noDiagnostics=False, seedWriter=True, StatesWriter=True, SummaryWriter=True))


    # Backward tracking with no IFT
    acc.merge(CKF2Cfg(configFlags, actsOutputTag=f"{filestem}_3station_backward", maskedLayers=[0, 1, 2], name="CKF_Back_woIFT", 
                      OutputCollection="CKFTrackCollectionBackwardWithoutIFT", 
                      BackwardPropagation=True,
                      noDiagnostics=False, seedWriter=True, StatesWriter=True, SummaryWriter=True))
#
#
# Configure output
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
itemList = [ "xAOD::EventInfo#*"
             , "xAOD::EventAuxInfo#*"
             , "xAOD::FaserTriggerData#*"
             , "xAOD::FaserTriggerDataAux#*"
             , "FaserSiHitCollection#*"  # Strip hits, do we want this?
             , "FaserSCT_RDO_Container#*" 
             , "FaserSCT_SpacePointContainer#*"
             , "Tracker::FaserSCT_ClusterContainer#*"
             , "TrackCollection#*"
]
#
if useLHC and not args.isOverlay:
    itemList.extend( ["xAOD::FaserLHCData#*", "xAOD::FaserLHCDataAux#*"] )

if args.isMC and not args.isOverlay:
    # Make xAOD versions of truth
    from Reconstruction.xAODTruthCnvAlgConfig import xAODTruthCnvAlgCfg
    acc.merge(xAODTruthCnvAlgCfg(configFlags))

    # Add MC information here
    itemList.extend( ["McEventCollection#*", "TrackerSimDataCollection#*"] )

# Output stream 
acc.merge(OutputStreamCfg(configFlags, "xAOD", itemList, disableEventTag=True))

# Try to turn off annoying INFO message, as we don't use this
# disableEventTag=True doesn't seem to work...
tagBuilder = CompFactory.EventInfoTagBuilder()
tagBuilder.PropagateInput=False
acc.addEventAlgo(tagBuilder)

if not args.isOverlay:
    # Waveform reconstruction output
    from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionOutputCfg    
    acc.merge(WaveformReconstructionOutputCfg(configFlags))
#
    # Calorimeter reconstruction output
    from CaloRecAlgs.CaloRecAlgsConfig import CalorimeterReconstructionOutputCfg
    acc.merge(CalorimeterReconstructionOutputCfg(configFlags))

# Check what we have
from OutputStreamAthenaPool.OutputStreamConfig import outputStreamName
print( "Writing out xAOD objects:" )
print( acc.getEventAlgo(outputStreamName("xAOD")).ItemList )

# Hack to avoid problem with our use of MC databases when isMC = False
if not args.isMC:
    replicaSvc = acc.getService("DBReplicaSvc")
    replicaSvc.COOLSQLiteVetoPattern = ""
    replicaSvc.UseCOOLSQLite = True
    replicaSvc.UseCOOLFrontier = False
    replicaSvc.UseGeomSQLite = True

# Configure verbosity    
if args.verbose:
    acc.foreach_component("*").OutputLevel = VERBOSE
    configFlags.dump()
    # Print out POOL conditions
    import os
    print(f"ATLAS_POOLCOND_PATH: {os.environ['ATLAS_POOLCOND_PATH']}")
    print(f"PoolSvc.ReadCatalog: {acc.getService('PoolSvc').ReadCatalog}")
    print(f"PoolSvc.WriteCatalog: {acc.getService('PoolSvc').WriteCatalog}")

else:
    acc.foreach_component("*").OutputLevel = INFO

    # Reduce event loop printout
    eventLoop = CompFactory.AthenaEventLoopMgr()
    eventLoop.EventPrintoutInterval = 100
    acc.addService(eventLoop)

acc.foreach_component("*ClassID*").OutputLevel = INFO

acc.getService("MessageSvc").Format = "% F%40W%S%7W%R%T %0W%M"

# Execute and finish
sc = acc.run(maxEvents=args.nevents)

b = time.time()
from AthenaCommon.Logging import log
log.info(f"Finish execution in {b-a} seconds")

# Signal errors
if sc.isSuccess():
    log.info("Execution succeeded")
    sys.exit(0)
else:
    log.info("Execution failed, return 1")
    sys.exit(1)

