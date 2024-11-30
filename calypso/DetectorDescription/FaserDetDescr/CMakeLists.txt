################################################################################
# Package: FaserDetDescr
################################################################################

# Declare the package name:
atlas_subdir( FaserDetDescr )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FaserDetDescr
                   src/FaserDetectorID.cxx
                   src/FaserDetectorIDHelper.cxx
                   src/FaserRegionHelper.cxx
                   PUBLIC_HEADERS FaserDetDescr
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} IdDict Identifier AthenaKernel
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( FaserDetDescrDict
                      FaserDetDescr/FaserDetDescrDict.h
                      FaserDetDescr/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} IdDict Identifier GaudiKernel FaserDetDescr )

