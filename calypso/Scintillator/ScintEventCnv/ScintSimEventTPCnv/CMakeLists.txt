###############################################################################
# Package: ScintSimEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( ScintSimEventTPCnv )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( ScintSimEventTPCnv
                   src/ScintHits/*.cxx
                   PUBLIC_HEADERS ScintSimEventTPCnv
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeneratorObjectsTPCnv ScintSimEvent AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects Identifier StoreGateLib)

atlas_add_dictionary( ScintSimEventTPCnvDict
                      ScintSimEventTPCnv/ScintSimEventTPCnvDict.h
                      ScintSimEventTPCnv/selection.xml
                      LINK_LIBRARIES ScintSimEventTPCnv)

