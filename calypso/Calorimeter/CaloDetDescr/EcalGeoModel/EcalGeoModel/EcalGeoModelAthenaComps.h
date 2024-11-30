/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EcalGeoModel_EcalGeoModelAthenaComps_H
#define EcalGeoModel_EcalGeoModelAthenaComps_H

#include "CaloGeoModelUtils/CaloDDAthenaComps.h"

class EcalID;


/// Class to hold various Athena components
class EcalGeoModelAthenaComps : public CaloDD::AthenaComps {

public:

  EcalGeoModelAthenaComps();

  void setIdHelper(const EcalID* idHelper);

  const EcalID* getIdHelper() const;

private:
  const EcalID* m_idHelper;

};

#endif // EcalGeoModel_EcalGeoModelAthenaComps_H

