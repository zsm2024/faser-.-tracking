#include "TrackerSpacePoint/TrackerSeed.h"

namespace Tracker {

  TrackerSeed::TrackerSeed() : m_strategyId(NULLID) {}

  TrackerSeed::TrackerSeed(const StrategyId id, const TrackerSeed& trackerSeed) : m_strategyId(id), m_seed(trackerSeed.m_seed), m_param(trackerSeed.m_param), m_stationId(trackerSeed.m_stationId) {}

  TrackerSeed::~TrackerSeed() {}

  TrackerSeed::TrackerSeed(const StrategyId id, vector<const FaserSCT_SpacePoint*> seed, int stationId, double* param) { m_strategyId = id; m_seed = seed; m_param = param; m_stationId = stationId; }
  
  void TrackerSeed::add(vector<const FaserSCT_SpacePoint*> seed) { m_seed = seed; }
  
  int TrackerSeed::size() const { return m_seed.size(); }

  void TrackerSeed::setParameters(double* param) { m_param = param; }

  void TrackerSeed::setStation(int station) { m_stationId= station; }

  TrackerSeed& TrackerSeed::operator=(const TrackerSeed& trackSeed){
    if(&trackSeed != this) {
      TrackerSeed::operator=(trackSeed);
      m_seed = trackSeed.m_seed;
      m_param = trackSeed.m_param;
      m_stationId =  trackSeed.m_stationId;
    }
    return *this;
  }

  // Infinite recursion compiler warning...??
  // MsgStream& TrackerSeed::dump(MsgStream& stream) const {
  //   stream << "TrackerSeed object at station " <<m_stationId<< " with strategy = "<<m_strategyId<< endl;
  //   this->TrackerSeed::dump(stream);
  //   return stream;
  // }

  // ostream& TrackerSeed::dump(ostream& stream) const {
  //   stream << "TrackerSeed object at station " <<m_stationId<< " with strategy = "<<m_strategyId<< endl;
  //   this->TrackerSeed::dump(stream);
  //   return stream;
  // }

  // MsgStream& operator << (MsgStream& stream, const TrackerSeed& trackSeed) {
  //   return trackSeed.dump(stream);
  // }

  // ostream& operator << (ostream& stream, const TrackerSeed& trackSeed) {
  //   return trackSeed.dump(stream);
  // }
}
