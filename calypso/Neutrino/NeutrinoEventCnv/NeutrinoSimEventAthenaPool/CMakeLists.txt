################################################################################
# Package: NeutrinoSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( NeutrinoSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( NeutrinoSimEventAthenaPoolPoolCnv
                           src/*.h src/*.cxx
                           FILES NeutrinoSimEvent/NeutrinoHitCollection.h 
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel NeutrinoSimEventTPCnv NeutrinoSimEvent )

# atlas_add_dictionary( NeutrinoSimEventAthenaPoolCnvDict
#                       NeutrinoSimEventAthenaPool/NeutrinoSimEventAthenaPoolCnvDict.h
#                       NeutrinoSimEventAthenaPool/selection.xml
#                       INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#                       LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel NeutrinoSimEventTPCnv NeutrinoSimEvent )

# Install files from the package:
# atlas_install_headers( NeutrinoSimEventAthenaPool )
# atlas_install_joboptions( share/*.py )

