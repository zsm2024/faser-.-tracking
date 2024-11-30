################################################################################
# Package: ScintSimEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ScintSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( ScintSimEventAthenaPoolPoolCnv
                           src/*.h src/*.cxx
                           FILES ScintSimEvent/ScintHitCollection.h 
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel ScintSimEventTPCnv ScintSimEvent )

# atlas_add_dictionary( ScintSimEventAthenaPoolCnvDict
#                       ScintSimEventAthenaPool/ScintSimEventAthenaPoolCnvDict.h
#                       ScintSimEventAthenaPool/selection.xml
#                       INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#                       LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AthenaPoolUtilities AtlasSealCLHEP GaudiKernel ScintSimEventTPCnv ScintSimEvent )

# Install files from the package:
# atlas_install_headers( ScintSimEventAthenaPool )
# atlas_install_joboptions( share/*.py )

