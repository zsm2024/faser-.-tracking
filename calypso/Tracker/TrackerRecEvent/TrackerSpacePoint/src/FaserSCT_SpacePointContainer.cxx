/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointContainer.cxx
//   Implementation file for class SpacePointContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"

// Constructor with parameters:
FaserSCT_SpacePointContainer::FaserSCT_SpacePointContainer(unsigned int max) :
  IdentifiableContainer<FaserSCT_SpacePointCollection>(max)
{
}

// Constructor with parameters:
FaserSCT_SpacePointContainer::FaserSCT_SpacePointContainer(FaserSCT_SpacePointCache *cache) :
  IdentifiableContainer<FaserSCT_SpacePointCollection>(cache)
{
}

// Destructor:
FaserSCT_SpacePointContainer::~FaserSCT_SpacePointContainer()
{

}


