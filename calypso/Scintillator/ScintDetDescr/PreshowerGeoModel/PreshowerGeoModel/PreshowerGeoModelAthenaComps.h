/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerGeoModelAthenaComps_H
#define PreshowerGeoModel_PreshowerGeoModelAthenaComps_H 1

#include "ScintGeoModelUtils/ScintDDAthenaComps.h"

class PreshowerID;

/// Class to hold various Athena components
// template <class ID_HELPER>
class PreshowerGeoModelAthenaComps : public ScintDD::AthenaComps {

public:

  PreshowerGeoModelAthenaComps();

  void setIdHelper(const PreshowerID* idHelper);

  const PreshowerID* getIdHelper() const;

private:
  const PreshowerID* m_idHelper;

};

#endif // PreshowerGeoModel_PreshowerGeoModelAthenaComps_H
