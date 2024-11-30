/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloGeoModelUtils_ExtraMaterial
#define CaloGeoModelUtils_ExtraMaterial

#include <cmath>
#include <string>
#include <sstream>
#include "RDBAccessSvc/IRDBAccessSvc.h"

class GeoPhysVol;
class GeoFullPhysVol;
class AbsMaterialManager;

namespace CaloDD {

class DistortedMaterialManager;

class ExtraMaterial
{
public:
  ExtraMaterial(IRDBRecordset_ptr xMatTable, const AbsMaterialManager * matManager);
  ExtraMaterial(const CaloDD::DistortedMaterialManager * manager);
  void add(GeoPhysVol * parent, const std::string & parentName, double zPos = 0);
  void add(GeoFullPhysVol * parent, const std::string & parentName, double zPos = 0);

private:
  void add(GeoPhysVol * parent, GeoFullPhysVol * fullparent, const std::string & parentName, double zPos);
  IRDBRecordset_ptr  m_xMatTable;
  const AbsMaterialManager * m_matManager;
};

} // end namespace

#endif // CaloGeoModelUtils_ExtraMaterial
