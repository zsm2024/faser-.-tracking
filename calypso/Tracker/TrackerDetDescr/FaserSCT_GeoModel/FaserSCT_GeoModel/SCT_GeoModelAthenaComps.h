/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GeoModel_SCT_GeoModelAthenaComps_H
#define FASERSCT_GeoModel_SCT_GeoModelAthenaComps_H

#include "TrackerGeoModelUtils/TrackerDDAthenaComps.h"

class FaserSCT_ID;


/// Class to hold various Athena components
class SCT_GeoModelAthenaComps : public TrackerDD::AthenaComps {

public:

  SCT_GeoModelAthenaComps();

  void setIdHelper(const FaserSCT_ID* idHelper);

  const FaserSCT_ID* getIdHelper() const;

private:
  const FaserSCT_ID* m_idHelper;

};

#endif // FASERSCT_GeoModel_SCT_GeoModelAthenaComps_H

