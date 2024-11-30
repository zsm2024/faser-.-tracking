/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEOMATERIAL2G4_FaserGeo2G4MatPropTableFactory_h
#define FASERGEOMATERIAL2G4_FaserGeo2G4MatPropTableFactory_h

class G4MaterialPropertiesTable;
class GeoMaterialPropertiesTable;

class FaserGeo2G4MatPropTableFactory {
public:
  G4MaterialPropertiesTable* Build(const GeoMaterialPropertiesTable*);
};

#endif


