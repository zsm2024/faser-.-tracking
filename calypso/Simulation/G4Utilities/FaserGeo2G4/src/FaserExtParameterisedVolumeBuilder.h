/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserExtParameterisedVolumeBuilder_H
#define FASERGEO2G4_FaserExtParameterisedVolumeBuilder_H

#include "FaserVolumeBuilder.h"
#include "FaserGeo2G4AssemblyFactory.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "CxxUtils/checker_macros.h"
#include <string>

class FaserGeo2G4AssemblyVolume;
class GeoMaterial;

class FaserExtParameterisedVolumeBuilder: public FaserVolumeBuilder, public AthMessaging
{
public:
  FaserExtParameterisedVolumeBuilder(std::string n, FaserGeo2G4AssemblyFactory* G4AssemblyFactory);
  ///
  G4LogicalVolume* Build(const PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const;
  ///
  FaserGeo2G4AssemblyVolume* BuildAssembly(PVConstLink pv) const;

 private:
  /// Prints info when some PhysVol contains both types (PV and ST) of daughters
  void PrintSTInfo(const std::string& volume) const;
  ///
  void getMatEther() const;

  mutable bool               m_getMatEther;
  mutable const GeoMaterial* m_matEther;
  mutable const GeoMaterial* m_matHypUr;
  
  FaserGeo2G4AssemblyFactory* m_G4AssemblyFactory;
};

#endif
