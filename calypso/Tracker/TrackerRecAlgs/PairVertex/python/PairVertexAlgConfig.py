"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

#Command line config
from argparse import ArgumentParser

test_file = '/eos/experiment/faser/sim/mdc/foresee/110001/rec/r0009/FaserMC-MDC_FS_Aee_100MeV_1Em5_shift-110001-00000-00009-s0007-r0009-xAOD.root'
parser=ArgumentParser(description="EventLooper")
parser.add_argument("-i","--inputfile",type=str,default=test_file)
parser.add_argument("-e", "--events", type=int, default=1000) # Max events processed in alg. Set to -1 to process all events.
parser.add_argument("-r","--runnum",type=int,default=110001) # 110001-4 
ui=parser.parse_args()

inputIsMC = 'FaserMC' in ui.inputfile

#Get/Make input/output file names
from ROOT import TFile
from glob import glob
dataDir=f"/eos/experiment/faser/sim/mdc/foresee/{ui.runnum}/rec/r0009/"
files=sorted(glob(f"{dataDir}/Faser*"))
name=files[0].split("r0009/")[-1].split("MDC_FS")[-1].split("shift")[0] #Generate output file name from input file name

#-------------------


def PairVertexAlgCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    acc.merge(MagneticFieldSvcCfg(flags))
    PairVertexAlg = CompFactory.Tracker.PairVertexAlg("PairVertexAlg",**kwargs)
    acc.addEventAlgo(PairVertexAlg)

    #Hist output
    thistSvc = CompFactory.THistSvc()
    thistSvc.Output = [f"HIST DATAFILE='Ntuple{name}.root' OPT='RECREATE'"]
    acc.addService(thistSvc)
    return acc

if __name__ == "__main__":

    import sys
    from AthenaCommon.Logging import log, logging
    from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

    # Set up logging and new style config
    log.setLevel(INFO)
    Configurable.configurableRun3Behavior = True

    # Configure
    configFlags = initConfigFlags()
    configFlags.Input.Files = files # [ui.inputfile]
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-02"             # Always needed; must match FaserVersionS
    configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
    configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig
    configFlags.Input.isMC = inputIsMC                                # Needed to bypass autoconfig
    configFlags.GeoModel.FaserVersion     = "FASERNU-03"           # FASER geometry
    configFlags.Common.isOnline = False
    #configFlags.GeoModel.Align.Dynamic = False
    configFlags.Beam.NumberOfCollisions = 0.
    configFlags.Detector.GeometryFaserSCT = True

    configFlags.lock()

    # Core components
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))

    # algorithm
    acc.merge(PairVertexAlgCfg(configFlags))

    # Hack to avoid problem with our use of MC databases when isMC = False
    if not inputIsMC:
        replicaSvc = acc.getService("DBReplicaSvc")
        replicaSvc.COOLSQLiteVetoPattern = ""
        replicaSvc.UseCOOLSQLite = True
        replicaSvc.UseCOOLFrontier = False
        replicaSvc.UseGeomSQLite = True

    # Execute and finish
    sc = acc.run(maxEvents=ui.events)

    # Success should be 0
    sys.exit(not sc.isSuccess())