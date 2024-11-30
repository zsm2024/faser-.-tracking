/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeo2G4Builder_H
#define FASERGEO2G4_FaserGeo2G4Builder_H

// main builder to create/position all volumes described in a GeoModel Tree

// GeoVPhysVol
#include "FaserVolumeBuilder.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "G4LogicalVolume.hh"
//#include "Geo2G4/GenericVolumeBuilder.h"

// Typedef
#include "GeoModelFaserUtilities/GeoBorderSurfaceContainer.h"

#include "AthenaBaseComps/AthMessaging.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// STL includes
#include <string>
#include <vector>

class GeoMaterial;
class StoreGateSvc;

class FaserGeo2G4Builder : public AthMessaging {

public:
  // Constructor:
  FaserGeo2G4Builder(std::string detectorName);
  // Destructor:
  ~FaserGeo2G4Builder() {;}

  // Build method - geometry
  G4LogicalVolume*        BuildTree();

  // Build method - optical surfaces
  void BuildOpticalSurfaces(const GeoBorderSurfaceContainer* surface_container,
                            const OpticalVolumesMap* optical_volumes);

  // Access volume builder:
  FaserVolumeBuilder*        GetVolumeBuilder(std::string);

  HepGeom::Transform3D GetDetectorTransform() {return Amg::EigenTransformToCLHEP(m_motherTransform);}

private:

  // GeoVDetectorManager* theDetectorElement;
  std::string m_detectorName;
  GeoTrf::Transform3D m_motherTransform;
  std::vector<PVConstLink> m_treeTops;
  FaserVolumeBuilder *m_theBuilder;

  // std::Air in the case when top boolean envelope has to be built
  const GeoMaterial* m_matAir;
  StoreGateSvc* m_pDetStore;
};

#endif
