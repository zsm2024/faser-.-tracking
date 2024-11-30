/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerGeoModel_TriggerGeoModelAthenaComps_H
#define TriggerGeoModel_TriggerGeoModelAthenaComps_H 1

#include "ScintGeoModelUtils/ScintDDAthenaComps.h"

class TriggerID;

/// Class to hold various Athena components
// template <class ID_HELPER>
class TriggerGeoModelAthenaComps : public ScintDD::AthenaComps {

public:

  TriggerGeoModelAthenaComps();

  void setIdHelper(const TriggerID* idHelper);

  const TriggerID* getIdHelper() const;

private:
  const TriggerID* m_idHelper;

};

#endif // TriggerGeoModel_TriggerGeoModelAthenaComps_H
