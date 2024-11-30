#ifndef TRACKERSPACEPOINT_TRACKERSEED_H
#define TRACKERSPACEPOINT_TRACKERSEED_H

#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"

#include <vector>

using namespace std;

class MsgStream;

namespace Tracker {

  class TrackerSeed {

  public:

    enum StrategyId{NULLID=0, TRIPLET_SP_FIRSTSTATION=1, TRIPLET_SP=2, DOUBLET_SP=3};//, DOUBLET=2};

    TrackerSeed();
    TrackerSeed(const StrategyId, const TrackerSeed &);
    virtual ~TrackerSeed();
    
    TrackerSeed(const StrategyId, vector<const FaserSCT_SpacePoint*> seed, int stationId= 1 , double* param=0);
    
    void       set_id(const StrategyId id) { m_strategyId = id; }
    StrategyId id() const                  { return m_strategyId; }

    void add(vector<const FaserSCT_SpacePoint*> seed);
    
    vector<const FaserSCT_SpacePoint*> getSpacePoints() const {return m_seed;};
    int size() const;
    double* getParameters() const{return m_param;};
    int getStation() const{return m_stationId;};
    void setParameters(double* param);
    void setStation(int station);
    
    TrackerSeed &operator=(const TrackerSeed &);

    // virtual MsgStream& dump(MsgStream& stream) const;
    // virtual ostream&   dump(ostream&   stream) const;
    
  private:
    
    StrategyId m_strategyId;
    vector<const FaserSCT_SpacePoint*> m_seed;
    double* m_param;
    int m_stationId;
    
  };
  
  // MsgStream& operator << (MsgStream& stream, const TrackerSeed& prd);
  // ostream&   operator << (ostream&   stream, const TrackerSeed& prd);
  
}
#endif // TRACKERRAWDATA_TRACKERSEED_H
  
