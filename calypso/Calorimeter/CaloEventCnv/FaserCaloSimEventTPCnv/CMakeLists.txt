###############################################################################
# Package: FaserCaloSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( FaserCaloSimEventTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( FaserCaloSimEventTPCnv
                   src/CaloHits/*.cxx
                   PUBLIC_HEADERS FaserCaloSimEventTPCnv
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeneratorObjectsTPCnv FaserCaloSimEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects Identifier StoreGateLib)

atlas_add_dictionary( FaserCaloSimEventTPCnvDict
                      FaserCaloSimEventTPCnv/CaloSimEventTPCnvDict.h
                      FaserCaloSimEventTPCnv/selection.xml
                      LINK_LIBRARIES FaserCaloSimEventTPCnv)
