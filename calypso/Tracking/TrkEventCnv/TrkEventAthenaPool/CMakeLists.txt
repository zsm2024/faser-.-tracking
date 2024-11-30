# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TrkSpacePoint/SpacePointCollection.h
         TrkSpacePoint/SpacePointContainer.h TrkTrack/TrackCollection.h
   LINK_LIBRARIES TrkSpacePoint TrkEventTopLevelCnv TrkEventTPCnv )

# Install files from the package:
# atlas_install_joboptions( share/*.py )
