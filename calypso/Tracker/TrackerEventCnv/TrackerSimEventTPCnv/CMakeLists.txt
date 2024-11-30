###############################################################################
# Package: TrackerSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrackerSimEventTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrackerSimEventTPCnv
                   src/TrackerHits/*.cxx
                   PUBLIC_HEADERS TrackerSimEventTPCnv
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeneratorObjectsTPCnv TrackerSimEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects Identifier StoreGateLib)

atlas_add_dictionary( TrackerSimEventTPCnvDict
                      TrackerSimEventTPCnv/TrackerSimEventTPCnvDict.h
                      TrackerSimEventTPCnv/selection.xml
                      LINK_LIBRARIES TrackerSimEventTPCnv)

