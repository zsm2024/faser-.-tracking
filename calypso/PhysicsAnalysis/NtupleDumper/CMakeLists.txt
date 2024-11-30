atlas_subdir( NtupleDumper )

find_package( nlohmann_json )

atlas_add_component(
        NtupleDumper
        src/NtupleDumperAlg.h
        src/NtupleDumperAlg.cxx
        src/component/NtupleDumper_entries.cxx
        LINK_LIBRARIES AthenaBaseComps StoreGateLib xAODFaserWaveform xAODFaserCalorimeter xAODFaserTrigger xAODFaserLHC ScintIdentifier FaserCaloIdentifier GeneratorObjects FaserActsGeometryLib TrackerSimEvent TrackerSimData TrackerIdentifier TrackerReadoutGeometry TrkTrack GeoPrimitives TrackerRIO_OnTrack TrackerSpacePoint FaserActsKalmanFilterLib AtlasHepMCLib WaveformConditionsToolsLib # FaserActsmanVertexingLib 
PRIVATE_LINK_LIBRARIES nlohmann_json::nlohmann_json 
)

atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.py scripts/*.sh )
 
