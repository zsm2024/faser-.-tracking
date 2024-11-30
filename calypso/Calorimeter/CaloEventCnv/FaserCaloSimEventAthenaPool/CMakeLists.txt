################################################################################
# Package: FaserCaloSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( FaserCaloSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( FaserCaloSimEventAthenaPoolPoolCnv
                           src/*.h src/*.cxx
                           FILES FaserCaloSimEvent/CaloHitCollection.h 
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel FaserCaloSimEventTPCnv FaserCaloSimEvent )

# atlas_add_dictionary( FaserCaloSimEventAthenaPoolCnvDict
#                       FaserCaloSimEventAthenaPool/CaloSimEventAthenaPoolCnvDict.h
#                       FaserCaloSimEventAthenaPool/selection.xml
#                       INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#                       LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel FaserCaloSimEventTPCnv FaserCaloSimEvent )

# Install files from the package:
# atlas_install_headers( FaserCaloSimEventAthenaPool )
#atlas_install_joboptions( share/*.py )

