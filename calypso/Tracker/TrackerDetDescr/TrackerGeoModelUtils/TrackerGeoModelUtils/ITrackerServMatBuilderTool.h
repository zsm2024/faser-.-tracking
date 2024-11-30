/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITrackerServMatBuilderTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITrackerServMatBuilderTool_H
#define ITrackerServMatBuilderTool_H

#include "GaudiKernel/IAlgTool.h"

#include <list>

namespace TrackerDD {
  class ServiceVolume;
}

static const InterfaceID IID_ITrackerServMatBuilderTool("ITrackerServMatBuilderTool", 1, 0);


/// Tool for building services in SLHC geometry
class ITrackerServMatBuilderTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID( ) ;
  
  virtual const std::vector<const TrackerDD::ServiceVolume *> & getServices() = 0;  
};
  
inline const InterfaceID& ITrackerServMatBuilderTool::interfaceID()
{
  return IID_ITrackerServMatBuilderTool;
}
  
#endif //ITrackerServMatBuilderTool_H
