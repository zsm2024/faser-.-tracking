/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoNuGeoModel_VetoNuGeoModelAthenaComps_H
#define VetoNuGeoModel_VetoNuGeoModelAthenaComps_H 1

#include "ScintGeoModelUtils/ScintDDAthenaComps.h"

class VetoNuID;

/// Class to hold various Athena components
// template <class ID_HELPER>
class VetoNuGeoModelAthenaComps : public ScintDD::AthenaComps {

public:

  VetoNuGeoModelAthenaComps();

  void setIdHelper(const VetoNuID* idHelper);

  const VetoNuID* getIdHelper() const;

private:
  const VetoNuID* m_idHelper;

};

#endif // VetoNuGeoModel_VetoNuGeoModelAthenaComps_H
