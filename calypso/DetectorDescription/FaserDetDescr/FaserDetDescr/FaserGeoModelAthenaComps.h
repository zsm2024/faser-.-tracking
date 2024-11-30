/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoModelFaserUtilities_FaserGeoModelAthenaComps_H
#define GeoModelFaserUtilities_FaserGeoModelAthenaComps_H

#include "TrackerGeoModelUtils/TrackerDDAthenaComps.h"

// class VetoID;


/// Class to hold various Athena components
template <class ID_HELPER>
class FaserGeoModelAthenaComps : public TrackerDD::AthenaComps {

public:

  FaserGeoModelAthenaComps();

  void setIdHelper(const ID_HELPER* idHelper);

  const ID_HELPER* getIdHelper() const;

private:
  const ID_HELPER* m_idHelper;

};

#endif // GeoModelFaserUtilities_FaserGeoModelAthenaComps_H
