################################################################################
# Package: TrackerIdCnv
################################################################################

# Declare the package name:
atlas_subdir( TrackerIdCnv )

# Component(s) in the package:
atlas_add_component( TrackerIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel TrackerIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )

