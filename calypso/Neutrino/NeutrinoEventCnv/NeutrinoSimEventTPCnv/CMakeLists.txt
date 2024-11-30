###############################################################################
# Package: NeutrinoSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( NeutrinoSimEventTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( NeutrinoSimEventTPCnv
                   src/NeutrinoHits/*.cxx
                   PUBLIC_HEADERS NeutrinoSimEventTPCnv
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeneratorObjectsTPCnv NeutrinoSimEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects Identifier StoreGateLib)

atlas_add_dictionary( NeutrinoSimEventTPCnvDict
                      NeutrinoSimEventTPCnv/NeutrinoSimEventTPCnvDict.h
                      NeutrinoSimEventTPCnv/selection.xml
                      LINK_LIBRARIES NeutrinoSimEventTPCnv)
