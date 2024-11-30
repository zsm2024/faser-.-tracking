# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def DetDescrCnvSvcCfg(configFlags=None):
    DetDescrCnvSvc=CompFactory.DetDescrCnvSvc
    EvtPersistencySvc=CompFactory.EvtPersistencySvc
    acc=ComponentAccumulator()
    # Specify primary Identifier dictionary to be use
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "FaserGeoModel/FASER_IDS.xml",IdDictFromRDB = True)
    acc.addService(detDescrCnvSvc)
    acc.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???
    return acc
