/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeutrinoGeoModelUtils_ExtraMaterial
#define NeutrinoGeoModelUtils_ExtraMaterial

#include <cmath>
#include <string>
#include <sstream>
#include "RDBAccessSvc/IRDBAccessSvc.h"

class GeoPhysVol;
class GeoFullPhysVol;
class AbsMaterialManager;

namespace NeutrinoDD {

class DistortedMaterialManager;

class ExtraMaterial
{
public:
  ExtraMaterial(IRDBRecordset_ptr xMatTable, const AbsMaterialManager * matManager);
  ExtraMaterial(const NeutrinoDD::DistortedMaterialManager * manager);
  void add(GeoPhysVol * parent, const std::string & parentName, double zPos = 0);
  void add(GeoFullPhysVol * parent, const std::string & parentName, double zPos = 0);

private:
  void add(GeoPhysVol * parent, GeoFullPhysVol * fullparent, const std::string & parentName, double zPos);
  IRDBRecordset_ptr  m_xMatTable;
  const AbsMaterialManager * m_matManager;
};

} // end namespace

#endif // NeutrinoGeoModelUtils_ExtraMaterial
