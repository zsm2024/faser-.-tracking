atlas_subdir(FaserSpacePoints)

atlas_add_component(
  FaserSpacePoints
  src/components/*.cxx src/*.cxx src/*.h
  LINK_LIBRARIES AthenaBaseComps TrackerSimData TrackerSimEvent TrackerSpacePoint TrackerPrepRawData)

atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
