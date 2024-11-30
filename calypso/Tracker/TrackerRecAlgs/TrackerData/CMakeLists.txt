atlas_subdir(TrackerData)

atlas_add_component(
  TrackerData
  src/TrackerSegmentFitDataAlg.h
  src/TrackerSegmentFitDataAlg.cxx
  src/TrackerTruthDataAlg.h
  src/TrackerTruthDataAlg.cxx
  src/components/TrackerData_entries.cxx
  LINK_LIBRARIES AthenaBaseComps GeneratorObjects TrackerSimEvent TrackerIdentifier
    TrackerReadoutGeometry TrackerRIO_OnTrack TrackerPrepRawData TrkTrack
)

atlas_install_python_modules(python/*.py)
atlas_install_scripts(test/*.py)