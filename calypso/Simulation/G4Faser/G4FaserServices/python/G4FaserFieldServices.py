# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
StandardFieldSvc=CompFactory.StandardFieldSvc
ConstantFieldSvc=CompFactory.ConstantFieldSvc

from AthenaCommon.SystemOfUnits import tesla
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def StandardFieldSvcCfg(ConfigFlags,name="StandardField", **kwargs):
    result = ComponentAccumulator()

    #setup the field and add the magneticfield service
    acc = MagneticFieldSvcCfg(ConfigFlags)
    result.merge(acc)

    kwargs.setdefault("MagneticFieldSvc", result.getService("FaserFieldSvc")) # TODO This should probably be based on simFlags.MagneticField?
    kwargs.setdefault("UseMagFieldSvc", True)  # Required to explicitly override default ATLAS behavior that doesn't work for FASER
    #kwargs.setdefault("FieldOn", True)

    result.addService(StandardFieldSvc(name, **kwargs))
    return result

def ConstantFieldSvcCfg(ConfigFlags,name="ConstantField", **kwargs):
    result = ComponentAccumulator()

    #setup the field and add the magneticfield service
    # acc = MagneticFieldSvcCfg(ConfigFlags)
    # result.merge(acc)

    # kwargs.setdefault("MagneticFieldSvc", result.getService("AtlasFieldSvc")) # TODO This should probably be based on simFlags.MagneticField?
    #kwargs.setdefault("FieldOn", True)

    kwargs.setdefault("FieldX", 0.55 * tesla)
    kwargs.setdefault("FieldY", 0.00 * tesla )
    kwargs.setdefault("FieldZ", 0.00 * tesla )
    result.addService(ConstantFieldSvc(name, **kwargs))
    return result


