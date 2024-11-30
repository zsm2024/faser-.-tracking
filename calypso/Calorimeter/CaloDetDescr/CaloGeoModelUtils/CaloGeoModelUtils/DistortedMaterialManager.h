/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloGeoModelUtils_DistortedMaterialManager_h
#define CaloGeoModelUtils_DistortedMaterialManager_h

class AbsMaterialManager;
#include "RDBAccessSvc/IRDBAccessSvc.h"

namespace CaloDD {

class DistortedMaterialManager
{
public:
  DistortedMaterialManager();  
  IRDBRecordset_ptr  extraMaterialTable() const {return  m_xMatTable;}
  const AbsMaterialManager * materialManager() const {return  m_materialManager;}

private:
  const AbsMaterialManager * m_materialManager;
  IRDBRecordset_ptr  m_xMatTable;
};


} // endnamespace

#endif // CaloGeoModelUtils_DistortedMaterialManager_h
