from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def FaserGeoIDSvcCfg(ConfigFlags, name="ISF_FaserGeoIDSvc", **kwargs):
    result = ComponentAccumulator()

    result.addService(CompFactory.ISF.FaserGeoIDSvc(name, **kwargs))

    return result
