################################################################################
# Package: Reconstruction
################################################################################

# Declare the package name:
atlas_subdir( Reconstruction )

# Component(s) in the package:
#atlas_add_component( GeoModelTest
#                     src/GeoModelTestAlg.cxx
#                     src/components/GeoModelTest_entries.cxx
#                     INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
#                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaBaseComps GeoModelFaserUtilities ScintReadoutGeometry TrackerReadoutGeometry MagFieldInterfaces MagFieldElements MagFieldConditions )

# Install files from the package:
#atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.sh scripts/*.py )

atlas_add_test( ProdRecoTI12
    SCRIPT scripts/faser_reco.py --geom=TI12Data ${CMAKE_CURRENT_SOURCE_DIR}/../rawdata/Faser-Physics-001920-filtered.raw 
    PROPERTIES TIMEOUT 300 )

atlas_add_test( ProdRecoTestBeam
    SCRIPT scripts/faser_reco.py --geom=TestBeamData ${CMAKE_CURRENT_SOURCE_DIR}/../RAWDATA/Faser-Physics-003613-filtered.raw 
    PROPERTIES TIMEOUT 300 )

atlas_add_test( ProdRecoPilotTracks
    SCRIPT scripts/faser_reco.py --geom=TI12Data ${CMAKE_CURRENT_SOURCE_DIR}/../RAWDATA/Faser-Physics-pilot_tracks-filtered.raw
    PROPERTIES TIMEOUT 300 )

# Test of TI12Data02 geometry (should auto-select from run number)
atlas_add_test( ProdRecoTI12-02-2022
    SCRIPT scripts/faser_reco.py ${CMAKE_CURRENT_SOURCE_DIR}/../rawdata/Faser-Physics-006525-filtered.raw
    PROPERTIES TIMEOUT 300 )

# Test of TI12Data03 geometry (should auto-select from run number)
atlas_add_test( ProdRecoTI12-03-2022
    SCRIPT scripts/faser_reco.py ${CMAKE_CURRENT_SOURCE_DIR}/../rawdata/Faser-Physics-007833-00003-TrigMask08.raw
    PROPERTIES TIMEOUT 300 )
