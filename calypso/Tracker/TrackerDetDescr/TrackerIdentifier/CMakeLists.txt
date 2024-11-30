################################################################################
# Package: TrackerIdentifier
################################################################################

# Declare the package name:
atlas_subdir( TrackerIdentifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrackerIdentifier
                   src/FaserSCT_ID.cxx
                   PUBLIC_HEADERS TrackerIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel FaserDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDictParser GaudiKernel )

atlas_add_dictionary( TrackerIdentifierDict
                      TrackerIdentifier/TrackerIdentifierDict.h
                      TrackerIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} FaserDetDescr IdDict Identifier IdDictParser GaudiKernel TrackerIdentifier )

atlas_add_executable( test_tracker_id
                      test/test_tracker_id.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} FaserDetDescr IdDict Identifier IdDictParser GaudiKernel TrackerIdentifier )

