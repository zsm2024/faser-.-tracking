atlas_subdir(PairVertex)

atlas_add_component(
        PairVertex
        src/PairVertexAlg.h
        src/PairVertexAlg.cxx
        src/component/PairVertex_entries.cxx
        LINK_LIBRARIES AthenaBaseComps StoreGateLib GeneratorObjects 
        FaserActsGeometryLib TrackerSimEvent TrackerIdentifier 
        TrackerReadoutGeometry TrkTrack GeoPrimitives TrackerRIO_OnTrack 
        xAODFaserWaveform  ScintIdentifier FaserCaloIdentifier
        FaserActsKalmanFilterLib
)

atlas_install_python_modules(python/*.py)
# atlas_install_scripts(test/*.py)
