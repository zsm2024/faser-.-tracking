# $Id: CMakeLists.txt 730306 2016-03-16 10:26:06Z krasznaa $
################################################################################
# Package: TrackerEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrackerEventTPCnv )

# Component(s) in the package:
atlas_add_tpcnv_library( TrackerEventTPCnv
   TrackerEventTPCnv/*.h src/*.cxx
   TrackerEventTPCnv/TrackerPrepRawData/*.h src/TrackerPrepRawData/*.cxx
   TrackerEventTPCnv/TrackerRIO_OnTrack/*.h src/TrackerRIO_OnTrack/*.cxx
   PUBLIC_HEADERS TrackerEventTPCnv
   LINK_LIBRARIES AthAllocators AthLinks DataModelAthenaPoolLib StoreGateLib
   AthenaPoolCnvSvcLib AthenaPoolUtilities Identifier GaudiKernel
   TrackerIdentifier 
   TrackerPrepRawData 
   TrackerRIO_OnTrack
   TrkEventTPCnv 
   TrkPrepRawData
   TrkSpacePoint
   TrackerSpacePoint
   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel 
   EventPrimitives TrackerReadoutGeometry TrkSurfaces TrkEventPrimitives
   TrkMeasurementBase TrkRIO_OnTrack EventContainers )

atlas_add_dictionary( TrackerEventTPCnvDict
   TrackerEventTPCnv/TrackerEventTPCnvDict.h TrackerEventTPCnv/selection.xml
   LINK_LIBRARIES TrackerEventTPCnv )

# A helper variable:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Set up all the tests in the package:
#foreach( name SiWidthCnv_p1_test SiWidthCnv_p2_test 
#      SCT_ClusterContainerCnv_p2_test
#      SCT_ClusterContainerCnv_p3_test
#      SCT_ClusterCnv_p1_test SCT_ClusterCnv_p2_test SCT_ClusterCnv_p3_test
#      )
#
#    atlas_add_test( ${name}
#      SOURCES test/${name}.cxx
#      LINK_LIBRARIES TrackerEventTPCnv GaudiKernel IdDictParser
#      SGTools TrackerReadoutGeometry TestTools TrackerIdentifier
#      ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

#endforeach()
