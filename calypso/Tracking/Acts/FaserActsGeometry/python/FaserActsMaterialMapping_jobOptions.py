"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

###############################################################
#
# Map material from a Geantino scan onto the surfaces and 
# volumes of the detector to creat a material map.
#
###############################################################


##########################################################################
# start from scratch with component accumulator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from FaserActsGeometry.ActsGeometryConfig import ActsMaterialStepConverterToolCfg
from FaserActsGeometry.ActsGeometryConfig import ActsSurfaceMappingToolCfg, ActsVolumeMappingToolCfg
from FaserActsGeometry.ActsGeometryConfig import ActsMaterialJsonWriterToolCfg

#from FaserActsGeometry.ActsGeometryConfig import ActsAlignmentCondAlgCfg
from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg

def ActsMaterialMappingCfg(configFlags, name = "FaserActsMaterialMapping", **kwargs):
  result = ComponentAccumulator()

  ActsSurfaceMappingTool = ActsSurfaceMappingToolCfg(configFlags)
  kwargs["SurfaceMappingTool"] = ActsSurfaceMappingTool.getPrimary()   
  result.merge(ActsSurfaceMappingTool)

  FaserActsVolumeMappingTool = ActsVolumeMappingToolCfg(configFlags)
  kwargs["VolumeMappingTool"] = FaserActsVolumeMappingTool.getPrimary()
  result.merge(FaserActsVolumeMappingTool)

  ActsMaterialJsonWriterTool = ActsMaterialJsonWriterToolCfg(OutputFile = "material-maps.json",
                                                            processSensitives = False,
                                                            processnonmaterial = False)
                                                            
  kwargs["MaterialJsonWriterTool"] = ActsMaterialJsonWriterTool.getPrimary()   
  result.merge(ActsMaterialJsonWriterTool)

  FaserActsMaterialMapping = CompFactory.FaserActsMaterialMapping
  alg = FaserActsMaterialMapping(name, **kwargs)
  result.addEventAlgo(alg)

  return result

if "__main__" == __name__:
  from AthenaCommon.Configurable import Configurable
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import INFO
  from CalypsoConfiguration.AllConfigFlags import initConfigFlags
  from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  from FaserActsGeometry.ActsGeometryConfig import ActsMaterialTrackWriterSvcCfg
  Configurable.configurableRun3Behavior = True

  ## Just enable ID for the moment.
  configFlags = initConfigFlags()
  configFlags.Input.isMC             = True
  configFlags.Beam.Type = "collisions" 
  configFlags.GeoModel.FaserVersion  = "FASERNU-03"
  configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersion
  configFlags.TrackingGeometry.MaterialSource = "geometry-maps.json"
  #configFlags.GeoModel.Align.Dynamic = False
  configFlags.lock()
  configFlags.dump()

  cfg = MainServicesCfg(configFlags)

  cfg.merge(FaserGeometryCfg(configFlags))
  cfg.merge(ActsMaterialTrackWriterSvcCfg("FaserActsMaterialTrackWriterSvc",
                                          "MaterialTracks_mapping.root"))

  cfg.merge(PoolReadCfg(configFlags))
  eventSelector = cfg.getService("EventSelector")
  eventSelector.InputCollections = ["MaterialStepCollection.root"]

#  from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
#  cfg.merge(BeamPipeGeometryCfg(configFlags))

#  alignCondAlgCfg = ActsAlignmentCondAlgCfg(configFlags)

#  cfg.merge(alignCondAlgCfg)

  alg = ActsMaterialMappingCfg(configFlags,
                               OutputLevel=INFO,
                               mapSurfaces = True,
                               mapVolumes = True)

  cfg.merge(alg)

  cfg.printConfig()

  log.info("CONFIG DONE")

  cfg.run(-1)
