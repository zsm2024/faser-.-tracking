# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LHCDataCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    dbInstance = kwargs.get("dbInstance", "COOLOFL_LHC")
    dbName = flags.IOVDb.DatabaseInstance # e.g. CONDBR3 

    #acc.merge(addFolders(flags, folder_list, dbInstance, className="AthenaAttributeList"))
    # COOLOFL_LHC is not known to ATLAS IOVDBSvc
    # Must use non-shorthand folder specification here
    folder_list = ["/LHC/FillData", "/LHC/BeamData", "/LHC/BCIDData"]
    for folder_name in folder_list:
        folder_string = f"<db>{dbInstance}/{dbName}</db> {folder_name}"
        acc.merge(addFolders(flags, folder_string, className="AthenaAttributeList"))
    return acc

