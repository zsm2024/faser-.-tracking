/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FaserInterfaces_FaserGeo2G4SvcBase_H
#define G4FaserInterfaces_FaserGeo2G4SvcBase_H

#include <string>

class FaserVolumeBuilder;

/// @todo NEEDS DOCUMENTATION
class FaserGeo2G4SvcBase
{
public:
  virtual FaserVolumeBuilder* GetVolumeBuilder(std::string s) const = 0;
  virtual FaserVolumeBuilder* GetDefaultBuilder() const = 0;

  virtual void SetDefaultBuilder(std::string) = 0;
  virtual void ListVolumeBuilders() const = 0;

  virtual bool UseTopTransforms() const = 0;
};
#endif // G4FaserInterfaces_FaserGeo2G4SvcBase_H

