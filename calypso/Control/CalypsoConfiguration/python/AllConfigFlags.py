# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AllConfigFlags import initConfigFlags as athenaConfigFlags
# from AthenaConfiguration.AllConfigFlags import GetFileMD
# from AthenaConfiguration.AthConfigFlags import AthConfigFlags
# from AthenaCommon.SystemOfUnits import TeV
# from AthenaConfiguration.AutoConfigFlags import GetFileMD, GetDetDescrInfo
import six


def _moduleExists (modName):
    if six.PY34:
        import importlib
        return importlib.util.find_spec (modName) is not None
    else:
        import imp
        try:
           imp.find_module (modName)
        except ImportError:
            return False
        return True


def _addFlagsCategory (acf, name, generator, modName = None):
    if _moduleExists (modName):
        return acf.addFlagsCategory (name, generator)
    return None

        
def _createCfgFlags():

    # acf=AthConfigFlags()
    fcf = athenaConfigFlags()

    fcf.Input.Files = ["_CALYPSO_GENERIC_INPUTFILE_NAME_",]  # former global.InputFiles
    # acf.addFlag('Input.SecondaryFiles', []) # secondary input files for DoubleEventSelector
    # acf.addFlag('Input.isMC', lambda prevFlags : "IS_SIMULATION" in GetFileMD(prevFlags.Input.Files).get("eventTypes",[]) ) # former global.isMC
    # acf.addFlag('Input.RunNumber', lambda prevFlags : list(GetFileMD(prevFlags.Input.Files).get("runNumbers",[]))) # former global.RunNumber
    # acf.addFlag('Input.ProjectName', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("project_name","data17_13TeV") ) # former global.ProjectName

    # def _inputCollections(inputFile):
    #     rawCollections = [type_key[1] for type_key in GetFileMD(inputFile).get("itemList",[])]
    #     collections = filter(lambda col: not col.endswith('Aux.'), rawCollections)
    #     return collections

    # acf.addFlag('Input.Collections', lambda prevFlags : _inputCollections(prevFlags.Input.Files) )

    # acf.addFlag('Concurrency.NumProcs', 0)
    # acf.addFlag('Concurrency.NumThreads', 0)
    # acf.addFlag('Concurrency.NumConcurrentEvents', 0)

    # acf.addFlag('Scheduler.CheckDependencies', True)
    # acf.addFlag('Scheduler.ShowDataDeps', True)
    # acf.addFlag('Scheduler.ShowDataFlow', True)
    # acf.addFlag('Scheduler.ShowControlFlow', True)

    # acf.addFlag('Common.isOnline', False ) #  Job runs in an online environment (access only to resources available at P1) # former global.isOnline
    # acf.addFlag('Common.useOnlineLumi', lambda prevFlags : prevFlags.Common.isOnline ) #  Use online version of luminosity. ??? Should just use isOnline?
    # acf.addFlag('Common.doExpressProcessing', False)

    # def _checkProject():
    #     import os
    #     if "AthSimulation_DIR" in os.environ:
    #         return "AthSimulation"
    #     #TODO expand this method.
    #     return "Athena"
    # acf.addFlag('Common.Project', _checkProject())

    # replace global.Beam*
    # acf.addFlag('Beam.BunchSpacing', 25) # former global.BunchSpacing
    # acf.addFlag('Beam.Type', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get('beam_type','collisions') )# former global.BeamType
    # acf.addFlag("Beam.NumberOfCollisions", lambda prevFlags : 2. if prevFlags.Beam.Type=='collisions' else 0.) # former global.NumberOfCollisions
    # acf.addFlag('Beam.Energy', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get('beam_energy',7*TeV)) # former global.BeamEnergy
    # acf.addFlag('Beam.estimatedLuminosity', lambda prevFlags : ( 1E33*(prevFlags.Beam.NumberOfCollisions)/2.3 ) *\
    #     (25./prevFlags.Beam.BunchSpacing)) # former flobal.estimatedLuminosity


    # acf.addFlag('Output.doESD', False) # produce ESD containers

    # acf.addFlag('Output.EVNTFileName','myEVNT.pool.root')
    # acf.addFlag('Output.HITSFileName','myHITS.pool.root')
    # acf.addFlag('Output.RDOFileName','myRDO.pool.root')
    # acf.addFlag('Output.ESDFileName','myESD.pool.root')
    # acf.addFlag('Output.AODFileName','myAOD.pool.root')
    # acf.addFlag('Output.HISTFileName','myHIST.root')
    
    # Might move this elsewhere in the future.
    # Some flags from https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/TrkDetDescrJobProperties.py
    # (many, e.g. those that set properties of one tool are not needed)
    # acf.addFlag('TrackingGeometry.MagneticFileMode', 6)
    # acf.addFlag('TrackingGeometry.MaterialSource', 'COOL') # Can be COOL, Input or None

#Detector Flags:
    def __detector():
        from CalypsoConfiguration.DetectorConfigFlags import createDetectorConfigFlags
        return createDetectorConfigFlags()
    # acf.addFlagsCategory( "Detector", __detector )
    fcf.join( __detector() )
    from CalypsoConfiguration.DetectorConfigFlags import modifyDetectorConfigFlags
    fcf = modifyDetectorConfigFlags(fcf)

#Simulation Flags:
    # def __simulation():
    #     from G4AtlasApps.SimConfigFlags import createSimConfigFlags
    #     return createSimConfigFlags()
    # _addFlagsCategory (fcf, "Sim", __simulation, 'G4AtlasApps' )

#Digitization Flags:
    # def __digitization():
    #     from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
    #     return createDigitizationCfgFlags()
    # _addFlagsCategory(acf, "Digitization", __digitization, 'Digitization' )

#Overlay Flags:
    # def __overlay():
    #     from OverlayConfiguration.OverlayConfigFlags import createOverlayConfigFlags
    #     return createOverlayConfigFlags()
    # _addFlagsCategory(acf, "Overlay", __overlay, 'OverlayConfiguration' )

#Geo Model Flags:
    def __geomodel():
        from CalypsoConfiguration.GeoModelConfigFlags import createGeoModelConfigFlags
        return createGeoModelConfigFlags()
    # _addFlagsCategory( fcf, "GeoModel", __geomodel )
    fcf.join( __geomodel() )
    from CalypsoConfiguration.GeoModelConfigFlags import modifyGeoModelConfigFlags
    fcf = modifyGeoModelConfigFlags(fcf)

    # acf.addFlag('GeoModel.Layout', 'atlas') # replaces global.GeoLayout
    # acf.addFlag("GeoModel.AtlasVersion", lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("GeoAtlas",None) or "ATLAS-R2-2016-01-00-01") #
    # acf.addFlag("GeoModel.Align.Dynamic", lambda prevFlags : (not prevFlags.Detector.Simulate))
    # acf.addFlag("GeoModel.StripGeoType", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('StripGeoType',"GMX")) # Based on CommonGeometryFlags.StripGeoType
    # acf.addFlag("GeoModel.Run", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('Run',"RUN2")) # Based on CommonGeometryFlags.Run (InDetGeometryFlags.isSLHC replaced by GeoModel.Run=="RUN4")
    # acf.addFlag("GeoModel.Type", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('GeoType',"UNDEFINED")) # Geometry type in {ITKLoI, ITkLoI-VF, etc...}
    # acf.addFlag("GeoModel.IBLLayout", lambda prevFlags : GetDetDescrInfo(prevFlags.GeoModel.AtlasVersion).get('IBLlayout',"UNDEFINED")) # IBL layer layout  in {"planar", "3D", "noIBL", "UNDEFINED"}
    # fcf.addFlag("GeoModel.FaserVersion", lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("GeoFaser",None) or "FASER-01")
    # fcf.addFlag("GeoModel.FaserVersion", lambda prevFlags : "FASER-01")
    # fcf.addFlag("GeoModel.GeoExportFile","")

#IOVDbSvc Flags:
    # acf.addFlag("IOVDb.GlobalTag",lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("IOVDbGlobalTag",None) or "CONDBR2-BLKPA-2017-05")
    # from IOVDbSvc.IOVDbAutoCfgFlags import getDatabaseInstanceDefault
    # acf.addFlag("IOVDb.DatabaseInstance",getDatabaseInstanceDefault)


    # def __lar():
    #     from LArConfiguration.LArConfigFlags import createLArConfigFlags
    #     return createLArConfigFlags()
    # _addFlagsCategory(acf, "LAr", __lar, 'LArConfiguration' ) 

    # def __tile():
    #     from TileConfiguration.TileConfigFlags import createTileConfigFlags
    #     return createTileConfigFlags()
    # _addFlagsCategory(acf, 'Tile', __tile, 'TileConfiguration' )

#CaloNoise Flags
    # acf.addFlag("Calo.Noise.fixedLumiForNoise",-1)
    # acf.addFlag("Calo.Noise.useCaloNoiseLumi",True)

#CaloCell flags
    # acf.addFlag("Calo.Cell.doLArHVCorr",False) # Disable for now as it is broken...
    # acf.addFlag("Calo.Cell.doPileupOffsetBCIDCorr", True)
#TopoCluster Flags:
    # acf.addFlag("Calo.TopoCluster.doTwoGaussianNoise",True)
    # acf.addFlag("Calo.TopoCluster.doTreatEnergyCutAsAbsolute",False)
    # acf.addFlag("Calo.TopoCluster.doTopoClusterLocalCalib",True)

#Random engine Flags:
    # acf.addFlag("Random.Engine", "dSFMT") # Random service used in {"dSFMT", "Ranlux64", "Ranecu"}

    # def __trigger():
    #     from TriggerJobOpts.TriggerConfigFlags import createTriggerFlags
    #     return createTriggerFlags()
    # _addFlagsCategory(acf, "Trigger", __trigger, 'TriggerJobOpts' )

    # def __muon():
    #     from MuonConfig.MuonConfigFlags import createMuonConfigFlags
    #     return createMuonConfigFlags()
    # _addFlagsCategory(acf, "Muon", __muon, 'MuonConfig' )

    # def __egamma():
    #     from egammaConfig.egammaConfigFlags import createEgammaConfigFlags
    #     return createEgammaConfigFlags()
    # _addFlagsCategory(acf, "Egamma", __egamma, 'egammaConfig' )

    # def __pflow():
    #     from eflowRec.PFConfigFlags import createPFConfigFlags
    #     return createPFConfigFlags()
    # _addFlagsCategory(acf,"PF",__pflow, 'eflowRec')

    # def __dq():
    #     from AthenaMonitoring.DQConfigFlags import createDQConfigFlags, createComplexDQConfigFlags
    #     dqf = createDQConfigFlags()
    #     dqf.join( createComplexDQConfigFlags() )
    #     return dqf
    # _addFlagsCategory(acf, "DQ", __dq, 'AthenaMonitoring' )

    return fcf

def initConfigFlags():
    fcf = _createCfgFlags()
    return fcf

if __name__=="__main__":
    import sys
    flags = initConfigFlags()
    if len(sys.argv)>1:
        flags.Input.Files = sys.argv[1:]
    else:
        flags.Input.Files = [ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1",]
    
    flags.loadAllDynamicFlags()
    flags.initAll()
    flags.dump()
    
