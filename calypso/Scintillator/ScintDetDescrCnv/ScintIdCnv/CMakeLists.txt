################################################################################
# Package: ScintIdCnv
################################################################################

# Declare the package name:
atlas_subdir( ScintIdCnv )

# Component(s) in the package:
atlas_add_component( ScintIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel ScintIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )

