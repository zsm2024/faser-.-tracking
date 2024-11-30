################################################################################
# Package: NeutrinoIdentifier
################################################################################

# Declare the package name:
atlas_subdir( NeutrinoIdentifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( NeutrinoIdentifier
                   src/EmulsionID.cxx
                   PUBLIC_HEADERS NeutrinoIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel FaserDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDictParser GaudiKernel )

atlas_add_dictionary( NeutrinoIdentifierDict
                      NeutrinoIdentifier/NeutrinoIdentifierDict.h
                      NeutrinoIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} FaserDetDescr IdDict Identifier IdDictParser GaudiKernel NeutrinoIdentifier )

