# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Copyright (C) 2002-2022 CERN for the benefit of the FASER collaboration

def xAODTruthCnvAlgCfg(flags, name="xAODTruthCnvAlg", **kwargs):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    kwargs.setdefault('WriteTruthMetaData', False)
    kwargs.setdefault('AODContainerName', 'TruthEvent')
    kwargs.setdefault('EventInfo', 'McEventInfo')

    from AthenaConfiguration.ComponentFactory import CompFactory
    algo = CompFactory.xAODMaker.xAODTruthCnvAlg(name, **kwargs)
    acc.addEventAlgo(algo, primary=True)

    toAOD = ["xAOD::TruthEventContainer#*", "xAOD::TruthEventAuxContainer#*",
             "xAOD::TruthVertexContainer#*", "xAOD::TruthVertexAuxContainer#*",
             "xAOD::TruthParticleContainer#*", "xAOD::TruthParticleAuxContainer#*"]

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, "xAOD", ItemList=toAOD))

    return acc
