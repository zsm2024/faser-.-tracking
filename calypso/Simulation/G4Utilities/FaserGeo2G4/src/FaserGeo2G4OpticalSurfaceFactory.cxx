/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserGeo2G4OpticalSurfaceFactory.h"
#include "FaserGeoMaterial2G4/FaserGeo2G4MatPropTableFactory.h"

#include "GeoModelFaserUtilities/GeoOpticalSurface.h"

#include "G4OpticalSurface.hh"

FaserGeo2G4OpticalSurfaceFactory::FaserGeo2G4OpticalSurfaceFactory()
{
}

G4OpticalSurface* FaserGeo2G4OpticalSurfaceFactory::Build(const GeoOpticalSurface* geoOpticalSurface)
{
  //
  // Check if this surface has already been defined.
  //
  if(m_definedOptSurfaces.find(geoOpticalSurface) != m_definedOptSurfaces.end())  {
    return m_definedOptSurfaces[geoOpticalSurface];
  }

  // map enums
  G4OpticalSurfaceModel g4Model;
  G4OpticalSurfaceFinish g4Finish;
  G4SurfaceType g4Type;

  switch(geoOpticalSurface->GetModel())
    {
    case GeoOpticalSurface::glisur:
      {
        g4Model = glisur;
        break;
      }
    case GeoOpticalSurface::unified:
      {
        g4Model = unified;
        break;
      }
    default:
      g4Model = glisur;
    }

  switch(geoOpticalSurface->GetFinish())
    {
    case GeoOpticalSurface::polished:
      {
        g4Finish = polished;
        break;
      }
    case GeoOpticalSurface::polishedfrontpainted:
      {
        g4Finish = polishedfrontpainted;
        break;
      }
    case GeoOpticalSurface::polishedbackpainted:
      {
        g4Finish = polishedbackpainted;
        break;
      }
    case GeoOpticalSurface::ground:
      {
        g4Finish = ground;
        break;
      }
    case GeoOpticalSurface::groundfrontpainted:
      {
        g4Finish = groundfrontpainted;
        break;
      }
    case GeoOpticalSurface::groundbackpainted:
      {
        g4Finish = groundbackpainted;
        break;
      }
    default:
      g4Finish = polished;
    }

  switch(geoOpticalSurface->GetType())
    {
    case GeoOpticalSurface::dielectric_metal:
      {
        g4Type = dielectric_metal;
        break;
      }
    case GeoOpticalSurface::dielectric_dielectric:
      {
        g4Type = dielectric_dielectric;
        break;
      }
    case GeoOpticalSurface::firsov:
      {
        g4Type = firsov;
        break;
      }
    case GeoOpticalSurface::x_ray:
      {
        g4Type = x_ray;
        break;
      }
    default:
      g4Type = dielectric_dielectric;
    }



  G4OpticalSurface* newG4Surface = new G4OpticalSurface(geoOpticalSurface->GetName(),
                                                        g4Model,
                                                        g4Finish,
                                                        g4Type,
                                                        geoOpticalSurface->GetParameter());

  // Create material properties table
  const GeoMaterialPropertiesTable* geoPropTable = geoOpticalSurface->GetMaterialPropertiesTable();

  if(geoPropTable){
    FaserGeo2G4MatPropTableFactory tFactory;
    G4MaterialPropertiesTable* g4PropTable = tFactory.Build(geoPropTable);
    if(g4PropTable)
      newG4Surface->SetMaterialPropertiesTable(g4PropTable);
  }

  return newG4Surface;
}
