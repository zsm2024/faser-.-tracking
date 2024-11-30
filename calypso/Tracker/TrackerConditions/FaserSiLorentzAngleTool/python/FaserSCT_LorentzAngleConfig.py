"""Define methods to configure FaserSCTLorentzAngleTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon import Logging
from AthenaConfiguration.ComponentFactory import CompFactory
FaserSiLorentzAngleTool=CompFactory.FaserSiLorentzAngleTool
FaserSCT_SiLorentzAngleCondAlg=CompFactory.FaserSCT_SiLorentzAngleCondAlg
from FaserSCT_ConditionsTools.FaserSCT_DCSConditionsConfig import FaserSCT_DCSConditionsCfg
from FaserSCT_ConditionsTools.FaserSCT_SiliconConditionsConfig import FaserSCT_SiliconConditionsCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def FaserSCT_LorentzAngleToolCfg(flags, name="FaserSCT_LorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for FaserSCT"""
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("SiLorentzAngleCondData", "SCTSiLorentzAngleCondData")
    kwargs.setdefault("UseMagFieldSvc", True)
    return FaserSiLorentzAngleTool(name, **kwargs)

def FaserSCT_LorentzAngleCfg(flags, name="FaserSCT_SiLorentzAngleCondAlg",
                             forceUseDB=False, forceUseGeoModel=False, **kwargs):
    """Return configured ComponentAccumulator and tool for FaserSCT_LorentzAngle

    SiLorentzAngleTool may be provided in kwargs
    """
    if forceUseDB and forceUseGeoModel:
        msg = Logging.logging.getLogger("FaserSCT_LorentzAngleCfg")
        msg.error("Setting is wrong: both forceUseDB and forceUseGeoModel cannot be True at the same time")
    # construct with field services
    acc = MagneticFieldSvcCfg(flags)
    tool = kwargs.get("SiLorentzAngleTool", FaserSCT_LorentzAngleToolCfg(flags))
    if not forceUseGeoModel:
        DCSkwargs = {}
        # For HLT
        if flags.Common.isOnline and not flags.Input.isMC:
            dcs_folder = "/SCT/HLT/DCS"
            DCSkwargs["dbInstance"] = "SCT"
            DCSkwargs["hvFolder"] = dcs_folder + "/HV"
            DCSkwargs["tempFolder"] = dcs_folder + "/MODTEMP"
            DCSkwargs["stateFolder"] = dcs_folder + "/CHANSTAT"
        DCSAcc = FaserSCT_DCSConditionsCfg(flags, **DCSkwargs)
        SCAcc = FaserSCT_SiliconConditionsCfg(flags, DCSConditionsTool=DCSAcc.popPrivateTools())
        acc.merge(DCSAcc)
        acc.merge(SCAcc)
    # set up FaserSCT_SiLorentzAngleCondAlg
    kwargs.setdefault("UseMagFieldSvc", tool.UseMagFieldSvc)
    # kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    kwargs.setdefault("UseGeoModel", forceUseGeoModel)
    kwargs.setdefault("useSctDefaults", False)
    acc.addCondAlgo(FaserSCT_SiLorentzAngleCondAlg(name, **kwargs))
    acc.setPrivateTools(tool)
    return acc
