/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserTruthController_H
#define FaserTruthController_H

class TrackVisualizationHelper;

class FaserTruthController {
public:
  static FaserTruthController *getTruthController();
  void setVisualizationHelper(TrackVisualizationHelper *);
  TrackVisualizationHelper* getVisualizationHelper() const
  {
    return m_theVisHelper;
  }
private:
  // this is a singleton
  static FaserTruthController *s_thePointer;
  FaserTruthController();
  FaserTruthController(const FaserTruthController&) {}
  FaserTruthController& operator= (const FaserTruthController&);
  ~FaserTruthController();
  TrackVisualizationHelper *m_theVisHelper;
};

#endif
