atlas_subdir(MyClusters)

atlas_add_component(
  MyClusters
  src/MyClustersAlg.h
  src/MyClustersAlg.cxx
  src/components/MyClusters_entries.cxx
  LINK_LIBRARIES AthenaBaseComps GeneratorObjects TrackerPrepRawData StoreGateLib GaudiKernel)

atlas_install_python_modules(python/*.py)
atlas_install_scripts(test/*.py)
