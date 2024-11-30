# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AthenaCommon.Configurable import Configurable
from AthenaCommon import Logging

def GeoModelCfg(configFlags):
    version       = configFlags.GeoModel.FaserVersion
    geoExportFile = configFlags.GeoModel.GeoExportFile

    from AthenaCommon.AppMgr import release_metadata
    rel_metadata = release_metadata()
    relversion = rel_metadata['release'].split('.')
    if len(relversion) < 3:
        relversion = rel_metadata['base release'].split('.')


    result=ComponentAccumulator()
    GeoModelSvc=CompFactory.GeoModelSvc
    gms=GeoModelSvc(FaserVersion=version,
                    GeoExportFile = geoExportFile,
                    SupportedGeometry = int(relversion[0])
                    )
    if configFlags.Common.ProductionStep == ProductionStep.Simulation:        ## Protects GeoModelSvc in the simulation from the AlignCallbacks
        gms.AlignCallbacks = False
    result.addService(gms,primary=True,create=True)


    #Get DetDescrCnvSvc (for identifier dictionaries (identifier helpers)
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    result.merge(DetDescrCnvSvcCfg(configFlags))

    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg	
    #tim_ca,tagInfoMgr = TagInfoMgrCfg(configFlags)
    #result.addService(tagInfoMgr)
    #result.merge(tim_ca)
    result.merge(TagInfoMgrCfg(configFlags))
    #TagInfoMgr used by GeoModelSvc but no ServiceHandle. Relies on string-name

    return result



if __name__ == "__main__":
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    # from AthenaConfiguration.TestDefaults import defaultTestFiles
    configFlags = initConfigFlags()
    # configFlags.Input.Files = defaultTestFiles.RAW
    Configurable.configurableRun3Behavior=1
    configFlags.GeoModel.FaserVersion = "Faser-01"
    configFlags.lock()

    acc = GeoModelCfg( configFlags )
    acc.store( open("test.pkl", "wb") )
    print("All OK")
