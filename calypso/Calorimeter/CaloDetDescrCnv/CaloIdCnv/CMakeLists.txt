################################################################################
# Package: CaloIdCnv
################################################################################

# Declare the package name:
atlas_subdir( CaloIdCnv )

# Component(s) in the package:
atlas_add_component( CaloIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib SGtests DetDescrCnvSvcLib IdDictDetDescr GaudiKernel FaserCaloIdentifier )

# Install files from the package:
#atlas_install_joboptions( share/*.py )

