/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackerGeoModelUtils_VolumeSplitter_H
#define TrackerGeoModelUtils_VolumeSplitter_H

#include "TrackerGeoModelUtils/VolumeSplitterUtils.h"

#include <string>
#include <vector>

namespace TrackerDD {
  class ServiceVolume;
  
  class VolumeSplitter
  {
  public:
    VolumeSplitter();
    ~VolumeSplitter();
    const std::vector<const ServiceVolume *> & splitAll(const Zone & zone,  const std::vector<const ServiceVolume *>);
    void split(const Zone & zone, const ServiceVolume & origVolume);
    const std::vector<const ServiceVolume *> & getVolumes() const {return m_volumes;}
    // Take ownership of volumes. If called user must delete the volumes. 
    void takeOwnership() {m_ownVolumes = false;}
  private:
    Ray makeRay(const ServiceVolume & origVolume);
    void makeVolumes(const Zone * zone, const ServiceVolume & origVolume);
    void splitVolume(const Zone * zone, const ServiceVolume & vol);
    void adjustR(const ServiceVolume & param, ServiceVolume * paramNew);

    bool m_ownVolumes;
    double m_epsilon;
    std::vector<const ServiceVolume *> m_volumes;
  };
 
}
#endif // TrackerGeoModelUtils_VolumeSplitter_H
