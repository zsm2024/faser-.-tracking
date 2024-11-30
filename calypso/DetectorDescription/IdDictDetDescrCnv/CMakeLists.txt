################################################################################
# Package: IdDictDetDescrCnv
################################################################################

# Declare the package name:
atlas_subdir( IdDictDetDescrCnv )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( IdDictDetDescrCnv
                     src/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps CxxUtils AthenaKernel StoreGateLib SGtests FaserDetDescr DetDescrCnvSvcLib GeoModelFaserUtilities IdDictDetDescr IdDictParser Identifier GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )
