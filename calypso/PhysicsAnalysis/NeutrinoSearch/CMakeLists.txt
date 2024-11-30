atlas_subdir(NeutrinoSearch)

atlas_add_component(
        NeutrinoSearch
        src/NeutrinoSearchAlg.h
        src/NeutrinoSearchAlg.cxx
        src/component/NeutrinoSearch_entries.cxx
        LINK_LIBRARIES AthenaBaseComps StoreGateLib xAODFaserWaveform ScintIdentifier ScintSimEvent FaserCaloIdentifier GeneratorObjects FaserActsGeometryLib TrackerSimEvent TrackerSimData TrackerIdentifier TrackerReadoutGeometry TrkTrack GeoPrimitives TrackerRIO_OnTrack
)

atlas_install_python_modules(python/*.py)
# atlas_install_scripts(test/*.py)
