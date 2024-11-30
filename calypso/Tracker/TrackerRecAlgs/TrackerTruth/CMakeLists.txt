atlas_subdir(TrackerTruth)

atlas_add_component(
  TrackerTruth
  src/TrackerTruthAlg.h
  src/TrackerTruthAlg.cxx
  src/components/TrackerTruth_entries.cxx
  LINK_LIBRARIES AthenaBaseComps GeneratorObjects TrackerSimEvent TrackerIdentifier TrackerReadoutGeometry
)

atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )