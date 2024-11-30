# $Id: CMakeLists.txt 749562 2016-05-25 04:45:43Z krasznaa $
################################################################################
# Package: TrackerEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrackerEventAthenaPool )

# Component(s) in the package:

# Component(s) in the package:
atlas_add_library( TrackerEventAthenaPool
   TrackerEventAthenaPool/*.h src/*.h src/*.cxx
   PUBLIC_HEADERS TrackerEventAthenaPool
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv AthAllocators AthContainers
   AthenaBaseComps AthenaKernel SGTools StoreGateLib AthenaPoolCnvSvcLib
   AthenaPoolUtilities GaudiKernel TrackerIdentifier
   TrackerReadoutGeometry TrackerEventTPCnv TrackerRawData TrackerSimData
   TrackerPrepRawData TrackerSpacePoint TrkTrack )   

   atlas_add_poolcnv_library( TrackerEventAthenaPoolPoolCnv
   FILES TrackerRawData/FaserSCT_RDO_Container.h
   TrackerPrepRawData/FaserSCT_ClusterContainer.h
   TrackerSpacePoint/FaserSCT_SpacePointContainer.h
   src/TrackerTrack.h
   TrackerSimData/TrackerSimDataCollection.h
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv AthAllocators AthContainers
   AthenaBaseComps AthenaKernel SGTools StoreGateLib AthenaPoolCnvSvcLib
   AthenaPoolUtilities GaudiKernel TrackerIdentifier
   TrackerReadoutGeometry TrackerEventTPCnv TrackerRawData TrackerSimData
   TrackerPrepRawData TrackerSpacePoint TrkTrack TrackerEventAthenaPool )

atlas_add_dictionary( TrackerEventAthenaPoolCnvDict
   TrackerEventAthenaPool/TrackerEventAthenaPoolCnvDict.h
   TrackerEventAthenaPool/selection.xml
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv )







# atlas_add_poolcnv_library( TrackerEventAthenaPoolPoolCnv
#    TrackerEventAthenaPool/*.h src/*.h src/*.cxx
#    FILES TrackerRawData/FaserSCT_RDO_Container.h
#    TrackerPrepRawData/FaserSCT_ClusterContainer.h
#    TrackerSimData/TrackerSimDataCollection.h
#    TrkSpacePoint/SpacePointContainer.h
# #   FaserSCT_ConditionsData/FaserSCT_FlaggedCondData.h
#    LINK_LIBRARIES Identifier GeneratorObjectsTPCnv AthAllocators AthContainers
#    AthenaBaseComps AthenaKernel SGTools StoreGateLib AthenaPoolCnvSvcLib
#    AthenaPoolUtilities AtlasSealCLHEP GaudiKernel TrackerIdentifier
#    TrackerReadoutGeometry TrackerEventTPCnv 
#    TrackerRawData TrackerSimData
#    TrackerPrepRawData TrkTrack TrackerRIO_OnTrack
#    TrkSpacePoint
#    TrackerSpacePoint
# #   FaserSCT_ConditionsData 
#    )

# atlas_add_dictionary( TrackerEventAthenaPoolCnvDict
#    TrackerEventAthenaPool/TrackerEventAthenaPoolCnvDict.h
#    TrackerEventAthenaPool/selection.xml
#    LINK_LIBRARIES Identifier GeneratorObjectsTPCnv )

# Install files from the package:
# atlas_install_headers( TrackerEventAthenaPool )
#atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
#if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
#   set( AthenaPoolUtilitiesTest_DIR
#      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
#endif()
#find_package( AthenaPoolUtilitiesTest )
#if( ATHENAPOOLUTILITIESTEST_FOUND )
#   set( TRACKEREVENTATHENAPOOL_REFERENCE_TAG
#        TrackerEventAthenaPoolReference-01-00-00 )
#   run_tpcnv_legacy_test( TrackerEventTPCnv_16.6.2.1 ESD-16.6.2.1
#                   REFERENCE_TAG ${TRACKEREVENTATHENAPOOL_REFERENCE_TAG} )
#else()
#   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
#endif()
