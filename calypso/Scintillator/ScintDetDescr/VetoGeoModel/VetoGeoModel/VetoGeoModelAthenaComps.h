/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoGeoModel_VetoGeoModelAthenaComps_H
#define VetoGeoModel_VetoGeoModelAthenaComps_H 1

#include "ScintGeoModelUtils/ScintDDAthenaComps.h"

class VetoID;

/// Class to hold various Athena components
// template <class ID_HELPER>
class VetoGeoModelAthenaComps : public ScintDD::AthenaComps {

public:

  VetoGeoModelAthenaComps();

  void setIdHelper(const VetoID* idHelper);

  const VetoID* getIdHelper() const;

private:
  const VetoID* m_idHelper;

};

#endif // VetoGeoModel_VetoGeoModelAthenaComps_H
