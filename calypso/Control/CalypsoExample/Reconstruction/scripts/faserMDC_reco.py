#!/usr/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Run with:
# ./faser_reco.py filepath [runtype]
# 
# filepath - fully qualified path, including url if needed, to the input raw data file
#   example: "root://hepatl30//atlas/local/torrence/faser/commissioning/TestBeamData/Run-004150/Faser-Physics-004150-00000.raw"
# 
# runtype - optionally specify the data type (TI12Data, TI12Data02, TI12Data03 or TestBeamData).
#   In a normal file system location, this will be extracted from the directory name,
#   but runtype will override this assignment. 
#   > TI12Data02 is needed for the IFT geometry.  
#   MDC will assume TI12Data03 geometry.
#
import sys
import time
import argparse

a = time.time()

parser = argparse.ArgumentParser(description="Run FASER reconstruction")

parser.add_argument("file_path",
                    help="Fully qualified path of the raw input file")
parser.add_argument("run_type", nargs="?", default="",
                    help="Specify run type (if it can't be parsed from path)")
parser.add_argument("-r", "--reco", default="",
                    help="Specify reco tag (to append to output filename)")
parser.add_argument("-n", "--nevents", type=int, default=-1,
                    help="Specify number of events to process (default: all)")
parser.add_argument("-v", "--verbose", action='store_true', 
                    help="Turn on DEBUG output")


args = parser.parse_args()

from pathlib import Path

filepath=Path(args.file_path)

# runtype has been provided
if len(args.run_type) > 0:
    runtype=args.run_type

# Assume based on MDC reco
else:
    runtype = "TI12Data03"

# Assume this is MC
args.isMC = True

print(f"Starting reconstruction of {filepath.name} with type {runtype}")
if args.nevents > 0:
    print(f"Reconstructing {args.nevents} events by command-line option")

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
configFlags.IOVDb.DatabaseInstance = "OFLP200"   # Use MC conditions for now

configFlags.Input.ProjectName = "data20"
#configFlags.GeoModel.Align.Dynamic    = False

# For tracking
configFlags.TrackingGeometry.MaterialSource = "/cvmfs/faser.cern.ch/repo/sw/database/DBRelease/current/acts/material-maps.json"

# TI12 Cosmics geometry
if runtype == "TI12Data":
    configFlags.GeoModel.FaserVersion = "FASER-01" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"

# Testbeam setup 
elif runtype == "TestBeamData" or runtype == "TestBeam2021":
    configFlags.GeoModel.FaserVersion = "FASER-TB00" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-TB00"

# New TI12 geometry (ugh)
elif runtype == "TI12Data02":
    configFlags.GeoModel.FaserVersion = "FASER-02" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"

# Final 2022 TI12 geometry
elif runtype == "TI12Data03":
    configFlags.GeoModel.FaserVersion = "FASERNU-03" 
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"

else:
    print("Invalid run type found:", runtype)
    print("Specify correct type or update list")
    sys.exit(-1)


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

if args.isMC:
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(configFlags))
else:    
    from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
    acc.merge(FaserByteStreamCnvSvcCfg(configFlags))

#
# Needed, or move to MainServicesCfg?
from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
acc.merge(FaserGeometryCfg(configFlags))

# Set up algorithms
from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionCfg    
acc.merge(WaveformReconstructionCfg(configFlags))

# Not ready for primetime
#from CaloRecAlgs.CaloRecAlgsConfig import CalorimeterReconstructionCfg
#acc.merge(CalorimeterReconstructionCfg(configFlags))

# Tracker clusters
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="SCT_RDOs"))

#
# SpacePoints
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
acc.merge(TrackerSpacePointFinderCfg(configFlags))

# Try Dave's new fitter
print("Configuring TrackerSegmentFit (new)")
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
acc.merge(SegmentFitAlgCfg(configFlags,
                           SharedHitFraction=0.61, 
                           MinClustersPerFit=5, 
                           TanThetaXZCut=0.083))
# 
# Ghost removal
from FaserActsKalmanFilter.GhostBustersConfig import GhostBustersCfg
acc.merge(GhostBustersCfg(configFlags))

#
# Kalman Filter for tracking
from FaserActsKalmanFilter.CKF2Config import CKF2Cfg
acc.merge(CKF2Cfg(configFlags, noDiagnostics=True))

# Add tracking collection with no IFT 
acc.merge(CKF2Cfg(configFlags, maskedLayers=[0, 1, 2], name="CKF_woIFT", 
                  OutputCollection="CKFTrackCollectionWithoutIFT", noDiagnostics=True))

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
#
if args.isMC:
    # Make xAOD versions of truth
    from Reconstruction.xAODTruthCnvAlgConfig import xAODTruthCnvAlgCfg
    acc.merge(xAODTruthCnvAlgCfg(configFlags))

    # Add MC information here
    itemList.extend( ["McEventCollection#*", "TrackerSimDataCollection#*"] )

acc.merge(OutputStreamCfg(configFlags, "xAOD", itemList))

# Waveform reconstruction output
from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionOutputCfg    
acc.merge(WaveformReconstructionOutputCfg(configFlags))

# Calorimeter reconstruction output
# from CaloRecAlgs.CaloRecAlgsConfig import CalorimeterReconstructionOutputCfg
# acc.merge(CalorimeterReconstructionOutputCfg(configFlags))

# Check what we have
print( "Writing out xAOD objects:" )
print( acc.getEventAlgo("OutputStreamxAOD").ItemList )

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
else:
    acc.foreach_component("*").OutputLevel = INFO

acc.foreach_component("*ClassID*").OutputLevel = INFO

acc.getService("MessageSvc").Format = "% F%40W%S%7W%R%T %0W%M"

# Execute and finish
sc = acc.run(maxEvents=args.nevents)

b = time.time()
from AthenaCommon.Logging import log
log.info(f"Finish execution in {b-a} seconds")

sys.exit(int(sc.isFailure()))

