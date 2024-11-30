""" Define methods to configure CaloRecTool

Copyright (C) 2022 CERN for the benefit of the FASER collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
CaloRecTool=CompFactory.CaloRecTool


def CaloRecToolCfg(flags, name="CaloRecTool", **kwargs):
    """ Return configured ComponentAccumulator and tool for Calo Calibration 

    CaloRecTool may be provided in kwargs
    """

    acc = ComponentAccumulator()
    # tool = kwargs.get("CaloRecTool", CaloRecTool(flags))
    # Probably need to figure this out!
    dbInstance = kwargs.get("dbInstance", "TRIGGER_OFL")

    MC_calibTag = kwargs.get("MC_calibTag", "")

    # MC calibration db folder MIP_ref has version tags
    if len(MC_calibTag) > 0:
        acc.merge(addFolders(flags, "/WAVE/Calibration/MIP_ref", dbInstance, className="CondAttrListCollection", tag=MC_calibTag))
    else:
        acc.merge(addFolders(flags, "/WAVE/Calibration/MIP_ref", dbInstance, className="CondAttrListCollection"))

    acc.merge(addFolders(flags, "/WAVE/Calibration/HV", dbInstance, className="CondAttrListCollection"))

    return acc


