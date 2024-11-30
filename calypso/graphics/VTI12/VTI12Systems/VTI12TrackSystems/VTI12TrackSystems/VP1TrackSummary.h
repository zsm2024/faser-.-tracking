/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1TrackSummary                     //
//                                                            //
//  Description: A few basic helpers providing track summary  //
//               information.                                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TRACKSUMMARY_H
#define VP1TRACKSUMMARY_H
#include <set>

namespace Trk { class Track; class Segment; class MeasurementBase; class TrkDetElementBase;}
class FaserDetectorID;

class VP1TrackSummary {
public:

  static bool countHits( const Trk::Track*,
       unsigned& nSCThits
       );
       
   static bool countHits( const Trk::Segment*,
       unsigned& nSCThits 
        );
        
    static void addCounts( std::set<const Trk::TrkDetElementBase*>& detelems, const Trk::MeasurementBase* meas,
       const FaserDetectorID * idhelper,
       unsigned& nSCThits 
        );
        
private:
  VP1TrackSummary();
  ~VP1TrackSummary();

};

#endif
