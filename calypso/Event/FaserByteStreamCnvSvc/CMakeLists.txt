################################################################################
# Package: FaserByteStreamCnvSvc
################################################################################

# Declare the package name:
atlas_subdir( FaserByteStreamCnvSvc )

# External dependencies:
find_package( Boost COMPONENTS system )
find_package( CORAL COMPONENTS CoralBase )
#find_package( tdaq-common COMPONENTS eformat_old eformat_write RawFileName
#   DataReader DataWriter )

# Stupid hack to work around faser-common Logging header
include_directories( ../../faser-common/Logging/include )

# Libraries in the package:
atlas_add_library( FaserByteStreamCnvSvcLib
   FaserByteStreamCnvSvc/*.h src/*.h src/*.cxx
   PUBLIC_HEADERS FaserByteStreamCnvSvc
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES AthenaBaseComps FaserEventStorageLib  GaudiKernel
   StoreGateLib rt FaserByteStreamCnvSvcBaseLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES}
   AthenaKernel SGTools CollectionBase FileCatalog
   AthenaPoolUtilities PersistentDataModel EventInfo  
   xAODEventInfo xAODTrigger)

atlas_add_component( FaserByteStreamCnvSvc
   src/components/*.cxx
   LINK_LIBRARIES FaserByteStreamCnvSvcBaseLib FaserByteStreamCnvSvcLib )

# Executables in the package:
# atlas_add_executable( AtlFindBSEvent test/AtlFindBSEvent.cxx
#    INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
#    LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES} )

# atlas_add_executable( AtlCopyBSEvent test/AtlCopyBSEvent.cxx
#    INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
#    ${Boost_INCLUDE_DIRS}
#    LINK_LIBRARIES ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES}
#    CollectionBase FileCatalog PersistentDataModel )

# atlas_add_executable( AtlListBSEvents test/AtlListBSEvents.cxx
#    INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
#    LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${Boost_LIBRARIES} )


# Test(s) in the package:
# atlas_add_test( BSEventSelector
#    SCRIPT "athena.py FaserByteStreamCnvSvc/BSEventSelector_test_jobOptions.py"
#    LOG_SELECT_PATTERN "ByteStream.*Svc" )

# atlas_add_test( AtlCopyBSEvent1_test
#    PRE_EXEC_SCRIPT "rm -f test.data"
#    SCRIPT "AtlCopyBSEvent -e 186882810,187403142,187404922,187419528 -o test.data /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"
#    PROPERTIES DEPENDS FaserByteStreamCnvSvc_BSEventSelector_ctest )

# atlas_add_test( AtlFindBSEvent2_test
#    SCRIPT "AtlFindBSEvent -e 187403142 test.data"
#    PROPERTIES DEPENDS FaserByteStreamCnvSvc_AtlCopyBSEvent1_test_ctest )

# atlas_add_test( AtlCopyBSEvent3_test
#    PRE_EXEC_SCRIPT "rm -f test_defl.data"
#    SCRIPT "AtlCopyBSEvent -d -e 186882810,187403142,187419528 -o test_defl.data test.data"
#    PROPERTIES DEPENDS FaserByteStreamCnvSvc_AtlCopyBSEvent1_test_ctest )

# atlas_add_test( AtlFindBSEvent4_test
#    SCRIPT "AtlFindBSEvent -e 187403142 test_defl.data"
#    LOG_IGNORE_PATTERN "+Timestamp"
#    PROPERTIES DEPENDS FaserByteStreamCnvSvc_AtlCopyBSEvent3_test_ctest )

# atlas_add_test( AtlCopyBSEvent5_test
#    PRE_EXEC_SCRIPT "rm -f test_infl.data"
#    SCRIPT "AtlCopyBSEvent -e 186882810,187403142,187419528 -o test_infl.data test_defl.data"
#    PROPERTIES DEPENDS FaserByteStreamCnvSvc_AtlCopyBSEvent3_test_ctest )

# atlas_add_test( AtlCopyBSEvent6_test
#    PRE_EXEC_SCRIPT "rm -f empty*.data && python ${CMAKE_CURRENT_SOURCE_DIR}/test/create_empty_bsfile.py"
#    SCRIPT "AtlCopyBSEvent -e all -o empty.data empty._0001.data"
#    PROPERTIES DEPENDS FaserByteStreamCnvSvc_AtlCopyBSEvent5_test_ctest )

# atlas_add_test( FaserByteStreamConfigTest
#    SCRIPT python -m FaserByteStreamCnvSvc.FaserByteStreamConfig
#    POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
# atlas_install_scripts( share/catalogBytestreamFiles.sh )
