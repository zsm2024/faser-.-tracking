/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEOMATERIAL2G4_FaserElementFactory_H
#define FASERGEOMATERIAL2G4_FaserElementFactory_H

class G4Element;
class GeoElement;

#include <unordered_map>
#include <string>
typedef std::unordered_map<std::string, G4Element*> elList;

class FaserGeo2G4ElementFactory {
public:
  FaserGeo2G4ElementFactory();
  G4Element* Build(const GeoElement*);
private:
  elList m_definedElements;
};

#endif


