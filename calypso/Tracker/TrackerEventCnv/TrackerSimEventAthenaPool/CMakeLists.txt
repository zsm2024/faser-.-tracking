################################################################################
# Package: TrackerSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrackerSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrackerSimEventAthenaPoolPoolCnv
                           src/*.h src/*.cxx
                           FILES TrackerSimEvent/FaserSiHitCollection.h 
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel TrackerSimEventTPCnv TrackerSimEvent )

# atlas_add_dictionary( TrackerSimEventAthenaPoolCnvDict
#                       TrackerSimEventAthenaPool/TrackerSimEventAthenaPoolCnvDict.h
#                       TrackerSimEventAthenaPool/selection.xml
#                       INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#                       LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel TrackerSimEventTPCnv TrackerSimEvent )

# Install files from the package:
# atlas_install_headers( TrackerSimEventAthenaPool )
#atlas_install_joboptions( share/*.py )

