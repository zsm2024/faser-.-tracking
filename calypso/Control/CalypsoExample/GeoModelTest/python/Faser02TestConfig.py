# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
from AthenaCommon.Constants import VERBOSE, INFO
from AthenaConfiguration.ComponentFactory import CompFactory

def GeoModelTestCfg(flags, name="GeoModelTestAlg", **kwargs):

    # Initialize GeoModel
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    a = FaserGeometryCfg(flags)

    # Initialize field service
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    a.merge(MagneticFieldSvcCfg(flags))

    # Configure the algorithm itself
    GeoModelTestAlg = CompFactory.GeoModelTestAlg
    a.addEventAlgo(GeoModelTestAlg(name, FirstSCTStation=0, 
                                         LastSCTStation=3,
                                         NumVetoNuStations=0,
                                         PrintSctIDs=True,
                                         **kwargs))

    return a

if __name__ == "__main__":
    from AthenaCommon.Logging import log#, logging
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags

    Configurable.configurableRun3Behavior = True
    
# Flags for this job
    configFlags = initConfigFlags()
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-02"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASERNU-02"         # Default FASER geometry
    configFlags.GeoModel.GeoExportFile    = "FaserNu02.db"        # Writes out a GeoModel file with the full geometry tree (optional, comment out to skip)
    # configFlags.Detector.EnableVeto     = True
    # configFlags.Detector.EnableTrigger  = True
    # configFlags.Detector.EnablePreshower= True
    # configFlags.Detector.EnableFaserSCT = True
    # configFlags.Detector.EnableUpstreamDipole = True
    # configFlags.Detector.EnableCentralDipole = True
    # configFlags.Detector.EnableDownstreamDipole = True
    # configFlags.Detector.EnableEcal = True
    configFlags.lock()

# Configure components
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(configFlags)

# Set up algorithm
    acc.merge(GeoModelTestCfg(configFlags))

# Configure verbosity    
    msgSvc = acc.getService("MessageSvc")
    msgSvc.Format = "% F%30W%S%7W%R%T %0W%M"
    # configFlags.dump()
    # logging.getLogger('forcomps').setLevel(VERBOSE)
    acc.foreach_component("*").OutputLevel = VERBOSE
    acc.foreach_component("*ClassID*").OutputLevel = INFO
    log.setLevel(VERBOSE)
    
# Execute and finish
    sys.exit(int(acc.run(maxEvents=1).isFailure()))
