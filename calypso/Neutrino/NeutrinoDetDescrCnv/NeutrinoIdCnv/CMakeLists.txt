################################################################################
# Package: NeutrinoIdCnv
################################################################################

# Declare the package name:
atlas_subdir( NeutrinoIdCnv )

# Component(s) in the package:
atlas_add_component( NeutrinoIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel NeutrinoIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )

