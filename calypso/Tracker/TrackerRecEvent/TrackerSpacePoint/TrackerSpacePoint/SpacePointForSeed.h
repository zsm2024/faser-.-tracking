#ifndef SpacePointForSeed_h
#define SpacePointForSeed_h

#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"

//namespace Trk{
//  class SpacePoint;
//}

class  SpacePointForSeed{
  public:
    SpacePointForSeed() = delete;
    SpacePointForSeed(const Tracker::FaserSCT_SpacePoint* sp) : m_spacepoint(sp) {};
    ~SpacePointForSeed() = default;
    const Tracker::FaserSCT_SpacePoint* SpacePoint() const {
      return m_spacepoint;
    }

  private:
    const Tracker::FaserSCT_SpacePoint * m_spacepoint;
};

#endif

