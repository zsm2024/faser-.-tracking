################################################################################
# Package: FaserByteStreamCnvSvcBase
################################################################################

# Declare the package name:
atlas_subdir( FaserByteStreamCnvSvcBase )

# External dependencies:
# find_package( tdaq-common COMPONENTS eformat eformat_write )

# Component(s) in the package:
atlas_add_library( FaserByteStreamCnvSvcBaseLib
                   src/*.cxx
                   PUBLIC_HEADERS FaserByteStreamCnvSvcBase
                   # INCLUDE_DIRS  ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests EventFormats
                   PRIVATE_LINK_LIBRARIES SGTools )

atlas_add_component( FaserByteStreamCnvSvcBase
                     src/components/*.cxx
                     # INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES FaserByteStreamCnvSvcBaseLib )


#atlas_add_test( ROBDataProviderSvcMT	
#		 SCRIPT test/test_ROBDataProviderSvcMT.sh	 
#		 PROPERTIES TIMEOUT 1200 )
#

# Install files from the package:
#atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

