################################################################################
# Package: FaserCaloIdentifier
################################################################################

# Declare the package name:
atlas_subdir( FaserCaloIdentifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FaserCaloIdentifier
                   src/EcalID.cxx
                   PUBLIC_HEADERS FaserCaloIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel FaserDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDictParser GaudiKernel )

atlas_add_dictionary( FaserCaloIdentifierDict
                      FaserCaloIdentifier/FaserCaloIdentifierDict.h
                      FaserCaloIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} FaserDetDescr IdDict Identifier IdDictParser GaudiKernel FaserCaloIdentifier )

#atlas_add_executable( test_indet_id
#                      test/test_indet_id.cxx
#                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#                      LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel InDetIdentifier )

