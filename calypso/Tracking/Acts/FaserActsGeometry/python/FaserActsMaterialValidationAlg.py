"""
This job options file will run an example extrapolation using the
Acts tracking geometry and the Acts extrapolation toolchain.

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

# start from scratch with component accumulator


from FaserActsGeometry.ActsGeometryConfig import FaserActsExtrapolationToolCfg
from FaserActsGeometry.FaserActsExtrapolationConfig import FaserActsExtrapolationAlgCfg
    
    
if "__main__" == __name__:
      from AthenaCommon.Configurable import Configurable
      from AthenaCommon.Logging import log
      from AthenaCommon.Constants import INFO
      from CalypsoConfiguration.AllConfigFlags import initConfigFlags
      from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
      from FaserActsGeometry.ActsGeometryConfig import FaserActsMaterialTrackWriterSvcCfg
      
      Configurable.configurableRun3Behavior = True
      
      ## Just enable ID for the moment.
      configFlags = initConfigFlags()
      configFlags.Input.isMC             = True
      configFlags.GeoModel.FaserVersion  = "FASER-01"
      configFlags.IOVDb.GlobalTag        = "OFLCOND-FASER-01"
      configFlags.TrackingGeometry.MaterialSource = "material-maps.json"
      #configFlags.Concurrency.NumThreads = 10
      #configFlags.Concurrency.NumConcurrentEvents = 10
      
      configFlags.lock()
      configFlags.dump()
      
      cfg = MainServicesCfg(configFlags)
      
#      alignCondAlgCfg = ActsAlignmentCondAlgCfg(configFlags)
#      cfg.merge(alignCondAlgCfg)
      cfg.merge(FaserActsMaterialTrackWriterSvcCfg(configFlags, "FaserActsMaterialTrackWriterSvc", "MaterialTracks_mapped.root"))
      
      print('DEF WRITER : ')
      extrapol = FaserActsExtrapolationToolCfg(configFlags,
         InteractionMultiScatering = True,
         InteractionEloss = True,
         InteractionRecord = True)
      cfg.merge(extrapol)
      
      alg = FaserActsExtrapolationAlgCfg(configFlags,
         OutputLevel=INFO,
         NParticlesPerEvent=int(1e3),
         EtaRange=[4.8, 100],
         PtRange=[999, 1001],
         XRange=[-150, 150],
         YRange=[-150, 150],
         WriteMaterialTracks = True,
         ExtrapolationTool=extrapol.getPrimary())
       
      cfg.merge(alg)
       
      #tgSvc = cfg.getService("FaserActsTrackingGeometrySvc")
      #cfg.printConfig()
      
      log.info("CONFIG DONE")
      
      cfg.run(100)
