/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionGeoModelAthenaComps_H
#define EmulsionGeoModel_EmulsionGeoModelAthenaComps_H 1

#include "NeutrinoGeoModelUtils/NeutrinoDDAthenaComps.h"

class EmulsionID;

/// Class to hold various Athena components
// template <class ID_HELPER>
class EmulsionGeoModelAthenaComps : public NeutrinoDD::AthenaComps {

public:

  EmulsionGeoModelAthenaComps();

  void setIdHelper(const EmulsionID* idHelper);

  const EmulsionID* getIdHelper() const;

private:
  const EmulsionID* m_idHelper;

};

#endif // EmulsionGeoModel_EmulsionGeoModelAthenaComps_H
