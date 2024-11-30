/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserGeo2G4STParameterisation.h"
#include "G4VPhysicalVolume.hh"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "CLHEP/Geometry/Transform3D.h"

FaserGeo2G4STParameterisation::FaserGeo2G4STParameterisation(const GeoXF::Function* func,
                                                   unsigned int copies):
  m_function(func->clone()),
  m_nCopies(copies)
{
  m_rotation = new G4RotationMatrix();
}

FaserGeo2G4STParameterisation::~FaserGeo2G4STParameterisation()
{
  delete m_rotation;
}

void FaserGeo2G4STParameterisation::ComputeTransformation(const G4int copyNo,
                                                     G4VPhysicalVolume* physVol) const
{
  HepGeom::Transform3D transform = Amg::EigenTransformToCLHEP((*m_function)(copyNo));
  G4ThreeVector translation = transform.getTranslation();
  *m_rotation = transform.getRotation().inverse();

  physVol->SetTranslation(translation);
  physVol->SetRotation(m_rotation);
}
