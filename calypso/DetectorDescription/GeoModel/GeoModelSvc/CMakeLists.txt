################################################################################
# Package: GeoModelSvc
################################################################################

# Declare the package name:
atlas_subdir( GeoModelSvc )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )


find_package( GeoModel COMPONENTS GeoModelKernel GeoModelWrite GeoModelDBManager)
find_package( Qt5 COMPONENTS Core Sql )

# Component(s) in the package:
atlas_add_component( GeoModelSvc
                     src/GeoModelSvc.cxx
                     src/GeoDbTagSvc.cxx
                     src/RDBMaterialManager.cxx
                     src/components/GeoModelSvc_entries.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${QT5_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}  
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${GEOMODEL_LIBRARIES} ${QT5_LIBRARIES} AthenaBaseComps CxxUtils AthenaKernel SGTools StoreGateLib SGtests GeoModelFaserUtilities EventInfo RDBAccessSvcLib EventInfoMgtLib GaudiKernel )

# Install files from the package:
#atlas_install_joboptions( share/*.py )

