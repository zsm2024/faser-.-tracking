# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# JobOption fragment to set up the FaserFieldSvc and conditions algorithms
# Valerio Ippolito - Harvard University

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# from MagFieldServices.MagFieldServicesConf import MagField__FaserFieldCacheCondAlg
# from MagFieldServices.MagFieldServicesConf import MagField__FaserFieldMapCondAlg

#--------------------------------------------------------------

# def FaserFieldSvc(name="FaserFieldSvc",**kwargs):
#   # configure properties of service, if any
#   # kwargs.setdefault( "UseDCS", False )
#   return CfgMgr.MagField__FaserFieldSvc(name,**kwargs)

# def FaserFieldCacheCondAlg(name="FaserFieldCacheCondAlg",**kwargs):
#   # kwargs.setdefault( "UseDCS", False )
#   return CfgMgr.MagField__FaserFieldCacheCondAlg(name,**kwargs)

# def FaserFieldMapCondAlg(name="FaserFieldMapCondAlg",**kwargs):
#   kwargs.setdefault( "UseMapsFromCOOL", False )
#   return CfgMgr.MagField__FaserFieldMapCondAlg(name,**kwargs)


# def GetFieldSvc(name="FaserFieldSvc",**kwargs):
#     return FaserFieldSvc(name, **kwargs)

# def GetFieldCacheCondAlg(name="FaserFieldCacheCondAlg",**kwargs):
#     return FaserFieldCacheCondAlg(name, **kwargs)
    
# def GetFieldMapCondAlg(name="FaserFieldMapCondAlg",**kwargs):
#     return FaserFieldMapCondAlg(name, **kwargs)


# The magneticfields is going to need a big update for MT, so this is all temporary. Ed
def MagneticFieldSvcCfg(flags, **kwargs):
    result=ComponentAccumulator()

    # Hack to avoid randomly failing to find sqlite file
    poolSvc = CompFactory.PoolSvc()
    # poolSvc.SortReplicas = False
    result.addService(poolSvc)
    
    # initialise required conditions DB folders
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    
    # This handles addFolderSplitMC https://gitlab.cern.ch/atlas/athena/blob/master/Database/IOVDbSvc/python/CondDB.py#L240
    # i.e. replaces conddb.addFolderSplitMC('GLOBAL','/GLOBAL/BField/Maps <noover/>','/GLOBAL/BField/Maps <noover/>')
    # def addFolders(configFlags,folderstrings,detDb=None,className=None):
    db='GLOBAL_OFL'
    # if flags.Input.isMC:
    #     db='GLOBAL_OFL'
    # else:
    #     db='GLOBAL'
        
    result.merge(addFolders(flags,['/GLOBAL/BField/Maps <noover/>'],detDb=db, className="CondAttrListCollection") )
    result.merge(addFolders(flags,['/GLOBAL/BField/Scales <noover/>'],detDb=db, className="CondAttrListCollection") )
        
    # if not flags.Common.isOnline:
    #     result.merge(addFolders(flags, ['/EXT/DCS/MAGNETS/SENSORDATA'], detDb='DCS_OFL', className="CondAttrListCollection") )

    # if flags.Common.isOnline:
    #   kwargs.setdefault( "UseDCS", False )
    # else:
    #   kwargs.setdefault( "UseDCS", True )

    # FaserFieldSvc - old one
    afsArgs = {
      "name": "FaserFieldSvc",
    }
    # afsArgs.update( UseDCS = False )

    mag_field_svc = CompFactory.MagField.FaserFieldSvc(**afsArgs)  
    mag_field_svc.FullMapFile = "MagneticFieldMaps/FaserFieldTable_v2.root"
    result.addService(mag_field_svc, primary=True)

    # FaserFieldMapCondAlg - for reading in map
    afmArgs = {
      "name": "FaserFieldMapCondAlg",
    }
    afmArgs.update( UseMapsFromCOOL = True )
    mag_field_map_cond_alg = CompFactory.MagField.FaserFieldMapCondAlg(**afmArgs) 
    result.addCondAlgo(mag_field_map_cond_alg)
    
    # FaserFieldCacheCondAlg - for reading in scale factor
    afcArgs = {
      "name": "FaserFieldCacheCondAlg",
    }
    afcArgs.update( UseDCS = True )

    mag_field_cache_cond_alg = CompFactory.MagField.FaserFieldCacheCondAlg(**afcArgs) 
    result.addCondAlgo(mag_field_cache_cond_alg)
    return result 
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MagneticField/MagFieldServices/python/MagFieldServicesConfig.py
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import VERBOSE
    from AthenaConfiguration.AllConfigFlags import initConfigFlags

    log.setLevel(VERBOSE)
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    configFlags = initConfigFlags()
    configFlags.Input.Files = defaultTestFiles.RAW
    configFlags.Input.isMC = False
    configFlags.lock()

    cfg=ComponentAccumulator()

    acc  = MagneticFieldSvcCfg(configFlags)
    log.verbose(acc.getPrimary())    
    cfg.merge(acc)


          
    f=open("MagneticFieldSvc.pkl","wb")
    cfg.store(f)
    f.close()
