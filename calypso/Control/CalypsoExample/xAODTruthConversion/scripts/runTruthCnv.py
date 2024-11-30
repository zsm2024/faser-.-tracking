#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from AthenaCommon.Constants import VERBOSE, INFO
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def GEN_AOD2xAODCfg(flags, name="GEN_AOD2xAOD", **kwargs):
    acc = ComponentAccumulator()

    # Use digiSteeringConf from metadata to write full-PU truth
    # Not available yet in metadata

    # from PyUtils.MetaReader import read_metadata
    # infile = flags.Input.Files[0]
    # thisFileMD = read_metadata(infile, None, 'full')
    # metadata = thisFileMD[infile]
    # digiSteeringConf = metadata['/Digitization/Parameters'].get("digiSteeringConf","")
    # if digiSteeringConf == 'StandardInTimeOnlyGeantinoTruthPileUpToolsAlg':
    #     writeInTimePileUpTruth = True

    kwargs.setdefault('WriteTruthMetaData', False)
    kwargs.setdefault('AODContainerName', 'TruthEvent')
    kwargs.setdefault('EventInfo','McEventInfo')

    algo = CompFactory.xAODMaker.xAODTruthCnvAlg(name, **kwargs)
    acc.addEventAlgo(algo, primary = True)

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    toAOD = ["xAOD::TruthEventContainer#*", "xAOD::TruthEventAuxContainer#*",
             "xAOD::TruthVertexContainer#*", "xAOD::TruthVertexAuxContainer#*",
             "xAOD::TruthParticleContainer#*", "xAOD::TruthParticleAuxContainer#*"]
    toESD = []

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    if flags.Output.doWriteESD:
        acc.merge(OutputStreamCfg(flags, "ESD", ItemList=toESD+toAOD, disableEventTag=True))
    if flags.Output.doWriteAOD:
        acc.merge(OutputStreamCfg(flags, "AOD", ItemList=toAOD))
    return acc


if __name__ == "__main__":
    from AthenaCommon.Logging import log#, logging
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags

    Configurable.configurableRun3Behavior = True
    
# Flags for this job
    configFlags = initConfigFlags()
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASERNU-03"           # Default FASER geometry
    configFlags.Input.Files = ['my.HITS.pool.root']
    configFlags.Output.doWriteAOD = True
    configFlags.Output.doWriteESD = False
    configFlags.Output.AODFileName = "my.AOD.pool.root"

    configFlags.lock()

# Configure components
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(configFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(configFlags))

    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    acc.merge(PoolWriteCfg(configFlags))

# Set up algorithms

    acc.merge(GEN_AOD2xAODCfg(configFlags))

    from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
    acc.merge(EventInfoCnvAlgCfg(configFlags, disableBeamSpot=True))

    if configFlags.Output.doWriteAOD:
        ostream = acc.getEventAlgo("OutputStreamAOD")
    else:
        ostream = acc.getEventAlgo("OutputStreamESD")

    ostream.ItemList += ["xAOD::EventInfo#EventInfo","xAOD::EventAuxInfo#EventInfoAux."]

    algo = CompFactory.xAODReader.xAODTruthReader("TruthReader")
    acc.addEventAlgo(algo)

# Configure verbosity    
    msgSvc = acc.getService("MessageSvc")
    msgSvc.Format = "% F%30W%S%7W%R%T %0W%M"
    # configFlags.dump()
    # logging.getLogger('forcomps').setLevel(VERBOSE)
    acc.foreach_component("*").OutputLevel = VERBOSE
    acc.foreach_component("*ClassID*").OutputLevel = INFO
    log.setLevel(VERBOSE)
    
# Execute and finish
    sys.exit(int(acc.run(maxEvents=-1).isFailure()))
